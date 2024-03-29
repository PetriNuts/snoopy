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
#include <wx/statline.h>
#include <wx/regex.h>
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
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"
#include "sp_gui/dialogs/SP_DLG_SelectXAxisVariable.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_PlacesSelection.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_gui/dialogs/SP_DLG_ShowAllModelView.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStSimulationResults.h"
#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNSimultionResults.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNSimulationResults.h"

#include "sp_gui/dialogs/dia_FPN/FuzzyBasics.h" //Added by G.A
#include "sp_gui/dialogs/dia_FPN/TriangularFN.h" //Added by G.A
#include <vector>

#include "sp_gui/dialogs/SP_DLG_BaseSimulation.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"//Added by G.A
#include <wx/busyinfo.h>
#include <wx/fs_zip.h>
#include <wx/zipstrm.h>

#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#include <wx/collpane.h>
#include <wx/statline.h>
#include <wx/regex.h>
#include <iostream>
//#include "sp_gui/widgets/sp_plot/plot_up.xpm"
//#include "sp_gui/widgets/sp_plot/plot_dwn.xpm"
#endif
IMPLEMENT_CLASS(SP_DLG_Simulation, SP_DLG_BaseSimulation )

enum
{

    SP_ID_TREECTRL_TABLE_PLOT_OVERVIEW = SP_ID_LAST_ID + 1,
    SP_ID_BUTTON_NEW_MODEL_VIEW,
    SP_ID_BUTTON_REMOVE_MODEL_VIEW,
    SP_ID_BUTTON_SAVE_TABLE_PLOT,
    SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES,
    SP_ID_GRID_TABLE,
    SP_ID_GRID_PLACE_CHOICE,
    SP_ID_BUTTON_SIMULATION_PROPERTIES,
    SP_ID_BUTTON_START_SIMULATION,
    SP_ID_BUTTON_PLOT_ZOOM_H_IN,
    SP_ID_BUTTON_PLOT_ZOOM_H_OUT,
    SP_ID_BUTTON_PLOT_ZOOM_V_IN,
    SP_ID_BUTTON_PLOT_ZOOM_V_OUT,
    SP_ID_BUTTON_PLOT_ZOOM_HV_IN,
    SP_ID_BUTTON_PLOT_ZOOM_HV_OUT,
    SP_ID_BUTTON_PLOT_SAVE_AREA,
    SP_ID_BUTTON_LOAD_DATA,
    SP_ID_TEXT_PLOT_STATUS,
    SP_ID_BUTTON_PLOT_ZOOM_CENTRAL,
    SP_ID_PLOT_CTRL_DATA,
    SP_ID_PRINT,
    SP_ID_CHOICE_RESULT_MATRIX_TYPE,
    SP_ID_TIMER_UPDATE_DIALOG,
	SP_ID_COLLAPSEPANEL_PROPERTY_SIZER,
	SP_ID_COLLAPSEPANEL_DIRECT_EXPORT_SIZER,
	SP_ID_COLLAPSEPANEL_SETS_SIZER,
	SP_ID_COLLAPSEPANEL_RESULT_SIZER,
	SP_ID_BUTTON_SHOW_SELECTED_VIEWS,
	SP_ID_BUTTON_SHOW_ALL_VIEWS,
	SP_ID_BUTTON_ADD_NEW_VIEW,
	SP_ID_BUTTON_REMOVE_VIEWS,
	SP_ID_BUTTON_RENAME_VIEW,
	SP_ID_LISTBOX_VIEWS,
	SP_ID_BUTTON_MORE_VIEW_OPTIONS
};

BEGIN_EVENT_TABLE( SP_DLG_Simulation, SP_DLG_BaseSimulation )

EVT_BUTTON( SP_ID_BUTTON_START_SIMULATION, SP_DLG_Simulation :: OnStartAbortSimulation )

EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_Simulation :: OnSimulationProperties )

EVT_BUTTON( wxID_CANCEL, SP_DLG_Simulation :: OnDlgCancel )

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

EVT_BUTTON( SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES, SP_DLG_Simulation :: OnDirectExportProperties )
EVT_BUTTON( SP_ID_BUTTON_LOAD_DATA, SP_DLG_Simulation :: OnLoadData )

EVT_BUTTON( SP_ID_BUTTON_MORE_VIEW_OPTIONS, SP_DLG_Simulation :: OnMoreViewOptions )


EVT_BUTTON( SP_ID_PRINT, SP_DLG_Simulation :: OnPrint )


EVT_TIMER( SP_ID_TIMER_UPDATE_DIALOG, SP_DLG_Simulation :: OnTimer)

EVT_COLLAPSIBLEPANE_CHANGED(SP_ID_COLLAPSEPANEL_PROPERTY_SIZER, SP_DLG_Simulation :: OnCollapsePropertySizer)
EVT_COLLAPSIBLEPANE_CHANGED(SP_ID_COLLAPSEPANEL_SETS_SIZER, SP_DLG_Simulation :: OnCollapseSetsSizer)
EVT_COLLAPSIBLEPANE_CHANGED(SP_ID_COLLAPSEPANEL_DIRECT_EXPORT_SIZER, SP_DLG_Simulation :: OnCollapseDirectExportSizer)

EVT_MENU( SP_ID_BUTTON_SHOW_SELECTED_VIEWS, SP_DLG_Simulation :: OnOpenSelectedGraphViews)
EVT_MENU( SP_ID_BUTTON_SHOW_ALL_VIEWS, SP_DLG_Simulation :: OnOpenAllViewsSeparately)

EVT_BUTTON( SP_ID_BUTTON_ADD_NEW_VIEW, SP_DLG_Simulation :: OnAddingNewModalView)
EVT_BUTTON( SP_ID_BUTTON_REMOVE_VIEWS, SP_DLG_Simulation :: OnRemovingModalViews)
EVT_BUTTON( SP_ID_BUTTON_RENAME_VIEW, SP_DLG_Simulation :: OnRenameModalView)
EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_VIEWS, SP_DLG_Simulation :: OnOpenSelectedGraphViews)
EVT_LISTBOX(SP_ID_LISTBOX_VIEWS, SP_DLG_Simulation::OnSelectedGraphView)//george, this is required for direct export of the viewe variables without having to open the selected view

END_EVENT_TABLE()

