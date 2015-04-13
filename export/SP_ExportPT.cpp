//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.2 $
// $Date: 2005/05/22 $
// Short Description: abstract export routine and help structures/funct
//                    for simple and extended petri nets
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportPT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"

#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

#include <wx/filename.h>

SP_Place::SP_Place(long p_id, const wxString& p_name, const wxString& p_value, unsigned p_m, SP_DS_Node* p_node) :
	m_id(p_id), m_name(p_name), m_value(p_value), m_marking(p_m), m_node(p_node)
{
}

SP_ContPlace::SP_ContPlace(long p_id, const wxString& p_name, const wxString& p_value, double p_m, bool p_f, SP_DS_Node* p_node) :
	m_id(p_id), m_name(p_name), m_value(p_value), m_marking(p_m), m_fixed(p_f), m_node(p_node)
{
}

SP_Transition::SP_Transition(long p_id, const wxString& p_name, const wxString& p_function, SP_DS_Node* p_node) :
	m_id(p_id), m_name(p_name), m_function(p_function), m_node(p_node)
{
}

SP_ContTransition::SP_ContTransition(long p_id, const wxString& p_name, const wxString& p_function, bool p_r, SP_DS_Node* p_node) :
	m_id(p_id), m_name(p_name), m_function(p_function), m_reversible(p_r), m_node(p_node)
{
}

SP_Parameter::SP_Parameter(long p_id, const wxString& p_name, double p_m, SP_DS_Node* p_node) :
	m_id(p_id), m_name(p_name), m_marking(p_m), m_node(p_node)
{
}

SP_DiscrArc::SP_DiscrArc(long p_id, unsigned p_m, const wxString& p_v, SP_DS_Edge* p_edge) :
	m_id(p_id), m_mult(p_m), m_value(p_v), m_edge(p_edge)
{
}

SP_ContArc::SP_ContArc(long p_id, double p_m, const wxString& p_v, SP_DS_Edge* p_edge) :
	m_id(p_id), m_mult(p_m), m_value(p_v), m_edge(p_edge)
{
}

SP_Constant::SP_Constant(long p_id, const wxString& p_name, const wxString& p_type, const wxString& p_group, const wxString& p_value, double p_m, SP_DS_Metadata* p_n) :
	m_id(p_id), m_name(p_name), m_type(p_type), m_group(p_group), m_value(p_value), m_marking(p_m), m_node(p_n)
{
}

template<typename A>
void ClearArcMap(std::map<long, std::list<A*>*> &p_m)
{
	typename std::map<long, std::list<A*>*>::const_iterator amIt;
	std::list<A*>* aList;
	typename std::list<A*>::const_iterator aIt;
	A* a;
	for (amIt = p_m.begin(); amIt != p_m.end(); amIt++)
	{
		aList = (*amIt).second;
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				wxDELETE(a);
			}
			wxDELETE(aList);
		}
	}
	p_m.clear();
}

template<typename T>
void ClearIdNameMaps(std::map<long, T*> &p_mId, std::map<wxString, T*> &p_mName)
{
	typename std::map<long, T*>::const_iterator pIt;
	T* p;
	for (pIt = p_mId.begin(); pIt != p_mId.end(); pIt++)
	{
		p = (*pIt).second;
		wxDELETE(p);
	}
	p_mId.clear();
	p_mName.clear();
}

void SP_ExportPT::ClearMaps()
{
	ClearIdNameMaps<SP_Place>(m_placeIdMap, m_placeNameMap);
	ClearIdNameMaps<SP_ContPlace>(m_contPlaceIdMap, m_contPlaceNameMap);

	ClearIdNameMaps<SP_Transition>(m_trIdMap, m_trNameMap);
	ClearIdNameMaps<SP_ContTransition>(m_contTrIdMap, m_contTrNameMap);

	ClearIdNameMaps<SP_Parameter>(m_parameterIdMap, m_parameterNameMap);
	ClearIdNameMaps<SP_Constant>(m_constantIdMap, m_constantNameMap);

	ClearArcMap<SP_DiscrArc>(m_prePlMap);
	ClearArcMap<SP_DiscrArc>(m_postPlMap);

	ClearArcMap<SP_DiscrArc>(m_preTrMap);
	ClearArcMap<SP_DiscrArc>(m_postTrMap);

	ClearArcMap<SP_ContArc>(m_preContPlMap);
	ClearArcMap<SP_ContArc>(m_postContPlMap);

	ClearArcMap<SP_ContArc>(m_preContTrMap);
	ClearArcMap<SP_ContArc>(m_postContTrMap);
}

