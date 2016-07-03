//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: dialog to browse through reductions
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/SP_DLG_ChooseFTreeReduction.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionTransformer.h"
#include "sp_ds/netclasses/SP_DS_ExtendedFaultTree.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "snoopy.h"

enum
{

	SP_DELETE_SELECTION,
	SP_START_REDUCTION
};

IMPLEMENT_CLASS(SP_DLG_ChooseFTreeReduction, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ChooseFTreeReduction, wxDialog)

	EVT_BUTTON(wxID_CANCEL, SP_DLG_ChooseFTreeReduction::OnDlgCancel)
	EVT_BUTTON(SP_DELETE_SELECTION, SP_DLG_ChooseFTreeReduction::OnDelete)
	EVT_BUTTON(SP_ID_BUTTON_FIRST_VEC, SP_DLG_ChooseFTreeReduction::OnFirst)
	EVT_BUTTON(SP_ID_BUTTON_NEXT_VEC, SP_DLG_ChooseFTreeReduction::OnNext)
	EVT_BUTTON(SP_ID_BUTTON_PREV_VEC, SP_DLG_ChooseFTreeReduction::OnPrev)
	EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOUR, SP_DLG_ChooseFTreeReduction::OnColour)
	EVT_BUTTON(SP_START_REDUCTION, SP_DLG_ChooseFTreeReduction::OnStart)

	EVT_UPDATE_UI(SP_ID_BUTTON_NEXT_VEC, SP_DLG_ChooseFTreeReduction::OnUpdateUI)
	EVT_UPDATE_UI(SP_ID_BUTTON_PREV_VEC, SP_DLG_ChooseFTreeReduction::OnUpdateUI)
	EVT_LISTBOX(SP_ID_LISTBOX_VEC, SP_DLG_ChooseFTreeReduction::OnKlick)
	EVT_RADIOBOX(SP_ID_RADIOBOX_FTREE_ANALYSIS, SP_DLG_ChooseFTreeReduction::OnKlick)

END_EVENT_TABLE()

//oldVersion: the fault tree is copied and is inserted in a separate window. On the basis of
//this copy, the reduction is started. Besides, the original file is not changed.
//But the used copy-function of Snoopy make no 1 to 1 copy, so that the sequence of the gates for instance
//are not the same. I had to search again the nodes for the reduction of the fault tree.
//But it can lead to that the order of the reduction results are changed. If the user select a reduction of a sum
//of results from the same reduction rule, it can lead to the fact that at the end a wrong reduction
//is executed by the same reduction rule.

//new version (18.12.2006): the reduction is executed on the original file. If the user would like
//to keep the original fault tree, he has to be save the fault tree.

