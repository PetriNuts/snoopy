
#include "SP_IA_DP_SetterByPosition.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_SetterByPosition::SP_IA_DP_SetterByPosition(SP_IA_POSITION_T p_ePos, bool p_bReset)
: SP_IA_DP_Setter(p_bReset),
m_ePos(p_ePos)
{ }

SP_IA_DP_SetterByPosition::~SP_IA_DP_SetterByPosition()
{ }


SP_IA_DataProcessor*
SP_IA_DP_SetterByPosition::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_SetterByPosition(m_ePos, m_bReset);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_SetterByPosition::DoProcessing(SP_ListData *p_pcData) {

	if (!p_pcData) return 0;
	if (m_ePos == SP_IA_ID && !ProcessSelected()) return p_pcData;

	SP_ListData l_pcResult;
	SP_ListData::iterator it;

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		SP_Data* l_pcData = *it;
		//check if we should only process selected elements
		//if so, just transfer the unselected ones
		if (ProcessSelected() && (l_pcData)->GetIAState() != SP_IA_SELECTED) {
			l_pcResult.push_back(l_pcData);
			continue;
		}
		if (m_bReset && m_ePos != SP_IA_ID) {
			//these elements are discarded from the process
			ReleaseData(l_pcData);
		} else {
			//if we chose to keep the old elements or we talk about the ID-position we add it
			l_pcResult.push_back(l_pcData);
		}

		switch (m_ePos) {
			case SP_IA_NEXT:
				if ((l_pcData)->GetElementType() == SP_ELEMENT_EDGE) {
					SP_DS_Edge *l_Edge = dynamic_cast<SP_DS_Edge*>(l_pcData);
					if (l_Edge->GetTarget()) {
						//if we process only selected data, new elements should be selected, too.
						if (ProcessSelected()) l_Edge->GetTarget()->SetIAState(SP_IA_SELECTED);
						l_pcResult.push_back(l_Edge->GetTarget());
					}
				} else if ((l_pcData)->GetElementType() == SP_ELEMENT_NODE) {
					const SP_ListEdge *l_tmp = dynamic_cast<SP_DS_Node*>(l_pcData)->GetSourceEdges();
					if (l_tmp) {
						for (SP_ListEdge::const_iterator it2 = l_tmp->begin(); it2 != l_tmp->end(); ++it2) {
							SP_DS_Edge* l_pcEdge = *it2;
							//if we process only selected data, new elements should be selected, too.
							if (ProcessSelected()) (l_pcEdge)->SetIAState(SP_IA_SELECTED);
							l_pcResult.push_back(l_pcEdge);
						}
					}
				}
				break;
			case SP_IA_PREV:
				if ((l_pcData)->GetElementType() == SP_ELEMENT_EDGE) {
					SP_DS_Edge *l_Edge = dynamic_cast<SP_DS_Edge*>(l_pcData);
					if (l_Edge->GetSource())	{
						//if we process only selected data, new elements should be selected, too.
						if (ProcessSelected()) l_Edge->GetSource()->SetIAState(SP_IA_SELECTED);
						l_pcResult.push_back(l_Edge->GetSource());
					}
				} else if ((l_pcData)->GetElementType() == SP_ELEMENT_NODE) {
					const SP_ListEdge *l_tmp = dynamic_cast<SP_DS_Node*>(l_pcData)->GetTargetEdges();
					if (l_tmp) {
						for (SP_ListEdge::const_iterator it2 = l_tmp->begin(); it2 != l_tmp->end(); ++it2) {
							SP_DS_Edge* l_pcEdge = *it2;
							//if we process only selected data, new elements should be selected, too.
							if (ProcessSelected()) (l_pcEdge)->SetIAState(SP_IA_SELECTED);
							l_pcResult.push_back(l_pcEdge);
						}
					}
				}
				break;
			default:
				break;
		}
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}
