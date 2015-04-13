//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/08/14 11:20:00 $
// @change M.Herajy 5.6.2010
// Short Description:
//////////////////////////////////////////////////////////////////////#
#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include "export/SP_EPSDC.h"
#include "import/SP_ImportCSV.h"
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StEditTablePlot.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"
#include "sp_gui/dialogs/SP_DLG_SelectXAxisVariable.h"
#include "sp_gui/dialogs/SP_DLG_ResultViewerProperties.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_ChangeCurveAttributes.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_PlacesSelection.h"
#include "sp_gui/dialogs/SP_DLG_ShowAllModelView.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

#include <wx/busyinfo.h>
#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
//#include "sp_gui/widgets/sp_plot/plot_up.xpm"
//#include "sp_gui/widgets/sp_plot/plot_dwn.xpm"
#endif
IMPLEMENT_CLASS( SP_DLG_Simulation, wxDialog )

enum
{

	SP_ID_TREECTRL_TABLE_PLOT_OVERVIEW = SP_ID_LAST_ID + 1,
	SP_ID_BUTTON_NEW_MODEL_VIEW,
	SP_ID_BUTTON_REMOVE_MODEL_VIEW,
	SP_ID_BUTTON_EDIT_TABLE_PLOT,
	SP_ID_BUTTON_SAVE_TABLE_PLOT,
	SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES,
	SP_ID_GRID_TABLE,
	SP_ID_GRID_PLACE_CHOICE,
	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,
	SP_ID_BUTTON_START_SIMULATION,
	SP_ID_BUTTON_PLOT_ZOOM_H_IN,
	SP_ID_BUTTON_PLOT_ZOOM_H_OUT,
	SP_ID_BUTTON_PLOT_ZOOM_V_IN,
	SP_ID_BUTTON_PLOT_ZOOM_V_OUT,
	SP_ID_BUTTON_PLOT_ZOOM_HV_IN,
	SP_ID_BUTTON_PLOT_ZOOM_HV_OUT,
	SP_ID_BUTTON_PLOT_SAVE_AREA,
	SP_ID_BUTTON_EXPORT_CSV,
	SP_ID_BUTTON_LOAD_DATA,
	SP_ID_TEXT_PLOT_STATUS,
	SP_ID_BUTTON_PLOT_ZOOM_CENTRAL,
	SP_ID_PLOT_CTRL_DATA,
	SP_ID_PRINT,
	SP_ID_COMBOBOX_RESULT_VIEWER_TYPE,
	SP_ID_COMBOBOX_RESULT_MATRIX_TYPE,
	SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES,
	SP_ID_BUTTON_EDIT_VIEWER_SELECT_X_AXIS,
	SP_ID_BUTTON_SHOW_EDIT_MODEL_VIEWS_MENU,
	SP_ID_MODEL_VIEWS_BOX,
	SP_ID_BUTTON_VIEW_IMAGE_EXPORT,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS,
	SP_ID_MENU_OPEN_CURRENT_VIEW_SEPARATELY,
	SP_ID_TIMER_UPDATE_DIALOG,
	SP_ID_MENU_OPEN_ALL_SEPARATELY

};
BEGIN_EVENT_TABLE( SP_DLG_Simulation, wxDialog )

EVT_BUTTON( SP_ID_BUTTON_START_SIMULATION, SP_DLG_Simulation :: OnStartAbortSimulation )

EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_Simulation :: OnSimulationProperties )

EVT_BUTTON( wxID_CANCEL, SP_DLG_Simulation :: OnDlgCancel )
EVT_COMBOBOX( SP_ID_COMBOBOX_RESULT_VIEWER_TYPE, SP_DLG_Simulation :: OnChangeResultViewer )

EVT_CHECKLISTBOX( SP_ID_CHECKLISTBOX_PLACE_CHOICE, SP_DLG_Simulation :: OnPlaceCheckUncheck )

EVT_GRID_CELL_LEFT_DCLICK( SP_DLG_Simulation :: OnColourClick )

EVT_GRID_LABEL_LEFT_DCLICK(SP_DLG_Simulation::OnLabelDClick)

EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_H_IN, SP_DLG_Simulation :: OnPlotZoomH_In )
EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_H_OUT, SP_DLG_Simulation :: OnPlotZoomH_Out )
EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_V_IN, SP_DLG_Simulation :: OnPlotZoomV_In )
EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_V_OUT, SP_DLG_Simulation :: OnPlotZoomV_Out )

EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_HV_IN, SP_DLG_Simulation :: OnPlotZoomHV_In )
EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_HV_OUT, SP_DLG_Simulation :: OnPlotZoomHV_Out )

EVT_BUTTON( SP_ID_BUTTON_PLOT_ZOOM_CENTRAL, SP_DLG_Simulation :: OnPlotZoomCentral )

EVT_BUTTON( SP_ID_BUTTON_PLOT_SAVE_AREA, SP_DLG_Simulation :: OnPlotSaveArea )

EVT_MENU( SP_ID_BUTTON_EXPORT_CSV, SP_DLG_Simulation :: OnExportToCSV )

EVT_BUTTON( SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES, SP_DLG_Simulation :: OnDirectExportProperties )
EVT_BUTTON( SP_ID_BUTTON_LOAD_DATA, SP_DLG_Simulation :: OnLoadData )

EVT_BUTTON( SP_ID_PRINT, SP_DLG_Simulation :: OnPrint )

EVT_CHECKBOX( SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, SP_DLG_Simulation :: OnSelectClearAllItems )

EVT_BUTTON( SP_ID_BUTTON_SHOW_EDIT_MODEL_VIEWS_MENU, SP_DLG_Simulation :: OnShowEditModelViewMenu)

EVT_BUTTON( SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, SP_DLG_Simulation :: OnEditViewerProperties)

EVT_MENU( SP_ID_BUTTON_NEW_MODEL_VIEW, SP_DLG_Simulation :: OnNewModelView)
EVT_MENU( SP_ID_BUTTON_REMOVE_MODEL_VIEW, SP_DLG_Simulation :: OnRemoveModelView)
EVT_MENU( SP_ID_BUTTON_EDIT_TABLE_PLOT, SP_DLG_Simulation :: OnEditOtherView)
EVT_MENU( SP_ID_BUTTON_EDIT_VIEWER_SELECT_X_AXIS, SP_DLG_Simulation :: OnSelectXAxis)
EVT_MENU( SP_ID_MENU_OPEN_CURRENT_VIEW_SEPARATELY, SP_DLG_Simulation :: OnOpenCurrentViewSeparately)

EVT_MENU( SP_ID_MENU_OPEN_ALL_SEPARATELY, SP_DLG_Simulation :: OnOpenAllViewsSeparately)

EVT_LISTBOX_DCLICK(SP_ID_CHECKLISTBOX_PLACE_CHOICE,SP_DLG_Simulation::OnItemDClick)

EVT_COMBOBOX( SP_ID_MODEL_VIEWS_BOX, SP_DLG_Simulation :: OnChangeModelView)

EVT_MENU( SP_ID_BUTTON_VIEW_IMAGE_EXPORT, SP_DLG_Simulation :: OnImageExport)

EVT_TIMER( SP_ID_TIMER_UPDATE_DIALOG, SP_DLG_Simulation :: OnTimer)

EVT_INIT_DIALOG(SP_DLG_Simulation::OnInitDialog)

EVT_CLOSE(SP_DLG_Simulation::OnCloseWindow)

END_EVENT_TABLE()

