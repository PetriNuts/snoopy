
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <wx/ffile.h>
#include <wx/busyinfo.h>
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNColDirSimResults.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"

#include "sp_gui/dialogs/SP_DLG_ColSimulationProperties.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"

//New simulator header file
#include "spsim/spsim.h"
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif
IMPLEMENT_CLASS(SP_DLG_CPNColDirSimResults, SP_DLG_DirColorSimulation)

enum
{
	SP_ID_BUTTON_MODIFY_MARKING_SETS = SP_ID_LAST_ID + 100,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,
	SP_ID_CHOICE_MARKING_SETS,
	SP_ID_CHOICE_FUNCTION_SETS,
	SP_ID_CHOICE_PARAMETER_SETS,
	SP_ID_CHOICE_SOLVER,
	SP_ID_RADIOBOX_SOLVER_TYPE,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,
	SP_ID_BUTTON_SAVE_ODE
};
BEGIN_EVENT_TABLE(SP_DLG_CPNColDirSimResults, SP_DLG_DirColorSimulation)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_CPNColDirSimResults::OnModifyMarkingSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_CPNColDirSimResults::OnModifyFunctionSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_CPNColDirSimResults::OnModifyParameterSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DLG_CPNColDirSimResults::OnModifyConstantSets)

EVT_CHOICE(SP_ID_CHOICE_MARKING_SETS, SP_DLG_CPNColDirSimResults::OnMarkingSetChanged)
EVT_CHOICE(SP_ID_CHOICE_FUNCTION_SETS, SP_DLG_CPNColDirSimResults::OnFunctionSetChanged)
EVT_CHOICE(SP_ID_CHOICE_PARAMETER_SETS, SP_DLG_CPNColDirSimResults::OnParameterSetChanged)
EVT_CHOICE(SP_ID_CHOICE_SOLVER, SP_DLG_CPNColDirSimResults::OnSolverChanged)
EVT_RADIOBOX(SP_ID_RADIOBOX_SOLVER_TYPE, SP_DLG_CPNColDirSimResults::OnSolverTypeChanged)
EVT_BUTTON(SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_CPNColDirSimResults::OnSimulationProperties)
EVT_BUTTON(SP_ID_BUTTON_SAVE_ODE, SP_DLG_CPNColDirSimResults::SaveODE)
END_EVENT_TABLE()
//

SP_DLG_CPNColDirSimResults::SP_DLG_CPNColDirSimResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	SP_DLG_DirColorSimulation(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle), m_bIsSimulatorInitialized(false), m_nRedraw(true)
{

	int l_nSimulatorIndex =
		LoadSimulatorProperties();

	wxSizer* l_pcRowSizer = NULL;

	if (!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		//function set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking overview:")), 1, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("")), 0, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100, -1)));
		l_pcRowSizer->Add(m_apcComboBoxes[0], 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		UpdateChoices();
		SP_SetString::iterator l_itChoice;
		for (l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
		{
			wxString l_sGroup = *l_itChoice;
			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, l_sGroup + wxT(':')), 1, wxALL | wxEXPAND, 5);
			m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_MARKING_SETS, wxDefaultPosition, wxSize(100, -1), 0, NULL, 0, wxDefaultValidator, l_sGroup));
			l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
			l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_CONSTANT_SETS, wxT("Modify")), 0, wxALL, 5);
			m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
		}
	}

	
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Solver Type")), 1, wxALL | wxEXPAND, 5);
	m_pcSolver = new wxChoice(m_pcPropertyWindowPropertySizer, SP_ID_CHOICE_SOLVER, wxDefaultPosition, wxSize(100, -1));
	m_pcSolver->Clear();

	//decode the solver type index
	int l_nSolverType = l_nSimulatorIndex /*/ 7*/;

	int L_nSolverAlgorithm = l_nSimulatorIndex - l_nSolverType * 7;


	wxString l_asSolverTypeChoices[] = { wxT("BDF(Stiff)"),wxT("ADAMS (Unstiff)") /*, wxT("Euler (Unstiff)")*/ };

	

	if (l_nSolverType == 0) {
		for (int i = 0; i < 2; i++)
			m_pcSolver->Append(l_asSolverTypeChoices[i]);
	}

	//set default solving algorithm
	m_pcSolver->SetSelection(L_nSolverAlgorithm);


	l_pcRowSizer->Add(m_pcSolver, 1, wxALL, 5);

	m_pcSimulationProperites = new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_SIMULATION_PROPERTIES, wxT("Properties"));
	l_pcRowSizer->Add(m_pcSimulationProperites, 0, wxALL
#if wxCHECK_VERSION(2,8,8)
		| wxRESERVE_SPACE_EVEN_IF_HIDDEN
#endif
		, 5);

	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = m_pcDirectExportSizer->GetChildren().back()->GetSizer();
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowDirectExportSizer, SP_ID_BUTTON_SAVE_ODE, wxT("Save ODE")), 1, wxALL, 5);

	// simulation Current time
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSimulationButtonSizer, -1, wxT("Current simulation time: ")), 0, wxALL | wxEXPAND, 5);
	m_pcCurrentSimulationTimeTxtCtrl = new wxStaticText(m_pcPropertyWindowSimulationButtonSizer, -1, wxT("0,0 units"));
	l_pcRowSizer->Add(m_pcCurrentSimulationTimeTxtCtrl, 0, wxALL | wxEXPAND, 5);
	m_pcSimulationButtonSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);

	//At the end call this function for alignment
	SetSizerAndFit(m_pcMainSizer);

	Layout();
}
void SP_DLG_CPNColDirSimResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		m_sNodeclass = SP_DS_CONTINUOUS_TRANS;
	}
	else
	{
		m_sNodeclass = SP_DS_CONTINUOUS_PLACE;
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

void SP_DLG_CPNColDirSimResults::LoadNetInformation()
{

	//LoadPlaces();

	//LoadTransitions();

}

void SP_DLG_CPNColDirSimResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);
	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}

