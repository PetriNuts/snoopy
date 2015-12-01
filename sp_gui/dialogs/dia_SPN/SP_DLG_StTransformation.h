//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/03/28 10:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_StTransformation__
#define __SP_DLG_StTransformation__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"

class SP_DLG_StTransformation : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcFunctionGrid;

	vector<wxString> m_ComboBoxValue;
    SP_ListNode* m_TransformingNodes;

  private:

    vector<wxString>  GetComboBoxDefaultValue();
	void LoadTransformData();
	void LoadTransformSetNames();
  
  public:
    
	vector<wxString>  GetComboBoxValue();
	
    SP_DLG_StTransformation( SP_ListNode* TransformingNodes,
	  wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Transformation between transitions"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    

   DECLARE_CLASS( SP_DLG_StTransformation )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_StTransformation__

