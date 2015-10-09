//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2015/10/08 $
// Short Description: dialog to divide nodes
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_DivideNodes.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

int wxCALLBACK
SP_CompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	SP_ItemData* i1 = reinterpret_cast<SP_ItemData*>(item1);
	SP_ItemData* i2 = reinterpret_cast<SP_ItemData*>(item2);
	if(sortData == 0)
	{
		if(i1->name < i2->name)
			return -1;
		if(i2->name < i1->name)
			return 1;
		if(i1->count < i2->count)
			return 1;
		if(i2->count < i1->count)
			return -1;
	}
	else
	{
		if(i1->count < i2->count)
			return 1;
		if(i2->count < i1->count)
			return -1;
		if(i1->name < i2->name)
			return -1;
		if(i2->name < i1->name)
			return 1;
	}

	return 0;
}


IMPLEMENT_CLASS(SP_DLG_DivideNodes, wxDialog)

enum
{		
	SP_ID_LISTCTRL_ELEMENT,
	SP_ID_RADIO,
	SP_ID_SPIN
};

BEGIN_EVENT_TABLE(SP_DLG_DivideNodes, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_DivideNodes::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_DivideNodes::OnDlgClose)
	EVT_BUTTON( wxID_APPLY, SP_DLG_DivideNodes::OnDlgApply )	
	EVT_RADIOBOX(SP_ID_RADIO, SP_DLG_DivideNodes::OnRadio)
	EVT_SPINCTRL(SP_ID_SPIN, SP_DLG_DivideNodes::OnSpin)
	EVT_LIST_COL_CLICK(SP_ID_LISTCTRL_ELEMENT, SP_DLG_DivideNodes::OnColClick)
END_EVENT_TABLE()

