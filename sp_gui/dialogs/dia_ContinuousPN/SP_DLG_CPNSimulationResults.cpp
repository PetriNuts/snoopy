/*
 * SP_DLG_CPNSimulationResults.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 31.5.2010
 * Short Description:
 * Implementation file for the Continuous result dialog
 */
//===============================================================================================
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
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"

#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
//by sl
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
IMPLEMENT_CLASS(SP_DLG_CPNSimulationResults, SP_DLG_Simulation )

enum
{
	SP_ID_BUTTON_MODIFY_MARKING_SETS = SP_ID_LAST_ID + 100,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,
	SP_ID_COMBOBOX_MARKING_SETS,
	SP_ID_COMBOBOX_FUNCTION_SETS,
	SP_ID_COMBOBOX_PARAMETER_SETS,
	SP_ID_COMBOBOX_SOLVER,
	SP_ID_RADIOBOX_SOLVER_TYPE,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,
	SP_ID_BUTTON_SAVE_ODE
};
BEGIN_EVENT_TABLE( SP_DLG_CPNSimulationResults, SP_DLG_Simulation )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_CPNSimulationResults :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_CPNSimulationResults :: OnModifyFunctionSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_CPNSimulationResults :: OnModifyParameterSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DLG_CPNSimulationResults :: OnModifyConstantSets )

EVT_COMBOBOX( SP_ID_COMBOBOX_MARKING_SETS, SP_DLG_CPNSimulationResults::OnMarkingSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_FUNCTION_SETS, SP_DLG_CPNSimulationResults::OnFunctionSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_PARAMETER_SETS, SP_DLG_CPNSimulationResults::OnParameterSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_SOLVER, SP_DLG_CPNSimulationResults::OnSolverChanged )
EVT_RADIOBOX(SP_ID_RADIOBOX_SOLVER_TYPE,SP_DLG_CPNSimulationResults::OnSolverTypeChanged)
EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_CPNSimulationResults :: OnSimulationProperties )
EVT_BUTTON( SP_ID_BUTTON_SAVE_ODE, SP_DLG_CPNSimulationResults :: SaveODE )
END_EVENT_TABLE()
//

SP_DLG_CPNSimulationResults::SP_DLG_CPNSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
		SP_DLG_Simulation(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle), m_bIsSimulatorInitialized(false), m_nRedraw(true)
{

//	int l_nSimulatorType =
			LoadSimulatorProperties();

	//Call the parent class layout
	SetMinimalLayout();

	wxSizer* l_pcRowSizer = NULL;

	if(!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		//function set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking overview:")), 1, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("")), 0, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Function set:") ), 1, wxALL | wxEXPAND, 5 );
		m_apcComboBoxes.push_back(new wxComboBox(m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_FUNCTION_SETS, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
		l_pcRowSizer->Add( m_apcComboBoxes[0], 1, wxALL, 5 );
		l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify") ), 0, wxALL, 5 );
		m_pcSetsSizer->Add( l_pcRowSizer , 1, wxEXPAND);

		UpdateChoices();
		SP_SetString::iterator l_itChoice;
		for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
		{
			wxString l_sGroup = *l_itChoice;
			l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
			l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, l_sGroup + wxT(':') ), 1, wxALL | wxEXPAND, 5 );
			m_apcComboBoxes.push_back(new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_MARKING_SETS , wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY, wxDefaultValidator, l_sGroup ));
			l_pcRowSizer->Add( m_apcComboBoxes[m_apcComboBoxes.size()-1], 1, wxALL, 5 );
			l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_CONSTANT_SETS, wxT("Modify") ), 0, wxALL, 5 );
			m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND);
		}
	}

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_sChoices[] =
	{ wxT("Stiff"), wxT("Unstiff") };
	wxString l_asStiffSolverChoices[] =
	{ wxT("BDF"), wxT("Rosenbrock-Method of Shampine"), wxT("Rosenbrock-Method GRK4T of Kaps-Rentrop"), wxT("Rosenbrock-Method GRK4A of Kaps-Rentrop"),
			wxT("Rosenbrock-Method of Van Veldhuizen [gamma = 1/2]"), wxT("Rosenbrock-Method of Van Veldhuizen [D-stable]"), wxT("an L-stable Rosenbrock-Method") };
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Solver Type")), 1, wxALL | wxEXPAND, 5);
	m_pcSolverType = new wxRadioBox(m_pcPropertyWindowPropertySizer, SP_ID_RADIOBOX_SOLVER_TYPE, wxT(""), wxDefaultPosition, wxDefaultSize, 2, l_sChoices, 2, wxRA_SPECIFY_COLS);
	l_pcRowSizer->Add(m_pcSolverType);
	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);
	//
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Simulator")), 1, wxALL | wxEXPAND, 5);
	m_pcSolver = new wxComboBox(m_pcPropertyWindowPropertySizer, SP_ID_COMBOBOX_SOLVER, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);
	m_pcSolver->Clear();
	for (int i = 0; i < 7; i++)
		m_pcSolver->Append(l_asStiffSolverChoices[i]);

	//set default solving algorithm
	m_pcSolver->SetSelection(0);

	//Set default solver type
	m_pcSolverType->SetSelection(0);
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
void SP_DLG_CPNSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
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
	LoadNodeColours(m_sNodeclass,l_asColours);

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

		if(l_asColours[l_nPosition]==wxT(""))
		{
			l_pcCurveInfoList->SetCell(l_nNewRow,3,GetColourString(l_nPosition));
		}
		else
		{
			l_pcCurveInfoList->SetCell(l_nNewRow,3,l_asColours[l_nPosition]);
		}

		l_pcCurveInfoList->SetCell(l_nNewRow, 4, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 5, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 6, l_sName);

		l_nPosition++;
	}
}

