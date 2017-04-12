/*
 * SP_GUI_SteeringDialog.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 23.03.2011
 * Short Description:
 */
//=================================================
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "spsa/spsa.h"

#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringSubWindow.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringDialog.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_DS_GUI_SteeringSetting.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_SelectNodes.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_SimulatorSetting.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"

#include <sp_gui/dialogs/SP_DLG_ViewerWindowSteering.h>


#include <wx/textdlg.h>
#include <wx/config.h>
#include <wx/msgdlg.h>
#include "wx/busyinfo.h"
#include <wx/filedlg.h>

#include <wx/statline.h>


enum
{
	SP_ID_GUI_STEERING_DIALOG_PAUSE_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_START_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_STOP_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_RESTART_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_SELECT_PLACES_TO_EDIT_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_SELECT_PARAMETERS_TO_EDIT_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_RESUME_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_REFRESH_BUTTON,
	SP_ID_GUI_STEERING_DIALOG_CHANGEVIEW_CHOICE,
	SP_ID_GUI_STEERING_DIALOG_CURRENTMODEL_CHANGED_CHOICE,
	SP_ID_GUI_STEERING_DIALOG_RESULT_REFRESH_TIMER,
	SP_ID_GUI_STEERING_DIALOG_RUNTIME_REFRESH_TIMER,
	SP_ID_GUI_STEERING_DIALOG_SETTING,
	SP_ID_GUI_STEERING_DIALOG_SIMULATOR_CATEGORY_CHANGED,
	SP_ID_GUI_STEERING_DIALOG_ADD_NEW_VIEW,
	SP_ID_GUI_STEERING_DIALOG_CHANGE_MODEL_VIEW,
	SP_ID_GUI_STEERING_DIALOG_OVERWRITE_MODEL_SETTING,
	SP_ID_GUI_STEERING_DIALOG_REFRESH_MODELNAMES,
	SP_ID_GUI_STEERING_DIALOG_REMOVE_CURRENT_VIEW,
	SP_ID_GUI_STEERING_DIALOG_SAVE_RESULTVIEWER_OUTPUT,
	SP_ID_GUI_STEERING_DIALOG_REFRESH_MODELVIEWS,
	SP_ID_GUI_STEERING_DIALOG_EDITVIEWS,
	SP_ID_GUI_STEERING_DIALOG_RESET_PARAMETER_VALUES,
	SP_ID_GUI_STEERING_DIALOG_RESET_PLACE_VALUES,
	SP_ID_GUI_STEERING_DIALOG_SIMULATOR_SETTING,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS,
	SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT,
	SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT_SELECTED,
	SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT_ALL,
	SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT_COLOURED,
	SP_ID_GUI_STEERING_DIALOG_IMG_EXPORT,
	SP_ID_GUI_STEERING_DIALOG_SIM_SPEED_CHANGED,
	SP_ID_COLLAPSEPANEL_SETS_SIZER,
	SP_ID_GUI_STEERING_DIALOG_MORE
};

enum
{
	SP_VIEW_MARKING = 0, SP_VIEW_RATE, SP_VIEW_MARKIG_DS
};
BEGIN_EVENT_TABLE(SP_GUI_SteeringDialog,SP_DLG_Simulation)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_PAUSE_BUTTON,SP_GUI_SteeringDialog::OnPauseButton)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_START_BUTTON,SP_GUI_SteeringDialog::OnStartButton)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_STOP_BUTTON,SP_GUI_SteeringDialog::OnStopButton)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_RESTART_BUTTON,SP_GUI_SteeringDialog::OnRestartButton)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_RESUME_BUTTON,SP_GUI_SteeringDialog::OnResumeButton)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_SELECT_PLACES_TO_EDIT_BUTTON,SP_GUI_SteeringDialog::OnSelectPlacesToEdit)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_RESET_PLACE_VALUES,SP_GUI_SteeringDialog::OnResetMarkingValues)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_SELECT_PARAMETERS_TO_EDIT_BUTTON,SP_GUI_SteeringDialog::OnSelectParametersToEdit)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_RESET_PARAMETER_VALUES,SP_GUI_SteeringDialog::OnResetParameterValues)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_REFRESH_BUTTON,SP_GUI_SteeringDialog::OnRefreshButton)
EVT_CHOICE(SP_ID_GUI_STEERING_DIALOG_CHANGEVIEW_CHOICE,SP_GUI_SteeringDialog::OnChangeViewCombobox)
EVT_CHOICE(SP_ID_GUI_STEERING_DIALOG_CURRENTMODEL_CHANGED_CHOICE, SP_GUI_SteeringDialog::OnModelNameChanged)
EVT_TIMER(SP_ID_GUI_STEERING_DIALOG_RESULT_REFRESH_TIMER,SP_GUI_SteeringDialog::OnResultRefreshTimer)
EVT_TIMER(SP_ID_GUI_STEERING_DIALOG_RUNTIME_REFRESH_TIMER,SP_GUI_SteeringDialog::OnRuntimeRefreshTimer)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_SETTING,SP_GUI_SteeringDialog::OnSettingButton)
EVT_CHOICE(SP_ID_GUI_STEERING_DIALOG_SIMULATOR_CATEGORY_CHANGED, SP_GUI_SteeringDialog::OnSimulatorCategoryChanged)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_ADD_NEW_VIEW,SP_GUI_SteeringDialog::OnAddNewView)
EVT_CHOICE(SP_ID_GUI_STEERING_DIALOG_CHANGE_MODEL_VIEW,SP_GUI_SteeringDialog::OnModelViewChanged)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_OVERWRITE_MODEL_SETTING,SP_GUI_SteeringDialog::OnOverWriteModelSetting)
EVT_BUTTON(wxID_CANCEL,SP_GUI_SteeringDialog::OnCloseDlg)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_SIMULATOR_SETTING,SP_GUI_SteeringDialog::OnSimulatorSetting)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_REFRESH_MODELNAMES,SP_GUI_SteeringDialog::OnRefershModelNames)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_REMOVE_CURRENT_VIEW,SP_GUI_SteeringDialog::OnRemoveCurrentView)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_REFRESH_MODELVIEWS,SP_GUI_SteeringDialog::OnRefreshModelViews)
EVT_BUTTON(SP_ID_GUI_STEERING_DIALOG_MORE,SP_GUI_SteeringDialog::OnMoreOptionButton)
EVT_CHECKBOX( SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, SP_GUI_SteeringDialog :: OnSelectClearAllItems )
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT,SP_GUI_SteeringDialog::OnExport2CSV)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT_SELECTED,SP_GUI_SteeringDialog::OnExport2CSVSelected)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT_COLOURED,SP_GUI_SteeringDialog::OnExport2CSVAll)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_CSV_EXPORT,SP_GUI_SteeringDialog::OnExport2CSVAllColoured)
EVT_MENU(SP_ID_GUI_STEERING_DIALOG_IMG_EXPORT,SP_GUI_SteeringDialog::OnImageExport)
EVT_COMMAND_SCROLL_THUMBRELEASE(SP_ID_GUI_STEERING_DIALOG_SIM_SPEED_CHANGED,SP_GUI_SteeringDialog::OnSimulationSpeedChanged)
EVT_COLLAPSIBLEPANE_CHANGED(SP_ID_COLLAPSEPANEL_SETS_SIZER, SP_DLG_Simulation :: OnCollapseSetsSizer)
END_EVENT_TABLE()

SP_GUI_SteeringDialog::SP_GUI_SteeringDialog(wxWindow* p_pcParent, spsa::SteeringClient* p_pcGUIClient, const wxString& p_sTitle) :
SP_DLG_Simulation(NULL, p_pcParent),
		m_pcGUIClient(p_pcGUIClient), m_nCurrentViewerIndex(1), m_nViewMatrixType(SP_VIEW_MARKING),
		m_bRefreshResult(false), m_bRefreshRuntime(false), m_nRefreshResultDuration(5000), m_nRefreshRuntimeDuration(5000),
		m_pcResultRefershTimer(NULL), m_pcRuntimeRefershTimer(NULL), m_nOldSimulationState(0), m_nCurrentView(0),
		m_nExportSpacer(3), m_sExportFilename(wxT("")), m_bIsModified(false), m_bEnableRefreshCalculationTime(true)

{
	m_pcResultMatrixInfo = new spsa::ResultMatrixInfo();

	SetMinimalLayout();


	CreateSimulatorBar(m_pcMainSizer);

	CreatStatisticBar(m_pcMainSizer);

	CreateControlButtonBar(m_pcMainSizer);


	//Update the simulation intervals
	m_nOutputStartPoint = m_pcGUIClient->GetCurrentModel()->GetOutputStartPoint();
	m_nOutputEndPoint = m_pcGUIClient->GetCurrentModel()->GetOutputEndPoint();
	m_nOutputSamplingSize = m_pcGUIClient->GetCurrentModel()->GetOutputSamplingSize();

	ShowSimulationIntervals();


	if (Initialize() == false)
	{
		SP_MESSAGEBOX(wxT("Can not initialize the GUI"), wxT("Information"));
		EndModal(wxID_CANCEL);
	}

	//Set the final alignment
	SetSizerAndFit(m_pcMainSizer);

	Layout();

	//Centre(wxBOTH);

}

void SP_GUI_SteeringDialog::SetMinimalLayout()
{
  wxSizer* l_pcRowSizer = NULL;

	  // SP_DLG_Simulation::SetMinimalLayout();

	    //Models
		l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(m_pcPropertyWindowSetsSizer, wxID_ANY, wxT("Models")), wxHORIZONTAL);
		m_pcModelsBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_CURRENTMODEL_CHANGED_CHOICE, wxDefaultPosition, wxSize(100, -1));

		l_pcRowSizer->Add(m_pcModelsBox, 1, wxALL);

		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_REFRESH_MODELNAMES, wxT("Refresh")), 0, wxALL);

		m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		CreateEditValuesBar(NULL);

		Layout();

		//m_pcSetsSizer->Add(l_pcRowSizer, 0, wxLEFT | wxRIGHT | wxEXPAND);
}

