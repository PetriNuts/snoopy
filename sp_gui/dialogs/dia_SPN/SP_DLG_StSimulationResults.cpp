//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/08/14 11:20:00 $
// @Change M.Herajy 10.6.2010
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/regex.h>
#include "export/SP_EPSDC.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStEditTablePlot.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
//bysl
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_SimulatorThreadStochastic.h"
#include "spsim/spsim.h"
#include "wx/busyinfo.h"
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif
IMPLEMENT_CLASS( SP_DLG_StSimulationResults, SP_DLG_Simulation )

enum
{
	SP_ID_COMBOBOX_SIMULATOR_CHOICE = SP_ID_LAST_ID + 100,

	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,

	SP_ID_BUTTON_MODIFY_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_WEIGHT_SETS,

	SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,

	SP_ID_BUTTON_LOAD_ILFORMULAE,
	SP_ID_BUTTON_ENTER_ILFORMULAE,
	SP_ID_BUTTON_CHECK_ILFORMULAE,

	SP_ID_BUTTON_CHECK_ILFORMULAE_MENU

};
BEGIN_EVENT_TABLE( SP_DLG_StSimulationResults, SP_DLG_Simulation )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_StSimulationResults :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_StSimulationResults :: OnModifyFunctionSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_StSimulationResults :: OnModifyParameterSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DLG_StSimulationResults :: OnModifyDelaySets ) //By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DLG_StSimulationResults :: OnModifyScheduleSets )//By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DLG_StSimulationResults :: OnModifyWeightSets )//By Liu on 6 Mar. 2009

EVT_BUTTON( SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DLG_StSimulationResults :: OnModifyConstantSets )

EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_StSimulationResults :: OnSimulationProperties )

EVT_COMBOBOX(SP_ID_COMBOBOX_SIMULATOR_CHOICE, SP_DLG_StSimulationResults::OnSimulatorSelect)

EVT_MENU( SP_ID_BUTTON_LOAD_ILFORMULAE, SP_DLG_StSimulationResults :: OnLoadApFormulae )
EVT_MENU( SP_ID_BUTTON_ENTER_ILFORMULAE, SP_DLG_StSimulationResults :: OnEnterApFormulae )
EVT_MENU( SP_ID_BUTTON_CHECK_ILFORMULAE, SP_DLG_StSimulationResults :: CheckApFormulae )
EVT_BUTTON( SP_ID_BUTTON_CHECK_ILFORMULAE_MENU, SP_DLG_StSimulationResults :: OnCheckFormulaMenu )

EVT_GRID_LABEL_RIGHT_DCLICK(SP_DLG_StSimulationResults::OnLabelRightDClick)

EVT_THREAD(SP_SIMULATION_THREAD_EVENT, SP_DLG_StSimulationResults::OnSimulatorThreadEvent)

END_EVENT_TABLE()

SP_DLG_StSimulationResults::SP_DLG_StSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, const wxString& p_sTitle, long p_nStyle)

:
SP_DLG_Simulation(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)
{

	int l_nSimulatorType = LoadSimulatorProperties();

	SetMinimalLayout();

	wxSizer* l_pcRowSizer = NULL;

	if(!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		//function set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking overview:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Function set:") ), wxSizerFlags(1).Expand().Border(wxALL, 2));
		m_apcComboBoxes.push_back(new wxComboBox(m_pcPropertyWindowSetsSizer, -1, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
		l_pcRowSizer->Add( m_apcComboBoxes[m_apcComboBoxes.size()-1], wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify") ), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add( l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Weight set:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		m_apcComboBoxes.push_back(new wxComboBox(m_pcPropertyWindowSetsSizer, -1, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_WEIGHT_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Delay set:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		m_apcComboBoxes.push_back(new wxComboBox(m_pcPropertyWindowSetsSizer, -1, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Schedule set:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		m_apcComboBoxes.push_back(new wxComboBox(m_pcPropertyWindowSetsSizer, -1, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

		UpdateChoices();
		SP_SetString::iterator l_itChoice;
		for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
		{
			wxString l_sGroup = *l_itChoice;
			l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
			l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, l_sGroup + wxT(':') ), wxSizerFlags(1).Expand().Border(wxALL, 2));
			m_apcComboBoxes.push_back(new wxComboBox( m_pcPropertyWindowSetsSizer, -1 , wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY, wxDefaultValidator, l_sGroup ));
			l_pcRowSizer->Add( m_apcComboBoxes[m_apcComboBoxes.size()-1], wxSizerFlags(1).Expand().Border(wxALL, 2));
			l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_CONSTANT_SETS, wxT("Modify") ), wxSizerFlags(0).Expand().Border(wxALL, 2));
			m_pcSetsSizer->Add( l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
		}
	}

	// rows of PropertySizer

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Simulator:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
	m_pcSimulatorComboBox = new wxComboBox(m_pcPropertyWindowPropertySizer, SP_ID_COMBOBOX_SIMULATOR_CHOICE, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);
	m_pcSimulatorComboBox->Append(wxT("Gillespie"));
	m_pcSimulatorComboBox->Append(wxT("Tau Leaping"));
	m_pcSimulatorComboBox->Append(wxT("Parameter Free"));
	m_pcSimulatorComboBox->Append(wxT("FAU"));
	m_pcSimulatorComboBox->SetSelection(l_nSimulatorType);
	l_pcRowSizer->Add(m_pcSimulatorComboBox, wxSizerFlags(1).Expand().Border(wxALL, 2));
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_SIMULATION_PROPERTIES, wxT("Properties")), wxSizerFlags(0).Expand().Border(wxALL, 2));
	m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

	// rows of direct export sizer
	l_pcRowSizer = m_pcDirectExportSizer->GetChildren().front()->GetSizer();
	m_pcDirectSingleExportRadioButton = new wxRadioButton(m_pcPropertyWindowDirectExportSizer, -1, wxT("Single trace export"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcDirectSingleExportRadioButton, wxSizerFlags(0).Border(wxALL, 5));
	m_pcDirectSingleExactExportRadioButton = new wxRadioButton(m_pcPropertyWindowDirectExportSizer, -1, wxT("Exact trace export"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcDirectSingleExactExportRadioButton, wxSizerFlags(0).Border(wxALL, 5));

	//Check formula
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_CHECK_ILFORMULAE_MENU, wxT("Check Formula ...")), 1, wxEXPAND | wxALL, 2);
	m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(1).Expand().Border(wxALL, 2));

	SetSizerAndFit(m_pcMainSizer);

	//Create a new worker thread
	m_pcWorkerThread = new SP_DS_SimulatorThreadStochastic(this, m_pcMainSimulator);
}

