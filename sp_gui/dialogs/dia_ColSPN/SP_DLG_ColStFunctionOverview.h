//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColStFunctionOverview__
#define __SP_DLG_ColStFunctionOverview__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"

class SP_DLG_ColStFunctionOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;
	 SP_DS_Nodeclass* m_pcNodeclass;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcFunctionGrid;

	wxString m_sFunctionType;
	wxString m_sNodeclass;

	map<int, SP_DS_Node*> m_mRow2Node;

    void LoadData();
    void LoadSetNames();

    void SaveData();

    bool ExistSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_ColStFunctionOverview( wxString p_sNodeclass, wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Function overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnCheckFunction( wxCommandEvent& p_cEvent );
    void OnFunctionAssistent( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );

    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

//    void OnCellValueChanged( wxGridEvent& ev );
//    void OnGridCellSelected( wxGridEvent& ev );

    void OnCellClick( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_ColStFunctionOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColStFunctionOverview__