SP_DLG_ChooseFTreeReduction::SP_DLG_ChooseFTreeReduction(SP_DS_Graph *p_pcGraph,
				 wxWindow *p_parent, ReductionMap redNodes,
				 const wxString& p_title)
  : wxDialog(p_parent, -1, p_title, wxDefaultPosition,
	     wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
	m_pcNodeColourizer(NULL),
	m_colorize(FALSE),
	m_bChanged(FALSE),
	m_pcListBox(NULL),
	m_pcGraph(p_pcGraph),
	m_nodeList(NULL),
	m_reduceList(redNodes)

{
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

	m_pcNodeColourizer = new SP_FTreeNodeColourizer(m_pcGraph);
	InitGUI();

}


void
SP_DLG_ChooseFTreeReduction::UpdateList(){

	m_pcListBox->Clear();

	if (!m_pcListBoxValues.IsEmpty()) {
	  m_pcListBox->InsertItems(m_pcListBoxValues, 0);
  }
}

void
SP_DLG_ChooseFTreeReduction::InitGUI(){
	m_pcListBox = new wxListBox(this, SP_ID_LISTBOX_VEC, wxDefaultPosition,
					wxSize(200,250), 0, NULL, wxLB_EXTENDED);

	wxBoxSizer* l_pcSizerTop = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerNorth = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* l_pcSizerButtons = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerList = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* l_pcSizerColour = new wxBoxSizer(wxHORIZONTAL);


	l_pcSizerList->Add(m_pcListBox, 1, wxEXPAND, 1);
	/* Buttons in the lower right hand corner */
	//wxT("&Search nodes by ID\tCtrl+Shift+F")
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_FIRST_VEC, wxT("&First")),
			 0, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_NEXT_VEC, wxT("&Next")),
			 0, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_ID_BUTTON_PREV_VEC, wxT("&Prev")),
			 0, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizerButtons->Add(new wxButton(this, SP_DELETE_SELECTION, wxT("&Delete")),
			 0, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizerColour->Add(new wxStaticText(this, -1, _T("Colour:")), 0, wxALL , 5);

	m_pcButtonColour = new wxColourPickerCtrl(this, SP_ID_BUTTON_COLOUR, *wxRED,
            wxDefaultPosition, wxDefaultSize,
            wxCLRP_DEFAULT_STYLE);

	l_pcSizerColour->Add(m_pcButtonColour, 0, wxALL, 5);
	l_pcSizerButtons->Add(l_pcSizerColour, 0, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizerButtons->Add(new wxButton(this, SP_START_REDUCTION, wxT("&Start")),
			 0, wxLEFT | wxRIGHT | wxTOP, 5);


	l_pcSizerButtons->Add(new wxButton(this, wxID_CANCEL, wxT("&Close")),
		   0, wxLEFT | wxRIGHT | wxTOP , 5);

 	l_pcSizerNorth->Add(l_pcSizerList, 1, wxEXPAND, 1);
	l_pcSizerNorth->Add(l_pcSizerButtons, 0, wxALIGN_RIGHT,0);


	l_pcSizerTop->Add(l_pcSizerNorth, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 1);

	SetSizerAndFit(l_pcSizerTop);

	Centre(wxBOTH);

	ReadReductionList();

}

void
SP_DLG_ChooseFTreeReduction::ReadReductionList()
{
	//possible reductions of the fault tree are inserted in a dialogue window
	ReductionMap::iterator pIt;
	for (pIt = m_reduceList.begin(); pIt != m_reduceList.end(); pIt++) {

		NodeList* n_list = (*pIt).second;
		long count = (*pIt).first;
		//new list entry
		Visit(n_list, count);
	}
}

void
SP_DLG_ChooseFTreeReduction::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    m_pcNodeColourizer->RestoreColours();
    m_pcNodeColourizer->DiscardOldColours();
	p_cEvent.Skip();
}

long
SP_DLG_ChooseFTreeReduction::SearchID(int l_nSel)
{
	//reads the ID of the selected reduction in the listbox
	wxString sID;
	wxString entry = m_pcListBoxValues.Item(l_nSel);

	int i = entry.Find(wxT(":"));
	if (i > -1)
		sID = entry.SubString(0, (i-1));
	long id;
	sID.ToLong(&id);

	return id;
}

void
SP_DLG_ChooseFTreeReduction::OnDelete(wxCommandEvent& p_cEvent)
{
	//delete one entry of the listbox
	m_aVectorNumbers.Clear();
	m_pcNodeColourizer->RestoreColours();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;

	if(l_nCount == 0)
	{
		return;
	} else {

		int l_nSel = m_aVectorNumbers.Item(0);

		m_pcListBox->Delete(l_nSel);
		//TODO: check if this is ok
		m_pcListBoxValues.RemoveAt(l_nSel,1);

	}

}

bool
SP_DLG_ChooseFTreeReduction::ExportToExtendedFTreeIsNeeded()
{

	SP_GM_DocTemplate* l_pcDocTemplate = dynamic_cast<SP_GM_DocTemplate*>(l_pcDoc->GetDocumentTemplate());

	//We have to check the typ of the reduction, because some new elements are member of Extended Fault Tree.
	//If the tree is from type wxT("FaultTree") and a possible reduction contains elements of Extended Fault Trees, then
	//we have to save the tree as Extended Fault Tree before starting the reduction.
    if (l_pcDocTemplate){

		if (l_pcDocTemplate->GetNetclass()->GetName()== SP_DS_FAULTTREE_CLASS) {
			wxString entry;
			long count = m_pcListBoxValues.GetCount();
			for (long i = 0; i != count; ++i){
				entry = m_pcListBoxValues.Item(i);
				if (wxStrstr(entry, wxT("M-OF-N")))
					return TRUE;
			}
		}
	}

	return FALSE;
}