void SP_DLG_StSimulationResults::OnSimulatorSelect(wxCommandEvent& p_cEvent)
{
	wxDELETE(m_pcMainSimulator);

	m_pcMainSimulator = CreateSimulator(m_pcSimulatorComboBox->GetSelection());
}

spsim::Simulator* SP_DLG_StSimulationResults::CreateSimulator(const int& p_nSimulatorType)
{
	if (p_nSimulatorType == 0)
	{
		return new spsim::Gillespie();
	}
	else if (p_nSimulatorType == 1)
	{
		return new spsim::TauLeaping();
	}
	else if (p_nSimulatorType == 2)
	{
		return new spsim::ParameterFree();
	}
	else if (p_nSimulatorType == 3)
	{
		return new spsim::Fau();
	}

	return new spsim::Gillespie();
}

void SP_DLG_StSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{

	if (m_pcMainSimulator->IsSimulationRunning())
	{
		m_pcMainSimulator->AbortSimulation();
		return;
	}

	LoadSimulatorData();

	if (m_pcDirectExportRadioButton->GetValue())
	{
		m_ExportType = SP_SIM_DIRECT_EXPORT;

		OpenExportFile();
	}
	else
	if (m_pcDirectSingleExportRadioButton->GetValue())
	{
		m_ExportType = SP_SIM_DIRECT_SINGLE_EXPORT;

		OpenExportFile();
	}
	else
	if (m_pcDirectSingleExactExportRadioButton->GetValue())
	{
		m_ExportType = SP_SIM_DIRECT_SINGLE_EXACT_EXPORT;

		OpenExportFile();
	}
	else
	{
		m_ExportType = SP_SIM_NO_EXPORT;
	}

	dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->SetExportType((spsim::ExportType) (m_ExportType));

	if (m_bComAuxVarSingleRun == true)
	{
		dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->SetExportType(spsim::DIRECT_SINGLE_EXPORT);
	}

	Update();

	if (InitializeSimulator())
	{
		SetSimulationProgressGauge(0);

		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		//start the simulator
		StartSimulation();
	}
	else
	{
		SP_LOGERROR( wxT("The simulation can not be initialized"));
	}

	return;

}
void SP_DLG_StSimulationResults::LoadNetInformation()
{
	SP_ListNode::const_iterator l_itElem;

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	unsigned long l_nPosition = 0;
	m_asPlaceNames.clear();

	//places
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_asPlaceNames.push_back(l_sName);
		m_mPlaceName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}

	//transitions
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);

	l_nPosition = 0;
	m_asTransitionNames.clear();

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_asTransitionNames.push_back(l_sName);
		m_mTransitionName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}
}

void SP_DLG_StSimulationResults::LoadSets()
{
	SP_DS_Node* l_pcNode = NULL;

	m_apcColListAttr.clear();

	if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	SP_DLG_Simulation::LoadSets();
}

void SP_DLG_StSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_MarkingOverview* l_pcDlg = new SP_DLG_MarkingOverview(this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void SP_DLG_StSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Transition"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();

}

void SP_DLG_StSimulationResults::OnModifyWeightSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Immediate Transition"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

}

void SP_DLG_StSimulationResults::OnModifyDelaySets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

	}

	l_pcDlg->Destroy();

}

void SP_DLG_StSimulationResults::OnModifyScheduleSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

	}

	l_pcDlg->Destroy();

}

void SP_DLG_StSimulationResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void SP_DLG_StSimulationResults::DirectExportToCSV()
{
	for (unsigned long i = 0; i < m_nResultPointCount; i++)
	{
		DirectSingleExactExportToCSV(0, m_pcMainSimulator->GetOutputStartPoint() + m_pcMainSimulator->GetOutputSampleSize() * i, m_anResultMatrix[i], m_anResultMatrix[i], i);
	}
}

void SP_DLG_StSimulationResults::DirectSingleExportToCSV(long p_nSimulationRunNumber)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	const SP_Vector2DDouble& l_aanSingleResults = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleResultMatrix();
	const SP_Vector2DDouble& l_aanSingleTransitionFireCount = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleRateMatrix();

	DirectSingleExactExportToCSV(p_nSimulationRunNumber, -1, l_aanSingleResults[0], l_aanSingleTransitionFireCount[0], 0);

	for (unsigned long i = 0; i < m_nResultPointCount; i++)
	{
		DirectSingleExactExportToCSV(p_nSimulationRunNumber, m_pcMainSimulator->GetOutputStartPoint() + m_pcMainSimulator->GetOutputSampleSize() * i, l_aanSingleResults[i],
				l_aanSingleTransitionFireCount[i], i);
	}
}