SP_DLG_Simulation::SP_DLG_Simulation(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
		wxDialog(p_pcParent, -1, p_sTitle, wxPoint(20, 20), wxSize(800, 750), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
		m_pcGraph(p_pcGraph),
#ifdef __WXGTK__
		m_DisableOtherWindows(p_pcParent),
#endif
		m_nRefreshRate(5000),
		m_nResultPointCount(0),
		m_sExportFilename(p_pcGraph->GetParentDoc()->GetFilename()),
		m_pcExportFileOutputStream(NULL),
		m_pcExportBufferdOutputStream(NULL),
		m_pcExport(NULL), m_nExportSpacer(1),
		m_nPlotYMax(1),
		m_pcCurrentTablePlot(NULL),
		m_bCompressExact(false),
		m_sNodeclass(SP_DS_CONTINUOUS_PLACE),
		m_bSumOfNodes(false),
		m_nCurrentViewer(1), //xy plot
		m_bCanChangeCurrentView(true),
		m_pcPlaceChoiceSizerLabel(NULL),
		m_bComAuxVarSingleRun(false)
{
	m_sExportFilename = m_sExportFilename.BeforeLast('.');
	m_sExportFilename << wxT(".csv");

	//choices for X axis
	//TODO: change it in your class
	//Note that another option will be added to this list (time)
	m_pcXAxisChoices[wxT("Place")] = &m_asPlaceNames;
	m_pcXAxisChoices[wxT("Transition")] = &m_asTransitionNames;

	m_pcTimer = new wxTimer(this, SP_ID_TIMER_UPDATE_DIALOG);

	//give the simulator a pointer to Snoopy log
	spsim::Simulator::SetLogFunction(SimulatorLogFunction);
}

void SP_DLG_Simulation::OnInitDialog(wxInitDialogEvent& event)
{
	wxBusyInfo l_info(wxT("Please wait while initializing the simulation dialog"), this);

	//initialize the model views. If it is the first time, a default view
	LoadNetInformation();

	//will be added
	InitializeViews();

	//load the current view
	LoadData();

	//load the marking, rate, and parameter sets
	LoadSets();

	//TODO: add general methods here
}

void SP_DLG_Simulation::SetMinimalLayout()
{
	m_apColours = new wxColour[15];

	m_apColours[0] = *wxBLUE;
	m_apColours[1] = *wxRED;
	m_apColours[2] = *wxGREEN;
	m_apColours[3] = *wxBLACK;
	m_apColours[4] = wxColour(102, 0, 204);
	m_apColours[5] = wxColour(153, 0, 51);
	m_apColours[6] = wxColour(44, 103, 0);
	m_apColours[7] = wxColour(128, 0, 255);
	m_apColours[8] = wxColour(0, 98, 139);
	m_apColours[9] = wxColour(131, 76, 36);
	m_apColours[10] = wxColour(140, 72, 159);
	m_apColours[11] = wxColour(255, 128, 0);
	m_apColours[12] = wxColour(9, 112, 84);
	m_apColours[13] = wxColour(0, 0, 102);
	m_apColours[14] = wxColour(204, 0, 0);

	wxSizer* l_pcRowSizer;

	//Main Sizer
	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

	//Controls
	wxSizer* l_pcLeftSizer = new wxBoxSizer(wxVERTICAL);
	//Plot or Table
	wxSizer* l_pcMiddleSizer = new wxBoxSizer(wxVERTICAL);
	//Table or plot Options
	wxSizer* l_pcRightSizer = new wxBoxSizer(wxVERTICAL);

	m_pcMainSizer->Add(l_pcContentSizer, 1, wxEXPAND);
	m_pcMainSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 0, wxALL, 5);

	l_pcContentSizer->Add(l_pcLeftSizer, 0, wxALL, 2);
	l_pcContentSizer->Add(l_pcMiddleSizer, 1, wxALL | wxEXPAND, 2);
	l_pcContentSizer->Add(l_pcRightSizer, 0, wxALL | wxEXPAND, 2);

	wxSizer* l_pcSimulationControlSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Simulation control")), wxVERTICAL);
	m_pcModelViewsSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	l_pcLeftSizer->Add(l_pcSimulationControlSizer);
	m_pcOutputSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);
	l_pcMiddleSizer->Add(m_pcOutputSizer, 1, wxEXPAND);
	/*
	 //control bar
	 m_pcTableControlSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1,wxT("") ), wxHORIZONTAL );

	 l_pcMiddleSizer->Add( m_pcTableControlSizer, 0, wxEXPAND | wxALL, 2 );
	 */
	wxSizer* l_pcPlaceChoiceSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Select an item to view")), wxVERTICAL);
	wxSizer* l_pcSelectDeselecteSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);
	l_pcRightSizer->Add(m_pcModelViewsSizer);
	l_pcRightSizer->Add(l_pcPlaceChoiceSizer, 1, wxEXPAND);
	l_pcRightSizer->Add(l_pcSelectDeselecteSizer, 0, wxEXPAND);

	//select/deselect all
	m_pcSelectClearAllChkBox = new wxCheckBox(this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
	l_pcSelectDeselecteSizer->Add(m_pcSelectClearAllChkBox, 1, wxEXPAND);
	m_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);

	/*
	 m_pcPlotStatusText = new wxStaticText( this, SP_ID_TEXT_PLOT_STATUS, wxT("                                       ") );
	 m_pcPlotStatusText->SetLabel( wxT("Place: -   Timepoint: -   Value: -") );
	 //l_pcPlotStatusSizer->Add( m_pcPlotStatusText, 0, wxALL, 2 );
	 m_pcPlotStatusText->Show( false );

	 m_pcPlotControlSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1,wxT("") ), wxHORIZONTAL );
	 m_pcPlotControlSizer->Add( new wxBitmapButton( this, SP_ID_BUTTON_PLOT_ZOOM_V_IN, GetShrinkBitmap(), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );
	 m_pcPlotControlSizer->Add( new wxBitmapButton( this, SP_ID_BUTTON_PLOT_ZOOM_V_OUT, GetEnlargeBitmap(), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );

	 m_pcPlotControlSizer->Add( new wxBitmapButton( this, SP_ID_BUTTON_PLOT_ZOOM_H_IN, GetZoomInBitmap(), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );
	 m_pcPlotControlSizer->Add( new wxBitmapButton( this, SP_ID_BUTTON_PLOT_ZOOM_H_OUT, GetZoomOutBitmap(), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );

	 m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_BUTTON_PLOT_ZOOM_HV_IN, wxT("+"), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );
	 m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_BUTTON_PLOT_ZOOM_HV_OUT, wxT("-"), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );

	 m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_BUTTON_PLOT_ZOOM_CENTRAL, wxT("C"), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxALL, 2 );
	 //  m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_BUTTON_PLOT_SAVE_AREA, wxT("S"), wxDefaultPosition, wxSize( 30, 30 ) ), 0, wxEXPAND | wxALL, 2 );
	 m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_PRINT, wxT("Print..."), wxDefaultPosition, wxSize( -1, 30 ) ), 0, wxALL, 2 );

	 m_pcPlotControlSizer->Add( new wxButton( this, SP_ID_BUTTON_EXPORT_CSV, wxT("CSV-Export"), wxDefaultPosition, wxSize( -1, 30 ) ), 0, wxALL, 2 );
	 m_pcPlotControlSizer->Show( false );

	 l_pcMiddleSizer->Add( m_pcPlotControlSizer, 0, wxALL | wxEXPAND, 2 );
	 */
	// place choice
	m_pcPlaceChoiceCheckListBox = new wxCheckListBox(this, SP_ID_CHECKLISTBOX_PLACE_CHOICE, wxDefaultPosition, wxSize(200, 200));
	l_pcPlaceChoiceSizer->Add(m_pcPlaceChoiceCheckListBox, 1, wxLEFT | wxEXPAND, 5);

	//Controls
	m_pcSetsSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);
	m_pcPropertySizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);
	m_pcDirectExportSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);
	m_pcSimulationButtonSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	l_pcSimulationControlSizer->Add(m_pcSetsSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
	l_pcSimulationControlSizer->Add(m_pcPropertySizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
	l_pcSimulationControlSizer->Add(m_pcDirectExportSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
	l_pcSimulationControlSizer->Add(m_pcSimulationButtonSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

	//Simulation Parameters
	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Interval start:")), 1, wxALL | wxEXPAND, 5);
	l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval start"));
	m_pcIntervalStartTextCtrl = new wxTextCtrl(this, -1, l_pcAttr->GetValueString(), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcIntervalStartTextCtrl, 0, wxALL, 5);
	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Interval end:")), 1, wxALL | wxEXPAND, 5);
	l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval end"));
	m_pcIntervalEndTextCtrl = new wxTextCtrl(this, -1, l_pcAttr->GetValueString(), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcIntervalEndTextCtrl, 0, wxALL, 5);
	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Output step count:")), 1, wxALL | wxEXPAND, 5);
	l_pcAttr = l_pcSimProp->GetAttribute(wxT("output step"));
	m_pcResultPointCountTextCtrl = new wxTextCtrl(this, -1, l_pcAttr->GetValueString(), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcResultPointCountTextCtrl, 0, wxALL, 5);
	m_pcPropertySizer->Add(l_pcRowSizer, 1, wxEXPAND);

	// rows of direct export sizer
	l_pcRowSizer = new wxBoxSizer(wxVERTICAL);
	m_pcNoExportRadioButton = new wxRadioButton(this, -1, wxT("No export"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_pcNoExportRadioButton->SetValue(true);
	l_pcRowSizer->Add(m_pcNoExportRadioButton, 1, wxALL, 5);
	m_pcDirectExportRadioButton = new wxRadioButton(this, -1, wxT("Direct export"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcDirectExportRadioButton, 1, wxALL, 5);
	m_pcDirectExportSizer->Add(l_pcRowSizer);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES, wxT("Properties")), 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_LOAD_DATA, wxT("Load Data")), 1, wxALL, 5);
	m_pcDirectExportSizer->Add(l_pcRowSizer);

	// rows of control button
	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);
	m_pcStartButton = new wxButton(this, SP_ID_BUTTON_START_SIMULATION, wxT("Start Simulation"), wxDefaultPosition, wxDefaultSize, 0);
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	l_pcRowSizer->Add(m_pcStartButton, 0, wxALL | wxEXPAND, 5);

	m_pcSimulationProgressGauge = new wxGauge(this, -1, 100, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcSimulationProgressGauge, 1, wxALL | wxEXPAND, 5);
	m_pcSimulationButtonSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	// simulation stop watch
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Simulation run time: ")), 0, wxALL | wxEXPAND, 5);
	m_pcSimulationStopWatch = new wxStaticText(this, -1, wxT("0,0 sec"));
	l_pcRowSizer->Add(m_pcSimulationStopWatch, 1, wxALL | wxEXPAND, 5);
	m_pcSimulationButtonSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	wxSizer* l_pcViewSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Current view")), wxHORIZONTAL);
	m_pcViewBox = new wxComboBox(this, SP_ID_MODEL_VIEWS_BOX, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);
	l_pcRowSizer->Add(m_pcViewBox, 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_SHOW_EDIT_MODEL_VIEWS_MENU, wxT("Options ...")), 0, wxALL | wxEXPAND, 2);
	l_pcViewSizer->Add(l_pcRowSizer, 1, wxALL | wxALIGN_LEFT);

	//viewer type
	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer type")), wxHORIZONTAL);
	m_pcOutputViewerType = new wxComboBox(this, SP_ID_COMBOBOX_RESULT_VIEWER_TYPE, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);
	l_pcRowSizer->Add(m_pcOutputViewerType, 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, wxT("Edit ...")), 0, wxALL | wxEXPAND, 2);
	l_pcViewSizer->Add(l_pcRowSizer, 1, wxALL | wxALIGN_LEFT);

	//create the viewers
	InitializeResultViewer();

	m_pcModelViewsSizer->Add(l_pcViewSizer, 0, wxALL | wxALIGN_LEFT);

	SetSizerAndFit(m_pcMainSizer);

}

