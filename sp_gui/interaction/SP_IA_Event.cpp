//////////////////////////////////////////////////////////////////////
// $Source: SP_IA_Event.cpp $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/29 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Event.h"
#include "sp_core/base/SP_ElementClass.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Attribute.h"

//note: order is synchronized with SP_IA_EVENT_T
static const wxChar* m_lsTypeStrings[] = {wxT("General"),
										wxT("Single"),
										wxT("Makro"),
										wxT("PedAnimation Step Fwd"),
										wxT("PedAnimation Step Bwd"),
										wxT("PedAnimation PreStep Fwd") };

wxString
SP_IA_Event::GetTriggerTypeString() const { return m_lsTypeStrings[m_eTrigger]; }


SP_IA_Event::SP_IA_Event(SP_Data* src, SP_IA_EVENT_T trig, bool p_bHoldsDataCopy)
: m_bHoldsDataCopy(p_bHoldsDataCopy)
{
	m_lSource = new SP_ListData();

	if (m_bHoldsDataCopy) {
		switch (src->GetElementType()) {
			case SP_ELEMENT_NODE:
				m_lSource->push_back(dynamic_cast<SP_DS_Node*>(src)->Clone());
				wxDELETE(src);
				break;
			case SP_ELEMENT_EDGE:
				m_lSource->push_back(dynamic_cast<SP_DS_Edge*>(src)->Clone());
				wxDELETE(src);
				break;
			case SP_ELEMENT_ATTRIBUTE:
				m_lSource->push_back(dynamic_cast<SP_DS_Attribute*>(src)->Clone());
				wxDELETE(src);
				break;
			default:
				break;
		}
	}	else {
		m_lSource->push_back(src);
	}

	m_eTrigger = trig;
}

SP_IA_Event::SP_IA_Event(SP_ListData* src, SP_IA_EVENT_T trig, bool p_bHoldsDataCopy)
: m_bHoldsDataCopy(p_bHoldsDataCopy)
{
	m_lSource = new SP_ListData();

	if (m_bHoldsDataCopy)
	{
		for (SP_ListData::iterator it = src->begin(); it != src->end(); ++it)
		{
			SP_Data* l_pcData = *it;
			switch (l_pcData->GetElementType()) {
				case SP_ELEMENT_NODE:
					m_lSource->push_back(dynamic_cast<SP_DS_Node*>(l_pcData)->Clone());
					wxDELETE(l_pcData);
					break;
				case SP_ELEMENT_EDGE:
					m_lSource->push_back(dynamic_cast<SP_DS_Edge*>(l_pcData)->Clone());
					wxDELETE(l_pcData);
					break;
				case SP_ELEMENT_ATTRIBUTE:
					m_lSource->push_back(dynamic_cast<SP_DS_Attribute*>(l_pcData)->Clone());
					wxDELETE(l_pcData);
					break;
				default:
					break;
			}
		}
	}
	else
	{
		for (SP_ListData::iterator it = src->begin(); it != src->end(); ++it)
		{
			m_lSource->push_back(*it);
		}
	}

	m_eTrigger = trig;
}


SP_IA_Event::~SP_IA_Event() {
	SP_Data *Data;
	SP_ElementClass *Class;

	if (m_bHoldsDataCopy) {
		SP_ListData::iterator it;
		for (it = m_lSource->begin(); it != m_lSource->end(); ++it) {
			Data = *it;
			//usually the class object is deleted by the graph
			//but this data doesn't belong to any graph, so we need to delete its class object manually
			Class = Data->GetClassObject();
			wxDELETE(Data);
			wxDELETE(Class);
		}
	}
	wxDELETE(m_lSource);
	m_lSource = 0;
}


SP_DS_Graph*
SP_IA_Event::GetSourceGraph() {
	if (!m_lSource || m_lSource->empty()) return 0;

	return m_lSource->front()->GetClassObject()->GetParentGraph();
}


SP_IA_Event*
SP_IA_Event::InitializedCopy(SP_DS_Graph* p_pcGraph) {
	SP_IA_Event* l_pcResult = new SP_IA_Event(m_lSource, m_eTrigger);

	SP_ListData *l_pcEL = l_pcResult->GetSourceElements();
	if (!l_pcEL->empty()) {
		//only the first element must contain the associated graph
		l_pcEL->front()->GetClassObject()->SetParentGraph(p_pcGraph);
	}

	return l_pcResult;
}

bool
SP_IA_Event::Equals(SP_IA_Event *p_pcEvent, bool p_bCheckGraph) {
	//null pointer never equals
	if (!p_pcEvent) return false;
	//same reference always equals
	if (this == p_pcEvent) return true;

	//first checking event trigger type
	if (m_eTrigger != p_pcEvent->GetTriggerType())	return false;

	//now checking parent graph of the elements of the two source lists
	if (p_bCheckGraph && GetSourceGraph() != p_pcEvent->GetSourceGraph()) return false;

	//general events don't have any source elements
	//so getting to this point means equality
	if (m_eTrigger == SP_IA_EVT_GENERAL) return true;

	//finally checking classes (name and type) of the elements of the two lists
	//every class contained in one source list must appear in the other at least once
	SP_ListString l_lElemClasses1, l_lElemClasses2;
	SP_ListData::iterator itS;
	wxString l_sClass;
	wxString l_sKey;
	//filling with elem data of one source list...
	for(itS = m_lSource->begin(); itS != m_lSource->end(); ++itS) {
		l_sClass = (*itS)->GetClassObject()->GetName();
		l_sKey = wxString::Format(wxT("%d|%s"), (*itS)->GetElementType(), l_sClass.c_str());
		if (SP_Find(l_lElemClasses1, l_sKey) == l_lElemClasses1.end()) {
			l_lElemClasses1.push_back(l_sKey);
		}
	}
	//you never know...
	l_lElemClasses1.sort();

	//...now filling the other...
	SP_ListData *l_pcToCheck = p_pcEvent->GetSourceElements();
	for(itS = l_pcToCheck->begin(); itS != l_pcToCheck->end(); ++itS) {
		l_sClass = (*itS)->GetClassObject()->GetName();
		l_sKey = wxString::Format(wxT("%d|%s"), (*itS)->GetElementType(), l_sClass.c_str());
		if (SP_Find(l_lElemClasses2, l_sKey) == l_lElemClasses2.end()) {
			l_lElemClasses2.push_back(l_sKey);
		}
	}
	//again, never know...
	l_lElemClasses2.sort();

	//...and finally return the result of comparing the two
	return l_lElemClasses1 == l_lElemClasses2;
}
