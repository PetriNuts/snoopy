//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/26 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_GuardOverview__
#define __SP_DLG_GuardOverview__

#include <wx/wx.h>
#include <wx/grid.h>
#include "sp_ds/SP_DS_Node.h"

class SP_DLG_GuardOverview : public wxDialog
{
  private:

    wxString m_sOldCellValue;
    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;
    wxGrid* m_pcGuardGrid;

	wxString m_sNodeClass;

    void LoadData();
    void LoadSetNames();

    void SaveData();

	vector<SP_DS_Node*> m_vTransNodes;
	void CollectTransNodes();

    bool ExistMarkingSetName( const wxString& p_sName );
    void RefreshFrame();

  protected:

  public:

    SP_DLG_GuardOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Guard overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnRenameSet( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_GuardOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_GuardOverview__

