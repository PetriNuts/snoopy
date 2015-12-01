//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STMARKINGOVERVIEW__
#define __SP_DLG_STMARKINGOVERVIEW__

#include <wx/wx.h>
#include <wx/grid.h>

class SP_DLG_StMarkingOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;
    /*
     * SPN Place or CPN Continuous Place;
     */
    wxString m_sNodeType;
    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcMarkingGrid;

    void LoadData();
    void LoadSetNames();

    void SaveData();

    bool ExistMarkingSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_StMarkingOverview( wxWindow* p_pcParent,const wxString & p_sNodeType,
      const wxString& p_sTitle = wxT("Marking overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_StMarkingOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_STMARKINGOVERVIEW__