SP_DLG_Simulation::SP_DLG_Simulation(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
//		wxDialog(p_pcParent, -1, p_sTitle, wxPoint(20, 20), wxSize(800, 750), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
		SP_DLG_BaseSimulation(p_pcParent, p_sHelpText, p_sTitle, p_nStyle),
		m_pcGraph(p_pcGraph),
        m_nRefreshRate(5000),
        m_nResultPointCount(0),
        m_pcExportFileOutputStream(NULL),
        m_pcExportBufferdOutputStream(NULL),
        m_pcExport(NULL), m_nExportSpacer(1),
        m_nPlotYMax(1),
        m_pcCurrentTablePlot(NULL),
        m_bCompressExact(false),
        m_sNodeclass(SP_DS_CONTINUOUS_PLACE),
        m_bSumOfNodes(false),
        m_bCanChangeCurrentView(true),
        m_pcPlaceChoiceSizerLabel(NULL),
        m_bComAuxVarSingleRun(false)

{
	//TODO: find a better way to deal with dialogs without p_pcGraph!
    if(p_pcGraph!=NULL)
    {
	   m_sExportFilename=
			   p_pcGraph->GetParentDoc()->GetFilename();
    }

    m_sExportFilename = m_sExportFilename.BeforeLast('.');
    m_sExportFilename << wxT(".csv");
    //choices for X axis
    //TODO: change it in your class
    //Note that another option will be added to this list (time)
    m_pcXAxisChoices[wxT("Place")] = &m_asPlaceNames;
    m_pcXAxisChoices[wxT("Transition")] = &m_asTransitionNames;

    //ScrollWindow(0, 1000);
    //give the simulator a pointer to Snoopy log
    spsim::Simulator::SetLogFunction(SimulatorLogFunction);

	SetMinimalLayout();

	 //create a timer
	 m_pcTimer = new wxTimer(this, SP_ID_TIMER_UPDATE_DIALOG);

	//load initial simulator configuration
	LoadInitialSimulatorConfig();

	m_nSimTypeForStoch = 0;

    Bind(wxEVT_CLOSE_WINDOW, &SP_DLG_Simulation::OnCloseWindow, this);
}

void SP_DLG_Simulation::OnInitDialog(wxInitDialogEvent& event)
{
    wxBusyInfo l_info(wxT("Please wait while initializing the simulation dialog"), this);

    //initialize the model views. If it is the first time, a default view
    LoadNetInformation();

    //will be added
    InitializeViews();

    //load the current view
    //LoadData();

    //load the marking, rate, and parameter sets
    LoadSets();

	LoadObservers();

    //show default view
    //OpenViewInSeparateWindow(m_pcCurrentTablePlot);

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

    /******************Model Configuration starts********************/
	m_pcCollpaneSetsSizer = new wxCollapsiblePane(m_pcScrolledWindow, SP_ID_COLLAPSEPANEL_SETS_SIZER, "Model Configuration", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
	m_pcPropertyWindowSetsSizer = m_pcCollpaneSetsSizer->GetPane();
	m_pcPropertyWindowSetsSizer->SetSizerAndFit(m_pcSetsSizer);
	m_pcSetsSizer->SetSizeHints(m_pcPropertyWindowSetsSizer);
	m_pcModelConfigSizer->Add(m_pcCollpaneSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
	/**********************Simulation Configuration starts*********************/
	m_pcCollpanePropertySizer = new wxCollapsiblePane(m_pcScrolledWindow, SP_ID_COLLAPSEPANEL_PROPERTY_SIZER, "Simulator Configuration", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
	m_pcPropertyWindowPropertySizer = m_pcCollpanePropertySizer->GetPane();

    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("interval start:")),
    					wxSizerFlags(1).Expand().Border(wxALL, 2));

    m_pcIntervalStartTextCtrl = new wxTextCtrl(m_pcPropertyWindowPropertySizer, -1,"0" , wxDefaultPosition, wxDefaultSize, 0);
    l_pcRowSizer->Add(m_pcIntervalStartTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
    m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("interval end:")),
    					wxSizerFlags(1).Expand().Border(wxALL, 2));

    //l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval end"));
    m_pcIntervalEndTextCtrl = new wxTextCtrl(m_pcPropertyWindowPropertySizer, -1,"100", wxDefaultPosition, wxDefaultSize, 0);
    l_pcRowSizer->Add(m_pcIntervalEndTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
    m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));


    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowPropertySizer, -1, wxT("interval splitting:")),
    					wxSizerFlags(1).Expand().Border(wxALL, 2));

    m_pcResultPointCountTextCtrl = new wxTextCtrl(m_pcPropertyWindowPropertySizer, -1,"100", wxDefaultPosition, wxDefaultSize, 0);
    l_pcRowSizer->Add(m_pcResultPointCountTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
    m_pcPropertySizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

    m_pcPropertyWindowPropertySizer->SetSizerAndFit(m_pcPropertySizer);
	m_pcPropertySizer->SetSizeHints(m_pcPropertyWindowPropertySizer);
	m_pcModelSimulationSizer->Add(m_pcCollpanePropertySizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
	/**********Export and import details starts************/
	m_pcCollpaneDirectExportSizer = new wxCollapsiblePane(m_pcScrolledWindow, SP_ID_COLLAPSEPANEL_DIRECT_EXPORT_SIZER, "Import/Export Details", wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
	m_pcPropertyWindowDirectExportSizer = m_pcCollpaneDirectExportSizer->GetPane();

    // rows of direct export sizer
    l_pcRowSizer = new wxBoxSizer(wxVERTICAL);
    m_pcNoExportRadioButton = new wxRadioButton(m_pcPropertyWindowDirectExportSizer, -1, wxT("no export"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_pcNoExportRadioButton->SetValue(true);
    l_pcRowSizer->Add(m_pcNoExportRadioButton, wxSizerFlags(0).Border(wxALL, 5));
    m_pcDirectExportRadioButton = new wxRadioButton(m_pcPropertyWindowDirectExportSizer, -1, wxT("direct export"), wxDefaultPosition, wxDefaultSize, 0);
    l_pcRowSizer->Add(m_pcDirectExportRadioButton, wxSizerFlags(0).Border(wxALL, 5));
    m_pcDirectExportSizer->Add(l_pcRowSizer);

    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowDirectExportSizer, SP_ID_BUTTON_DIRECT_EXPORT_PROPERTIES, wxT("Properties")),
    					wxSizerFlags(0).Border(wxALL, 5));
    l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowDirectExportSizer, SP_ID_BUTTON_LOAD_DATA, wxT("Load Data")),
    					wxSizerFlags(0).Border(wxALL, 5));
    m_pcDirectExportSizer->Add(l_pcRowSizer);

	m_pcPropertyWindowDirectExportSizer->SetSizerAndFit(m_pcDirectExportSizer);
	m_pcDirectExportSizer->SetSizeHints(m_pcPropertyWindowDirectExportSizer);
	m_pcExportSizer->Add(m_pcCollpaneDirectExportSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
    /***************Simulator starts*********************/
	// rows of control button#
	m_pcPropertyWindowSimulationButtonSizer = new wxWindow(m_pcScrolledWindow, -1);

	// rows of control button#
	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(m_pcPropertyWindowSimulationButtonSizer, -1, wxT("Views")), wxVERTICAL);
	m_pcListboxShowAllGraphViewName = new wxListBox(m_pcPropertyWindowSimulationButtonSizer,SP_ID_LISTBOX_VIEWS, wxDefaultPosition, wxSize(-1, 125), 0, NULL, wxLB_EXTENDED | wxLB_HSCROLL);
	l_pcRowSizer->Add(m_pcListboxShowAllGraphViewName, wxSizerFlags(1).Expand().Border(wxALL, 5));
	m_pcListboxShowAllGraphViewName->SetToolTip(wxT("Double click to open selected view(s)"));

	/*wxSizer *l_pctemp = new wxBoxSizer(wxHORIZONTAL);
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_SHOW_SELECTED_VIEWS, wxT("Show Selected Views")),
					wxSizerFlags(0).Border(wxALL, 2));
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_SHOW_ALL_VIEWS, wxT("Show All")),
					wxSizerFlags(0).Border(wxALL, 2));
	l_pcRowSizer->Add(l_pctemp, wxSizerFlags(0).Border(wxALL, 2));*/

	wxSizer* l_pctemp = new wxBoxSizer(wxHORIZONTAL);
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_ADD_NEW_VIEW, wxT("Add")),
					wxSizerFlags(0).Border(wxALL, 2));
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_REMOVE_VIEWS, wxT("Remove")),
					wxSizerFlags(0).Border(wxALL, 2));
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_RENAME_VIEW, wxT("Rename")),
					wxSizerFlags(0).Border(wxALL, 2));
	l_pctemp->Add(new wxButton(m_pcPropertyWindowSimulationButtonSizer, SP_ID_BUTTON_MORE_VIEW_OPTIONS, wxT("...")),
						wxSizerFlags(0).Border(wxALL, 2));
	l_pcRowSizer->Add(l_pctemp, wxSizerFlags(0).Border(wxALL, 2));

	m_pcSimulationButtonSizer->Add(l_pcRowSizer, wxSizerFlags(1).Expand().Border(wxALL, 5));

	m_pcPropertyWindowSimulationButtonSizer->SetSizerAndFit(m_pcSimulationButtonSizer);
	m_pcSimulationButtonSizer->SetSizeHints(m_pcPropertyWindowSimulationButtonSizer);
	m_pcSimulationControlSizer->Add(m_pcPropertyWindowSimulationButtonSizer, wxSizerFlags(1).Expand().Border(wxALL, 5));

	//if the graph is null, there should be something wrong
	if(m_pcGraph!=NULL)
	{
		CreateStartSimulationButton();
	}

    SetSizerAndFit(m_pcMainSizer);

}

