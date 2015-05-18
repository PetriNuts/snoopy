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
#include <wx/regex.h>

IMPLEMENT_CLASS(SP_DLG_ShowAllModelView, wxDialog)

enum
{
	SP_ID_BUTTON_REFRESH,
	SP_ID_BUTTON_DISCONNECT,
	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS,
	SP_ID_COMBOBOX_RESULT_VIEWER_TYPE,
	SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES,
	SP_ID_BUTTON_EXPORT,
	SP_ID_CHANGE_X_AXIS,
	SP_ID_BUTTON_EDIT_NODE_LIST,
	SP_ID_BUTTON_SHOW_HIDE_NODE_LIST
};

BEGIN_EVENT_TABLE(SP_DLG_ShowAllModelView,wxDialog)

EVT_BUTTON(SP_ID_BUTTON_REFRESH,SP_DLG_ShowAllModelView::OnRefresh)
EVT_BUTTON(wxID_CANCEL,SP_DLG_ShowAllModelView::OnClose)

EVT_BUTTON(SP_ID_BUTTON_DISCONNECT,SP_DLG_ShowAllModelView::OnDisconnect)

EVT_LISTBOX_DCLICK(SP_ID_CHECKLISTBOX_PLACE_CHOICE,SP_DLG_ShowAllModelView::OnItemDoubleClick)
EVT_CHECKLISTBOX( SP_ID_CHECKLISTBOX_PLACE_CHOICE, SP_DLG_ShowAllModelView::OnPlaceCheckUncheck)
EVT_CHECKBOX( SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, SP_DLG_ShowAllModelView::OnSelectClearAllItems)
EVT_COMBOBOX( SP_ID_COMBOBOX_RESULT_VIEWER_TYPE, SP_DLG_ShowAllModelView :: OnChangeResultViewer )
EVT_BUTTON( SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, SP_DLG_ShowAllModelView :: OnEditViewerProperties)
EVT_BUTTON( SP_ID_BUTTON_EXPORT, SP_DLG_ShowAllModelView :: OnExportClick)
EVT_BUTTON( SP_ID_CHANGE_X_AXIS, SP_DLG_ShowAllModelView :: OnChangeXAxis)
EVT_BUTTON( SP_ID_BUTTON_EDIT_NODE_LIST, SP_DLG_ShowAllModelView :: OnEditNodeList)
EVT_BUTTON( SP_ID_BUTTON_SHOW_HIDE_NODE_LIST, SP_DLG_ShowAllModelView :: OnShowHideNodeList)

EVT_ACTIVATE(SP_DLG_ShowAllModelView::OnWindowActivate)
END_EVENT_TABLE()

/*
 * some function in this class are defined in SP_DLG_Simulation.cpp file
 * please have a look
 */

