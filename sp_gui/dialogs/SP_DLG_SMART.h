//////////////////////////////////////////////////////////////////////
// $Source: SP_DLG_SMART.h
// $Author: SLiese $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description:  The dialog for the SMART export
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_DLG_SMART_H__)
#define __SP_DLG_SMART_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include "export/SP_ExportConfiguration.h"
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "export/SP_ExportSMART.h"

class SP_GPR_Fonts;


class SP_DLG_SMART: public wxDialog {

private:
  DECLARE_CLASS(SP_DLG_SMART)
	DECLARE_EVENT_TABLE()
	SP_ExportSMART* ESMART;
	
	//partition checkboxes
	wxCheckBox* CheckBox_Part_1;
	wxCheckBox* CheckBox_Part_2;
	wxCheckBox* CheckBox_Part_3;
	wxCheckBox* CheckBox_Part_4;
	wxListBox* ListBoxallpart1;
	wxListBox* ListBoxallpart2;
	wxListBox* ListBoxallpart3;
	
	wxCheckBox* checkbox_GarbageCollection;
	wxComboBox* combobox_GarbageCollection;
	wxCheckBox* checkbox_ParameterWarnings;
	wxComboBox* combobox_ParameterWarnings;
	wxCheckBox* checkbox_StateStorage;
	wxComboBox* combobox_StateStorage;
	wxCheckBox* checkbox_MarkovStorage;
	wxComboBox* combobox_MarkovStorage;
	wxCheckBox* checkbox_RealFormat;
	wxComboBox* combobox_RealFormat;
	wxCheckBox* checkbox_Report;
	wxComboBox* combobox_Report;
	wxCheckBox* checkbox_Verbose;
	wxComboBox* combobox_Verbose;
	wxCheckBox* checkbox_init_option;
	wxComboBox* combobox_init_option;
	wxCheckBox* checkbox_num_states;
	wxComboBox* combobox_num_states;
	wxCheckBox* checkbox_num_arcs;
	wxComboBox* combobox_num_arcs;

	wxCheckBox* checkbox_IgnoreWeightClasses;
	wxComboBox* combobox_IgnoreWeightClasses;
	wxCheckBox* checkbox_InfinityString;
	wxTextCtrl* Textbox_InfinityString;
	wxCheckBox* checkbox_BFSTrace;
	wxComboBox* combobox_BFSTrace;
	wxCheckBox* checkbox_WriteDotOutput;
	wxComboBox* combobox_WriteDotOutput;
	wxCheckBox* checkbox_DotOutputFile;
	wxTextCtrl* Textbox_DotOutputFile;
	wxCheckBox* checkbox_DotPrecision;
	wxComboBox* combobox_DotPrecision;
	wxCheckBox* checkbox_DotTitle;
	wxTextCtrl* Textbox_DotTitle;
	wxCheckBox* checkbox_EmbedWithElim;
	wxComboBox* combobox_EmbedWithElim;
	wxCheckBox* checkbox_EUStrategy;
	wxComboBox* combobox_EUStrategy;
	wxCheckBox* checkbox_GarbageUnit;
	wxComboBox* combobox_GarbageUnit;
	wxCheckBox* checkbox_GarbageSize;
	wxTextCtrl* Textbox_GarbageSize;
	wxCheckBox* checkbox_MatrixByRows;
	wxComboBox* combobox_MatrixByRows;
	wxCheckBox* checkbox_MaxConvergeIters;
	wxTextCtrl* Textbox_MaxConvergeIters;
	wxCheckBox* checkbox_MaxNumericalIters;
	wxTextCtrl* Textbox_MaxNumericalIters;
	wxCheckBox* checkbox_MaxPrintedItems;
	wxTextCtrl* Textbox_MaxPrintedItems;

	
	wxTextCtrl* partition_depth;
	wxTextCtrl* partition_min_number;
	//CTL
	wxTextCtrl* ctl_textbox;
	//staedy state
	wxListBox* ListBox_steadystate;
	wxTextCtrl* Textbox_steadystate;
	//transiente Anaylse
	wxListBox* ListBox_transiente;
	wxTextCtrl* Textbox_transiente;
	wxTextCtrl* Textbox_transiente_time;

protected:
	wxGridSizer* Gridpanel;