void SP_DLG_Simulation::CreateStartSimulationButton()
{
	 wxSizer* l_pcRowSizer;
	 // start simulation
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	
	m_pcStartButton = new wxButton(this, SP_ID_BUTTON_START_SIMULATION, wxT("Start Simulation"));
	m_pcStartButton->SetBackgroundColour(*wxGREEN);
	l_pcRowSizer->Add(m_pcStartButton, wxSizerFlags(0).Expand().Border(wxALL, 5));
	m_pcSimulationProgressGauge = new wxGauge(this, -1, 100, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcSimulationProgressGauge, wxSizerFlags(1).Expand().Border(wxALL, 5));
	m_pcSimulationProgressText = new wxStaticText(this, -1, wxT("    0 %"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxALIGN_RIGHT);
	l_pcRowSizer->Add(m_pcSimulationProgressText, wxSizerFlags(0).Expand().Border(wxALL, 5));
	m_pcMainSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
	
	// simulation stop watch
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Simulation runtime:")), 0, wxALL, 5);
	m_pcSimulationStopWatch = new wxStaticText(this, -1, wxT("0.000 s"));
	l_pcRowSizer->Add(m_pcSimulationStopWatch, wxSizerFlags(0).Expand().Border(wxALL, 5));
	m_pcMainSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));

	m_pcMainSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), wxSizerFlags(0).Border(wxALL, 5));

}

void SP_DLG_Simulation::OnMoreViewOptions(wxCommandEvent& WXUNUSED(event))
{
	wxMenu *l_pcEditViewMenu = new wxMenu;

	l_pcEditViewMenu->Append(SP_ID_BUTTON_SHOW_SELECTED_VIEWS, wxT("Show Selected Views"));
	l_pcEditViewMenu->Append(SP_ID_BUTTON_SHOW_ALL_VIEWS, wxT("Show All"));

	l_pcEditViewMenu->AppendSeparator();

	PopupMenu(l_pcEditViewMenu);
}


void SP_DLG_Simulation::LoadInitialSimulatorConfig()
{
	    //Simulation Parameters
	    SP_DS_Metadata* l_pcSimProp=NULL;

	    SP_DS_Attribute* l_pcAttr=NULL;

	    if(m_pcGraph!=NULL)
	    {
	    	l_pcSimProp=
	    		  *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());


	    	//interval start
	    	l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval start"));
	    	wxString l_sValue=l_pcAttr->GetValueString();
	    	m_pcIntervalStartTextCtrl->SetValue(l_sValue);

	    	//interval end
	    	l_pcAttr = l_pcSimProp->GetAttribute(wxT("interval end"));
			l_sValue=l_pcAttr->GetValueString();
			m_pcIntervalEndTextCtrl->SetValue(l_sValue);

			//interval split
			l_pcAttr = l_pcSimProp->GetAttribute(wxT("output step"));
			l_sValue=l_pcAttr->GetValueString();
			m_pcResultPointCountTextCtrl->SetValue(l_sValue);
	    }

}

void SP_DLG_Simulation::SimulatorLogFunction(spsim::LogLevel p_nLogLevel, const std::string& p_sLogMsg)
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

void SP_DLG_Simulation :: OnCollapsePropertySizer(wxCollapsiblePaneEvent& event)
{
	Layout();
}

void SP_DLG_Simulation :: OnCollapseSetsSizer(wxCollapsiblePaneEvent& event)
{
	Layout();
}

void SP_DLG_Simulation :: OnCollapseDirectExportSizer(wxCollapsiblePaneEvent& event)
{
	Layout();
}


void SP_DLG_Simulation::OnSelectedGraphView(wxCommandEvent& p_cEvent) {

	wxArrayInt currentSelections;
	currentSelections.Clear();
	m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);
	size_t tmp = currentSelections.GetCount();

	SP_DS_Metadata* l_pcView;

	for (size_t i = 0; i < tmp; ++i) {
		// get a pointer to the view
		l_pcView = FindView(m_pcListboxShowAllGraphViewName->GetString(currentSelections[i]));
		OpenViewInSeparateWindow(l_pcView,true);
		ChangeCurrentView(l_pcView);
	}

}

void SP_DLG_Simulation :: OnOpenSelectedGraphViews(wxCommandEvent& p_cEvent) {
	wxArrayInt currentSelections;
	currentSelections.Clear();
	m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);
	size_t tmp = currentSelections.GetCount();

	SP_DS_Metadata* l_pcView;

	for (size_t i = 0; i < tmp; ++i) {
		// get a pointer to the view
		l_pcView = FindView(m_pcListboxShowAllGraphViewName->GetString(currentSelections[i]));
		OpenViewInSeparateWindow(l_pcView);
        ChangeCurrentView(l_pcView);
	}

}

