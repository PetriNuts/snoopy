// SP_IA_DMP_Colorize.cpp: Implementierung der Klasse SP_IA_DMP_Colorize.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_MapperByAttribute.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_AttributeTranslator.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_FilterByAttribute.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_MapperByAttribute::SP_IA_DP_MapperByAttribute()
: SP_IA_DataProcessor(),
m_pcFromAttrib(NULL),
m_pcToAttrib(NULL),
m_bMap2Selected(false)
{ }

SP_IA_DP_MapperByAttribute::SP_IA_DP_MapperByAttribute(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom, bool p_bMap2Selected)
: SP_IA_DataProcessor(),
m_pcFromAttrib(NULL),
m_pcToAttrib(NULL),
m_bMap2Selected(p_bMap2Selected)
{
	SetParams(p_pcTo, p_eOp, p_pcFrom);
}

SP_IA_DP_MapperByAttribute::~SP_IA_DP_MapperByAttribute()
{
	if (!IsCopy()) {
		if (m_pcToAttrib) wxDELETE(m_pcToAttrib);
		if (m_pcFromAttrib) wxDELETE(m_pcFromAttrib);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_MapperByAttribute::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_MapperByAttribute(m_pcToAttrib, m_eMapOp, m_pcFromAttrib, m_bMap2Selected);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}

SP_ListData*
SP_IA_DP_MapperByAttribute::DoProcessing(SP_ListData *p_pcData) {

	if (!m_pcFromAttrib || !m_pcToAttrib || !p_pcData) return p_pcData;

	//this attribute translator creates us the attribute list which is then passed to the filter
	SP_IA_DP_AttributeTranslator *l_pcTranslator = new SP_IA_DP_AttributeTranslator(m_pcToAttrib, m_eMapOp, m_pcFromAttrib);
	//we set the copy stamp because the passed parameters are owned by the mapper which takes care of them
	l_pcTranslator->SetCopyStamp(true);
	//if we only process selected data, we tell that the translator
	l_pcTranslator->SetProcessSelected(ProcessSelected());
	p_pcData = l_pcTranslator->DoProcessing(p_pcData);
	//this filter takes the attribute list of the translator and filters the target graph with it
	//here we don't need to set the copy stamp, because the Translator creates new attributes anyway
	//so we even want the filter to take care of them
	SP_ListData *l_pcTranslatedAttributes = new SP_ListData();
	SP_ListData l_lNonAttributes;
	if (!ProcessSelected()) {
		*l_pcTranslatedAttributes = *p_pcData;
	} else {
		//if only selected data was processed by the translator there might be some non-attributes left
		//so we should filter them out
		for (SP_ListData::iterator it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			if ((*it)->GetElementType() == SP_ELEMENT_ATTRIBUTE) {
				l_pcTranslatedAttributes->push_back(*it);
			} else {
				l_lNonAttributes.push_back(*it);
			}
		}
	}
	//transfer the previously saved non-attributes back to the data list
	*p_pcData = l_lNonAttributes;

	if (l_pcTranslatedAttributes->empty()) {
		//found no attributes to map
		wxDELETE(l_pcTranslator);
		return p_pcData;
	}

	//retrieving possibly selected data to map to
	SP_ListData *l_pcData2Map = new SP_ListData();
	SP_DS_Graph *l_pcGraph = m_pcParentCmd->GetTargetGraph();
	if (!l_pcGraph) return 0;

	const SP_ListNodeclass *l_lcNC = l_pcGraph->GetNodeclasses();
	SP_ListNodeclass::const_iterator itNC;
	for (itNC = l_lcNC->begin(); itNC != l_lcNC->end(); ++itNC) {
		const SP_ListNode *l_pcNodes = (*itNC)->GetElements();
		SP_ListNode::const_iterator it;
		for (it = l_pcNodes->begin(); it != l_pcNodes->end(); ++it) {
			SP_DS_Node* l_pcNode = *it;
			if (!m_bMap2Selected || (m_bMap2Selected && l_pcNode->GetIAState() == SP_IA_MAP_SELECTED)) {
				l_pcData2Map->push_back(l_pcNode);
				if (ProcessSelected()) (l_pcNode)->SetIAState(SP_IA_SELECTED);
			}
		}
	}
	const SP_ListEdgeclass *l_lcEC = l_pcGraph->GetEdgeclasses();
	SP_ListEdgeclass::const_iterator itEC;
	for (itEC = l_lcEC->begin(); itEC != l_lcEC->end(); ++itEC) {
		const SP_ListEdge *l_pcEdges = (*itEC)->GetElements();
		SP_ListEdge::const_iterator it;
		for (it = l_pcEdges->begin(); it != l_pcEdges->end(); ++it) {
			SP_DS_Edge* l_pcEdge = *it;
			if (!m_bMap2Selected || (m_bMap2Selected && l_pcEdge->GetIAState() == SP_IA_MAP_SELECTED)) {
				l_pcData2Map->push_back(l_pcEdge);
				if (ProcessSelected()) (l_pcEdge)->SetIAState(SP_IA_SELECTED);
			}
		}
	}

	//do mapping
	SP_IA_DataProcessor *l_pcFilter = new SP_IA_DP_FilterByAttribute(m_eMapOp, l_pcTranslatedAttributes);
	l_pcFilter->SetProcessSelected(ProcessSelected());
	l_pcData2Map = (*l_pcFilter).DoProcessing(l_pcData2Map);

	//append the mapping result
	for (SP_ListData::iterator it = l_pcData2Map->begin(); it != l_pcData2Map->end(); ++it) {
		p_pcData->push_back(*it);
	}
	//clean up before leaving
	wxDELETE(l_pcData2Map);
	wxDELETE(l_pcTranslator);
	wxDELETE(l_pcFilter);

	return p_pcData;
}


void
SP_IA_DP_MapperByAttribute::SetParams(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom) {

	if (!p_pcTo || !p_pcFrom || p_eOp < SP_IA_NO_VALUE_COMPARE || p_eOp >= SP_IA_LAST) return;

	if (!IsCopy()) {
		if (m_pcToAttrib) wxDELETE(m_pcToAttrib);
		if (m_pcFromAttrib) wxDELETE(m_pcFromAttrib);
	}

	m_pcFromAttrib = p_pcFrom;
	m_pcToAttrib = p_pcTo;
	m_eMapOp = p_eOp;
}
