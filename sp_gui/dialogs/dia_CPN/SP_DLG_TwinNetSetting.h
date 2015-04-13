//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: dialog to choose a master net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_TwinNetSetting__
#define __SP_DLG_TwinNetSetting__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "sp_ds/SP_DS_Graph.h"
#include <wx/filepicker.h>


class SP_DLG_TwinNetSetting: public wxDialog
{
private:

DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_TwinNetSetting)

protected:
	
private:

	
	
	wxTextCtrl* m_pcColorSetTextCtrl;
	wxTextCtrl* m_pcVariableTextCtrl;
	wxTextCtrl* m_pcColorsTextCtrl;


	wxTextCtrl* m_pcFusionPlaceTextCtrl;
	wxTextCtrl* m_pcScheduledTransitionTextCtrl;
	wxTextCtrl* m_pcExchangeCoarsePlaceTextCtrl;

	wxTextCtrl* m_pcNewPlaceTextCtrl;
	wxTextCtrl* m_pcNewTransitionTextCtrl;

	wxComboBox* m_pcTypeComboBoxCtrl;

	wxTextCtrl* m_pcParameterNameTextCtrl;
	wxTextCtrl* m_pcParameterValueTextCtrl;

	wxCheckBox* m_cbGenerateSideConditions;



	wxString m_sColorSetName;
	wxString m_sVariableName;
	wxString m_sType;
	wxString m_sColors;
	wxString m_sFusionPlaceName;
	wxString m_sScheduleTransitionName;
	wxString m_sCoarsePlaceName;
	wxString m_sNewPlaceName;
	wxString m_sNewTransitionName;

	wxString m_sParameterName;
	wxString m_sParameterValue;

	bool m_bGenerateSideConditions;

	
public:
	SP_DLG_TwinNetSetting(wxWindow *p_parent, const wxString& p_title = wxT("Settings for generating twin nets"));
	virtual ~SP_DLG_TwinNetSetting();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);

	void OnComboBoxSelected(wxCommandEvent& p_cEvent);

	wxString GetColorSetName(){ return m_sColorSetName; }

	wxString GetVariableName(){ return m_sVariableName; }

	wxString GetType(){ return m_sType; }

	wxString GetColors(){ return m_sColors; }

	wxString GetFusionPlaceName(){ return m_sFusionPlaceName; }

	wxString GetScheduleTransitionName(){ return m_sScheduleTransitionName; }

	wxString GetCoarsePlaceName(){ return m_sCoarsePlaceName; }

	wxString GetNewPlaceName(){ return m_sNewPlaceName; }

	wxString GetNewTransitionName() {return m_sNewTransitionName;}

	wxString GetParameterName() {return m_sParameterName;}

	wxString GetParameterValue() {return m_sParameterValue;}

	bool GetGenerateSideConditions() {return m_bGenerateSideConditions;}
	
};

#endif // __SP_DLG_TwinNetSetting__