SP_DLG_ShowAllModelView::SP_DLG_ShowAllModelView(wxWindow* p_pcWnd, SP_DS_Metadata* p_pcModelView) :
		wxDialog(p_pcWnd, -1, wxT("Show all views"), wxPoint(700, 10), wxSize(800, 750),  wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxDEFAULT_DIALOG_STYLE),
		m_pcModelView(p_pcModelView), m_pcParentWnd(p_pcWnd), m_bIsDisconnected(false), m_bIsShown(true)
{
	if (m_pcModelView == NULL)
			return;
	wxSizer* l_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSizer* l_pcLeftSizer = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer = new wxBoxSizer(wxVERTICAL);

	l_pcMainSizer->Add(m_pcContentSizer, 1, wxALL | wxEXPAND);

	m_pcContentSizer->Add(l_pcLeftSizer, 1, wxALL | wxEXPAND, 5);
	m_pcContentSizer->Add(m_pcRightSizer, 0, wxALL | wxEXPAND, 5);

	wxSizer* l_pcPlaceChoiceSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Select an item to view")), wxVERTICAL);
	wxSizer* l_pcSelectDeselecteSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);

	m_pcRightSizer->Add(l_pcPlaceChoiceSizer, 1, wxEXPAND);
	m_pcRightSizer->Add(l_pcSelectDeselecteSizer, 0, wxEXPAND);

	m_pcPlaceChoiceCheckListBox = new wxCheckListBox(this, SP_ID_CHECKLISTBOX_PLACE_CHOICE, wxDefaultPosition, wxSize(200, 200));
	l_pcPlaceChoiceSizer->Add(m_pcPlaceChoiceCheckListBox, 1, wxALL | wxEXPAND, 5);

	//select/deselect all
	m_pcSelectClearAllChkBox = new wxCheckBox(this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
	m_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);
	l_pcSelectDeselecteSizer->Add(m_pcSelectClearAllChkBox, 1, wxEXPAND);



	SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("Name"));

	if (l_pcAttribute == NULL)
		return;

	wxString l_sViewName = l_pcAttribute->GetValueString();

	SetTitle(l_sViewName);
	m_IsSelectedPlace.Clear();
	m_IsSelectedTransition.Clear();
	wxString l_RegExOutputType = m_pcModelView->GetAttribute(wxT("RegExOutputType"))->GetValueString();
	wxString l_sElementType = m_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
	int l_nSize, l_nSize1 = 0;
	if (l_sElementType == wxT("Place")) {
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayUnPlaces.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayColPlaces.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayAuxPlaces.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
	} else {
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayUnTranstions.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayColTranstions.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
			l_nSize = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayAuxtranstions.GetCount();
			if (l_nSize > l_nSize1) l_nSize1 = l_nSize;
	}

	for (int i = 0; i < l_nSize1; i++) {
		m_IsSelectedPlace.Add(1);
		m_PlaceLineStyle.Add(0);
		m_PlaceLineWidth.Add(2);
		m_PlaceColor.Add((dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->GetColourString(i));
	}

	for (int i = 0; i < l_nSize1; i++) {
		m_IsSelectedTransition.Add(1);
		m_TransitionLineStyle.Add(0);
		m_TransitionLineWidth.Add(2);
		m_TransitionColor.Add((dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->GetColourString(i));
	}

	m_pcViewerSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	l_pcLeftSizer->Add(m_pcViewerSizer, 1, wxALL | wxEXPAND);

	CreateResultViewer();

	LoadView(m_pcResultViewer, m_pcModelView);
	wxSizer *l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSizer *l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer Type")), wxHORIZONTAL);
	m_pcOutputViewerType = new wxComboBox(this, SP_ID_COMBOBOX_RESULT_VIEWER_TYPE, wxT(""), wxDefaultPosition, wxSize(129, -1), 0, NULL, wxCB_READONLY);
	m_pcOutputViewerType->Append(wxT("Tabular"));
	m_pcOutputViewerType->Append(wxT("xyPlot"));
	m_pcOutputViewerType->Append(wxT("Histogram"));
	m_pcOutputViewerType->SetValue(m_pcModelView->GetAttribute(wxT("ViewerType"))->GetValueString());

	l_pcRowSizer->Add(m_pcOutputViewerType, 0, wxALL, 1);
	m_pcViewerTypeButton = new wxButton(this, SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, wxT("Edit"));

	l_pcRowSizer->Add(m_pcViewerTypeButton, 0, wxALL, 1);
	l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);
	m_pcXAxis = new wxButton(this, SP_ID_CHANGE_X_AXIS, wxT("Change X Axis"));
	l_pcRowSizer->Add(m_pcXAxis, 0, wxALL, 1);
	m_pcEditNodeListButton = new wxButton(this, SP_ID_BUTTON_EDIT_NODE_LIST, wxT("Edit Node List"));
	l_pcRowSizer->Add(m_pcEditNodeListButton, 0, wxALL, 1);
	m_pcShowHideButton = new wxButton(this, SP_ID_BUTTON_SHOW_HIDE_NODE_LIST, wxT("Hide Node List"), wxDefaultPosition, wxSize(125, -1), 0);
	l_pcRowSizer->Add(m_pcShowHideButton, 0, wxALL, 1);

	l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);

	l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Export")), wxHORIZONTAL);
	m_pcOutputExportType = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	m_pcOutputExportType->Append(wxT("CSV Export"));
	m_pcOutputExportType->Append(wxT("Image Export"));
	m_pcOutputExportType->SetSelection(0);
	l_pcRowSizer->Add(m_pcOutputExportType, 0, wxALL, 1);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_EXPORT, wxT("Export")), 0, wxALL, 1);

	l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);
	m_pcRefreshButton = new wxButton(this, SP_ID_BUTTON_REFRESH, wxT("Refresh"));
	l_pcRowSizer->Add(m_pcRefreshButton, 0, wxALL, 1);
	m_pcConnectButton = new wxButton(this, SP_ID_BUTTON_DISCONNECT, wxT("Disconnect"));
	m_pcConnectButton->SetToolTip(wxT("Turn automatic refresh on/off"));
	l_pcRowSizer->Add(m_pcConnectButton, 0, wxALL, 1);
	l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 0, wxALL, 1);

	l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);
	SetSizerAndFit(l_pcMainSizer);


	wxSize temp = l_pcMainSizer->GetSize();
	cout << temp.GetWidth() << " " << temp.GetHeight()<< endl;
	temp.SetHeight( wxSystemSettings::GetMetric(wxSYS_SCREEN_X));

	wxString temp1 = m_pcModelView->GetAttribute(wxT("ChartWidth"))->GetValueString();
	long l_ChartWidth = 0;
	temp1.ToLong(&l_ChartWidth);

	wxString temp2 = m_pcModelView->GetAttribute(wxT("ChartHeight"))->GetValueString();
	long l_ChartHeight = 0;
	temp2.ToLong(&l_ChartHeight);
	if (l_ChartHeight <= 633) {
		l_ChartHeight += (633 - l_ChartHeight);
	} else {
		l_ChartHeight += 33;
	}
	temp.SetHeight(l_ChartHeight + 180);
	temp.SetWidth(l_ChartWidth + 246);

	this->SetSize(temp);
}