void SP_DLG_Simulation::RefreshCurrentExternalView(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle)
{
	//check if a view with the same name is already opened
	for (auto l_itWindow : m_pcExternalWindows)
	{

		if (m_pcCurrentTablePlot == (dynamic_cast<SP_DLG_ShowAllModelView*>(l_itWindow))->GetModelView())
		{

			l_itWindow->RefreshCurrentWindow(p_nCurveIndex, p_nColor, p_nLineWidth, p_nLineStyle);

			return;
		}
	}
}

void SP_DLG_Simulation::OnAddingNewModalView(wxCommandEvent& p_cEvent) {

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
	//SaveCurrentView();

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("0"));

	//set the new metadata as the current one
	m_pcCurrentTablePlot = CreateNewView(l_sViewName);//, true);

	CHECK_POINTER(m_pcCurrentTablePlot, return);

	SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

	//Add the new view name to the existing views box
	m_pcListboxShowAllGraphViewName->Insert(l_sViewName, m_pcListboxShowAllGraphViewName->GetCount());
	//select the new view as the current one
	m_pcListboxShowAllGraphViewName->DeselectAll();
	m_pcListboxShowAllGraphViewName->SetStringSelection(l_sViewName);
	LoadData(false);

	//destroy the dialog
	l_pcModelViewDlg->Destroy();

	SP_Core::Instance()->GetRootDocument()->Modify(true);


}

