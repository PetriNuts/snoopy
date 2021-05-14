/*
 * SP_DLG_NewConstantDefinition.h
 *
 *  Created on: 07.09.2012
 *      Author: Steffen Laarz
 */

#ifndef SP_DLG_NEWCONSTANTDEFINITION_H_
#define SP_DLG_NEWCONSTANTDEFINITION_H_

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"

class SP_DS_Metadataclass;

class SP_DLG_NewConstantDefinition : public wxDialog
{

  private:

	bool l_bWhite;
    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcConstantSetGrid;
	 
    wxArrayString m_asGroups;

	wxArrayString  m_datatypes;
	wxString *datatypes;


	wxArrayString m_ftn;// Added by G.A
	wxString *dataFn;//Added by G.A
	std::set<wxString> m_vAlreadyAcceptedConstants;
	std::map<wxString, wxString> m_mAcceptedConst2Val;
	wxString m_sAvailableDatatypes;

	SP_DS_Graph* m_pcGraph;
	SP_DS_Metadataclass* m_pcConstants;

	const int m_nMainSet;

	SP_ListMetadata m_deleted;

	bool m_bIsApplyExecuted;

	SP_SetString m_Places;

	unsigned m_norgRow;//for move row
	wxArrayString m_RowData;
	bool m_bIsAsc;
	wxButton* m_pcSortingButton;
	wxButton* m_pcSortingCol;
  private:

	bool SaveData();
	bool LoadData();

	void InitializeChoice();
	void InitializeDataTypes();
	void LoadSetNames();

	bool ExistSetName(const wxString& p_sName);
	bool ExistConstant(const wxString& p_sName, const int row);

	void LoadPlaces();
	void LoadPlaceOfType(const wxString& p_sPlaceType);

	bool FindCycle();

	bool DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue);

	bool SubstituteConstant(const wxString& p_sName, wxString& p_sValue);//by george to fix bug of constants dependencies

	void  OnRowRightClick(wxGridEvent& event);//by george for re-ordering constants

	void MoveRow(long p_nToPos);//by george for re-ordering constants

	void OnSortVsets(wxCommandEvent& p_cEvent);//by george for sorting v-sets
	void DeleteGroup(wxCommandEvent& p_cEvent);
	void  OnMultiColSorting(wxCommandEvent&  event);
  protected:
	bool FindString(wxArrayString& p_asStrArray,wxString& p_sValue);

	//checks the user inputs
	bool CheckInput();

	void SortVlaueSets(std::multimap<std::string, float>&, std::vector<std::string>&p_vRes,bool p_bIsAscending = true);//by george
 
	bool  EvalConstantExpression(const wxString& p_sexp, double& p_dVal);

public:

	SP_DLG_NewConstantDefinition( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Constant Definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );
    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
    void OnEditSet( wxCommandEvent& p_cEvent );
	void OnCheckFunction( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );
	void OnAddValueSet( wxCommandEvent& p_cEvent );
	void OnDeleteValueSet( wxCommandEvent& p_cEvent );
	void AddNewChoice( wxCommandEvent& p_cEvent );
	void OnRenameSet( wxCommandEvent& p_cEvent );

	void OnGridCellValueChanged( wxGridEvent& p_gEvent);
	void OnGridCellSelected(wxGridEvent& ev);
	void OnEditorShown(wxGridEvent& ev);
	void OnGridLabelLeftClick(wxGridEvent& event);

   DECLARE_CLASS( SP_DLG_NewConstantDefinition )
   DECLARE_EVENT_TABLE()

};


#endif /* SP_DLG_NEWCONSTANTDEFINITION_H_ */
