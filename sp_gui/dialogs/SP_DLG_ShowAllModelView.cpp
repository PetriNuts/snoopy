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

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/extensions/SP_DS_Simulation.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_PlacesSelection.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_ChangeCurveAttributes.h"
#include "sp_gui/dialogs/SP_DLG_ShowAllModelView.h"
#include "sp_gui/dialogs/SP_DLG_ResultViewerProperties.h"

#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"


#include <wx/scrolwin.h>
#include <wx/regex.h>

IMPLEMENT_CLASS(SP_DLG_ShowAllModelView, wxFrame)

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

BEGIN_EVENT_TABLE( SP_DLG_ShowAllModelView, wxFrame )
END_EVENT_TABLE()
/*
 * some function in this class are defined in SP_DLG_Simulation.cpp file
 * please have a look
 */

SP_DLG_ShowAllModelView::SP_DLG_ShowAllModelView(SP_DLG_Simulation* p_pcWnd, SP_DS_Metadata* p_pcModelView, SP_DS_Graph* p_pcGraph) :
        SP_DLG_ViewerWindow(p_pcWnd),
		m_pcModelView(p_pcModelView),
		m_pcGraph(p_pcGraph)
{
	if (m_pcModelView == NULL)
		return;

	//use this to check for coloured simulation
	m_bIsColouredSimulation = m_pcModelView->GetAttribute(wxT("OutputType")) != nullptr;


	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("Name"));

	if (l_pcAttribute == NULL)
		return;

	wxString l_sViewName = l_pcAttribute->GetValueString();

	SetTitle(l_sViewName);

	wxString l_sViewerType=GetViewerType();

	CreateResultViewer(l_sViewerType);

	LoadView(m_pcResultViewer, m_pcModelView);

	m_pcOutputViewerType->SetStringSelection(m_pcModelView->GetAttribute(wxT("ViewerType"))->GetValueString());

//	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);
	wxString m_sNetClassName = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();//Added by G.A 
	if (m_sNetClassName.Contains(wxT("Fuzzy")))
	{
		m_slider->SetRange(0, m_anXValues.size());
		//m_pscrollBar->SetRange(m_anXValues.size());//required when loading a saved view
		//m_pscrollBar->Refresh();
		this->RefreshWindow();
	}
	wxSize temp = m_pcMainSizer->GetSize();
//	cout << temp.GetWidth() << " " << temp.GetHeight() << endl;

	wxString temp1 = m_pcModelView->GetAttribute(wxT("WindowWidth"))->GetValueString();
	long l_WindowWidth = 0;
	temp1.ToLong(&l_WindowWidth);

	wxString temp2 = m_pcModelView->GetAttribute(wxT("WindowHeight"))->GetValueString();
	long l_WindowHeight = 0;
	temp2.ToLong(&l_WindowHeight);
	if (l_WindowHeight <= 633)
	{
		l_WindowHeight += (633 - l_WindowHeight);
	}
	else
	{
		l_WindowHeight += 33;
	}
	temp.SetHeight(l_WindowHeight + 177);
	temp.SetWidth(l_WindowWidth + 284);

	this->SetSize(temp);

	wxPoint pos = p_pcWnd->GetPosition();
	wxSize size = p_pcWnd->GetSize();
	pos.x += size.GetX() + 10;
	SetPosition(pos);

	//by george, for loading default variables of the view
	wxString l_TempClassName = m_pcGraph->GetNetclass()->GetName();

		if (l_TempClassName == SP_DS_COLSPN_CLASS
			|| l_TempClassName == SP_DS_COLCPN_CLASS
			|| l_TempClassName == SP_DS_COLHPN_CLASS
			|| l_TempClassName == SP_DS_FUZZY_ColCPN_CLASS//by george
			|| l_TempClassName == SP_DS_FUZZY_ColSPN_CLASS//by george
			|| l_TempClassName == SP_DS_FUZZY_ColHPN_CLASS)//by george
		{
			//m_pcParentWnd->SaveCurrentView();

			SP_DLG_ColPlacesSelection* l_pcDlg = new SP_DLG_ColPlacesSelection(m_pcParentWnd, m_pcModelView, this);

			if (l_pcDlg)
			{
				l_pcDlg->DoSave();
				m_pcParentWnd->InitializeViews();
				m_pcParentWnd->LoadData(true);
				RefreshWindow();
				SP_Core::Instance()->GetRootDocument()->Modify(true);
			}

			l_pcDlg->Destroy();
		}
		else
		{
			//m_pcParentWnd->SaveCurrentView();

			SP_DLG_PlacesSelection* l_pcDlg = new SP_DLG_PlacesSelection(m_pcModelView, this);
			l_pcDlg->DoSave();
			if (l_pcDlg)
			{
				m_pcParentWnd->InitializeViews();
				m_pcParentWnd->LoadData(true);
				RefreshWindow();
				SP_Core::Instance()->GetRootDocument()->Modify(true);
			}
			l_pcDlg->Destroy();
		}
}
void SP_DLG_ShowAllModelView::OnChangeThumbPage( wxScrollEvent&  event)//wxCommandEvent&
{
	long dtimePoint = m_pscrollBar->GetThumbPosition();
	wxString sTimePoint;
	sTimePoint << dtimePoint;
	m_pscrollBar->SetToolTip(sTimePoint);
	m_pscrollBar->Refresh();

}
wxString SP_DLG_ShowAllModelView::GetViewerType()
{
	  SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("ViewerType"));

	  if (l_pcAttribute == NULL)
		  return wxT("");

	wxString l_sViewerType = l_pcAttribute->GetValueString();

	return l_sViewerType;
}

