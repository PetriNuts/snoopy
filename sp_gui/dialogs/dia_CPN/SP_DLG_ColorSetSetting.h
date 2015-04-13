//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#ifndef __SP_DLG_ColorSetSetting__
#define __SP_DLG_ColorSetSetting__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/widgets/SP_WDG_Notebook.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"


#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"

enum SP_CPN_GridNumber
{
	BASICCOLORSETGRID,
	STRUCTUREDCOLORSETGRID,
	ALIASCOLORSETGRID

};

 const wxString m_Datatypes[] =
    {
        _T("dot"),
		_T("string"),
		_T("int"),
        _T("bool"),	
		_T("enum"),
		_T("index")
    };
 
 const wxString m_Structuredtypes[] =
    {
        _T("product"),
		_T("union")
    };

  const wxString m_SubsetNull[] =
    {
        _T(" ")		
    };


class SP_DLG_ColorSetSetting : public wxDialog
{

  private:

    wxString m_sOldCellValue;

    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;


    wxGrid* m_pcBasicColorSetGrid;
	wxGrid* m_pcStructuredColorSetGrid;	
	wxGrid* m_pcAliasColorSetGrid;

	SP_WDG_Notebook* m_pcNotebook;

	vector<wxString> m_ComboBoxValue;

	wxArrayString  m_choices;
	wxString *choices;

	//for storing and loading
	SP_DS_Graph* l_pcGraph ;
	SP_DS_Metadataclass* l_pcMetadataclass;
	
	SP_DS_Metadata* l_pcNewMetadata ;
	SP_DS_ColListAttribute * l_pcColList;

	//Colorset checking
	SP_CPN_ColorProcessing m_cColorProcessing;


  private:


	bool SaveData();
	bool SaveData2();
	bool SaveData4();
	
	bool LoadData();
	bool LoadData2();
	bool LoadData4();

	void InitializeChoice();
	void InitializeBasicChoice();
	void InitializeStructuredChoice();

  
  public:
	
    SP_DLG_ColorSetSetting( wxWindow* p_pcParent, SP_CPN_GridNumber p_GridNumber,
      const wxString& p_sTitle = wxT("ColorSet Definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnAddColorSet( wxCommandEvent& p_cEvent ); 
    void OnDeleteColorSet( wxCommandEvent& p_cEvent ); 
    void OnEditColorSet( wxCommandEvent& p_cEvent ); 
    void OnPageChanged( wxNotebookEvent& event); 
    void OnPageChanging( wxNotebookEvent& event);
	void OnCellValueChanged(wxGridEvent& p_gEvent);
	void OnEditorShown( wxGridEvent& ev );
	void OnCheckFunction( wxCommandEvent& p_cEvent );

	

	void OnExportDeclaration( wxCommandEvent& p_cEvent );

	SP_WDG_NotebookPage* AddPage(const wxString& p_sLabel = wxT(""));


   DECLARE_CLASS( SP_DLG_ColorSetSetting )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColorSetSetting__

