//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/26 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/dia_CPN/SP_DLG_BindingSelection.h"

#include <wx/valgen.h>
#include <wx/tipwin.h>
#include "sp_core/SP_Core.h"

IMPLEMENT_CLASS( SP_DLG_BindingSelection, wxDialog )

enum
{	
	SP_ID_LISTCTRL_BINDING
};

BEGIN_EVENT_TABLE( SP_DLG_BindingSelection, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_BindingSelection::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_BindingSelection::OnDlgCancel )

END_EVENT_TABLE()

BEGIN_EVENT_TABLE( SP_DLG_BindingSelection_ListCtrl, wxListCtrl )

EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_BINDING, SP_DLG_BindingSelection_ListCtrl::OnDblClick )
EVT_LIST_ITEM_SELECTED( SP_ID_LISTCTRL_BINDING, SP_DLG_BindingSelection_ListCtrl::OnSelected )

END_EVENT_TABLE()

SP_DLG_BindingSelection::SP_DLG_BindingSelection( vector<wxString>& p_BindingArray,
		wxWindow* p_pcParent, wxString p_sHelpText,
		const wxString& p_sTitle, long p_nStyle)

:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize(300, 500),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX )

{	
	m_nSelection = 0;
	
	SetTitle(wxT("Binding selection"));	

	m_BindingArray = p_BindingArray;

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	//listboxes


	wxStaticBoxSizer* l_pcParameterListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Bindings")), wxHORIZONTAL);
	m_pcBindingListCtrl = new SP_DLG_BindingSelection_ListCtrl(this, this, SP_ID_LISTCTRL_BINDING, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcParameterListBoxSizer->Add(m_pcBindingListCtrl, 1, wxALL| wxEXPAND, 2);	

	m_pcSizer->Add(l_pcParameterListBoxSizer, 1, wxALL | wxEXPAND, 1);
	//buttons
	wxBoxSizer* l_pcButtonSizer =	new wxBoxSizer( wxHORIZONTAL);
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, 5);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALL , 1);

	LoadData();

	//  SetAutoLayout( TRUE );
	SetSizer(m_pcSizer);

	//  m_pcSizer->Fit( this );
	m_pcSizer->SetSizeHints(this);

	Layout();

}

//------------------------------------------------------------------------
void SP_DLG_BindingSelection::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow())
	{		

		m_nReturnSelection = m_nSelection;

		if (IsModal())
		{
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}

	}

}

void SP_DLG_BindingSelection::OnDlgCancel(wxCommandEvent& p_cEvent)
{
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


void SP_DLG_BindingSelection::LoadData()
{
	
	wxListItem l_cItem;

	m_pcBindingListCtrl->InsertColumn(0, wxT("No."), wxLIST_FORMAT_LEFT);
	m_pcBindingListCtrl->InsertColumn(1, wxT("Binding"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);	
	l_cItem.m_mask = wxLIST_MASK_TEXT;

	for( unsigned i = 0; i < m_BindingArray.size(); i++)
	{
		l_cItem.m_col = 0;
		l_cItem.m_itemId = i;
		l_cItem.m_format = wxLIST_FORMAT_LEFT;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << i ;
		long l_nIndex = m_pcBindingListCtrl->InsertItem(l_cItem);

		m_pcBindingListCtrl->SetItem(l_nIndex,1,m_BindingArray[i]);
	}

	m_pcBindingListCtrl->SetColumnWidth(0, 30);
	m_pcBindingListCtrl->SetColumnWidth(1, 160);

}


void SP_DLG_BindingSelection_ListCtrl::OnDblClick(wxListEvent& event)
{	

	m_pcParentDlg->SetSelection( event.m_itemIndex );


}


void SP_DLG_BindingSelection_ListCtrl::OnSelected( wxListEvent& event )
{	
	m_pcParentDlg->SetSelection( event.m_itemIndex );
}
