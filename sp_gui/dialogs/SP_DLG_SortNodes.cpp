//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_SortNodes.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include <wx/spinctrl.h>
#include "sp_ds/attributes/SP_DS_NameAttribute.h"



IMPLEMENT_CLASS(SP_DLG_SortNodes, wxDialog)

enum
{		
	SP_ID_LISTCTRL_ELEMENT,
	SP_ID_RADIO
};

BEGIN_EVENT_TABLE(SP_DLG_SortNodes, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_SortNodes::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_SortNodes::OnDlgClose)
	EVT_BUTTON( wxID_APPLY, SP_DLG_SortNodes::OnDlgApply )	
	EVT_RADIOBOX(SP_ID_RADIO, SP_DLG_SortNodes::OnRadio)
END_EVENT_TABLE()

SP_DLG_SortNodes::SP_DLG_SortNodes(SP_DS_Graph *p_graph, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
	 m_choice(NULL), m_sort(NULL), m_direction(NULL), m_graph(p_graph)
{
	

	dragging = 0;
	m_bListCtrlChanged = false;
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* UserdefiningSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* elemSizer = new wxBoxSizer(wxVERTICAL);

	wxArrayString choices;
	//lookup nodeclasses
	SP_ListNodeclass::const_iterator ncIt;
	for (ncIt = m_graph->GetNodeclasses()->begin(); ncIt != m_graph->GetNodeclasses()->end(); ++ncIt)
	{
		if ((*ncIt)->GetShowInElementTree())
		{
			if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && (*ncIt)->GetPrototype()->GetAttribute(wxT("Name")))
			{
				choices.Add((*ncIt)->GetDisplayName());
			}
		}
	}
	//lookup metadataclasses
	SP_ListMetadataclass::const_iterator mcIt;
	for (mcIt = m_graph->GetMetadataclasses()->begin(); mcIt != m_graph->GetMetadataclasses()->end(); ++mcIt)
	{
		if ((*mcIt)->GetShowInDeclarationTreeOther() && !(*mcIt)->GetShowInDeclarationTreeColorSet())
		{
			if ((*mcIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && (*mcIt)->GetPrototype()->GetAttribute(wxT("Name")))
			{
				choices.Add((*mcIt)->GetDisplayName());
			}
		}
	}

	if (!choices.IsEmpty())
	{
		m_choice = new wxRadioBox(this, SP_ID_RADIO, wxT("Elements"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);

		elemSizer->Add(m_choice);

		choices.Clear();
		choices.Add(wxT("Name"));
		choices.Add(wxT("ID"));
		choices.Add(wxT("Manual"));
		m_sort = new wxRadioBox(this, -1, wxT("Sort by"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);

		elemSizer->Add(m_sort);

		choices.Clear();
		choices.Add(wxT("Ascending"));
		choices.Add(wxT("Descending"));
		m_direction = new wxRadioBox(this, -1, wxT("Direction"), wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);

		elemSizer->Add(m_direction);

		UserdefiningSizer->Add(elemSizer, 0, wxALL | wxEXPAND, 1);

		m_pcListCtrl = new wxListCtrl(this, SP_ID_LISTCTRL_ELEMENT, wxDefaultPosition,wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL );

		UserdefiningSizer->Add(m_pcListCtrl, 0, wxALL | wxEXPAND, 1);

		
	}
	else
	{
		UserdefiningSizer->Add(new wxStaticText(this, -1, wxT("No nodes with IDs!")));
	}

	topSizer->Prepend(UserdefiningSizer, 1, wxALL, 5);

	wxSizer* l_pcButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);

	topSizer->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 1);

	if (m_choice)
	{
		LoadData(m_choice->GetStringSelection());

		m_pcListCtrl->Connect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( SP_DLG_SortNodes::OnBeginDrag ), 0, this );
 		m_pcListCtrl->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( SP_DLG_SortNodes::OnLeftUp ), 0, this );
 		m_pcListCtrl->Connect( wxEVT_MOTION, wxMouseEventHandler( SP_DLG_SortNodes::OnMotion ), 0, this );

		m_pcListCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SP_DLG_SortNodes::OnSelected ), 0, this );
		m_pcListCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( SP_DLG_SortNodes::OnKeyUp ), 0, this );
		m_pcListCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( SP_DLG_SortNodes::OnKeyDown ), 0, this );
	}

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_SortNodes::~SP_DLG_SortNodes()
{
	if( dragging )
 	{
 		delete dragging;
 		dragging = 0;
 	}
}