void SP_DLG_CPNColDirSimResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	wxString l_sCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariableName"));

	double l_nStartOutputPoint = m_pcMainSimulator->GetOutputStartPoint();

	p_anXAxisValues.clear();
	if (l_sCategory.IsSameAs(wxT("Time")))
	{
		double l_nTime = 0;

		for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
		{
			p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

			l_nTime += m_pcMainSimulator->GetOutputSampleSize();
		}

	}
	else
		if (l_sCategory.IsSameAs(wxT("Place")) && !l_sSubCategory.IsEmpty())
		{
			unsigned long l_nPlacePosition = m_mPlaceName2Position[l_sSubCategory];

			for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
			{
				double l_nValue = m_pcMainSimulator->GetResultMatrix()[l_nPoint][l_nPlacePosition];

				p_anXAxisValues.push_back(l_nValue);
			}

		}
		else
			if (l_sCategory.IsSameAs(wxT("Transition")) && !l_sSubCategory.IsEmpty())
			{
				unsigned long l_nTransitionPosition = m_mTransitionName2Position[l_sSubCategory];

				for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
				{
					double l_nValue = m_pcMainSimulator->GetRateResultMatrix()[l_nPoint][l_nTransitionPosition];

					p_anXAxisValues.push_back(l_nValue);
				}

			}
			else
			{
				double l_nTime = 0;

				for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
				{
					p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

					l_nTime += m_pcMainSimulator->GetOutputSampleSize();
				}

			}
}

SP_DLG_CPNColDirSimResults::~SP_DLG_CPNColDirSimResults()
{
	SaveSimulatorProperties();

	wxDELETE(m_pcSimulationProperites);
}



