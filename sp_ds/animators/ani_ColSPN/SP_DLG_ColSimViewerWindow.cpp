

#include "sp_ds/animators/ani_ColSPN/SP_DLG_ColSimViewerWindow.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_ds/extensions/SP_DS_Animation.h"

IMPLEMENT_CLASS(SP_DLG_ColSimViewerWindow, wxFrame)

enum
{
	SP_ID_BUTTON_CLOSE = 2000,
	SP_ID_BUTTON_REFRESH,
	SP_ID_BUTTON_DISCONNECT,
	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS,
	SP_ID_CHOICE_RESULT_VIEWER_TYPE,
	SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES,
	SP_ID_BUTTON_EXPORT,
	SP_ID_CHANGE_X_AXIS,
	SP_ID_BUTTON_EDIT_NODE_LIST,
	SP_ID_BUTTON_SHOW_HIDE_NODE_LIST,
	SP_ID_SCROLL_BAR,
	SP_ID_Time_Label,
	SP_ID_Time_VALUE
	//SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT
};

BEGIN_EVENT_TABLE(SP_DLG_ColSimViewerWindow, wxFrame)
//EVT_COMMAND_SCROLL_THUMBRELEASE(SP_ID_SCROLL_BAR, SP_DLG_ViewerWindow::OnScrol)
END_EVENT_TABLE()