void SP_DLG_SortNodes::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		DoApply();

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

void SP_DLG_SortNodes::OnDlgApply( wxCommandEvent& p_cEvent )
{
	if (Validate() && TransferDataFromWindow() )
	{
		DoApply();
		LoadData( m_choice->GetStringSelection() );
	}
}

void SP_DLG_SortNodes::DoApply()
{
	if( m_bListCtrlChanged )
	{
		//write id to node class
		map<wxString,long> l_mName2ID;
		long l_nID = 0;

		long item = m_pcListCtrl->GetNextItem(-1, wxLIST_NEXT_BELOW);
		while ( item != -1 )
		{
			wxString l_sItem = m_pcListCtrl->GetItemText(item).c_str();
			l_mName2ID[l_sItem] = l_nID;
			item = m_pcListCtrl->GetNextItem(item, wxLIST_NEXT_BELOW);
			l_nID++;
		}

		SP_DS_Nodeclass* l_pcNodeclass = m_graph->GetNodeclassByDisplayedName( m_choice->GetStringSelection() );
		if (l_pcNodeclass)
		{
			SP_ListNode::const_iterator l_itElem;
			for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
				++l_itElem )
			{
				wxString l_sElementName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				long l_nSingleID = l_mName2ID[l_sElementName];
				dynamic_cast<SP_DS_IdAttribute*>((*l_itElem)->GetAttribute(wxT("ID")))->SetValue(l_nSingleID);
				(*l_itElem)->Update(true);
			}
		}
		else
		{
			SP_DS_Metadataclass* mc = m_graph->GetMetadataclassByDisplayedName(m_choice->GetStringSelection());
			if (mc)
			{
				SP_ListMetadata::const_iterator l_itElem;
				for( l_itElem = mc->GetElements()->begin(); l_itElem != mc->GetElements()->end();
					++l_itElem )
				{
					wxString l_sElementName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					long l_nSingleID = l_mName2ID[l_sElementName];
					dynamic_cast<SP_DS_IdAttribute*>((*l_itElem)->GetAttribute(wxT("ID")))->SetValue(l_nSingleID);
					(*l_itElem)->Update(true);
				}
			}
		}
		m_bListCtrlChanged = false;
	}

	if(Sort(m_choice->GetStringSelection(), m_sort->GetStringSelection(), m_direction->GetSelection()))
	{
		m_graph->GetParentDoc()->Modify(true);
	}
}

void SP_DLG_SortNodes::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}


template<typename T>
struct CompareNodes
{
	CompareNodes(wxString p_sAttribute, bool p_bDirection)
	: m_sAttribute(p_sAttribute), m_bDirection(p_bDirection)
	{}

	wxString m_sAttribute;
	bool m_bDirection;

