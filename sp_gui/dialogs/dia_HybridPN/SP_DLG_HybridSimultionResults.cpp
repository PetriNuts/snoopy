/*
 * SP_DLG_HybridSimulationResults.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 2.8.2010
 * Short Description:
 */
//==============================================================================
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <math.h>
#include <wx/ffile.h>
#include <wx/busyinfo.h>
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"

#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"

#include "sp_ds/extensions/HybridPN/SP_DS_SimulatorThreadHybrid.h"

#include "spsim/spsim.h"
#include "spsim/helpers/property.h"
#include "spsim/helpers/simulationOptions.h"

IMPLEMENT_CLASS(SP_DLG_HybridSimulationResults, SP_DLG_Simulation)

enum
{
	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
	SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,
	SP_ID_CHOICE_MARKING_SETS,
	SP_ID_CHOICE_FUNCTION_SETS,
	SP_ID_CHOICE_PARAMETER_SETS,
	SP_ID_CHOICE_TIME_SYNC,
	SP_ID_CHOICE_DELAY_SETS,
	SP_ID_CHOICE_IMMEDIATE_SETS,
	SP_ID_CHOICE_SCHEDULED_SETS,
	SP_ID_CHOICE_STOCHASTIC_RATE_SETS,
	SP_ID_CHOICE_SOLVER,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,
	SP_ID_BUTTON_SAVE_ODE,
	SP_ID_CHOICE_STOCHASTIC_SIMULATOR,
	SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES,
	SP_ID_BUTTON_MODIFY_DELAY_SETS,
	SP_ID_BUTTON_MODIFY_IMMDIATE_SETS,
	SP_ID_BUTTON_MODIFY_SCHEDULED_SETS

};
BEGIN_EVENT_TABLE( SP_DLG_HybridSimulationResults, SP_DLG_Simulation )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_HybridSimulationResults :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_HybridSimulationResults :: OnModifyFunctionSets )
//EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_HybridSimulationResults :: OnModifyParameterSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, SP_DLG_HybridSimulationResults :: OnModifyStochasicSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DLG_HybridSimulationResults :: OnModifyDelaySets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, SP_DLG_HybridSimulationResults :: OnModifyImmediateSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, SP_DLG_HybridSimulationResults :: OnModifyScheduledSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DLG_HybridSimulationResults :: OnModifyConstantSets )

EVT_CHOICE( SP_ID_CHOICE_MARKING_SETS, SP_DLG_HybridSimulationResults::OnMarkingSetChanged )
EVT_CHOICE( SP_ID_CHOICE_FUNCTION_SETS, SP_DLG_HybridSimulationResults::OnFunctionSetChanged )
EVT_CHOICE( SP_ID_CHOICE_STOCHASTIC_RATE_SETS, SP_DLG_HybridSimulationResults::OnStochasticRateChanged )
EVT_CHOICE( SP_ID_CHOICE_IMMEDIATE_SETS, SP_DLG_HybridSimulationResults::OnImmediateSetChanged )
EVT_CHOICE( SP_ID_CHOICE_DELAY_SETS, SP_DLG_HybridSimulationResults::OnDelaySetChanged )
EVT_CHOICE( SP_ID_CHOICE_SCHEDULED_SETS, SP_DLG_HybridSimulationResults::OnScheduledSetChanged )

//EVT_CHOICE( SP_ID_CHOICE_PARAMETER_SETS, SP_DLG_HybridSimulationResults::OnParameterSetChanged )
//EVT_CHOICE( SP_ID_CHOICE_DELAY_SETS, SP_DLG_HybridSimulationResults::OnParameterSetChanged )
EVT_CHOICE( SP_ID_CHOICE_SOLVER, SP_DLG_HybridSimulationResults::OnSolverChanged )
EVT_CHOICE( SP_ID_CHOICE_TIME_SYNC, SP_DLG_HybridSimulationResults::OnTimeSyncTypeChanged )
EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_HybridSimulationResults :: OnSimulationProperties )
EVT_BUTTON( SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES, SP_DLG_HybridSimulationResults :: OnODESolverProperties )
EVT_BUTTON( SP_ID_BUTTON_SAVE_ODE, SP_DLG_HybridSimulationResults :: SaveODE )

EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT,SP_DLG_HybridSimulationResults::OnSimulatorThreadEvent)

END_EVENT_TABLE()
//