void SP_DLG_Simulation::InitializeResultViewer()
{
	SP_DS_ResultViewer* l_pcResultViewer;
	SP_VectorViewer::const_iterator l_itViewer;

	//Create the different viewers. Add more viewers here
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

	m_apcResultViewers[m_nCurrentViewer]->Create();

	m_pcOutputViewerType->SetSelection(m_nCurrentViewer);
}

void SP_DLG_Simulation::SimulatorLogFunction(wxLogLevel p_nLogLevel, const wxString& p_sLogMsg)
{
	switch (p_nLogLevel)
	{
	case spsim::LOG_EXP_ERROR:
		SP_LOGERROR( p_sLogMsg);
		break;
	case spsim::LOG_EXP_WARN:
		SP_LOGWARNING( p_sLogMsg);
		break;
	default:
		SP_LOGMESSAGE( p_sLogMsg);
		break;
	}

}

bool SP_DLG_Simulation::LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces)
{
	wxString l_sName;

	CHECK_POINTER(p_pcView, return false);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	CHECK_POINTER(l_pcAttribute, return false);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	CHECK_POINTER(p_pcViewer, return false);

	//remove old curves
	p_pcViewer->ClearCurves();

	//remove shown curves
	p_pcViewer->ClearShownCurves();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcView->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return false);

	for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	{
		wxString l_sPosition = l_pcCurveInfoList->GetCell(l_nRow, 0);
		unsigned long l_nPosition = 0;
		if (!l_sPosition.ToULong(&l_nPosition))
		{
			return false;
		}

		wxString l_sOutType = l_pcCurveInfoList->GetCell(l_nRow, 1);

		if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_asPlaceNames.size()) //unfolded place
		{
			l_sName = m_asPlaceNames[l_nPosition];

			p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
		}
		else
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_asTransitionNames.size()) //unfolded place
			{
				l_sName = m_asTransitionNames[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
			}
			else
			{
				SP_LOGERROR( wxT("Invalid node names, we stop loading the rest of the file"));

				//invalid row index, therefore we ignore the remaining rows
				l_pcCurveInfoList->RemoveRemainingRows(l_nRow);

				break;
			}

		wxString l_sOrignialName = l_pcCurveInfoList->GetCell(l_nRow, 6);

		if (l_sOrignialName != l_sName)
		{
			SP_LOGWARNING( wxT("Name for position (") + wxString::Format(wxT("%d"), l_nRow) + wxT(") is changed to ") + l_sName);
		}

		//update curve name
		l_pcCurveInfoList->SetCell(l_nRow, 6, l_sName);

		p_asPlaces.Add(l_sName);
	}

	return true;
}

void SP_DLG_Simulation::LoadData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView)
{
	wxArrayString p_asPlaces;

	LoadViewerData(p_pcViewer, p_pcView, p_asPlaces);
}

void SP_DLG_Simulation::LoadData(bool p_bCreateNewTree)
{
	wxArrayString l_asPlaces;

	LoadViewerData(m_apcResultViewers[m_nCurrentViewer], m_pcCurrentTablePlot, l_asPlaces);

	//Add the loaded items to the list
	m_pcPlaceChoiceCheckListBox->Set(l_asPlaces);

	//load old setting
	LoadViewProperties();
}

void SP_DLG_Simulation::LoadSets()
{
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_ListMetadata::const_iterator l_itElem;

	SP_SetString::iterator l_itChoice;
	for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
		wxString l_sChoice = *l_itChoice;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			if (l_sChoice == l_sGroup)
			{
				m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				break;
			}
		}
	}

	for (size_t j = 0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		wxComboBox* l_pcCombobox = m_apcComboBoxes[j];
		l_pcCombobox->Clear();
		if (l_pcAttr)
		{
			for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				wxString l_sSetName = l_pcAttr->GetCell(i, 0);
				l_pcCombobox->Append(l_sSetName);
			}
			l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
		}
	}
}

void SP_DLG_Simulation::LoadParameters()
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	m_msParameterName2Value.clear();

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_asParameterNames.push_back(l_sName);

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if(l_FE.IsOk())
		{
			SP_FunctionPtr l_Function = l_FE.getFunction();
			double l_nValue = 0;
			if (l_Function->isValue())
			{
				l_nValue = l_Function->getValue();
			}
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if(l_sType == wxT("int"))
				{
					SP_DS_FunctionEvaluatorLong el(l_pcFR, l_Function);
					try
					{
						l_nValue = el();
					}
					catch(dsszmc::functions::Exception& e)
					{
						SP_LOGERROR( wxString(e.getExcName().c_str(), wxConvUTF8) + wxT(": ") + wxString(e.getMsg().c_str(), wxConvUTF8));
					}
				}
				else if(l_sType == wxT("double"))
				{
					SP_DS_FunctionEvaluatorDouble el(l_pcFR, l_Function);
					try
					{
						l_nValue = el();
					}
					catch(dsszmc::functions::Exception& e)
					{
						SP_LOGERROR( wxString(e.getExcName().c_str(), wxConvUTF8) + wxT(": ") + wxString(e.getMsg().c_str(), wxConvUTF8));
					}
				}
			}
			//****************************************************************************************

			l_anParameterValue.push_back(l_nValue);

			m_msParameterName2Value[l_sName]=l_nValue;
		}
	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);
}

void SP_DLG_Simulation::SaveSelectedCurves()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	for (unsigned int l_nCurve = 0; l_nCurve < l_pcPlaceIdList->GetRowCount(); l_nCurve++)
	{
		//if the index is out of range, then continue
		if (l_nCurve >= m_pcPlaceChoiceCheckListBox->GetCount())
			continue;

		//Get the item ID
		wxString l_sItemID = l_pcPlaceIdList->GetCell(l_nCurve, 0);

		//Save the user selection
		long l_nSelected = (long) m_pcPlaceChoiceCheckListBox->IsChecked(l_nCurve);
		l_pcPlaceIdList->SetCell(l_nCurve, 2, wxString::Format(wxT("%ld"), l_nSelected));

		//Save Curve color
		wxString l_sColor = m_apcResultViewers[m_nCurrentViewer]->GetCurveColor(l_nCurve);
		l_pcPlaceIdList->SetCell(l_nCurve, 3, l_sColor);

		//Save Line width
		long l_nLineWidth = m_apcResultViewers[m_nCurrentViewer]->GetCurveLineWidth(l_nCurve);
		l_pcPlaceIdList->SetCell(l_nCurve, 4, wxString::Format(wxT("%ld"), l_nLineWidth));

		//Save Line style
		long l_nLineStyle = m_apcResultViewers[m_nCurrentViewer]->GetCurveLineStyle(l_nCurve);
		l_pcPlaceIdList->SetCell(l_nCurve, 5, wxString::Format(wxT("%ld"), l_nLineStyle));
	}
}

void SP_DLG_Simulation::LoadSelectedCurves()
{

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	m_apcResultViewers[m_nCurrentViewer]->ClearShownCurves();

	for (unsigned int l_nCurve = 0; l_nCurve < l_pcPlaceIdList->GetRowCount(); l_nCurve++)
	{

		//if the index is out of range, then continue
		if (l_nCurve >= m_pcPlaceChoiceCheckListBox->GetCount())
			continue;

		//Get the item ID
		wxString l_sItemID = l_pcPlaceIdList->GetCell(l_nCurve, 0);

		//load the user selection
		wxString l_sSelectionStr = l_pcPlaceIdList->GetCell(l_nCurve, 2);
		long l_nSelected = 0;
		l_sSelectionStr.ToLong(&l_nSelected);

		bool l_bIsSelected = l_nSelected > 0 ? true : false;

		m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);
		m_apcResultViewers[m_nCurrentViewer]->ShowCurve(l_nCurve, l_bIsSelected);

		//load Curve color
		wxString l_sColor = l_pcPlaceIdList->GetCell(l_nCurve, 3);
		if (l_sColor == wxT(""))
		{
			l_sColor = GetColourString(l_nCurve);
		}
		m_apcResultViewers[m_nCurrentViewer]->SetCurveColor(l_nCurve, l_sColor);

		//load Line width
		wxString l_sLineWidthStr = l_pcPlaceIdList->GetCell(l_nCurve, 4);
		long l_nLineWidth = 0;
		l_sLineWidthStr.ToLong(&l_nLineWidth);
		m_apcResultViewers[m_nCurrentViewer]->SetCurveLineWidth(l_nCurve, l_nLineWidth);

		//load Line width
		wxString l_sLineStyleStr = l_pcPlaceIdList->GetCell(l_nCurve, 5);
		long l_nLineStyle = 0;
		l_sLineStyleStr.ToLong(&l_nLineStyle);
		m_apcResultViewers[m_nCurrentViewer]->SetCurveLineStyle(l_nCurve, l_nLineStyle);
	}

}

void SP_DLG_Simulation::OnPlaceCheckUncheck(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	//get item index
	unsigned int l_nSelection = p_cEvent.GetSelection();

	//get check state
	bool l_bCheckState = m_pcPlaceChoiceCheckListBox->IsChecked(l_nSelection);

	//update the curve state
	m_apcResultViewers[m_nCurrentViewer]->ShowCurve(l_nSelection, l_bCheckState);

	//save the user choice
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	l_pcPlaceIdList->SetCell(l_nSelection, 2, wxString::Format(wxT("%d"), l_bCheckState));

	//update the viewer
	UpdateViewer();

	RefreshExternalWindows();

	p_cEvent.Skip();

}

void SP_DLG_Simulation::OnColourClick(wxGridEvent& p_cEvent)
{
}