void SP_DLG_CPNSimulationResults::LoadNetInformation()
{

	LoadPlaces();

	LoadTransitions();

}

void SP_DLG_CPNSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);
	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}

void SP_DLG_CPNSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
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

SP_DLG_CPNSimulationResults::~SP_DLG_CPNSimulationResults()
{
	SaveSimulatorProperties();

	wxDELETE(m_pcSimulationProperites);
}



void SP_DLG_CPNSimulationResults::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{
	for(size_t j = 0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		wxComboBox* l_pcCombobox = m_apcComboBoxes[j];
		unsigned int l_nCurrentMarkingSet = l_pcCombobox->GetSelection();
		if(l_pcAttr)
		{
			l_pcAttr->SetActiveList(l_nCurrentMarkingSet);
			//l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
		}
	}


	/*
	//Get the Places Nodes
	unsigned int l_nCurrentMarkingSet = m_pcMarkingSetComboBox->GetSelection();
	const SP_ListNode* l_pcPlaces = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itPlace = l_pcPlaces->begin(); l_itPlace != l_pcPlaces->end(); l_itPlace++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itPlace)->GetAttribute(wxT("MarkingList"))))->SetActiveList(l_nCurrentMarkingSet);
	*/
	m_bIsSimulatorInitialized = false;

	//TODO: load only the current active Marking
	//LoadPlaces();

	//by sl
	//LoadConnections();
}
//
void SP_DLG_CPNSimulationResults::OnParameterSetChanged(wxCommandEvent& p_cEvent)
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
void SP_DLG_CPNSimulationResults::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
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
void SP_DLG_CPNSimulationResults::OnSolverTypeChanged(wxCommandEvent& p_cEven)
{
	wxString l_asUnstiffSolverChoices[] =
	{ wxT("ADAMS"), wxT("Euler (fixed step size)"), wxT("Modificated Euler (fixed step size)"), wxT("Classic Runge-Kutta (fixed step size)"), wxT("Kuntzmann 4th order (fixed step size)"),
			wxT("Runge-Kutta-Fehlberg-4(5) (dynamic step size)"), wxT("Dormand-Prince-5(4) (dynamic step size)") };
	wxString l_asStiffSolverChoices[] =
	{ wxT("BDF"), wxT("Rosenbrock-Method of Shampine"), wxT("Rosenbrock-Method GRK4T of Kaps-Rentrop"), wxT("Rosenbrock-Method GRK4A of Kaps-Rentrop"),
			wxT("Rosenbrock-Method of Van Veldhuizen [gamma = 1/2]"), wxT("Rosenbrock-Method of Van Veldhuizen [D-stable]"), wxT("an L-stable Rosenbrock-Method") };
	if (p_cEven.GetEventType() == wxEVT_COMMAND_RADIOBOX_SELECTED)
	{
		m_pcSolver->Clear();
		if (m_pcSolverType->GetSelection() == 0)
		{
			for (int i = 0; i < 7; i++)
				m_pcSolver->Append(l_asStiffSolverChoices[i]);

			//Set default solver algorithm
			m_pcMainSimulator = new spsim::CVODEContinuous(CV_BDF, CV_NEWTON);
		}
		else
		{
			for (int i = 0; i < 7; i++)
			{
				m_pcSolver->Append(l_asUnstiffSolverChoices[i]);
			}

			//Set default solver algorithm
			m_pcMainSimulator = new spsim::CVODEContinuous();
		}

		//Select the first algorithm of the selected group
		m_pcSolver->SetSelection(0);

		m_bIsSimulatorInitialized = false;

		//add gui properties
		AddGuiOption2Simulator();

	}

}
//
void SP_DLG_CPNSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_MarkingOverview* l_pcDlg = new SP_DLG_MarkingOverview(this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		//TODO: load only the current active Marking
		LoadPlaces();

		//by sl
		LoadConnections();

	}

	l_pcDlg->Destroy();
}