template<typename M, typename A>
void AddArcToMap(M &p_m, long p_id, A* p_a)
{
	std::list<A*>* aList;
	typename std::list<A*>::iterator aIt;

	if (!p_m[p_id])
	{
		aList = new list<A*>();
		p_m[p_id] = aList;
	}
	else
	{
		aList = p_m[p_id];

		// check if it is not the first arc between the same nodes
		// duplicate arcs between the same nodes are
		// replaced with one arc with a sum of multiplicities
		// (if requested)
		for (auto a : *aList)
		{
			if (a->m_id == p_a->m_id && DeleteDuplicateArcs(a->m_edge, p_a->m_edge))
			{
				a->m_mult += p_a->m_mult;
				a->m_value += "+" + p_a->m_value;
				wxDELETE(p_a);
				return;
			}
		}
	}
	aList->push_back(p_a);
}

bool SP_ExportPT::InitNodes()
{

	CHECK_POINTER(m_graph, return FALSE);

	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_Attribute* attr;

	// gets the iterator of nodeclass
	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_graph->GetNodeclasses();

	CHECK_POINTER(ncList, return FALSE);

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	wxString sId;
	long id;
	wxString name;
	wxString function;
	wxString sMarking;
	long marking;
	double cmarking;

	SP_Place* newP;
	SP_Place* p;
	SP_ContPlace* newCP;
	SP_ContPlace* cp;
	SP_Transition* newT;
	SP_Transition* t;
	SP_ContTransition* newCT;
	SP_ContTransition* ct;
	SP_Parameter* newPa;
	SP_Parameter* pa;

	//iterate the nodeclasses
	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)
	{
		nc = (*ncIt);
		nList = nc->GetElements();

		if (nc->GetName() == wxT("Place"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);
				if (m_placeIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("place with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate place id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				p = m_placeNameMap[name];
				if (p)
				{
					//probably there will be no such name, we don't check it again
					name = wxT("_P_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("places %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" place %ld got the name %s"),
									p->m_id, id,
									p->m_name.c_str(), id,
									name.c_str()));
				}

				attr = ( *nIt )->GetAttribute(wxT("Marking"));
				if(attr)
				{
					sMarking = attr->GetValueString();
					marking = dynamic_cast<SP_DS_MarkingAttribute*>(attr)->GetValue(true);
				}

				newP = new SP_Place(id, name, sMarking, (unsigned) marking, *nIt);
				m_placeIdMap[id] = newP;
				m_placeNameMap[name] = newP;
			}
		}

		if (nc->GetName() == SP_DS_CONTINUOUS_PLACE)
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);
				if (m_contPlaceIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("place with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate place id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				cp = m_contPlaceNameMap[name];
				if (cp)
				{
					//probably there will be no such name, we don't check it again
					name = wxT("_P_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("places %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" place %ld got the name %s"),
									cp->m_id, id,
									cp->m_name.c_str(), id,
									name.c_str()));
				}

				attr = ( *nIt )->GetAttribute(wxT("Marking"));
				if(attr)
				{
					sMarking = attr->GetValueString();
					cmarking = dynamic_cast<SP_DS_DoubleMarkingAttribute*>(attr)->GetValue(true);
				}

				bool l_bFixed = false;
				attr = (*nIt)->GetAttribute(wxT("Fixed"));
				if(attr)
				{
					l_bFixed = dynamic_cast<SP_DS_BoolAttribute*>(attr)->GetValue();
				}

				newCP = new SP_ContPlace(id, name, sMarking, cmarking, l_bFixed, *nIt);
				m_contPlaceIdMap[id] = newCP;
				m_contPlaceNameMap[name] = newCP;
			}
		}

		if (nc->GetName() == wxT("Transition"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);

				if (m_trIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("tr with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate tr id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				t = m_trNameMap[name];
				if (t)
				{
					// hope there are no such names, don't check it
					name = wxT("_T_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("tr %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" tr %ld got the name %s"),
									t->m_id, id,
									t->m_name.c_str(), id,
									name.c_str()));
				}

				if(( *nIt )->GetAttribute(wxT("FunctionList")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("FunctionList")) );
					function = l_pcColList->GetActiveCellValue( 1);
				}
				//else if(( *nIt )->GetAttribute(wxT("Duration")))
				//{
					//l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("Duration")) );
					//function = l_pcColList->GetActiveCellValue( 1);
				//}
				else if(( *nIt )->GetAttribute(wxT("Interval")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("Interval")) );
					function = l_pcColList->GetActiveCellValue( 1);
					function << wxT(",");
					function << l_pcColList->GetActiveCellValue( 2); 
				}
				else
				{
					function.Clear();
				}

				newT = new SP_Transition(id, name, function, *nIt);
				m_trIdMap[id] = newT;
				m_trNameMap[name] = newT;
			}
		}

		if (nc->GetName() == wxT("Immediate Transition"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);

				if (m_trIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("tr with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate tr id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				t = m_trNameMap[name];
				if (t)
				{
					// hope there are no such names, don't check it
					name = wxT("_T_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("tr %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" tr %ld got the name %s"),
									t->m_id, id,
									t->m_name.c_str(), id,
									name.c_str()));
				}

				if(( *nIt )->GetAttribute(wxT("FunctionList")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("FunctionList")) );
					function = wxT("ImmediateFiring(") + l_pcColList->GetActiveCellValue( 1) + wxT(")");
				}
				else
				{
					function.Clear();
				}

				newT = new SP_Transition(id, name, function, *nIt);
				m_trIdMap[id] = newT;
				m_trNameMap[name] = newT;
			}
		}

		if (nc->GetName() == wxT("Deterministic Transition"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);

				if (m_trIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("tr with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate tr id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				t = m_trNameMap[name];
				if (t)
				{
					// hope there are no such names, don't check it
					name = wxT("_T_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("tr %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" tr %ld got the name %s"),
									t->m_id, id,
									t->m_name.c_str(), id,
									name.c_str()));
				}

				if(( *nIt )->GetAttribute(wxT("DelayList")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("DelayList")) );
					function = wxT("TimedFiring(") + l_pcColList->GetActiveCellValue( 1) + wxT(")");
				}
				else
				{
					function.Clear();
				}

				newT = new SP_Transition(id, name, function, *nIt);
				m_trIdMap[id] = newT;
				m_trNameMap[name] = newT;
			}
		}

		if (nc->GetName() == wxT("Scheduled Transition"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);

				if (m_trIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("tr with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate tr id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				t = m_trNameMap[name];
				if (t)
				{
					// hope there are no such names, don't check it
					name = wxT("_T_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("tr %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" tr %ld got the name %s"),
									t->m_id, id,
									t->m_name.c_str(), id,
									name.c_str()));
				}

				if(( *nIt )->GetAttribute(wxT("PeriodicList")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("PeriodicList")) );
					function = wxT("FixedTimedFiring_Periodic(");
					function << l_pcColList->GetActiveCellValue( 1);
					function << wxT(",");
					function << l_pcColList->GetActiveCellValue( 2);
					function << wxT(",");
					function << l_pcColList->GetActiveCellValue( 3);
					function <<  wxT(")");
				}
				else
				{
					function.Clear();
				}

				newT = new SP_Transition(id, name, function, *nIt);
				m_trIdMap[id] = newT;
				m_trNameMap[name] = newT;
			}
		}

		if (nc->GetName() == SP_DS_CONTINUOUS_TRANS)
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);

				if (m_contTrIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("tr with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate tr id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				ct = m_contTrNameMap[name];
				if (ct)
				{
					// hope there are no such names, don't check it
					name = wxT("_T_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("tr %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" tr %ld got the name %s"),
									ct->m_id, id,
									ct->m_name.c_str(), id,
									name.c_str()));
				}

				if(( *nIt )->GetAttribute(wxT("FunctionList")))
				{
					l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("FunctionList")) );
					function = l_pcColList->GetActiveCellValue( 1);
				}
				else
				{
					function.Clear();
				}

		        bool l_bReversible = false;
		        attr = (*nIt)->GetAttribute(wxT("Reversible"));
		        if (attr)
		        {
		          l_bReversible = dynamic_cast<SP_DS_BoolAttribute*>(attr)->GetValue();
		        }

				newCT = new SP_ContTransition(id, name, function, l_bReversible, *nIt);
				m_contTrIdMap[id] = newCT;
				m_contTrNameMap[name] = newCT;
			}
		}

		if (nc->GetName() == wxT("Parameter"))
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&id);
				if (m_parameterIdMap[id])
				{
					SP_LOGERROR(
							wxString::Format(wxT("parameter with duplicate id: %ld"), id));
					SetLastError(wxString::Format(wxT("duplicate place id %ld"), id));
					return FALSE;
				}

				name = dynamic_cast<SP_DS_NameAttribute*>((*nIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				pa = m_parameterNameMap[name];
				if (pa)
				{
					//probably there will be no such name, we don't check it again
					name = wxT("_PA_") + sId + wxT("_") + name + wxT("_");
					SP_LOGWARNING( wxString::Format(wxT("parameters %ld and %ld")
									wxT(" have the same name %s.")
									wxT(" parameter %ld got the name %s"),
									pa->m_id, id,
									pa->m_name.c_str(), id,
									name.c_str()));
				}

				l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(( *nIt )->GetAttribute(wxT("ParameterList")) );
				double l_nParamValue;
				l_pcColList->GetActiveCellValue( 1).ToDouble(&l_nParamValue);

				newPa = new SP_Parameter(id, name, l_nParamValue, *nIt);
				m_parameterIdMap[id] = newPa;
				m_parameterNameMap[name] = newPa;
			}
		}
	}
	return TRUE;
}

