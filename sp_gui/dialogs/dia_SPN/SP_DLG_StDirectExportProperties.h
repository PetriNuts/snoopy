//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/09/26 11:20:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STDIRECTEXPORTPROPERTIES__
#define __SP_DLG_STDIRECTEXPORTPROPERTIES__

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/filepicker.h>

typedef enum {
  
  SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT,
  SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT,
  SP_ST_SIM_EXPORT_CSV_EXPORT_MEMBERSHIP_FUN

} SP_DS_StExportCSVEditDlgType;

class SP_DLG_StDirectExportProperties : public wxDialog
{

  protected:
  
    SP_DS_StExportCSVEditDlgType m_eDlgType;

    wxChoice* m_pcSpacerChoice;
    wxBoxSizer* m_pcMainSizer;

    wxSizer* m_pcPropertiesSizer;

    wxFilePickerCtrl* m_pcFilePickerCtrl;
    wxCheckBox* m_pcCompressCheckBox;
	wxCheckBox* m_pcAllTracesesForFuzzySimulation;//by george
	wxCheckBox* m_pcExportMembershipFunction;//by george
	 
	bool* m_pbAlltraces;//by george

    wxString* m_psFilename;
    int* m_pnSpacer;
    bool* m_pbCompressExact;

    virtual void LoadData();

    virtual bool SaveData();

  protected:
  
  public:
  
    SP_DLG_StDirectExportProperties( SP_DS_StExportCSVEditDlgType p_eType, 
      wxDialog* p_pcParent,
      wxString* p_psFilename,
      int* p_pnSpacer,
      bool* p_pbCompressExact,
	  bool* p_pbAlltraces,
      const wxString& p_sTitle = wxT("Direct export properties"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

	void OnCheckMembershipFunction(wxCommandEvent& p_cEvent);//by george

    bool IsCompressChecked() { return m_pcCompressCheckBox->GetValue(); }//george 12.2020

	bool IsAllTracesChecked() {
		wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();///Added by G.A
		bool isFuzzyNet = netClass.Contains(wxT("Fuzzy")) ? true : false;

		if(isFuzzyNet)
		return m_pcAllTracesesForFuzzySimulation->GetValue();
		else
		return false;
	}


	bool IsMembershipFunctionChecked() {
		wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();///Added by G.A
		bool isFuzzyNet = netClass.Contains(wxT("Fuzzy")) ? true : false;

		if (isFuzzyNet)
			return m_pcExportMembershipFunction->GetValue();
		else
			return false;
	}

	bool IsMembershipFunctionEnabled() {
		wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();///Added by G.A
		bool isFuzzyNet = netClass.Contains(wxT("Fuzzy")) ? true : false;

		if (isFuzzyNet)
			return m_pcExportMembershipFunction->IsEnabled();
		else
			return false;
	}

 
    DECLARE_CLASS( SP_DLG_StDirectExportProperties )
    DECLARE_EVENT_TABLE()
};

#endif // __SP_DLG_StDirectExportProperties__

