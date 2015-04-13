//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: dialog for configuring interaction (connections, commands)
//////////////////////////////////////////////////////////////////////////////


#if !defined(__SP_DLG_IACONFIGURE_H__)
#define __SP_DLG_IACONFIGURE_H__

#include <wx/wx.h>
#include "sp_gui/interaction/SP_IA_Manager.h"

class SP_DLG_IAConfigure : public wxDialog  {

private:
  DECLARE_CLASS(SP_DLG_IAConfigure)
	DECLARE_EVENT_TABLE()

	SP_IA_Manager *m_pcManager;
	wxBoxSizer *m_pcMainSizer;
	//dialog controls
	wxListBox *m_lbOpenNets;
	wxListBox *m_lbConnectedNets;
	wxTextCtrl *m_tcSourceName;
	wxTextCtrl *m_tcTargetName;
	wxListBox *m_lbRegCommands;
	wxListBox *m_lbActCommands;
	wxTextCtrl *m_tcCmdDescription;
	wxButton *m_buCmdConfig;

	//get currently open net files, which can be connected to the source net
	wxArrayString GetOpenNets();
	//get currently active net, which serves as the source net
	wxString GetSourceName();
	//creates the command's description string using its attributes
	wxString CreateDescription(SP_IA_Command *p_pcCmd);

	//this map is only for faster lookup 
	SP_MapString2String m_mFilename2Netclass;
	//when activating a command all other commands with the same event are blocked
	map<SP_IA_Event*, wxArrayString> m_mBlockedCmds;

protected:
	//event handling functions
	void OnDlgClose(wxCommandEvent& p_cEvent);
	void OnLoadConfig(wxCommandEvent& p_cEvent);
	void OnSaveConfig(wxCommandEvent& p_cEvent);
	void OnConnect(wxCommandEvent& p_cEvent);
	void OnDisconnect(wxCommandEvent& p_cEvent);
	void OnSelectTarget(wxCommandEvent& p_cEvent);
	void OnActivate(wxCommandEvent& p_cEvent);
	void OnDeactivate(wxCommandEvent& p_cEvent);
	void OnSelectActiveCommand(wxCommandEvent& p_cEvent);
	void OnCommandConfig(wxCommandEvent& p_cEvent);
	void ShowDescription(wxCommandEvent& p_cEvent);
	
public:
	SP_DLG_IAConfigure(wxWindow *p_pcParent, SP_IA_Manager *p_pcMgr);

	SP_PairString *GetSource();
	SP_PairString *GetTarget();
	wxArrayString GetActiveCmds();

	void BlockCommands(SP_IA_Event *p_pcEvent);
	void UnblockCommands(SP_IA_Event *p_pcEvent);

};

#endif // !defined(__SP_DLG_IACONFIGURE_H__)
