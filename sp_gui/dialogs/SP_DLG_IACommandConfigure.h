//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: dialog for individually configuring interaction commands
//////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_DLG_IACOMMANDCONFIGURE_H__)
#define __SP_DLG_IACOMMANDCONFIGURE_H__

#include <wx/wx.h>
#include "sp_gui/interaction/commands/SP_IA_Command.h"

class wxColourPickerEvent;

class SP_DLG_IACommandConfigure : public wxDialog {

private:

  DECLARE_CLASS(SP_DLG_IACommandConfigure)
	DECLARE_EVENT_TABLE()

	SP_IA_Command *m_pcCommand;
	wxBoxSizer *m_pcSizer;

	//event handling functions
	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgCancel(wxCommandEvent& p_cEvent);
	void OnChooseColor(wxColourPickerEvent& p_cEvent);
	void OnSetCurrentNode(wxCommandEvent& p_cEvent);

protected:

public:
	
	SP_DLG_IACommandConfigure(wxWindow *p_pcParent, SP_IA_Command *p_pcCmd);

};

#endif // !defined(__SP_DLG_IACOMMANDCONFIGURE_H__)
