//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/07/05 $
// Short Description: dialog to search for nodes by id
//////////////////////////////////////////////////////////////////////

#include <wx/regex.h>

#include "sp_gui/dialogs/SP_DLG_SearchId.h"

#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
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
	m_title(p_title), m_nSelect(0), m_sSelect(wxT("")), m_id(-1), m_sName(wxT("")), m_graph(p_graph), m_edgeList(nullptr), m_nodeList(nullptr)
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
		choices.Add(wxT("Constants"));

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
	wxString l_sSelect = m_select->GetStringSelection();


	if (m_grIt != m_grList.end())
		m_grIt++;
	if (m_grIt == m_grList.end())
	{
		m_pcNext->Enable(false);
		m_pcPrev->Enable(true);

		if (l_sSelect.IsSameAs(wxT("Constants")))
		{
			if(!SearchAllElements())
			{
				return;
			}
		}
		else
		{
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
	wxString l_sSelect = m_select->GetStringSelection();

	if (m_grIt == m_grList.end())
		m_grIt--;
	if (m_grIt != m_grList.begin())
		m_grIt--;

	ShowFound();
}

void SP_DLG_SearchId::OnFind(wxCommandEvent& p_cEvent)
{
	m_id = m_idSpinCtrl->GetValue();
	m_nclass = m_choice->GetStringSelection();
	m_sName = m_NameTextCtrl->GetValue();
	m_nSelect = m_select->GetSelection();
	m_sSelect = m_select->GetStringSelection();


	m_grList.clear();

	if (m_sSelect.IsSameAs(wxT("Constants"))) {
		m_itNC = m_graph->GetNodeclasses()->begin();
		m_itEC = m_graph->GetEdgeclasses()->begin();

		if (SearchAllElements())
		{
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

	if (!SearchNodeClasses())
	{
		if (!SearchEdgeClasses())
		{
			return false;
		}
	}


	return true;
}




bool
SP_DLG_SearchId::SearchNodeClasses() {

	for(; m_itNC != m_graph->GetNodeclasses()->end(); ++m_itNC)
	{
		wxString l_sNodeName = (*m_itNC)->GetDisplayName();
		SP_LOGDEBUG(l_sNodeName);

		if(!m_nodeList)
		{
			m_nodeList = (*m_itNC)->GetElements();
			m_nodeIt = m_nodeList->begin();
		}
		if (FindNextElement())
			return true;
	}

	return false;
}

bool
SP_DLG_SearchId::SearchEdgeClasses() {

	for(; m_itEC != m_graph->GetEdgeclasses()->end(); ++m_itEC)
	{
		wxString l_sNodeName = (*m_itEC)->GetDisplayName();
		SP_LOGDEBUG(l_sNodeName);

		if(!m_edgeList)
		{
			m_edgeList = (*m_itEC)->GetElements();
			m_edgeIt = m_edgeList->begin();
		}
		if (FindNextEdge())
			return true;
	}

	return false;
}

bool SP_DLG_SearchId::FindNextNode()
{
	CHECK_POINTER(m_nodeList, return false);

	SP_DS_Attribute* attr;

	for (; m_nodeIt != m_nodeList->end(); m_nodeIt++)
	{
		attr = (*m_nodeIt)->GetAttribute(m_sSelect);
		if(m_nSelect == 0)
		{
			bool l_bFound = false;
			if (m_exactName->IsChecked())
			{
				l_bFound = (attr)->GetValueString().IsSameAs(m_sName);
			}
			else
			{
				wxRegEx regex(m_sName);
				l_bFound = regex.Matches((attr)->GetValueString());
			}

			if(l_bFound)
			{
				SP_LOGDEBUG(wxT("found node"));
				m_grIt = m_grList.insert(m_grList.end(), (*m_nodeIt)->GetGraphics()->begin(), (*m_nodeIt)->GetGraphics()->end());

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
				m_grIt = m_grList.insert(m_grList.end(), (*m_nodeIt)->GetGraphics()->begin(), (*m_nodeIt)->GetGraphics()->end());

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

	for (; m_nodeIt != m_nodeList->end(); ++m_nodeIt)
	{
		if(SearchAttributes((*m_nodeIt)->GetAttributes()))
		{
			SP_LOGDEBUG((*m_nodeIt)->GetAttribute(wxT("Name"))->GetValueString());

			SP_LOGDEBUG(wxT("found node"));
			m_grIt = m_grList.insert(m_grList.end(), (*m_nodeIt)->GetGraphics()->begin(), (*m_nodeIt)->GetGraphics()->end());

			// ok, we can start next search from the next node
			++m_nodeIt;
			return true;
		}
	}
	if(m_nodeIt == m_nodeList->end())
	{
		m_nodeList = nullptr;
	}
	return false;
}

bool SP_DLG_SearchId::FindNextEdge() {
	CHECK_POINTER(m_edgeList, return false);

	for (; m_edgeIt != m_edgeList->end(); ++m_edgeIt)
	{
		if(SearchAttributes((*m_edgeIt)->GetAttributes()))
		{
			SP_LOGDEBUG(wxT("found edge"));

			m_grIt = m_grList.insert(m_grList.end(), (*m_edgeIt)->GetGraphics()->begin(), (*m_edgeIt)->GetGraphics()->end());

			// ok, we can start next search from the next edge
			++m_edgeIt;
			return true;
		}
	}
	if(m_edgeIt == m_edgeList->end())
	{
		m_edgeList = nullptr;
	}
	return false;
}

bool SP_DLG_SearchId::SearchAttributes(const SP_ListAttribute* attributes)
{
	bool l_bFound = false;
	for(SP_DS_Attribute* attr : *(attributes))
	{
		if(attr->GetName() == wxT("Name") || attr->GetName() == wxT("ID"))
		{
			continue;
		}
		if(attr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
		{
			SP_DS_ColListAttribute* attrCol = static_cast<SP_DS_ColListAttribute*>(attr);
			for(unsigned i = 0; i < attrCol->GetRowCount(); ++i)
			{
				for(unsigned k = 0; k < attrCol->GetColCount(); ++k)
				{
					if (m_exactName->IsChecked())
					{
						l_bFound = (attrCol)->GetCell(i,k).IsSameAs(m_sName);
					}
					else
					{
						wxRegEx regex(m_sName);
						l_bFound = regex.Matches((attrCol)->GetCell(i,k));
					}
					if(l_bFound) return true;
				}
			}
		}
		else
		{
			if (m_exactName->IsChecked())
			{
				l_bFound = (attr)->GetValueString().IsSameAs(m_sName);
			}
			else
			{
				wxRegEx regex(m_sName);
				l_bFound = regex.Matches((attr)->GetValueString());
			}
			if(l_bFound) return true;
		}
	}
	return false;
}

bool SP_DLG_SearchId::ShowFound(bool all)
{
	CHECK_BOOL((m_grIt != m_grList.end()), return false);

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
	if(!all) {
		view->SelectAll(false);
	}
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
	{
		doc->Refresh();
	}
	if (m_grIt == m_grList.begin())
	{
		m_pcPrev->Enable(false);
	}
	else
	{
		m_pcPrev->Enable(true);
	}
	if (m_grIt == m_grList.end())
	{
		m_pcNext->Enable(false);
	}
	else
	{
		m_pcNext->Enable(true);
	}

	this->Raise();

	return true;
}

void SP_DLG_SearchId::OnChangedSelect(wxCommandEvent& p_cEvent)
{
	wxString l_sSelect = m_select->GetStringSelection();

	if(m_sSelect != l_sSelect){
		m_grList.clear();
		m_pcNext->Enable(false);
		m_pcPrev->Enable(false);
	}

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