SP_DLG_DivideNodes::SP_DLG_DivideNodes(SP_DS_Graph *p_graph, unsigned int p_netNumber, SP_ListGraphic* p_graphics, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
	 m_choice(NULL), m_sort(NULL), m_direction(NULL), m_graph(p_graph), m_netNumber(p_netNumber), m_graphics(p_graphics)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* UserdefiningSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* elemSizer = new wxBoxSizer(wxVERTICAL);

	wxArrayString choices;
	//lookup nodeclasses
	SP_ListNodeclass::const_iterator ncIt;
	for (SP_DS_Nodeclass* nc : *m_graph->GetNodeclasses())
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LOGIC))
			{
				choices.Add(nc->GetDisplayName());
			}
		}
	}

	if (!choices.IsEmpty())
	{
		int l_max = 0;
		for (SP_DS_Edgeclass* ec : *m_graph->GetEdgeclasses())
		{
			l_max += ec->GetElements()->size();
		}

		wxBoxSizer* numberSizer = new wxBoxSizer(wxHORIZONTAL);

		m_text = new wxStaticText(this, wxID_ANY, wxT("min # of arcs "));
		numberSizer->Add(m_text);

		m_number = new wxSpinCtrl(this, SP_ID_SPIN, wxT("2"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, l_max);
		numberSizer->Add(m_number);
		elemSizer->Add(numberSizer);

		m_choice = new wxRadioBox(this, SP_ID_RADIO, wxT("Elements"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
		elemSizer->Add(m_choice);

		UserdefiningSizer->Add(elemSizer, 0, wxALL | wxEXPAND, 1);

		m_pcListCtrl = new wxListCtrl(this, SP_ID_LISTCTRL_ELEMENT, wxDefaultPosition,wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES );
		UserdefiningSizer->Add(m_pcListCtrl, 1, wxALL | wxEXPAND, 1);

		
	}
	else
	{
		UserdefiningSizer->Add(new wxStaticText(this, -1, wxT("No nodes with logic attribute!")));
	}

	topSizer->Prepend(UserdefiningSizer, 1, wxALL | wxEXPAND, 5);

	wxStdDialogButtonSizer* l_pcButtonSizer = CreateStdDialogButtonSizer(wxOK|wxCANCEL);

	topSizer->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 1);

	if (m_choice)
	{
		LoadData(m_choice->GetStringSelection(), m_number->GetValue());
	}

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_DivideNodes::~SP_DLG_DivideNodes()
{
}

void SP_DLG_DivideNodes::Clear()
{
	long item = m_pcListCtrl->GetNextItem(-1, wxLIST_NEXT_BELOW);
	while (item != -1) {
		wxString l_sItem = m_pcListCtrl->GetItemText(item);
		SP_ItemData* data = (reinterpret_cast<SP_ItemData*>(m_pcListCtrl->GetItemData(item)));
		wxDELETE(data);
		m_pcListCtrl->SetItemData(item, 0);
		item = m_pcListCtrl->GetNextItem(item, wxLIST_NEXT_BELOW);
	}
	m_pcListCtrl->ClearAll();
}


void SP_DLG_DivideNodes::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		DoApply();
		Clear();

		if (IsModal() )
		{
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}

}

void SP_DLG_DivideNodes::OnDlgApply( wxCommandEvent& p_cEvent )
{
	if (Validate() && TransferDataFromWindow() )
	{
		DoApply();
		LoadData( m_choice->GetStringSelection(), m_number->GetValue() );
	}
}

void SP_DLG_DivideNodes::DoApply()
{
	m_graphics->clear();

	int state = ( m_pcListCtrl->GetSelectedItemCount() == 0 ) ? wxLIST_STATE_DONTCARE : wxLIST_STATE_SELECTED;

	long item = m_pcListCtrl->GetNextItem(-1, wxLIST_NEXT_BELOW, state);
	while (item != -1) {
		wxString l_sItem = m_pcListCtrl->GetItemText(item);
		SP_ItemData* data = reinterpret_cast<SP_ItemData*>(m_pcListCtrl->GetItemData(item));
		m_graphics->push_back(data->pGraphic);
		item = m_pcListCtrl->GetNextItem(item, wxLIST_NEXT_BELOW, state);
	}

	m_graph->GetParentDoc()->Modify(true);
}

void SP_DLG_DivideNodes::OnDlgClose(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		Clear();
		if (IsModal())
		{
			EndModal(wxID_CANCEL);
		}
		else
		{
			SetReturnCode(wxID_CANCEL);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_DivideNodes::LoadData(const wxString& p_sNodeClassName, int p_ncount)
{
	//Load data
	Clear();
	m_pcListCtrl->InsertColumn( 0, p_sNodeClassName );
	m_pcListCtrl->InsertColumn( 1, wxT("# of arcs") );

	int l_nPos = 0;
	long l_nIndex = 0;

	SP_DS_Nodeclass* l_pcNodeclass = m_graph->GetNodeclassByDisplayedName( p_sNodeClassName );
	if(l_pcNodeclass)
	{
		for( SP_DS_Node* l_pcNode : *l_pcNodeclass->GetElements())
		{
			wxString l_sElementName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			std::map<SP_Graphic*, size_t> l_GrCount;
			for (SP_DS_Edge* l_pcEdge : *l_pcNode->GetSourceEdges())
			{
				for (SP_Graphic *l_pcGr : *l_pcEdge->GetGraphics())
				{
					if(l_pcGr->GetNetnumber() == m_netNumber)
					{
						SP_GR_Edge *l_grEdge = static_cast<SP_GR_Edge *>(l_pcGr);
						++l_GrCount[l_grEdge->GetSource()];
					}
				}
			}
			for (SP_DS_Edge* l_pcEdge : *l_pcNode->GetTargetEdges())
			{
				for (SP_Graphic *l_pcGr : *l_pcEdge->GetGraphics())
				{
					if(l_pcGr->GetNetnumber() == m_netNumber)
					{
						SP_GR_Edge *l_grEdge = static_cast<SP_GR_Edge *>(l_pcGr);
						++l_GrCount[l_grEdge->GetTarget()];
					}
				}
			}
			for(auto it : l_GrCount)
			{
				if (it.second >= p_ncount)
				{
					wxListItem l_cItem;
					l_cItem.SetBackgroundColour(*wxWHITE);
					l_cItem.m_col = 0;
					l_cItem.m_mask = wxLIST_MASK_TEXT;
					l_cItem.m_itemId = l_nPos++;
					l_cItem.m_text = l_sElementName;
					SP_ItemData* data = new SP_ItemData(it.first, it.second, l_sElementName);
					l_cItem.SetData(data);
					l_nIndex = m_pcListCtrl->InsertItem(l_cItem);
					m_pcListCtrl->SetItem(l_nIndex, 1, wxString::Format(wxT("%lu"), it.second));
				}
			}
		}
		m_pcListCtrl->SortItems(SP_CompareFunction, 1);
	}
}

void SP_DLG_DivideNodes::OnRadio( wxCommandEvent& p_cEvent )
{
	LoadData( m_choice->GetStringSelection(), m_number->GetValue() );

}

void SP_DLG_DivideNodes::OnSpin(wxSpinEvent &p_cEvent)
{
	LoadData( m_choice->GetStringSelection(), m_number->GetValue() );

}

void SP_DLG_DivideNodes::OnColClick(wxListEvent &event)
{
	int col = event.GetColumn();
	m_pcListCtrl->SortItems(SP_CompareFunction, col);
}
