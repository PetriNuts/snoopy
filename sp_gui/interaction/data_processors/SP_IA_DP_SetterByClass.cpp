
#include "SP_IA_DP_SetterByClass.h"
#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/interaction/data_processors/SP_IA_DP_FilterByClass.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_SetterByClass::SP_IA_DP_SetterByClass()
: SP_IA_DP_Setter(true),
m_pcElemClass(NULL),
m_bFromTarget(false)
{ }

SP_IA_DP_SetterByClass::SP_IA_DP_SetterByClass(SP_ElementClass *p_pcEC, bool p_bFromTarget, bool p_bReset)
: SP_IA_DP_Setter(p_bReset),
m_pcElemClass(p_pcEC),
m_bFromTarget(p_bFromTarget)
{ }

SP_IA_DP_SetterByClass::~SP_IA_DP_SetterByClass()
{
	if (!IsCopy()) {
		if (m_pcElemClass) wxDELETE(m_pcElemClass);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_SetterByClass::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_SetterByClass(m_pcElemClass, m_bFromTarget, m_bReset);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_SetterByClass::DoProcessing(SP_ListData *p_pcData) {

	if (!p_pcData) return 0;

	SP_DS_Graph *l_pcGraph;
	if (m_bFromTarget) {
		l_pcGraph = m_pcParentCmd->GetTargetGraph();
	} else {
		l_pcGraph = m_pcParentCmd->GetSourceGraph();
	}
	if (!l_pcGraph) return p_pcData;

	SP_ListData l_pcResult;
	for (SP_ListData::iterator it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		if (!m_bReset || (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED)) {
			l_pcResult.push_back(*it);
		} else {
			ReleaseData(*it);
		}
	}
	*p_pcData = l_pcResult;

	if (m_pcElemClass) {
		switch(m_pcElemClass->GetElementType()) {
			case SP_ELEMENT_NODECLASS: {
				SP_DS_Nodeclass *l_pcNC = l_pcGraph->GetNodeclass(m_pcElemClass->GetName());
				if (!l_pcNC) {
					SP_LOGDEBUG(wxT("ERROR in SP_IA_DP_SetterByClass::DoProcessing(): Specified nodeclass not found. No elements added."));
					return p_pcData;
				} else {
					for (SP_ListNode::const_iterator it = l_pcNC->GetElements()->begin(); it != l_pcNC->GetElements()->end(); ++it) {
						SP_DS_Node* l_pcNode = *it;
						if (SP_Find(*p_pcData, l_pcNode) == p_pcData->end()) {
							if (ProcessSelected()) (l_pcNode)->SetIAState(SP_IA_SELECTED);
							p_pcData->push_back(l_pcNode);
						}
					}
				}
				break;
			}
			case SP_ELEMENT_EDGECLASS: {
				SP_DS_Edgeclass *l_pcEC = l_pcGraph->GetEdgeclass(m_pcElemClass->GetName());
				if (!l_pcEC) {
					SP_LOGDEBUG(wxT("ERROR in SP_IA_DP_SetterByClass::DoProcessing(): Specified edgeclass not found. No elements added."));
					return p_pcData;
				} else {
					for (SP_ListEdge::const_iterator it = l_pcEC->GetElements()->begin(); it != l_pcEC->GetElements()->end(); ++it) {
						SP_DS_Edge* l_pcEdge = *it;
						if (SP_Find(*p_pcData, l_pcEdge) == p_pcData->end()) {
							if (ProcessSelected()) (l_pcEdge)->SetIAState(SP_IA_SELECTED);
							p_pcData->push_back(l_pcEdge);
						}
					}
				}
				break;
			}
			case SP_ELEMENT_METADATACLASS: {
				SP_DS_Metadataclass *l_pcMC = l_pcGraph->GetMetadataclass(m_pcElemClass->GetName());
				if (!l_pcMC) {
					SP_LOGDEBUG(wxT("ERROR in SP_IA_DP_SetterByClass::DoProcessing(): Specified metadataclass not found. No elements added."));
					return p_pcData;
				} else {
					for (SP_ListMetadata::const_iterator it = l_pcMC->GetElements()->begin(); it != l_pcMC->GetElements()->end(); ++it) {
						SP_DS_Metadata* l_pcMetadata = *it;
						if (SP_Find(*p_pcData, l_pcMetadata) == p_pcData->end()) {
							if (ProcessSelected()) (l_pcMetadata)->SetIAState(SP_IA_SELECTED);
							p_pcData->push_back(l_pcMetadata);
						}
					}
				}
				break;
			}
			default:
				break;
		}
	}

	return p_pcData;
}


void
SP_IA_DP_SetterByClass::SetParams(SP_ElementClass *p_pcEC, bool p_bFromTarget) {

	//clean up old parameter
	if (!IsCopy()) {
		if (m_pcElemClass) wxDELETE(m_pcElemClass);
	}

	m_pcElemClass = p_pcEC;
	m_bFromTarget = p_bFromTarget;
}
