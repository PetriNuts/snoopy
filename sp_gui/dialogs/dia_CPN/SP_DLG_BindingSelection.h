//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/26 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_BindingSelection__
#define __SP_DLG_BindingSelection__

#include <wx/wx.h>
#include <wx/textctrl.h>

#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include "sp_core/SP_Vector.h"

class SP_DLG_BindingSelection;

class SP_DLG_BindingSelection_ListCtrl : public wxListCtrl
{
  public:

    SP_DLG_BindingSelection_ListCtrl( SP_DLG_BindingSelection* p_pcDlg,
      wxWindow *parent, const wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style = wxLC_ALIGN_LEFT )
    : wxListCtrl( parent, id, pos, size, style )
    {		

		m_pcParentDlg = p_pcDlg;

    };

    void OnDblClick( wxListEvent& event );
	void OnSelected( wxListEvent& event );

  private :

    SP_DLG_BindingSelection* m_pcParentDlg;

    DECLARE_EVENT_TABLE()

};

class SP_DLG_BindingSelection : public wxDialog
{

  private :

    DECLARE_EVENT_TABLE()
   
    wxBoxSizer* m_pcSizer;

	long m_nReturnSelection;
	long m_nSelection;

	vector<wxString> m_BindingArray;

    SP_DLG_BindingSelection_ListCtrl* m_pcBindingListCtrl;


    void LoadData();

    DECLARE_CLASS( SP_DLG_BindingSelection )

  protected :

  public:


    SP_DLG_BindingSelection( vector<wxString>& p_BindingArray,
     wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
      const wxString& p_sTitle = wxT("Binding selection"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );  

	const long GetReturnSelection(){ return m_nReturnSelection; }
	void SetSelection(long p_nSelection) {m_nSelection = p_nSelection;}

};

#endif // __SP_DLG_BindingSelection__

