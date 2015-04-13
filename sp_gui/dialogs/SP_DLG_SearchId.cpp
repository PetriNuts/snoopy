//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/07/05 $
// Short Description: dialog to search for nodes by id
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/SP_DLG_SearchId.h"

#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/SP_Core.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Canvas.h"

IMPLEMENT_CLASS(SP_DLG_SearchId, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_SearchId, wxDialog)
	EVT_RADIOBOX(SP_ID_RADIOBOX_SELECT, SP_DLG_SearchId::OnChangedSelect )

	EVT_BUTTON(wxID_OK, SP_DLG_SearchId::OnDlgOk)
	EVT_BUTTON(SP_ID_BUTTON_FIND, SP_DLG_SearchId::OnFind)
	EVT_BUTTON(SP_ID_BUTTON_NEXT, SP_DLG_SearchId::OnNext)
	EVT_BUTTON(SP_ID_BUTTON_PREV, SP_DLG_SearchId::OnPrev)
END_EVENT_TABLE()

SP_DLG_SearchId::SP_DLG_SearchId(SP_DS_Graph *p_graph, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	m_title(p_title), m_nSelect(0), m_sSelect(wxT("")), m_id(-1), m_sName(wxT("")), m_graph(p_graph), m_nodeList(NULL), m_grList(NULL)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* elemSizer = new wxBoxSizer(wxVERTICAL);

	unsigned int ncWithId = 0;
	SP_ListNodeclass::const_iterator ncIt;

	wxArrayString choices;

	choices.Add(wxT("Name"));
	choices.Add(wxT("ID"));

	SP_DS_Metadataclass* mc = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc != NULL)
		choices.Add(mc->GetDisplayName());

	m_select = new wxRadioBox(this, SP_ID_RADIOBOX_SELECT, wxT("Search"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
	elemSizer->Add(m_select, 0, wxALL | wxEXPAND, 5);


	m_exactName = new wxCheckBox(this, -1, wxT("exact name"));
	elemSizer->Add(m_exactName, 0, wxALL | wxEXPAND, 5);

	m_idSpinCtrl = new wxSpinCtrl(this, -1, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, INT_MAX, 0);
	elemSizer->Add(m_idSpinCtrl, 0, wxALL | wxEXPAND, 5);

	m_NameTextCtrl = new wxTextCtrl(this, -1);
	elemSizer->Add(m_NameTextCtrl, 0, wxALL | wxEXPAND, 5);


	for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
	{
		if ((*ncIt)->GetShowInElementTree())
		{
			if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && (*ncIt)->GetPrototype()->GetAttribute(wxT("Name")))
				ncWithId++;
		}
	}

	if (ncWithId > 0)
	{
		choices.Clear();

		unsigned int i = 0;
		for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
		{

			if ((*ncIt)->GetShowInElementTree())
			{

				if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && (*ncIt)->GetPrototype()->GetAttribute(wxT("Name")))
				{
					choices.Add((*ncIt)->GetDisplayName());
					i++;
				}

			}

		}

		m_choice = new wxRadioBox(this, -1, wxT("Nodeclass"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
		elemSizer->Add(m_choice, 0, wxALL | wxEXPAND, 5);

	}
	else
	{
		elemSizer->Add(new wxStaticText(this, -1, wxT("No nodes with IDs!")));
	}

	topSizer->Prepend(elemSizer, 1, wxALL | wxEXPAND, 5);

	/* Buttons in the lower right hand corner */
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);
	if (ncWithId > 0)
	{
		m_pcFind = new wxButton(this, SP_ID_BUTTON_FIND, wxT("&First"));
		buttonSizer->Add(m_pcFind, 0, wxLEFT | wxRIGHT | wxTOP, 5);
		m_pcNext = new wxButton(this, SP_ID_BUTTON_NEXT, wxT("&Next"));
		buttonSizer->Add(m_pcNext, 0, wxLEFT | wxRIGHT | wxTOP, 5);
		m_pcPrev = new wxButton(this, SP_ID_BUTTON_PREV, wxT("&Prev"));
		buttonSizer->Add(m_pcPrev, 0, wxLEFT | wxRIGHT | wxTOP, 5);

		m_pcNext->Enable(false);
		m_pcPrev->Enable(false);
	}

	buttonSizer->Add(new wxButton(this, wxID_OK, wxT("&Close")), 0, wxLEFT | wxRIGHT | wxTOP, 5);

	topSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);

	m_NameTextCtrl->Show(true);

	m_exactName->Show(true);
	m_idSpinCtrl->Show(false);

}