void SP_DLG_Simulation::OnLabelDClick(wxGridEvent& p_cEvent)
{

	//Check the table pointer is OK
	//CHECK_POINTER(m_pcTableGrid,return);
	/*
	 if(m_pcGraph->GetName()==SP_DS_COLSPN_CLASS || m_pcGraph->GetName()==SP_DS_COLCPN_CLASS )
	 {
	 SP_MESSAGEBOX(wxT("Currently this option is not implemented"));
	 return;
	 }
	 */
	//Get the current selected row by the user
	/*int l_nSelectedRow=(m_pcTableGrid->GetSelectedRows())[0];

	 if(l_nSelectedRow<0 || l_nSelectedRow>m_pcTableGrid->GetRows())
	 {
	 SP_MESSAGEBOX(wxT("Invalid row selection"));
	 return;
	 }

	 SP_VectorDouble l_pcCurrentRow=m_pcSimulator->GetResultMatrix()[l_nSelectedRow];

	 wxTextEntryDialog l_pcDialog( this, _T("Name of the new marking set:"), wxT("Add marking set"));
	 wxString l_sName= wxT("M-Set ");

	 SP_DS_ColListAttribute* l_pcColList=GetOneMakingAttribute();

	 CHECK_POINTER(l_pcColList,return);

	 l_sName.Append(wxString::Format(wxT("%d"), l_pcColList->GetRowCount()) );

	 l_pcDialog.SetValue(l_sName);

	 int l_nModalResult = l_pcDialog.ShowModal();

	 if (wxID_OK == l_nModalResult)
	 {

	 if (IsMarkingSetNameExist(l_pcDialog.GetValue() ) )
	 {

	 SP_MESSAGEBOX(wxT("The inputed name is already existing."),
	 wxT("Invalid marking set name"),
	 wxOK | wxICON_ERROR);

	 return;

	 }

	 l_sName = l_pcDialog.GetValue();

	 AddNewMarkingSet(l_sName,l_nSelectedRow);

	 //Refresh the sets by reloading them
	 LoadSets();
	 }*/
}

void SP_DLG_Simulation::AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow)
{
	//Get current row result
	SP_VectorDouble l_nCurrentTableRow;

	l_nCurrentTableRow = m_pcMainSimulator->GetResultMatrix()[p_nRow];

	SP_DS_ColListAttribute* l_pcColList;
	SP_ListNode::const_iterator l_itElem;

	SP_VectorDouble::const_iterator l_itCol = l_nCurrentTableRow.begin();

	//Continuous places
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	if (l_pcNodeclass != NULL)
		//Add the new making to each node
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			//Get the ColList attribute
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("MarkingList")));

			//Add new Column to the marking list
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

			l_pcColList->SetCell(l_nNewRow, 0, p_sMarkingSetName);

			wxString l_sValue = wxString::Format(wxT("%g"), *l_itCol);

			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);

			l_itCol++;

		}

	//Discrete places
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	if (l_pcNodeclass != NULL)
		//Add the new making to each node
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			//Get the ColList attribute
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("MarkingList")));

			//Add new Column to the marking list
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

			l_pcColList->SetCell(l_nNewRow, 0, p_sMarkingSetName);

			wxString l_sValue = wxString::Format(wxT("%d"), (long) (*l_itCol));

			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);

			l_itCol++;
		}
}

SP_DS_ColListAttribute* SP_DLG_Simulation::GetOneMakingAttribute()
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	if (l_pcNodeclass == NULL)
		l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();

	CHECK_POINTER(l_pcNode, return NULL);

	return dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
}
bool SP_DLG_Simulation::IsMarkingSetNameExist(const wxString& p_sName)
{

	SP_DS_ColListAttribute* l_pcColList = GetOneMakingAttribute();

	CHECK_POINTER(l_pcColList, return true);

	for (unsigned long i = 0l; i < l_pcColList->GetRowCount(); i++)
	{
		if (l_pcColList->GetCell(i, 0) == p_sName)
			return true;
	}

	return false;

}

void SP_DLG_Simulation::OnModifyConstantSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}



void SP_DLG_Simulation::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	//ask the user if he wants to close the external windows
	if (m_pcExternalWindows.size() != 0)
	{
		int l_nAnswer = SP_MESSAGEBOX(wxT("Would you like to close external view windows?"), wxT("Questions"), wxYES_NO, this);

		if (l_nAnswer == wxYES)
		{
			std::list<wxWindow*>::iterator l_itWindow;

			for (l_itWindow = m_pcExternalWindows.begin(); l_itWindow != m_pcExternalWindows.end(); l_itWindow++)
			{
				(*l_itWindow)->Destroy();
			}

			//clear pointers
			m_pcExternalWindows.clear();
		}
	}

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

void SP_DLG_Simulation::OnCloseWindow(wxCloseEvent& p_cEvent)
{
	Destroy();  // you may also do:  event.Skip();
                // since the default event handler does call Destroy(), too
}

void SP_DLG_Simulation::OnDirectExportProperties(wxCommandEvent& p_cEvent)
{
	SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT, this, &m_sExportFilename, &m_nExportSpacer, &m_bCompressExact);

	l_pcDlg->ShowModal();

	l_pcDlg->Destroy();
}
void SP_DLG_Simulation::OnSimulationProperties(wxCommandEvent& p_cEvent)
{

	if (m_pcMainSimulator == NULL || m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_SimulationProperties* l_pcDlg;

	l_pcDlg = new SP_DLG_SimulationProperties(m_pcMainSimulator, NULL);

	l_pcDlg->ShowModal();

	l_pcDlg->Destroy();

	CHECK_POINTER(m_pcMainSimulator->GetSimulatorOptions(),return);
	spsim::Property* l_pcRefershRate=m_pcMainSimulator->GetSimulatorOptions()->GetOption(wxT("Refreshrate"));

	CHECK_POINTER(l_pcRefershRate,return);

	m_nRefreshRate=l_pcRefershRate->GetValuelong();
}

wxColour SP_DLG_Simulation::GetNodeColour(SP_DS_Node* p_pcNode)
{
	wxColour l_Return = *wxWHITE;

	//if Place has other Brush then wxWHITE, its used in Plot
	long l_nNet = p_pcNode->GetNetnumber();
	SP_GR_Node* l_pcGrNode = dynamic_cast<SP_GR_Node*>(p_pcNode->GetGraphicInSubnet(l_nNet));
	if (l_pcGrNode)
	{
		l_Return = l_pcGrNode->GetBrush()->GetColour();
	}

	return l_Return;
}

void SP_DLG_Simulation::OnPlotZoomH_In(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x * 1.5, m_pcPlotCtrl->GetZoom().m_y), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomH_Out(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x / 1.5, m_pcPlotCtrl->GetZoom().m_y), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomV_In(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x, m_pcPlotCtrl->GetZoom().m_y / 1.5), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomV_Out(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x, m_pcPlotCtrl->GetZoom().m_y * 1.5), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomHV_In(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x * 1.5, m_pcPlotCtrl->GetZoom().m_y * 1.5), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomHV_Out(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 m_pcPlotCtrl->SetZoom(wxPoint2DDouble(m_pcPlotCtrl->GetZoom().m_x / 1.5, m_pcPlotCtrl->GetZoom().m_y / 1.5), true);

	 }*/

}

void SP_DLG_Simulation::OnPlotZoomCentral(wxCommandEvent& p_cEvent)
{

	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 wxRect2DDouble l_pRect( -0.01 * m_nResultPointCount
	 * m_pcSimulator->GetIntervalSize(), m_nPlotYMax * -0.01, 1.01
	 * m_nResultPointCount * m_pcSimulator->GetIntervalSize(),
	 m_nPlotYMax * 1.15);
	 m_pcPlotCtrl->SetViewRect(l_pRect);

	 }*/

}

void SP_DLG_Simulation::OnPlotSaveArea(wxCommandEvent& p_cEvent)
{
	/*if (m_pcCurrentTablePlotType == SP_TABLE_PLOT_TYPE_PLOT)
	 {

	 if (m_pcCurrentTablePlot)
	 {

	 wxRect2DDouble l_pRect = m_pcPlotCtrl->GetViewRect();

	 double l_nXMin = l_pRect.m_x;
	 double l_nYMin = l_pRect.m_y;
	 double l_nXMax = l_pRect.GetRight();
	 double l_nYMax = l_pRect.GetBottom();

	 wxPoint2DDouble point = m_pcPlotCtrl->GetZoom();

	 dynamic_cast< SP_DS_BoolAttribute*>( m_pcCurrentTablePlot->GetAttribute( wxT("FixedAdjustment") ) )->SetValue( true);

	 m_pcCurrentTablePlot->GetAttribute( wxT("X_Axis_Min") )->SetValueString(wxString::Format(wxT("%f"),	l_nXMin) );
	 m_pcCurrentTablePlot->GetAttribute( wxT("X_Axis_Max") )->SetValueString(wxString::Format(wxT("%f"),	l_nXMax) );
	 m_pcCurrentTablePlot->GetAttribute( wxT("Y_Axis_Min") )->SetValueString(wxString::Format(wxT("%f"),	l_nYMin) );
	 m_pcCurrentTablePlot->GetAttribute( wxT("Y_Axis_Max") )->SetValueString(wxString::Format(wxT("%f"),	l_nYMax) );

	 }

	 }*/
}

wxBitmap SP_DLG_Simulation::GetEnlargeBitmap()
{
#if defined(__WXMSW__) || defined(__WXPM__)

	return wxBitmap( wxT("plot_enl_bmp"), wxBITMAP_TYPE_RESOURCE );

#else

	return wxBitmap(plot_enl_xpm);

#endif
}

wxBitmap SP_DLG_Simulation::GetShrinkBitmap()
{
#if defined(__WXMSW__) || defined(__WXPM__)

	return wxBitmap( wxT("plot_shr_bmp"), wxBITMAP_TYPE_RESOURCE );

#else

	return wxBitmap(plot_shr_xpm);

#endif
}