void SP_DLG_Simulation::OnRemovingModalViews(wxCommandEvent& p_cEvent) {

	wxArrayInt currentSelections;
	wxArrayString currentSelections1;
	currentSelections.Clear();
	m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);

	size_t tmp = currentSelections.GetCount();
	for (int i = 0; i < (int)tmp; i++) {
		currentSelections1.Add(m_pcListboxShowAllGraphViewName->GetString(currentSelections[i]));
	}
	SP_DS_Metadata* l_pcView;

	for (int i = 0; i < (int)tmp; i++) {
		// get a pointer to the view
		l_pcView = FindView(currentSelections1[i]);
		ChangeCurrentView(l_pcView);

		SP_DS_Metadataclass* l_pcMetadataclass;

		CHECK_POINTER(m_pcCurrentTablePlot, return);

		l_pcMetadataclass = m_pcGraph->GetMetadataclass(wxT("Plot"));

		CHECK_POINTER(l_pcMetadataclass, return);

		//the user can not delete the main table
		if (l_pcMetadataclass->GetElements()->size() == 1)
		{
			SP_MESSAGEBOX(wxT("Can not remove default view."), wxT("Error"), wxOK | wxICON_ERROR);
			continue;
		}

		l_pcMetadataclass->RemoveElement(m_pcCurrentTablePlot);

		m_pcCurrentTablePlot = NULL;
		InitializeViews();

		//reload the curves
		LoadData(true);
	}

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_Simulation::OnRenameModalView(wxCommandEvent& p_cEvent) {

	wxArrayInt currentSelections;
	m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);

	if(currentSelections.IsEmpty() || currentSelections.GetCount() > 1)
	{
		SP_MESSAGEBOX(wxT("Please select one view to rename."), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}

	// get a pointer to the view
	wxString l_sView = m_pcListboxShowAllGraphViewName->GetString(currentSelections[0]);
	SP_DS_Metadata* l_pcView = FindView(l_sView);
	//ChangeCurrentView(l_pcView);

	//the user can not delete the main table
	if (m_pcListboxShowAllGraphViewName->GetCount() == 1)
	{
		SP_MESSAGEBOX(wxT("Can not rename default view."), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}

	wxTextEntryDialog* l_pcModelViewDlg;
	wxString l_sViewName;

	int l_nReturnCode = 0;

	l_pcModelViewDlg = new wxTextEntryDialog(this, wxT("Enter view name:"), wxGetTextFromUserPromptStr, l_sView);

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

		if (IsViewNameExist(l_sViewName))
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

	l_pcView->GetAttribute(wxT("Name"))->SetValueString(l_sViewName);
    l_pcView->GetAttribute(wxT("ViewTitle"))->SetValueString(l_sViewName);

	//m_pcCurrentTablePlot = NULL;
	InitializeViews();

	//reload the curves
	LoadData(true);

	SP_Core::Instance()->GetRootDocument()->Modify(true);

}

void SP_DLG_Simulation::OnEditXAxis(wxWindow *p_pcExternalWindowDialog) {
    CHECK_POINTER(m_pcCurrentTablePlot, return);
    CHECK_POINTER(m_pcMainSimulator, return);

    if(m_pcMainSimulator->GetResultMatrix().empty())
    {
        //no results so far
        return;
    }

    m_bCanChangeCurrentView = false;

    wxString l_sCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"));
    wxString l_sSubCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariableName"));

    SP_VectorStdString l_asTimeVector;
    l_asTimeVector.push_back("Simulation Time");
    l_asTimeVector.push_back("Run Time");
    m_pcXAxisChoices[wxT("Time")] = &l_asTimeVector;
	 

    SP_DLG_SelectXAxisVariable* l_pcDialog = new SP_DLG_SelectXAxisVariable(p_pcExternalWindowDialog, m_pcXAxisChoices, l_sCategory, l_sSubCategory);

    if (l_pcDialog->ShowModal() == wxID_OK)
    {
        SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"), l_pcDialog->GetSelectedCategory());
        SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariableName"), l_pcDialog->GetSelectedSubCategory());

        SetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisTitle"), l_pcDialog->GetSelectedSubCategory());

        UpdateXAxisValues();

        UpdateViewer();
    }

    l_pcDialog->Destroy();

    m_bCanChangeCurrentView = true;
}

void SP_DLG_Simulation::OnExportClicked(wxWindow *p_pcExternalWindowDialog, int p_nSelection) {
	if (p_nSelection == 0) {
		OnExportToCSV();
	}
}

bool SP_DLG_Simulation::CreateViewerDataFromRegex(SP_DS_Metadata* p_pcView)
{
    CHECK_POINTER(p_pcView,return false);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*> (p_pcView->GetAttribute(wxT("CurveInfo")));
	if(l_pcCurveInfoList->GetRowCount() > 0)
	{
		//curveinfo already created
		return false;
	}

	SP_DS_Attribute* l_pcRegExAttr = p_pcView->GetAttribute(wxT("RegEx"));
    CHECK_POINTER(l_pcRegExAttr, return false);

    wxString l_RegExString = l_pcRegExAttr->GetValueString();
    if(l_RegExString.IsEmpty())
    {
    	return false;
    }

	bool l_RegExInvert = static_cast<SP_DS_BoolAttribute*>(p_pcView->GetAttribute(wxT("RegExInvert")))->GetValue();
	wxString l_RegExOutputType = wxT("Unfolded");
	SP_DS_Attribute* l_pcAttr =	p_pcView->GetAttribute(wxT("OutputType"));
	if(l_pcAttr)
	{
		l_RegExOutputType = l_pcAttr->GetValueString();
	}

	SP_VectorString l_asColours;

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute,return false);
	wxString l_sElementType = l_pcAttribute->GetValueString();

	if(l_sElementType==wxT("Place"))
	{
		 LoadNodeColours(SP_DS_CONTINUOUS_PLACE,l_asColours);
		 LoadNodeColours(SP_DS_DISCRETE_PLACE,l_asColours);
	}
	else if(l_sElementType==wxT("Transition"))
	{
		LoadNodeColours(SP_DS_CONTINUOUS_TRANS,l_asColours);
		LoadNodeColours(SP_DS_DISCRETE_TRANS,l_asColours);
	}

	wxString l_sCheckState = wxT("1");
	wxString l_sCurveColor;
	wxString l_sLinewidth;
	l_sLinewidth <<-1;
	wxString l_sLineStype;
	l_sLineStype << -1;

	wxRegEx l_RegEx;
	if (l_RegEx.Compile(l_RegExString)) {
		unsigned l_nPosition = 0;
		if (l_sElementType.IsSameAs(wxT("Place"))) {
			if (l_RegExOutputType == wxT("Unfolded")) {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayUnPlaces.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayUnPlaces[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			} else if (l_RegExOutputType == wxT("Colored")) {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayColPlaces.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayColPlaces[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			} else {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayAuxPlaces.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayAuxPlaces[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			}
		} else if (l_sElementType.IsSameAs(wxT("Transition"))) {
			if (l_RegExOutputType == wxT("Unfolded")) {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayUnTranstions.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayUnTranstions[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			} else if (l_RegExOutputType == wxT("Colored")) {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayColTranstions.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayColTranstions[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			} else {
				for (unsigned int l_nRow = 0; l_nRow < m_ArrayAuxtranstions.GetCount(); l_nRow++)
				{
					wxString l_sName = m_ArrayAuxtranstions[l_nRow];
					bool match = l_RegEx.Matches(l_sName);
					if(l_RegExInvert) { match = !match; }
					if(match) {
						//get either the node colour or assign an arbitrary colour
						if(l_nPosition < l_asColours.size() && l_asColours[l_nPosition] != wxT(""))
						{
							l_sCurveColor = l_asColours[l_nPosition];
						}
						else
						{
							l_sCurveColor = SP_DLG_Simulation::GetColourString(l_nPosition);
						}
						unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
						l_pcCurveInfoList->SetCell(l_nRow,0,wxString::Format(wxT("%u"),l_nPosition));
						l_pcCurveInfoList->SetCell(l_nRow,1,l_RegExOutputType);
						l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
						l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
						l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
						l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
						l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
					}
					l_nPosition++;
				}
			}
		}
	}
	return true;
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

    m_ArrayUnPlaces.Clear();
	m_ArrayUnTranstions.Clear();
	for (unsigned int i = 0; i < m_asPlaceNames.size(); i++) {
		m_ArrayUnPlaces.Add(m_asPlaceNames[i]);
	}
	for (unsigned int i = 0; i < m_asTransitionNames.size(); i++) {
		m_ArrayUnTranstions.Add(m_asTransitionNames[i]);
	}

    CreateViewerDataFromRegex(p_pcView);

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
			wxString l_sCategory = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("XAxisVariable"));
			wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();

			if (netClass.Contains(wxT("Fuzzy")) ) {
				//p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
				p_pcViewer->AddFuzzyCurves(l_sName, l_nPosition, &(*m_pcCompressedBand));
			//	p_pcViewer->AddFuzzyCurves(l_sName, l_nPosition, m_nFuzzyResultBand, m_FuzzyBand,m_nAlphaLevels,m_nLevels,m_nSamples,m_fnCount);

			}
			else
			p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
			 
		}
		else if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_asTransitionNames.size()) //unfolded place
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
			SP_LOGWARNING( l_sOrignialName + wxT("  ") + l_sName + wxT("Name for position (") + wxString::Format(wxT("%d"), l_nRow) + wxT(") is changed to ") + l_sName);
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

    //LoadViewerData(m_apcResultViewers[m_nCurrentViewer], m_pcCurrentTablePlot, l_asPlaces);

    //load old setting
    //LoadViewProperties();
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
                //m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
            	auto itFind = m_mGroup2Position.find(l_sGroup);

            	if(itFind!= m_mGroup2Position.end())
            	{

				SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));//george, remember the chosen sets
				m_apcColListAttr[itFind->second] = l_pcColList;
				
				auto it = m_mGroup2Selction.find(l_sGroup);

				if(it== m_mGroup2Selction.end())
				m_mGroup2Selction[l_sGroup] = l_pcColList->GetActiveColumn();

				break;
            	}
            }
        }
    }

    for (size_t j = 0; j < m_apcColListAttr.size(); j++)
    {
        SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
        wxChoice* l_pcCombobox = m_apcComboBoxes[j];
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

    SP_VectorStdString l_asParameterNames;
    SP_VectorDouble l_anParameterValue;

    m_msParameterName2Value.clear();

    for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
    {
        SP_DS_Metadata* l_pcConstant = *it;
        wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A
		
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
                	l_nValue = SP_DS_FunctionEvaluatorLong{l_pcFR, l_Function}();
                }
                else if(l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_Function}();
				}
				l_pcFR->registerFunction(l_sName, to_string(l_nValue));
            }
            //****************************************************************************************
			
				l_anParameterValue.push_back(l_nValue);

				m_msParameterName2Value[l_sName] = l_nValue;

			 
        }
    }
  
    m_pcMainSimulator->SetParameterNames(l_asParameterNames);
    m_pcMainSimulator->SetParameterValues(l_anParameterValue);
}
void SP_DLG_Simulation::LoadObservers()
{
	std::map<std::string, unsigned int> l_mPlaceToPosition;
	std::map<std::string, unsigned int> l_mTransitionToPosition;

	for (auto const& elem : m_mPlaceName2Position) {
		l_mPlaceToPosition[elem.first] = elem.second;
	}

	for (auto const& elem : m_mTransitionName2Position) {
		l_mTransitionToPosition[elem.first] = elem.second;
	}

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_VectorString l_asParameterNames;

		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

			unsigned long l_nPos = 0;

			// check the function
			SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
			SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sBody));
			if (!l_pcFunction)
			{
				SP_MESSAGEBOX(wxT("the body of observer ") + l_sName + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
				continue;
			}

			SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

			if (l_sType == wxT("Place"))
			{
				m_asPlaceNames.push_back(l_sName);
				m_mPlaceName2Position[l_sName] = l_nPos = m_asPlaceNames.size() - 1;
				m_mObserverPlaceFunctions[l_nPos] = l_pcExpanded;

				l_mPlaceToPosition[l_sName] = l_nPos;

				dssd::functions::convertOptions co;
				l_pcExpanded->setVariableIds(l_mPlaceToPosition, co);
			}
			else {

				m_asTransitionNames.push_back(l_sName);
				m_mTransitionName2Position[l_sName] = l_nPos = m_asTransitionNames.size() - 1;
				m_mObserverTransitionFunctions[l_nPos] = l_pcExpanded;

				l_mTransitionToPosition[l_sName] = l_nPos;

				dssd::functions::convertOptions co;
				l_pcExpanded->setVariableIds(l_mTransitionToPosition, co);
			}			
		}
	}
}
/*
void SP_DLG_Simulation::SaveSelectedCurves()
{
    CHECK_POINTER(m_pcCurrentTablePlot, return);

    SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

    CHECK_POINTER(l_pcAttribute, return);

    SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

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
//	SP_LOGMESSAGE(wxT("load selected curves"));

    CHECK_POINTER(m_pcCurrentTablePlot, return);

    SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo"));

    CHECK_POINTER(l_pcAttribute, return);

    SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

    CHECK_POINTER(m_apcResultViewers[m_nCurrentViewer], return);

    m_apcResultViewers[m_nCurrentViewer]->ClearShownCurves();

    SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColPN_Unfolding *l_pcUnfoldedNet;

	long l_nPos = 0;
	wxString l_TempClassName = m_pcGraph->GetNetclass()->GetName();

//	SP_LOGMESSAGE(l_TempClassName);

	if (l_TempClassName == SP_DS_COLSPN_CLASS || l_TempClassName == SP_DS_COLCPN_CLASS || l_TempClassName == SP_DS_COLHPN_CLASS)
	{
		vector<wxString>* l_vAuxVar = nullptr;

		if (l_TempClassName == SP_DS_COLSPN_CLASS) {
			l_pcUnfoldedNet = dynamic_cast<SP_DLG_ColStSimulationResults*>(this)->GetUnfoldedNet();
			l_vAuxVar =  dynamic_cast<SP_DLG_ColStSimulationResults*>(this)->GetAuxPLVariables();
		} else if (l_TempClassName == SP_DS_COLCPN_CLASS) {
			l_pcUnfoldedNet = dynamic_cast<SP_DLG_ColCPNSimulationResults*>(this)->GetUnfoldedNet();
			l_vAuxVar =  dynamic_cast<SP_DLG_ColCPNSimulationResults*>(this)->GetAuxPLVariables();
		} else if (l_TempClassName == SP_DS_COLHPN_CLASS) {
			l_pcUnfoldedNet = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(this)->GetUnfoldedNet();
			l_vAuxVar =  dynamic_cast<SP_DLG_ColHPNSimultionResults*>(this)->GetAuxPLVariables();
		}
		m_ArrayAuxPlaces.Clear();
		for(unsigned int i= 0; i < l_vAuxVar->size(); i++)
		{
			wxString l_sPlaceName = (*l_vAuxVar)[i];
			m_ArrayAuxPlaces.Add(l_sPlaceName);
		}


		if(!l_pcUnfoldedNet)
			return;
		m_ArrayUnPlaces.Clear();
		m_ArrayUnTranstions.Clear();
		m_ArrayColPlaces.Clear();
		m_ArrayColTranstions.Clear();

		vector<wxString>* l_pvUnfoldedPlace = l_pcUnfoldedNet->GetPlaceNames();
		vector<wxString>* l_pvColoredPlace = l_pcUnfoldedNet->GetColoredPlaceNames();


		vector<wxString>* l_pvUnfoldedTrans = l_pcUnfoldedNet->GetTransitionNames();
		vector<wxString>* l_pvColoredTrans = l_pcUnfoldedNet->GetColoredTransitionNames();
		for(unsigned int i= 0; i < l_pvUnfoldedPlace->size(); i++)
		{
			wxString l_sPlaceName = (*l_pvUnfoldedPlace)[i];
			m_ArrayUnPlaces.Add(l_sPlaceName);
		}

		for(unsigned int i= 0; i < l_pvColoredPlace->size(); i++)
		{
			wxString l_sPlaceName = (*l_pvColoredPlace)[i];
			m_ArrayColPlaces.Add(l_sPlaceName);
		}


		for(unsigned int i= 0; i < l_pvUnfoldedTrans->size(); i++)
		{
			wxString l_sTransName = (*l_pvUnfoldedTrans)[i];
			m_ArrayUnTranstions.Add(l_sTransName);
		}

		for(unsigned int i= 0; i < l_pvColoredTrans->size(); i++)
		{
			wxString l_sTransName = (*l_pvColoredTrans)[i];
			m_ArrayColTranstions.Add(l_sTransName);
		}

	} else {

		m_ArrayUnPlaces.Clear();
		m_ArrayUnTranstions.Clear();
		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
			if(l_pcNodeclass)
			{
				for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
				{
					SP_DS_Node* l_pcNode = *l_itElem;
					wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					m_ArrayUnPlaces.Add(l_sPlaceName);
					l_nPos++;
				}
			}

		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnPlaces.Add(l_sPlaceName);
				l_nPos++;
			}
		}

		l_nPos = 0;

		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnTranstions.Add(l_sPlaceName);
				l_nPos++;
			}
		}

		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnTranstions.Add(l_sPlaceName);
				l_nPos++;
			}
		}
		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnTranstions.Add(l_sPlaceName);
				l_nPos++;
			}
		}
		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnTranstions.Add(l_sPlaceName);
				l_nPos++;
			}
		}

		l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
		if(l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcNode = *l_itElem;
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				m_ArrayUnTranstions.Add(l_sPlaceName);
				l_nPos++;
			}
		}
	}

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
*/

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

            wxString l_sValue = dssd::aux::toString(*l_itCol);

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

