//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/11/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_FunctionDefinition__
#define __SP_DLG_FunctionDefinition__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"


class SP_DLG_FunctionDefinition : public wxDialog
{

  private:
	SP_DS_Graph* m_pcGraph;
	bool m_bIsColored;
    wxString m_sOldCellValue;
    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;
    wxGrid* m_pcGrid;
	vector<wxString> m_ComboBoxValue;
	map<int,vector<wxString> > m_FuncParaMap;
	wxArrayString  m_choices;
	SP_Vector2DString m_BackupData;

  private:
	bool SaveData();
	bool LoadData();
	void InitializeChoice();

	bool DoCheckFunction();

	void RestoreBackup();

  public:
    SP_DLG_FunctionDefinition( wxWindow* p_pcParent,
    		const wxString& p_sTitle = wxT("Function Definition"),
    		long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnAddFunction( wxCommandEvent& p_cEvent );
	void OnCheckFunction( wxCommandEvent& p_cEvent );
    void OnDeleteFunction( wxCommandEvent& p_cEvent );
    void OnEditFunction( wxCommandEvent& p_cEvent );
	void OnGridCellValueChanged(wxGridEvent& p_gEvent);
	void OnDlgApply( wxCommandEvent& p_cEvent );

	void OnEditorShown( wxGridEvent& ev );

	void OnAssistent(wxCommandEvent& p_cEvent);

	

   DECLARE_CLASS( SP_DLG_FunctionDefinition )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_FunctionDefinition__

