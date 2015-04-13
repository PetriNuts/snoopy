//////////////////////////////////////////////////////////////////////
// $Source: SP_IA_Manager.h $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/29 $
// Short Description: this class manages the entire interaction
//////////////////////////////////////////////////////////////////////

#ifndef __SP_IA_MANAGER_H__
#define __SP_IA_MANAGER_H__

#include <wx/wx.h>
#include "sp_core/SP_Map.h"

#include "sp_gui/interaction/commands/SP_IA_Command.h"
#include "sp_gui/interaction/SP_IA_Event.h"

class SP_DS_Graph;


class SP_IA_Manager {

private:
	//list of command prototypes, registered by netclasses
	//mapping key is the <source netclass|target netclass>
	map<wxString, list<SP_IA_Command*>*> m_mRegisteredCommands;
	//map from events to active commands
	map<SP_IA_Event*, list<SP_IA_Command*>*> m_mActiveCommands;
	//map from source net to connected target nets
	map<wxString, wxArrayString> m_mConnectedNets;
	//list of "source|target" (entry means: connection is enabled)
	SP_ListString m_lEnabledConnections;
	//map from filename to graph (for saving lookup results)
	map<wxString, SP_DS_Graph*> m_mFilename2Graph;
	//list of executed commands of last submitted event
	//needed for Undo()
	list<SP_IA_Command*> m_lCommandHistory;

	//this function is used to find activated commands of enabled connections, which are stored in p_pcCmdList
	//it also returns a reference to the entire list of active commands for adding further ones
	list<SP_IA_Command*> *FindActiveCommands(SP_IA_Event* p_pcEvent, list<SP_IA_Command*> *p_pcCmdList = 0);
	//this function is called by SubmitEvent(...)
	//it executes the commands that are activated for a certain event
	bool ExecuteMatchingCommands(SP_IA_Event* p_pcEvent);

protected:

public:
	SP_IA_Manager();
	virtual ~SP_IA_Manager();

	//this function receives interaction events and starts command execution
	bool SubmitEvent(SP_IA_Event* p_pcEvent);
	//this function undoes the last executed commands
	void UndoLastSubmit(SP_DS_Graph *p_pcGraph = 0, bool p_bForceUndo = false);
	//this is called for registering command prototypes
	bool RegisterCommand(SP_IA_Command* p_pcCmd);
	//these are called for (de-)activating command prototypes
	bool ActivateCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName);
	bool DeactivateCommand(const wxString& p_sSource, const wxString& p_sTarget = wxT(""), const wxString& p_sCmdName = wxT(""));

	//just getters
	SP_IA_Command *GetRegisteredCommand(const wxString& p_pcSource, const wxString& p_pcTarget, const wxString& p_sCmdName);
	SP_IA_Command *GetActiveCommand(const wxString& p_sSource, const wxString& p_sTarget, const wxString& p_sCmdName);
	wxArrayString GetConnectedNets(const wxString& source);
	wxArrayString GetRegisteredCommandNames(const wxString& p_sSource = wxT(""), const wxString& p_sTarget = wxT(""));
	wxArrayString GetActiveCommandNames(const wxString& p_sSource, const wxString& p_sTarget);
	SP_DS_Graph *GetGraphFromFilename(const wxString& p_sFName);
	wxString GetFilenameFromGraph(SP_DS_Graph *p_pcGraph);
	SP_MDI_Doc *GetDocFromGraph(SP_DS_Graph *p_pcGraph);

	//this actually connects two nets, so commands can be activated for this connection
	bool ConnectNet(const wxString& p_sSource, const wxString& p_sTarget, bool p_bAutoEnable = true);
	//these functions disconnect nets
	bool DisconnectNet(const wxString& p_sSource, const wxString& p_sTarget);
	bool DisconnectNet(const wxString& p_sNet, bool p_bFileClosed = false);

	//these provide the opportunity to temporaryly en- or disable a connection
	bool IAModeEnabled(const wxString& p_sSource, const wxString& p_sTarget = wxT(""));
	bool IAModeEnabled(SP_DS_Graph *p_pcSource = 0, SP_DS_Graph *p_pcTarget = 0);
	void EnableConnection(bool p_bEnable, const wxString& p_sSource = wxT(""), const wxString& p_sTarget = wxT(""));

};

#endif // __SP_IA_MANAGER_H__