void SP_DLG_StSimulationResults::DirectSingleExactExportToCSV(long p_nSimulationRunNumber, double p_nTimepoint, const SP_VectorDouble& p_anMarking, const SP_VectorDouble& p_anHazardValues,
		long p_nCurrentTimePoint)
{
	if (!m_pcExport)
		return;

	wxString l_sValue;
	wxString l_sName;
	wxString l_sOutput = wxT("");
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	if (p_nTimepoint < 0)
	{
		m_sLastResultExport.Clear();

		l_sOutput << wxT("--- run: ") << p_nSimulationRunNumber << wxT(" ---\n");
		*m_pcExport << l_sOutput;
	}
	else
	{

		SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
		CHECK_POINTER(l_pcAttribute, return);
		wxString l_sElementType = l_pcAttribute->GetValueString();

		SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
		CHECK_POINTER(l_pcCurveInfoList, return);

		l_sOutput = wxT("");
		l_sOutput << p_nTimepoint;
		wxString l_sCurrentRow = wxT("");

		for (unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
		{
			wxString l_sPos = l_pcCurveInfoList->GetCell(l_n, 0);
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			wxString l_sOutType = l_pcCurveInfoList->GetCell(l_n, 1);
			wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n, 2);
			if (l_sCheckState != wxT("1"))
				continue;
			wxString l_sName = l_pcCurveInfoList->GetCell(l_n, 6);

			double l_dResult = 0;
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
			{
				l_dResult = p_anMarking[l_nPos];
			}

			else
				if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
				{
					l_dResult = m_aanAuxPLVarsResults[p_nCurrentTimePoint][l_nPos];
				}

				else
					if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
					{
						l_dResult = p_anHazardValues[l_nPos];
					}

					else
						if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
						{
							l_dResult = m_aanAuxTRVarsResults[p_nCurrentTimePoint][l_nPos];
						}
						else
						{
							l_dResult = 0;
						}

			//write
			l_sCurrentRow << l_sSpacer;
			l_sCurrentRow << l_dResult;
		}

		if (!l_sCurrentRow.IsSameAs(m_sLastResultExport) || !m_bCompressExact)
		{
			l_sOutput << l_sCurrentRow;
			*m_pcExport << l_sOutput << wxT("\n");
		}
		m_sLastResultExport = l_sCurrentRow;
	}

}

void SP_DLG_StSimulationResults::LoadResults()
{
	UpdateXAxisValues();

	UpdateViewer();

	RefreshExternalWindows();
}

SP_DLG_StSimulationResults::~SP_DLG_StSimulationResults()
{
	SaveSimulatorProperties();
}