wxBitmap SP_DLG_Simulation::GetZoomInBitmap()
{
#if defined(__WXMSW__) || defined(__WXPM__)

	return wxBitmap( wxT("plot_zin_bmp"), wxBITMAP_TYPE_RESOURCE );

#else

	return wxBitmap(plot_zin_xpm);

#endif
}

wxBitmap SP_DLG_Simulation::GetZoomOutBitmap()
{
#if defined(__WXMSW__) || defined(__WXPM__)

	return wxBitmap( wxT("plot_zot_bmp"), wxBITMAP_TYPE_RESOURCE );

#else

	return wxBitmap(plot_zot_xpm);

#endif
}

wxString SP_DLG_Simulation::GetSpacer(int p_nSpacer)
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

wxString SP_DLG_Simulation::GetCurveName(long p_nCurveIndex)
{

	int l_nIndex = 0;

	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{

		if (l_nIndex == p_nCurveIndex)
		{

			return m_pcPlaceChoiceCheckListBox->GetString(i);

		}

		if (m_pcPlaceChoiceCheckListBox->IsChecked(i))
		{

			l_nIndex++;

		}

	}

	return wxT("");

}

void SP_DLG_Simulation::OpenExportFile()
{
	wxString l_sExportFilename = m_sExportFilename.BeforeLast('.');

	if (l_sExportFilename == m_pcGraph->GetParentDoc()->GetFilename().BeforeLast('.'))
	{
		l_sExportFilename << wxT("(")
		<< wxDateTime::Now().Format(wxT("%Y-%m-%dT%H:%M:%S%Z")) << wxT(")");
	}
	l_sExportFilename << wxT(".csv");

	m_pcExportFileOutputStream = new wxFileOutputStream(l_sExportFilename);
	if (!m_pcExportFileOutputStream->IsOk())
	{
		m_pcExportFileOutputStream = new wxFileOutputStream(m_sExportFilename);

		if (!m_pcExportFileOutputStream->IsOk())
		{
			return;
		}
	}

	m_pcExportBufferdOutputStream = new wxBufferedOutputStream(*m_pcExportFileOutputStream);
	m_pcExport = new wxTextOutputStream(*m_pcExportBufferdOutputStream);

	wxString l_sName;
	wxString l_sOutput = wxT("Time");
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));

	for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
	{
		wxString l_sCheckState = l_pcPlaceIdList->GetCell(i, 2);
		if (l_sCheckState == wxT("1"))
		{
			l_sName = l_pcPlaceIdList->GetCell(i, 6);
			l_sOutput << l_sSpacer << l_sName;
		}
	}

	(*m_pcExport) << l_sOutput << endl;
}

void SP_DLG_Simulation::CloseExportFile()
{
	if (m_pcCurrentTablePlot)
	{
		SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
		wxString l_sElementType = l_pcAttribute->GetValueString();

		SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));

		unsigned l_nChecked = 0;
		for (unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
		{
			wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n, 2);
			if (l_sCheckState == wxT("1"))
			{
				++l_nChecked;
			}
		}
		wxString l_sMsg = wxT("\n        You exported ");

		l_sMsg << l_nChecked << wxT("/") << l_pcCurveInfoList->GetRowCount() << wxT(" ") << l_sElementType;
		l_sMsg << wxT("s.        \n");
		SP_MESSAGEBOX(l_sMsg, wxT("Export Information"));
	}
	wxDELETE(m_pcExport);
	wxDELETE(m_pcExportBufferdOutputStream);
	wxDELETE(m_pcExportFileOutputStream);
}
void SP_DLG_Simulation::OnExportToCSV(wxCommandEvent& p_cEvent)
{
	wxString l_sFilename = m_sExportFilename;
	bool l_bCompressExact = false;
	SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, this, &l_sFilename, &m_nExportSpacer, &l_bCompressExact);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		wxString l_sBackupFilename = m_sExportFilename;
		m_sExportFilename = l_sFilename;

		OpenExportFile();
		DirectExportToCSV();
		CloseExportFile();

		m_sExportFilename = l_sBackupFilename;
	}

	l_pcDlg->Destroy();
}

void SP_DLG_Simulation::LoadResults()
{
	//Update the x axis values first
	UpdateXAxisValues();

	//update the current viewer
	UpdateViewer();

	RefreshExternalWindows();
}

void SP_DLG_Simulation::OnLoadData(wxCommandEvent& p_cEvent)
{
	wxString l_sFilename = wxFileSelector(wxT("Choose a file to open"));
	if (!l_sFilename.empty())
	{
		SP_ImportCSV* l_pcImport = new SP_ImportCSV();
		InitializeSimulator();
		if (l_pcImport->ReadFile(l_sFilename, m_pcMainSimulator))
		{
			m_pcIntervalStartTextCtrl->SetValue(wxString() << l_pcImport->GetIntervalStart());
			m_pcIntervalEndTextCtrl->SetValue(wxString() << l_pcImport->GetIntervalEnd());
			m_pcResultPointCountTextCtrl->SetValue(wxString() << l_pcImport->GetOutputStepCount());
			//Update();
			InitializeSimulator();
			m_pcMainSimulator->SetGeneratedResultPointsCount(l_pcImport->GetOutputStepCount());
			if (l_pcImport->GetClassName() == wxT("Place"))
			{
				m_pcMainSimulator->GetResultMatrix() = l_pcImport->GetResults();
			}
			else
			{
				m_pcMainSimulator->GetRateResultMatrix() = l_pcImport->GetResults();
			}
			LoadResults();
		}

		wxDELETE(l_pcImport);
	}

}

SP_DLG_Simulation::~SP_DLG_Simulation()
{
	//Save the current view
	SaveCurrentView();

	wxDELETE(m_pcMainSimulator);

	wxDELETEA(m_apColours);

	wxDELETE(m_pcExport);
	wxDELETE(m_pcExportBufferdOutputStream);
	wxDELETE(m_pcExportFileOutputStream);

	//delete the viewers
	for (unsigned int l_nViewer = 0; l_nViewer < m_apcResultViewers.size(); l_nViewer++)
	{
		wxDELETE(m_apcResultViewers[l_nViewer]);
	}

	//delete the timer
	wxDELETE(m_pcTimer);

	std::list<wxWindow*>::iterator l_itWindow;
	//set the pointer of external pointer to NULL
	for (l_itWindow = m_pcExternalWindows.begin(); l_itWindow != m_pcExternalWindows.end(); l_itWindow++)
	{
		dynamic_cast<SP_DLG_ShowAllModelView*>((*l_itWindow))->RemoveExternalWindowsPointer();
	}
}

