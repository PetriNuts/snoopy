// SP_IA_DataProcessor.cpp: Implementierung der Klasse SP_IA_DataProcessor.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DataProcessor.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DataProcessor::SP_IA_DataProcessor()
: m_pcParentCmd(NULL),
m_bIsCopy(false),
m_bProcessSelected(false)
{ }

SP_IA_DataProcessor::~SP_IA_DataProcessor()
{ }


SP_ListData*
SP_IA_DataProcessor::DoProcessing(SP_Data *p_pcData) {

	if (!p_pcData) return 0;

	SP_ListData *l_pcResult = new SP_ListData();
	l_pcResult->push_back(p_pcData);

	return (*this).DoProcessing(l_pcResult);
}


SP_ListData*
SP_IA_DataProcessor::DoProcessing(SP_DS_Graph *p_pcGraph) {

	if (!p_pcGraph) return 0;

	SP_ListData *l_pcResult = new SP_ListData();
	SP_ListData *l_pcSelectedResult = new SP_ListData();

	const SP_ListNodeclass *l_lcNC = p_pcGraph->GetNodeclasses();
	SP_ListNodeclass::const_iterator itNC;
	for (itNC = l_lcNC->begin(); itNC != l_lcNC->end(); ++itNC) {
		const SP_ListNode *l_pcNodes = (*itNC)->GetElements();
		SP_ListNode::const_iterator it;
		for (it = l_pcNodes->begin(); it != l_pcNodes->end(); ++it) {
			SP_DS_Node* l_pcNode = *it;
			if ((*this).ProcessSelected() && (l_pcNode)->GetIAState() == SP_IA_SELECTED) {
				l_pcSelectedResult->push_back(l_pcNode);
			} else {
				l_pcResult->push_back(l_pcNode);
			}
		}
	}

	const SP_ListEdgeclass *l_lcEC = p_pcGraph->GetEdgeclasses();
	SP_ListEdgeclass::const_iterator itEC;
	for (itEC = l_lcEC->begin(); itEC != l_lcEC->end(); ++itEC) {
		const SP_ListEdge *l_pcEdges = (*itEC)->GetElements();
		SP_ListEdge::const_iterator it;
		for (it = l_pcEdges->begin(); it != l_pcEdges->end(); ++it) {
			SP_DS_Edge* l_pcEdge = *it;
			if ((*this).ProcessSelected() && (l_pcEdge)->GetIAState() == SP_IA_SELECTED) {
				l_pcSelectedResult->push_back(l_pcEdge);
			} else {
				l_pcResult->push_back(l_pcEdge);
			}
		}
	}

	if (!l_pcSelectedResult->empty()) {
		return (*this).DoProcessing(l_pcSelectedResult);
	}

	return (*this).DoProcessing(l_pcResult);
}


SP_IA_DataProcessor*
SP_IA_DataProcessor::InitializedCopy(SP_IA_Command *p_pcPCmd) {

	SP_IA_DataProcessor *l_pcCopy = Copy();
	l_pcCopy->SetParentCommand(p_pcPCmd);

	return l_pcCopy;
}


void
SP_IA_DataProcessor::ReleaseData(SP_Data *p_pcData, bool p_bDelete, bool p_bResetState) {

	if (p_bDelete)
	{
		//before deleting we do at least some basic safety checks
		//which means attributes should not have any parent
		//and nodes and edges should not belong to any graph
		if (p_pcData->GetElementType() == SP_ELEMENT_ATTRIBUTE &&
			!dynamic_cast<SP_DS_Attribute*>(p_pcData)->GetParent())
		{
			wxDELETE(p_pcData);
		}
		else if (!p_pcData->GetClassObject()->GetParentGraph())
		{
			wxDELETE(p_pcData);
		}
	}
	else
	{
		RemoveAttachedAttributes(p_pcData);
		if (p_bResetState || p_pcData->GetIAState() == SP_IA_SELECTED)
		{
			p_pcData->SetIAState(SP_IA_NORMAL);
		}
	}
}


void
SP_IA_DataProcessor::RemoveAttachedAttributes(SP_Data *p_pcData) {

	const SP_ListAttribute *l_pcAttribList = p_pcData->GetAttributes();
	if (!l_pcAttribList) return;
	SP_ListAttribute::const_iterator itA = l_pcAttribList->begin(), itA2;

	//attributes may have attached attributes as well, so remove them first.
	RemoveAttachedAttributes(l_pcAttribList);

	while (itA != l_pcAttribList->end())
	{
		if ((*itA)->GetName().StartsWith(wxT("#IA#")))
		{
			itA2 = itA;
			++itA;
			SP_DS_Attribute* l_pcAttr = p_pcData->RemoveAttribute(*itA2);
			wxDELETE(l_pcAttr);
		} else {
			++itA;
		}
	}
}


void
SP_IA_DataProcessor::RemoveAttachedAttributes(const SP_ListAttribute* p_pcAttributes) {
	SP_ListAttribute::const_iterator it;

	for (it = p_pcAttributes->begin(); it != p_pcAttributes->end(); ++it) {
		RemoveAttachedAttributes(*it);
	}
}


