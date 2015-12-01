//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ConstantDefinition__
#define __SP_DLG_ConstantDefinition__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"



class SP_DLG_ConstantDefinition : public wxDialog
{

  private:


    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcColorSetGrid;

	wxArrayString  m_choices;
	wxString *choices;

  private:

	bool SaveData();
	bool LoadData();
	void InitializeChoice();
  
  public:
	
    SP_DLG_ConstantDefinition( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Constant Definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnAddColorSet( wxCommandEvent& p_cEvent ); 
    void OnDeleteColorSet( wxCommandEvent& p_cEvent ); 
    void OnEditColorSet( wxCommandEvent& p_cEvent ); 
	void OnCheckFunction( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );
	void OnGridCellValueChanged(wxGridEvent& p_gEvent);
	

   DECLARE_CLASS( SP_DLG_ColorSetSetting )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ConstantDefinition__