void SP_ExportPT::InitArcMaps()
{
	CHECK_POINTER(m_graph, return);

	SP_ListEdgeclass::const_iterator ecIt;
	const SP_ListEdgeclass* ecList = m_graph->GetEdgeclasses();

	CHECK_POINTER(ecList, return);

	wxString sFrom;
	long lFrom;
	wxString sTo;
	long lTo;
	wxString sMult;
	long mult;
	double cmult;
	SP_DS_Node* nFrom;
	SP_DS_Node* nTo;
	SP_DS_Attribute* multAt;

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* elemList;

	//only one edgeclass in simple ped nets, but more in extended
	for (ecIt = ecList->begin(); ecIt != ecList->end(); ecIt++)
	{

		if ((*ecIt)->GetName() == wxT("Undirected Edge"))
		{
			continue;
		}

		elemList = (*ecIt)->GetElements();
		if (!elemList)
			continue;

		for (eIt = elemList->begin(); eIt != elemList->end(); ++eIt)
		{
			nFrom = (SP_DS_Node*) (*eIt)->GetSource();
			nTo = (SP_DS_Node*) (*eIt)->GetTarget();
			if (!nFrom && !nTo)
			{
				SP_LOGWARNING( wxString(wxT("you have error in your file, ")
								wxT("an edge without a source and a target")));
				SetLastError(wxT("from and to are null!"));
				continue;
			}
			if (!nFrom)
			{
				SP_LOGERROR(
						wxString::Format(wxT("source is null, target %s id: %s"),
								nTo->GetClassName().c_str(),
								nTo->GetAttribute(wxT("ID"))->GetValueString().c_str()));
				SetLastError(wxString::Format(wxT("from is null, target %s id: %s"),
						nTo->GetClassName().c_str(), nTo->GetAttribute(wxT("ID"))->GetValueString().c_str()));
				return;
			}
			if (!nTo)
			{
				SP_LOGERROR(
						wxString::Format(wxT("target is null, source %s id: %s"),
								nFrom->GetClassName().c_str(),
								nFrom->GetAttribute(wxT("ID"))->GetValueString().c_str()));
				SetLastError(wxString::Format(
						wxT("target is null, source %s id: %s"),
						nFrom->GetClassName().c_str(), nFrom->GetAttribute(wxT("ID"))->GetValueString().c_str()));
				return;
			}

			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);

			sTo = nTo->GetAttribute(wxT("ID"))->GetValueString();
			sTo.ToLong(&lTo);

			mult = 1;
			cmult = 1;
			multAt = (*eIt)->GetAttribute(wxT("Multiplicity"));
			if (multAt)
			{
				sMult = multAt->GetValueString();
				if(dynamic_cast<SP_DS_MultiplicityAttribute*>(multAt))
				{
					mult = dynamic_cast<SP_DS_MultiplicityAttribute*>(multAt)->GetValue(true);
				}
				else if(dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(multAt))
				{
					mult = dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(multAt)->GetValue(true);
				}
				else if(dynamic_cast<SP_DS_DoubleMarkingDependentMultiplicity*>(multAt))
				{
					cmult = dynamic_cast<SP_DS_DoubleMarkingDependentMultiplicity*>(multAt)->GetValue(true);
				}
			}

			if (nFrom->GetClassName() == SP_DS_CONTINUOUS_PLACE)
			{
				AddArcToMap(m_postContPlMap, lFrom, new SP_ContArc(lTo, cmult, sMult, *eIt));
				AddArcToMap(m_preContTrMap, lTo, new SP_ContArc(lFrom, cmult, sMult, *eIt));
			}
			else if (nFrom->GetClassName() == SP_DS_CONTINUOUS_TRANS)
			{
				AddArcToMap(m_postContTrMap, lFrom, new SP_ContArc(lTo, cmult, sMult, *eIt));
				AddArcToMap(m_preContPlMap, lTo, new SP_ContArc(lFrom, cmult, sMult, *eIt));
			}
			else if (nFrom->GetClassName() == SP_DS_DISCRETE_PLACE)
			{
				AddArcToMap(m_postPlMap, lFrom, new SP_DiscrArc(lTo, (unsigned) mult, sMult, *eIt));
				AddArcToMap(m_preTrMap, lTo, new SP_DiscrArc(lFrom, (unsigned) mult, sMult, *eIt));
			}
			else
			{
				AddArcToMap(m_postTrMap, lFrom, new SP_DiscrArc(lTo, (unsigned) mult, sMult, *eIt));
				AddArcToMap(m_prePlMap, lTo, new SP_DiscrArc(lFrom, (unsigned) mult, sMult, *eIt));
			}
		}
	}
}

