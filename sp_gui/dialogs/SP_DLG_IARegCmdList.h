// SP_DLG_IA_RegCmdList.h: Schnittstelle fr die Klasse SP_DLG_IA_RegCmdList.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_DLG_IAREGCMDLIST_H__)
#define __SP_DLG_IAREGCMDLIST_H__


//#include <wx/wx.h>
#include "sp_gui/interaction/SP_IA_Manager.h"

class SP_DLG_IARegCmdList : public wxDialog {

private:
  DECLARE_CLASS(SP_DLG_IARegCmdList)
	DECLARE_EVENT_TABLE()

	SP_IA_Manager *m_pcManager;
	wxBoxSizer *m_pcMainSizer;
	//dialog controls
	wxListBox *m_lbRegCommands;
	wxTextCtrl *m_tcCmdDescription;

	//get all registered commands from the interaction manager
	wxArrayString GetOpenNets();
	//creates the command's description string using its attributes
	wxString CreateDescription(SP_IA_Command *p_pcCmd);

protected:
	void ShowDescription(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);

public:
	SP_DLG_IARegCmdList(wxWindow *p_pcParent, SP_IA_Manager *p_pcMgr);

};

#endif // !defined(__SP_DLG_IAREGCMDLIST_H__)