void SP_DLG_CPNColDirSimResults::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{
	for (size_t j = 0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		wxChoice* l_pcCombobox = m_apcComboBoxes[j];
		unsigned int l_nCurrentMarkingSet = l_pcCombobox->GetSelection();
		if (l_pcAttr)
		{
			l_pcAttr->SetActiveList(l_nCurrentMarkingSet);
			
		}
	}
	

	
	m_bIsSimulatorInitialized = false;

	
}
//
void SP_DLG_CPNColDirSimResults::OnParameterSetChanged(wxCommandEvent& p_cEvent)
{
	unsigned int l_nCurrentParameterSet = m_pcParameterSetComboBox->GetSelection();
	const SP_ListNode* l_pcParams = m_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itParam = l_pcParams->begin(); l_itParam != l_pcParams->end(); l_itParam++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itParam)->GetAttribute(wxT("ParameterList"))))->SetActiveList(l_nCurrentParameterSet);

	//Reload the parameters
	LoadParameters();

}
//
void SP_DLG_CPNColDirSimResults::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
{
	unsigned int l_nCurrentFunctionSet = m_apcComboBoxes[0]->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveList(l_nCurrentFunctionSet);

	//TODO: load only rate functions to save time
	m_bIsSimulatorInitialized = false;

}
//
void SP_DLG_CPNColDirSimResults::OnSolverTypeChanged(wxCommandEvent& p_cEven)
{
	
	wxString l_asSolverTypeChoices[] = { wxT("BDF(Stiff)"),wxT("ADAMS (Unstiff)") };
	
	
	
			for (int i = 0; i < 2; i++)
				m_pcSolver->Append(l_asSolverTypeChoices[i]);

			//Set default solver algorithm
			m_pcMainSimulator = new spsim::CVODEColContinuous(CV_BDF, CV_NEWTON);
		
		m_pcSolver->SetSelection(0);

		m_bIsSimulatorInitialized = false;

		//add gui properties
		AddGuiOption2Simulator();

	

}
//
void SP_DLG_CPNColDirSimResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_MarkingOverview* l_pcDlg = new SP_DLG_MarkingOverview(this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		//TODO: load only the current active Marking
		//LoadPlaces();

		//by sl
		//LoadConnections();

	}

	l_pcDlg->Destroy();
}

void SP_DLG_CPNColDirSimResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(SP_DS_CONTINUOUS_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		//TODO: load only rate functions to save time
		m_bIsSimulatorInitialized = false;
	}
	l_pcDlg->Destroy();
}

