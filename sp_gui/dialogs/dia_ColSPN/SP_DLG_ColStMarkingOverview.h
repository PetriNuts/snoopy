//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColStMarkingOverview__
#define __SP_DLG_ColStMarkingOverview__

#include <wx/wx.h>
#include <wx/grid.h>

class SP_DLG_ColStMarkingOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcMarkingGrid;

	wxString m_sNodeClass;

	SP_DS_Graph* m_pcGraph;

	


    void LoadData();
    void LoadSetNames();

    void SaveData();

    bool ExistMarkingSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_ColStMarkingOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Marking overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

	void OnRandomMarking(wxCommandEvent& p_cEvent);

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_ColStMarkingOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColStMarkingOverview__