SP_DLG_ShowAllModelView::~SP_DLG_ShowAllModelView()
{
	if (m_pcResultViewer != NULL)
	{
		wxDELETE(m_pcResultViewer);
	}

}

void SP_DLG_ShowAllModelView::OnItemDoubleClick(wxCommandEvent& p_cEvent) {
	unsigned int l_selection = p_cEvent.GetSelection();
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnItemDoubleClick(this, l_selection, m_pcPlaceChoiceCheckListBox->GetCount());
}

void SP_DLG_ShowAllModelView::OnPlaceCheckUncheck(wxCommandEvent& p_cEvent) {
	unsigned int l_selection = p_cEvent.GetSelection();
	wxString l_RegExString = m_pcModelView->GetAttribute(wxT("RegEx"))->GetValueString();
	if (l_RegExString != wxT("")) {
		wxString l_sElementType = m_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
		if (l_sElementType.IsSameAs(wxT("Place"))) {
			m_IsSelectedPlace[l_selection] = 1 - m_IsSelectedPlace[l_selection];
		} else if (l_sElementType.IsSameAs(wxT("Transition"))) {
			m_IsSelectedTransition[l_selection] = 1 - m_IsSelectedTransition[l_selection];
		}
	}
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnItemCheckUncheck(l_selection, m_pcPlaceChoiceCheckListBox->IsChecked(l_selection));
}