void SP_GUI_SteeringDialog::CreateControlButtonBar(wxSizer* p_pcParentSizer)
{
	wxBoxSizer* l_pcBottomSizer = new wxBoxSizer(wxHORIZONTAL);

	//Control Buttons
	l_pcBottomSizer->Add(new wxButton(this, SP_ID_GUI_STEERING_DIALOG_START_BUTTON, wxT("&Start")), 0, wxEXPAND | wxALL , 5);
	l_pcBottomSizer->Add(new wxButton(this, SP_ID_GUI_STEERING_DIALOG_STOP_BUTTON, wxT("&Stop")), 0, wxEXPAND | wxALL , 5);
	l_pcBottomSizer->Add(new wxButton(this, SP_ID_GUI_STEERING_DIALOG_REFRESH_BUTTON, wxT("&Refresh")), 0, wxEXPAND | wxALL , 5);
	l_pcBottomSizer->Add(new wxButton(this, SP_ID_GUI_STEERING_DIALOG_MORE, wxT("&...")), 0, wxEXPAND | wxALL , 5);
	l_pcBottomSizer->Add(new wxButton(this, wxID_CANCEL, wxT("&Close")), 0, wxEXPAND | wxALL , 5);

	p_pcParentSizer->Add(l_pcBottomSizer, 0);
}


void SP_GUI_SteeringDialog::CreatStatisticBar(wxSizer* p_pcParentSizer)
{
	wxBoxSizer* l_pcStatisticsSizer = new wxBoxSizer(wxHORIZONTAL);

	//Statistic bar
	m_pcSimulatorState = new wxStaticText(this, wxID_ANY, wxT("Stopped"));
	m_pcSimulatorState->SetToolTip(wxT("Simulator's current state"));
	l_pcStatisticsSizer->Add(m_pcSimulatorState, 1, wxEXPAND | wxALL, 5);
	l_pcStatisticsSizer->AddSpacer(1);

	m_pcSimulationRunTime = new wxStaticText(this, wxID_ANY, wxT("0.000 sec."));
	m_pcSimulationRunTime->SetToolTip(wxT("Simulation run time"));
	l_pcStatisticsSizer->Add(m_pcSimulationRunTime, 1, wxEXPAND | wxALL, 5);
	l_pcStatisticsSizer->AddSpacer(1);

	m_pcCurrentSimulatorTextBox = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(150, -1), wxCENTER);
	m_pcCurrentSimulatorTextBox->SetToolTip(wxT("Current Simulator"));
	l_pcStatisticsSizer->Add(m_pcCurrentSimulatorTextBox, 0, wxEXPAND | wxALL , 5);
	l_pcStatisticsSizer->AddSpacer(1);

	m_pcSimulationProgressPercentage = new wxStaticText(this, wxID_ANY, wxT("0 %"));
	m_pcSimulationProgressPercentage->SetToolTip(wxT("Simulator progress (percentage)"));
	l_pcStatisticsSizer->Add(m_pcSimulationProgressPercentage, 1, wxSHAPED | wxALL, 5);
	l_pcStatisticsSizer->AddSpacer(1);

	m_pcSimulationProgressBar = new wxGauge(this, -1, 100, wxDefaultPosition, wxSize(100, 20), 0);
	l_pcStatisticsSizer->Add(m_pcSimulationProgressBar, 0, wxEXPAND | wxALL, 5);
	l_pcStatisticsSizer->AddSpacer(1);

	m_pcSimulationProgressBar->SetRange(100);
	m_pcSimulationProgressBar->SetToolTip(wxT("simulator progress"));

	p_pcParentSizer->Add(l_pcStatisticsSizer, 0, wxEXPAND );

}
void SP_GUI_SteeringDialog::CreateEditValuesBar(wxSizer* p_pcParentSizer)
{
	m_pcModelConfigSizer->Add(new wxStaticLine(m_pcScrolledWindow), wxSizerFlags(0).Expand().Border(wxALL, 5));
    m_pcCollPanalSteeringPlaceSizer=
			new wxCollapsiblePane(m_pcScrolledWindow,SP_ID_COLLAPSEPANEL_SETS_SIZER, "Steered places", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
	m_pcPropertyWindowSetsSizer = m_pcCollPanalSteeringPlaceSizer->GetPane();


	wxBoxSizer* l_pcEditPlaceSizer = new wxStaticBoxSizer(new wxStaticBox(m_pcPropertyWindowSetsSizer, wxID_ANY, wxT("Edit Place Values")), wxVERTICAL);

	//Places
	wxBoxSizer* l_pcPlaceTreeSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* l_pcEditPlaceButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcEditPlaceSizer->Add(l_pcPlaceTreeSizer, 1, wxEXPAND );
	l_pcEditPlaceSizer->Add(l_pcEditPlaceButtonsSizer, 0, wxEXPAND );

	m_pcPlaceEditingWindow = new SP_GUI_SteeringSubWindow(m_pcPropertyWindowSetsSizer, m_pcGUIClient, wxT("Places"));

	l_pcPlaceTreeSizer->Add(m_pcPlaceEditingWindow, 1, wxEXPAND );

	l_pcEditPlaceButtonsSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_SELECT_PLACES_TO_EDIT_BUTTON, wxT("&Select")), 0, wxEXPAND , 5);
	l_pcEditPlaceButtonsSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_RESET_PLACE_VALUES, wxT("&Reset")), 0, wxEXPAND , 5);
	//l_pcEditPlaceButtonsSizer->Add(new wxButton( this, wxID_ANY, wxT("&Use as initial") ), 0, wxEXPAND ,5);


	m_pcPropertyWindowSetsSizer->SetSizerAndFit(l_pcEditPlaceSizer);
	m_pcSetsSizer->SetSizeHints(m_pcPropertyWindowSetsSizer);
	m_pcModelConfigSizer->Add(m_pcCollPanalSteeringPlaceSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));

    //parameters
	m_pcModelConfigSizer->Add(new wxStaticLine(m_pcScrolledWindow,wxID_ANY,wxDefaultPosition), wxSizerFlags(0).Expand().Border(wxALL, 5));

	//Left sizer
	m_pcCollPanalSteeringParamSizer=
			new wxCollapsiblePane(m_pcScrolledWindow,SP_ID_COLLAPSEPANEL_SETS_SIZER, "Steered parameters", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
	m_pcPropertyWindowSetsSizer = m_pcCollPanalSteeringParamSizer->GetPane();

    wxBoxSizer* l_pcEditParameterSizer = new wxStaticBoxSizer(new wxStaticBox(m_pcPropertyWindowSetsSizer, wxID_ANY, wxT("Edit Parameter Values")), wxVERTICAL);

    wxBoxSizer* l_pcParameterTreeSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* l_pcEditParameterButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

    l_pcEditParameterSizer->Add(l_pcParameterTreeSizer, 1, wxEXPAND );
    l_pcEditParameterSizer->Add(l_pcEditParameterButtonsSizer, 0, wxEXPAND );

    m_pcParameterEditingWindow = new SP_GUI_SteeringSubWindow(m_pcPropertyWindowSetsSizer, m_pcGUIClient, wxT("Parameters"));

    l_pcParameterTreeSizer->Add(m_pcParameterEditingWindow, 1, wxEXPAND );

    l_pcEditParameterButtonsSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_SELECT_PARAMETERS_TO_EDIT_BUTTON, wxT("&Select")), 0, wxEXPAND , 5);
    l_pcEditParameterButtonsSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_GUI_STEERING_DIALOG_RESET_PARAMETER_VALUES, wxT("&Reset")), 0, wxEXPAND , 5);


	m_pcPropertyWindowSetsSizer->SetSizerAndFit(l_pcEditParameterSizer);
	l_pcEditPlaceSizer->SetSizeHints(m_pcPropertyWindowSetsSizer);
	m_pcModelConfigSizer->Add(m_pcCollPanalSteeringParamSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
}

void SP_GUI_SteeringDialog::CreateSimulatorBar(wxSizer* p_pcParentSizer)
{
	wxBoxSizer* l_pcSimulatorSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Simulator")), wxHORIZONTAL);

		wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Type:")), 0, wxALL);
		//l_pcRowSizer->AddSpacer(1);
		m_pcSimulatorsCategoryBox = new wxChoice(this, SP_ID_GUI_STEERING_DIALOG_SIMULATOR_CATEGORY_CHANGED, wxDefaultPosition, wxSize(70, -1));
		l_pcRowSizer->Add(m_pcSimulatorsCategoryBox, 0, wxALL);
		l_pcSimulatorSizer->Add(l_pcRowSizer, 1, wxEXPAND);
		l_pcSimulatorSizer->AddSpacer(1);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Algorithm:")), 0, wxALL);
		//l_pcRowSizer->AddSpacer(2);
		m_pcSimulatorsAlgorithmBox = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1));
		l_pcRowSizer->Add(m_pcSimulatorsAlgorithmBox, 0, wxALL);
		l_pcSimulatorSizer->Add(l_pcRowSizer, 1, wxALL);

		m_pcSimulatorSpeedSlider =
				new wxSlider(this, SP_ID_GUI_STEERING_DIALOG_SIM_SPEED_CHANGED, spsa::MAX_SIMULATION_SPEED, 1, spsa::MAX_SIMULATION_SPEED, wxDefaultPosition, wxSize(100, -1),
						wxSL_HORIZONTAL | wxSL_LABELS | wxSL_AUTOTICKS);

		m_pcSimulatorSpeedSlider->SetToolTip(wxT("Simulator Speed (lower <-> higher)"));

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(m_pcSimulatorSpeedSlider, 0, wxALL);

		l_pcSimulatorSizer->AddSpacer(1);
		l_pcRowSizer->Add(new wxButton(this, SP_ID_GUI_STEERING_DIALOG_SIMULATOR_SETTING, wxT("&Setting ...")), 0, wxALL);
		l_pcSimulatorSizer->Add(l_pcRowSizer, 0, wxALL);

		p_pcParentSizer->Add(l_pcSimulatorSizer, 0);
}