void SP_DLG_StSimulationResults::OnPrint(wxCommandEvent& p_cEvent)
{
	wxString l_sBuffer(wxT(""));
	wxString l_sText(wxT(""));
	wxString l_sFilename(SP_Core::Instance()->GetRootDocument()->GetUserReadableName());
	wxPrintData l_pcPrintData;

	long l_nResolution = 600;
	long l_nPen = 5;
	long l_nOffset = 200;
	l_pcPrintData.SetPaperId(wxPAPER_A4);
	wxAlignment l_nAlign = wxALIGN_RIGHT;

#ifdef __WXMSW__
	wxPrintDialog l_cPrintDialog(NULL,&l_pcPrintData);
	if(wxID_CANCEL == l_cPrintDialog.ShowModal())
	return;
	wxDC *l_pcDC = l_cPrintDialog.GetPrintDC();
	l_pcPrintData = l_cPrintDialog.GetPrintData();
	l_nResolution = l_pcDC->GetPPI().GetX();
	double l_nFontSize = 7*l_nResolution/72;
#else
	l_sFilename = wxFileSelector(wxT("Save..."), wxT(""), l_sFilename, wxT("eps"), +wxT("*.eps"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (l_sFilename.empty())
		return;

	l_pcPrintData.SetFilename(l_sFilename);
	l_pcPrintData.SetPrintMode(wxPRINT_MODE_FILE);
	wxPostScriptDC* l_pcDC = new wxPostScriptDC(l_pcPrintData);
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	l_pcDC->SetGraphicsContext(wxGraphicsContext::Create());
#endif
	l_nPen = 1;
	l_nOffset = 20;
	l_nResolution = 72;
#if wxABI_VERSION > 30000

#else
	l_pcDC->SetResolution(l_nResolution);
#endif
	l_nAlign = wxALIGN_CENTER;

#endif
	wxSize l_cSize = l_pcDC->GetSize();
	wxRect l_cRect(l_cSize);
	wxRect l_cCurrentRect(l_cRect.x + l_nOffset, l_cRect.y + (2 * l_nOffset), l_cRect.GetWidth() - (2 * l_nOffset), l_cRect.GetHeight() * 3 / 7);

	l_pcDC->StartDoc(l_sFilename);

	/*m_pcPlotCtrl->SetPlotTitle(wxT("Stochastic Result: ")+ SP_Core::Instance()->GetRootDocument()->GetUserReadableName());
	 m_pcPlotCtrl->SetXAxisLabel(wxT("Time"));
	 m_pcPlotCtrl->SetYAxisLabel(wxT("Value"));
	 m_pcPlotCtrl->SetShowPlotTitle(true);
	 m_pcPlotCtrl->SetShowXAxisLabel(true);
	 m_pcPlotCtrl->SetShowYAxisLabel(true);
	 m_pcPlotCtrl->SetGridColour(*wxBLACK);*/

//	m_pcPlotCtrl->SetShowKey(false);
	/*for(long i=0; i<m_pcPlotCtrl->GetCurveCount(); i++)
	 {
	 wxPlotData *l_pcData = m_pcPlotCtrl->GetDataCurve(i);
	 wxBitmap l_cSymbol = l_pcData->GetSymbol(wxPLOTPEN_ACTIVE);

	 wxGenericPen l_cPen = l_pcData->GetPen(wxPLOTPEN_NORMAL);
	 l_cPen.SetWidth(l_nPen);

	 l_pcData->SetPen(wxPLOTPEN_NORMAL, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_ACTIVE, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_SELECTED, l_cPen);

	 }*/

	/*m_pcPlotCtrl->DrawWholePlot(l_pcDC, l_cCurrentRect, l_nResolution);
	 m_pcPlotCtrl->SetShowPlotTitle(false);
	 m_pcPlotCtrl->SetShowXAxisLabel(false);
	 m_pcPlotCtrl->SetShowYAxisLabel(false);

	 for(long i=0; i<m_pcPlotCtrl->GetCurveCount(); i++)
	 {
	 wxPlotData *l_pcData = m_pcPlotCtrl->GetDataCurve(i);
	 wxBitmap l_cSymbol = l_pcData->GetSymbol(wxPLOTPEN_ACTIVE);

	 wxGenericPen l_cPen = l_pcData->GetPen(wxPLOTPEN_NORMAL);
	 l_cPen.SetWidth(1);

	 l_pcData->SetPen(wxPLOTPEN_NORMAL, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_ACTIVE, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_SELECTED, l_cPen);

	 }

	 l_nCurrentX = l_nOffset;
	 l_nCurrentY = l_cCurrentRect.GetBottom();

	 l_cCurrentRect.SetX(l_nCurrentX);
	 l_cCurrentRect.SetY(l_nCurrentY);
	 l_cCurrentRect.SetHeight(l_cSize.GetHeight()-l_nCurrentY);
	 l_pcDC->SetClippingRegion(l_cCurrentRect);
	 l_pcDC->SetDeviceOrigin(wxCoord(l_nCurrentX), wxCoord(l_nCurrentY));

	 l_nMaxX = l_nMaxX<l_nCurrentX?l_nCurrentX:l_nMaxX;
	 l_nMaxY = l_nMaxY<l_nCurrentY?l_nCurrentY:l_nMaxY;

	 wxGrid l_cGrid(this,wxID_ANY, wxPoint(0,0), l_cCurrentRect.GetSize());
	 l_cGrid.Show(true);
	 l_cGrid.CreateGrid(1,15);
	 l_cGrid.SetRowLabelSize(0);
	 l_cGrid.SetColLabelSize(0);
	 l_cGrid.SetDefaultCellOverflow(true);
	 wxFont l_cFont = l_cGrid.GetDefaultCellFont();
	 l_cFont.SetPointSize(l_nFontSize);
	 l_cGrid.SetDefaultCellFont(l_cFont);


	 const SP_ListNode* l_lNodes = m_pcGraph->GetNodeclass(wxT("Place"))->GetElements();
	 SP_ListNode::const_iterator itN;
	 unsigned long l_nNumRows = (l_cGrid.GetNumberRows()-1);
	 if( l_nNumRows < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-l_nNumRows);
	 }
	 long l_nCol = 0, l_nRow = 0;
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("Initial Marking"));
	 for(itN = l_lNodes->begin(); itN != l_lNodes->end();itN++)
	 {
	 l_nRow++;
	 l_nCol=0;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, (*itN)->GetAttribute(wxT("Name"))->GetValueString());
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("="));
	 SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itN)->GetAttribute(wxT("MarkingList")));
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetCell(m_apcComboBoxes[0]->GetSelection(),1));
	 }

	 l_lNodes = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements();
	 l_nNumRows = (l_cGrid.GetNumberRows()-1);
	 if( l_nNumRows < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-l_nNumRows);
	 }
	 l_nCol = 3, l_nRow = 0;
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("Function"));
	 for(itN = l_lNodes->begin(); itN != l_lNodes->end();itN++)
	 {
	 l_nRow++;
	 l_nCol=3;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, (*itN)->GetAttribute(wxT("Name"))->GetValueString());
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("="));
	 SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itN)->GetAttribute(wxT("FunctionList")));
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetCell(m_apcComboBoxes[1]->GetSelection(),1));
	 }

	 l_lNodes = m_pcGraph->GetNodeclass(wxT("Parameter"))->GetElements();
	 l_nNumRows = (l_cGrid.GetNumberRows()-1);
	 if( l_nNumRows < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-l_nNumRows);
	 }
	 l_nCol = 6, l_nRow = 0;
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("Parameter"));
	 for(itN = l_lNodes->begin(); itN != l_lNodes->end();itN++)
	 {
	 l_nRow++;
	 l_nCol=6;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, (*itN)->GetAttribute(wxT("Name"))->GetValueString());
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("="));
	 SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itN)->GetAttribute(wxT("ParameterList")));
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetCell(m_apcComboBoxes[4]->GetSelection(),1));
	 }

	 /////// By Liu on 11 Mar. 2009
	 l_lNodes = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements();
	 l_nNumRows = (l_cGrid.GetNumberRows()-1);
	 if( l_nNumRows < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-l_nNumRows);
	 }
	 l_nCol = 9, l_nRow = 0;
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("Delay"));
	 for(itN = l_lNodes->begin(); itN != l_lNodes->end();itN++)
	 {
	 l_nRow++;
	 l_nCol=9;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, (*itN)->GetAttribute(wxT("Name"))->GetValueString());
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("="));
	 SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itN)->GetAttribute(wxT("DelayList")));
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetCell(m_apcComboBoxes[2]->GetSelection(),1));
	 }
	 ///////
	 ///////
	 l_lNodes = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements();
	 l_nNumRows = (l_cGrid.GetNumberRows()-1);
	 if( l_nNumRows < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-l_nNumRows);
	 }
	 l_nCol = 12, l_nRow = 0;
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("Schedule"));
	 for(itN = l_lNodes->begin(); itN != l_lNodes->end();itN++)
	 {
	 l_nRow++;
	 l_nCol=12;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, (*itN)->GetAttribute(wxT("Name"))->GetValueString());
	 l_cGrid.SetCellAlignment(l_nAlign, l_nRow, l_nCol);
	 l_nCol++;
	 l_cGrid.SetCellValue(l_nRow, l_nCol, wxT("="));
	 SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itN)->GetAttribute(wxT("PeriodicList")));
	 l_nCol++;
	 wxString l_Selection=l_pcColAttr->GetCell(m_apcComboBoxes[2]->GetSelection(),1) +
	 l_pcColAttr->GetCell(m_apcComboBoxes[2]->GetSelection(),2) +
	 l_pcColAttr->GetCell(m_apcComboBoxes[2]->GetSelection(),3);
	 l_cGrid.SetCellValue(l_nRow, l_nCol,l_Selection);
	 }
	 ///////

	 l_cGrid.AutoSize();
	 l_cGrid.ClearSelection();


	 for(l_nRow = 0; l_nRow < l_cGrid.GetNumberRows(); l_nRow++)
	 {
	 for(l_nCol = 0; l_nCol < l_cGrid.GetNumberCols(); l_nCol++)
	 {
	 l_cGrid.DrawCell((*l_pcDC), wxGridCellCoords(l_nRow,l_nCol));
	 }
	 }
	 l_cGrid.DrawCell((*l_pcDC), wxGridCellCoords(0,0));
	 */
	l_pcDC->EndDoc();
	wxDELETE(l_pcDC);
}

void SP_DLG_StSimulationResults::OnLoadApFormulae(wxCommandEvent& p_cEvent)
{
	wxString l_sFilename = wxFileSelector(wxT("Choose a file to open"));
	if (!l_sFilename.empty())
	{
		wxTextFile* l_pcFile = new wxTextFile(l_sFilename);
		if (l_pcFile->Open())
		{
			m_sApFormulae = l_pcFile->GetFirstLine();
			while (!l_pcFile->Eof())
			{
				m_sApFormulae += l_pcFile->GetNextLine();
			}
			if (!m_sApFormulae.IsEmpty())
			{
				SP_DS_ApParser l_ApParser(m_pcGraph);

				l_ApParser.Initialise();
				l_ApParser.Parse(m_sApFormulae);
			}
			l_pcFile->Close();
		}
		wxDELETE(l_pcFile);
	}
}

void SP_DLG_StSimulationResults::OnEnterApFormulae(wxCommandEvent& p_cEvent)
{
	m_sApFormulae = wxGetTextFromUser(wxT("Please enter at least one ap formula."), wxT("AP Formulae"), m_sApFormulae);
	if (!m_sApFormulae.IsEmpty())
	{
		SP_DS_ApParser l_ApParser(m_pcGraph);

		l_ApParser.Initialise();
		l_ApParser.Parse(m_sApFormulae);
	}
}

void SP_DLG_StSimulationResults::CheckApFormulae(wxCommandEvent& p_cEvent)
{
	//clear
	m_aApFormulaHappend.clear();
	m_aApFormulas.clear();
	m_aApResults.clear();
	m_sApFormulas.clear();

	//initialize it
	m_pcApParser = new SP_DS_ApParser(m_pcGraph);

	m_pcApParser->Initialise();

	wxString l_sApFormulae = m_sApFormulae;

	if (!l_sApFormulae.IsEmpty())
	{
		if (m_pcApParser->Parse(l_sApFormulae))
		{
			rfdag::vectorRFDAG l_Expressions = m_pcApParser->GetExpressions();
			for (size_t i = 0; i < l_Expressions.size(); ++i)
			{
				m_aApFormulas.push_back(SP_DS_ApFormula(m_pcGraph, l_Expressions[i]));

				m_aApFormulaHappend.push_back(false);
				m_aApResults.push_back(0);
				m_sApFormulas.push_back(wxString(l_Expressions[i].toString().c_str(), wxConvUTF8));
			}
		}
	}

	m_aApFormulaHappend.assign(m_aApFormulas.size(), false);
	m_aApResults.assign(m_aApFormulas.size(), 0);

	//get the average value of multiple simulation runs
	const SP_Vector2DDouble& l_aanResults = m_pcMainSimulator->GetResultMatrix();
	if (l_aanResults.size() == 0)
		return;

	SP_MapLong2Double l_mnResult;
	for (unsigned long l_nRow = 0; l_nRow < m_nResultPointCount; l_nRow++)
	{
		SP_VectorDouble m_anCurrentMarking;
		for (unsigned long l_nCol = 0; l_nCol < m_pcMainSimulator->GetPlaceCount(); l_nCol++)
		{
			m_anCurrentMarking.push_back(l_aanResults[l_nRow][l_nCol]);
		}
		for (size_t i = 0; i < m_aApFormulas.size(); ++i)
		{
			if (!m_aApFormulaHappend[i])
			{
				double l_nResult = m_aApFormulas[i].PostEvaluate(&m_anCurrentMarking);
				if (l_nResult > 0)
				{
					m_aApFormulaHappend[i] = true;
					m_aApResults[i] += l_nResult;
				}
			}
		}
	}

	//write results
	for (size_t i = 0; i < m_aApFormulas.size(); ++i)
	{
		double l_nValue = m_aApResults[i];

		wxString l_sOutput = wxT("Result of formula ");
		l_sOutput << wxString(m_aApFormulas[i].GetExpressionTree().toString().c_str(), wxConvUTF8);
		SP_LOGMESSAGE( l_sOutput);

		l_sOutput = wxT("Prop: ");
		l_sOutput << l_nValue;
		SP_LOGMESSAGE( l_sOutput);
	}

}

void SP_DLG_StSimulationResults::AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow)
{
	if (m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		return SP_DLG_Simulation::AddNewMarkingSet(p_sMarkingSetName, p_nRow);
}

bool SP_DLG_StSimulationResults::IsMarkingSetNameExist(const wxString& p_sName)
{
	if (m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		return SP_DLG_Simulation::IsMarkingSetNameExist(p_sName);

	return false;
}

void SP_DLG_StSimulationResults::OnLabelRightDClick(wxGridEvent& p_cEvent)
{
}

void SP_DLG_StSimulationResults::OnCheckFormulaMenu(wxCommandEvent& p_cEvent)
{
	wxMenu *l_pcFormulaMenu = new wxMenu;

	l_pcFormulaMenu->Append(SP_ID_BUTTON_LOAD_ILFORMULAE, wxT("Load State Property"));
	l_pcFormulaMenu->Append(SP_ID_BUTTON_ENTER_ILFORMULAE, wxT("Enter State Property"));
	l_pcFormulaMenu->Append(SP_ID_BUTTON_CHECK_ILFORMULAE, wxT("Check State Property"));
	PopupMenu(l_pcFormulaMenu);
}

void SP_DLG_StSimulationResults::UpdateViewer()
{
	CHECK_POINTER(m_pcMainSimulator, return);

	//Update the plot

	//select the suited matrix to view (rate/marking)
	UpdateSimulationMatrix();

	m_apcResultViewers[m_nCurrentViewer]->SetXAxisValues(&m_anXAxisValues);

	m_apcResultViewers[m_nCurrentViewer]->Update();
}

void SP_DLG_StSimulationResults::UpdateSimulationMatrix()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);
	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	unsigned int l_nColCount = 0;

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		//rate
		m_anResultMatrix = m_pcMainSimulator->GetRateResultMatrix();

		l_nColCount = m_pcMainSimulator->GetTransitionCount();
	}
	else
	{
		//marking
		m_anResultMatrix = m_pcMainSimulator->GetResultMatrix();

		l_nColCount = m_pcMainSimulator->GetPlaceCount();
	}

	if (m_pcWorkerThread->GetRunCount() > 1)
	{
		//get the average values
		for (unsigned int l_nRow = 0; l_nRow < m_anResultMatrix.size(); l_nRow++)
			for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
			{
				m_anResultMatrix[l_nRow][l_nCol] /= (double) (m_pcWorkerThread->GetCurrentRunCount() + 1);
			}
	}

	UpdateXAxisValues();
}