	wxBoxSizer *m_pcMainSizer;
	wxBoxSizer *m_pcTopSizer;
	wxBoxSizer *m_pcPageSizer;

		//left side of dialog
		//wxRadioBox *m_rbNetClasses;
	//right side of dialog
	wxNotebook *m_pcNotebook;
	wxScrolledWindow *m_pcNotebookPage;

	wxSpinCtrl *m_scGridSpacing;
	wxCheckBox *m_cbGridVisible;
	int lastSelectedPartition;



	SP_ExportConfiguration *ExportConfig;
	SP_GPR_Fonts *m_pcFontPrefs;

	wxString *m_sNetClasses;
	wxString m_sCurrentNetClass;

	
	void UpdateCanvas();

protected:

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgCancel(wxCommandEvent& p_cEvent);
	//void OnKlick(wxCommandEvent& p_cEvent);
	//void OnUpdateNetClass(wxCommandEvent& p_cEvent);
	//void OnUpdateTransShape(wxCommandEvent& p_cEvent);
	//void OnUpdateTransWidth(wxSpinEvent& p_cEvent);
	//void OnUpdateTransHeight(wxSpinEvent& p_cEvent);
	//void OnUpdateFont(wxCommandEvent& p_cEvent);


public:
	SP_DLG_SMART(wxWindow *p_pcParent);
	wxString getnextToken(wxString data ,long fileSize,long& atpos);
	//Partition
	void Load_Partition_fromDATA(wxString data ,long fileSize);
	void OnKlick_checkbox_partition1(wxCommandEvent& p_cEvent);
	void OnKlick_checkbox_partition2(wxCommandEvent& p_cEvent);
	void OnKlick_checkbox_partition3(wxCommandEvent& p_cEvent);
	void OnKlick_checkbox_partition4(wxCommandEvent& p_cEvent);
	void ChoosePartition(bool p_value);
	void Partition_Button1(wxCommandEvent& p_cEvent);
	void Partition_Button2(wxCommandEvent& p_cEvent);
	void Partition_Button3(wxCommandEvent& p_cEvent);
	void Partition_Button4(wxCommandEvent& p_cEvent);
	void Partition_Save(wxCommandEvent& p_cEvent);
	void Partition_Load(wxCommandEvent& p_cEvent);
	void PartitionList_Click(wxCommandEvent& p_cEvent);
	//CTL
	void CTL_LOAD(wxCommandEvent& p_cEvent);
	void CTL_SAVE(wxCommandEvent& p_cEvent);
	void CTL_CLEAR(wxCommandEvent& p_cEvent);
	void CTL_CHECK(wxCommandEvent& p_cEvent);
	//SteadyState
	void SteadyStateList_Click(wxCommandEvent& p_cEvent);
	void SteadyState_ADD(wxCommandEvent& p_cEvent);
	void SteadyState_REMOVE(wxCommandEvent& p_cEvent);
	void SteadyState_CLEAR(wxCommandEvent& p_cEvent);
	void SteadyState_CHECK(wxCommandEvent& p_cEvent);
	void SteadyState_LOAD(wxCommandEvent& p_cEvent);
	void SteadyState_SAVE(wxCommandEvent& p_cEvent);

	//Transiente Analyse
	void TransienteList_Click(wxCommandEvent& p_cEvent);
	void Transiente_ADD(wxCommandEvent& p_cEvent);
	void Transiente_REMOVE(wxCommandEvent& p_cEvent);
	void Transiente_CLEAR(wxCommandEvent& p_cEvent);
	void Transiente_CHECK(wxCommandEvent& p_cEvent);
	void Transiente_LOAD(wxCommandEvent& p_cEvent);
	void Transiente_SAVE(wxCommandEvent& p_cEvent);

	void IL_SAVE(wxString p_line);
	wxString IL_LOAD();

	void CreateNotebookPages();


	void SetConfiguration(SP_ExportConfiguration *EC);
	void SetExportSMART(SP_ExportSMART* exp);
	virtual ~SP_DLG_SMART();
	
	void UpdateListboxes();
	void savesettings();
	bool PreCheck();
};

#endif // !defined(__SP_DLG_SMART_H__)