	bool operator()(T* p_pcFirst, T* p_pcSecond)
	{
		bool l_bReturn = false;
		if(m_sAttribute == wxT("Name"))
		{
			if(m_bDirection)
			{
				if( p_pcFirst->GetAttribute(m_sAttribute)->GetValueString() >
					p_pcSecond->GetAttribute(m_sAttribute)->GetValueString())
				{
					l_bReturn = true;
				}
			}
			else
			{
				if( p_pcFirst->GetAttribute(m_sAttribute)->GetValueString() <
					p_pcSecond->GetAttribute(m_sAttribute)->GetValueString())
				{
					l_bReturn = true;
				}
			}
		}
		else if(m_sAttribute == wxT("ID"))
		{
			long l_nFirst = dynamic_cast<SP_DS_IdAttribute*>(p_pcFirst->GetAttribute(m_sAttribute))->GetValue();
			long l_nSecond = dynamic_cast<SP_DS_IdAttribute*>(p_pcSecond->GetAttribute(m_sAttribute))->GetValue();
			if(m_bDirection)
			{
				if( l_nFirst > l_nSecond )
				{
					l_bReturn = true;
				}
			}
			else
			{
				if( l_nFirst < l_nSecond )
				{
					l_bReturn = true;
				}
			}
		}

		return l_bReturn;
	}
};

bool SP_DLG_SortNodes::Sort(const wxString& p_sNode, const wxString& p_sAttribute, bool p_bDirection)
{
	CHECK_POINTER(m_graph, return false);

	wxString l_sAttribute = p_sAttribute;
	if(l_sAttribute == wxT("Manual"))
	{
		l_sAttribute = wxT("ID");
	}

	SP_DS_Nodeclass* nc = m_graph->GetNodeclassByDisplayedName(p_sNode);

	if(nc)
	{
		const SP_ListNode* l_nodeList = nc->GetElements();
		//TODO:
		CompareNodes<SP_DS_Node> comp(l_sAttribute,p_bDirection);
		const_cast<SP_ListNode*>(l_nodeList)->sort(comp);
	}
	else
	{
		SP_DS_Metadataclass* mc = m_graph->GetMetadataclassByDisplayedName(m_choice->GetStringSelection());
		if(mc)
		{
			const SP_ListMetadata* l_metadataList = mc->GetElements();
			CompareNodes<SP_DS_Metadata> comp(l_sAttribute,p_bDirection);
			const_cast<SP_ListMetadata*>(l_metadataList)->sort(comp);
		}
	}
	return true;
}

