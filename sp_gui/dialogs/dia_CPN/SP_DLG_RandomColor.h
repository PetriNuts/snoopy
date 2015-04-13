//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: during animation 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_RandomColor__
#define __SP_DLG_RandomColor__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "sp_ds/SP_DS_Graph.h"
#include <wx/filepicker.h>

class SP_DLG_RandomColor: public wxDialog
{
private:

DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_RandomColor)

	
private:	

	wxTextCtrl* m_pcNumberTextCtrl;

	wxComboBox* m_pcColorsComboBoxCtrl;

	wxString m_sNumber;
	wxString m_sColors;

	vector<wxString>* m_pvColors;
	
public:
	SP_DLG_RandomColor(vector<wxString>* p_pvColors, wxWindow *p_parent, const wxString& p_title = wxT("Choose a color"));
	virtual ~SP_DLG_RandomColor();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);

	void OnTextChanged(wxCommandEvent& p_cEvent);
	
	long GetNumber()
	{ 
		long l_nNumber;
		if(!m_sNumber.ToLong(&l_nNumber) )
			l_nNumber = 1;
		return l_nNumber; 
	}
	wxString GetColor(){ return m_sColors; }

	
};

#endif // __SP_DLG_RandomColor__