SP_DLG_ShowAllModelView::~SP_DLG_ShowAllModelView()
{
}

void SP_DLG_ShowAllModelView::OnItemDoubleClick(wxCommandEvent& p_cEvent) {
	int l_nCurveIndex = p_cEvent.GetSelection();

	wxString l_sColor = m_pcResultViewer->GetCurveColor(l_nCurveIndex);
	int l_nLineWidth = m_pcResultViewer->GetCurveLineWidth(l_nCurveIndex);
	int l_nLineStyle = m_pcResultViewer->GetCurveLineStyle(l_nCurveIndex);

	SP_DLG_ChangeCurveAttributes l_dial(this, l_sColor, l_nLineWidth, l_nLineStyle);

	if (l_dial.ShowModal() == wxID_OK)
	{

		wxColour l_nColor = l_dial.GetLineColor();
		int l_nLineWidth = l_dial.GetLineWidth();
		int l_nLineStyle = l_dial.GetLineStyle();

		SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcModelView->GetAttribute(wxT("CurveInfo")));
		l_pcCurveInfoList->SetCell(l_nCurveIndex, 3, l_nColor.GetAsString(wxC2S_HTML_SYNTAX));
		l_pcCurveInfoList->SetCell(l_nCurveIndex, 4, wxString::Format(wxT("%i"), l_nLineWidth));
		l_pcCurveInfoList->SetCell(l_nCurveIndex, 5, wxString::Format(wxT("%i"), l_nLineStyle));

		RefreshCurrentWindow(l_nCurveIndex, l_nColor.GetAsString(wxC2S_HTML_SYNTAX), l_dial.GetLineWidth(), l_dial.GetLineStyle());
	}
}

void SP_DLG_ShowAllModelView::OnPlaceCheckUncheck(wxCommandEvent& p_cEvent) {
	//get item index
	unsigned int l_nSelection = p_cEvent.GetSelection();

	//get check state
	bool l_bCheckState = m_pcPlaceChoiceCheckListBox->IsChecked(l_nSelection);

	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	l_pcPlaceIdList->SetCell(l_nSelection, 2, wxString::Format(wxT("%d"), l_bCheckState));

	RefreshWindow();
}