void
SP_DLG_ChooseFTreeReduction::OnStart(wxCommandEvent& p_cEvent)
{
	m_pcNodeColourizer->RestoreColours();
    m_pcNodeColourizer->DiscardOldColours();

	if (ExportToExtendedFTreeIsNeeded())
		SP_MESSAGEBOX(wxString(wxT("Reduction(s) use(s) gates which are member of Extended Fault Trees.")
		wxT("Please, export the Fault Tree to Extended Fault Tree before reducing the graph!")) ,
		wxT("Information"), wxOK | wxICON_INFORMATION);
	else{


		//If the user would not like to take over all possible reductions, then we have to update
		//the list of all reductions.
		UpdateReductionList();
		//start all possible reductions
		DoReductions();
	}
	this->Show(false);
}


void
SP_DLG_ChooseFTreeReduction::UpdateReductionList()
{
	ReductionMap::iterator pIt;

	long i;
	ReductionMap tempList;
	//update reduction list with list in the dialog
	long count = m_pcListBoxValues.GetCount();
	for (i = 0; i != count; ++i){
		//determine the number of list entry to compare ID in reduction list and ID in dialog
		long id = SearchID(i);
		for (pIt = m_reduceList.begin(); pIt != m_reduceList.end(); ++pIt) {
			if (id == (*pIt).first){

				tempList[(*pIt).first] = (*pIt).second;
				break;
			}
		}
	}
	m_reduceList.clear();
	m_reduceList = tempList;

}

void
SP_DLG_ChooseFTreeReduction::DoReductions()
{

	ReductionMap::iterator pIt;

	doReduction = new SP_DS_FTreeReductionTransformer(m_pcGraph, l_pcDoc);

	wxString msgList = wxT("");
	//Start of all selected reductions
	for (pIt = m_reduceList.begin(); pIt != m_reduceList.end(); pIt++) {

		NodeList* n_list = (*pIt).second;


		SP_ListNode* l_pclAllNodes = n_list->r_nodeList;
		//before starting the next reduction, check whether all nodes
		//in the next reduction-list still exist
		if (ExistenceOfNodes(l_pclAllNodes)){

			long typ = (*pIt).second->m_typ;

			switch (typ){
			case 1:
				doReduction->DoEliminateUnneededNodes(l_pclAllNodes);
				break;
			case 2:
				doReduction->DoCombineIdenticalGates(l_pclAllNodes);
				break;
			case 3:
				doReduction->DoEliminateUnneededNodes(l_pclAllNodes);
				break;
			case 4:
				doReduction->DoCombineToMofNGate(l_pclAllNodes);
				break;
			case 5:
				doReduction->DoRelocateMultipleInputs(l_pclAllNodes);
				break;
			default:
				break;
			};


		}else{
			wxString sNum;
			sNum.Printf(wxT("%ld"), (*pIt).first);
			msgList += sNum + wxT(": ") + (*pIt).second->description +  wxT("\n");
		}
	}
	if (!msgList.empty())
		SP_MESSAGEBOX(wxT("Reduction : \n") + msgList  + wxT("\n can't be in progress! ") +
		wxT("Some nodes are replaced / deleted by an other reduction.") ,wxT("Information"), wxOK | wxICON_INFORMATION);
}


bool
SP_DLG_ChooseFTreeReduction::ExistenceOfNodes(SP_ListNode* l_pclAllNodes)
{
	//If one of the nodes from the reduction losz doesn't exist any more in the
	//fault tree, this reductions is not possible any more.
	SP_ListNode::const_iterator nIt;
	for (nIt = l_pclAllNodes->begin(); nIt != l_pclAllNodes->end(); ++nIt){
		if (!(*nIt)->GetGraphics())
			return FALSE;
	}
	return TRUE;
}



void
SP_DLG_ChooseFTreeReduction::OnFirst(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);
	//select first entry of the listbox
	m_aVectorNumbers.Clear();
	if(m_pcListBox->GetSelections(m_aVectorNumbers)>0){
		for(size_t Counter = 0; Counter < m_aVectorNumbers.GetCount(); Counter++ )
		{
		  int l_nNum = m_aVectorNumbers.Item(Counter);
			m_pcListBox->Deselect(l_nNum);
		}
	}

	if(m_pcListBox->GetCount() > 0){
		m_pcListBox->SetSelection(0);
		OnChoosingReduction();
	}
}