SP_DLG_ColSimViewerWindow::SP_DLG_ColSimViewerWindow(SP_DLG_Animation* p_pcParentWnd, std::map<wxString, long> p_mselectedPlace2Index, SP_Vector2DDouble p_vvResultMatrix
        ,SP_VectorDouble p_vAxisData, wxString p_sExportName) :
	wxFrame(p_pcParentWnd, -1, wxT("Show all views"),
		wxDefaultPosition,
		wxSize(800, 750),
		wxFRAME_FLOAT_ON_PARENT | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX),
	m_pcParentWnd(p_pcParentWnd),
	m_bIsDisconnected(false),
	m_bIsShown(true)
{

	//m_arPlaces = p_arItems;
	m_vvResultMatrix = p_vvResultMatrix;
	m_anXValues = p_vAxisData;
	m_mselectedPlace2Index = p_mselectedPlace2Index;
	m_sExportFilename = p_sExportName;

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
	m_pcSelectClearAllChkBox = new wxCheckBox(this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS, wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);
	l_pcPlaceChoiceSizer->Add(m_pcSelectClearAllChkBox, 0, wxEXPAND);
	m_pcSelectClearAllChkBox->Set3StateValue(wxCHK_CHECKED);

	 

	m_pcViewerSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer")), wxVERTICAL);

	l_pcLeftSizer->Add(m_pcViewerSizer, 1, wxALL | wxEXPAND);

	m_pcResultViewer = new SP_DS_xyPlotViewer(this, m_pcViewerSizer);

	m_pcResultViewer->Create();

	m_pcResultViewer->SetXAxisValues(&m_anXValues);


	//wxSizer *l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Viewer Type")), wxHORIZONTAL);
	//m_pcOutputViewerType = new wxChoice(this, SP_ID_CHOICE_RESULT_VIEWER_TYPE, wxDefaultPosition, wxSize(129, -1));
	//m_pcOutputViewerType->Append(wxT("Tabular"));
	//m_pcOutputViewerType->Append(wxT("xyPlot"));
	//m_pcOutputViewerType->Append(wxT("Histogram"));

	wxSizer *l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);
	//l_pcRowSizer->Add(m_pcOutputViewerType, 0, wxALL, 1);
	//m_pcViewerTypeButton = new wxButton(this, SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES, wxT("Edit"));
	//l_pcRowSizer->Add(m_pcViewerTypeButton, 0, wxALL, 1);
	//l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);

	wxSizer* l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Export")), wxHORIZONTAL);
	m_pcOutputExportType = new wxChoice(this, -1);
	m_pcOutputExportType->Append(wxT("CSV Export"));
	m_pcOutputExportType->Append(wxT("Image Export"));
	m_pcOutputExportType->SetSelection(0);
	l_pcRowSizer->Add(m_pcOutputExportType, 0, wxALL, 1);
	l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_EXPORT, wxT("Export")), 0, wxALL, 1);

		m_pcScroll = nullptr;
	
	l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);

	

	m_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);

	l_pcViewSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	//m_pcXAxis = new wxButton(this, SP_ID_CHANGE_X_AXIS, wxT("Change X Axis"));
	//l_pcRowSizer->Add(m_pcXAxis, 0, wxALL, 1);
	//m_pcEditNodeListButton = new wxButton(this, SP_ID_BUTTON_EDIT_NODE_LIST, wxT("Edit Node List"));
	//l_pcRowSizer->Add(m_pcEditNodeListButton, 0, wxALL, 1);
	//m_pcShowHideButton = new wxButton(this, SP_ID_BUTTON_SHOW_HIDE_NODE_LIST, wxT("Hide Node List"), wxDefaultPosition, wxSize(125, -1), 0);
	//l_pcRowSizer->Add(m_pcShowHideButton, 0, wxALL, 1);
	//m_pcRefreshButton = new wxButton(this, SP_ID_BUTTON_REFRESH, wxT("Refresh"));
	//l_pcRowSizer->Add(m_pcRefreshButton, 0, wxALL, 1);
	//m_pcConnectButton = new wxButton(this, SP_ID_BUTTON_DISCONNECT, wxT("Disconnect"));
	//m_pcConnectButton->SetToolTip(wxT("Turn automatic refresh on/off"));
	//l_pcRowSizer->Add(m_pcConnectButton, 0, wxALL, 1);
	//l_pcRowSizer->Add(new wxButton(this, SP_ID_BUTTON_CLOSE, wxT("Close")), 0, wxALL, 1);

	//l_pcViewSizer->Add(l_pcRowSizer, 0, wxALL, 5);
	m_pcMainSizer->Add(l_pcViewSizer, 0, wxALL, 1);
	SetSizerAndFit(m_pcMainSizer);

	AddItems();
	Bind(wxEVT_CHECKBOX, &SP_DLG_ColSimViewerWindow::OnSelectClearAllItems, this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS);
	//Bind(wxEVT_CLOSE_WINDOW, &SP_DLG_ColSimViewerWindow::OnWindowClose, this);
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnClose, this, SP_ID_BUTTON_CLOSE);
	Bind(wxEVT_CHECKLISTBOX, &SP_DLG_ColSimViewerWindow::OnPlaceCheckUncheck, this, SP_ID_CHECKLISTBOX_PLACE_CHOICE);
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnExportClick, this, SP_ID_BUTTON_EXPORT);
	Bind(wxEVT_CHOICE, &SP_DLG_ColSimViewerWindow::OnChangeResultViewer, this, SP_ID_CHOICE_RESULT_VIEWER_TYPE);

	 
	//event handler
	/**
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnClose, this, SP_ID_BUTTON_CLOSE);
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnShowHideNodeList, this, SP_ID_BUTTON_SHOW_HIDE_NODE_LIST);
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnDisconnect, this, SP_ID_BUTTON_DISCONNECT);
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnEditNodeList, this, SP_ID_BUTTON_EDIT_NODE_LIST);
	Bind(wxEVT_LISTBOX_DCLICK, &SP_DLG_ColSimViewerWindow::OnItemDoubleClick, this, SP_ID_CHECKLISTBOX_PLACE_CHOICE);

	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnRefresh, this, SP_ID_BUTTON_REFRESH);
	
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnEditViewerProperties, this, SP_ID_BUTTON_EDIT_VIEWER_PROPERTIES);
	
	Bind(wxEVT_BUTTON, &SP_DLG_ColSimViewerWindow::OnChangeXAxis, this, SP_ID_CHANGE_X_AXIS);
	Bind(wxEVT_ACTIVATE, &SP_DLG_ColSimViewerWindow::OnWindowActivate, this);
	
	Bind(wxEVT_TEXT_ENTER, &SP_DLG_ColSimViewerWindow::OnTextEner, this, SP_ID_Time_VALUE);//wxEVT_COMMAND_TEXT_ENTER
	*/
	

}