void SP_DLG_CPNSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
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

void SP_DLG_CPNSimulationResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
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
void SP_DLG_CPNSimulationResults::LoadSets()
{
	SP_DS_Node* l_pcNode = NULL;

	m_apcColListAttr.clear();

	if(m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}

	SP_DLG_Simulation::LoadSets();
}

void SP_DLG_CPNSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
{

	if (m_pcMainSimulator->IsSimulationRunning())
	{
		m_pcMainSimulator->AbortSimulation();
		return;
	}

	//Export Type
	if (m_pcDirectExportRadioButton->GetValue())
		m_ExportType = SP_SIM_DIRECT_EXPORT;
	else
		m_ExportType = SP_SIM_NO_EXPORT;

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
		if (m_ExportType == SP_SIM_DIRECT_EXPORT)
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
		SP_LOGERROR( wxT("The simulation can not be initialized"));
	}

	m_pcStartButton->SetLabel(wxT("Start Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	return;
}

bool SP_DLG_CPNSimulationResults::InitializeSimulator()
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

void SP_DLG_CPNSimulationResults::DoStartSimulation()
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

void SP_DLG_CPNSimulationResults::UpdateSimulationDialog(const unsigned long& p_nGeneratedResultPointCount)
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

void SP_DLG_CPNSimulationResults::OnSolverChanged(wxCommandEvent& p_cEven)
{
	//Solver Algorithm
	int l_nSolverIndex = m_pcSolver->GetSelection();

	//Stiff/unstiff
	int l_nSolverType = m_pcSolverType->GetSelection();
	int l_nSolver = l_nSolverType * 7 + l_nSolverIndex;

	//get a pointer to the old simulator
	spsim::Simulator* l_pcOldSimulator = m_pcMainSimulator;

	//create a solver
	m_pcMainSimulator = CreateSimulator(l_nSolver);

	AddGuiOption2Simulator();

	//copy old setting
	m_pcMainSimulator->CopySettingFrom(l_pcOldSimulator);

	wxDELETE(l_pcOldSimulator);

	m_bIsSimulatorInitialized = false;

	//tell the viewer the new data source
	UpdateViewer();
}

int SP_DLG_CPNSimulationResults::GetCureentSelectedSimulator()
{
	//Solver Algorithm
	/*int l_nSolverIndex=m_pcSolver->GetSelection();

	 //Stiff/unstiff
	 int l_nSolverType=m_pcSolverType->GetSelection();
	 int l_nSolver=l_nSolverType*7+l_nSolverIndex;*/

	return 0;
}

spsim::Simulator* SP_DLG_CPNSimulationResults::CreateSimulator(const int& p_nSimulatorType)
{

	//select the user selected solver
	switch (p_nSimulatorType)
	{
	case 0:
		return new spsim::CVODEContinuous(CV_BDF, CV_NEWTON);
		break;
	case 1:
		return new spsim::RosenBrock(1);
		break;
	case 2:
		return new spsim::RosenBrock(2);
		break;
	case 3:
		return new spsim::RosenBrock(3);
		break;
	case 4:
		return new spsim::RosenBrock(4);
		break;
	case 5:
		return new spsim::RosenBrock(5);
		break;
	case 6:
		return new spsim::RosenBrock(6);
		break;
	case 7:
		return new spsim::CVODEContinuous();
		break;
	case 8:
		return new spsim::Euler();
		break;
	case 9:
		return new spsim::ModifiedEuler();
		break;
	case 10:
		return new spsim::ClassicRungeKutta();
		break;
	case 11:
		return new spsim::SolverKuntzmann4();
		break;
	case 12:
		return new spsim::CVODEContinuous();
		//TODO: RungeKuttaFehlberg4_5 needs to be adjusted to the new code
		//return new spsim::RungeKuttaFehlberg4_5();
		break;
	case 13:
		return new spsim::CVODEContinuous();
		//TODO: DormandPrince5_4 needs to be adjusted to the new code
		//return new spsim::DormandPrince5_4();
		break;
	default:
		return new spsim::Euler();
		break;
	}

	return new spsim::Euler();
}

void SP_DLG_CPNSimulationResults::DirectExportToCSV()
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
		l_sOutput << wxString::Format(wxT("%g"), m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
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
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
			{
				l_dResult = l_aanPLResults[l_nRow][l_nPos];
			}
			else
			if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
			{
				l_dResult = m_aanAuxPLVarsResults[l_nRow][l_nPos];
			}
			else
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
			{
				l_dResult = l_aanTRResults[l_nRow][l_nPos];
			}
			else
			if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
			{
				l_dResult = m_aanAuxTRVarsResults[l_nRow][l_nPos];
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

void SP_DLG_CPNSimulationResults::SaveODE(wxCommandEvent& p_cEvent)
{
	wxFFile l_File;
	wxFileName fn = wxFileName(wxT("parsed-ode.txt"));
	wxString l_sOutName;
	wxString l_sSelName;
	wxString l_sSelDir;
	wxString l_sFileExt;
	wxString l_sFileExtFilter;
	bool l_bSelAgain;
	l_sSelName = fn.GetName() + wxT(".txt");
	l_sFileExt = wxT("txt");
	l_sFileExtFilter = wxT("Text file (*.txt)|*.txt");
	l_sSelDir = wxT("");
	do
	{
		l_bSelAgain = false;
		l_sOutName = wxFileSelector(wxT("Simulation result file"), l_sSelDir, l_sSelName, l_sFileExt, l_sFileExtFilter, wxFD_SAVE);
		if (l_sOutName.empty())
		{
			break;
		}
		if (wxFileName::FileExists(l_sOutName))
		{
			int l_nAnswer = SP_MESSAGEBOX(wxT("File ") + l_sOutName + wxT(" exists.")
			wxT(" Overwrite?"), wxT("Overwrite?"), wxYES_NO | wxICON_QUESTION);
			if (l_nAnswer == wxNO)
			{
				l_bSelAgain = true;
				wxFileName l_tmpFN = wxFileName(l_sOutName);
				l_sSelDir = l_tmpFN.GetPath();
				l_sSelName = l_tmpFN.GetFullName();
			}
		}
	}
	while (l_bSelAgain);

	if (!InitializeSimulator())
	{
		SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODE"));
		return;
	}

	wxString l_sResult;
	//Iterate for all the places
	for (unsigned long l_nPlacePos = 0; l_nPlacePos < m_asPlaceNames.size(); l_nPlacePos++)
	{
		wxString l_sName = m_asPlaceNames[l_nPlacePos];
		l_sResult << wxT("d")<<l_sName << wxT("/dt = ");
		l_sResult << (dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->GetPlaceODEString(l_nPlacePos) << wxT("\n\n");
	}

	//after the simulator is initialized, it will automatically set running. Therefore we need to stop it here
	m_pcMainSimulator->AbortSimulation();

	if (l_sOutName.empty())
		return;
	if (!l_File.Open(l_sOutName.c_str(), wxT("wt")))
		return;
	l_File.Write(l_sResult);
	if (l_File.Close())
		return;
}

void SP_DLG_CPNSimulationResults::LoadTransitions()
{

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;

	m_pcMainSimulator->SetTransitionCount(l_pcNodeclass->GetElements()->size());

	unsigned long l_nPosition = 0;

	//clear old transitions
	m_pcMainSimulator->ClearTransitions();

	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		//add a transition
		m_pcMainSimulator->AddTransition(l_sName, l_pcColList->GetActiveCellValue(1), spsim::TRANSITION_TYPE_CONTINUOUS);

		m_asTransitionNames.push_back(l_sName);
		m_mTransitionName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}

}

void SP_DLG_CPNSimulationResults::LoadConnections()
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

	//Modifier
	LoadConnectionOfType(SP_DS_MODIFIER_EDGE, spsim::MODIFIER_ARC);
}

void SP_DLG_CPNSimulationResults::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
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
		wxString l_sArcWeight=GetEdgeWeight((*l_itEdge));
		//converted arc weight as a number
		double l_nNumericArcWeight=0;

		//Get the arc Weight
		if(l_pcSourceNode->GetClassName().Contains(wxT("Transition"))==true)
		{
			if (IsConstantArcWeight(l_sArcWeight,l_nNumericArcWeight)==true)
			{
				m_pcMainSimulator->SetPostTransitionConnection(l_sSourceNodeName,l_sDestNodeName,l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPostSelfModifyingWeights(l_sSourceNodeName,l_sDestNodeName,l_sArcWeight);
			}

		}
		else
		{
			if (IsConstantArcWeight(l_sArcWeight,l_nNumericArcWeight)==true)
			{
				m_pcMainSimulator->SetPreTransitionConnection(l_sDestNodeName,l_sSourceNodeName,p_ArcType,l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPreSelfModifyingWeights(l_sDestNodeName,l_sSourceNodeName,p_ArcType,l_sArcWeight);
			}

		}
	}
}

wxString SP_DLG_CPNSimulationResults::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{
	wxString l_sWeight;

	//Get the Edge weight
	SP_DS_Attribute* l_pcAtt = p_pcEdge->GetAttribute(wxT("Multiplicity"));

	if(l_pcAtt==NULL) //e.g., modifier arc has no Multiplicity
	{
		return wxT("0.0");
	}


	l_sWeight = l_pcAtt->GetValueString();

	return l_sWeight;
}

bool SP_DLG_CPNSimulationResults::IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue)
{
 SP_MapString2Double::iterator l_itConstant;

	  //explicit constant value
	  if(p_sWeight.ToDouble(&p_nReturnValue))
	  {
		  return true; //constant
	  }

	  //check constant list
	  if((l_itConstant=m_msParameterName2Value.find(p_sWeight))!=m_msParameterName2Value.end())
	  {
		  p_nReturnValue=(*l_itConstant).second;

		  return true; //constant
	  }

	  return false; //not a constant
}
void SP_DLG_CPNSimulationResults::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	SP_VectorDouble l_anCurrentMarking;
	SP_VectorString l_asPlaceNames;
	unsigned long l_nPlaceCount;

	SP_ListNode::const_iterator l_itElem;
	SP_VectorDouble::iterator l_itCurrentMarking;
	SP_VectorString::iterator l_itPlaceName;

	//SP_DS_ColListAttribute* l_pcColList;
	SP_DS_DoubleMarkingAttribute* l_pcMarking;

	l_nPlaceCount = l_pcNodeclass->GetElements()->size();

	m_pcMainSimulator->SetPlaceCount(l_nPlaceCount);

	l_anCurrentMarking.assign(l_nPlaceCount, 0);

	l_asPlaceNames.assign(l_nPlaceCount, wxT(""));

	l_itCurrentMarking = l_anCurrentMarking.begin();

	l_itPlaceName = l_asPlaceNames.begin();

	m_asPlaceNames.clear();

	unsigned long l_nPosition = 0;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		(*l_itPlaceName++) = l_sName;

		l_pcMarking = dynamic_cast<SP_DS_DoubleMarkingAttribute*>((*l_itElem)->GetAttribute(wxT("Marking")));
		(*l_itCurrentMarking++) = l_pcMarking->GetValue(true);

		m_asPlaceNames.push_back(l_sName);
		m_mPlaceName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}

	m_pcMainSimulator->SetPlaceNames(l_asPlaceNames);

	(dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->SetInitialMarking(l_anCurrentMarking);
}

void SP_DLG_CPNSimulationResults::UpdateViewer()
{
	CHECK_POINTER(m_pcMainSimulator, return);

	//select the suited matrix to view (rate/marking)
	UpdateSimulationMatrix();

	m_apcResultViewers[m_nCurrentViewer]->SetXAxisValues(&m_anXAxisValues);

	m_apcResultViewers[m_nCurrentViewer]->Update();
}

void SP_DLG_CPNSimulationResults::UpdateSimulationMatrix()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);
	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
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