bool SP_DLG_StSimulationResults::ComputeAuxiliaryVars()
{

	return true;
}

void SP_DLG_StSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		m_sNodeclass = SP_DS_STOCHASTIC_TRANS;
	}
	else
	{
		m_sNodeclass = SP_DS_DISCRETE_PLACE;
	}

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(m_sNodeclass);

	CHECK_POINTER(l_pcNodeclass, return);

	l_pcAttribute = p_pcView->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	SP_VectorString l_asColours;

	l_asColours.clear();

	//load element colours
	LoadNodeColours(m_sNodeclass, l_asColours);

	//position of the element in the result matrix
	unsigned long l_nPosition = 0;

	for (SP_ListNode::const_iterator l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		unsigned int l_nNewRow = l_pcCurveInfoList->AppendEmptyRow();

		l_pcCurveInfoList->SetCell(l_nNewRow, 0, wxString::Format(wxT("%ld"), l_nPosition));
		l_pcCurveInfoList->SetCell(l_nNewRow, 1, wxT("Unfolded"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 2, wxT("0"));

		if (l_asColours[l_nPosition] == wxT(""))
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, GetColourString(l_nPosition));
		}
		else
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, l_asColours[l_nPosition]);
		}

		l_pcCurveInfoList->SetCell(l_nNewRow, 4, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 5, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 6, l_sName);

		l_nPosition++;
	}
}