SP_DLG_HybridSimulationResults::SP_DLG_HybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
		SP_DLG_Simulation(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle),
		m_bIsSimulatorInitialized(false),
		m_nRedraw(false),
		m_pcWorkerThread(NULL),
		m_sSimulatorType(wxT("Static (exact)")),
		m_nCurrentODESimulator(2)
{

	int l_nSimulatorType = LoadSimulatorProperties();

	//Add special UI element for Continuous class
	wxSizer* l_pcRowSizer = NULL;

	if (!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		//Function set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking overview:")), 1, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("")), 0, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Continuous rate:")), 1, wxALL | wxEXPAND, 5);
		m_pcFunctionSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcFunctionSetComboBox, 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		//Rate set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Stochastic  rate:")), 1, wxALL | wxEXPAND, 5);
		m_pcStochasticRateSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_STOCHASTIC_RATE_SETS, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcStochasticRateSetComboBox, 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		//immediate Set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Immediate set:")), 1, wxALL | wxEXPAND, 5);
		m_pcImmediateSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_IMMEDIATE_SETS, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcImmediateSetComboBox, 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		//delay Set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Delay set:")), 1, wxALL | wxEXPAND, 5);
		m_pcDelaySetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_DELAY_SETS, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcDelaySetComboBox, 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), 0, wxALL, 5);
		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		//scheduled Set
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Scheduled set:")), 1, wxALL | wxEXPAND, 5);
		m_pcScheduledSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_SCHEDULED_SETS, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcScheduledSetComboBox, 1, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, wxT("Modify")), 0, wxALL, 5);
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

	//Continuous Solver
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Solver Type")), 1, wxALL | wxEXPAND, 5);
	m_pcContinuousSolver = new wxChoice(m_pcPropertyWindowPropertySizer, SP_ID_CHOICE_SOLVER, wxDefaultPosition, wxSize(100, -1));
	m_pcContinuousSolver->Clear();

	//Add the ODE Solver Types
	m_pcContinuousSolver->Append(wxT("ARK"));
	m_pcContinuousSolver->Append(wxT("BDF"));
	m_pcContinuousSolver->Append(wxT("ADAMS"));

	//set default solving algorithm
	m_pcContinuousSolver->SetSelection(m_nCurrentODESimulator);
	l_pcRowSizer->Add(m_pcContinuousSolver, 1, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES, wxT("Properties")), 0, wxALL, 5);

	//Solver Properties
	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	//Time synchronization
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("Time Synch.:")), 1, wxALL | wxEXPAND, 5);
	m_pcTimeSyncComboBox = new wxChoice(m_pcPropertyWindowPropertySizer, SP_ID_CHOICE_TIME_SYNC, wxDefaultPosition, wxSize(100, -1));
	m_pcTimeSyncComboBox->Append(wxT("Static (exact)"));
	m_pcTimeSyncComboBox->Append(wxT("Static (accelerated)"));
	m_pcTimeSyncComboBox->Append(wxT("Dynamic"));
	m_pcTimeSyncComboBox->Append(wxT("Approximate"));
	m_pcTimeSyncComboBox->Append(wxT("Continuous"));
	m_pcTimeSyncComboBox->Append(wxT("Stochastic"));
	m_pcTimeSyncComboBox->SetSelection(l_nSimulatorType);
	l_pcRowSizer->Add(m_pcTimeSyncComboBox, 1, wxALL, 5);

	m_pcSimulationProperites = new wxButton(m_pcPropertyWindowPropertySizer, SP_ID_BUTTON_SIMULATION_PROPERTIES, wxT("Properties"));
	l_pcRowSizer->Add(m_pcSimulationProperites, 0, wxALL
#if wxVERSION_NUMBER >= 2808
			| wxRESERVE_SPACE_EVEN_IF_HIDDEN
#endif
			, 5);

	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = m_pcDirectExportSizer->GetChildren().back()->GetSizer();
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowDirectExportSizer, SP_ID_BUTTON_SAVE_ODE, wxT("Save ODE")), 1, wxALL, 5);

	//At the end call this function for alignment
	SetSizerAndFit(m_pcMainSizer);

	//Create a new worker thread
	m_pcWorkerThread = new SP_DS_SimulatorThreadHybrid(this, m_pcMainSimulator);
}

