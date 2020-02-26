/*
 * SPNetUnfolder.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: crohr
 */

#include "sp_core/tools/SP_NetUnfolder.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_utilities.h"

#include "dssd/auxi/auxi.h"
#include "dssd/auxi/timer.h"
#include "dssd/andl/andl_reader.h"
#include "dssd/andl/andl_writer.h"
#include "dssd/candl/candl_writer.h"
#include "dssd/unfolding/net_unfolding.icc"
#include "dssd/unfolding/gecode_representation.h"
#include "dssd/unfolding/idd_representation.h"

template<typename Repr>
bool
SP_AbstractNetUnfolder<Repr>::operator ()(SP_DS_Graph* p_pcGraph, bool evalTokens, bool evalArcInscriptions)
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;

	SP_ColoredNetBuilder builder;
	if(builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();
		//dssd::candl::writer w(m_ColoredNet);
		//std::stringstream tout;
		//w(tout);
		//SP_LOGMESSAGE(wxString(tout.str()));
		try
		{
			dssd::functions::FunctionRegistry fReg;
			dssd::unfolding::net_unfolder<Repr> unfolder(m_ColoredNet,fReg);
			dssd::aux::Timer unfoldTime;
			unfolder(evalTokens, evalArcInscriptions);
			m_UnfoldedNet = unfolder.result();
			std::stringstream sout;
			sout << "time for unfolding: " << unfoldTime << "\n";
			sout << "\nunfolding complete |P|=" << m_UnfoldedNet->nrPlaces() << "|T|=" << m_UnfoldedNet->nrTransitions() << "|A|=" << m_UnfoldedNet->nrArcs() << "\n";
			wxString l_sMsg = sout.str();
			SP_LOGMESSAGE(l_sMsg);
			return true;
		}
		catch (const std::exception &exc)
		{
				wxString l_sMsg = exc.what();
				SP_LOGERROR(l_sMsg);
		}
		catch(...)
		{
			SP_LOGERROR(wxT("unkown exception!"));
		}
	}
	else
	{
		SP_LOGERROR(wxT(" while building colored net!"));
	}
	return false;
}

namespace
{

	template<typename Node>
	std::shared_ptr<Node> FindColoredNode(const std::string& p_UnfoldedName, std::list<std::shared_ptr<Node>>& p_ColoredNodes)
	{
		std::string::size_type pos = 0;
		std::shared_ptr<Node> cn;
		for (auto& n : p_ColoredNodes)
		{
			if(!n) continue;

			if(dssd::aux::startsWith(p_UnfoldedName, n->name_))
			{
				if(pos < n->name_.length())
				{
					pos = n->name_.length();
					cn = n;
				}
			}
		}
		return cn;
	}

}