void SP_DLG_Simulation::UpdateObservers(const wxString & p_sObsersverType, unsigned long p_nRowCount)
{
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction;

	if (p_sObsersverType.IsSameAs(wxT("Place"))) {
        for(auto it : m_mObserverPlaceFunctions) {
            l_pcFunction = it.second;

            for (unsigned int l_nRow = 0; l_nRow < p_nRowCount; l_nRow++) {
                double l_Val = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_pcFunction, std::numeric_limits<double>::min()}(
                        m_anResultMatrix[l_nRow]);
                m_anResultMatrix[l_nRow].push_back(l_Val);
            }
        }
	} else {
        for(auto it : m_mObserverTransitionFunctions) {
            l_pcFunction = it.second;

            for (unsigned int l_nRow = 0; l_nRow < p_nRowCount; l_nRow++) {
                double l_Val = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_pcFunction, std::numeric_limits<double>::min()}(
                        m_anResultMatrix[l_nRow]);
                m_anResultMatrix[l_nRow].push_back(l_Val);
            }
        }
	}
}

void SP_DLG_Simulation::OnModifyConstantSets(wxCommandEvent& p_cEvent)
{
    //SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);
	if (m_pcGraph->GetNetclass()->GetName().Contains("Fuzzy")) {
		SP_DLG_FpnConstantDefinition* l_pcDlg = new SP_DLG_FpnConstantDefinition(NULL);
		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			LoadSets();

		}

		l_pcDlg->Destroy();
	}
	else {

		SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);
		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			     LoadSets();
			     
			    //recover selected groups after modifying constants, by george 03/05/2020
			    unsigned i = 1;

				if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_HYBRIDPN_CLASS)
				{
					i = 0;
				}
				else if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
				{
					i = 4;
				}

				for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
				{
					auto itFind = m_mGroup2Position.find(it->first);

					if (itFind != m_mGroup2Position.end())
					{
					  m_apcComboBoxes[itFind->second]->SetSelection((it)->second);
					}
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
		}

		l_pcDlg->Destroy();

	}
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
        //int l_nAnswer = SP_MESSAGEBOX(wxT("Would you like to close external view windows?"), wxT("Questions"), wxYES_NO, this);

        //if (l_nAnswer == wxYES)
        {
        	for (auto l_itWindow : m_pcExternalWindows)
            {
                l_itWindow->Destroy();
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
	//Destroy();  // you may also do:  event.Skip();
                // since the default event handler does call Destroy(), too
	p_cEvent.Skip();
}

void SP_DLG_Simulation::OnDirectExportProperties(wxCommandEvent& p_cEvent)
{
    SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT, this, &m_sExportFilename, &m_nExportSpacer, &m_bCompressExact,&m_bExportAllTracesForFuzzy);

    l_pcDlg->ShowModal();

	m_bExportAllTracesForFuzzy = l_pcDlg->IsAllTracesChecked();

    l_pcDlg->Destroy();
}
void SP_DLG_Simulation::OnSimulationProperties(wxCommandEvent& p_cEvent)
{

    if (m_pcMainSimulator == NULL || m_pcMainSimulator->IsSimulationRunning())
    {
        return;
    }

    SP_DLG_SimulationProperties* l_pcDlg;

    l_pcDlg = new SP_DLG_SimulationProperties(m_pcMainSimulator, this, m_nSimTypeForStoch);

    l_pcDlg->ShowModal();

    l_pcDlg->Destroy();

    SetFocus();

    CHECK_POINTER(m_pcMainSimulator->GetSimulatorOptions(),return);
    spsim::Property* l_pcRefershRate=m_pcMainSimulator->GetSimulatorOptions()->GetOption("Refreshrate");

    CHECK_POINTER(l_pcRefershRate,return);

    m_nRefreshRate=l_pcRefershRate->GetValuelong();

	SP_Core::Instance()->GetRootDocument()->Modify(true);

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

     m_pcCurrentTablePlot->GetAttribute( wxT("X_Axis_Min") )->SetValueString(wxString::Format(wxT("%f"),  l_nXMin) );
     m_pcCurrentTablePlot->GetAttribute( wxT("X_Axis_Max") )->SetValueString(wxString::Format(wxT("%f"),  l_nXMax) );
     m_pcCurrentTablePlot->GetAttribute( wxT("Y_Axis_Min") )->SetValueString(wxString::Format(wxT("%f"),  l_nYMin) );
     m_pcCurrentTablePlot->GetAttribute( wxT("Y_Axis_Max") )->SetValueString(wxString::Format(wxT("%f"),  l_nYMax) );

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

void SP_DLG_Simulation::OpenExportFile()
{
    wxString l_sExportFilename = m_sExportFilename.BeforeLast('.');


    if (m_bIsCompressChosen)
    	{//by george12.2020
    		//l_sExportFilename.Replace(_T(":"), _T("-"));
    		l_sExportFilename.Replace(_T("("), _T(""));
    		l_sExportFilename.Replace(_T(")"), _T(""));
    		l_sExportFilename.Replace(_T(" "), _T(""));
    		l_sExportFilename.Replace(_T(".EuropeStandardTime"), _T(""));
    		l_sExportFilename.Replace(_T("."), _T(""));
    		m_sExportFilename = l_sExportFilename;
    	}
    l_sExportFilename << wxT(".csv");
    m_sDirectExportName = l_sExportFilename;//george12.2020

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
	wxString sNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();
    SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));

    if(l_pcPlaceIdList)// by george
    {
    if (l_pcPlaceIdList->GetRowCount() == 0)
    	{
    		wxArrayInt currentSelections;
    		currentSelections.Clear();
    		m_pcListboxShowAllGraphViewName->GetSelections(currentSelections);
    		size_t tmp = currentSelections.GetCount();

    		SP_DS_Metadata* l_pcView;

    		for (size_t i = 0; i < 1; ++i)
    		{//take the default viewe only
    			// get a pointer to the view
    			l_pcView = FindView(m_pcListboxShowAllGraphViewName->GetString(currentSelections[i]));
    			OpenViewInSeparateWindow(l_pcView, true);
    			ChangeCurrentView(l_pcView);
    		}
    	}
    }

    for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
    {
        wxString l_sCheckState = l_pcPlaceIdList->GetCell(i, 2);
		if (l_sCheckState == wxT("1"))
		{
			
			if (!sNetClass.Contains(wxT("Fuzzy")))// Added by G.A
			{
			l_sName = l_pcPlaceIdList->GetCell(i, 6);
			l_sOutput << l_sSpacer << l_sName;
		    }
        }
    }

	if (!sNetClass.Contains(wxT("Fuzzy")))// Added by G.A
	{
		(*m_pcExport) << l_sOutput << endl;
	}
	/**
	if (sNetClass.Contains(wxT("Fuzzy")) && m_bExportMembershipFunction) {

		wxString l_sMembershipFunctions = m_sExportFilename;

		l_sMembershipFunctions << wxT("_tfn.csv");

		m_pcExportFileOutputStreamMembershipFun = new wxFileOutputStream(l_sMembershipFunctions);

		if (!m_pcExportFileOutputStreamMembershipFun->IsOk())
		{
			m_pcExportFileOutputStreamMembershipFun = new wxFileOutputStream(l_sMembershipFunctions);

			if (!m_pcExportFileOutputStreamMembershipFun->IsOk())
			{
				return;
			}
		}

		m_pcExportBufferdOutputStreamMembershipFun = new wxBufferedOutputStream(*m_pcExportFileOutputStreamMembershipFun);
		m_pcExportMembershipfuns = new wxTextOutputStream(*m_pcExportBufferdOutputStreamMembershipFun);
	}
	*/
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
void SP_DLG_Simulation::OnExportToCSV()
{
	wxString l_sViewName = GetViewAttributeValue(m_pcCurrentTablePlot, wxT("Name"));
	l_sViewName.Replace(wxT(" "), wxT(""));
	wxString l_sIntermediateName = m_sExportFilename.BeforeLast(wxChar('.'));

    wxString l_sFilename = l_sIntermediateName + wxT("_") +l_sViewName + wxT(".csv");
    bool l_bCompressExact = false;
	bool l_bAlltraces = false;
    SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, this, &l_sFilename, &m_nExportSpacer, &l_bCompressExact, &l_bAlltraces);


    if (l_pcDlg->ShowModal() == wxID_OK)
    {
        wxString l_sBackupFilename = m_sExportFilename;
        m_sExportFilename = l_sFilename;

        if (m_sExportFilename == m_pcGraph->GetParentDoc()->GetFilename().BeforeLast('.')
        	&& !l_pcDlg->IsCompressChecked())
         {
        		m_sExportFilename << wxT("(")
        		 << wxDateTime::Now().Format(wxT("%Y-%m-%dT%H-%M-%S%Z")) << wxT(")");
         }
		m_bExportAllTracesForFuzzy = l_pcDlg->IsAllTracesChecked();
		m_bExportMembershipFunction = l_pcDlg->IsMembershipFunctionChecked();
	 
        OpenExportFile();
        DirectExportToCSV();
        CloseExportFile();

        m_sExportFilename = l_sBackupFilename;

   	   /***********compression if required****/
   		if (l_pcDlg->IsCompressChecked())
   		{
   			if (CompressFile(l_sFilename))
   			{
   				SP_LOGMESSAGE(wxT("the output trace has been comprissed successfully"));
   				wxRemoveFile(l_sFilename);
   			}
   			else
   			{
   				SP_LOGMESSAGE(wxT("Something wrong happend while compression!"));
   			}

   		}
   		/**********************/
    }

    l_pcDlg->Destroy();
}

