//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.40 $
// $Date: 2011/08/14 11:20:00 $/* 
 
//////////////////////////////////////////////////////////////////////

#ifndef SP_DLG_ColStUnfolding_H_
#define SP_DLG_ColStUnfolding_H_

#include <wx/wx.h>
#include <wx/grid.h>

#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/filepicker.h>
#include <set>
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_ColPN_Unfolding;
 

class  SP_DLG_ColStUnfolding : public wxDialog
{
 
private:
    wxBoxSizer* m_pcSizer;


	//wxRadioButton* m_pcUnfoldRadioBut;
	//wxRadioButton* m_pcLoadRadioBut;
	//wxFilePickerCtrl* m_pcLoadFilePickerCtrl;
	//wxFilePickerCtrl* m_pcSaveFilePickerCtrl;
	//wxCheckBox* m_pcWriteCheckBox;
	wxRadioBox* m_pcUnfolderRadioBox;
	//wxCheckBox* m_pcCSPCheckBox;
	wxButton* m_pcStartButton;
	wxButton* m_pcPauseButton;
	wxGauge* m_pcUnfoldingProgressGauge;
	wxChoice* m_pcThreadCountComboBox;

	wxStopWatch m_cUnfoldingStopWatch;
	wxStaticText* m_pcUnfoldingStopWatch;

	SP_DS_ColPN_Unfolding* m_pcColPN_Unfolding;
	std::set<wxString> m_SetChoices;//by george
	std::vector<wxChoice*> m_apcComboBoxes;
	wxChoice* m_pcFunctionSetComboBox;
	wxChoice* m_pcFunctionStochSetComboBox;
	wxChoice* m_pcMarkingSetComboBox;
	vector<SP_DS_ColListAttribute*> m_apcColListAttr;
	SP_DS_ColListAttribute* m_apcColListAttrMarking;
	SP_DS_ColListAttribute* m_apcColListAttrFunctions;
	SP_DS_ColListAttribute* m_apcColListAttrFunctionsStoch;
  public:
	

   SP_DLG_ColStUnfolding( SP_DS_ColPN_Unfolding* p_pcColPN_Unfolding, wxWindow* p_pcParent, const wxString& p_sTitle = wxT("Unfolding"),long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    ~SP_DLG_ColStUnfolding();

    void OnDlgOk( wxCommandEvent& p_cEvent );	
    void OnDlgCancel( wxCommandEvent& p_cEvent );
	void OnPause( wxCommandEvent& p_cEvent );	

	//void OnUnfoldRadioButton( wxCommandEvent& p_cEvent );
	//void OnUnLoadRadioButton( wxCommandEvent& p_cEvent );
	

	void OnColoringSetChanged(wxCommandEvent& p_cEvent);
	void OnModyfyingConstants(wxCommandEvent& p_cEvent);
	void OnClose(wxCloseEvent& event);

	bool  IsExistColoringGroup();
	void  LoadColringGroupChoices();
	void  LoadOtherSets();


	void OnModyfyingMarkingGroup(wxCommandEvent& p_cEvent);
	void OnModyfyingFunStochGroup(wxCommandEvent& p_cEvent);
	void OnModyfyingFunContSetGroup(wxCommandEvent& p_cEvent);
	void OnMarkingsetChanged(wxCommandEvent& p_cEvent);
	void OnFunctionChanged(wxCommandEvent& p_cEvent);
	void OnFunctionStochChanged(wxCommandEvent& p_cEvent);

	void SetUnfoldingStopWatch(long p_nTime) 
	{ 
		m_pcUnfoldingStopWatch->SetLabel(wxString::Format(wxT("%.3f sec"), (float)p_nTime/1000)); 
	}
	void SetUnfoldingProgressGauge(long p_nValue)
	{ 
		m_pcUnfoldingProgressGauge->SetValue(p_nValue); 
		Update();
	}

	 wxButton* GetStartButton() { return m_pcStartButton; }

	DECLARE_CLASS( SP_DLG_ColStUnfolding )
	DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColStUnfolding__