bool SP_GUI_SteeringDialog::Initialize()
{
	//create a timer
	m_pcResultRefershTimer = new wxTimer(this, SP_ID_GUI_STEERING_DIALOG_RESULT_REFRESH_TIMER);
	m_pcRuntimeRefershTimer = new wxTimer(this, SP_ID_GUI_STEERING_DIALOG_RUNTIME_REFRESH_TIMER);

	//Add the viewer to the list
	//InitializeResultViewers();

	ReadSettingFromRegistery();

	m_pcCurrentModel = m_pcGUIClient->GetCurrentModel();

	CHECK_POINTER(m_pcCurrentModel, return false);

	SetupSimulators();

	UpdateModelNames();

	SelectCurrentSimulator();

	UpdateModelViews();

	//update simulator statistics
	UpdateStatistics();

	//load steered variables
	LoadSteeredVariables();

	StartRefreshTimers();

	return true;
}

void SP_GUI_SteeringDialog::SetupSimulators()
{
	unsigned int l_nSimulatorCategory;

	m_msSimulatorByCategory.clear();

	if (m_pcGUIClient->ReadAvailableSimulators(m_msSimulatorByCategory) != spsa::SUCCESS)
	{
		return;
	}

	for (l_nSimulatorCategory = 0; l_nSimulatorCategory < m_msSimulatorByCategory.size(); l_nSimulatorCategory++)
	{
		m_pcSimulatorsCategoryBox->Append(m_msSimulatorByCategory[l_nSimulatorCategory].sCategory);
	}

}

void SP_GUI_SteeringDialog::SelectCurrentSimulator()
{
	wxString l_sSimulatorCategory;
	wxString l_sSimulatorAlgorithm;

	unsigned int l_nPosition = 0;

	SP_MapString2Long l_mSimulatorCategory2Position;

	l_mSimulatorCategory2Position.clear();

	for (l_nPosition = 0; l_nPosition < m_msSimulatorByCategory.size(); l_nPosition++)
	{
		wxString l_sCategory = m_msSimulatorByCategory[l_nPosition].sCategory;

		l_mSimulatorCategory2Position[l_sCategory] = l_nPosition;
	}

	//read current simulator
	if (m_pcGUIClient->ReadCurrentSimulatorName(l_sSimulatorCategory, l_sSimulatorAlgorithm) != spsa::SUCCESS
		|| (l_sSimulatorCategory.IsEmpty() || l_sSimulatorAlgorithm.IsEmpty()))
	{
		//there is no specific simulator selected, so we can select the best
		CHECK_POINTER(m_pcCurrentModel, return);

		spsa::NET_TYPE l_bNetType = (spsa::NET_TYPE) (m_pcCurrentModel->GetNetType());

		for (l_nPosition = 0; l_nPosition < m_msSimulatorByCategory.size(); l_nPosition++)
		{
			if (m_msSimulatorByCategory[l_nPosition].nFitToNetType == l_bNetType)
			{
				l_sSimulatorCategory = m_msSimulatorByCategory[l_nPosition].sCategory;

				l_sSimulatorAlgorithm = (*m_msSimulatorByCategory[l_nPosition].asAlgorithms.begin());

				break;
			}
		}

	}

	ChangeSimulatorCategory(l_mSimulatorCategory2Position[l_sSimulatorCategory]);

	m_pcSimulatorsCategoryBox->SetStringSelection(l_sSimulatorCategory);

	m_pcSimulatorsAlgorithmBox->SetStringSelection(l_sSimulatorAlgorithm);

}

SP_GUI_SteeringDialog::~SP_GUI_SteeringDialog()
{

	//write setting to registery
	WriteSettingToRegistery();

	//delete the timer
	wxDELETE(m_pcResultRefershTimer);
	wxDELETE(m_pcRuntimeRefershTimer);

	wxDELETE(m_pcGUIClient);

	wxDELETE(m_pcResultMatrixInfo);
}

void SP_GUI_SteeringDialog::UpdateModelNames()
{
	//Fill in the model with the model names
	spsa::VectorString l_asModelNames;
	unsigned int l_nCurrentSelectedModel = 0;

	spsa::VectorString::const_iterator l_itModel;
	m_pcGUIClient->GetRuningModelNames(l_asModelNames);

	m_pcModelsBox->Clear();

	for (l_itModel = l_asModelNames.begin(); l_itModel != l_asModelNames.end(); l_itModel++)
	{
		m_pcModelsBox->Append((*l_itModel));

		//Get the index of the current working Model name
		if (m_pcGUIClient->GetCurrentModel()->GetModelName() == (*l_itModel))
		{
			l_nCurrentSelectedModel = m_pcModelsBox->GetCount() - 1;
		}
	}

	m_pcModelsBox->SetSelection(l_nCurrentSelectedModel);

	//update the dialog title with the new model name
	SetTitle(wxT("Steering GUI-") + m_pcGUIClient->GetCurrentModel()->GetModelName());
}

void SP_GUI_SteeringDialog::OnPauseButton(wxCommandEvent& WXUNUSED(event))
{
	m_pcGUIClient->CMD_Pause();

	//update simulator statistics
	UpdateStatistics();
}

bool SP_GUI_SteeringDialog::CheckSelectedSimulator()
{
	int l_nSimulatorCategory = m_pcSimulatorsCategoryBox->GetSelection();
	spsa::NET_TYPE l_nNetType;

	l_nNetType = (spsa::NET_TYPE) (m_pcCurrentModel->GetNetType());

	if (m_msSimulatorByCategory[l_nSimulatorCategory].nFitToNetType != l_nNetType)
	{
		int l_nAnswer = SP_MESSAGEBOX(wxT("The selected simulator type does not best fit this net class. Do you want to proceed with this settting?"), wxT("warning"), wxYES_NO);

		return (l_nAnswer == wxYES);
	}

	return true;
}
void SP_GUI_SteeringDialog::OnStartButton(wxCommandEvent& WXUNUSED(event))
{
	wxString l_sSimulationAlgorithm = m_pcSimulatorsAlgorithmBox->GetStringSelection();

	if (CheckSelectedSimulator() && UpdateSimulationIntervals() == true)
	{
		//clears the simulator statistics
		ClearSimulatorStatistics();

		m_pcSimulatorState->SetLabel(wxT("Initializing"));

		m_pcGUIClient->CMD_Start(l_sSimulationAlgorithm, m_nOutputStartPoint, m_nOutputEndPoint, m_nOutputSamplingSize);

		//refresh the result matrix
		m_an2DResultMatrix.clear();

		//Update the viewer
		UpdateView();

		//say that we can accept interval update
		m_pcIntervalStartTextCtrl->SetModified(false);
		m_pcIntervalEndTextCtrl->SetModified(false);
		m_pcResultPointCountTextCtrl->SetModified(false);
	}
}

void SP_GUI_SteeringDialog::OnResumeButton(wxCommandEvent& WXUNUSED(event))
{
	m_pcGUIClient->CMD_Resume();

	//update simulator statistics
	UpdateStatistics();
}
void SP_GUI_SteeringDialog::OnRestartButton(wxCommandEvent& WXUNUSED(event))
{
	wxString l_sSimulationAlgorithm = m_pcSimulatorsAlgorithmBox->GetStringSelection();

	if (CheckSelectedSimulator() && UpdateSimulationIntervals() == true)
	{
		//clears the simulator statistics
		ClearSimulatorStatistics();

		m_pcGUIClient->CMD_Restart(l_sSimulationAlgorithm, m_nOutputStartPoint, m_nOutputEndPoint, m_nOutputSamplingSize);

		//refresh the result matrix
		m_an2DResultMatrix.clear();

		//Update the viewer
		UpdateView();
	}

}

void SP_GUI_SteeringDialog::ClearSimulatorStatistics()
{
	m_pcSimulatorState->SetLabel(wxT("Initializing"));

	m_pcSimulationProgressPercentage->SetLabel(wxT("0%"));

	//simulator progress bar
	m_pcSimulationProgressBar->SetValue(0);

	m_pcSimulationRunTime->SetLabel(wxT("0 sec."));
}

void SP_GUI_SteeringDialog::OnStopButton(wxCommandEvent& WXUNUSED(event))
{
	m_pcGUIClient->CMD_Stop();
}

void SP_GUI_SteeringDialog::OnRefreshButton(wxCommandEvent& WXUNUSED(event))
{
	//Reload the result matrix
	RefreshResultMatrix();

	//Update the viewer
	UpdateView();

	//update simulator statistics
	UpdateStatistics();
}

void SP_GUI_SteeringDialog::OnChangeViewCombobox(wxCommandEvent& WXUNUSED(event))
{
	SaveCurrentViewSetting();

	ChangeCurrentViewerType();

	LoadCurrentView();

	//we need to ask the user about saving the model
	SetModified();
}

void SP_GUI_SteeringDialog::ChangeCurrentViewerType()
{
	StopRefreshTimers();

	//Destroy old viewer
	m_apcResultViewers[m_nCurrentViewerIndex]->Destroy();

	//Get the index of the new selected viewer
	m_nCurrentViewerIndex = m_pcOutputViewerType->GetSelection();

	if (m_nCurrentViewerIndex < m_apcResultViewers.size())
	{
		m_apcResultViewers[m_nCurrentViewerIndex]->Create();

		//load the view information to the views
		m_apcResultViewers[m_nCurrentViewerIndex]->LoadViewFromServerFormat((*m_pcModelViews)[m_nCurrentView]);

	}

	StartRefreshTimers();
}