void SP_DLG_StSimulationResults::LoadSimulatorData()
{
	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{
		if (m_apcColListAttr[i])
		{
			m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
		}
	}
}

void SP_DLG_StSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}

void SP_DLG_StSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	wxString l_sCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariableName"));

	double l_nStartOutputPoint = m_pcMainSimulator->GetOutputStartPoint();

	p_anXAxisValues.clear();
	if (l_sCategory.IsSameAs(wxT("Time")))
	{
		double l_nTime = 0;

		for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
		{
			p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

			l_nTime += m_pcMainSimulator->GetOutputSampleSize();
		}

	}
	else
		if (l_sCategory.IsSameAs(wxT("Place")) && !l_sSubCategory.IsEmpty())
		{
			unsigned long l_nPlacePosition = m_mPlaceName2Position[l_sSubCategory];

			for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
			{
				double l_nValue = m_pcMainSimulator->GetResultMatrix()[l_nPoint][l_nPlacePosition];

				p_anXAxisValues.push_back(l_nValue);
			}

		}
		else
			if (l_sCategory.IsSameAs(wxT("Transition")) && !l_sSubCategory.IsEmpty())
			{
				unsigned long l_nTransitionPosition = m_mTransitionName2Position[l_sSubCategory];

				for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
				{
					double l_nValue = m_pcMainSimulator->GetRateResultMatrix()[l_nPoint][l_nTransitionPosition];

					p_anXAxisValues.push_back(l_nValue);
				}

			}
			else
			{
				double l_nTime = 0;

				for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
				{
					p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

					l_nTime += m_pcMainSimulator->GetOutputSampleSize();
				}

			}
}

void SP_DLG_StSimulationResults::LoadConnections()
{
	//Map transition name to their positions
	m_pcMainSimulator->MapTransitionNames2Positions();

	m_pcMainSimulator->MapPlaceNames2Positions();

	//Edge
	LoadConnectionOfType(SP_DS_EDGE, spsim::STANDARD_ARC);

	//Read Arc
	LoadConnectionOfType(SP_DS_READ_EDGE, spsim::READ_ARC);

	//Inhibitor edge
	LoadConnectionOfType(SP_DS_INHIBITOR_EDGE, spsim::INHIBITOR_ARC);

	//equal
	LoadConnectionOfType(SP_DS_EQUAL_EDGE, spsim::EQUAL_ARC);

	//reset
	LoadConnectionOfType(SP_DS_RESET_EDGE, spsim::RESET_ARC);

	//Modifier
	LoadConnectionOfType(SP_DS_MODIFIER_EDGE, spsim::MODIFIER_ARC);
}