bool SP_DLG_Simulation::CompressFile(const wxString& p_sFileName)
{
	try {
		wxString l_sZip = p_sFileName + _T(".zip");
		l_sZip.Replace(wxT(".csv"), wxT(""));
		wxFileName wfnZipFile(l_sZip);

		wxString wsT1 = wfnZipFile.GetFullPath();
		wxFFileOutputStream out(wsT1);

		if (!out.IsOk()) return false;

		wxZipOutputStream zip(out);

		if (!zip.IsOk()) return false;


		wxFileName wfnSource(p_sFileName);


		wxFileInputStream in(p_sFileName);

		zip.PutNextEntry(wfnSource.GetFullName());
		zip.Write(in);
		zip.CloseEntry();
		zip.Close();

	}catch(exception e) {
		SP_LOGERROR_(e.what()); return false;
	}
	return true;
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
    //SaveCurrentView();

	if(m_pcGraph!=NULL)
	{
		wxDELETE(m_pcMainSimulator);

		wxDELETE(m_pcExport);
		wxDELETE(m_pcExportBufferdOutputStream);
		wxDELETE(m_pcExportFileOutputStream);
	}

     wxDELETEA(m_apColours);

     //delete the timer
     // wxDELETE(m_pcTimer);

     //read all views
     if(m_pcGraph!=NULL)
		 for (SP_DS_Metadata* l_pcMeta : *m_pcGraph->GetMetadataclass(wxT("Plot"))->GetElements())
		 {
			 // clear curveinfo, if we have regex
			 if (!l_pcMeta->GetAttribute(wxT("RegEx"))->GetValueString().IsEmpty())
			 {
				 static_cast<SP_DS_ColListAttribute*>(l_pcMeta->GetAttribute(wxT("CurveInfo")))->Clear();
			 }
		 }

    //set the pointer of external pointer to NULL
	for (auto l_itWindow : m_pcExternalWindows)
    {
        l_itWindow->Close(true);
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

     // m_pcPlotCtrl->SetShowKey(false);

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
//    m_pcShowGraphViewBox->Clear();
    m_pcListboxShowAllGraphViewName->Clear();
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

        m_pcListboxShowAllGraphViewName->Insert(l_sName, m_pcListboxShowAllGraphViewName->GetCount());
        wxString l_sCurrent = GetViewAttributeValue((*l_itElem), wxT("IsCurrent"));

        if (l_sCurrent.IsSameAs(wxT("1")))
        {
            m_pcCurrentTablePlot = (*l_itElem);

            m_pcListboxShowAllGraphViewName->SetSelection(l_nPos);
        }

        l_nPos++;
    }

    //check if we set the current view
    if (m_pcCurrentTablePlot == NULL)
    {
        m_pcCurrentTablePlot = l_pcViewList->front();

        SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

        m_pcListboxShowAllGraphViewName->SetSelection(0);
    }
}

