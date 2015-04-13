/*
 * SP_DLG_ShowAllModelView.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 09.08.2010
 * @Description
 * Display all model views in one widow
 */ //======================================================================================
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

#include "sp_gui/dialogs/SP_DLG_ShowAllModelView.h"

#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"
#include <wx/scrolwin.h>

IMPLEMENT_CLASS(SP_DLG_ShowAllModelView, wxDialog)

enum
{
	SP_ID_BUTTON_REFRESH,

	SP_ID_BUTTON_EXPORT_TO_IMAGE,

	SP_ID_BUTTON_DISCONNECT
};

BEGIN_EVENT_TABLE(SP_DLG_ShowAllModelView,wxDialog)

EVT_BUTTON(SP_ID_BUTTON_REFRESH,SP_DLG_ShowAllModelView::OnRefresh)
EVT_BUTTON(wxID_CANCEL,SP_DLG_ShowAllModelView::OnClose)
EVT_BUTTON(SP_ID_BUTTON_EXPORT_TO_IMAGE,SP_DLG_ShowAllModelView::OnImageExport)

EVT_BUTTON(SP_ID_BUTTON_DISCONNECT,SP_DLG_ShowAllModelView::OnDisconnect)

EVT_ACTIVATE(SP_DLG_ShowAllModelView::OnWindowActivate)
END_EVENT_TABLE()

SP_DLG_ShowAllModelView::SP_DLG_ShowAllModelView(wxWindow* p_pcWnd, SP_DS_Metadata* p_pcModelView) :
		wxDialog(p_pcWnd, -1, wxT("Show all views"), wxPoint(20, 20), wxSize(800, 750), wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE), 
		m_pcModelView(p_pcModelView), m_pcParentWnd(p_pcWnd), m_bIsDisconnected(false)
{
	wxSizer* l_pcMainSizer = new wxBoxSizer(wxVERTICAL);

	if (m_pcModelView == NULL)
		return;

	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("Name"));

	if (l_pcAttribute == NULL)
		return;

	wxString l_sViewName = l_pcAttribute->GetValueString();

	SetTitle(l_sViewName);

	m_pcViewerSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	l_pcMainSizer->Add(m_pcViewerSizer, 1, wxEXPAND);

	CreateResultViewer();

	LoadView(m_pcResultViewer, m_pcModelView);

	wxSizer* l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);

	m_pcRefreshButton = new wxButton(this, SP_ID_BUTTON_REFRESH, wxT("Refresh"));

	l_pcRowSizer->Add(m_pcRefreshButton, 0, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_EXPORT_TO_IMAGE, wxT("Image export")), 0, wxALL, 5);

	m_pcConnectButton = new wxButton(this, SP_ID_BUTTON_DISCONNECT, wxT("Disconnect"));

	m_pcConnectButton->SetToolTip(wxT("Turn automatic refresh on/off"));

	l_pcRowSizer->Add(m_pcConnectButton, 0, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 0, wxALL, 5);

	l_pcMainSizer->Add(l_pcRowSizer, 0, wxALL, 5);

	SetSizerAndFit(l_pcMainSizer);

	Center();
}

SP_DLG_ShowAllModelView::~SP_DLG_ShowAllModelView()
{
	if (m_pcResultViewer != NULL)
	{
		wxDELETE(m_pcResultViewer);
	}

}

bool SP_DLG_ShowAllModelView::LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView)
{

	CHECK_POINTER(m_pcParentWnd, return false);

	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->LoadData(p_pcResultViewer, p_pcModelView);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcModelView->GetAttribute(wxT("CurveInfo")));
	if (l_pcCurveInfoList == NULL)
		return false;

	for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	{
		//show selected curves
		LoadCurveSetting(p_pcResultViewer, l_pcCurveInfoList, l_nRow);

	}

	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->CalculateXAxisValues(p_pcModelView, m_anXValues);

	p_pcResultViewer->SetXAxisValues(&m_anXValues);

	p_pcResultViewer->LoadViewFromSnoopyFormat(p_pcModelView);
	p_pcResultViewer->Update();

	return true;
}
void SP_DLG_ShowAllModelView::LoadCurveSetting(SP_DS_ResultViewer* p_pcViewer, SP_DS_ColListAttribute* p_pcCurveInfoList, const unsigned int& p_nRow)
{
	//Get the item ID
	wxString l_sItemID = p_pcCurveInfoList->GetCell(p_nRow, 0);

	//load the user selection
	wxString l_sSelectionStr = p_pcCurveInfoList->GetCell(p_nRow, 2);
	long l_nSelected = 0;
	l_sSelectionStr.ToLong(&l_nSelected);

	bool l_bIsSelected = l_nSelected > 0 ? true : false;

	p_pcViewer->ShowCurve(p_nRow, l_bIsSelected);

	//load Curve color
	wxString l_sColor = p_pcCurveInfoList->GetCell(p_nRow, 3);
	p_pcViewer->SetCurveColor(p_nRow, l_sColor);

	//load Line width
	wxString l_sLineWidthStr = p_pcCurveInfoList->GetCell(p_nRow, 4);
	long l_nLineWidth = 0;
	l_sLineWidthStr.ToLong(&l_nLineWidth);
	p_pcViewer->SetCurveLineWidth(p_nRow, l_nLineWidth);

	//load Line width
	wxString l_sLineStyleStr = p_pcCurveInfoList->GetCell(p_nRow, 5);
	long l_nLineStyle = 0;
	l_sLineStyleStr.ToLong(&l_nLineStyle);
	p_pcViewer->SetCurveLineStyle(p_nRow, l_nLineStyle);
}
void SP_DLG_ShowAllModelView::OnRefresh(wxCommandEvent& event)
{
	RefreshWindow();
}

