//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: dialog to choose a master net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_ChooseMasterNet__
#define __SP_DLG_ChooseMasterNet__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "sp_ds/SP_DS_Graph.h"
#include <wx/filepicker.h>

class SP_DLG_ChooseMasterNet: public wxDialog
{
private:

DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_ChooseMasterNet)

protected:
	
private:
	wxFilePickerCtrl* m_pcFilePickerCtrl;
	wxTextCtrl* m_pcColorSetTextCtrl;
	wxTextCtrl* m_pcVariableTextCtrl;

	wxString m_sFilePath;
	wxString m_sColorSetName;
	wxString m_sVariableName;
	
public:
	SP_DLG_ChooseMasterNet(wxWindow *p_parent, const wxString& p_title = wxT("Colorizing"));
	virtual ~SP_DLG_ChooseMasterNet();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);

	wxString GetFilePath(){ return m_sFilePath; }

	wxString GetColorSetName(){ return m_sColorSetName; }

	wxString GetVariableName(){ return m_sVariableName; }
	
};

#endif // __SP_DLG_ChooseMasterNet__
