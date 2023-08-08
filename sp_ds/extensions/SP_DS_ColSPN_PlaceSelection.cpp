#include "sp_ds/extensions/SP_DS_ColSPN_PlaceSelection.h"
#include "sp_utilities.h"
enum {
	SP_ID_APPEND_BUTTON,
	SP_ID_BACK_BUTTON,
	SP_ID_APPENDALL_BUTTON,
	SP_ID_BACKALL_BUTTON,
	SP_ID_OK_BUTTON
};
BEGIN_EVENT_TABLE(SP_DS_ColSPN_PlaceSelection, wxDialog)
EVT_BUTTON(SP_ID_APPEND_BUTTON, SP_DS_ColSPN_PlaceSelection::OnAppendOne)
EVT_BUTTON(SP_ID_APPENDALL_BUTTON, SP_DS_ColSPN_PlaceSelection::OnAppendAll)
EVT_BUTTON(SP_ID_BACKALL_BUTTON, SP_DS_ColSPN_PlaceSelection::OnBackAll)
EVT_BUTTON(SP_ID_BACK_BUTTON, SP_DS_ColSPN_PlaceSelection::OnBackOne)
EVT_BUTTON(SP_ID_OK_BUTTON, SP_DS_ColSPN_PlaceSelection::OnOk)

END_EVENT_TABLE()
SP_DS_ColSPN_PlaceSelection::SP_DS_ColSPN_PlaceSelection(wxWindow *p_pcParent,const wxString& title, wxArrayString p_list)
	: wxDialog(p_pcParent, wxID_ANY, title, wxDefaultPosition, wxSize(500, 400))
{
	ar_PlaceList =  p_list;
	 
	// Create the main sizer
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	// Create the text box
	wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	//mainSizer->Add(textBox, 0, wxEXPAND | wxALL, 5);

	// Create the sizer for the list controls
	wxBoxSizer* listSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the left list control
	leftList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	long l_nIndex = 0;
	//leftList->InsertColumn(0);
	leftList->InsertColumn(0, "places", wxLIST_FORMAT_LEFT, 100);
	for (const wxString& element : ar_PlaceList) {
			leftList->InsertItem(l_nIndex, element);// , wxLIST_FORMAT_LEFT, 100);
			leftList->SetItem(l_nIndex, 0, element);
		
		l_nIndex++;
	}
	
	listSizer->Add(leftList, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* listButtonSizer = new wxBoxSizer(wxVERTICAL);
	listButtonSizer->Add(new wxButton(this, SP_ID_APPEND_BUTTON , wxT(">")), 0, wxEXPAND | wxALL, 2);
	listButtonSizer->Add(new wxButton(this, SP_ID_BACK_BUTTON, wxT("<")), 0, wxEXPAND | wxALL, 2);
	listButtonSizer->Add(new wxButton(this, SP_ID_APPENDALL_BUTTON, wxT(">>")), 0, wxEXPAND | wxALL, 2);
	listButtonSizer->Add(new wxButton(this, SP_ID_BACKALL_BUTTON, wxT("<<")), 0, wxEXPAND | wxALL, 2);


	listSizer->Add(listButtonSizer, 0, wxEXPAND | wxALL, 5);
	
	// Create the right list control
    rightList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	rightList->InsertColumn(0, "chosen place ID", wxLIST_FORMAT_RIGHT, 200);
	listSizer->Add(rightList, 1, wxEXPAND | wxALL, 5);

	// Add the list sizer to the main sizer
	mainSizer->Add(listSizer, 1, wxEXPAND | wxALL, 5);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
	//mainSizer->Add(new wxButton(this, SP_ID_OK_BUTTON, wxT("ok")), 0, wxEXPAND | wxALL, 5);
	// Set the main sizer for the frame
	SetSizerAndFit(mainSizer);

	// Bind the text enter event to a handler
	textBox->Bind(wxEVT_TEXT_ENTER, &SP_DS_ColSPN_PlaceSelection::OnTextEnter, this);
}

std::vector<wxString> SP_DS_ColSPN_PlaceSelection::GetChosenVariables()
{
	return m_vChosen;
}

void SP_DS_ColSPN_PlaceSelection::OnTextEnter(wxCommandEvent& event)
{
	//wxMessageBox("You entered: " + event.GetString(), "Text Entered", wxOK | wxICON_INFORMATION);
	SP_LOGMESSAGE(wxT("This feature still under development"));
}
void SP_DS_ColSPN_PlaceSelection::OnOk(wxCommandEvent& event)
{

}
void SP_DS_ColSPN_PlaceSelection::OnBackOne(wxCommandEvent& event)
{
	long selectedItem = -1;
	int index = -1;
	while ((index = rightList->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1) {
		selectedItem = index;
		break;  // Assuming only one item can be selected
	}

	if (selectedItem != -1) {
		rightList->DeleteItem(selectedItem);//remove the selected one
	}
 
}

void SP_DS_ColSPN_PlaceSelection::OnBackAll(wxCommandEvent& event)
{
	long l_nCount = rightList->GetItemCount();

	rightList->DeleteAllItems();

	m_vChosen.clear();
	
}

void SP_DS_ColSPN_PlaceSelection::OnAppendAll(wxCommandEvent& event)
{
	long l_nCount =leftList->GetItemCount();
	long l_nIndex = 0;
	wxString l_sSelected;
	while (l_nIndex<l_nCount) {
		//selectedItem = index;
		l_sSelected = leftList->GetItemText(l_nIndex, 0);
		l_nIndex++;
		int cl = rightList->GetItemCount();

		auto it_findelem = std::find(m_vChosen.begin(), m_vChosen.end(), l_sSelected);

		if (it_findelem != m_vChosen.end()) continue;//avoid doublicate an element

		rightList->InsertItem(cl, l_sSelected);
		rightList->SetItem(cl, 0, l_sSelected);
		m_vChosen.push_back(l_sSelected);
	}
}


void SP_DS_ColSPN_PlaceSelection::OnAppendOne(wxCommandEvent& event) {
 
	long selectedItem = -1;
	int index = -1;
	while ((index = leftList->GetNextItem(index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1) {
		selectedItem = index;
		break;  // Assuming only one item can be selected
	}

	// Get the text of the selected item
	if (selectedItem != -1) {
		wxString text = leftList->GetItemText(selectedItem);


		auto it_findelem = std::find(m_vChosen.begin(), m_vChosen.end(), text);

		if (it_findelem != m_vChosen.end()) return;//avoid doublicate an element

		if (rightList->GetItemCount() == 0) {
			rightList->InsertItem(0, text);
			rightList->SetItem(0, 0, text);
			m_vChosen.push_back(text);
		}
		else {
			int cl = rightList->GetItemCount();
			rightList->InsertItem(cl, text);
			rightList->SetItem(cl, 0, text);
			m_vChosen.push_back(text);

		}
	}

 }