void SP_DLG_Simulation::OnPrint(wxCommandEvent& p_cEvent)
{
	/*wxString l_sBuffer(wxT(""));
	 wxString l_sText(wxT(""));
	 wxString l_sFilename(SP_Core::Instance()->GetRootDocument()->GetUserReadableName());
	 wxPrintData l_pcPrintData;
	 int l_nResolution=600;
	 int l_nPen = 5;
	 int l_nOffset = 200;
	 int l_nFontSize = 10;
	 l_pcPrintData.SetPaperId(wxPAPER_A4);
	 wxAlignment l_nAlign = wxALIGN_RIGHT;

	 #ifdef __WXMSW__
	 wxPrintDialog l_cPrintDialog(NULL,&l_pcPrintData);
	 if(wxID_CANCEL == l_cPrintDialog.ShowModal())
	 return;
	 wxDC *l_pcDC = l_cPrintDialog.GetPrintDC();
	 l_pcPrintData = l_cPrintDialog.GetPrintData();
	 l_nResolution = l_pcDC->GetPPI().GetX();
	 l_nFontSize = 7*l_nResolution/72;
	 #else
	 l_sFilename = wxFileSelector(wxT("Save..."), wxT(""), l_sFilename, wxT("eps"), wxT("*.eps"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	 if ( l_sFilename.empty() )
	 return;

	 l_pcPrintData.SetFilename(l_sFilename);
	 l_pcPrintData.SetPrintMode(wxPRINT_MODE_FILE);
	 wxPostScriptDC* l_pcDC = new wxPostScriptDC(l_pcPrintData);
	 #if wxMAC_USE_CORE_GRAPHICS
	 // we need at least a measuring context because people start measuring before a page
	 // gets printed at all
	 l_pcDC->SetGraphicsContext( wxGraphicsContext::Create());
	 #endif
	 l_nPen=1;
	 l_nFontSize = 10;
	 l_nOffset=20;
	 l_nResolution=72;
	 l_pcDC->SetResolution(l_nResolution);
	 l_nAlign = wxALIGN_CENTER;

	 #endif

	 int l_nCurrentX = 0, l_nCurrentY = 0;
	 wxSize l_cSize = l_pcDC->GetSize();
	 wxRect l_cRect(l_cSize);
	 wxRect l_cCurrentRect(l_cRect.x+l_nOffset, l_cRect.y+(2*l_nOffset),
	 l_cRect.GetWidth()-(2*l_nOffset), l_cRect.GetHeight()*3/7);

	 l_pcDC->StartDoc(l_sFilename);

	 m_pcPlotCtrl->SetPlotTitle(wxT("Continuous Result: ")+ SP_Core::Instance()->GetRootDocument()->GetUserReadableName());
	 m_pcPlotCtrl->SetXAxisLabel(wxT("Time"));
	 m_pcPlotCtrl->SetYAxisLabel(wxT("Value"));
	 m_pcPlotCtrl->SetShowPlotTitle(true);
	 m_pcPlotCtrl->SetShowXAxisLabel(true);
	 m_pcPlotCtrl->SetShowYAxisLabel(true);
	 m_pcPlotCtrl->SetGridColour(*wxBLACK);

	 //	m_pcPlotCtrl->SetShowKey(false);

	 for(long i=0; i<m_pcPlotCtrl->GetCurveCount(); i++)
	 {
	 wxPlotData *l_pcData = m_pcPlotCtrl->GetDataCurve(i);
	 wxBitmap l_cSymbol = l_pcData->GetSymbol(wxPLOTPEN_ACTIVE);

	 wxGenericPen l_cPen = l_pcData->GetPen(wxPLOTPEN_NORMAL);
	 l_cPen.SetWidth(l_nPen);

	 l_pcData->SetPen(wxPLOTPEN_NORMAL, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_ACTIVE, l_cPen);
	 l_pcData->SetPen(wxPLOTPEN_SELECTED, l_cPen);

	 }

	 m_pcPlotCtrl->DrawWholePlot(l_pcDC, l_cCurrentRect, l_nResolution);
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

	 SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	 int l_nMaxX = 0, l_nMaxY = 0;

	 l_nMaxX = l_nMaxX<l_nCurrentX?l_nCurrentX:l_nMaxX;
	 l_nMaxY = l_nMaxY<l_nCurrentY?l_nCurrentY:l_nMaxY;

	 wxGrid l_cGrid(this,wxID_ANY, wxPoint(0,0), l_cCurrentRect.GetSize());
	 l_cGrid.Show(true);
	 l_cGrid.CreateGrid(1,9);
	 l_cGrid.SetRowLabelSize(0);
	 l_cGrid.SetColLabelSize(0);
	 l_cGrid.SetDefaultCellOverflow(true);
	 wxFont l_cFont = l_cGrid.GetDefaultCellFont();
	 l_cFont.SetPointSize(l_nFontSize);
	 l_cGrid.SetDefaultCellFont(l_cFont);


	 SP_ListNode* l_lNodes = l_pcGraph->GetNodeclass(wxT("Place"))->GetElements();
	 SP_ListNode::const_iterator itN;
	 if((l_cGrid.GetNumberRows()-1) < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-(l_cGrid.GetNumberRows()-1));
	 }
	 int l_nCol = 0, l_nRow = 0;
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
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetRow(m_pcMarkingSetComboBox->GetSelection())[1]);
	 }

	 l_lNodes = l_pcGraph->GetNodeclass(wxT("Transition"))->GetElements();
	 if((l_cGrid.GetNumberRows()-1) < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-(l_cGrid.GetNumberRows()-1));
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
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetRow(m_pcFunctionSetComboBox->GetSelection())[1]);
	 }

	 l_lNodes = l_pcGraph->GetNodeclass(wxT("Parameter"))->GetElements();
	 if((l_cGrid.GetNumberRows()-1) < l_lNodes->size())
	 {
	 l_cGrid.AppendRows(l_lNodes->size()-(l_cGrid.GetNumberRows()-1));
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
	 l_cGrid.SetCellValue(l_nRow, l_nCol, l_pcColAttr->GetRow(m_pcParameterSetComboBox->GetSelection())[1]);
	 }

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

	 l_pcDC->EndDoc();
	 wxDELETE(l_pcDC);*/
}
void SP_DLG_Simulation::InitializeViews()
{
	wxString l_sName;
	SP_DS_Metadataclass* l_pcMetadataclass;
	long l_nPos = 0;

	const SP_ListMetadata* l_pcViewList;

	//remove old views
	m_pcViewBox->Clear();

	//get plot list. We ignore tables in old implementation
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return);

	l_pcViewList = l_pcMetadataclass->GetElements();

	CHECK_POINTER(l_pcViewList, return);

	if (l_pcViewList->size() == 0)
	{
		m_pcCurrentTablePlot = CreateNewView(wxT("Default View"));
		CHECK_POINTER(m_pcCurrentTablePlot, return);

		//make it the current view
		SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));
	}

	l_nPos = 0;

	//read all views
	for (SP_ListMetadata::const_iterator l_itElem = l_pcViewList->begin(); l_itElem != l_pcViewList->end(); ++l_itElem)
	{
		l_sName = (*l_itElem)->GetAttribute(wxT("Name"))->GetValueString();

		m_pcViewBox->Append(l_sName);

		wxString l_sCurrent = GetViewAttributeValue((*l_itElem), wxT("IsCurrent"));

		if (l_sCurrent.IsSameAs(wxT("1")))
		{
			m_pcCurrentTablePlot = (*l_itElem);

			m_pcViewBox->SetSelection(l_nPos);
		}

		//check if the view is empty, initialize it with the default values
		SP_DS_Attribute* l_pcAttribute = (*l_itElem)->GetAttribute(wxT("CurveInfo"));

		CHECK_POINTER(l_pcAttribute, return);

		SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

		if (l_pcPlaceIdList->GetRowCount() == 0)
		{
			InitializeEmptyView((*l_itElem));
		}

		l_nPos++;
	}

	//check if we set the current view
	if (m_pcCurrentTablePlot == NULL)
	{
		m_pcCurrentTablePlot = l_pcViewList->front();

		SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

		m_pcViewBox->SetSelection(0);
	}

	//change the current viewer type
	ChangeCurrentViewerType();
}

void SP_DLG_Simulation::ChangeCurrentViewerType()
{
	unsigned int l_nNewCurrentView = 0;

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	wxString l_sViewerType = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("ViewerType"));

	//set the current viewer type
	if (l_sViewerType.IsSameAs(wxT("Tabular")))
	{
		l_nNewCurrentView = 0;
	}
	else
		if (l_sViewerType.IsSameAs(wxT("xyPlot")))
		{
			l_nNewCurrentView = 1;
		}
		else
			if (l_sViewerType.IsSameAs(wxT("Histogram")))
			{
				l_nNewCurrentView = 2;
			}
			else
			{
				l_nNewCurrentView = 1;
			}

	if (l_nNewCurrentView < m_apcResultViewers.size())
	{
		//Destroy old viewer
		m_apcResultViewers[m_nCurrentViewer]->Destroy();

		//Get the index of the new selected viewer
		m_nCurrentViewer = l_nNewCurrentView;

		m_apcResultViewers[m_nCurrentViewer]->Create();
	}

	//Change the box selection
	m_pcOutputViewerType->SetSelection(m_nCurrentViewer);

	RefreshExternalWindows();

}
void SP_DLG_Simulation::OnChangeResultViewer(wxCommandEvent & envent)
{
	//save current view
	SaveCurrentView();

	//Change the current view viewer
	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("ViewerType"));
	CHECK_POINTER(l_pcAttribute, return);

	unsigned int l_nSelectedViewer = m_pcOutputViewerType->GetSelection();

	l_pcAttribute->SetValueString(GetCurrentViewerType(l_nSelectedViewer));

	ChangeCurrentViewerType();

	LoadData(false);

	//update viewer matrix
	UpdateViewer();
}
void SP_DLG_Simulation::OnEditViewerProperties(wxCommandEvent& p_cEvent)
{
	m_bCanChangeCurrentView = false;

	DoEditViewerProperties();

	m_bCanChangeCurrentView = true;
}

void SP_DLG_Simulation::DoEditViewerProperties()
{
	SP_DLG_ResultViewerProperties* l_pcViewerProperties = new SP_DLG_ResultViewerProperties(m_apcResultViewers[m_nCurrentViewer], this);

	if (l_pcViewerProperties->ShowModal() == wxID_OK)
	{
		UpdateViewer();

		SaveCurrentView();

		//refresh
		RefreshExternalWindows();
	}

	l_pcViewerProperties->Destroy();
}

void SP_DLG_Simulation::OnItemDClick(wxCommandEvent& p_cEvent)
{

	if (p_cEvent.GetSelection() < 0 && (unsigned int) p_cEvent.GetSelection() >= m_pcPlaceChoiceCheckListBox->GetCount())
	{
		return;
	}

	SP_DS_ResultViewer* l_pcCurrentResultViewer = m_apcResultViewers[m_nCurrentViewer];

	m_bCanChangeCurrentView = false;

	CHECK_POINTER(l_pcCurrentResultViewer, return);

	int l_nCurveIndex = m_pcPlaceChoiceCheckListBox->GetSelection();

	wxString l_sColor = l_pcCurrentResultViewer->GetCurveColor(l_nCurveIndex);
	int l_nLineWidth = l_pcCurrentResultViewer->GetCurveLineWidth(l_nCurveIndex);
	int l_nLineStyle = l_pcCurrentResultViewer->GetCurveLineStyle(l_nCurveIndex);

	SP_DLG_ChangeCurveAttributes l_dial(this, l_sColor, l_nLineWidth, l_nLineStyle);

	if (l_dial.ShowModal() == wxID_OK)
	{
		l_pcCurrentResultViewer->SetCurveLineWidth(l_nCurveIndex, l_dial.GetLineWidth());

		l_pcCurrentResultViewer->SetCurveLineStyle(l_nCurveIndex, l_dial.GetLineStyle());

		wxColour l_nColor = l_dial.GetLineColor();

		l_pcCurrentResultViewer->SetCurveColor(l_nCurveIndex, l_nColor.GetAsString(wxC2S_HTML_SYNTAX));

		//Update the current viewer
		UpdateViewer();
	}

	m_bCanChangeCurrentView = true;
}