void SP_DLG_ShowAllModelView::OnSelectClearAllItems(wxCommandEvent& p_cEvent)
{
	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("CurveInfo"));

	CHECK_POINTER(l_pcAttribute, return);

	SP_DS_ColListAttribute* l_pcCurveInfo = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	if (m_pcSelectClearAllChkBox->IsChecked())
	{
		for (unsigned int l_nCurve = 0; l_nCurve < l_pcCurveInfo->GetRowCount(); l_nCurve++)
		{
			l_pcCurveInfo->SetCell(l_nCurve, 2, wxT("1"));
		}
	}
	else
	{
		for (unsigned int l_nCurve = 0; l_nCurve < l_pcCurveInfo->GetRowCount(); l_nCurve++)
		{
			l_pcCurveInfo->SetCell(l_nCurve, 2, wxT("0"));
		}
	}

	RefreshWindow();
}

void SP_DLG_ShowAllModelView::OnEditViewerProperties(wxCommandEvent& p_cEvent)
{
	SP_DLG_ResultViewerProperties* l_pcViewerProperties = new SP_DLG_ResultViewerProperties(m_pcResultViewer, this);

	if (l_pcViewerProperties->ShowModal() == wxID_OK)
	{
		m_pcResultViewer->SaveViewToSnoopyFormat(m_pcModelView);
		RefreshWindow();
		SP_Core::Instance()->GetRootDocument()->Modify(true);
	}

	l_pcViewerProperties->Destroy();

	wxSize temp;
	wxString temp1 = m_pcModelView->GetAttribute(wxT("WindowWidth"))->GetValueString();
	long l_WindowWidth = 0;
	temp1.ToLong(&l_WindowWidth);

	wxString temp2 = m_pcModelView->GetAttribute(wxT("WindowHeight"))->GetValueString();
	long l_WindowHeight = 0;
	temp2.ToLong(&l_WindowHeight);

	if (l_WindowHeight <= 633)
	{
		l_WindowHeight += (633 - l_WindowHeight);
	}
	else
	{
		l_WindowHeight += 33;
	}
	temp.SetHeight(l_WindowHeight + 177);
	temp.SetWidth(l_WindowWidth + 284);

	this->SetSize(temp);
}

void SP_DLG_ShowAllModelView::OnChangeResultViewer(wxCommandEvent& event)
{
	//Change the current view viewer
	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("ViewerType"));
	CHECK_POINTER(l_pcAttribute, return);
	if (m_pcOutputViewerType->GetStringSelection() == wxT("Tabular"))
	{
		m_pcOutputExportType->Delete(1);
		m_pcXAxis->Enable(false);
		m_pcOutputExportType->SetSelection(0);
		l_pcAttribute->SetValueString(wxT("Tabular"));
	}
	else
	if (m_pcOutputViewerType->GetStringSelection() == wxT("xyPlot"))
	{
		m_pcXAxis->Enable(true);
		if (m_pcOutputExportType->GetCount() != 2)
		{
			m_pcOutputExportType->Append(wxT("Image Export"));
		}
		l_pcAttribute->SetValueString(wxT("xyPlot"));
	}
	else
	if (m_pcOutputViewerType->GetStringSelection() == wxT("Histogram"))
	{
		if (m_pcOutputExportType->GetCount() != 2)
		{
			m_pcOutputExportType->Append(wxT("Image Export"));
		}
		m_pcXAxis->Enable(false);
		l_pcAttribute->SetValueString(wxT("Histogram"));
	}

	m_pcParentWnd->LoadData(false);
	RefreshWindow();
}

