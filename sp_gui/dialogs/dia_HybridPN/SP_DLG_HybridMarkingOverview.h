/*
 * SP_DLG_HybridMarkingOverview.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 5.7.2010
 * Short Description:
 *  provides a definition for the hybrid marking overview dialog
 */
 //======================================================================================

#ifndef SP_DLG_HYBRIDMARKINGOVERVIEW_H_
#define SP_DLG_HYBRIDMARKINGOVERVIEW_H_

#include <wx/wx.h>
#include <wx/grid.h>

class SP_DLG_HybridMarkingOverview : public wxDialog
{

  private:

	long m_nDiscretePlacesNumber;
	long m_nContinuousPlacesNumber;

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcMarkingGrid;

    bool m_bDiscretePlacesVisible;
    bool m_bContinuousPlacesVisiable;

    SP_VectorString m_sPlacesNammes;

    void LoadData();
    void LoadSetNames();

    void SaveData();

    bool ExistMarkingSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_HybridMarkingOverview( wxWindow* p_pcParent,
                                const wxString& p_sTitle = wxT("Marking overview"),
                                long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

    void OnDiscretePlaceCheckedUnchecked(wxCommandEvent& ev);
    void OnContinuousPlaceCheckedUnchecked(wxCommandEvent & ev);
    void CheckPlacesNamesVisiability();

  DECLARE_CLASS( SP_DLG_HybridMarkingOverview )
  DECLARE_EVENT_TABLE()

};
#endif /* SP_DLG_HYBRIDMARKINGOVERVIEW_H_ */
