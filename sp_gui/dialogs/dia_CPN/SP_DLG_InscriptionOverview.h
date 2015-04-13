//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/26 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_InscriptionOverview__
#define __SP_DLG_InscriptionOverview__

#include <wx/wx.h>
#include <wx/grid.h>
#include "sp_ds/SP_DS_Edge.h"

class SP_DLG_InscriptionOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;
    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;
    wxGrid* m_pcInscriptionGrid; 

	wxString m_sEdgeClass;
	vector<SP_DS_Edge*> m_pcEdgeVector;

    void SaveData();
	void LoadData();
    void RefreshFrame();

	//void SaveDataForColoredCPN();
	//void LoadDataForColoredCPN();

	void CollectEdges();

  protected:

  public:

    SP_DLG_InscriptionOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Expression overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

  DECLARE_CLASS( SP_DLG_InscriptionOverview )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_InscriptionOverview__