SP_DLG_SearchId::~SP_DLG_SearchId()
{
}

void SP_DLG_SearchId::OnDlgOk(wxCommandEvent& p_cEvent)
{
	this->Show(false);
}

void SP_DLG_SearchId::OnNext(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_grList, return);


	wxString l_sSelect = m_select->GetStringSelection();


	if (m_grIt != m_grList->end())
		m_grIt++;
	if (m_grIt == m_grList->end())
	{
		m_pcNext->Enable(false);
		m_pcPrev->Enable(true);


		if (l_sSelect.IsSameAs(wxT("Constants"))) {

			if (m_grConstIt != m_grConst.end()) {
				m_grConstIt++;
				m_grList = *m_grConstIt;
				m_grIt = m_grList->begin();
			}



		} else {
			if (!FindNextNode())
			{
				return;
			}
		}
	}

	ShowFound();
}

void SP_DLG_SearchId::OnPrev(wxCommandEvent& p_cEvent)
{
	CHECK_POINTER(m_grList, return);
	//CHECK_BOOL((m_grIt != m_grList->begin()), return);

	wxString l_sSelect = m_select->GetStringSelection();

	if (l_sSelect.IsSameAs(wxT("Constants"))) {
		if (m_grConstIt != m_grConst.begin()) {
			m_grConstIt--;
			m_grList = *m_grConstIt;
			m_grIt = m_grList->begin();
		}
	} else {
		if (m_grIt == m_grList->end())
			m_grIt--;
		if (m_grIt != m_grList->begin())
			m_grIt--;
	}

	ShowFound();
}

void SP_DLG_SearchId::OnFind(wxCommandEvent& p_cEvent)
{
	m_id = m_idSpinCtrl->GetValue();
	m_nclass = m_choice->GetStringSelection();
	m_sName = m_NameTextCtrl->GetValue();
	m_nSelect = m_select->GetSelection();
	m_sSelect = m_select->GetStringSelection();




	if (m_sSelect.IsSameAs(wxT("Constants"))) {
		l_itNC = m_graph->GetNodeclasses()->begin();
		l_itEC = m_graph->GetEdgeclasses()->begin();

		m_grConst.clear();
		m_grList = NULL;
		if (SearchAllElements())
		{
			m_grConstIt = m_grConst.begin();
			m_grList = *m_grConstIt;
			m_grIt = m_grList->begin();
			ShowFound();
		}
		else
		{
			m_pcNext->Enable(false);
			m_pcPrev->Enable(false);
			SP_MESSAGEBOX(wxT("Constant not found"), wxT("Search"), wxOK | wxICON_INFORMATION );
		}

	} else {
		m_nodeList = m_graph->GetNodeclassByDisplayedName(m_nclass)->GetElements();
		m_nodeIt = m_nodeList->begin();

		m_grList = NULL;
		if (FindNextNode())
		{
			ShowFound();
		}
		else
		{
			m_pcNext->Enable(false);
			m_pcPrev->Enable(false);
			SP_MESSAGEBOX(wxT("Node not found"), wxT("Search"), wxOK | wxICON_INFORMATION );
		}

	}


}


bool
SP_DLG_SearchId::SearchAllElements() {

	bool b_lReturn = false;

	if (SearchNodeClasses())
		b_lReturn = true;

	if (SearchEdgeClasses())
		b_lReturn = true;



	return b_lReturn;
}




bool
SP_DLG_SearchId::SearchNodeClasses() {

	bool l_bReturn = false;


	for(; l_itNC != m_graph->GetNodeclasses()->end(); ++l_itNC)
	{
		wxString l_sNodeName = (*l_itNC)->GetDisplayName();
		SP_LOGDEBUG(l_sNodeName);

		if (l_sNodeName.Cmp(wxT("Place")) == 0)
			l_sAttribute = wxT("Marking");

		if (l_sNodeName.Cmp(wxT("Immediate Transition")) == 0)
			l_sAttribute = wxT("Weight");

		if (l_sNodeName.Cmp(wxT("Deterministic Transition")) == 0)
			l_sAttribute = wxT("Delay");

		if (l_sNodeName.Cmp(wxT("Scheduled Transition")) == 0)
			l_sAttribute = wxT("Begin");

		if (l_sNodeName.Cmp(wxT("Transition")) == 0)
			continue;


		m_nodeList = (*l_itNC)->GetElements();
		m_nodeIt = m_nodeList->begin();
		if (FindNextElement())
			l_bReturn = true;
	}

	return l_bReturn;
}