void SP_GUI_SteeringDialog::LoadCurrentView()
{
	spsa::ModelView* l_pcCurrentView = (*m_pcModelViews)[m_nCurrentView];
	spsa::Attribute* l_pcAttribute = NULL;

	spsa::AttributeVectorAttribute* l_pcCurveInfo;

	unsigned long l_nIndex = 0;
	int l_nLineWidth = -1;
	int l_nLineStyle = -1;
	wxString l_nLineColorName = wxT("");
	wxString l_sCurveName = wxT("");
	bool l_bShow = false;

	spsa::RM_ROW_TYPE l_nNodeType;

	CHECK_POINTER(l_pcCurrentView, return);

	StopRefreshTimers();

	l_pcAttribute = l_pcCurrentView->GetCurveInfo();

	CHECK_POINTER(l_pcAttribute, return);

	//delete the current items if any
	m_pcItemsChoiceCheckListBox->Clear();

	//clear old curves
	m_apcResultViewers[m_nCurrentViewerIndex]->ClearCurves();

	//clear shown curve
	m_apcResultViewers[m_nCurrentViewerIndex]->ClearShownCurves();

	l_pcCurveInfo = dynamic_cast<spsa::AttributeVectorAttribute*>(l_pcAttribute);

	unsigned int l_nCurveCount = 0;

	m_pcResultMatrixInfo->SetRowCount(l_pcCurveInfo->size());

	//get shown matrix type
	l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);

	m_nViewMatrixType = dynamic_cast<spsa::AttributeUint*>(l_pcAttribute)->GetValue();

	//load curves from the current view
	for (unsigned long l_nElement = 0; l_nElement < l_pcCurveInfo->size(); l_nElement++)
	{

		CHECK_POINTER(l_pcCurveInfo->GetElement(l_nElement), return);

		spsa::AttributeStruct* l_pcAttribute = dynamic_cast<spsa::AttributeStruct*>(l_pcCurveInfo->GetElement(l_nElement));

		if (l_pcAttribute->GetField(wxT("INDEX")) != NULL)
		{
			l_nIndex = dynamic_cast<spsa::AttributeUlong*>(l_pcAttribute->GetField(wxT("INDEX")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_NAME")) != NULL)
		{
			l_sCurveName = l_pcAttribute->GetField(wxT("CURVE_NAME"))->GetValueString();
		}

		if (l_pcAttribute->GetField(wxT("LINE_WIDTH")) != NULL)
		{
			l_nLineWidth = dynamic_cast<spsa::AttributeInt*>(l_pcAttribute->GetField(wxT("LINE_WIDTH")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("LINE_STYLE")) != NULL)
		{
			l_nLineStyle = dynamic_cast<spsa::AttributeInt*>(l_pcAttribute->GetField(wxT("LINE_STYLE")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_COLOR_NAME")) != NULL)
		{
			l_nLineColorName = l_pcAttribute->GetField(wxT("CURVE_COLOR_NAME"))->GetValueString();
		}

		if (l_pcAttribute->GetField(wxT("SHOW")) != NULL)
		{
			l_bShow = dynamic_cast<spsa::AttributeBool*>(l_pcAttribute->GetField(wxT("SHOW")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_TYPE")) != NULL)
		{
			l_nNodeType = (spsa::RM_ROW_TYPE) (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute->GetField(wxT("CURVE_TYPE")))->GetValue());
		}

		m_pcItemsChoiceCheckListBox->Append(l_sCurveName);

		m_pcItemsChoiceCheckListBox->Check(l_nCurveCount, l_bShow);

		//fill in the result matrix info
		m_pcResultMatrixInfo->SetRowInfo(l_nElement, l_nIndex, l_nNodeType);

		//Add curve to the viewer for this item. Initially this curve will not be shown
		m_apcResultViewers[m_nCurrentViewerIndex]->AddCurve(l_sCurveName, l_nElement, &m_an2DResultMatrix);

		m_apcResultViewers[m_nCurrentViewerIndex]->ShowCurve(l_nCurveCount, l_bShow);

		l_nCurveCount++;
	}

	//send the server a request to update the result matrix info.
	m_pcGUIClient->UpdateResultMatrixInfo(m_pcResultMatrixInfo);

	RefreshResultMatrix();

	//Update the viewer
	UpdateView();

	StartRefreshTimers();
}

void SP_GUI_SteeringDialog::OnItemStateChanged(wxCommandEvent& event)
{
	unsigned int l_nSelectedItem = event.GetSelection();

	if (l_nSelectedItem >= m_pcItemsChoiceCheckListBox->GetCount())
	{
		return;
	}

	//check the item state
	if (m_pcItemsChoiceCheckListBox->IsChecked(l_nSelectedItem))
	{
		m_apcResultViewers[m_nCurrentViewerIndex]->ShowCurve(l_nSelectedItem, true);

		SaveItemCheckState(l_nSelectedItem, true);
	}
	else
	{
		m_apcResultViewers[m_nCurrentViewerIndex]->ShowCurve(l_nSelectedItem, false);

		SaveItemCheckState(l_nSelectedItem, false);
	}

	//Update the viewer
	m_apcResultViewers[m_nCurrentViewerIndex]->Update();

	//we need to ask the user about saving the model
	SetModified();

}

void SP_GUI_SteeringDialog::OnResultRefreshTimer(wxTimerEvent& WXUNUSED(event))
{
	m_pcResultRefershTimer->Stop();

	wxStopWatch l_sw;

	l_sw.Start();

	//Reload the result matrix
	RefreshResultMatrix();

	//TODO: lightweight update here
	UpdateView();

	//estimate the required time to transfer the result matrix
	//TODO: give the user the option to turn off this check
	if (l_sw.Time() > m_nRefreshResultDuration && m_bEnableRefreshCalculationTime == true)
	{
		wxString l_sMsg(wxT("Need more time to transfer the result matrix. "));

		l_sMsg << wxT("Refresh time is set to ") << m_nRefreshResultDuration << wxT(" ms. ");
		l_sMsg << wxT("While we need ") << l_sw.Time() << wxT(" ms. ");
		l_sMsg << wxT("The refresh duration is automatically adjusted.");

		m_nRefreshResultDuration = l_sw.Time() + 100;

		SP_LOGMESSAGE( l_sMsg);
	}

	//restart the timer
	m_pcResultRefershTimer->Start(m_nRefreshResultDuration);
}

void SP_GUI_SteeringDialog::OnRuntimeRefreshTimer(wxTimerEvent& WXUNUSED(event))
{
	m_pcRuntimeRefershTimer->Stop();

	//update simulator statistics
	UpdateStatistics();

	m_pcRuntimeRefershTimer->Start(m_nRefreshRuntimeDuration);
}

void SP_GUI_SteeringDialog::UpdateView()
{
		if (m_an2DResultMatrix.size() == 0)
		{
			return;
		}

		//TODO:update all view window here


	//Update the plot
	//m_apcResultViewers[m_nCurrentViewerIndex]->SetXAxisValues(&m_anXAxisValues);

	//TODO: update all open view windows
	//m_apcResultViewers[m_nCurrentViewerIndex]->Update();
}

bool SP_GUI_SteeringDialog::UpdateSimulationIntervals()
{
	//check the user input
	if (GetStartOutputPoint() > GetEndOutputPoint())
	{
		SP_MESSAGEBOX(wxT("Invalid input, Output start point has to be less than output end point"));

		return false;
	}

	if (GetStartOutputPoint() < 0 || GetEndOutputPoint() < 0 || GetSamplingStepSize() < 0)
	{
		SP_MESSAGEBOX(wxT("Invalid input, Output start point and output end point can not be less than zero"));

		return false;
	}

	m_nOutputStartPoint = GetStartOutputPoint();
	m_nOutputEndPoint = GetEndOutputPoint();
	m_nOutputSamplingSize = GetSamplingStepSize();

	return true;

}
void SP_GUI_SteeringDialog::ShowSimulationIntervals()
{

	if (m_pcIntervalStartTextCtrl->IsModified() == false)
	{
		m_pcIntervalStartTextCtrl->SetValue(dsszmc::aux::toString(m_nOutputStartPoint));
	}

	if (m_pcIntervalEndTextCtrl->IsModified() == false)
	{
		m_pcIntervalEndTextCtrl->SetValue(dsszmc::aux::toString(m_nOutputEndPoint));
	}

	if (m_pcResultPointCountTextCtrl->IsModified() == false)
	{
		m_pcResultPointCountTextCtrl->SetValue(dsszmc::aux::toString(m_nOutputSamplingSize));
	}
}

unsigned long SP_GUI_SteeringDialog::GetOutputResultCount()
{
	return (unsigned long) ((GetEndOutputPoint() - GetStartOutputPoint()) / GetSamplingStepSize() + 1);
}

double SP_GUI_SteeringDialog::GetStartOutputPoint()
{
	double l_nValue = 0;
	wxString l_sValueString;

	l_sValueString = m_pcIntervalStartTextCtrl->GetValue();
	l_sValueString.ToDouble(&l_nValue);

	//Check The Value
	if (l_nValue < 0)
	{
		SP_MESSAGEBOX(wxT("start output point has to be >=0"));
		return -1;
	}

	return l_nValue;
}
double SP_GUI_SteeringDialog::GetEndOutputPoint()
{
	double l_nValue;
	wxString l_sValueString;

	l_sValueString = m_pcIntervalEndTextCtrl->GetValue();
	l_sValueString.ToDouble(&l_nValue);

	//Check The Value
	if (l_nValue < 0)
	{
		SP_MESSAGEBOX(wxT("End output point has to be >=0"));
		return -1;
	}

	return l_nValue;
}
double SP_GUI_SteeringDialog::GetSamplingStepSize()
{
	double l_nValue = 0;
	wxString l_sValueString;

	l_sValueString = m_pcResultPointCountTextCtrl->GetValue();
	l_sValueString.ToDouble(&l_nValue);

	//Check The Value
	if (l_nValue < 0)
	{
		SP_MESSAGEBOX(wxT("Sampling step size has to be >=0"));
		return -1;
	}

	return l_nValue;
}

void SP_GUI_SteeringDialog::InitializeResultViewers()
{
	SP_DS_ResultViewer* l_pcResultViewer;
	std::vector<SP_DS_ResultViewer*>::const_iterator l_itViewer;

	//Table viewer
	l_pcResultViewer = new SP_DS_TableViewer(this, m_pcOutputSizer);
	m_apcResultViewers.push_back(l_pcResultViewer);

	//xy plot viewer
	l_pcResultViewer = new SP_DS_xyPlotViewer(this, m_pcOutputSizer);
	m_apcResultViewers.push_back(l_pcResultViewer);

	//histogram
	l_pcResultViewer = new SP_DS_HistogramPlotViewer(this, m_pcOutputSizer);
	m_apcResultViewers.push_back(l_pcResultViewer);

	//Add all the viewers to the list
	m_pcOutputViewerType->Clear();
	for (l_itViewer = m_apcResultViewers.begin(); l_itViewer != m_apcResultViewers.end(); l_itViewer++)
	{
		m_pcOutputViewerType->Append((*l_itViewer)->GetName());
	}
}

void SP_GUI_SteeringDialog::OnModelNameChanged(wxCommandEvent& WXUNUSED(event))
{
	wxBusyInfo* l_pcInfo;

	CHECK_POINTER(m_pcGUIClient, return);
	CHECK_POINTER(m_pcCurrentModel, return);

	//check if the user did not change the current name
	if (m_pcCurrentModel->GetModelName() == m_pcModelsBox->GetStringSelection())
	{
		return;
	}

	StopRefreshTimers();

	if (GetIsModified())
	{

		int l_nAnswer = SP_MESSAGEBOX(wxT("Do you want to replace the server side model settings by your settings?"), wxT("Question"), wxYES_NO, this);
		if (l_nAnswer == wxYES)
		{
			SaveCurrentViewSetting();
			OverWriteModelViews();
		}

	}

	if (m_pcGUIClient->ChangeCurrentModel(m_pcModelsBox->GetSelection()))
	{
		//Get the new model name
		m_pcCurrentModel = m_pcGUIClient->GetCurrentModel();

		l_pcInfo = new wxBusyInfo(wxT("Sorting model information, please wait..."), this);

		//Update the simulation intervals
		m_pcGUIClient->ReadSimulationInterval(m_nOutputStartPoint, m_nOutputEndPoint, m_nOutputSamplingSize);

		ShowSimulationIntervals();

		wxDELETE(l_pcInfo);

		UpdateModelViews();

		ChangeViewTo(m_pcCurrentModel->GetActiveView());

		//update simulator statistics
		UpdateStatistics();

		InitializeEditedList();

		SelectCurrentSimulator();

		//load steered variables
		LoadSteeredVariables();

		SetModified(false);

		//update the dialog title with the new model name
		SetTitle(wxT("Steering GUI-") + m_pcGUIClient->GetCurrentModel()->GetModelName());
	}

	StartRefreshTimers();
}
/*void SP_GUI_SteeringDialog::OnViewerPropertiesButton(wxCommandEvent& event)
{
	OnDoEditViewerProperties(event);
}*/
void SP_GUI_SteeringDialog::RefreshResultMatrix()
{
	int l_nReturn = 0;

	//Update the simulation intervals
	m_pcGUIClient->ReadSimulationInterval(m_nOutputStartPoint, m_nOutputEndPoint, m_nOutputSamplingSize);

	ShowSimulationIntervals();

	l_nReturn = m_pcGUIClient->ReadResultMatrix(m_an2DResultMatrix);

	if (l_nReturn != spsa::SUCCESS)
	{
		m_an2DResultMatrix.clear();

		SP_LOGERROR( wxT("Can not download result from the server. The server might be busy"));

		return;
	}

	//update x axis values
	m_anXAxisValues.clear();

	//Only time as x-axis variable is available
	for (double l_nTime = m_nOutputStartPoint; l_nTime <= m_nOutputEndPoint; l_nTime += m_nOutputSamplingSize)
	{
		m_anXAxisValues.push_back(l_nTime);
	}

}

const SP_VectorString& SP_GUI_SteeringDialog::GetCheckListItemNames()
{
	if (m_nViewMatrixType == SP_VIEW_RATE)
	{
		return m_pcCurrentModel->GetTransitionNames();
	}
	else
	{
		return m_pcCurrentModel->GetPlaceNames();

	}

}

void SP_GUI_SteeringDialog::InitializeEditedList()
{
}

void SP_GUI_SteeringDialog::UpdateStatistics()
{
	wxString l_sSimulatorCategory;
	wxString l_sSimulatorAlgorithm;

	spsa::ModelRuntimeState l_ModelStatistics;

	///read the simulator statistics
	if (m_pcGUIClient->ReadModelStatistics(l_ModelStatistics) != spsa::SUCCESS)
	{
		return;
	}

	int l_nSimulatorState = l_ModelStatistics.GetSimulatorState();

	//simulator state
	switch (l_nSimulatorState)
	{
	case spsa::SIMULATOR_INITIALISING:
		m_pcSimulatorState->SetLabel(wxT("Initializing"));
		break;
	case spsa::SIMULATOR_STOPPED:
		m_pcSimulatorState->SetLabel(wxT("Stopped"));
		break;
	case spsa::SIMULATOR_RUNNING:
		m_pcSimulatorState->SetLabel(wxT("Running"));
		break;
	case spsa::SIMULATOR_PAUSED:
		m_pcSimulatorState->SetLabel(wxT("Paused"));
		break;
	default:
		m_pcSimulatorState->SetLabel(wxT("Stopped"));
		break;

	}

	m_pcCurrentSimulatorTextBox->SetLabel(l_ModelStatistics.GetCurrentSimulatorCategory() + wxT("(") + l_ModelStatistics.GetCurrentSimulatorAlgorithm() + wxT(")"));

	//simulator progress
	double l_nSimulatorProgress = l_ModelStatistics.GetSimulatorProgress();

	m_pcSimulationProgressPercentage->SetLabel(dsszmc::aux::toString(l_nSimulatorProgress) + wxT("%"));

	//simulator progress bar
	m_pcSimulationProgressBar->SetValue(l_nSimulatorProgress);

	m_pcSimulationRunTime->SetLabel(FormatRuntime(l_ModelStatistics.GetSimulatorRuntime() / 1000));

#ifdef __WXMSW__
	//set the simulator speed
	if(!m_pcSimulatorSpeedSlider->IsMouseInWindow())
	{
		m_pcSimulatorSpeedSlider->SetValue(l_ModelStatistics.GetSimulatorSpeed());
	}
#else
	m_pcSimulatorSpeedSlider->SetValue(l_ModelStatistics.GetSimulatorSpeed());
#endif

	//check the error messages
	if (l_ModelStatistics.GetErrorState() > 0)
	{
		spsa::String l_sLogMsg;

		if (m_pcGUIClient->ReadLogMessage(l_sLogMsg) == spsa::SUCCESS)
		{
			SP_LOGMESSAGE( wxT("the following error(s) have been occurred at the server side\n") + l_sLogMsg);
		}
	}

	if (m_nOldSimulationState != l_nSimulatorState)
	{
		//refresh the result matrix
		RefreshResultMatrix();

		//Update the viewer
		UpdateView();

		m_nOldSimulationState = l_nSimulatorState;
	}
}

wxString SP_GUI_SteeringDialog::FormatRuntime(const double& p_nTime)
{
	//can be represented as hours
	if (p_nTime / (60 * 60) > 1)
	{
		return wxString::Format(wxT("%.5g hrs."), p_nTime / (60 * 60));
	}
	//can be represented as minutes
	else
		if (p_nTime / (60) > 1)
		{
			return wxString::Format(wxT("%.5g min."), p_nTime / (60));
		}
		else
		{
			return wxString::Format(wxT("%.5g sec."), p_nTime);
		}

	return wxT("");
}

void SP_GUI_SteeringDialog::OnSelectPlacesToEdit(wxCommandEvent& WXUNUSED(event))
{
	m_pcPlaceEditingWindow->OnSelectItemToEdit();

	//we need to ask the user about saving the model
	SetModified();

}

void SP_GUI_SteeringDialog::OnSelectParametersToEdit(wxCommandEvent& WXUNUSED(event))
{
	m_pcParameterEditingWindow->OnSelectItemToEdit();

	//we need to ask the user about saving the model
	SetModified();
}

void SP_GUI_SteeringDialog::OnSettingButton(wxCommandEvent& WXUNUSED(event))
{
	SP_DS_GUI_SteeringSetting l_dlg(this);

	// read current setting
	l_dlg.SetRefreshResult(m_bRefreshResult);
	l_dlg.SetRefreshRuntime(m_bRefreshRuntime);

	l_dlg.SetResultRefreshDuration(m_nRefreshResultDuration / 1000);
	l_dlg.SetRuntimeRefreshDuration(m_nRefreshRuntimeDuration / 1000);

	l_dlg.SetTimeOut(m_pcGUIClient->GetTimeout());

	//set enable estimation of refresh time
	l_dlg.SetEnableRefreshCalculationTime(m_bEnableRefreshCalculationTime);

	l_dlg.SetSteeringConstant(m_pcPlaceEditingWindow->GetSpinIncrement());

	//update the gui with these values
	l_dlg.Update();

	if (l_dlg.ShowModal() == wxID_OK)
	{
		//write current setting
		m_nRefreshResultDuration = l_dlg.GetResultRefreshDuration() * 1000;
		m_nRefreshRuntimeDuration = l_dlg.GetRuntimeRefreshDuration() * 1000;

		m_bRefreshResult = l_dlg.GetRefreshResult();
		m_bRefreshRuntime = l_dlg.GetRefreshRuntime();

		m_bEnableRefreshCalculationTime = l_dlg.GetEnableRefreshCalculationTime();

		m_pcGUIClient->SetTimeout(l_dlg.GetTimeout());

		m_pcPlaceEditingWindow->SetSpinIncrement(l_dlg.GetSteeringConstant());
		m_pcParameterEditingWindow->SetSpinIncrement(l_dlg.GetSteeringConstant());

		//restart the timers
		StopRefreshTimers();

		StartRefreshTimers();
	}

}


void SP_GUI_SteeringDialog::OnSimulatorCategoryChanged(wxCommandEvent& event)
{
	unsigned int l_nSelection = event.GetSelection();

	ChangeSimulatorCategory(l_nSelection);

	//TODO: check the right category her

	//we need to ask the user about saving the model
	SetModified();
}

void SP_GUI_SteeringDialog::ChangeSimulatorCategory(const unsigned int& p_nCategory)
{
	SP_VectorString l_aAlgorithms;

	unsigned int l_nSimulatorAlgorithm = 0;

	if (p_nCategory > m_msSimulatorByCategory.size())
	{
		return;
	}

	l_aAlgorithms = m_msSimulatorByCategory[p_nCategory].asAlgorithms;

	//clear old selection
	m_pcSimulatorsAlgorithmBox->Clear();

	//add the new algorithms
	for (l_nSimulatorAlgorithm = 0; l_nSimulatorAlgorithm < l_aAlgorithms.size(); l_nSimulatorAlgorithm++)
	{
		m_pcSimulatorsAlgorithmBox->Append(l_aAlgorithms[l_nSimulatorAlgorithm]);
	}

	m_pcSimulatorsAlgorithmBox->SetSelection(0);
}
void SP_GUI_SteeringDialog::WriteSettingToRegistery()
{
	wxConfig l_config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	l_config.Write(wxT("Refresh Result"), m_bRefreshResult);
	l_config.Write(wxT("Refresh Runtime"), m_bRefreshRuntime);

	//Refresh rate
	l_config.Write(wxT("Refresh Result Duration"), m_nRefreshResultDuration);
	l_config.Write(wxT("Refresh Runtime Duration"), m_nRefreshRuntimeDuration);

	//calculate refresh time
	l_config.Write(wxT("calculate Refresh Time"), m_bEnableRefreshCalculationTime);

	//spin increment
	l_config.Write(wxT("spin increment"), m_pcPlaceEditingWindow->GetSpinIncrement());

	//timeout
	l_config.Write(wxT("Timeout"), m_pcGUIClient->GetTimeout());

	l_config.Flush();
}
void SP_GUI_SteeringDialog::ReadSettingFromRegistery()
{
	wxConfig l_config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	//refresh data
	if (!l_config.Read(wxT("Refresh Result"), &m_bRefreshResult))
	{
		m_bRefreshResult = true;
	}

	if (!l_config.Read(wxT("Refresh Runtime"), &m_bRefreshRuntime))
	{
		m_bRefreshRuntime = true;
	}

	//Refresh rate
	if (!l_config.Read(wxT("Refresh Result Duration"), &m_nRefreshResultDuration))
	{
		m_nRefreshResultDuration = 1000;
	}

	if (!l_config.Read(wxT("Refresh Runtime Duration"), &m_nRefreshRuntimeDuration))
	{
		m_nRefreshRuntimeDuration = 1000;
	}

	//timeout
	int l_nTimeout;
	if (!l_config.Read(wxT("Timeout"), &l_nTimeout))
	{
		l_nTimeout = 30;
	}

	m_pcGUIClient->SetTimeout(l_nTimeout);

	double l_nSpinInc;

	if (!l_config.Read(wxT("spin increment"), &l_nSpinInc))
	{
		l_nSpinInc = 0.1;
	}

	//set the spin increment
	m_pcPlaceEditingWindow->SetSpinIncrement(l_nSpinInc);
	m_pcParameterEditingWindow->SetSpinIncrement(l_nSpinInc);

	//calculate refresh time
	l_config.Read(wxT("calculate Refresh Time"), &m_bEnableRefreshCalculationTime);

	if (m_nRefreshResultDuration <= 0)
	{
		m_nRefreshResultDuration = 5000;
	}

	if (m_nRefreshRuntimeDuration <= 0)
	{
		m_nRefreshRuntimeDuration = 5000;
	}
}

void SP_GUI_SteeringDialog::UpdateModelViews()
{
	//get current model views
	m_pcModelViews = m_pcGUIClient->GetCurrentModel()->GetViews();

	CHECK_POINTER(m_pcModelViews, return);

	//clear old views if any
	m_pcListboxShowAllGraphViewName->Clear();

	if (m_pcModelViews->size() == 0)
	{
		m_pcListboxShowAllGraphViewName->Append(wxT("Default view"));
		m_pcListboxShowAllGraphViewName->SetSelection(0);
		m_pcModelViews->push_back(CreateNewView(wxT("Default view")));
	}
	else
	{
		for (unsigned int l_nView = 0; l_nView < m_pcModelViews->size(); l_nView++)
		{
			m_pcListboxShowAllGraphViewName->Append(((*m_pcModelViews)[l_nView])->GetName());
			m_pcListboxShowAllGraphViewName->SetSelection(0);
		}
	}
}
void SP_GUI_SteeringDialog::OnAddNewView(wxCommandEvent& WXUNUSED(event))
{
	wxTextEntryDialog* l_pcModelViewDlg;
	wxString l_sViewName;

	int l_nAnswer = 0;

	l_pcModelViewDlg = new wxTextEntryDialog(this, wxT("Enter view name:"));

	//ask the user to enter view name until he gives a valid
	//name or give up
	while ((l_nAnswer = l_pcModelViewDlg->ShowModal()) != wxID_CANCEL)
	{
		l_sViewName = l_pcModelViewDlg->GetValue();

		//check if the user supplied name is valid
		if (l_sViewName == wxT(""))
		{
			SP_MESSAGEBOX(wxT("Invalid view name. View name can not be empty"), wxT("Error"));
			continue;
		}

		if (IsViewNameExist(l_sViewName) == true)
		{
			SP_MESSAGEBOX(wxT("Invalid view name. A view with this name already exists"), wxT("Error"));
			continue;
		}

		break;
	}

	if (l_nAnswer == wxID_CANCEL)
	{
		return;
	}

	//add the new view to the views list

	//save old view setting
	SaveCurrentViewSetting();

	spsa::ModelView* l_pcModelView = (*m_pcModelViews)[m_nCurrentView]->Clone();

	CHECK_POINTER(l_pcModelView, return);

	l_pcModelView->SetName(l_sViewName);

	m_pcModelViews->push_back(l_pcModelView);

	unsigned int l_nNewView = m_pcModelViews->size() - 1;

	UpdateModelViews();

	//select the new view as the current one
	//m_pcModelViewBox->SetSelection(l_nNewView);

	//change the view matrix type to the current view type;
	ChangeViewTo(l_nNewView);

	//we need to ask the user about saving the model
	SetModified();

	//destroy the dialog
	l_pcModelViewDlg->Destroy();

}
bool SP_GUI_SteeringDialog::IsViewNameExist(const wxString p_sViewName)
{
	spsa::VectorModelView::const_iterator l_itView;

	CHECK_POINTER(m_pcModelViews, return false);

	for (l_itView = m_pcModelViews->begin(); l_itView != m_pcModelViews->end(); l_itView++)
	{
		if ((*l_itView)->GetName() == p_sViewName)
		{
			return true;
		}
	}

	return false;
}

spsa::ModelView* SP_GUI_SteeringDialog::CreateNewView(const wxString& p_sModelViewName)
{
	spsa::ModelView* l_pcView;
	spsa::Attribute* l_pcAttribute = NULL;

	l_pcView = new spsa::ModelView(p_sModelViewName);

	//get current viewer type
	l_pcAttribute = l_pcView->GetAttribute(wxT("VIEWER_TYPE"));
	CHECK_POINTER(l_pcAttribute, return NULL);
	(dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->SetValue(spsa::VIEWTYPE_XYPLOT);

	//get current view matrix type
	l_pcAttribute = l_pcView->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return NULL);
	(dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->SetValue(spsa::VIEWITEMTYPE_MARKING);

	l_pcView->SetTitle(p_sModelViewName);

	//get the model type
	spsa::MODEL_TYPE l_nModelType = m_pcCurrentModel->GetModelType();

	spsa::RM_ROW_TYPE l_nCurveType;

	if (l_nModelType == spsa::COLORED_MODEL)
	{
		m_asEditedPlaceNames = dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColoredPlaceNames();

		//colored curve type
		l_nCurveType = spsa::RM_ROW_TYPE_COLORED_PLACE;
	}
	else
	{
		m_asEditedPlaceNames = m_pcCurrentModel->GetPlaceNames();

		//Uncolored node
		l_nCurveType = spsa::RM_ROW_TYPE_UNCOLORED_PLACE;
	}

	unsigned long l_nPosition = 0;

	for (SP_VectorString::iterator l_itName = m_asEditedPlaceNames.begin(); l_itName != m_asEditedPlaceNames.end(); l_itName++)
	{
		l_pcView->AddNewCurve(l_nPosition++, (*l_itName), -1, -1, l_nCurveType);

		//for colored model, it is impossible to load all of the unfolded places
		if (l_nPosition > 100)
		{
			break;
		}
	}

	return l_pcView;
}

void SP_GUI_SteeringDialog::OnModelViewChanged(wxCommandEvent& event)
{
	if (event.GetSelection() < 0)
	{
		return;
	}

	SaveCurrentViewSetting();

	//Get the current selection
	ChangeViewTo((unsigned int) event.GetSelection());

	//we need to ask the user about saving the model
	SetModified();
}

void SP_GUI_SteeringDialog::ChangeViewTo(const unsigned int& l_ViewIndex)
{
	spsa::Attribute* l_pcAttribute = NULL;

	m_nCurrentView = l_ViewIndex;

	CHECK_POINTER(m_pcModelViews, return);

	if (m_nCurrentView >= m_pcModelViews->size())
	{
		SP_LOGWARNING( wxT("invalid current view index, reset to 0"));

		m_nCurrentView = 0;
	}
	//UpdateModelViews->SetSelection(m_nCurrentView);

	//get current viewer type
	l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEWER_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);
	unsigned int l_nViewerType = (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->GetValue();

	//get current view matrix type
	l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);
	unsigned int l_nViewerMatrixType = (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->GetValue();

	if (m_pcOutputViewerType->GetCount() <= l_nViewerType)
	{
		l_nViewerType = 1;

		SP_LOGWARNING( wxT("invalid viewer index, reset to xy plot"));
	}

	if (m_pcOutputViewerType->GetCount() <= l_nViewerType)
	{
		l_nViewerMatrixType = 0;

		SP_LOGWARNING( wxT("invalid result viewer index, reset to marking view"));
	}

	m_pcOutputViewerType->SetSelection(l_nViewerType);

	ChangeCurrentViewerType();

	LoadCurrentView();

	//refresh the result matrix
	RefreshResultMatrix();
}

void SP_GUI_SteeringDialog::SaveCurrentViewSetting()
{
	spsa::Attribute* l_pcAttribute = NULL;

	//copy old setting
	l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEWER_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->SetValue(m_pcOutputViewerType->GetSelection());

	//get current view matrix type
	l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->SetValue(m_nViewMatrixType);

	m_apcResultViewers[m_nCurrentViewerIndex]->SaveViewToServerFormat((*m_pcModelViews)[m_nCurrentView]);
}

void SP_GUI_SteeringDialog::OverWriteModelViews()
{
	CHECK_POINTER(m_pcGUIClient, return);

	m_pcCurrentModel->SetActiveView(m_nCurrentView);

	m_pcGUIClient->WriteModelViews();

	//save the current steered positions
	m_pcPlaceEditingWindow->SaveSteeredPositions();

	m_pcParameterEditingWindow->SaveSteeredPositions();
}

void SP_GUI_SteeringDialog::LoadSteeredVariables()
{
	//load the current steered positions
	m_pcPlaceEditingWindow->LoadSteeredPositions();

	m_pcParameterEditingWindow->LoadSteeredPositions();
}

void SP_GUI_SteeringDialog::OnOverWriteModelSetting(wxCommandEvent& WXUNUSED(event))
{
	int l_nAnswer = SP_MESSAGEBOX(wxT("Are you sure you want to replace the server side model settings by your settings?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION, this);
	if (l_nAnswer == wxYES)
	{
		SaveCurrentViewSetting();

		OverWriteModelViews();
	}
}

void SP_GUI_SteeringDialog::OnCloseDlg(wxCommandEvent& WXUNUSED(event))
{
	if (GetIsModified())
	{
		int l_nAnswer = SP_MESSAGEBOX(wxT("Do you want to replace the server side model settings by your settings?"), wxT("Question"), wxYES_NO | wxICON_QUESTION, this);
		if (l_nAnswer == wxYES)
		{
			wxBusyInfo l_busy(wxT("Sending the model information to the server, please wait..."));
			SaveCurrentViewSetting();

			OverWriteModelViews();
		}

	}

    SP_Core::Instance()->SetSimulationMode(false);


	//close external windows
	for (auto l_itWindow : m_pcExternalWindows)
	{
		l_itWindow->Destroy();
	}

	//clear pointers
	m_pcExternalWindows.clear();

	//end the dialog
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(FALSE);
		Destroy();
	}
}

void SP_GUI_SteeringDialog::OnRefershModelNames(wxCommandEvent& WXUNUSED(event))
{
	UpdateModelNames();
}
void SP_GUI_SteeringDialog::OnRemoveCurrentView(wxCommandEvent& WXUNUSED(event))
{
	unsigned int l_nNextView = 0;
	int l_nAnswer = 0;

	CHECK_POINTER(m_pcModelViews, return);
	CHECK_POINTER((*m_pcModelViews)[m_nCurrentView], return);

	if (m_pcModelViews->size() < 2)
	{
		SP_MESSAGEBOX(wxT("Can not remove the current view"), wxT("information"), wxOK, this);
		return;
	}

	l_nAnswer = SP_MESSAGEBOX(wxT("Are you sure you want to remove the current view?"), wxT("Confirm"), wxYES_NO, this);

	if (l_nAnswer == wxYES)
	{
		//remove the current view
		m_pcModelViews->erase(m_pcModelViews->begin() + m_nCurrentView);

		//the new view will be either the next or the previous
		l_nNextView = m_nCurrentView >= m_pcModelViews->size() ? m_nCurrentView -= 1 : m_nCurrentView += 1;

		//update the model view names
		UpdateModelViews();

		//change the current view
		ChangeViewTo(l_nNextView);

		//we need to ask the user about saving the model
		SetModified();
	}

}

void SP_GUI_SteeringDialog::OnRefreshModelViews(wxCommandEvent& WXUNUSED(event))
{
	CHECK_POINTER(m_pcGUIClient, return);

	//warn the user before replacing his editing
	int l_nAnswer = SP_MESSAGEBOX(wxT("Your local changed will be replaced by the one in the server side. Do you want to continue?"), wxT("Warning"), wxYES_NO, this);

	if (l_nAnswer == wxNO)
	{
		return;
	}

	if (m_pcGUIClient->ReadModelViews() == spsa::SUCCESS)
	{
		//update the model view names
		UpdateModelViews();

		//change the current view
		ChangeViewTo(m_pcCurrentModel->GetActiveView());

	}

}


void SP_GUI_SteeringDialog::OnExport2CSV(wxCommandEvent& WXUNUSED(event))
{

	//PopupMenu(l_pcExportMenu);
}

void SP_GUI_SteeringDialog::OnExport2CSVSelected(wxCommandEvent& WXUNUSED(event))
{
	SP_VectorLong l_anExportedCurves;

	SP_VectorString l_asCurveType;

	bool l_bCompressExact = false;

	if (m_sExportFilename == wxT(""))
	{
		//set the export file name to the current view file name
		m_sExportFilename = (*m_pcModelViews)[m_nCurrentView]->GetName();
	}

	SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, this, &m_sExportFilename, &m_nExportSpacer, &l_bCompressExact);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		//clear old selected curves
		l_anExportedCurves.clear();

		l_asCurveType.clear();

		OpenExportFile(l_anExportedCurves, l_asCurveType);

		DirectExportToCSV(l_anExportedCurves, l_asCurveType);

		CloseExportFile();
	}

	l_pcDlg->Destroy();
}
void SP_GUI_SteeringDialog::OnExport2CSVAll(wxCommandEvent& WXUNUSED(event))
{

}
void SP_GUI_SteeringDialog::OnExport2CSVAllColoured(wxCommandEvent& WXUNUSED(event))
{

}

void SP_GUI_SteeringDialog::OpenExportFile(SP_VectorLong& p_anSelectedCurves, SP_VectorString& p_asCurveType)
{
	wxString l_sCurveName;
	wxString l_sOutput = wxT("Time");
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	spsa::ModelView* l_pcCurrentView = (*m_pcModelViews)[m_nCurrentView];

	spsa::AttributeVectorAttribute* l_pcCurveInfo;

	long l_nIndex = 0;

	wxString l_sNodeType;

	bool l_bShow = false;

	m_pcExportFileOutputStream = new wxFileOutputStream(m_sExportFilename);
	if (!m_pcExportFileOutputStream->IsOk())
	{
		return;
	}

	m_pcExportBufferdOutputStream = new wxBufferedOutputStream(*m_pcExportFileOutputStream);
	m_pcExport = new wxTextOutputStream(*m_pcExportBufferdOutputStream);

	CHECK_POINTER(l_pcCurrentView, return);

	CHECK_POINTER(l_pcCurrentView->GetCurveInfo(), return);

	l_pcCurveInfo = dynamic_cast<spsa::AttributeVectorAttribute*>(l_pcCurrentView->GetCurveInfo());

	//load curves from the current view
	for (unsigned long l_nElement = 0; l_nElement < l_pcCurveInfo->size(); l_nElement++)
	{

		CHECK_POINTER(l_pcCurveInfo->GetElement(l_nElement), return);

		spsa::AttributeStruct* l_pcAttribute = dynamic_cast<spsa::AttributeStruct*>(l_pcCurveInfo->GetElement(l_nElement));

		if (l_pcAttribute->GetField(wxT("INDEX")) != NULL)
		{
			l_nIndex = dynamic_cast<spsa::AttributeUlong*>(l_pcAttribute->GetField(wxT("INDEX")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_NAME")) != NULL)
		{
			l_sCurveName = l_pcAttribute->GetField(wxT("CURVE_NAME"))->GetValueString();
		}

		if (l_pcAttribute->GetField(wxT("SHOW")) != NULL)
		{
			l_bShow = dynamic_cast<spsa::AttributeBool*>(l_pcAttribute->GetField(wxT("SHOW")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_TYPE")) != NULL)
		{
			l_sNodeType = l_pcAttribute->GetField(wxT("CURVE_TYPE"))->GetValueString();
		}

		if (l_bShow == true)
		{
			l_sOutput << l_sSpacer << l_sCurveName;

			//add this index to the list of exported curves
			p_anSelectedCurves.push_back(l_nElement);

			//add the curve type
			p_asCurveType.push_back(l_sNodeType);
		}

	}

	(*m_pcExport) << l_sOutput << endl;
}

void SP_GUI_SteeringDialog::DirectExportToCSV(const SP_VectorLong& p_anExportedCurves, const SP_VectorString& p_asCurveType)
{
	unsigned long l_nRowCount;

	wxString l_sOutput;

	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	double l_nTime = m_nOutputStartPoint;

	//Refresh the result matrix
	RefreshResultMatrix();

	l_nRowCount = m_an2DResultMatrix.size();

	//
	for (unsigned long l_nRow = 0; l_nRow < l_nRowCount; l_nRow++)
	{
		l_sOutput = wxT("");

		//write the current time
		l_sOutput << l_nTime;

		for (unsigned long l_nColumn = 0; l_nColumn < p_anExportedCurves.size(); l_nColumn++)
		{
			l_sOutput << l_sSpacer << m_an2DResultMatrix[l_nRow][p_anExportedCurves[l_nColumn]];
		}

		l_nTime += m_nOutputSamplingSize;

		//new line
		(*m_pcExport) << l_sOutput << endl;
	}

}

wxString SP_GUI_SteeringDialog::GetSpacer(int p_nSpacer)
{
	wxString l_sSpacer = wxT(" ");

	switch (p_nSpacer)
	{
	case 0:
		l_sSpacer = wxT(":");
		break;
	case 1:
		l_sSpacer = wxT(",");
		break;
	case 2:
		l_sSpacer = wxT(";");
		break;
	case 3:
		l_sSpacer = wxT("\t");
		break;
	}

	return l_sSpacer;
}

void SP_GUI_SteeringDialog::CloseExportFile()
{
	wxDELETE(m_pcExport);
	wxDELETE(m_pcExportBufferdOutputStream);
	wxDELETE(m_pcExportFileOutputStream);
}

void SP_GUI_SteeringDialog::OnImageExport(wxCommandEvent &event)
{
	spsa::Attribute* l_pcAttribute = NULL;

	if (m_pcOutputViewerType->GetSelection() > 0)
	{

		SP_DS_PlotViewer* l_pcPlotResultViewer = dynamic_cast<SP_DS_PlotViewer*>(m_apcResultViewers[m_nCurrentViewerIndex]);

		l_pcAttribute = (*m_pcModelViews)[m_nCurrentView]->GetAttribute(wxT("VIEW_NAME"));
		CHECK_POINTER(l_pcAttribute, return);
		wxString l_sDefaultName = l_pcAttribute->GetValueString();

		wxFileDialog* l_pcSaveDlg = new wxFileDialog(this, wxT("Export file name"), wxT(""), l_sDefaultName, wxT(""), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		l_pcSaveDlg->SetWildcard(l_pcPlotResultViewer->GetSupportedSaveExtensions());

		if (l_pcSaveDlg->ShowModal() == wxID_OK)
		{
			wxString l_sFileName = l_pcSaveDlg->GetPath();
			int l_nExportType = l_pcSaveDlg->GetFilterIndex();

			if (l_sFileName.BeforeLast(wxT('.')) != wxT(""))
			{
				l_sFileName = l_sFileName.BeforeLast(wxT('.'));
			}

			l_pcPlotResultViewer->ExportToImageFile(l_sFileName, l_nExportType);

		}

		l_pcSaveDlg->Destroy();
	}
}

/*void SP_GUI_SteeringDialog::OnEditCurrentViewMenu(wxCommandEvent& WXUNUSED(event))
{
	SP_DLG_SelectNodes* l_pcSelectCurvsDlg;

	l_pcSelectCurvsDlg = new SP_DLG_SelectNodes(this, m_pcCurrentModel, m_nCurrentView);
	CHECK_POINTER(l_pcSelectCurvsDlg, return);

	if (l_pcSelectCurvsDlg->ShowModal() == wxID_OK)
	{
		LoadCurrentView();
	}

	l_pcSelectCurvsDlg->Destroy();
}*/
/*void SP_GUI_SteeringDialog::OnDoEditViewerProperties(wxCommandEvent& WXUNUSED(event))
{
	SP_DLG_ResultViewerProperties* l_pcViewerProperties = new SP_DLG_ResultViewerProperties(m_apcResultViewers[m_nCurrentViewerIndex], this);

	if (l_pcViewerProperties->ShowModal() == wxID_OK)
	{
		UpdateView();

		//we need to ask the user about saving the model
		SetModified();
	}
	l_pcViewerProperties->Destroy();
}*/


void SP_GUI_SteeringDialog::OnResetParameterValues(wxCommandEvent& WXUNUSED(event))
{
	m_pcGUIClient->ResetParameterValues();

	m_pcParameterEditingWindow->RefreshValues();
}

void SP_GUI_SteeringDialog::OnResetMarkingValues(wxCommandEvent& WXUNUSED(event))
{
	m_pcGUIClient->ResetMarkingValues();

	m_pcPlaceEditingWindow->RefreshValues();
}

void SP_GUI_SteeringDialog::SaveItemCheckState(const unsigned int& p_nPosition, const bool& b_bState)
{
	spsa::ModelView* l_pcCurrentView = (*m_pcCurrentModel->GetViews())[m_nCurrentView];

	spsa::AttributeVectorAttribute* l_pcCurveInfo;

	CHECK_POINTER(l_pcCurrentView, return);

	CHECK_POINTER(l_pcCurrentView->GetCurveInfo(), return);

	l_pcCurveInfo = dynamic_cast<spsa::AttributeVectorAttribute*>(l_pcCurrentView->GetCurveInfo());

	spsa::AttributeStruct* l_pcAttribute = dynamic_cast<spsa::AttributeStruct*>(l_pcCurveInfo->GetElement(p_nPosition));

	if (l_pcAttribute->GetField(wxT("SHOW")) != NULL)
	{
		dynamic_cast<spsa::AttributeBool*>(l_pcAttribute->GetField(wxT("SHOW")))->SetValue(b_bState);
	}
}

void SP_GUI_SteeringDialog::OnSimulatorSetting(wxCommandEvent& WXUNUSED(event))
{
	SP_DLG_SimulatorSetting* l_pcSimulationSettingDlg;

	l_pcSimulationSettingDlg = new SP_DLG_SimulatorSetting(this, m_pcGUIClient, m_pcSimulatorsAlgorithmBox->GetStringSelection());

	if (!l_pcSimulationSettingDlg->IsInitialised())
	{
		SP_MESSAGEBOX(wxT("The simulator remote setting could not be loaded"), wxT("information"), wxOK, this);
		return;
	}
	if (l_pcSimulationSettingDlg->ShowModal() == wxID_OK)
	{

	}

	l_pcSimulationSettingDlg->Destroy();
}

void SP_GUI_SteeringDialog::OnSelectClearAllItems(wxCommandEvent& p_cEvent)
{
	if (m_pcSelectClearAllChkBox->Get3StateValue() == wxCHK_CHECKED)
	{
		for (unsigned int l_nItem = 0; l_nItem < m_pcItemsChoiceCheckListBox->GetCount(); l_nItem++)
		{
			m_apcResultViewers[m_nCurrentViewerIndex]->ShowCurve(l_nItem, true);

			SaveItemCheckState(l_nItem, true);

			m_pcItemsChoiceCheckListBox->Check(l_nItem, true);
		}
	}
	else
		if (m_pcSelectClearAllChkBox->Get3StateValue() == wxCHK_UNCHECKED)
		{

			for (unsigned int l_nItem = 0; l_nItem < m_pcItemsChoiceCheckListBox->GetCount(); l_nItem++)
			{
				m_apcResultViewers[m_nCurrentViewerIndex]->ShowCurve(l_nItem, false);

				SaveItemCheckState(l_nItem, false);

				m_pcItemsChoiceCheckListBox->Check(l_nItem, false);
			}
		}

	//Update the viewer
	m_apcResultViewers[m_nCurrentViewerIndex]->Update();

	//we need to ask the user about saving the model
	SetModified();
}

void SP_GUI_SteeringDialog::OnSimulationSpeedChanged(wxScrollEvent& p_cEvent)
{
	wxString l_sPosition;

	int l_nSelection;

	l_nSelection = p_cEvent.GetPosition();

	l_sPosition << l_nSelection;

	m_pcGUIClient->ChanageSimulatorSpeed(l_nSelection);

	SP_LOGDEBUG(l_sPosition);
}

void SP_GUI_SteeringDialog::StartRefreshTimers()
{
	if (m_bRefreshResult == true)
	{
		m_pcResultRefershTimer->Start(m_nRefreshResultDuration);
	}

	if (m_bRefreshRuntime == true)
	{
		m_pcRuntimeRefershTimer->Start(m_nRefreshRuntimeDuration);
	}

}
void SP_GUI_SteeringDialog::StopRefreshTimers()
{
	m_pcResultRefershTimer->Stop();

	m_pcRuntimeRefershTimer->Stop();
}

void SP_GUI_SteeringDialog::OnMoreOptionButton(wxCommandEvent& WXUNUSED(event))
{
	    wxMenu *l_pcEditViewMenu = new wxMenu;

		l_pcEditViewMenu->Append(SP_ID_GUI_STEERING_DIALOG_RESTART_BUTTON, wxT("&Restart"));
		l_pcEditViewMenu->Append(SP_ID_GUI_STEERING_DIALOG_PAUSE_BUTTON, wxT("&Pause"));
		l_pcEditViewMenu->Append(SP_ID_GUI_STEERING_DIALOG_RESUME_BUTTON, wxT("&Resume"));
		l_pcEditViewMenu->Append(wxID_ANY, wxT("&Compare with"));
		l_pcEditViewMenu->Append(SP_ID_GUI_STEERING_DIALOG_SETTING, wxT("&Setting"));

		PopupMenu(l_pcEditViewMenu);
}

void SP_GUI_SteeringDialog :: OnOpenSelectedGraphViews(wxCommandEvent& p_cEvent) {


	    wxArrayInt currentSelections;
		currentSelections.Clear();
		m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);
		size_t tmp = currentSelections.GetCount();

		spsa::ModelView* l_pcView;

		for (size_t i = 0; i < tmp; ++i) {

			// get a pointer to the view
			wxString l_sViewName=m_pcListboxShowAllGraphViewName->GetString(currentSelections[i]);

			l_pcView = FindSteeringView(l_sViewName);

			OpenViewInSeparateWindow(l_pcView);

	        //ChangeCurrentView(l_pcView);
		}

}

spsa::ModelView* SP_GUI_SteeringDialog::FindSteeringView(const wxString& p_sViewName)
{
			for(auto l_pcView:(*m_pcModelViews))
			{
				if(l_pcView->GetName()==p_sViewName)
				{
					return l_pcView;
				}
			}

    //if we didn't find it, return NULL
    return NULL;
}

void SP_GUI_SteeringDialog::OpenViewInSeparateWindow(spsa::ModelView* p_pcModelView)
{
    //check if a view with the same name is already opened
	for (auto l_itWindow : m_pcExternalWindows)
    {
        if (p_pcModelView ==  (dynamic_cast<SP_DLG_ViewerWindowSteering*>(l_itWindow))->GetModelView())
        {
            SP_LOGERROR( wxT("this view(") + l_itWindow->GetName() + wxT(") is already opened.  A view Can't be opened twice"));

            return;
        }
    }

	SP_DLG_ViewerWindowSteering* l_pcViewerWindow=
			new SP_DLG_ViewerWindowSteering(this,
					                        p_pcModelView,
											m_pcResultMatrixInfo,
											&m_an2DResultMatrix,
											&m_anXAxisValues,
											m_pcCurrentModel);

    // check if the window is created correctly
    if (l_pcViewerWindow != NULL)
    {
        m_pcExternalWindows.push_back(l_pcViewerWindow);

        l_pcViewerWindow->Show(true);
    }

}


