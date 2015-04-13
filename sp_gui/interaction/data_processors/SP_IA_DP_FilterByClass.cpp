// SP_IA_DP_FilterByElementClass.cpp: Implementierung der Klasse SP_IA_DP_FilterByElementClass.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_FilterByClass.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_FilterByClass::SP_IA_DP_FilterByClass(SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcElemClasses(NULL)
{ }

SP_IA_DP_FilterByClass::SP_IA_DP_FilterByClass(SP_ElementClass *p_pcEC, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcElemClasses(NULL)
{
	AddParams(p_pcEC);
}

SP_IA_DP_FilterByClass::SP_IA_DP_FilterByClass(list<SP_ElementClass*> *p_pcEC, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcElemClasses(p_pcEC)
{ }

SP_IA_DP_FilterByClass::~SP_IA_DP_FilterByClass() {

	if (!IsCopy() && m_pcElemClasses) {
		list<SP_ElementClass*>::iterator it;
		for (it = m_pcElemClasses->begin(); it != m_pcElemClasses->end(); ++it) {
			SP_ElementClass *l_pcEC = *it;
			wxDELETE(l_pcEC);
		}
		wxDELETE(m_pcElemClasses);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_FilterByClass::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_FilterByClass(m_pcElemClasses, m_eSelection);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_FilterByClass::DoProcessing(SP_ListData *p_pcData) {

	if (!p_pcData) return 0;

	SP_ListData l_pcResult;
	SP_ListData::iterator it;
	list<SP_ElementClass*>::iterator itEC;
	bool l_bMatch;

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		//check if we should only process selected elements
		//if so, just transfer the unselected ones and go on to the next
		if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
			l_pcResult.push_back(*it);
			continue;
		}
		//no filtering criterea means no filtering,
		//so everything passes and we can go on to the next
		if (!m_pcElemClasses) {
			l_pcResult.push_back(*it);
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) (*it)->SetIAState(m_eSelection);
			continue;
		}

		SP_ElementClass *EC = (*it)->GetClassObject();
		l_bMatch = false;
		for (itEC = m_pcElemClasses->begin(); itEC != m_pcElemClasses->end(); ++itEC) {
			//filter particular class names or just class types (nodes, edges)
			l_bMatch |= ((wxString((*itEC)->GetName()).Cmp(EC->GetName()) == 0 || wxString((*itEC)->GetName()).Cmp(wxT("")) == 0)
					&& (*itEC)->GetElementType() == EC->GetElementType());
		}
		if (l_bMatch) {
			l_pcResult.push_back(*it);
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) (*it)->SetIAState(m_eSelection);
		} else {
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) {
				l_pcResult.push_back(*it);
				if ((*it)->GetIAState() == m_eSelection) (*it)->SetIAState(SP_IA_NORMAL);
			} else {
				ReleaseData(*it);
			}
		}
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}


void
SP_IA_DP_FilterByClass::AddParams(SP_ElementClass *p_pcEC) {

	if (!p_pcEC) return;

	if (!m_pcElemClasses) {
		m_pcElemClasses = new list<SP_ElementClass*>();
	}
	m_pcElemClasses->push_back(p_pcEC);
}


void
SP_IA_DP_FilterByClass::SetParams(SP_ElementClass *p_pcEC) {

	//clean up old parameter
	if (!IsCopy() && m_pcElemClasses) {
		list<SP_ElementClass*>::iterator it;
		for (it = m_pcElemClasses->begin(); it != m_pcElemClasses->end(); ++it) {
			SP_ElementClass *l_pcEC = *it;
			wxDELETE(l_pcEC);
		}
		m_pcElemClasses->clear();
	}

	AddParams(p_pcEC);
}


void
SP_IA_DP_FilterByClass::SetParams(list<SP_ElementClass*> *p_pcEC) {

	//clean up old parameter
	if (!IsCopy() && m_pcElemClasses) {
		list<SP_ElementClass*>::iterator it;
		for (it = m_pcElemClasses->begin(); it != m_pcElemClasses->end(); ++it) {
			SP_ElementClass *l_pcEC = *it;
			wxDELETE(l_pcEC);
		}
		wxDELETE(m_pcElemClasses);
	}

	m_pcElemClasses = p_pcEC;
}
