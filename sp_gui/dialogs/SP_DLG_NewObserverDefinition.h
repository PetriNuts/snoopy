/*
 * SP_DLG_NewObserverDefinition.h
 *
 *  Created on: 07.05.2017
 *   Author: Patrick Henschel
 *   Update : George Assaf
 */

#ifndef SP_DLG_NEWOBSERVERDEFINITION_H_
#define SP_DLG_NEWOBSERVERDEFINITION_H_

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"

class SP_DS_Metadataclass;

class SP_DLG_NewObserverDefinition : public wxDialog
{

  private:

	bool l_bWhite;
    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

    wxBoxSizer* m_pcSizer;

    wxGrid* m_pcObserverSetGrid;

    wxArrayString m_asGroups;

	wxArrayString  m_datatypes;
	wxString *datatypes;

	wxString m_sAvailableDatatypes;

	SP_DS_Graph* m_pcGraph;
	SP_DS_Metadataclass* m_pcObservers;

	SP_ListMetadata m_deleted;

	std::set<std::string> m_Places;
	std::set<std::string> m_Transitions;

	std::map<wxString, wxString> m_mObserve2Type;
	std::set<std::string> m_vUnfoldedTransnames;
	std::set<std::string>m_vUnfoldedPlacenames;
	std::set<std::string> m_vAllElements;

  private:

	bool SaveData();
	bool LoadData();

	void InitializeDataTypes();
	void LoadSetNames();

	bool ExistConstant(const wxString& p_sName, const int row);

	void LoadNodes();
	void LoadNodeOfType(const wxString& p_sPlaceType, std::set<std::string>& p_names);

	bool DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue,const bool& b_showTip=true);

  protected:
	bool FindString(wxArrayString& p_asStrArray,wxString& p_sValue);

	//checks the user inputs
	bool CheckInput();

	//by george 2021
	bool IsConst(const wxString& p_sVar);
	void UnfoldNet();
	void OnAssistant(wxCommandEvent& p_cEvent);


  public:

	SP_DLG_NewObserverDefinition( wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Observer Definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );
    void OnAddSet( wxCommandEvent& p_cEvent );
    void OnDeleteSet( wxCommandEvent& p_cEvent );
	void OnCheckFunction( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );

	void OnGridCellValueChanged( wxGridEvent& p_gEvent);
	void OnGridCellSelected(wxGridEvent& ev);
	void OnEditorShown(wxGridEvent& ev);


   DECLARE_CLASS( SP_DLG_NewObserverDefinition )
   DECLARE_EVENT_TABLE()

};

#endif /* SP_DLG_NEWOBSERVERDEFINITION_H_ */