wxString SP_ExportPT::GetNetName(const wxString& p_fName)
{
	SP_DS_Metadata* l_pcMeta = m_graph->GetMetadataclass(wxT("General"))->GetElements()->front();
	wxString netName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();
	if(netName.IsEmpty())
	{
		wxFileName fn = wxFileName(p_fName);
		netName = fn.GetName();
	}
	return NormalizeString(netName);
}

wxString SP_ExportPT::NormalizeString(const wxString& p_S)
{
	if (p_S.IsEmpty())
		return p_S;

	bool changed = false;

	wxString tmp = p_S;
	for (size_t i = 0; i < p_S.length(); i++)
	{
		wxChar c = tmp.GetChar(i);
		if (!wxIsalnum(c) && c != '_')
		{
			tmp.SetChar(i, '_');
			changed = true;
		}
	}
	if(wxIsdigit(tmp.GetChar(0)))
	{
		tmp.Prepend("_");
		changed = true;
	}

	if(changed)
	{
		SP_LOGMESSAGE(wxT("changed '") + p_S + wxT("' to '") + tmp + wxT("'"));
	}
	return tmp;
}

// if true duplicate arcs between the same nodes are
// replaced with one arc with a sum of multiplicities
// let successors avoid it
bool DeleteDuplicateArcs(SP_DS_Edge* p_edge1, SP_DS_Edge* p_edge2)
{
	return (p_edge1->GetClassName().Cmp(p_edge2->GetClassName()) == 0);
}