void SP_DLG_CPNColDirSimResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_StParameterOverview l_cDlg(this);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadSets();

		//Reload the parameters
		LoadParameters();
	}

	l_cDlg.Destroy();
}
//=============================================================================================
void SP_DLG_CPNColDirSimResults::LoadSets()
{
	SP_DS_Node* l_pcNode = NULL;

	m_apcColListAttr.clear();

	if (m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	SP_DLG_DirColorSimulation::LoadSets();
}

void SP_DLG_CPNColDirSimResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{
	 

	
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		m_pcMainSimulator->AbortSimulation();
		return;
	}

	//Export Type
	if (m_pcDirectExportRadioButton->GetValue())
		m_ExportType = SP_SIM_DIRECT_DEXPORT;
	else
		m_ExportType = SP_SIM_NO_DEXPORT;

	SetSimulationProgressGauge(0);
	m_pcStartButton->SetLabel(wxT("Abort Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxRED);
	Update();

	if (InitializeSimulator())
	{
		//mark the simulator initialized
		m_bIsSimulatorInitialized = true;

		//start the simulator
		StartSimulation();

		//export final result
		if (m_ExportType == SP_SIM_DIRECT_DEXPORT)
		{
			//open export file
			OpenExportFile();

			//do the export
			DirectExportToCSV();

			//close the export
			CloseExportFile();
		}
	}
	else
	{
		SP_LOGERROR(wxT("The simulation can not be initialized"));
	}

	m_pcStartButton->SetLabel(wxT("Start Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	return;
}

bool SP_DLG_CPNColDirSimResults::InitializeSimulator()
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
		SP_MESSAGEBOX(wxT("The interval end value must be non-negative real number >0. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR);
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

	if (!m_bIsSimulatorInitialized)
	{
		wxBusyInfo l_Info(wxT("Loading data, please wait...."), this);
		//Places
		LoadPlaces();

		//Transitions
		LoadTransitions();

		//Parameters
		LoadParameters();

		//Load Connections
		LoadConnections();
	}

	wxBusyInfo* l_pcInfo = new wxBusyInfo(wxT("Initializing the ODEs solver, please wait...."), this);
	bool l_bIsInitialized = m_pcMainSimulator->Initialise(true);

	wxDELETE(l_pcInfo);

	//Get some properties from the user
	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization")) != NULL)
	{
		m_nRedraw = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("InBetweenVisualization"))->GetValueBool();
	}

	if (m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate")) != NULL)
	{
		m_nRefreshRate = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate"))->GetValuelong();
	}

	SetSimulationProgressGaugeRange(l_nLong0);

	return l_bIsInitialized;
}

void SP_DLG_CPNColDirSimResults::DoStartSimulation()
{

	double l_nCurrentTime = 0;

	m_nLastUpdateTime = 0;

	m_cSimulationStopWatch.Start(0);

	SetSimulationCurrentTime(m_pcMainSimulator->GetCurrentTime());

	SetSimulationStopWatch(0);

	Update();

	while (l_nCurrentTime <= m_pcMainSimulator->GetOutputEndPoint() && m_pcMainSimulator->IsSimulationRunning())
	{
		l_nCurrentTime = m_pcMainSimulator->RunSimulationToNextOutputPoint();

		UpdateSimulationDialog(m_pcMainSimulator->GetGeneratedResultPointsCount());
	}

	//Stop the stopwatch
	m_cSimulationStopWatch.Pause();

	SetSimulationProgressGauge(m_pcMainSimulator->GetOutputPointsCount());

	SetSimulationStopWatch(m_cSimulationStopWatch.Time());

	m_pcMainSimulator->AbortSimulation();

	float l_nSimulationCurrentTime = m_pcMainSimulator->GetCurrentTime() > m_pcMainSimulator->GetOutputEndPoint() ? m_pcMainSimulator->GetOutputEndPoint() : m_pcMainSimulator->GetCurrentTime();

	SetSimulationCurrentTime(l_nSimulationCurrentTime);

	//Load colored or auxiliary variables
	LoadColAuxResults();
	//Load the final result
	LoadResults();
}

void SP_DLG_CPNColDirSimResults::UpdateSimulationDialog(const unsigned long& p_nGeneratedResultPointCount)
{
	bool l_bUpdate = false;
	unsigned long l_nUpdateTime = m_cSimulationStopWatch.Time();
	if ((m_nLastUpdateTime + m_nRefreshRate) < l_nUpdateTime || m_nLastUpdateTime > l_nUpdateTime)
	{
		l_bUpdate = true;

	}
	if (l_bUpdate)
	{
		SetSimulationProgressGauge(p_nGeneratedResultPointCount);
		SetSimulationStopWatch(l_nUpdateTime);

		SetSimulationCurrentTime(m_pcMainSimulator->GetCurrentTime());

		if (m_nRedraw)
		{
			LoadResults();
		}
		m_nLastUpdateTime = l_nUpdateTime;

		CHECK_EVENTS(m_pcMainSimulator->IsSimulationRunning(), return);

		RefreshExternalWindows();
	}
}

void SP_DLG_CPNColDirSimResults::OnSolverChanged(wxCommandEvent& p_cEven)
{
	
	/*if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	int l_nSolverIndex = m_pcSolver->GetSelection();

	//change the ODE simulator
	spsim::ColContinuousSimulator* l_pcContinuousSimulator = dynamic_cast<spsim::ColContinuousSimulator*>(m_pcMainSimulator);

	CHECK_POINTER(l_pcContinuousSimulator, return);

	l_pcContinuousSimulator->SetContinuousSolver((spsim::ContinuousSolver) (l_nSolverIndex));*/

	//Solver Algorithm
	int l_nSolverIndex = m_pcSolver->GetSelection();

	//Stiff/unstiff
	//int l_nSolverType = m_pcSolverType->GetSelection();
	//int l_nSolver = l_nSolverType * 7 + l_nSolverIndex;

	//get a pointer to the old simulator
	spsim::Simulator* l_pcOldSimulator = m_pcMainSimulator;

	//create a solver
	m_pcMainSimulator = CreateSimulator(l_nSolverIndex/*l_nSolver*/);

	AddGuiOption2Simulator();

	//copy old setting
	m_pcMainSimulator->CopySettingFrom(l_pcOldSimulator);

	wxDELETE(l_pcOldSimulator);

	m_bIsSimulatorInitialized = false;

	//tell the viewer the new data source
	UpdateViewer();
	
	
	
}

int SP_DLG_CPNColDirSimResults::GetCureentSelectedSimulator()
{
	//Solver Algorithm
	int l_nSolverAlgorithm = m_pcSolver->GetSelection();

	//Stiff/unstiff
	int l_nSolverType = /*m_pcSolverType*/ m_pcSolver->GetSelection();
	int l_nSolver = l_nSolverType * 7 + l_nSolverAlgorithm;

	return l_nSolver;
}
/*
int SP_DLG_CPNColDirSimResults::LoadSimulatorProperties()
{
	int l_nSimulatorType;
	int l_nODESolverType;

	//call the parent class load properties
	l_nSimulatorType = SP_DLG_DirColorSimulation::LoadSimulatorProperties();

	//Retrieve simulator name
	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	l_pcAttr = l_pcSimProp->GetAttribute(wxT("ODE Solver"));

	CHECK_POINTER(l_pcAttr, return l_nSimulatorType);

	//get the value of the ode solver type
	l_nODESolverType = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttr))->GetValue();

	//change the ODE simulator
	spsim::ColContinuousSimulator* l_pccolcontinuousSimulator = dynamic_cast<spsim::ColContinuousSimulator*>(m_pcMainSimulator);

	CHECK_POINTER(l_pccolcontinuousSimulator, return l_nSimulatorType);

	l_pccolcontinuousSimulator->SetContinuousSolver((spsim::ContinuousSolver) (l_nODESolverType));

	m_nContinuousSimulator = l_nODESolverType;

	return l_nSimulatorType;
}*/
spsim::ColorSimulator* SP_DLG_CPNColDirSimResults::CreateSimulator(const int& p_nSolverType)
{

	//select the user selected solver
	switch (p_nSolverType)
	{
	case 0:
		return new spsim::CVODEColContinuous(CV_BDF, CV_NEWTON);
		break;
	case 1:
	    return new spsim::CVODEColContinuous();
		
		break;
	
	}
	 
	return new spsim::CVODEColContinuous();
	
}


void SP_DLG_CPNColDirSimResults::DirectExportToCSV()
{
	if (!m_pcExport)
		return;

	const SP_Vector2DDouble& l_aanPLResults = m_pcMainSimulator->GetResultMatrix();
	const SP_Vector2DDouble& l_aanTRResults = m_pcMainSimulator->GetRateResultMatrix();
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);
	wxString l_sCurrentRow, l_sOutput;

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return);
	wxString l_sElementType = l_pcAttribute->GetValueString();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return);

	for (unsigned long l_nRow = 0; l_nRow < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nRow++)
	{
		l_sOutput = wxT("");
		l_sOutput << dsszmc::aux::toString(m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow = wxT("");

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
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place"))) //uncolored place
			{
				l_dResult = l_aanPLResults[l_nRow][l_nPos];
			}
			else
				if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //uncolored place
				{
					l_dResult = m_aanAuxPLVarsResults[l_nRow][l_nPos];
				}
				
						else
						{
							l_dResult = 0;
						}

			//write 
			l_sCurrentRow << l_sSpacer;
			l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);
		}

		l_sOutput << l_sCurrentRow;
		*m_pcExport << l_sOutput << wxT("\n");
	}

}

void SP_DLG_CPNColDirSimResults::SaveODE(wxCommandEvent& p_cEvent)
{
	
}

void SP_DLG_CPNColDirSimResults::LoadTransitions()
{

	
	
}

void SP_DLG_CPNColDirSimResults::LoadConnections()
{

}

void SP_DLG_CPNColDirSimResults::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
	
}