SP_DLG_ColSimViewerWindow::~SP_DLG_ColSimViewerWindow()
{
 
	if (m_pcResultViewer != nullptr)
	{
		wxDELETE(m_pcResultViewer);
	}
	 
}

 bool SP_DLG_ColSimViewerWindow::IsCurveCreated(wxString p_sName)
{
	 auto it = m_mcreatedCurves.find(p_sName);
	 return it== m_mcreatedCurves.end();
}

void SP_DLG_ColSimViewerWindow::AddItems() {

		m_pcPlaceChoiceCheckListBox->Clear();
 		long i = 0;
		
		long size = m_mselectedPlace2Index.size();

		for (long k = 0; k < size; k++) {

			for (auto it = m_mselectedPlace2Index.begin(); it != m_mselectedPlace2Index.end(); ++it)
			{
				if (k != it->second) continue;// for sorting the list

				m_arPlaces.Add(it->first);

				m_pcResultViewer->AddCurve(it->first, it->second, &m_vvResultMatrix);

				//m_pcPlaceChoiceCheckListBox->Check(i);

				i++;

				m_pcResultViewer->ShowCurve(it->second, true);

				m_pcResultViewer->Update();

			}
		}
	if (!m_arPlaces.IsEmpty())
	{
		m_pcPlaceChoiceCheckListBox->InsertItems(m_arPlaces, 0);
		for (int j = 0; j < m_arPlaces.size(); j++)
		{
			//m_pcPlaceChoiceCheckListBox->Check(j);
			m_pcPlaceChoiceCheckListBox->Check(j, true);

			//m_pcResultViewer->ShowCurve(j, true);

			m_pcResultViewer->Update();
		}
 	}
}

 void SP_DLG_ColSimViewerWindow::OnSelectClearAllItems(wxCommandEvent& p_cEvent)
{

	 if (!m_pcSelectClearAllChkBox->IsChecked())
	 {
		 for (int i = 0; i < m_arPlaces.size(); i++)
		 {
			 m_pcPlaceChoiceCheckListBox->Check(i,false);
			 m_pcResultViewer->ShowCurve(i, false);

			 m_pcResultViewer->Update();
		 }
	 }
	 else
	 {
		 for (int i = 0; i < m_arPlaces.size(); i++)
		 {
			 m_pcPlaceChoiceCheckListBox->Check(i,true);

			 m_pcResultViewer->ShowCurve(i, true);

			 m_pcResultViewer->Update();
		 }
	 }

}


 void SP_DLG_ColSimViewerWindow::OnWindowClose(wxCloseEvent& event)
 {
	 //this->Destroy();
	// this->Close();
	 Close(true);

 
 }

 void SP_DLG_ColSimViewerWindow::OnClose(wxCommandEvent& p_cEvent)
 {
	 Close(true);

 }


 void SP_DLG_ColSimViewerWindow::OnPlaceCheckUncheck(wxCommandEvent& p_cEvent) {
	
	 //get item index
	 unsigned int l_nSelection = p_cEvent.GetSelection();

	 //get check state
	 bool l_bCheckState = m_pcPlaceChoiceCheckListBox->IsChecked(l_nSelection);

	 wxString l_sName = m_pcPlaceChoiceCheckListBox->GetItem(l_nSelection)->GetName();

	 SP_MapString2Long l_mCurve2PosMap = m_pcResultViewer->GetCurvePositions();

	 long l_nMatrixItem;

	 auto itFindPlace = l_mCurve2PosMap.find(l_sName);

	 if (itFindPlace != l_mCurve2PosMap.end())
	 {
		 l_nMatrixItem = itFindPlace->second;
	 }
	 else
	 {
		 return;
	 }



	 auto it = l_mCurve2PosMap.find(l_sName);
	 if (it == l_mCurve2PosMap.end() && l_bCheckState)
	 {

		 m_pcResultViewer->ShowCurve(l_nMatrixItem, true);

 		 m_pcResultViewer->Update();
	 }
	 else {
		 m_pcResultViewer->ShowCurve(l_nMatrixItem, l_bCheckState);

		 m_pcResultViewer->Update();

 	 }
 
 }

  void SP_DLG_ColSimViewerWindow::OnExportClick(wxCommandEvent& event)
 {
	  if (m_pcOutputExportType->GetSelection()==0)
	  {
		  wxString l_sFilename = m_sExportFilename;
		  ;
		  bool l_bCompressExact = false;
		  bool l_bAlltraces = false;
		 
 		  SP_DLG_StDirectExportProperties* l_pcDlg = new SP_DLG_StDirectExportProperties(SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT, (wxDialog*)this, &l_sFilename, &m_nExportSpacer, &l_bCompressExact, &l_bAlltraces);

		  if (l_pcDlg->ShowModal() == wxID_OK)
		  {
			  wxString l_sBackupFilename = l_sFilename;
			  m_sExportFilename = l_sFilename;

			  OpenExportFile();

			  m_sExportFilename = l_sBackupFilename;
		  }

		  l_pcDlg->Destroy();
	  }
	  else
	  {
        //ToDo: Image
		  SP_LOGMESSAGE(wxT("This feature is under development!"));
	  }

 }


  void SP_DLG_ColSimViewerWindow::OpenExportFile()
  {

	  wxString l_sExportFilename = m_sExportFilename.BeforeLast('.');
 
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

	  wxArrayInt l_arSelectedItems;

	  int l_nSelectedItems = m_pcPlaceChoiceCheckListBox->GetCheckedItems(l_arSelectedItems);

	  for (long i = 0; i < l_nSelectedItems; i++)
	  {
		  wxString l_sName= m_pcPlaceChoiceCheckListBox->GetItem(l_arSelectedItems[i])->GetName();

		  l_sOutput << l_sSpacer << l_sName;
	  }

	  (*m_pcExport) << l_sOutput << endl;

	  wxString l_sCurrentRow;
      
	  for (long r = 0; r < m_vvResultMatrix.size(); r++)
	  {
		 
		  l_sCurrentRow << m_anXValues[r] << l_sSpacer;
		   
			  for (long i = 0; i < l_nSelectedItems; i++)
			  {
				  wxString l_sName = m_pcPlaceChoiceCheckListBox->GetItem(l_arSelectedItems[i])->GetName();

				  auto iterator = m_mselectedPlace2Index.find(l_sName);

				  if (iterator != m_mselectedPlace2Index.end())
				  {
					  long l_nCol = iterator->second;

					  double l_dResult = m_vvResultMatrix[r][l_nCol];
					  l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);
					  l_sCurrentRow << l_sSpacer;

				  }

			  }
			  l_sCurrentRow << wxT("\n");

	  }
	  (*m_pcExport) << l_sCurrentRow << wxT("\n");

	  wxDELETE(m_pcExport);
	  wxDELETE(m_pcExportBufferdOutputStream);
	  wxDELETE(m_pcExportFileOutputStream);

	  SP_MESSAGEBOX(wxT("Export done!"), wxT("CSV Export"));
 
  }


  wxString SP_DLG_ColSimViewerWindow::GetSpacer(int p_nSpacer)
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


  void SP_DLG_ColSimViewerWindow::OnChangeResultViewer(wxCommandEvent& event)
  {

  }