bool
SP_DLG_SearchId::SearchEdgeClasses() {
	bool l_bReturn = false;

	for(; l_itEC != m_graph->GetEdgeclasses()->end(); ++l_itEC)
	{
		wxString l_sNodeName = (*l_itEC)->GetDisplayName();
		SP_LOGDEBUG(l_sNodeName);
		if (l_sNodeName.Cmp(wxT("Edge")) == 0)
				l_sAttribute = wxT("Multiplicity");

		m_edgeList = (*l_itEC)->GetElements();
		m_edgeIt = m_edgeList->begin();
		if (FindNextEdge())
			l_bReturn = true;
	}

	return l_bReturn;
}

bool SP_DLG_SearchId::FindNextNode()
{
	CHECK_POINTER(m_nodeList, return false);

	SP_DS_Attribute* attr;

	for (; m_nodeIt != m_nodeList->end(); m_nodeIt++)
	{
		attr = (*m_nodeIt)->GetAttribute(m_sSelect);
		if(!m_nSelect)
		{
			bool l_bFound = false;
			if (m_exactName->IsChecked())
			{
				l_bFound = (attr)->GetValueString().IsSameAs(m_sName);
			}
			else
			{
				l_bFound = (attr)->GetValueString().Contains(m_sName);
			}

			if(l_bFound)
			{
				SP_LOGDEBUG(wxT("found node"));
				m_grList = (*m_nodeIt)->GetGraphics();
				m_grIt = m_grList->begin();

				// ok, we can start next search from the next node
				m_nodeIt++;
				return true;
			}
		}
		else
		{
			if (m_id == dynamic_cast<SP_DS_IdAttribute*> (attr)->GetValue())
			{
				SP_LOGDEBUG(wxT("found node"));
				m_grList = (*m_nodeIt)->GetGraphics();
				m_grIt = m_grList->begin();

				// ok, we can start next search from the next node
				m_nodeIt++;
				return true;
			}
		}
	}
	return false;
}


bool
SP_DLG_SearchId::FindNextElement() {

	CHECK_POINTER(m_nodeList, return false);

	SP_DS_Attribute* attr;
	SP_DS_Attribute* attr2;
	SP_DS_Attribute* attr3;




	bool l_bReturn = false;

	for (; m_nodeIt != m_nodeList->end(); m_nodeIt++)
	{
		attr = (*m_nodeIt)->GetAttribute(l_sAttribute);

		if (l_sAttribute.Cmp(wxT("Begin")) == 0) {
			attr2 = (*m_nodeIt)->GetAttribute(wxT("Repetition"));
			attr3 = (*m_nodeIt)->GetAttribute(wxT("End"));
		}


		bool l_bFound = false;
		if (m_exactName->IsChecked())
		{
			l_bFound = (attr)->GetValueString().IsSameAs(m_sName);
			if (l_sAttribute.Cmp(wxT("Begin")) == 0) {
				l_bFound = (attr2)->GetValueString().IsSameAs(m_sName);
				l_bFound = (attr3)->GetValueString().IsSameAs(m_sName);
			}
		}
		else
		{
			l_bFound = (attr)->GetValueString().Contains(m_sName);
			if (l_sAttribute.Cmp(wxT("Begin")) == 0) {
				l_bFound = (attr2)->GetValueString().Contains(m_sName);
				l_bFound = (attr3)->GetValueString().Contains(m_sName);
			}
		}

		if(l_bFound)
		{
			SP_LOGDEBUG((*m_nodeIt)->GetAttribute(wxT("Name"))->GetValueString());
			l_bReturn = true;

			SP_LOGDEBUG(wxT("found node"));
			m_grList = (*m_nodeIt)->GetGraphics();
			m_grConst.push_back(m_grList);

		}


	}
	return l_bReturn;
}

bool SP_DLG_SearchId::FindNextEdge() {
	CHECK_POINTER(m_edgeList, return false);

	SP_DS_Attribute* attr;

	bool l_bReturn = false;

	for (; m_edgeIt != m_edgeList->end(); m_edgeIt++)
	{

		attr = (*m_edgeIt)->GetAttribute(l_sAttribute);

		bool l_bFound = false;
		if (m_exactName->IsChecked())
		{
			l_bFound = (attr)->GetValueString().IsSameAs(m_sName);
		}
		else
		{
			l_bFound = (attr)->GetValueString().Contains(m_sName);
		}

		if(l_bFound)
		{


			l_bReturn = true;
			SP_LOGDEBUG(wxT("found edge"));

			m_grList = (*m_edgeIt)->GetGraphics();
			m_grConst.push_back(m_grList);

		}


	}
	return l_bReturn;
}