void SP_DLG_Simulation::OnShowEditModelViewMenu(wxCommandEvent& p_cEvent)
{
	wxMenu *l_pcEditModelViewsMenu = new wxMenu;
	wxString l_sCurrentViewerType = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("ViewerType"));

	l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_NEW_MODEL_VIEW, wxT("New"));
	l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_EDIT_TABLE_PLOT, wxT("Edit"));
	l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_REMOVE_MODEL_VIEW, wxT("Remove"));
	l_pcEditModelViewsMenu->AppendSeparator();

	l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_EXPORT_CSV, wxT("CSV export"));
	if (l_sCurrentViewerType.IsSameAs(wxT("xyPlot")) || l_sCurrentViewerType.IsSameAs(wxT("Histogram")))
	{
		l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_VIEW_IMAGE_EXPORT, wxT("Image export"));
	}
	if (l_sCurrentViewerType.IsSameAs(wxT("xyPlot")))
	{
		l_pcEditModelViewsMenu->AppendSeparator();

		l_pcEditModelViewsMenu->Append(SP_ID_BUTTON_EDIT_VIEWER_SELECT_X_AXIS, wxT("change X axis"));
	}
	l_pcEditModelViewsMenu->AppendSeparator();
	l_pcEditModelViewsMenu->Append(SP_ID_MENU_OPEN_CURRENT_VIEW_SEPARATELY, wxT("Open separately"));
	l_pcEditModelViewsMenu->Append(SP_ID_MENU_OPEN_ALL_SEPARATELY, wxT("Open all separately"));

	PopupMenu(l_pcEditModelViewsMenu);
}

void SP_DLG_Simulation::OnSelectXAxis(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	m_bCanChangeCurrentView = false;

	wxString l_sCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariableName"));

	SP_VectorString l_asTimeVector;
	l_asTimeVector.push_back(wxT("Simulation Time"));
	l_asTimeVector.push_back(wxT("Run Time"));
	m_pcXAxisChoices[wxT("Time")] = &l_asTimeVector;

	SP_DLG_SelectXAxisVariable* l_pcDialog = new SP_DLG_SelectXAxisVariable(this, m_pcXAxisChoices, l_sCategory, l_sSubCategory);

	if (l_pcDialog->ShowModal() == wxID_OK)
	{
		SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"), l_pcDialog->GetSelectedCategory());
		SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariableName"), l_pcDialog->GetSelectedSubCategory());

		SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisTitle"), l_pcDialog->GetSelectedSubCategory());

		m_apcResultViewers[m_nCurrentViewer]->LoadViewFromSnoopyFormat(m_pcCurrentTablePlot);

		UpdateXAxisValues();

		UpdateViewer();
	}

	l_pcDialog->Destroy();

	m_bCanChangeCurrentView = true;
}

void SP_DLG_Simulation::OnOpenCurrentViewSeparately(wxCommandEvent& p_cEvent)
{
	OpenViewInSeparateWindow(m_pcCurrentTablePlot);
}

void SP_DLG_Simulation::OpenViewInSeparateWindow(SP_DS_Metadata* p_pcModelView)
{
	SP_DLG_ShowAllModelView* l_pcShowModelViewsDlg;

	std::list<wxWindow*>::iterator l_itWindow;

	//check if a view with the same name is already opened
	for (l_itWindow = m_pcExternalWindows.begin(); l_itWindow != m_pcExternalWindows.end(); l_itWindow++)
	{
		if (p_pcModelView == (dynamic_cast<SP_DLG_ShowAllModelView*>(*l_itWindow))->GetModelView())
		{
			SP_LOGERROR( wxT("This view(") + GetViewAttributeValue(p_pcModelView, wxT("Name")) + wxT(") is already opened.  A view Can't be opened twice"));

			return;
		}
	}

	l_pcShowModelViewsDlg = new SP_DLG_ShowAllModelView(this, p_pcModelView);

	// check if the window is created correctly
	if (l_pcShowModelViewsDlg != NULL)
	{
		m_pcExternalWindows.push_back(l_pcShowModelViewsDlg);

		l_pcShowModelViewsDlg->Show(true);
	}

}

void SP_DLG_Simulation::OnOpenAllViewsSeparately(wxCommandEvent& p_cEvent)
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	const SP_ListMetadata* l_pcViewList;

	//get plot list. We ignore tables in old implementation
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return);

	l_pcViewList = l_pcMetadataclass->GetElements();

	CHECK_POINTER(l_pcViewList, return);

	//read all views
	for (SP_ListMetadata::const_iterator l_itElem = l_pcViewList->begin(); l_itElem != l_pcViewList->end(); ++l_itElem)
	{
		OpenViewInSeparateWindow((*l_itElem));
	}

}
void SP_DLG_Simulation::RefreshExternalWindows()
{
	std::list<wxWindow*>::iterator l_itWindow;

	for (l_itWindow = m_pcExternalWindows.begin(); l_itWindow != m_pcExternalWindows.end(); l_itWindow++)
	{
		if ((*l_itWindow) != NULL)
		{
			(dynamic_cast<SP_DLG_ShowAllModelView*>((*l_itWindow)))->RefreshWindow();
		}
	}
}

void SP_DLG_Simulation::OnNewModelView(wxCommandEvent& p_cEvent)
{
	wxTextEntryDialog* l_pcModelViewDlg;
	wxString l_sViewName;

	int l_nReturnCode = 0;

	l_pcModelViewDlg = new wxTextEntryDialog(this, wxT("Enter view name:"));

	//ask the user to enter view name until he gives a valid
	//name or give up
	while ((l_nReturnCode = l_pcModelViewDlg->ShowModal()) != wxID_CANCEL)
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

		//if there is no problem, break
		break;
	}

	//check if the user canceled the dialog
	if (l_nReturnCode == wxID_CANCEL)
	{
		return;
	}

	//Save the current view
	SaveCurrentView();

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("0"));

	//set the new metadata as the current one
	m_pcCurrentTablePlot = CreateNewView(l_sViewName, true);

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

	//Add the new view name to the existing views box
	m_pcViewBox->Append(l_sViewName);

	//select the new view as the current one
	m_pcViewBox->SetStringSelection(l_sViewName);

	LoadData(false);

	//destroy the dialog
	l_pcModelViewDlg->Destroy();

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

SP_DS_Metadata* SP_DLG_Simulation::CreateNewView(const wxString& p_sName, bool p_bCloneCurrent)
{
	SP_DS_Metadataclass* l_pcMetadataclass;

	CHECK_POINTER(m_pcGraph, return NULL);

	//get the metadata class
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return NULL);

	//Add a new meta-data element
	SP_DS_Metadata* l_pcNewMetadata = NULL;
	if (p_bCloneCurrent == true)
	{
		l_pcNewMetadata = m_pcCurrentTablePlot->Clone();

		l_pcMetadataclass->AddElement(l_pcNewMetadata);
	}
	else
	{
		l_pcNewMetadata = l_pcMetadataclass->NewElement(1);
	}
	CHECK_POINTER(l_pcNewMetadata, return NULL);

	l_pcNewMetadata->GetAttribute(wxT("Name"))->SetValueString(p_sName);

	(l_pcNewMetadata->GetAttribute(wxT("ViewTitle")))->SetValueString(p_sName);

	return l_pcNewMetadata;
}

bool SP_DLG_Simulation::IsViewNameExist(const wxString& p_sViewName)
{
	SP_DS_Metadata* l_pcView = NULL;

	l_pcView = FindView(p_sViewName);

	if (l_pcView != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void SP_DLG_Simulation::OnChangeModelView(wxCommandEvent& p_cEvent)
{
	SP_DS_Metadata* l_pcView;

	// get a pointer to the view
	l_pcView = FindView(m_pcViewBox->GetStringSelection());

	ChangeCurrentView(l_pcView);
}

void SP_DLG_Simulation::ChangeCurrentView(SP_DS_Metadata* p_pcView)
{
	//Save the current view
	SaveCurrentView();

	CHECK_POINTER(p_pcView, return);

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("0"));

	m_pcCurrentTablePlot = p_pcView;

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

	//change the current view
	ChangeCurrentViewerType();

	//call loadData to add the curves to the viewer
	LoadData(false);
}

void SP_DLG_Simulation::ChangeViewTo(SP_DS_Metadata* p_pcView)
{
	if (m_bCanChangeCurrentView)
	{
		ChangeCurrentView(p_pcView);

		wxString l_sViewName = GetViewAttributeValue(p_pcView, wxT("Name"));

		m_pcViewBox->SetStringSelection(l_sViewName);
	}

}

SP_DS_Metadata* SP_DLG_Simulation::FindView(const wxString& p_sViewName)
{
	wxString l_sName;
	SP_DS_Metadataclass* l_pcMetadataclass;

	const SP_ListMetadata* l_pcViewList;

	//get plot list. We ignore tables in old implementation
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return NULL);

	l_pcViewList = l_pcMetadataclass->GetElements();

	CHECK_POINTER(l_pcViewList, return NULL);

	//read all views
	for (SP_DS_Metadata* l_pcMeta : *l_pcViewList)
	{
		l_sName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();

		//TODO: select the current view not the main
		if (l_sName.IsSameAs(p_sViewName))
		{
			return l_pcMeta;
		}
	}

	//if we didn't find it, return NULL
	return NULL;
}