void SP_DLG_SortNodes::LoadData(wxString p_sNodeClassName)
{
	//Load data
	m_pcListCtrl->ClearAll();
	m_pcListCtrl->InsertColumn( 0, p_sNodeClassName,wxLIST_FORMAT_LEFT,195 );

	int l_nPos = 0;
	long l_nIndex = 0;

	SP_DS_Nodeclass* l_pcNodeclass = m_graph->GetNodeclassByDisplayedName( p_sNodeClassName );
	if(l_pcNodeclass)
	{
		SP_ListNode::const_iterator l_itElem;
		for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem )
		{
			wxString l_sElementName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			//m_pcListCtrl->InsertItem( 0, l_sElementName );

			wxListItem l_cItem;
			l_cItem.SetBackgroundColour(*wxWHITE);
			l_cItem.m_col = 0;
			l_cItem.m_mask = wxLIST_MASK_TEXT;
			l_cItem.m_itemId = l_nPos++;
			l_cItem.m_text = wxT("");
			l_cItem.m_text << l_sElementName;
			l_nIndex = m_pcListCtrl->InsertItem(l_cItem);
		}
	}
	else
	{
		SP_DS_Metadataclass* mc = m_graph->GetMetadataclassByDisplayedName(p_sNodeClassName);
		if(mc)
		{
			SP_ListMetadata::const_iterator l_itElem;
			for( l_itElem = mc->GetElements()->begin(); l_itElem != mc->GetElements()->end();
				++l_itElem )
			{
				wxString l_sElementName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				//m_pcListCtrl->InsertItem( 0, l_sElementName );

				wxListItem l_cItem;
				l_cItem.SetBackgroundColour(*wxWHITE);
				l_cItem.m_col = 0;
				l_cItem.m_mask = wxLIST_MASK_TEXT;
				l_cItem.m_itemId = l_nPos++;
				l_cItem.m_text = wxT("");
				l_cItem.m_text << l_sElementName;
				l_nIndex = m_pcListCtrl->InsertItem(l_cItem);

			}
		}
	}
}


 	void SP_DLG_SortNodes::OnBeginDrag( wxListEvent& event )
 	{
 		dragging = new int( event.GetIndex() );
 	}
 	
 	void SP_DLG_SortNodes::OnLeftUp( wxMouseEvent& event )
 	{
 		if( dragging ) {
 			delete dragging;
 			dragging = 0;
 		}
 	}
 	
 	void SP_DLG_SortNodes::OnMotion( wxMouseEvent& e )
 	{
 		if( dragging )
 		{
 			wxListCtrl* list = wxDynamicCast(e.GetEventObject(), wxListCtrl);
 			int hitFlags;
 			int hitIndex = list->HitTest( wxPoint( e.GetX(), e.GetY() ), hitFlags, 0 );
 			if( hitIndex != wxNOT_FOUND )
 			{
				m_bListCtrlChanged = true;
				m_sort->SetStringSelection(wxT("Manual"));


 				for( int i = 0; i < (hitIndex - *dragging); ++i )
 					swap( list, *dragging + i, *dragging + i + 1 );
 				for( int i = 0; i < (*dragging - hitIndex); ++i )
 					swap( list, *dragging - i, *dragging - i - 1 );
 				*dragging = hitIndex;
 			}
 		}
 		else
 			e.Skip();
 	}
 	
	void SP_DLG_SortNodes::swap( wxListCtrl* list, int index1, int index2 )
 	{
 		wxListItem item1;
 		item1.SetId( index1 );
		item1.SetMask( wxLIST_MASK_STATE | wxLIST_MASK_TEXT );
		list->GetItem( item1 );
		wxListItem item2;
		item2.SetId( index2 );
		item2.SetMask( wxLIST_MASK_STATE | wxLIST_MASK_TEXT ); 
		list->GetItem( item2 );
		item1.SetId( index2 ); list->SetItem( item1 );
		item2.SetId( index1 ); list->SetItem( item2 );
 	}


	void SP_DLG_SortNodes::OnRadio( wxCommandEvent& p_cEvent )
	{
		m_bListCtrlChanged = false;
		LoadData( m_choice->GetStringSelection() );

	}


	void SP_DLG_SortNodes::OnKeyDown(wxKeyEvent& p_cEvent)
	{
		wxListCtrl* list = wxDynamicCast(p_cEvent.GetEventObject(), wxListCtrl);
		int hitIndex = list->GetNextItem(-1,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);		

		int l_nCount = list->GetItemCount();		

		
		{
			switch(p_cEvent.GetKeyCode())
			{
				case WXK_DOWN:
				{
					if( !( hitIndex == wxNOT_FOUND || hitIndex == l_nCount-1) )
 					{
						m_bListCtrlChanged = true;
						m_sort->SetStringSelection(wxT("ID"));

 						swap( list, hitIndex, hitIndex+1 );

						wxListItem info;
						info.SetId(hitIndex+1);
						info.SetState(wxLIST_STATE_SELECTED );
						list->SetItem(info);
 					}						   
			   		break;
				 }			  
				case WXK_UP:
	    		{
					if( !( hitIndex == wxNOT_FOUND || hitIndex == 0) )
 					{
						m_bListCtrlChanged = true;
						m_sort->SetStringSelection(wxT("ID"));

						swap( list, hitIndex-1, hitIndex );

						wxListItem info;
						info.SetId(hitIndex-1);
						info.SetState(wxLIST_STATE_SELECTED );
						list->SetItem(info);	
 						
 					}						   
			   		break;
				 }	
				default:
					p_cEvent.Skip();
					break;
			  }			
		}

	
	}

	void SP_DLG_SortNodes::OnKeyUp(wxKeyEvent& p_cEvent)
	{
		

	}

	void SP_DLG_SortNodes::OnSelected( wxListEvent& event )
	{
		//m_nSel =  int( event.GetIndex() );
	}
