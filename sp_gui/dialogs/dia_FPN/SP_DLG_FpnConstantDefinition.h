/*
* SP_DLG_NewConstantDefinition.h
*
*  Created on: 07.02.2019
*      Author: G. Assaf
*/

#ifndef SP_DLG_FPNCONSTANTDEFINITION_H_
#define SP_DLG_FPNCONSTANTDEFINITION_H_

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"
#include"TriangularFN.h"
#include<vector>
#include "sp_gui\dialogs\dia_FPN\SP_DLG_FuzzyNumber_Drawing.h"
class SP_DS_Metadataclass;
//class TriangularFN;
class SP_DLG_FpnConstantDefinition : public wxDialog
{

private:
	std::vector <TriangularFN> ffn;

	bool l_bWhite;
	wxString m_sOldCellValue;

	wxWindow* m_pcParent;

	wxBoxSizer* m_pcSizer;

	wxGrid* m_pcConstantSetGrid;

	wxArrayString m_asGroups;

	wxArrayString  m_datatypes;
	wxString *datatypes;
    
	//TriangularFN tfn;
	wxArrayString m_ftn;// Added by G.A
	wxString *dataFn;//Added by G.A

	wxString m_sAvailableDatatypes;

	SP_DS_Graph* m_pcGraph;
	SP_DS_Metadataclass* m_pcConstants;

	const int m_nMainSet;

	SP_ListMetadata m_deleted;

	SP_SetString m_Places;

	SP_DLG_FuzzyNumber_Drawing* m_pcDraw;

	wxString l_sTFNFromDrawingPanel;
	int m_nRow;
	int m_nCol;

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
	bool CheckFN(const wxString& type, wxString &consta, wxString& constb, wxString& constc, wxString& constd);
	bool  ParseFN(const wxString& type,const wxString &val);
	SP_VectorDouble  GetFNConstants( const wxString &val);

	void OnChildDestroy(wxCloseEvent& event);
protected:
	bool FindString(wxArrayString& p_asStrArray, wxString& p_sValue);

	//checks the user inputs
	bool CheckInput();


public:

	SP_DLG_FpnConstantDefinition(wxWindow* p_pcParent,
		const wxString& p_sTitle = wxT("Constant Definition"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgCancel(wxCommandEvent& p_cEvent);
	void OnAddSet(wxCommandEvent& p_cEvent);
	void OnDeleteSet(wxCommandEvent& p_cEvent);
	void OnEditSet(wxCommandEvent& p_cEvent);
	void OnCheckFunction(wxCommandEvent& p_cEvent);
	void OnDlgApply(wxCommandEvent& p_cEvent);
	void OnAddValueSet(wxCommandEvent& p_cEvent);
	void OnDeleteValueSet(wxCommandEvent& p_cEvent);
	void AddNewChoice(wxCommandEvent& p_cEvent);
	void OnRenameSet(wxCommandEvent& p_cEvent);

	void OnGridCellValueChanged(wxGridEvent& p_gEvent);
	void OnGridCellSelected(wxGridEvent& ev);
	void OnEditorShown(wxGridEvent& ev);
	void OnGridCellDClicked(wxGridEvent& ev);

	DECLARE_CLASS(SP_DLG_FpnConstantDefinition)
	DECLARE_EVENT_TABLE()

};


#endif /* SP_DLG_FPNCONSTANTDEFINITION_H_ */