void SP_DLG_Simulation::OnRemoveModelView(wxCommandEvent& event)
{
	SP_DS_Metadataclass* l_pcMetadataclass;

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

	CHECK_POINTER(l_pcMetadataclass, return);

	//the user can not delete the main table
	if (l_pcMetadataclass->GetElements()->size() == 1)
	{
		SP_MESSAGEBOX(wxT("Can not remove default view."), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}

	l_pcMetadataclass->RemoveElement(m_pcCurrentTablePlot);

	m_pcCurrentTablePlot = NULL;

	InitializeViews();

	//reload the curves
	LoadData(true);
}

void SP_DLG_Simulation::OnEditOtherView(wxCommandEvent& p_cEvent)
{
	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	SaveCurrentView();

	SP_DLG_PlacesSelection* l_pcDlg = new SP_DLG_PlacesSelection(l_sElementType, m_pcCurrentTablePlot, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		InitializeViews();

		LoadData(true);
	}
	l_pcDlg->Destroy();
}

void SP_DLG_Simulation::SaveCurrentView()
{

	//get the current viewer type
	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("ViewerType"));
	CHECK_POINTER(l_pcAttribute, return);

	l_pcAttribute->SetValueString(GetCurrentViewerType(m_nCurrentViewer));

	//Save the user selection
	SaveSelectedCurves();

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	m_apcResultViewers[m_nCurrentViewer]->SaveViewToSnoopyFormat(m_pcCurrentTablePlot);

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}
wxString SP_DLG_Simulation::GetCurrentViewerType(const unsigned int& p_nType)
{
	switch (p_nType)
	{
	case 0:
		return wxT("Tabular");
	case 1:
		return wxT("xyPlot");
	case 2:
		return wxT("Histogram");
	default:
		return wxT("xyPlot");
	}
}

void SP_DLG_Simulation::LoadViewProperties()
{
	//load selected curves options
	LoadSelectedCurves();

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	m_apcResultViewers[m_nCurrentViewer]->LoadViewFromSnoopyFormat(m_pcCurrentTablePlot);

	UpdateViewer();
}

void SP_DLG_Simulation::SetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName, const wxString& p_sValue)
{
	CHECK_POINTER(p_pcView, return);
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return);

	l_pcAttribute->SetValueString(p_sValue);

}
wxString SP_DLG_Simulation::GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}
void SP_DLG_Simulation::OnImageExport(wxCommandEvent &event)
{

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	wxString l_sCurrentViewerType = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("ViewerType"));

	if (l_sCurrentViewerType.IsSameAs(wxT("xyPlot")) || l_sCurrentViewerType.IsSameAs(wxT("Histogram")))
	{

		SP_DS_PlotViewer* l_pcPlotResultViewer = dynamic_cast<SP_DS_PlotViewer*>(m_apcResultViewers[m_nCurrentViewer]);

		wxString l_sDefaultName = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("Name"));

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
void SP_DLG_Simulation::OnSelectClearAllItems(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcCurveInfo = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

	m_apcResultViewers[m_nCurrentViewer]->ClearShownCurves();

	if (m_pcSelectClearAllChkBox->Get3StateValue() == wxCHK_CHECKED)
	{
		for (unsigned int l_nCurve = 0; l_nCurve < l_pcCurveInfo->GetRowCount(); l_nCurve++)
		{
			l_pcCurveInfo->SetCell(l_nCurve, 2, wxT("1"));

			m_pcPlaceChoiceCheckListBox->Check(l_nCurve, true);

			m_apcResultViewers[m_nCurrentViewer]->ShowCurve(l_nCurve, true);
		}
	}
	else
		if (m_pcSelectClearAllChkBox->Get3StateValue() == wxCHK_UNCHECKED)
		{
			for (unsigned int l_nCurve = 0; l_nCurve < l_pcCurveInfo->GetRowCount(); l_nCurve++)
			{
				l_pcCurveInfo->SetCell(l_nCurve, 2, wxT("0"));

				m_pcPlaceChoiceCheckListBox->Check(l_nCurve, false);

			}

		}

	//Update the current viewer
	UpdateViewer();
}
wxString SP_DLG_Simulation::GetColourString(unsigned int p_nIndex)
{
	wxColour l_defaultColours[] =
	{ wxColour(255, 0, 0), wxColour(0, 255, 0), wxColour(0, 0, 255), wxColour(200, 100, 100), wxColour(200, 200, 100), wxColour(0, 200, 100), wxColour(200, 0, 100), wxColour(200, 200, 0), wxColour(
			200, 200, 200), wxColour(0, 0, 0),
	// TODO: add more colors
			};
	if (p_nIndex >= WXSIZEOF(l_defaultColours))
	{
		p_nIndex = p_nIndex % WXSIZEOF(l_defaultColours);
	}
	return l_defaultColours[p_nIndex].GetAsString(wxC2S_HTML_SYNTAX);
}

void SP_DLG_Simulation::AddGuiOption2Simulator()
{
	//display them in the same dialog
	m_pcMainSimulator->GetSimulatorOptions()->InsertNewOption(0, wxT("InBetweenVisualization"), wxT("1"), wxT("Visualize in between results"), spsim::GUI_TYPE_CHECKBOX);
	m_pcMainSimulator->GetSimulatorOptions()->InsertNewOption(1, wxT("Refreshrate"), wxT("5000"), wxT("Refresh rate(ms)"), spsim::GUI_TYPE_TXTBOX);
}

int SP_DLG_Simulation::LoadSimulatorProperties()
{
	int l_nSimulatorType = 0;
	SP_DS_ColListAttribute* l_pcSimulatorProperties;
	int l_nCount = 0;

	//Retrieve simulator name
	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	if (l_pcSimProp)
	{
		l_pcAttr = l_pcSimProp->GetAttribute(wxT("simulator"));
	}

	//if the net has no simulator attribute, allocate the first simulator type and return
	if (l_pcAttr == NULL)
	{
		m_pcMainSimulator = CreateSimulator(0);

		return 0;
	}

	l_nSimulatorType = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttr))->GetValue();
	m_pcMainSimulator = CreateSimulator(l_nSimulatorType);

	//add add here Snoopy specific setting to the simulator, so that we can
	AddGuiOption2Simulator();

	//load simulator properties
	l_pcAttr = l_pcSimProp->GetAttribute(wxT("simulator properties"));
	CHECK_POINTER(l_pcAttr, return -1);

	l_pcSimulatorProperties = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);

	spsim::VectorProperty* l_pcProperties = m_pcMainSimulator->GetSimulatorOptions()->GetAllOptions();
	spsim::VectorProperty::iterator l_itProperty;

	if (l_pcProperties->size() != l_pcSimulatorProperties->GetRowCount())
	{
		return 0;
	}

	for (l_itProperty = l_pcProperties->begin(); l_itProperty != l_pcProperties->end(); l_itProperty++, l_nCount++)
	{
		//read data from the saved list
		wxString l_sName = l_pcSimulatorProperties->GetCell(l_nCount, 0);
		wxString l_sValue = l_pcSimulatorProperties->GetCell(l_nCount, 1);

		if ((*l_pcProperties)[l_nCount]->GetName() == l_sName)
		{
			(*l_pcProperties)[l_nCount]->SetValue(l_sValue);
		}
	}

	return l_nSimulatorType;
}
void SP_DLG_Simulation::SaveSimulatorProperties()
{
	SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
	SP_DS_Attribute* l_pcAttr = NULL;

	SP_DS_ColListAttribute* l_pcSimulatorProperties;
	int l_nMethodType = GetCureentSelectedSimulator();
	int l_nCount = 0;

	l_pcAttr = l_pcSimProp->GetAttribute(wxT("simulator"));
	CHECK_POINTER(l_pcAttr, return);

	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttr))->SetValue(l_nMethodType);

	//save simulator properties
	l_pcAttr = l_pcSimProp->GetAttribute(wxT("simulator properties"));
	CHECK_POINTER(l_pcAttr, return);

	l_pcSimulatorProperties = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);

	l_pcSimulatorProperties->Clear();

	spsim::VectorProperty* l_pcProperties = m_pcMainSimulator->GetSimulatorOptions()->GetAllOptions();
	spsim::VectorProperty::iterator l_itProperty;

	for (l_itProperty = l_pcProperties->begin(); l_itProperty != l_pcProperties->end(); l_itProperty++, l_nCount++)
	{
		//read data from the saved list
		wxString l_sName = (*l_pcProperties)[l_nCount]->GetName();
		wxString l_sValue = (*l_pcProperties)[l_nCount]->GetValue();

		l_pcSimulatorProperties->AppendEmptyRow();

		l_pcSimulatorProperties->SetCell(l_nCount, 0, l_sName);

		l_pcSimulatorProperties->SetCell(l_nCount, 1, l_sValue);

	}
}

void SP_DLG_Simulation::LoadNodeColours(const wxString& p_nNodeName, SP_VectorString& p_asColours)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	const SP_ListNode* l_pcNodeList;

	SP_ListNode::const_iterator l_itNode;

	l_pcNodeclass = m_pcGraph->GetNodeclass(p_nNodeName);

	CHECK_POINTER(l_pcNodeclass, return);

	l_pcNodeList = l_pcNodeclass->GetElements();

	CHECK_POINTER(l_pcNodeList, return);

	for (l_itNode = l_pcNodeList->begin(); l_itNode != l_pcNodeList->end(); l_itNode++)
	{
		SP_ListGraphic* l_pcGraphics = (*l_itNode)->GetGraphics();

		CHECK_POINTER(l_pcGraphics, return);

		wxColour l_cColour = *wxWHITE;
		for(SP_ListGraphic::const_iterator itG = l_pcGraphics->begin();
				itG != l_pcGraphics->end() && l_cColour == *wxWHITE; ++itG)
		{
			if((*itG)->GetGraphicState() == SP_STATE_NORMAL ||
					(*itG)->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
			{
				l_cColour = (*itG)->GetBrush()->GetColour();
			}
		}

		if (l_cColour != *wxWHITE)
		{
			p_asColours.push_back(l_cColour.GetAsString(wxC2S_HTML_SYNTAX));
		}
		else
		{
			p_asColours.push_back(wxT(""));
		}
	}
}

void SP_DLG_Simulation::UpdateChoices()
{
	m_choices.clear();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(mc)
	{
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata = NULL;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Group"));
			if(l_pcAttr)
			{
				wxString l_sGroup = l_pcAttr->GetValueString();
				m_choices.insert(l_sGroup);
			}
		}
	}
}

void SP_DLG_Simulation::StartSimulation()
{
	DoStartSimulation();
}