SP_ExportPT::SP_ExportPT() :
	m_doc(NULL), m_graph(NULL)
{
}

SP_ExportPT::~SP_ExportPT()
{
	ClearMaps();
}

bool SP_ExportPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS);
}

bool SP_ExportPT::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();
	m_fileName = p_fileName;

	ClearMaps();

	if (!InitConstants())
		return false;

	if (!InitNodes())
		return false;

	InitArcMaps();

	if (!m_file.Open(m_fileName.c_str(), wxT("wt")) )
	{
		return false;
	}

	// will be implemented in successors
	bool res = DoWrite();

	ClearMaps();

	// close file handle and check if this was ok
	CHECK_BOOL((m_file.Close()), return false);
	return res && !m_bError;
}

bool SP_ExportPT::InitConstants()
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_ListMetadata::const_iterator l_itElem;
	SP_Constant* newC;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(!l_pcMetadataclass)
		return false;

	if(!l_pcMetadataclass->GetPrototype()->GetAttribute(wxT("ConstantList")))
	{
		unsigned int i = 0;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString sId = l_pcConstant->GetAttribute(wxT("ID"))->GetValueString();
			long id = 0;
			sId.ToLong(&id);
			if (m_constantIdMap[id])
			{
				SP_LOGERROR(
						wxString::Format(wxT("constant with duplicate id: %ld"), id));
				SetLastError(wxString::Format(wxT("duplicate constant id %ld"), id));
				return FALSE;
			}
			wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sMetadataType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
			wxString l_sMetadataGroup = l_pcConstant->GetAttribute(wxT("Group"))->GetValueString();
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcConstant->GetAttribute(wxT("ValueList")));
			wxString l_sMetadataValue = l_pcColList->GetActiveCellValue(1);
			double l_Marking = 0;

			SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();
			SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sMetadataName);
			if(l_FE.IsOk())
			{
				SP_FunctionPtr l_Function = l_FE.getFunction();
				if (l_Function->isValue())
				{
					l_Marking = l_Function->getValue();
				}
				else
				{
					//evaluate string
					wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
					if(l_sType == wxT("int"))
					{
						SP_DS_FunctionEvaluatorLong el(l_pcFR, l_Function);
						try
						{
							l_Marking = el();
						}
						catch(dsszmc::functions::Exception& e)
						{
							SP_LOGERROR( wxString(e.getExcName().c_str(), wxConvUTF8) + wxT(": ") + wxString(e.getMsg().c_str(), wxConvUTF8));
						}
					}
					else if(l_sType == wxT("double"))
					{
						SP_DS_FunctionEvaluatorDouble el(l_pcFR, l_Function);
						try
						{
							l_Marking = el();
						}
						catch(dsszmc::functions::Exception& e)
						{
							SP_LOGERROR( wxString(e.getExcName().c_str(), wxConvUTF8) + wxT(": ") + wxString(e.getMsg().c_str(), wxConvUTF8));
						}
					}
				}
			}

			newC = new SP_Constant(id, l_sMetadataName, l_sMetadataType, l_sMetadataGroup, l_sMetadataValue, l_Marking, l_pcConstant);
			m_constantIdMap[id] = newC;
			m_constantNameMap[l_sMetadataName] = newC;
			i++;
		}
	}
	return true;
}