bool SP_DLG_SearchId::ShowFound()
{
	CHECK_POINTER(m_grList, return false);
	CHECK_BOOL((m_grIt != m_grList->end()), return false);

	unsigned int netNr = (*m_grIt)->GetNetnumber();
	SP_LOGDEBUG(wxString::Format(wxT("found graphic, netnumber %d"), netNr));
	SP_Data *data = m_graph->GetCoarseNode(netNr);
	SP_MDI_Doc *doc;

	if (!data)
	{ // must be the top level document
		SP_LOGDEBUG(wxString::Format(wxT("GetCoarseNode(%d) returned null"), netNr));
		doc = SP_Core::Instance()->GetRootDocument();
		doc->ShowAll();
	}
	else
	{
		// show document, will be opened if needed
		data->GetCoarse()->Show();
		doc = data->GetCoarse()->GetCoarseDoc();
	}
	SP_MDI_View *view = dynamic_cast<SP_MDI_View*> (doc->GetFirstView());
	wxList& views = doc->GetViews();
	SP_LOGDEBUG(wxString::Format(wxT("views: %lu"), views.GetCount()));
	view->SelectAll(false);
	(*m_grIt)->Select(true);

	//make the node visible
	double l_nPosX=0, l_nPosY=0;
	(*m_grIt)->GetPosXY(&l_nPosX, &l_nPosY);
	int l_nSize = wxGetApp().GetCanvasPrefs()->GetGridSpacing(doc->GetNetclassName());
	wxRect l_Rect;
	view->GetCanvas()->GetViewStart(&(l_Rect.x), &(l_Rect.y));
	l_Rect.x *= l_nSize;
	l_Rect.y *= l_nSize;
	view->GetCanvas()->GetClientSize(&l_Rect.width, &l_Rect.height);
	if(l_Rect.GetX() > l_nPosX || l_nPosX > (l_Rect.GetX() + l_Rect.GetWidth())
		|| l_Rect.GetY() > l_nPosY || l_nPosY > (l_Rect.GetY() + l_Rect.GetHeight()))
	{
		int l_nX = (l_nPosX-(l_Rect.GetWidth()/2))/l_nSize;
		int l_nY = (l_nPosY-(l_Rect.GetHeight()/2))/l_nSize;
		l_nX = (l_nX < 0) ? 0 : l_nX;
		l_nY = (l_nY < 0) ? 0 : l_nY;
		view->GetCanvas()->Scroll(l_nX, l_nY);
	}

	// in other cases not needed?
	if (!data)
		doc->Refresh();
	if (m_grIt == m_grList->begin())
	{
		m_pcPrev->Enable(false);
	}
	else
	{
		m_pcPrev->Enable(true);
	}
	if (m_grIt == m_grList->end())
	{
		m_pcNext->Enable(false);
	}
	else
	{
		m_pcNext->Enable(true);
	}

	if (m_sSelect.IsSameAs(wxT("Constants") )){
		m_pcNext->Enable(true);
		m_pcPrev->Enable(true);

		if (m_grConstIt == m_grConst.begin() ) {
			m_pcPrev->Enable(false);
		}

		if (m_grConstIt == m_grConst.end() ) {
			m_pcNext->Enable(false);
		}

	}


	this->Raise();

	return true;
}

void SP_DLG_SearchId::OnChangedSelect(wxCommandEvent& p_cEvent)
{
	wxString l_sSelect = m_select->GetStringSelection();

	if (l_sSelect.IsSameAs(wxT("Constants") ) ){
		m_idSpinCtrl->Show(false);
		m_NameTextCtrl->Show(true);
		m_exactName->Show(true);
		m_choice->Show(false);
		m_NameTextCtrl->Refresh();
	}

	if (l_sSelect.IsSameAs(wxT("Name") ) ){
		m_idSpinCtrl->Show(false);
		m_NameTextCtrl->Show(true);
		m_exactName->Show(true);
		m_choice->Show(true);
		m_NameTextCtrl->Refresh();
	}

	if (l_sSelect.IsSameAs(wxT("ID") ) ){
		m_idSpinCtrl->Show(true);
		m_idSpinCtrl->Refresh();
		m_NameTextCtrl->Show(false);
		m_exactName->Show(false);
		m_choice->Show(true);
	}
}

void SP_DLG_SearchId::SetTitlePrefix(const wxString& p_val)
{
	m_titlePrefix = p_val;
	SetTitle(p_val + m_title);
}

wxString SP_DLG_SearchId::GetTitlePrefix()
{
	return m_titlePrefix;
}
