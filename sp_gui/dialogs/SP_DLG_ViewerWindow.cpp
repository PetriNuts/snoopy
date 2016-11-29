/*
 * SP_DLG_ViewerWindow.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.011.2016
 * @Description
 * Display all model views in one widow
 */ //======================================================================================
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/SP_DLG_ViewerWindow.h"

#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"

IMPLEMENT_CLASS(SP_DLG_ViewerWindow, wxFrame)

enum
{
	SP_ID_BUTTON_REFRESH,
	SP_ID_BUTTON_DISCONNECT,
	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS,
	SP_ID_CHOICE_RESULT_VIEWER_TYPE,
	SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES,
	SP_ID_BUTTON_EXPORT,
	SP_ID_CHANGE_X_AXIS,
	SP_ID_BUTTON_EDIT_NODE_LIST,
	SP_ID_BUTTON_SHOW_HIDE_NODE_LIST
};

BEGIN_EVENT_TABLE( SP_DLG_ViewerWindow, wxFrame )
END_EVENT_TABLE()

SP_DLG_ViewerWindow::SP_DLG_ViewerWindow(SP_DLG_Simulation* p_pcParentWnd):
		wxFrame(p_pcParentWnd, -1, wxT("Show all views"),
				wxDefaultPosition,
				wxSize(800, 750),
				wxFRAME_FLOAT_ON_PARENT | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX),
				m_pcParentWnd(p_pcParentWnd),
				m_bIsDisconnected(false),
				m_bIsShown(true)
{
	    m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
		m_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

		wxSizer* l_pcLeftSizer = new wxBoxSizer(wxVERTICAL);
		m_pcRightSizer = new wxBoxSizer(wxVERTICAL);

		m_pcMainSizer->Add(m_pcContentSizer, 1, wxALL | wxEXPAND);

		m_pcContentSizer->Add(l_pcLeftSizer, 1, wxALL | wxEXPAND, 5);
		m_pcContentSizer->Add(m_pcRightSizer, 0, wxALL | wxEXPAND, 5);

		wxSizer* l_pcPlaceChoiceSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Select an item to view")), wxVERTICAL);
		m_pcRightSizer->Add(l_pcPlaceChoiceSizer, 1, wxEXPAND);

		m_pcPlaceChoiceCheckListBox = new wxCheckListBox(this, SP_ID_CHECKLISTBOX_PLACE_CHOICE, wxDefaultPosition, wxSize(200, 200));
		l_pcPlaceChoiceSizer->Add(m_pcPlaceChoiceCheckListBox, 1, wxALL | wxEXPAND, 5);

		//select/deselect all
		m_pcSelectClearAllChkBox = new wxCheckBox(this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
		m_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);
		l_pcPlaceChoiceSizer->Add(m_pcSelectClearAllChkBox, 0, wxEXPAND);

		m_pcViewerSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer")), wxVERTICAL);

		l_pcLeftSizer->Add(m_pcViewerSizer, 1, wxALL | wxEXPAND);


		wxSizer *l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer Type")), wxHORIZONTAL);
		m_pcOutputViewerType = new wxChoice(this, SP_ID_CHOICE_RESULT_VIEWER_TYPE, wxDefaultPosition, wxSize(129, -1));
		m_pcOutputViewerType->Append(wxT("Tabular"));
		m_pcOutputViewerType->Append(wxT("xyPlot"));
		m_pcOutputViewerType->Append(wxT("Histogram"));

		wxSizer *l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(m_pcOutputViewerType, 0, wxALL, 1);
		m_pcViewerTypeButton = new wxButton(this, SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, wxT("Edit"));
		l_pcRowSizer->Add(m_pcViewerTypeButton, 0, wxALL, 1);
		l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);

		l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Export")), wxHORIZONTAL);
		m_pcOutputExportType = new wxChoice(this, -1);
		m_pcOutputExportType->Append(wxT("CSV Export"));
		m_pcOutputExportType->Append(wxT("Image Export"));
		m_pcOutputExportType->SetSelection(0);
		l_pcRowSizer->Add(m_pcOutputExportType, 0, wxALL, 1);
		l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_EXPORT, wxT("Export")), 0, wxALL, 1);

		l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);
		m_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);

		l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		m_pcXAxis = new wxButton(this, SP_ID_CHANGE_X_AXIS, wxT("Change X Axis"));
		l_pcRowSizer->Add(m_pcXAxis, 0, wxALL, 1);
		m_pcEditNodeListButton = new wxButton(this, SP_ID_BUTTON_EDIT_NODE_LIST, wxT("Edit Node List"));
		l_pcRowSizer->Add(m_pcEditNodeListButton, 0, wxALL, 1);
		m_pcShowHideButton = new wxButton(this, SP_ID_BUTTON_SHOW_HIDE_NODE_LIST, wxT("Hide Node List"), wxDefaultPosition, wxSize(125, -1), 0);
		l_pcRowSizer->Add(m_pcShowHideButton, 0, wxALL, 1);
		m_pcRefreshButton = new wxButton(this, SP_ID_BUTTON_REFRESH, wxT("Refresh"));
		l_pcRowSizer->Add(m_pcRefreshButton, 0, wxALL, 1);
		m_pcConnectButton = new wxButton(this, SP_ID_BUTTON_DISCONNECT, wxT("Disconnect"));
		m_pcConnectButton->SetToolTip(wxT("Turn automatic refresh on/off"));
		l_pcRowSizer->Add(m_pcConnectButton, 0, wxALL, 1);
		l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 0, wxALL, 1);

		l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);
		m_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);
		SetSizerAndFit(m_pcMainSizer);

}

SP_DLG_ViewerWindow::~SP_DLG_ViewerWindow()
{
	if (m_pcResultViewer != NULL)
	{
		wxDELETE(m_pcResultViewer);
	}
}

void SP_DLG_ViewerWindow::CreateResultViewer(const wxString& p_sViewerType)
{
	wxString l_sViewerType=p_sViewerType;

	//set the current viewer type
	if (l_sViewerType.IsSameAs(wxT("Tabular")))
	{
		m_pcResultViewer = new SP_DS_TableViewer(this, m_pcViewerSizer);
	}
	else
	if (l_sViewerType.IsSameAs(wxT("xyPlot")))
	{
		m_pcResultViewer = new SP_DS_xyPlotViewer(this, m_pcViewerSizer);
	}
	else
	if (l_sViewerType.IsSameAs(wxT("Histogram")))
	{
		m_pcResultViewer = new SP_DS_HistogramPlotViewer(this, m_pcViewerSizer);
	}
	else
	{
		return;

	}

	m_pcResultViewer->Create();

	m_pcResultViewer->SetXAxisValues(&m_anXValues);
}