void SP_DLG_HybridSimulationResults::LoadNetInformation()
{
	unsigned long l_nPosition = 0;

	m_asPlaceNames.clear();
	m_mPlaceName2Position.clear();

	LoadPlacesOfType(SP_DS_CONTINUOUS_PLACE, l_nPosition);
	LoadPlacesOfType(SP_DS_DISCRETE_PLACE, l_nPosition);

	//load transition information
	LoadTransitions();
}

void SP_DLG_HybridSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}

void SP_DLG_HybridSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	wxString l_sCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariableName"));

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

					m_anXAxisValues.push_back(l_nValue);
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

void SP_DLG_HybridSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	//position of the element in the result matrix
	unsigned long l_nPosition = 0;

	m_asPlaceNames.clear();
	m_mPlaceName2Position.clear();

	LoadPlacesOfType(SP_DS_CONTINUOUS_PLACE, l_nPosition);
	LoadPlacesOfType(SP_DS_DISCRETE_PLACE, l_nPosition);

	SP_VectorString l_asColours;

	l_asColours.clear();

	//load continuous  place colour
	LoadNodeColours(SP_DS_CONTINUOUS_PLACE, l_asColours);

	//load continuous  place colour
	LoadNodeColours(SP_DS_DISCRETE_PLACE, l_asColours);

	for (unsigned long l_nPlace = 0; l_nPlace < m_asPlaceNames.size(); l_nPlace++)
	{
		unsigned int l_nNewRow = l_pcCurveInfoList->AppendEmptyRow();

		l_pcCurveInfoList->SetCell(l_nNewRow, 0, wxString::Format(wxT("%ld"), l_nPlace));
		l_pcCurveInfoList->SetCell(l_nNewRow, 1, wxT("Unfolded"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 2, wxT("0"));

		if (l_asColours[l_nPlace] == wxT(""))
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, GetColourString(l_nPlace));
		}
		else
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, l_asColours[l_nPlace]);
		}

		l_pcCurveInfoList->SetCell(l_nNewRow, 4, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 5, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 6, m_asPlaceNames[l_nPlace]);
	}
}

SP_DLG_HybridSimulationResults::~SP_DLG_HybridSimulationResults()
{
	SaveSimulatorProperties();

	wxDELETE(m_pcWorkerThread);
}
void SP_DLG_HybridSimulationResults::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	for (size_t j = 0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		wxChoice* l_pcCombobox = m_apcComboBoxes[j];
		unsigned int l_nCurrentMarkingSet = l_pcCombobox->GetSelection();
		if (l_pcAttr)
		{
			l_pcAttr->SetActiveList(l_nCurrentMarkingSet);
			l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
		}
	}

   m_bIsSimulatorInitialized = false;
}
//


void SP_DLG_HybridSimulationResults::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcFunctionSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveList(l_nCurrentFunctionSet);

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnStochasticRateChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentStochaticSet = m_pcStochasticRateSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveList(l_nCurrentStochaticSet);

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnImmediateSetChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcImmediateSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveList(l_nCurrentFunctionSet);

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnDelaySetChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcDelaySetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("DelayList"))))->SetActiveList(l_nCurrentFunctionSet);

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnScheduledSetChanged(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcScheduledSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("PeriodicList"))))->SetActiveList(l_nCurrentFunctionSet);

	m_bIsSimulatorInitialized = false;
}