void SP_DLG_StSimulationResults::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
	const SP_ListEdge* l_pcEdgeList;
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sSourceNodeName, l_sDestNodeName;

	SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;
	;

	SP_DS_NameAttribute* l_pcNameAttribute;

	CHECK_POINTER(m_pcGraph->GetEdgeclass(p_sArcType), return);

	l_pcEdgeList = m_pcGraph->GetEdgeclass(p_sArcType)->GetElements();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		//Get source Node Name
		l_pcSourceNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

		CHECK_POINTER(l_pcSourceNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sSourceNodeName = l_pcNameAttribute->GetValue();

		//Get target Node Name
		l_pcTargetNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

		CHECK_POINTER(l_pcTargetNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sDestNodeName = l_pcNameAttribute->GetValue();

		//get the arc weight as a string
		wxString l_sArcWeight = GetEdgeWeight((*l_itEdge));

		//converted arc weight as a number
		double l_nNumericArcWeight = 0;

		//Get the arc Weight
		if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")))
		{
			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPostTransitionConnection(l_sSourceNodeName, l_sDestNodeName, l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPostSelfModifyingWeights(l_sSourceNodeName, l_sDestNodeName, l_sArcWeight);
			}

		}
		else
		{
			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPreTransitionConnection(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPreSelfModifyingWeights(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_sArcWeight);
			}

		}
	}
}

wxString SP_DLG_StSimulationResults::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{
	wxString l_sWeight;

	//Get the Edge weight
	SP_DS_Attribute* l_pcAtt = p_pcEdge->GetAttribute(wxT("Multiplicity"));

	if (l_pcAtt == NULL) //e.g., modifier arc has no Multiplicity
	{
		return wxT("0.0");
	}

	l_sWeight = l_pcAtt->GetValueString();

	return l_sWeight;
}

bool SP_DLG_StSimulationResults::IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue)
{
	SP_MapString2Double::iterator l_itConstant;

	//explicit constant value
	if (p_sWeight.ToDouble(&p_nReturnValue))
	{
		return true; //constant
	}

	//check constant list
	if ((l_itConstant = m_msParameterName2Value.find(p_sWeight)) != m_msParameterName2Value.end())
	{
		p_nReturnValue = (*l_itConstant).second;

		return true; //constant
	}

	return false; //not a constant
}

void SP_DLG_StSimulationResults::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
{
	switch (event.GetEventTask())
	{
	case SP_THREAD_EVENT_END_SIMULATION:

		//stop the timer no need to update
		m_pcTimer->Stop();

		//write the simulation data
		//dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->WriteResults();

		UpdateSimulationMatrix();

		//Load the final result
		LoadResults();
		SetSimulationProgressGauge(GetSimulatorProgress());
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);

		//user data should be the current run time
		SetSimulationStopWatch(event.GetLongValue());

		//export final result
		if (m_ExportType == SP_SIM_DIRECT_EXPORT)
		{
			DirectExportToCSV();
		}

		if (m_ExportType == SP_SIM_DIRECT_EXPORT || m_ExportType == SP_SIM_DIRECT_SINGLE_EXPORT || m_ExportType == SP_SIM_DIRECT_SINGLE_EXACT_EXPORT)
		{
			CloseExportFile();
		}

		Update();
		break;

	default:
		break;
	}
}
void SP_DLG_StSimulationResults::OnTimer(wxTimerEvent& event)
{
	if (m_pcWorkerThread->GetRunCount() == 1)
	{
		UpdateSimulationMatrix();
	}

	//Load the final result
	LoadResults();

	SetSimulationProgressGauge(GetSimulatorProgress());

	SetSimulationStopWatch(m_pcWorkerThread->GetSimulationRuntime());
}

double SP_DLG_StSimulationResults::GetSimulatorProgress()
{
	CHECK_POINTER(m_pcMainSimulator, return 0.0);

	double l_nSimulatorProgress = m_pcMainSimulator->GetSimulationProgress();

	return l_nSimulatorProgress;
}