void SP_DLG_Simulation::OpenViewInSeparateWindow(SP_DS_Metadata* p_pcModelView,const bool& p_bHide)
{
    //check if a view with the same name is already opened
	for (auto l_itWindow : m_pcExternalWindows)
    {
        if (p_pcModelView ==  (dynamic_cast<SP_DLG_ShowAllModelView*>(l_itWindow))->GetModelView())
        {
            SP_LOGERROR( wxT("this view(") + GetViewAttributeValue(p_pcModelView, wxT("Name")) + wxT(") is already opened.  A view Can't be opened twice"));

            return;
        }
    }

    SP_DLG_ShowAllModelView* l_pcShowModelViewsDlg = new SP_DLG_ShowAllModelView(this, p_pcModelView, m_pcGraph);

    // check if the window is created correctly
    if (l_pcShowModelViewsDlg != NULL)
    {
        m_pcExternalWindows.push_back(l_pcShowModelViewsDlg);

        if(!p_bHide)
        {

        	l_pcShowModelViewsDlg->Show(true);
        }
        else//by george
        {
        	l_pcShowModelViewsDlg->Show(false);

        	l_pcShowModelViewsDlg->Close();
        }
    }

}

void SP_DLG_Simulation::OnOpenAllViewsSeparately(wxCommandEvent& p_cEvent)
{
	//SP_DLG_BaseSimulation *l_pcdlg = new SP_DLG_BaseSimulation(this);
	//l_pcdlg->Show(true);
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
void SP_DLG_Simulation::RefreshExternalWindows(const bool& p_bUpdate)
{
    for (auto l_itWindow : m_pcExternalWindows)
    {
        if (l_itWindow != NULL)
        {
            l_itWindow->RefreshWindow(p_bUpdate);
        }
    }
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



void SP_DLG_Simulation::ChangeCurrentView(SP_DS_Metadata* p_pcView)
{
    //Save the current view
    //SaveCurrentView();

    CHECK_POINTER(p_pcView, return);

    SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("0"));

    m_pcCurrentTablePlot = p_pcView;

    SetViewAttributeValue(m_pcCurrentTablePlot, wxT("IsCurrent"), wxT("1"));

    //change the current view
    //ChangeCurrentViewerType();

    //call loadData to add the curves to the viewer
    LoadData(false);
}

void SP_DLG_Simulation::ChangeViewTo(SP_DS_Metadata* p_pcView)
{
    if (m_bCanChangeCurrentView)
    {
        ChangeCurrentView(p_pcView);

        wxString l_sViewName = GetViewAttributeValue(p_pcView, wxT("Name"));

//        m_pcShowGraphViewBox->SetStringSelection(l_sViewName);
        m_pcListboxShowAllGraphViewName->SetStringSelection(l_sViewName);
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
    m_pcMainSimulator->GetSimulatorOptions()->InsertNewOption(0, "InBetweenVisualization", "1", "Visualize in between results", spsim::GUI_TYPE_CHECKBOX);
    m_pcMainSimulator->GetSimulatorOptions()->InsertNewOption(1, "Refreshrate", "5000", "Refresh rate(ms)", spsim::GUI_TYPE_TXTBOX);
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
        return l_nSimulatorType;
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

    bool l_UseNodeColour = static_cast<SP_DS_BoolAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("NodeColour")))->GetValue();

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

        if (l_cColour != *wxWHITE && l_UseNodeColour)
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
