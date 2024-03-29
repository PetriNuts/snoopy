//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// @Modified and updated: George Assaf
// @Date:15/01/2020
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ConstantDefinition__
#define __SP_DLG_ConstantDefinition__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"

class SP_DS_Metadataclass;

class SP_DLG_ConstantDefinition : public wxDialog
{

  private:


    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcColorSetGrid;

	wxArrayString  m_choices;
	wxString *choices;
	wxString *groups;
	wxArrayString m_asGroups;
	bool l_bWhite;
	 int m_lMain;
	SP_DS_Metadataclass* m_pcConstants;
	SP_ListMetadata m_deleted;
	SP_SetString m_Places;
	SP_DS_Graph* l_pcGraph;
	map<wxString, wxString> m_mID2Val;
	unsigned m_norgRow;//for move row
	wxArrayString m_RowData;
	bool m_bIsAsc;
	wxButton* m_pcSortingButton;
	wxButton* m_pcMovingRowsButton;
	wxButton* m_pcSortingCol;


  private:

    void LoadSetNames();
	bool SaveNewDate();
	bool SaveData();
	bool LoadData();
	void InitializeChoice();
	void LoadHarmonizedData();//Added by G.A for Constants harmonizing
	bool FindString(wxArrayString& p_asStrArray, wxString& p_sValue);
	void AddNewChoice(wxCommandEvent& p_cEvent);
	bool DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue);
	bool CheckInput();
	bool ExistConstant(const wxString& p_sName, const int row);
	void OnGridCellSelected(wxGridEvent& ev);
	void OnEditorShown(wxGridEvent& ev);
	void LoadPlaceOfType(const wxString& p_sPlaceType);
	void LoadPlaces();
	bool  ExistSetName(const wxString& p_sName);
	bool GetIntConstantValue(wxString p_sConstExpr, double& p_nValue);
    bool DoCheckUserInput(const int& p_nRow);

	void OnMultiColSorting(wxCommandEvent&  event);//column-based sorting

	void  OnRowRightClick(wxGridEvent& event);

	void MoveRow(long p_nToPos);

	void DeleteGroup(wxCommandEvent& p_cEvent);
	bool  EvalConstantExpression(const wxString& p_sArcWeight, double& p_dVal);



  public:
	
    SP_DLG_ConstantDefinition( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Constant Definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnAddConstant( wxCommandEvent& p_cEvent ); 
    void OnDeleteColorSet( wxCommandEvent& p_cEvent ); 
    void OnEditColorSet( wxCommandEvent& p_cEvent ); 
	void OnCheckFunction( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );
	void OnGridCellValueChanged(wxGridEvent& p_gEvent);
	void OnAddValueSet(wxCommandEvent& p_cEvent);
	void OnRenameSet(wxCommandEvent& p_cEvent);
	void OnDeleteValueSet(wxCommandEvent& p_cEvent);
	void OnSortVsets(wxCommandEvent& p_cEvent);
   DECLARE_CLASS( SP_DLG_ColorSetSetting )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ConstantDefinition__