void SP_DLG_ShowAllModelView::CreateResultViewer()
{
	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("ViewerType"));

	if (l_pcAttribute == NULL)
		return;

	wxString l_sViewerType = l_pcAttribute->GetValueString();

	//set the current viewer type
	if (l_sViewerType.IsSameAs(wxT("Tabular")))
	{
		m_pcResultViewer = new SP_DS_TableViewer(this, m_pcViewerSizer);
	}
	else if (l_sViewerType.IsSameAs(wxT("xyPlot")))
	{
		m_pcResultViewer = new SP_DS_xyPlotViewer(this, m_pcViewerSizer);
	}
	else if (l_sViewerType.IsSameAs(wxT("Histogram")))
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
void SP_DLG_ShowAllModelView::OnClose(wxCommandEvent& event)
{
	if (m_pcParentWnd != NULL)
	{
		(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->RemoveExternalWindow(this);
	}

	Destroy();
}
void SP_DLG_ShowAllModelView::RemoveExternalWindowsPointer()
{
	//do not allow refresh
	m_pcRefreshButton->Enable(false);

	//disable refresh switching
	m_pcConnectButton->Enable(false);

	//no parent window
	m_pcParentWnd = NULL;

	//no current view
	m_pcModelView = NULL;
}
void SP_DLG_ShowAllModelView::RefreshWindow()
{
	if (m_bIsDisconnected)
	{
		return; //no need to refresh
	}

	m_pcResultViewer->Destroy();

	wxDELETE(m_pcResultViewer);

	CreateResultViewer();

	LoadView(m_pcResultViewer, m_pcModelView);
}

void SP_DLG_ShowAllModelView::OnWindowActivate(wxActivateEvent& event)
{
	if (event.GetActive())
	{
		if (m_pcParentWnd != NULL)
		{
			(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->ChangeViewTo(m_pcModelView);
		}
#ifndef __WXMSW__
		Raise();
	}
	else
	{
		Lower();
#endif
	}
	event.Skip();
}

void SP_DLG_ShowAllModelView::OnImageExport(wxCommandEvent &event)
{

	CHECK_POINTER(m_pcResultViewer, return);

	CHECK_POINTER(m_pcModelView, return);

	wxString l_sCurrentViewerType = GetViewAttributeValue(m_pcModelView, wxT("ViewerType"));

	if (l_sCurrentViewerType.IsSameAs(wxT("xyPlot")) || l_sCurrentViewerType.IsSameAs(wxT("Histogram")))
	{

		wxString l_sDefaultName = GetViewAttributeValue(m_pcModelView, wxT("Name"));

		wxFileDialog* l_pcSaveDlg = new wxFileDialog(this, wxT("Export file name"), wxT(""), l_sDefaultName, wxT(""), wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxSTAY_ON_TOP);

		l_pcSaveDlg->SetWildcard(dynamic_cast<SP_DS_PlotViewer*>(m_pcResultViewer)->GetSupportedSaveExtensions());

		if (l_pcSaveDlg->ShowModal() == wxID_OK)
		{
			wxString l_sFileName = l_pcSaveDlg->GetPath();
			int l_nExportType = l_pcSaveDlg->GetFilterIndex();

			if (l_sFileName.BeforeLast(wxT('.')) != wxT(""))
			{
				l_sFileName = l_sFileName.BeforeLast(wxT('.'));
			}

			dynamic_cast<SP_DS_PlotViewer*>(m_pcResultViewer)->ExportToImageFile(l_sFileName, l_nExportType);

		}

		l_pcSaveDlg->Destroy();
	}
}

wxString SP_DLG_ShowAllModelView::GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}

void SP_DLG_ShowAllModelView::OnDisconnect(wxCommandEvent &event)
{
	if (!m_bIsDisconnected)
	{
		//mark this window as disconnected from getting update from the simulator
		m_bIsDisconnected = true;

		m_pcRefreshButton->Enable(false);

		m_pcConnectButton->SetLabel(wxT("Connect"));
	}
	else
	{
		m_bIsDisconnected = false;

		m_pcRefreshButton->Enable(true);

		m_pcConnectButton->SetLabel(wxT("Disconnect"));
	}
}

