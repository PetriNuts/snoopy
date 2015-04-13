//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/04/15 10:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_CPNDeclaration__
#define __SP_DLG_CPNDeclaration__

#include <wx/wx.h>
#include <wx/grid.h>

#include<wx/filepicker.h>
#include <wx/ffile.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"

class SP_DLG_CPNDeclaration : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    vector<wxString> m_ComboBoxValue;
    //SP_ListNode* m_TransformingNodes;

  private:

	  wxFilePickerCtrl* m_pcFilePickerCtrl;
	  wxFFile m_file;

   // vector<wxString>  GetComboBoxDefaultValue();
	//void LoadTransformData();
	//void LoadTransformSetNames();
  
  public:
    
	//vector<wxString>  GetComboBoxValue();
	
    SP_DLG_CPNDeclaration( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Export declarations"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    

	bool WriteDeclaration();

	bool WriteColorSets();

   DECLARE_CLASS( SP_DLG_CPNDeclaration )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_CPNDeclaration__