bool SP_ExportPT::WriteDeclarations()
{
	m_file.Write(wxT("Declarations:\n\n"));

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;	

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;

		
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset;

		if( l_pcColList->GetCell(i,1) == wxT("bool") ||
			l_pcColList->GetCell(i,1) == wxT("dot") )
			l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(";\n");
		else
			l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");

		m_file.Write(l_sColorset);
	}


	m_file.Write(wxT("\n"));

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{	
		wxString l_sColorset = wxT("Colorset ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");
		m_file.Write(l_sColorset);
	}

	m_file.Write(wxT("\n"));

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());

	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Variable ") + l_pcColList->GetCell(i,0) + wxT(" : ") + l_pcColList->GetCell(i,1) +wxT(";\n");
		m_file.Write(l_sColorset);
	}

	m_file.Write(wxT("\n"));

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Constant ") + l_pcColList->GetCell(i,0) + wxT(" = ") + l_pcColList->GetCell(i,1) + wxT(" with ") + l_pcColList->GetCell(i,2)+wxT(";\n");
		m_file.Write(l_sColorset);
	}


	m_file.Write(wxT("\n"));

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sColorset = wxT("Function ") + l_pcColList->GetCell(i,0) + wxT(" ") + l_pcColList->GetCell(i,1) + wxT("( ") + l_pcColList->GetCell(i,2)+ wxT(") { ") + l_pcColList->GetCell(i,3)+wxT("; }");
		l_sColorset.Replace(wxT("\n"),wxT(" ") );
		m_file.Write(l_sColorset);
		m_file.Write(wxT("\n"));
	}

	m_file.Write(wxT("\n\n"));

	return true;
}