void SP_DLG_ShowAllModelView::OnExportClick(wxCommandEvent& event)
{
	if (m_pcOutputExportType->GetSelection() == 1)
	{
		CHECK_POINTER(m_pcResultViewer, return);

		CHECK_POINTER(m_pcModelView, return);

		wxString l_sCurrentViewerType = GetViewAttributeValue(m_pcModelView, wxT("ViewerType"));

		if (l_sCurrentViewerType.IsSameAs(wxT("xyPlot")) || l_sCurrentViewerType.IsSameAs(wxT("Histogram")))
		{
			wxFileName l_FileName{m_pcParentWnd->GetDirectExportFilename()};
			wxString l_sDefaultName = l_FileName.GetName() + wxT("_")
									  + GetViewAttributeValue(m_pcModelView, wxT("Name"));

			wxFileDialog* l_pcSaveDlg = new wxFileDialog(this, wxT("Export file name"), l_FileName.GetPath(), l_sDefaultName, wxT(""), wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxSTAY_ON_TOP);

			l_pcSaveDlg->SetWildcard(dynamic_cast<SP_DS_PlotViewer*>(m_pcResultViewer)->GetSupportedSaveExtensions());
			l_pcSaveDlg->SetFilterIndex(2);
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
	else
	{
		m_pcParentWnd->OnExportClicked(this, m_pcOutputExportType->GetSelection());

	}
}

void SP_DLG_ShowAllModelView::OnChangeXAxis(wxCommandEvent& event)
{
	m_pcParentWnd->OnEditXAxis(this);
	RefreshWindow();
}

void SP_DLG_ShowAllModelView::OnEditNodeList(wxCommandEvent& event)
{
	wxString l_TempClassName = m_pcGraph->GetNetclass()->GetName();

	if (l_TempClassName == SP_DS_COLSPN_CLASS
		|| l_TempClassName == SP_DS_COLCPN_CLASS
		|| l_TempClassName == SP_DS_COLHPN_CLASS
		|| l_TempClassName == SP_DS_FUZZY_ColCPN_CLASS//by george
		|| l_TempClassName == SP_DS_FUZZY_ColSPN_CLASS//by george
		|| l_TempClassName == SP_DS_FUZZY_ColHPN_CLASS)//by george
	{
		//m_pcParentWnd->SaveCurrentView();

		SP_DLG_ColPlacesSelection* l_pcDlg = new SP_DLG_ColPlacesSelection(m_pcParentWnd, m_pcModelView, this);

		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			m_pcParentWnd->InitializeViews();
			m_pcParentWnd->LoadData(true);
			RefreshWindow();
			SP_Core::Instance()->GetRootDocument()->Modify(true);
		}

		l_pcDlg->Destroy();
	}
	else
	{
		//m_pcParentWnd->SaveCurrentView();

		SP_DLG_PlacesSelection* l_pcDlg = new SP_DLG_PlacesSelection(m_pcModelView, this);

		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			m_pcParentWnd->InitializeViews();
			m_pcParentWnd->LoadData(true);
			RefreshWindow();
			SP_Core::Instance()->GetRootDocument()->Modify(true);
		}
		l_pcDlg->Destroy();
	}
}


