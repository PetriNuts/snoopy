//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STPARAMETEROVERVIEW__
#define __SP_DLG_STPARAMETEROVERVIEW__

#include <wx/wx.h>
#include <wx/grid.h>
class SP_DLG_StParameterOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;
    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcParameterGrid;

    void LoadData();
    void LoadSetNames();

    void SaveData();

    bool ExistSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_StParameterOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Parameter overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_StParameterOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_STPARAMETEROVERVIEW__

