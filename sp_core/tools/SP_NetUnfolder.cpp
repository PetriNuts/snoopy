/*
 * SPNetUnfolder.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: crohr
 */

#include "sp_core/tools/SP_NetUnfolder.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

#include "sp_utilities.h"

#include "dssz/auxi/auxi.h"
#include "dssz/auxi/timer.h"
#include "dssz/andl/andl_reader.h"
#include "dssz/andl/andl_writer.h"
#include "dssz/unfolding/net_unfolding.icc"
#include "dssz/unfolding/gecode_representation.h"
#include "dssz/unfolding/idd_representation.h"
#include "dssz/functions/functionRegistry.h"

template<typename Repr>
bool
SP_AbstractNetUnfolder<Repr>::operator ()(SP_DS_Graph* p_pcGraph)
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;
	bool l_bFinish = true;


	SP_ColoredNetBuilder builder;
	if(builder(m_pcGraph))
	{
		m_ColoredNet = builder.GetNet();
		try
		{
			dsszmc::functions::FunctionRegistry fReg;
			dsszmc::unfolding::net_unfolder<Repr> unfolder(m_ColoredNet,fReg);
			dsszmc::aux::Timer unfoldTime;
			unfolder();
			m_UnfoldedNet = unfolder.result_;
			std::stringstream sout;
			sout << "time for unfolding: " << unfoldTime << "\n";
			sout << "\nunfolding complete |P|=" << m_UnfoldedNet->nrPlaces() << "|T|=" << m_UnfoldedNet->nrTransitions() << "|A|=" << m_UnfoldedNet->nrArcs() << "\n";
			wxString l_sMsg = sout.str();
			SP_LOGMESSAGE(l_sMsg);
		}
		catch (dsszmc::exc::Exc &exc)
		{
				wxString l_sMsg = exc.getMsg();
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
	return l_bFinish;
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

			if(dsszmc::aux::startsWith(p_UnfoldedName, n->name_))
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

	dsszmc::andl::Place_ptr coloredPlace;
	wxString l_sColoredPlaceName;
	SP_CPN_UnfoldedPlace* l_mUnfoldedPlace;
	for(auto& p : *m_UnfoldedNet->places_)
	{
		if(!p) continue;

		if(!coloredPlace || ! dsszmc::aux::startsWith(p->name_, coloredPlace->name_))
		{
			coloredPlace = FindColoredNode<dsszmc::andl::Place>(p->name_, *m_ColoredNet->places_);
			l_sColoredPlaceName = coloredPlace->name_;
			//p_pcResults->m_mssColPlaceName2ColorSetforOrdering[l_sColoredPlaceName] = l_sColorSetName;
			if(p->type_ == dsszmc::andl::PlType::CONTINUOUS_T)
			{
				l_mUnfoldedPlace = &(p_pcResults->GetUnfoldedContPlaces()[l_sColoredPlaceName]);
			}
			else
			{
				l_mUnfoldedPlace = &(p_pcResults->GetUnfoldedDiscPlaces()[l_sColoredPlaceName]);
			}
		}
		// length()+1 because of '_' as delimiter between name and color
		wxString l_sColor = p->name_.substr(coloredPlace->name_.length()+1);
		if(p->type_ == dsszmc::andl::PlType::CONTINUOUS_T)
		{
			SP_CPN_UnfoldedPlaceInfo& l_UnfoldedPlaceInfo = (*l_mUnfoldedPlace)[l_sColor];
			l_UnfoldedPlaceInfo.m_bIsolated = false;
			l_UnfoldedPlaceInfo.m_sColorSet = coloredPlace->colorset_;
			l_UnfoldedPlaceInfo.m_sNodeType = SP_DS_CONTINUOUS_PLACE;
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

	dsszmc::andl::Transition_ptr coloredTrans;
	wxString l_sColoredTransName;
	SP_CPN_UnfoldedTransition* l_UnfoldedTrans;
	for(auto& t : *m_UnfoldedNet->transitions_)
	{
		if(!t) continue;

		if(!coloredTrans || ! dsszmc::aux::startsWith(t->name_, coloredTrans->name_))
		{
			coloredTrans = FindColoredNode<dsszmc::andl::Transition>(t->name_, *m_ColoredNet->transitions_);
			l_sColoredTransName = coloredTrans->name_;
			switch (t->type_) {
			case dsszmc::andl::TrType::CONTINUOUS_T:
				l_UnfoldedTrans = &(p_pcResults->GetUnfoldedContTransions()[l_sColoredTransName]);
				break;
			case dsszmc::andl::TrType::IMMEDIATE_T:
				l_UnfoldedTrans = &(p_pcResults->GetUnfoldedImmTransions()[l_sColoredTransName]);
				break;
			case dsszmc::andl::TrType::DETERMINISTIC_T:
				l_UnfoldedTrans = &(p_pcResults->GetUnfoldedDetTransions()[l_sColoredTransName]);
				break;
			case dsszmc::andl::TrType::SCHEDULED_T:
				l_UnfoldedTrans = &(p_pcResults->GetUnfoldedSchedTransions()[l_sColoredTransName]);
				break;
			default:
				l_UnfoldedTrans = &(p_pcResults->GetUnfoldedStochTransions()[l_sColoredTransName]);
				break;
			}
		}
		// length()+1 because of '_' as delimiter between name and color
		wxString l_sBinding = t->name_.substr(coloredTrans->name_.length()+1);
		SP_CPN_UnfoldedTransInfo* l_UnfoldedTransInfo;
		switch (t->type_) {
		case dsszmc::andl::TrType::CONTINUOUS_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_CONTINUOUS_TRANS;
			break;
		case dsszmc::andl::TrType::IMMEDIATE_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_IMMEDIATE_TRANS;
			break;
		case dsszmc::andl::TrType::DETERMINISTIC_T:
			l_UnfoldedTransInfo = &((*l_UnfoldedTrans)[l_sBinding]);
			l_UnfoldedTransInfo->m_sType = SP_DS_DETERMINISTIC_TRANS;
			break;
		case dsszmc::andl::TrType::SCHEDULED_T:
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

		//arcs

		for(auto& c : *t->conditions_)
		{
			if(!c) continue;
			SP_CPN_UnfoldedArcInfo l_UnfoldedArcInfo;

			coloredPlace = FindColoredNode<dsszmc::andl::Place>(c->place_, *m_ColoredNet->places_);
			l_sColoredPlaceName = coloredPlace->name_;
			// length()+1 because of '_' as delimiter between name and color
			wxString l_sColor = c->place_.substr(coloredPlace->name_.length()+1);
			l_UnfoldedArcInfo.m_sColPlaceName = l_sColoredPlaceName;
			l_UnfoldedArcInfo.m_sColor = l_sColor;
			wxString l_sInscription = c->value_;
			if(coloredPlace->type_ == dsszmc::andl::PlType::CONTINUOUS_T)
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
				case dsszmc::andl::CondType::READ_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_READ_EDGE;
					break;
				case dsszmc::andl::CondType::INHIBITOR_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_INHIBITOR_EDGE;
					break;
				case dsszmc::andl::CondType::EQUAL_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EQUAL_EDGE;
					break;
				case dsszmc::andl::CondType::MODIFIER_T:
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

			coloredPlace = FindColoredNode<dsszmc::andl::Place>(u->place_, *m_ColoredNet->places_);
			l_sColoredPlaceName = coloredPlace->name_;
			// length()+1 because of '_' as delimiter between name and color
			wxString l_sColor = u->place_.substr(coloredPlace->name_.length()+1);
			l_UnfoldedArcInfo.m_sColPlaceName = l_sColoredPlaceName;
			l_UnfoldedArcInfo.m_sColor = l_sColor;
			wxString l_sInscription = u->value_;
			if(coloredPlace->type_ == dsszmc::andl::PlType::CONTINUOUS_T)
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
				case dsszmc::andl::UpdateType::DECREASE_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
					l_UnfoldedTransInfo->m_vInputArcs.push_back(l_UnfoldedArcInfo);
					break;
				case dsszmc::andl::UpdateType::INCREASE_T:
					l_UnfoldedArcInfo.m_sArcType = SP_DS_EDGE;
					l_UnfoldedTransInfo->m_vOutputArcs.push_back(l_UnfoldedArcInfo);
					break;
				case dsszmc::andl::UpdateType::ASSIGN_T:
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


template class SP_AbstractNetUnfolder<dsszmc::unfolding::idd_guard_representation>;
template class SP_AbstractNetUnfolder<dsszmc::unfolding::gecode_guard_representation>;

