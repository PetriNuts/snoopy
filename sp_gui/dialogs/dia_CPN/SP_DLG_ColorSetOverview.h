//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColorSetOverview__
#define __SP_DLG_ColorSetOverview__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"




class SP_WDG_ColExtMarkingList;

class SP_DLG_ColorSetOverview : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcColorSetGrid;

	vector<wxString> m_ComboBoxValue;

	wxString m_sColorSetName;


  private:


	bool SaveData();
	bool LoadData();
  
  public:
	
    SP_DLG_ColorSetOverview( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Colorsets Overview"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnImportColors( wxCommandEvent& p_cEvent ); 
    void OnImportColorSet( wxCommandEvent& p_cEvent ); 
    void OnEditColorSet( wxCommandEvent& p_cEvent ); 

	bool ImportData();

	wxString GetColorSetName() { return m_sColorSetName; }

	

   DECLARE_CLASS( SP_DLG_ColorSetOverview )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColorSetOverview__

