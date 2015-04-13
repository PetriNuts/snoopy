//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColorSetSelection__
#define __SP_DLG_ColorSetSelection__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/event.h>

class SP_DLG_ColorSetSelection : public wxDialog
{

  private:
	wxListBox * m_pListBoxIn;
	wxListBox * m_pListBoxOut;
	wxBoxSizer* m_pcBoxSizer;
	wxArrayString  m_ArrayString;

  
  public:
	
    SP_DLG_ColorSetSelection( wxArrayString& p_ArrayString,wxWindow* p_pcParent,
    const wxString& p_sTitle = wxT("Colorsets Selection"),
    long p_nStyle = wxDEFAULT_DIALOG_STYLE );


	void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

	void OnLeftToRight(wxCommandEvent& p_cEvent );
	void OnRightToLeft(wxCommandEvent& p_cEvent );
	

	wxArrayString GetStrings();

   DECLARE_CLASS( SP_DLG_ColorSetSelection )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColorSetSelection__