template<typename Repr>
bool
SP_AbstractNetUnfolder<Repr>::FillInResults(SP_DS_ColPN_Unfolding* p_pcResults)
{
	if(!m_UnfoldedNet || ! m_ColoredNet) return false;

	//places

	dssd::andl::Place_ptr coloredPlace;
	wxString l_sColoredPlaceName;
	SP_CPN_UnfoldedPlace* l_mUnfoldedPlace;
	for(auto& p : *m_UnfoldedNet->places_)
	{
		if(!p) continue;

		if(!coloredPlace || ! dssd::aux::startsWith(p->name_, coloredPlace->name_))
		{
			coloredPlace = FindColoredNode<dssd::andl::Place>(p->name_, *m_ColoredNet->places_);
		}
		l_sColoredPlaceName = p->name_.substr(0, p->prefix_);
		//p_pcResults->m_mssColPlaceName2ColorSetforOrdering[l_sColoredPlaceName] = l_sColorSetName;
		if(p->type_ == dssd::andl::PlType::CONTINUOUS_T)
		{
			l_mUnfoldedPlace = &(p_pcResults->GetUnfoldedContPlaces()[l_sColoredPlaceName]);
		}
		else
		{
			l_mUnfoldedPlace = &(p_pcResults->GetUnfoldedDiscPlaces()[l_sColoredPlaceName]);
		}
		// prefix+1 because of '_' as delimiter between name and color
		wxString l_sColor = p->name_.substr(p->prefix_+1);
		if(p->type_ == dssd::andl::PlType::CONTINUOUS_T)
		{
			SP_CPN_UnfoldedPlaceInfo& l_UnfoldedPlaceInfo = (*l_mUnfoldedPlace)[l_sColor];
			l_UnfoldedPlaceInfo.m_bIsolated = false;
			l_UnfoldedPlaceInfo.m_sColorSet = coloredPlace->colorset_;
			l_UnfoldedPlaceInfo.m_sNodeType = SP_DS_CONTINUOUS_PLACE;
			l_UnfoldedPlaceInfo.m_bFixed = p->fixed_;
			wxString l_Marking = p->marking_;
			double l_nMarking = 0;
			if(!l_Marking.ToDouble(&l_nMarking))
			{
				wxString l_sMsg = p->name_;
				l_sMsg << wxT(" can't convert marking '") << l_Marking << wxT("' to double value!");
				SP_LOGERROR(l_sMsg);
			}
			l_UnfoldedPlaceInfo.m_adNetMarkings.push_back(l_nMarking);
 
		}
		else
		{
			SP_CPN_UnfoldedPlaceInfo& l_UnfoldedPlaceInfo = (*l_mUnfoldedPlace)[l_sColor];
			l_UnfoldedPlaceInfo.m_bIsolated = false;
			l_UnfoldedPlaceInfo.m_sColorSet = coloredPlace->colorset_;
			l_UnfoldedPlaceInfo.m_sNodeType = SP_DS_DISCRETE_PLACE;
			l_UnfoldedPlaceInfo.m_bFixed = p->fixed_;
			SP_VectorLong l_vMarkings;
			wxString l_Marking = p->marking_;
			long l_nMarking = 0;
			if(!l_Marking.ToLong(&l_nMarking))
			{
				wxString l_sMsg = p->name_;
				l_sMsg << wxT(" can't convert marking '") << l_Marking << wxT("' to integer value!");
				SP_LOGERROR(l_sMsg);
			}
			l_UnfoldedPlaceInfo.m_anNetMarkings.push_back(l_nMarking);
		}
	}
	//transitions

	wxString l_sColoredTransName;
	SP_CPN_UnfoldedTransition* l_UnfoldedTrans;
	for(auto& t : *m_UnfoldedNet->transitions_)
	{
		if(!t) continue;

		l_sColoredTransName = t->name_.substr(0, t->prefix_);
		switch (t->type_) {
		case dssd::andl::TrType::CONTINUOUS_T:
			l_UnfoldedTrans = &(p_pcResults->GetUnfoldedContTransions()[l_sColoredTransName]);
			break;
		case dssd::andl::TrType::IMMEDIATE_T:
			l_UnfoldedTrans = &(p_pcResults->GetUnfoldedImmTransions()[l_sColoredTransName]);
			break;
		case dssd::andl::TrType::DETERMINISTIC_T:
			l_UnfoldedTrans = &(p_pcResults->GetUnfoldedDetTransions()[l_sColoredTransName]);
			break;
		case dssd::andl::TrType::SCHEDULED_T:
			l_UnfoldedTrans = &(p_pcResults->GetUnfoldedSchedTransions()[l_sColoredTransName]);
			break;
		default:
			l_UnfoldedTrans = &(p_pcResults->GetUnfoldedStochTransions()[l_sColoredTransName]);
			break;
		}
		// prefix+1 because of '_' as delimiter between name and color
		wxString l_sBinding = t->name_.substr(t->prefix_+1);
		SP_CPN_UnfoldedTransInfo* l_UnfoldedTransInfo;
		switch (t->type_) {
		case dssd::andl::TrType::CONTINUOUS_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_CONTINUOUS_TRANS;
			break;
		case dssd::andl::TrType::IMMEDIATE_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_IMMEDIATE_TRANS;
			break;
		case dssd::andl::TrType::DETERMINISTIC_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_DETERMINISTIC_TRANS;
			break;
		case dssd::andl::TrType::SCHEDULED_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_SCHEDULED_TRANS;
			break;
		default:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_DISCRETE_TRANS;
			break;
		}
		l_UnfoldedTransInfo->m_sAnimTransInstName = l_sBinding;
		wxString l_sFunction = t->function_;
		l_UnfoldedTransInfo->m_anNetFunctions.push_back(l_sFunction);
		l_UnfoldedTransInfo->m_bReversible = t->reversible_;
		//arcs

		for(auto& c : *t->conditions_)
		{
			if(!c) continue;
			SP_CPN_UnfoldedArcInfo l_UnfoldedArcInfo;

			coloredPlace = FindColoredNode<dssd::andl::Place>(c->place_, *m_ColoredNet->places_);
			l_sColoredPlaceName = coloredPlace->name_;
			// length()+1 because of '_' as delimiter between name and color
			wxString l_sColor = c->place_.substr(coloredPlace->name_.length()+1);
			l_UnfoldedArcInfo.m_sColPlaceName = l_sColoredPlaceName;
			l_UnfoldedArcInfo.m_sColor = l_sColor;
			wxString l_sInscription = c->value_;
			l_UnfoldedArcInfo.m_sMultiplicity = l_sInscription;
			if(coloredPlace->type_ == dssd::andl::PlType::CONTINUOUS_T)
			{
				l_UnfoldedArcInfo.m_sDiscContType = SP_DS_CONTINUOUS_PLACE;
				l_sInscription.ToDouble(&l_UnfoldedArcInfo.m_dMultiplicity);
			}
			else
			{
				l_UnfoldedArcInfo.m_sDiscContType = SP_DS_DISCRETE_PLACE;
				l_sInscription.ToLong(&l_UnfoldedArcInfo.m_nMultiplicity);
			}
			switch (c->type_) {
				case dssd::andl::CondType::READ_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_READ_EDGE;
					break;
				case dssd::andl::CondType::INHIBITOR_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_INHIBITOR_EDGE;
					break;
				case dssd::andl::CondType::EQUAL_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EQUAL_EDGE;
					break;
				case dssd::andl::CondType::MODIFIER_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_MODIFIER_EDGE;
					break;
				default:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
					break;
			}

			l_UnfoldedTransInfo->m_vInputArcs.push_back(l_UnfoldedArcInfo);
		}

		for(auto& u : *t->updates_)
		{
			if(!u) continue;
			SP_CPN_UnfoldedArcInfo l_UnfoldedArcInfo;

			coloredPlace = FindColoredNode<dssd::andl::Place>(u->place_, *m_ColoredNet->places_);
			l_sColoredPlaceName = coloredPlace->name_;
			// length()+1 because of '_' as delimiter between name and color
			wxString l_sColor = u->place_.substr(coloredPlace->name_.length()+1);
			l_UnfoldedArcInfo.m_sColPlaceName = l_sColoredPlaceName;
			l_UnfoldedArcInfo.m_sColor = l_sColor;
			wxString l_sInscription = u->value_;
			l_UnfoldedArcInfo.m_sMultiplicity = l_sInscription;
			if(coloredPlace->type_ == dssd::andl::PlType::CONTINUOUS_T)
			{
				l_UnfoldedArcInfo.m_sDiscContType = SP_DS_CONTINUOUS_PLACE;
				l_sInscription.ToDouble(&l_UnfoldedArcInfo.m_dMultiplicity);
			}
			else
			{
				l_UnfoldedArcInfo.m_sDiscContType = SP_DS_DISCRETE_PLACE;
				l_sInscription.ToLong(&l_UnfoldedArcInfo.m_nMultiplicity);
			}
			switch (u->type_) {
				case dssd::andl::UpdateType::DECREASE_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
					l_UnfoldedTransInfo->m_vInputArcs.push_back(l_UnfoldedArcInfo);
					break;
				case dssd::andl::UpdateType::INCREASE_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
					l_UnfoldedTransInfo->m_vOutputArcs.push_back(l_UnfoldedArcInfo);
					break;
				case dssd::andl::UpdateType::ASSIGN_T:
					if(!(l_sInscription == wxT("0") || l_sInscription == wxT("0.0") ))
					{
						l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
						l_UnfoldedTransInfo->m_vOutputArcs.push_back(l_UnfoldedArcInfo);
					}
					l_UnfoldedArcInfo.m_sArcType = SP_DS_RESET_EDGE;
					l_UnfoldedArcInfo.m_dMultiplicity = 0;
					l_UnfoldedArcInfo.m_nMultiplicity = 0;
					l_UnfoldedTransInfo->m_vInputArcs.push_back(l_UnfoldedArcInfo);
					break;
				default:
					break;
			}

		}
	}


	return true;
}


template class SP_AbstractNetUnfolder<dssd::unfolding::idd_guard_representation>;
template class SP_AbstractNetUnfolder<dssd::unfolding::gecode_guard_representation>;