//
void SP_DLG_HybridSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_MarkingOverview* l_pcDlg = new SP_DLG_MarkingOverview(this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(SP_DS_CONTINUOUS_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}
void SP_DLG_HybridSimulationResults::OnModifyStochasicSets(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(SP_DS_STOCHASTIC_TRANS, this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnModifyImmediateSets(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(SP_DS_IMMEDIATE_TRANS, this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnModifyDelaySets(wxCommandEvent& p_cEvent)
{

	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}

void SP_DLG_HybridSimulationResults::OnModifyScheduledSets(wxCommandEvent& p_cEvent)
{

	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	l_pcDlg->Destroy();

	m_bIsSimulatorInitialized = false;
}

//=============================================================================================
void SP_DLG_HybridSimulationResults::LoadSets()
{
	m_pcFunctionSetComboBox->Clear();
	m_pcFunctionSetComboBox->Clear();
	m_pcStochasticRateSetComboBox->Clear();
	m_pcImmediateSetComboBox->Clear();
	m_pcDelaySetComboBox->Clear();
	m_pcScheduledSetComboBox->Clear();
	const SP_ListNode *l_pcNodesList;
	SP_DS_Node *l_pcFstNode;
	SP_DS_Attribute *l_pcAttr;
	SP_DS_ColListAttribute* l_pcColListAttr;

	//load function set
	l_pcNodesList = (m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
			m_pcFunctionSetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		//select the first function
		m_pcFunctionSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//Load stochastic rate set
	m_pcStochasticRateSetComboBox->Clear();
	l_pcNodesList = (m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
			m_pcStochasticRateSetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		//select the first function
		m_pcStochasticRateSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//Load immediate rate set
	m_pcImmediateSetComboBox->Clear();
	l_pcNodesList = (m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
			m_pcImmediateSetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		//select the first function
		m_pcImmediateSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//Load Delay set
	m_pcDelaySetComboBox->Clear();
	l_pcNodesList = (m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("DelayList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
			m_pcDelaySetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		//select the first function
		m_pcDelaySetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//Load scheduled set
	m_pcDelaySetComboBox->Clear();
	l_pcNodesList = (m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("PeriodicList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
			m_pcDelaySetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		//select the first function
		m_pcDelaySetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//Load all other sets
	m_apcColListAttr.clear();
	SP_DLG_Simulation::LoadSets();
}

void SP_DLG_HybridSimulationResults::OnStartAbortSimulation(wxCommandEvent& p_cEvent)
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

	if (InitializeSimulator())
	{
		//mark the simulator initialized
		m_bIsSimulatorInitialized = true;

		SetSimulationProgressGauge(0);

		m_pcStartButton->SetLabel(wxT("Abort Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxRED);

		Update();

		//start the simulator
		StartSimulation();
	}
	else
	{
		SP_LOGERROR(wxT("The simulation can not be initialized"));
	}

	return;
}

void SP_DLG_HybridSimulationResults::DoStartSimulation()
{
	unsigned long l_nRunCount = 0;

	m_nLastUpdateTime = 0;

	SetSimulationStopWatch(0);

	Update();

	//Get the simulator run count
	CHECK_POINTER( m_pcMainSimulator->GetSimulatorOptions(), return);

	CHECK_POINTER( m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount")), return);

	l_nRunCount = m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"))->GetValuelong();

	if (m_pcWorkerThread != NULL)
	{
		//Create a new worker thread
		m_pcWorkerThread = new SP_DS_SimulatorThreadHybrid(this, m_pcMainSimulator);
	}

	//Check the thread
	CHECK_POINTER(m_pcWorkerThread, return);

	m_pcWorkerThread->SetRunCount(l_nRunCount);

	//Create the worker thread for this model
	if (m_pcWorkerThread->Create() != wxTHREAD_NO_ERROR)
	{
		SP_LOGERROR(wxT("Can not start the simulation"));
		return;
	}

	//simulation threads have the highest priority
	m_pcWorkerThread->SetPriority(WXTHREAD_MAX_PRIORITY);

	//Run the simulation
	if (m_pcWorkerThread->Run() != wxTHREAD_NO_ERROR)
	{
		SP_LOGERROR(wxT("Can not start the simulation"));
		return;
	}

	CHECK_POINTER(m_pcTimer, return);

	//start the timer
	m_pcTimer->Start(m_nRefreshRate);
}

spsim::Simulator* SP_DLG_HybridSimulationResults::CreateSimulator(const int& p_nSimulatorType)
{
	switch (p_nSimulatorType)
	{
	case 0: //static (exact)
		m_sSimulatorType = wxT("Static (exact)");
		return new spsim::HybridStatic();
	case 1: //static (exact)
		m_sSimulatorType = wxT("Static (accelerated)");
		return new spsim::HybridAccelerated();
	case 2: //dynamic
		m_sSimulatorType = wxT("Dynamic");
		return new spsim::HybridDynamic();
	case 3: //approximate
		m_sSimulatorType = wxT("Approximate");
		return new spsim::HybridApproximate();
	case 4: //continuous
		m_sSimulatorType = wxT("Continuous");
		return new spsim::HybridStatic();
	case 5: //stochastic
		m_sSimulatorType = wxT("Stochastic");
		return new spsim::Gillespie();
	}

	m_sSimulatorType = wxT("Static");

	return new spsim::HybridStatic();
}

void SP_DLG_HybridSimulationResults::OnTimeSyncTypeChanged(wxCommandEvent& p_cEven)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	int l_nMethodType = m_pcTimeSyncComboBox->GetSelection();

	//Store properties of the old one
	spsim::Simulator* l_pcNewSimulator = CreateSimulator(l_nMethodType);

	m_bIsSimulatorInitialized = false;

	//copy old setting
	l_pcNewSimulator->CopySettingFrom(m_pcMainSimulator);

	wxDELETE(m_pcMainSimulator);
	m_pcMainSimulator = l_pcNewSimulator;

	m_pcWorkerThread->SetSimulator(m_pcMainSimulator);

	AddGuiOption2Simulator();

	//change the current ODE solver
	int l_nSolverIndex = m_pcContinuousSolver->GetSelection();

	//change the ODE simulator
	spsim::HybridSimulator* l_pcHybridSimulator = dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator);

	CHECK_POINTER(l_pcHybridSimulator, return);

	l_pcHybridSimulator->SetODESolver((spsim::ODESolverType) (l_nSolverIndex));
}

void SP_DLG_HybridSimulationResults::ChangeODESolver()
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	int l_nSolverIndex = m_pcContinuousSolver->GetSelection();

	//change the ODE simulator
	spsim::HybridSimulator* l_pcHybridSimulator = dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator);

	CHECK_POINTER(l_pcHybridSimulator, return);

	l_pcHybridSimulator->SetODESolver((spsim::ODESolverType) (l_nSolverIndex));

}
void SP_DLG_HybridSimulationResults::OnSolverChanged(wxCommandEvent& p_cEven)
{
	ChangeODESolver();
}
void SP_DLG_HybridSimulationResults::DirectExportToCSV()
{
	wxString l_sCurrentRow, l_sOutput;

	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	if (!m_pcExport)
	{
		return;
	}

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return);

	for (unsigned long i = 0; i <= m_pcMainSimulator->GetOutputPointsCount(); i++)

	{
		l_sOutput = wxT("");

		double l_nTime=m_pcMainSimulator->GetOutputStartPoint()+m_pcMainSimulator->GetOutputSampleSize() * i;

		l_sOutput << wxString::Format(wxT("%g"),l_nTime );
		l_sCurrentRow = wxT("");

		for (unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
		{

			wxString l_sPos = l_pcCurveInfoList->GetCell(l_n, 0);
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n, 2);

			if (l_sCheckState == wxT("1"))
			{
				l_sCurrentRow << l_sSpacer;
				l_sCurrentRow << wxString::Format(wxT("%.16g"), m_anResultMatrix[i][l_nPos]);
			}

		}
		l_sOutput << l_sCurrentRow;
		*m_pcExport << l_sOutput << wxT("\n");
	}
}

void SP_DLG_HybridSimulationResults::SaveODE(wxCommandEvent& p_cEvent)
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

	if (m_pcMainSimulator->GetSimulatorClass() == wxT("Stochastic"))
	{
		SP_LOGERROR(wxT("ODEs cannot be saved for stochastic simulation"));

		return;
	}
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
	for (unsigned long l_nPlacePos = 0; l_nPlacePos < m_nContinuousPlaceCount; l_nPlacePos++)
	{
		wxString l_sName = m_asPlaceNames[l_nPlacePos];
		l_sResult << wxT("d")<<l_sName << wxT("/dt = ");
		l_sResult << (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->GetPlaceODEString(l_nPlacePos) << wxT("\n\n");
	}

	//Cumulative propensity
	l_sResult << wxT("\n\n=========== Total propensitiy ODE ==========\n\n");

	l_sResult << wxT("da0")<<wxT("/dt = ");
	l_sResult << (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->GetPlaceODEString(m_nContinuousPlaceCount) << wxT("\n\n");

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

void SP_DLG_HybridSimulationResults::LoadPlacesOfType(const wxString& p_sPlaceType, unsigned long& p_nPosition)
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

		double l_nValue;

		if (p_sPlaceType == SP_DS_CONTINUOUS_PLACE)
		{
			SP_DS_DoubleMarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_DoubleMarkingAttribute*>(l_pcAttribute);
			l_nValue = l_pcMarkAttr->GetValue(true);
		}
		else
		{
			SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*>(l_pcAttribute);
			l_nValue = l_pcMarkAttr->GetValue(true);
		}

		m_anCurrentMarking[p_nPosition] = l_nValue;

		p_nPosition++;
	}
}

void SP_DLG_HybridSimulationResults::OnODESolverProperties(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	if (m_pcMainSimulator->GetSimulatorClass() == wxT("Stochastic"))
	{
		return;
	}

	spsim::Simulator* l_pcODESolver = ((dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->GetContinuousSimulator());
	SP_DLG_SimulationProperties* l_pcDlg = new SP_DLG_SimulationProperties(l_pcODESolver, NULL);

	l_pcDlg->ShowModal();

	l_pcDlg->Destroy();
}

void SP_DLG_HybridSimulationResults::UpdateViewer(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	UpdateSimulationMatrix(p_pcView);

}

void SP_DLG_HybridSimulationResults::UpdateSimulationMatrix(SP_DS_Metadata* p_pcView)
{
	SP_DS_Attribute* l_pcAttribute = nullptr;
	if(p_pcView)
	{
		l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));
	}
	else if(m_pcCurrentTablePlot)
	{
		l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	}
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
		double l_nRunCount=(double) (m_pcWorkerThread->GetCurrentRunCount());

		//we need to account for the current run
		if(m_pcMainSimulator->IsSimulationRunning())
		{
			l_nRunCount+=1;
		}

		//get the average values
		for (unsigned int l_nRow = 0; l_nRow < m_anResultMatrix.size(); l_nRow++)
			for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
			{
				m_anResultMatrix[l_nRow][l_nCol] /= l_nRunCount;
			}
	}

	UpdateXAxisValues();
}
bool SP_DLG_HybridSimulationResults::InitializeSimulator()
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

	wxBusyInfo* l_pcInfo = new wxBusyInfo(wxT("Initializing, please wait...."), this);
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

	SetSimulationProgressGaugeRange(100);

	return l_bIsInitialized;
}

void SP_DLG_HybridSimulationResults::LoadPlaces()
{
	m_anCurrentMarking.clear();

	unsigned long l_nPosition = 0;

	m_asPlaceNames.clear();
	m_mPlaceName2Position.clear();

	m_anCurrentMarking.clear();

	SP_VectorBool l_abPlaceType;

	l_abPlaceType.clear();

	LoadPlacesOfType(SP_DS_CONTINUOUS_PLACE, l_nPosition);

	m_nContinuousPlaceCount = l_nPosition;

	l_abPlaceType.assign(l_nPosition, true);

	LoadPlacesOfType(SP_DS_DISCRETE_PLACE, l_nPosition);

	if (m_sSimulatorType == wxT("Continuous"))
	{
		l_abPlaceType.resize(l_nPosition, true);

		m_nContinuousPlaceCount = l_nPosition;
	}
	else
	{
		l_abPlaceType.resize(l_nPosition, false);
	}

	if (m_sSimulatorType == wxT("Stochastic"))
	{
		SP_VectorLong l_anCurrentMarking;

		l_anCurrentMarking.clear();

		l_anCurrentMarking.assign(m_anCurrentMarking.size(), 0);

		for (unsigned long l_nPlace = 0; l_nPlace < m_anCurrentMarking.size(); l_nPlace++)
		{
			l_anCurrentMarking[l_nPlace] = m_anCurrentMarking[l_nPlace];
		}

		m_pcMainSimulator->SetInitialMarking(l_anCurrentMarking);
	}
	else
	{
		//set initial marking
		m_pcMainSimulator->SetInitialMarking(m_anCurrentMarking);

		(dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->SetPlaceTypes(l_abPlaceType);
	}

	//set place names
	m_pcMainSimulator->SetPlaceNames(m_asPlaceNames);
}
void SP_DLG_HybridSimulationResults::LoadTransitions()
{
	//Stochastic Transitions
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	CHECK_POINTER(l_pcNodeclass, return);

	//Continuous Transition
	SP_DS_Nodeclass* l_pcNodeContinuousTrans;
	l_pcNodeContinuousTrans = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	CHECK_POINTER(l_pcNodeContinuousTrans, return);

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
	spsim::TransitionType l_nTransitionType = m_sSimulatorType == wxT("Continuous") ? spsim::TRANSITION_TYPE_CONTINUOUS : spsim::TRANSITION_TYPE_STOCHASTIC;

	//Stochastic Transition
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, l_nTransitionType);
	}

	//if the user wants to run the drawn net as stochastic nets, then continuous transitions need to be added as stochastic
	l_nTransitionType = m_sSimulatorType == wxT("Stochastic") ? spsim::TRANSITION_TYPE_STOCHASTIC : spsim::TRANSITION_TYPE_CONTINUOUS;

	for (l_itElem = l_pcNodeContinuousTrans->GetElements()->begin(); l_itElem != l_pcNodeContinuousTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

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
		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

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

		wxString l_delayvalue = l_pcColList->GetActiveCellValue(1);

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
		wxString l_Begin = l_pcColList->GetActiveCellValue(1);
		wxString l_Repetition = l_pcColList->GetActiveCellValue(2); //FixedTimedFiring_Single( . ) not realized
		wxString l_End = l_pcColList->GetActiveCellValue(3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
		wxString l_sTransitionFunction = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");

		m_asTransitionNames.push_back(l_sTransitionName);

		m_mTransitionName2Position[l_sTransitionName] = l_nTransitionPosition++;

		m_pcMainSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, spsim::TRANSITION_TYPE_SCHEDULED);
	}
}

void SP_DLG_HybridSimulationResults::LoadConnections()
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

void SP_DLG_HybridSimulationResults::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
	const SP_ListEdge* l_pcEdgeList;
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sSourceNodeName, l_sDestNodeName;

	SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;

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
		if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
		{
			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight) == true)
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
			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight) == true)
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

wxString SP_DLG_HybridSimulationResults::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
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

bool SP_DLG_HybridSimulationResults::IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue)
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

void SP_DLG_HybridSimulationResults::OnSimulatorThreadEvent(SP_DS_ThreadEvent& event)
{
	unsigned long l_nFiringCount;

	switch (event.GetEventTask())
	{
	case SP_THREAD_EVENT_END_SIMULATION:

		//stop the timer no need to update
		m_pcTimer->Stop();

		UpdateSimulationMatrix();

		//Load the final result
		LoadResults();
		SetSimulationProgressGauge(GetSimulatorProgress());
		m_pcStartButton->SetLabel(wxT("Start Simulation"));
		m_pcStartButton->SetBackgroundColour(*wxGREEN);

		//user data should be the current run time
		SetSimulationStopWatch(event.GetLongValue());

		Update();

		//log the total number of firing
		l_nFiringCount=dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetFireCounter();

		SP_LOGMESSAGE(wxString::Format("Total number of firing events: %ld",l_nFiringCount));

		break;

	case SP_THREAD_EVENT_END_ONE_RUN:
		UpdateSimulationMatrix();
		break;

	default:
		break;
	}
}
void SP_DLG_HybridSimulationResults::OnTimer(wxTimerEvent& event)
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

double SP_DLG_HybridSimulationResults::GetSimulatorProgress()
{
	CHECK_POINTER(m_pcMainSimulator, return 0.0);

	double l_nSimulatorProgress = m_pcMainSimulator->GetSimulationProgress();

	return l_nSimulatorProgress;
}

int SP_DLG_HybridSimulationResults::LoadSimulatorProperties()
{
	int l_nSimulatorType;
	int l_nODESolverType;

	//call the parent class load properties
	l_nSimulatorType = SP_DLG_Simulation::LoadSimulatorProperties();

	//Retrieve simulator name
	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	l_pcAttr = l_pcSimProp->GetAttribute(wxT("ODE Solver"));

	CHECK_POINTER(l_pcAttr, return l_nSimulatorType);

	//get the value of the ode solver type
	l_nODESolverType = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttr))->GetValue();

	//change the ODE simulator
	spsim::HybridSimulator* l_pcHybridSimulator = dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator);

	CHECK_POINTER(l_pcHybridSimulator, return l_nSimulatorType);

	l_pcHybridSimulator->SetODESolver((spsim::ODESolverType) (l_nODESolverType));

	m_nCurrentODESimulator = l_nODESolverType;

	return l_nSimulatorType;
}

void SP_DLG_HybridSimulationResults::SaveSimulatorProperties()
{
	SP_DLG_Simulation::SaveSimulatorProperties();

	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	l_pcAttr = l_pcSimProp->GetAttribute(wxT("ODE Solver"));
	CHECK_POINTER(l_pcAttr, return);

	int l_nODESolverType = m_pcContinuousSolver->GetCurrentSelection();

	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttr))->SetValue(l_nODESolverType);
}

