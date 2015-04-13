//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2013/09/19 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_MARKINGOVERVIEW__
#define __SP_DLG_MARKINGOVERVIEW__

#include <wx/wx.h>
#include <wx/grid.h>

class SP_DLG_MarkingOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    unsigned m_nNumDiscretePlaces;
    unsigned m_nNumContinuousPlaces;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcMarkingGrid;

    void LoadData();

    void SaveData();

    void RefreshFrame();

  protected:

  public:

    SP_DLG_MarkingOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Marking overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_MarkingOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_MARKINGOVERVIEW__

