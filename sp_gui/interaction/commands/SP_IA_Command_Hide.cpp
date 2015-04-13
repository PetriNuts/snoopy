// SP_IA_Command_Hide.cpp: Implementierung der Klasse SP_IA_Command_Hide.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Command_Hide.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_Command_Hide::SP_IA_Command_Hide()
: SP_IA_Command(SP_IA_CMD_HIDE, wxT("Command: Hide <Default Name>"), false, false),
m_pcCommandHistory(0)
{ }

SP_IA_Command_Hide::SP_IA_Command_Hide(const wxString& p_sName, bool p_bHasConfig, bool p_bCanUndo)
: SP_IA_Command(SP_IA_CMD_HIDE, p_sName, p_bHasConfig, p_bCanUndo),
m_pcCommandHistory(0)
{ }

SP_IA_Command_Hide::~SP_IA_Command_Hide()
{
	if (m_pcCommandHistory) wxDELETE(m_pcCommandHistory);
}


SP_IA_Command*
SP_IA_Command_Hide::Copy(bool p_bTryInit) {
	SP_IA_Command_Hide* l_pcResult = new SP_IA_Command_Hide(GetName(), HasConfig(), CanUndo());
	l_pcResult->SetDescription(m_sDescription);
	l_pcResult->SetNetclassNames(m_sSourceNetclass, m_sTargetNetclass);
	l_pcResult->SetCopyStamp(true);
	if (p_bTryInit) {
		l_pcResult->SetTargetGraph(m_pcTarget);
		l_pcResult->SetTriggerEvent(m_pcTriggerEvent);
		l_pcResult->SetDataProcessors(m_pcDataProcessors);
	}
	return l_pcResult;
}


bool
SP_IA_Command_Hide::Do(SP_ListData* p_pcData) {

	//calculating the elements to hide...
	DoDataProcessing(p_pcData);

	//we found nothing means that something went wrong...
	//...or the connected nets do not really correspond
	if (!p_pcData || p_pcData->empty()) return false;

	SP_ListData::iterator it;
	SP_ListGraphic *l_gr, l_pcGraphics;
	SP_ListGraphic::iterator itgr;

	//collecting graphic objects of the elements to hide, ...
	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		l_gr = (*it)->GetGraphics();
		if (!l_gr) continue;
		for (itgr = l_gr->begin(); itgr != l_gr->end(); ++itgr) {
			if ((*itgr)->GetGraphicType() == SP_GRAPHIC_NODE ||
				(*itgr)->GetGraphicType() == SP_GRAPHIC_EDGE ||
				(*itgr)->GetGraphicType() == SP_GRAPHIC_METADATA)
			{
				l_pcGraphics.push_front(*itgr);
			}
		}
		SP_IA_DataProcessor::ReleaseData(*it, false, true);
	}

	//...creating the delete command with them...
	m_pcCommandHistory = new SP_CMD_Edit(SP_CMD_HIDE, &l_pcGraphics);
	//...and executing it.
	return m_pcCommandHistory->Do();
}


bool
SP_IA_Command_Hide::Undo() {

	bool l_bReturn = m_pcCommandHistory->Undo();

	wxDELETE(m_pcCommandHistory);
	m_pcCommandHistory = 0;

	return l_bReturn;
}


