//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: dialog to browse through cut sets
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/SP_DLG_ChooseFTreeCutSet.h"

#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"


IMPLEMENT_CLASS(SP_DLG_ChooseFTreeCutSet, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ChooseFTreeCutSet, wxDialog)

	EVT_BUTTON(wxID_OK, SP_DLG_ChooseFTreeCutSet::OnDlgOk)
	EVT_BUTTON(wxID_CANCEL, SP_DLG_ChooseFTreeCutSet::OnDlgCancel)
	EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOUR, SP_DLG_ChooseFTreeCutSet::OnColour)
	EVT_LISTBOX(SP_ID_LISTBOX_VEC, SP_DLG_ChooseFTreeCutSet::OnKlick)	
END_EVENT_TABLE()


SP_DLG_ChooseFTreeCutSet::SP_DLG_ChooseFTreeCutSet(SP_DS_Graph *p_pcGraph,
				 wxWindow *p_parent, list<SP_ListNode*>* cutSets,
				 const wxString& p_title): 
		wxDialog(p_parent, -1, p_title, wxDefaultPosition,
	     wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
 	m_pcMapListNum2MarkValue(NULL),
	m_pcMapListNum2CutSet(NULL),
    m_cutSets(cutSets),
	m_pcNodeColourizer(NULL),
	m_pcListBox(NULL),
	m_pcGraph(p_pcGraph)
	
{
	
	m_pcMapListNum2CutSet = new map<long, SP_ListNode*>();
	m_pcMapListNum2MarkValue = new map<SP_DS_Node*, long>();
	m_pcNodeColourizer = new SP_FTreeNodeColourizer(m_pcGraph);
	InitGUI();
}


void
SP_DLG_ChooseFTreeCutSet::UpdateList(){
	
	m_pcListBox->Clear();
	
	if (!m_pcListBoxValues.IsEmpty()) {
	  m_pcListBox->InsertItems(m_pcListBoxValues, 0);
  }
}
	
void
SP_DLG_ChooseFTreeCutSet::InitGUI(){
	
	m_pcMapListNum2CutSet->clear();
	
	m_pcListBox = new wxListBox(this, SP_ID_LISTBOX_VEC, wxDefaultPosition, 
					wxSize(200,250), 0, NULL, wxLB_EXTENDED);
	
	
  
	wxBoxSizer* l_pcSizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* l_pcSizerNorth = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* l_pcSizerButtons = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerList = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* l_pcSizerColour = new wxBoxSizer(wxHORIZONTAL);

	
	l_pcSizerList->Add(m_pcListBox, 1, wxEXPAND, 1);
	l_pcSizerColour->Add(new wxStaticText(this, -1, _T("Colour:")), 0, wxALL , 5);
	m_pcButtonColour = new wxColourPickerCtrl(this, SP_ID_BUTTON_COLOUR, *wxRED,
            wxDefaultPosition, wxDefaultSize,
            wxCLRP_DEFAULT_STYLE);

    l_pcSizerColour->Add(m_pcButtonColour, 0, wxALL, 5);
    l_pcSizerButtons->Add(l_pcSizerColour, 0, wxLEFT | wxRIGHT | wxTOP, 5);
  

	l_pcSizerButtons->Add(this->CreateButtonSizer(wxOK|wxCANCEL),
		   0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP , 5);

 	l_pcSizerNorth->Add(l_pcSizerList, 1, wxEXPAND, 1);
	l_pcSizerNorth->Add(l_pcSizerButtons, 0,0);
	
	// @todo insert color choosing 
	l_pcSizerTop->Add(l_pcSizerNorth, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 1);
	
	SetSizerAndFit(l_pcSizerTop);
  
    Centre(wxBOTH);

    ReadCutSets();
	
}

bool
SP_DLG_ChooseFTreeCutSet::ReadCutSets()
{
	m_pcListBoxValues.Clear();
	CHECK_POINTER(m_cutSets, return FALSE);
	//all cut sets are inserted in a pop-up window
	long cutSetNum = 0;
	list<SP_ListNode*>::iterator pIt;
	for (pIt = m_cutSets->begin(); pIt != m_cutSets->end(); ++pIt) {
			cutSetNum++;
			SP_ListNode* singleCutSet = (*pIt);
			SP_ListNode::const_iterator nIt;
			wxString setNames = wxT("");
			
			for (nIt = singleCutSet->begin(); nIt != singleCutSet->end(); ++nIt){
		
				
				wxString singleName = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();
				if (singleName.IsEmpty()) {
					
					wxString sID = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
					singleName = wxT("X") + sID;
				}
				
				setNames += singleName + wxT(", ");
			}

			m_pcMapListNum2CutSet->insert(pair<long, SP_ListNode*>(cutSetNum, (*pIt)));
			m_pcListBoxValues.Add(wxString::Format(wxT("%ld: %s"),cutSetNum, setNames.c_str()));	
			UpdateList();
	}
	return TRUE;
}

void
SP_DLG_ChooseFTreeCutSet::OnDlgCancel(wxCommandEvent& p_cEvent)
{	
	m_pcNodeColourizer->RestoreColours();
	m_pcNodeColourizer->DiscardOldColours();
	p_cEvent.Skip();
}

void
SP_DLG_ChooseFTreeCutSet::OnDlgOk(wxCommandEvent& p_cEvent)
{	
	m_aVectorNumbers.Clear();
	m_pcNodeColourizer->RestoreColours();
	m_pcNodeColourizer->DiscardOldColours();
	//If the user has selected a cut set from the list, 
	//the events of this cut set get in each case a token assigned. 
	if(m_pcListBox->GetSelections(m_aVectorNumbers))
	{
		long l_nSel = m_aVectorNumbers.Item(0);
		map<long, SP_ListNode*>:: iterator l_pcIt = m_pcMapListNum2CutSet->begin();
		while (l_pcIt != m_pcMapListNum2CutSet->end()){
			if ((*l_pcIt).first == (l_nSel + 1)){
				SetFailureStatus((*l_pcIt).second);
				break;
			}else
				l_pcIt++;
		}
	}
	RefreshFrame();
	Show(false);
}

void
SP_DLG_ChooseFTreeCutSet::OnKlick(wxCommandEvent& p_cEvent)
{
	m_aVectorNumbers.Clear();
	m_pcNodeColourizer->RestoreColours();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;
	
	if(l_nCount == 0)
	{
		return;
	} else {
	   OnChoosingCutSet();
	}
}

void
SP_DLG_ChooseFTreeCutSet::OnChoosingCutSet()
{
	m_pcNodeColourizer->RestoreColours();
	//The events of a selected cut set are presented in the tree coloredly 
	m_aVectorNumbers.Clear();
	m_pcListBox->GetSelections(m_aVectorNumbers);
	long l_nSel = m_aVectorNumbers.Item(0);
	map<long, SP_ListNode*>:: iterator l_pcIt = m_pcMapListNum2CutSet->begin();
	while (l_pcIt != m_pcMapListNum2CutSet->end()){
		if ((*l_pcIt).first == (l_nSel + 1)){
			m_pcNodeColourizer->ColourNodes((*l_pcIt).second, m_pcButtonColour->GetColour());
			break;
		}else
			l_pcIt++;
	}
}



bool
SP_DLG_ChooseFTreeCutSet::SetFailureStatus(SP_ListNode* cutSet)
{
	//Each event of the selected cut set gets a token assigned (receive the failure status assigned) 
	CHECK_POINTER(cutSet, return FALSE);
	ResetMarking();

	SP_ListNode::const_iterator nIt;
	for (nIt = cutSet->begin(); nIt != cutSet->end(); ++nIt){
		SP_DS_NumberAttribute* n_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
				((*nIt)->GetAttribute(wxT("Failure-Status")));
		
		long defaultMarking = n_pcAttribute->GetValue();
	
		m_pcMapListNum2MarkValue->insert(pair< SP_DS_Node*, long>
											((*nIt), defaultMarking ));

		if (n_pcAttribute->GetValue() == 0) {
			n_pcAttribute->SetValue(n_pcAttribute->GetValue() + 1);
			(*nIt)->Update(TRUE);
		}
	}
	return TRUE;
}

bool
SP_DLG_ChooseFTreeCutSet::ResetMarking()
{
	//Sets the original condition (original number of token in a place) 
	if (m_pcMapListNum2MarkValue->size() == 0)
		return FALSE;

	map<SP_DS_Node*, long>::iterator l_pcIt = m_pcMapListNum2MarkValue->begin();
	
	while(l_pcIt != m_pcMapListNum2MarkValue->end())
	{
  		SP_DS_Node* m_pcNode = (*l_pcIt).first;
		long defaultMarking =  (*l_pcIt).second;

		SP_DS_NumberAttribute* n_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
				(m_pcNode->GetAttribute(wxT("Failure-Status")));
		n_pcAttribute->SetValue(defaultMarking);
		m_pcNode->Update(TRUE);

		l_pcIt++;
	}
	 m_pcMapListNum2MarkValue->clear();

	 RefreshFrame();

	 return TRUE;
}

void
SP_DLG_ChooseFTreeCutSet::OnColour(wxColourPickerEvent& p_cEvent)
{
    RefreshColour();
    OnKlick(p_cEvent);
}

void
SP_DLG_ChooseFTreeCutSet::RefreshColour()
{
    m_pcButtonColour->Refresh();
}

void
SP_DLG_ChooseFTreeCutSet::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}