wxString SP_DLG_CPNColDirSimResults::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{
	SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
if (!l_pcColList1)

    wxString l_sExpression;
	wxString l_sExpression = l_pcColList1->GetCell(0, 1);
	// search in expression
	stringstream testA(l_sExpression);
	string segment;
	string dash("`");
	wxString l_sArcmult;
	std::size_t foundEx = l_sExpression.find(dash);
	if (foundEx != std::string::npos)
	{

		std::getline(testA, segment, '`');
		l_sArcmult = segment;
		//iss >> segment;
		//stringstream(segment) >> real;
	}
	else
		l_sArcmult = wxT("1");

	return l_sArcmult;
	
	
}

bool SP_DLG_CPNColDirSimResults::IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue)
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
void SP_DLG_CPNColDirSimResults::LoadPlaces()
{
	
}

void SP_DLG_CPNColDirSimResults::UpdateViewer(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	//select the suited matrix to view (rate/marking)
	UpdateSimulationMatrix(p_pcView);
	
}

void SP_DLG_CPNColDirSimResults::UpdateSimulationMatrix(SP_DS_Metadata* p_pcView)
{
	SP_DS_Attribute* l_pcAttribute = nullptr;
	if (p_pcView)
	{
		l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));
	}
	else if (m_pcCurrentTablePlot)
	{
		l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	}
	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		//rate
		m_anResultMatrix = m_pcMainSimulator->GetRateResultMatrix();
	}
	else
	{
		//marking
		m_anResultMatrix = m_pcMainSimulator->GetResultMatrix();
	}

	UpdateXAxisValues();
}