void SP_DLG_ShowAllModelView::OnSelectClearAllItems(wxCommandEvent& p_cEvent) {
	if (m_pcSelectClearAllChkBox->IsChecked()) {
		wxString l_RegExString = m_pcModelView->GetAttribute(wxT("RegEx"))->GetValueString();
		if (l_RegExString != wxT("")) {
			wxString l_sElementType = m_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
			if (l_sElementType.IsSameAs(wxT("Place"))) {
				for (unsigned int i = 0; i < m_IsSelectedPlace.GetCount(); i++)
					m_IsSelectedPlace[i] = 1;
			} else if (l_sElementType.IsSameAs(wxT("Transition"))) {
				for (unsigned int i = 0; i < m_IsSelectedTransition.GetCount(); i++)
					m_IsSelectedTransition[i] = 1;
			}
		}
		(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnClearPlaceList(true);
	} else {
		wxString l_RegExString = m_pcModelView->GetAttribute(wxT("RegEx"))->GetValueString();
		if (l_RegExString != wxT("")) {
			wxString l_sElementType = m_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
			if (l_sElementType.IsSameAs(wxT("Place"))) {
				for (unsigned int i = 0; i < m_IsSelectedPlace.GetCount(); i++)
					m_IsSelectedPlace[i] = 0;
			} else if (l_sElementType.IsSameAs(wxT("Transition"))) {
				for (unsigned int i = 0; i < m_IsSelectedTransition.GetCount(); i++)
					m_IsSelectedTransition[i] = 0;
			}
		}
		(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnClearPlaceList(false);
	}
}

void SP_DLG_ShowAllModelView::OnEditViewerProperties(wxCommandEvent& p_cEvent) {
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnEditViewerTypeProperties(this);

	wxSize temp;
	wxString temp1 = m_pcModelView->GetAttribute(wxT("ChartWidth"))->GetValueString();
	long l_ChartWidth = 0;
	temp1.ToLong(&l_ChartWidth);

	wxString temp2 = m_pcModelView->GetAttribute(wxT("ChartHeight"))->GetValueString();
	long l_ChartHeight = 0;
	temp2.ToLong(&l_ChartHeight);

	if (l_ChartHeight <= 633) {
		l_ChartHeight += (633 - l_ChartHeight);
	} else {
		l_ChartHeight += 33;
	}
	temp.SetHeight(l_ChartHeight + 180);
	temp.SetWidth(l_ChartWidth + 246);

	this->SetSize(temp);
}

void SP_DLG_ShowAllModelView::OnChangeResultViewer(wxCommandEvent& event) {
	unsigned int l_nLocation = m_pcOutputViewerType->GetSelection();
	if (m_pcOutputViewerType->GetStringSelection() == wxT("Tabular")) {
		m_pcOutputExportType->Delete(1);
		m_pcXAxis->Enable(false);
		m_pcOutputExportType->SetSelection(0);
	} else if (m_pcOutputViewerType->GetStringSelection() == wxT("xyPlot")) {
		m_pcXAxis->Enable(true);
		if (m_pcOutputExportType->GetCount() != 2) {
			m_pcOutputExportType->Append(wxT("Image Export"));
		}
	} else if (m_pcOutputViewerType->GetStringSelection() == wxT("Histogram")) {
		if (m_pcOutputExportType->GetCount() != 2) {
			m_pcOutputExportType->Append(wxT("Image Export"));
		}
		m_pcXAxis->Enable(false);
	}
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnChangingResultViewer(l_nLocation);
}

void SP_DLG_ShowAllModelView::OnExportClick(wxCommandEvent& event) {
	if (m_pcOutputExportType->GetSelection() == 1) {
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
	} else {
		(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnExportClicked(this, m_pcOutputExportType->GetSelection());

	}
}

void SP_DLG_ShowAllModelView::OnChangeXAxis(wxCommandEvent& event) {
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnEditXAxis(this);
}

void SP_DLG_ShowAllModelView::OnEditNodeList(wxCommandEvent& event) {
	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->OnEditOtherNodeList(this);
}

void SP_DLG_ShowAllModelView::OnShowHideNodeList(wxCommandEvent& event) {
	m_bIsShown = !m_bIsShown;
	if (m_bIsShown) {
		m_pcRightSizer->Show(true);
		m_pcShowHideButton->SetLabel("Hide Node List");
	} else {
		m_pcRightSizer->Show(false);
		m_pcShowHideButton->SetLabel("Show Node List");
	}
	m_pcContentSizer->Layout();
}

bool SP_DLG_ShowAllModelView::LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView)
{	//SP_LOGMESSAGE(wxT("Separate view load view"));

	CHECK_POINTER(m_pcParentWnd, return false);

	(dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->LoadData(p_pcResultViewer, p_pcModelView);

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcModelView->GetAttribute(wxT("CurveInfo")));
	if (l_pcCurveInfoList == NULL)
		return false;
	m_pcPlaceChoiceCheckListBox->Clear();

	wxString l_RegExString = p_pcModelView->GetAttribute(wxT("RegEx"))->GetValueString();
	wxRegEx l_RegEx;

	if (l_RegExString == wxT("")) {
		for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
		{
			m_pcPlaceChoiceCheckListBox->Insert(l_pcCurveInfoList->GetCell(l_nRow, 6), m_pcPlaceChoiceCheckListBox->GetCount());
			//show selected curves
			LoadCurveSetting(p_pcResultViewer, l_pcCurveInfoList, l_nRow);
		}
	} else {
		wxString l_RegExOutputType = m_pcModelView->GetAttribute(wxT("RegExOutputType"))->GetValueString();
		if(l_RegEx.Compile(l_RegExString, wxRE_DEFAULT)) {
			wxArrayString l_ArrayPlaces, l_ArrayTranstions;

			m_pcPlaceChoiceCheckListBox->Clear();
			wxString l_sElementType = p_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
			int l_nCurve = 0;
			if (l_sElementType.IsSameAs(wxT("Place"))) {
			//	SP_LOGMESSAGE("place");
				if (l_RegExOutputType == wxT("Unfolded")) {
					l_ArrayPlaces = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayUnPlaces;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayPlaces.GetCount(); l_nRow++)
					{
						wxString l_sName = l_ArrayPlaces[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedPlace[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_PlaceColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_PlaceLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_PlaceLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				} else if (l_RegExOutputType == wxT("Colored")) {
					l_ArrayPlaces = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayColPlaces;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayPlaces.GetCount(); l_nRow++)
					{

						wxString l_sName = l_ArrayPlaces[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedPlace[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_PlaceColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_PlaceLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_PlaceLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				} else {
					l_ArrayPlaces = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayAuxPlaces;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayPlaces.GetCount(); l_nRow++)
					{

						wxString l_sName = l_ArrayPlaces[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedPlace[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_PlaceColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_PlaceLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_PlaceLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				}

			} else if (l_sElementType.IsSameAs(wxT("Transition"))) {
			//	SP_LOGMESSAGE("transition");
				l_nCurve = 0;
				if (l_RegExOutputType == wxT("Unfolded")) {
					l_ArrayTranstions = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayUnTranstions;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayTranstions.GetCount(); l_nRow++)
					{
						wxString l_sName = l_ArrayTranstions[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedTransition[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_TransitionColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_TransitionLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_TransitionLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				} else if (l_RegExOutputType == wxT("Unfolded")) {
					l_ArrayTranstions = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayColTranstions;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayTranstions.GetCount(); l_nRow++)
					{
						wxString l_sName = l_ArrayTranstions[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedTransition[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_TransitionColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_TransitionLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_TransitionLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				} else {
					l_ArrayTranstions = (dynamic_cast<SP_DLG_Simulation*>(m_pcParentWnd))->m_ArrayAuxtranstions;
					for (unsigned int l_nRow = 0; l_nRow < l_ArrayTranstions.GetCount(); l_nRow++)
					{
						wxString l_sName = l_ArrayTranstions[l_nRow];
						if (l_RegEx.Matches(l_sName)) {
							m_pcPlaceChoiceCheckListBox->Insert(l_sName, m_pcPlaceChoiceCheckListBox->GetCount());
							int l_nSelected = m_IsSelectedTransition[l_nCurve];

							bool l_bIsSelected = l_nSelected > 0 ? true : false;
							p_pcResultViewer->ShowCurve(l_nCurve, l_bIsSelected);
							m_pcPlaceChoiceCheckListBox->Check(l_nCurve, l_bIsSelected);

							p_pcResultViewer->SetCurveColor(l_nCurve, m_TransitionColor[l_nCurve]);
							p_pcResultViewer->SetCurveLineWidth(l_nCurve, m_TransitionLineWidth[l_nCurve]);
							p_pcResultViewer->SetCurveLineStyle(l_nCurve, m_TransitionLineStyle[l_nCurve]);
							l_nCurve++;
						}
					}
				}
			}
		}
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

void SP_DLG_ShowAllModelView::RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle)
{
	if (m_bIsDisconnected)
	{
		return; //no need to refresh
	}

	m_pcResultViewer->Destroy();

	wxDELETE(m_pcResultViewer);

	wxString l_sElementType = m_pcModelView->GetAttribute(wxT("Nodeclass"))->GetValueString();
	if (l_sElementType.IsSameAs(wxT("Transition"))) {
		m_TransitionLineStyle[p_nCurveIndex] = p_nLineStyle;
		m_TransitionLineWidth[p_nCurveIndex] = p_nLineWidth;
		m_TransitionColor[p_nCurveIndex] = p_nColor;
	} else if (l_sElementType.IsSameAs(wxT("Place"))) {
		m_PlaceLineStyle[p_nCurveIndex] = p_nLineStyle;
		m_PlaceLineWidth[p_nCurveIndex] = p_nLineWidth;
		m_PlaceColor[p_nCurveIndex] = p_nColor;
	}
	CreateResultViewer();
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


		m_pcShowHideButton->Enable(false);
		m_pcRightSizer->Show(false);
		m_isShown = false;
		m_pcShowHideButton->SetLabel("Show Node List");
		m_pcOutputViewerType->Enable(false);
		m_pcViewerTypeButton->Enable(false);
		m_pcXAxis->Enable(false);
		m_pcEditNodeListButton->Enable(false);
	}
	else
	{
		m_bIsDisconnected = false;
		m_pcRefreshButton->Enable(true);
		m_pcConnectButton->SetLabel(wxT("Disconnect"));

		m_pcShowHideButton->Enable(true);
		m_pcRightSizer->Show(true);
		m_isShown = true;
		m_pcShowHideButton->SetLabel("Hide Node List");
		m_pcOutputViewerType->Enable(true);
		m_pcViewerTypeButton->Enable(true);
		m_pcXAxis->Enable(true);
		m_pcEditNodeListButton->Enable(true);
	}
	m_pcContentSizer->Layout();
}