void
SP_DLG_ChooseFTreeReduction::OnNext(wxCommandEvent& p_cEvent)
{
	//select next entry of the listbox
	CHECK_POINTER(m_pcListBox,return);
	m_aVectorNumbers.Clear();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers);
	if(l_nCount != 1){
		//do nothing
	}else{
		unsigned int l_nSel = m_aVectorNumbers.Item(0);
			if(l_nSel+1 <  m_pcListBox->GetCount()){
			m_pcListBox->SetSelection(l_nSel+1);
			m_pcListBox->Deselect(l_nSel);

			OnChoosingReduction();
		}
	}
}

void
SP_DLG_ChooseFTreeReduction::OnPrev(wxCommandEvent& p_cEvent)
{
	//select previous entry  of the listbox
	CHECK_POINTER(m_pcListBox,return);
	m_aVectorNumbers.Clear();
	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;
	if(l_nCount == 1){
		//one item selected
		int l_nSel = m_aVectorNumbers.Item(0);
		if(l_nSel > 0){
			m_pcListBox->SetSelection(l_nSel-1);
			m_pcListBox->Deselect(l_nSel);

			OnChoosingReduction();
		}
	}
}

void
SP_DLG_ChooseFTreeReduction::OnColour(wxColourPickerEvent& p_cEvent)
{
    RefreshColour();
    OnKlick(p_cEvent);
}

void
SP_DLG_ChooseFTreeReduction::RefreshColour()
{
   m_pcButtonColour->Refresh();
}

void
SP_DLG_ChooseFTreeReduction::OnKlick(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_pcListBox,return);

	m_aVectorNumbers.Clear();

	m_pcNodeColourizer->RestoreColours();

	int l_nCount = m_pcListBox->GetSelections(m_aVectorNumbers) ;

	if(l_nCount == 0)
	{
		return;
	} else {
	   //one item selected
		OnChoosingReduction();
	}
}

void
SP_DLG_ChooseFTreeReduction::OnChoosingReduction()
{
	m_colorize = TRUE;
	m_pcNodeColourizer->RestoreColours();

	m_aVectorNumbers.Clear();
	m_pcListBox->GetSelections(m_aVectorNumbers);
	long l_nSel = m_aVectorNumbers.Item(0);
	long id = SearchID(l_nSel);
	ReductionMap::iterator pIt;
	//To the selected entry in the listbox, the list of nodes is
	//determined. These are presented in the tree coloredly by call of Visit().
	for (pIt = m_reduceList.begin(); pIt != m_reduceList.end(); pIt++) {

		long count = (*pIt).first;
		if ( count == id ) {

			NodeList* n_list = (*pIt).second;
			Visit(n_list, count);
			break;
		}
	}
}

/*
 * -checks if the next or previous button has to be grey
*/
void
SP_DLG_ChooseFTreeReduction::OnUpdateUI(wxUpdateUIEvent& p_event)
{

	m_aVectorNumbers.Clear();
	m_pcListBox->GetSelections(m_aVectorNumbers);
	if(m_aVectorNumbers.IsEmpty()){
		return;
	}

	switch (p_event.GetId()) {
	case SP_ID_BUTTON_NEXT:
		p_event.Enable(m_pcListBox &&
		m_pcListBox->GetCount() > (unsigned int) m_aVectorNumbers.Item(0)+1 );
		break;
	case SP_ID_BUTTON_PREV:
		p_event.Enable(m_pcListBox &&
		m_aVectorNumbers.Item(0) != 0 );
		break;
	}
}

bool
SP_DLG_ChooseFTreeReduction::Visit(NodeList* n_list, long count ){

	CHECK_POINTER(n_list, return FALSE);
	if(!m_nodeList){
		m_nodeList = new SP_ListNode();
	}else{
		m_nodeList->clear();
	}

	//If an entry of the listbox is selected, the nodes of the reduction
	//are presented coloredly.
	if (m_colorize){
		SP_ListNode* l_pclAllNodes = n_list->r_nodeList;

		for(m_nodeIt = l_pclAllNodes->begin();
			m_nodeIt != l_pclAllNodes->end();
			m_nodeIt++){

			 m_nodeList->push_back((*m_nodeIt));
		}

		m_pcNodeColourizer->ColourNodes(m_nodeList, m_pcButtonColour->GetColour());


		return TRUE;
	}else{
		//Or a new reduction is inserted in the listbox

		wxString n_description = n_list->description;
		m_pcListBoxValues.Add(wxString::Format(wxT("%ld: %s"), count, n_description.c_str()));

		UpdateList();

		return TRUE;
	}
	return FALSE;

}