bool SP_DLG_ShowAllModelView::LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView)
{

	CHECK_POINTER(m_pcParentWnd, return false);

	m_pcParentWnd->LoadData(p_pcResultViewer, p_pcModelView);
	//Update the current viewer
	m_pcParentWnd->UpdateViewer(p_pcModelView);

		SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcModelView->GetAttribute(wxT("CurveInfo")));
	if (l_pcCurveInfoList == NULL)
		return false;
	m_pcPlaceChoiceCheckListBox->Clear();

	/////////////////////////////////////////////////////
	wxString l_sViewerName = p_pcModelView->GetAttribute(wxT("Name"))->GetValueString();
	//if the selected view is the defuat view, then we initialise it with reg exp "."
	if (l_sViewerName.IsSameAs(wxT("Default View"))|| l_sViewerName.IsSameAs(wxT("Main Plot")))
	{
		wxString l_sOutputType;
		//by george
		if (p_pcModelView)
		{
			SP_DS_Attribute* l_pcAttr = p_pcModelView->GetAttribute(wxT("OutputType"));
			if (l_pcAttr)
				l_sOutputType << l_pcAttr->GetValueString();

			if (!l_sOutputType.IsEmpty())
			{
				for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
				{
					if (l_pcCurveInfoList->GetCell(l_nRow, 1) != l_sOutputType)
					{
						l_pcCurveInfoList->RemoveRow(l_nRow);
						l_nRow = 0;
					}
				}
			}
		}

		for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
		{
			wxString l_sType = l_pcCurveInfoList->GetCell(l_nRow, 1);

			if (l_sType == l_sOutputType)//by george
			{
				wxString l_sItem = l_pcCurveInfoList->GetCell(l_nRow, 6);
				wxString l_sSelected = l_pcCurveInfoList->GetCell(l_nRow, 2);
				m_pcPlaceChoiceCheckListBox->Insert(l_pcCurveInfoList->GetCell(l_nRow, 6), m_pcPlaceChoiceCheckListBox->GetCount());
				wxArrayString ss = m_pcPlaceChoiceCheckListBox->GetStrings();
				//show selected curves
				LoadCurveSetting(p_pcResultViewer, l_pcCurveInfoList, l_nRow);

			}

			if (l_sOutputType.IsEmpty())//uncoloured net
			{
				m_pcPlaceChoiceCheckListBox->Insert(l_pcCurveInfoList->GetCell(l_nRow, 6), m_pcPlaceChoiceCheckListBox->GetCount());
				//show selected curves
				LoadCurveSetting(p_pcResultViewer, l_pcCurveInfoList, l_nRow);
				wxString ddd = l_pcCurveInfoList->GetCell(l_nRow, 6);
			}
		}
	}
	else
	{//a pre-defined view
		for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
		{
			m_pcPlaceChoiceCheckListBox->Insert(l_pcCurveInfoList->GetCell(l_nRow, 6), m_pcPlaceChoiceCheckListBox->GetCount());
			//show selected curves
			LoadCurveSetting(p_pcResultViewer, l_pcCurveInfoList, l_nRow);
			wxString ddd = l_pcCurveInfoList->GetCell(l_nRow, 6);
		}
	}

	m_pcParentWnd->CalculateXAxisValues(p_pcModelView, m_anXValues);

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
	m_pcPlaceChoiceCheckListBox->Check(p_nRow, l_bIsSelected);
	//load Curve color
	wxString l_sColor = p_pcCurveInfoList->GetCell(p_nRow, 3);
	p_pcViewer->SetCurveColor(p_nRow, l_sColor);

	//load Line width
	wxString l_sLineWidthStr = p_pcCurveInfoList->GetCell(p_nRow, 4);
	long l_nLineWidth = 0;
	l_sLineWidthStr.ToLong(&l_nLineWidth);
	p_pcViewer->SetCurveLineWidth(p_nRow, l_nLineWidth);

	//load Line style
	wxString l_sLineStyleStr = p_pcCurveInfoList->GetCell(p_nRow, 5);
	long l_nLineStyle = 0;
	l_sLineStyleStr.ToLong(&l_nLineStyle);
	p_pcViewer->SetCurveLineStyle(p_nRow, l_nLineStyle);
}
void SP_DLG_ShowAllModelView::OnRefresh(wxCommandEvent& event)
{
	RefreshWindow();
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

	wxString l_sViewerType=GetViewerType();

	CreateResultViewer(l_sViewerType);

	LoadView(m_pcResultViewer, m_pcModelView);

}

void SP_DLG_ShowAllModelView::RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle)
{
	if (m_bIsDisconnected)
	{
		return; //no need to refresh
	}

	m_pcResultViewer->Destroy();

	wxDELETE(m_pcResultViewer);

	wxString l_sViewerType=GetViewerType();

	CreateResultViewer(l_sViewerType);
	LoadView(m_pcResultViewer, m_pcModelView);
	m_pcResultViewer->SetCurveColor(p_nCurveIndex, p_nColor);
	m_pcResultViewer->SetCurveLineWidth(p_nCurveIndex, p_nLineWidth);
	m_pcResultViewer->SetCurveLineStyle(p_nCurveIndex, p_nLineStyle);
	m_pcResultViewer->Update();
}

void SP_DLG_ShowAllModelView::OnWindowActivate(wxActivateEvent& event)
{
	if (event.GetActive())
	{
		if (m_pcParentWnd != NULL)
		{
			m_pcParentWnd->ChangeViewTo(m_pcModelView);
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

wxString SP_DLG_ShowAllModelView::GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName)
{
	CHECK_POINTER(p_pcView, return wxT(""));
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(p_sAttributeName);

	CHECK_POINTER(l_pcAttribute, return wxT(""));

	return l_pcAttribute->GetValueString();
}