bool SP_DLG_StSimulationResults::InitializeSimulator()
{
	double l_nOutputEndPoint = 0;
	double l_nOutputStartPoint;
	long l_nLong0;

	if (m_pcIntervalEndTextCtrl->GetValue().ToDouble(&l_nOutputEndPoint) && l_nOutputEndPoint > 0)
	{
		m_pcMainSimulator->SetOutputEndPoint(l_nOutputEndPoint);

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval end"));
		l_pcAttr->SetValueString(m_pcIntervalEndTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The interval end value must be non-negative real number > 0. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	if (m_pcIntervalStartTextCtrl->GetValue().ToDouble(&l_nOutputStartPoint) && l_nOutputStartPoint >= 0 && l_nOutputStartPoint < l_nOutputEndPoint)
	{
		m_pcMainSimulator->SetOutputStartPoint(l_nOutputStartPoint);

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval start"));
		l_pcAttr->SetValueString(m_pcIntervalStartTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The interval start value must be smaller than the interval end value. The inputed values are invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	if (m_pcResultPointCountTextCtrl->GetValue().ToLong(&l_nLong0) && l_nLong0 > 0)
	{
		m_nResultPointCount = l_nLong0 + 1;

		m_pcMainSimulator->SetOutputSampleSize((l_nOutputEndPoint - l_nOutputStartPoint) / (m_nResultPointCount - 1));

		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_Attribute* l_pcAttr = l_pcSimProp->GetAttribute(wxT("output step"));
		l_pcAttr->SetValueString(m_pcResultPointCountTextCtrl->GetValue());
	}
	else
	{
		SP_MESSAGEBOX(wxT("The output interval count value must be positive real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
		return false;
	}

	wxBusyInfo l_Info(wxT("Initialising, please wait...."), this);

	SetSimulationProgressGaugeRange(100);

	//Places
	LoadPlaces();

	//Transitions
	LoadTransitions();

	//Parameters
	LoadParameters();

	//Load Connections
	LoadConnections();

	return m_pcMainSimulator->Initialise(true);
}

void SP_DLG_StSimulationResults::LoadPlaces()
{
	m_anCurrentMarking.clear();

	unsigned long l_nPosition = 0;

	m_asPlaceNames.clear();
	m_mPlaceName2Position.clear();

	m_anCurrentMarking.clear();

	LoadPlacesOfType(SP_DS_DISCRETE_PLACE, l_nPosition);

	//set initial marking
	m_pcMainSimulator->SetInitialMarking(m_anCurrentMarking);

	//set place names
	m_pcMainSimulator->SetPlaceNames(m_asPlaceNames);
}

void SP_DLG_StSimulationResults::LoadPlacesOfType(const wxString& p_sPlaceType, unsigned long& p_nPosition)
{
	wxString l_sName;
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(p_sPlaceType);

	CHECK_POINTER(l_pcNodeclass, return);

	unsigned long l_nPlaceSize = p_nPosition + l_pcNodeclass->GetElements()->size();

	m_asPlaceNames.resize(l_nPlaceSize);

	m_anCurrentMarking.resize(l_nPlaceSize);

	for (SP_ListNode::const_iterator l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_asPlaceNames[p_nPosition] = l_sName;

		m_mPlaceName2Position[l_sName] = p_nPosition;

		SP_DS_Attribute* l_pcAttribute = (*l_itElem)->GetAttribute(wxT("Marking"));
		CHECK_POINTER(l_pcAttribute, return);

		SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*>(l_pcAttribute);
		long l_nValue = l_pcMarkAttr->GetValue(true);

		m_anCurrentMarking[p_nPosition] = l_nValue;

		p_nPosition++;
	}
}
void SP_DLG_StSimulationResults::LoadTransitions()
{
	//Stochastic Transitions
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	CHECK_POINTER(l_pcNodeclass, return);

	//Immediate Transitions
	SP_DS_Nodeclass* l_pcNodeclassforImmediateTrans;
	l_pcNodeclassforImmediateTrans = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	CHECK_POINTER(l_pcNodeclassforImmediateTrans, return);

	//Timed Transition
	SP_DS_Nodeclass* l_pcNodeclassforDeterminTrans;
	l_pcNodeclassforDeterminTrans = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	CHECK_POINTER(l_pcNodeclassforDeterminTrans, return);

	//Scheduled Transition
	SP_DS_Nodeclass* l_pcNodeclassforScheduedTrans;
	l_pcNodeclassforScheduedTrans = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	CHECK_POINTER(l_pcNodeclassforScheduedTrans, return);

	SP_DS_ColListAttribute* l_pcColList = NULL;

	SP_ListNode::const_iterator l_itElem;

	m_pcMainSimulator->ClearTransitions();

	m_asTransitionNames.clear();

	m_mTransitionName2Position.clear();

	unsigned long l_nTransitionPosition = 0;

	//if the user wants to run the drawn net as continuous nets, then stochastic transitions need to be added as continuous
	spsim::TransitionType l_nTransitionType = spsim::TRANSITION_TYPE_STOCHASTIC;

	//Stochastic Transition
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue( 1);

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, l_nTransitionType);
	}

	//Immediate Transition
	for (l_itElem = l_pcNodeclassforImmediateTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforImmediateTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue( 1);

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, spsim::TRANSITION_TYPE_IMMEDIATE);

	}

	//Timed Transition
	for (l_itElem = l_pcNodeclassforDeterminTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforDeterminTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));

		wxString l_delayvalue = l_pcColList->GetActiveCellValue( 1);

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_delayvalue, spsim::TRANSITION_TYPE_DETERMINISTIC);
	}

	//Scheduled transition
	for (l_itElem = l_pcNodeclassforScheduedTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforScheduedTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
		wxString l_Begin = l_pcColList->GetActiveCellValue( 1);
		wxString l_Repetition = l_pcColList->GetActiveCellValue( 2); //FixedTimedFiring_Single( . ) not realized
		wxString l_End = l_pcColList->GetActiveCellValue( 3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
		wxString l_sTransitionFunction = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, spsim::TRANSITION_TYPE_SCHEDULED);
	}
}

void SP_DLG_StSimulationResults::DoStartSimulation()
{
	unsigned long l_nRunCount = 1;

	SetSimulationStopWatch(0);

	Update();

	//Get the simulator run count
	spsim::Property* l_pcProp = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
	if (l_pcProp != NULL)
	{
		l_nRunCount = l_pcProp->GetValuelong();
	}

	if (m_pcWorkerThread != NULL)
	{
		//Create a new worker thread
		m_pcWorkerThread = new SP_DS_SimulatorThreadStochastic(this, m_pcMainSimulator);
	}

	//Check the thread
	CHECK_POINTER(m_pcWorkerThread, return);

	m_pcWorkerThread->SetRunCount(l_nRunCount);

	//Create the worker thread for this model
	if (m_pcWorkerThread->Create() != wxTHREAD_NO_ERROR)
	{
		SP_LOGERROR( wxT("Can not start the simulation"));
		return;
	}

	//simulation threads have the highest priority
	m_pcWorkerThread->SetPriority(WXTHREAD_MAX_PRIORITY);

	//Run the simulation
	if (m_pcWorkerThread->Run() != wxTHREAD_NO_ERROR)
	{
		SP_LOGERROR( wxT("Can not start the simulation"));
		return;
	}

	CHECK_POINTER(m_pcTimer, return);

	//start the timer
	m_pcTimer->Start(5000);
}

void SP_DLG_StSimulationResults::DoSingleExport(const unsigned long& p_nCurrentRunNumber)
{
	if (m_ExportType == SP_SIM_DIRECT_SINGLE_EXPORT)
	{
		ComputeEachSimulationAuxiliaryVars();

		DirectSingleExportToCSV(p_nCurrentRunNumber);
	}
}
