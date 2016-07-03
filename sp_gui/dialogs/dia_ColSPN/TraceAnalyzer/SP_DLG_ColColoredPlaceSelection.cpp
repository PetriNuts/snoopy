//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColColoredPlaceSelection.h"

IMPLEMENT_CLASS( SP_DLG_ColColoredPlaceSelection, wxDialog )

enum
{
	SP_ID_LISTBOX_IN,
	SP_ID_BUTTON_TOPLR,
	SP_ID_BUTTON_TOPRL,
	SP_ID_LISTBOX_OUT,
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_IMPORTCOLORSET,
	SP_ID_GRID_MARKING
};

BEGIN_EVENT_TABLE( SP_DLG_ColColoredPlaceSelection, wxDialog )

	EVT_BUTTON( wxID_OK, SP_DLG_ColColoredPlaceSelection::OnDlgOk )
	EVT_BUTTON( wxID_CANCEL, SP_DLG_ColColoredPlaceSelection::OnDlgCancel )
	
	EVT_BUTTON(SP_ID_BUTTON_TOPLR, SP_DLG_ColColoredPlaceSelection::OnLeftToRight)
	EVT_BUTTON(SP_ID_BUTTON_TOPRL, SP_DLG_ColColoredPlaceSelection::OnRightToLeft)


END_EVENT_TABLE()



SP_DLG_ColColoredPlaceSelection::SP_DLG_ColColoredPlaceSelection(wxArrayString& p_ArrayString,wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
	m_ArrayString = p_ArrayString;
	m_pcBoxSizer =  new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* l_pcRowSizerforEnzym = new wxBoxSizer( wxVERTICAL );
	wxStaticText* m_pStaticTextIn=new wxStaticText(this,-1,wxT("The overall colored places"),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	m_pListBoxIn = new wxListBox(this, SP_ID_LISTBOX_IN,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_EXTENDED);
	l_pcRowSizerforEnzym->Add(m_pStaticTextIn, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizerforEnzym->Add(m_pListBoxIn, 1, wxEXPAND | wxALL, 5);

	m_pListBoxIn->Clear();
	if (!m_ArrayString.IsEmpty())
	{
	  m_pListBoxIn->InsertItems(m_ArrayString,0);
    }

    wxBoxSizer* l_pcRowSizerleft = new wxBoxSizer( wxVERTICAL );
	wxStaticText* m_pStatic1=new wxStaticText(this,-1,wxT(""),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	wxButton* l_pButton1=new wxButton( this, SP_ID_BUTTON_TOPLR, wxT( ">>" ) );
	wxButton* l_pButton2=new wxButton( this, SP_ID_BUTTON_TOPRL, wxT( "<<" ) );
    l_pcRowSizerleft->Add( m_pStatic1, 0,  wxALL, 10);
	l_pcRowSizerleft->Add(l_pButton1, 0,   wxALL, 5);
	l_pcRowSizerleft->Add(l_pButton2, 0,  wxALL, 5);


	wxBoxSizer* l_pcRowSizerforrever = new wxBoxSizer( wxVERTICAL );
	wxStaticText* m_pStaticTextOut=new wxStaticText(this,-1,wxT("Selected colored places"),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	m_pListBoxOut = new wxListBox(this,SP_ID_LISTBOX_OUT,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_EXTENDED);
	l_pcRowSizerforrever->Add(m_pStaticTextOut, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizerforrever->Add(m_pListBoxOut, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer->Add(l_pcRowSizerforEnzym, 1, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(l_pcRowSizerleft, 1, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(l_pcRowSizerforrever, 1, wxEXPAND | wxALL, 5);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);

	
	m_pcBoxSizer->Add(l_pcRowSizer, 1, wxEXPAND );
	m_pcBoxSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND );

	SetAutoLayout( TRUE);
	SetSizer(m_pcBoxSizer);

	m_pcBoxSizer->Fit( this);
	m_pcBoxSizer->SetSizeHints( this);

	Layout();	

}

void SP_DLG_ColColoredPlaceSelection::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow())
	{
		m_ArrayString.clear();
		m_ArrayString = m_pListBoxOut->GetStrings();

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

void SP_DLG_ColColoredPlaceSelection::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void
SP_DLG_ColColoredPlaceSelection::OnLeftToRight(wxCommandEvent& p_cEvent )
{

	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	int l_number=m_pListBoxIn->GetSelections(l_Selections);
	if(l_number>0)
	{
		for(int Counter = 0; Counter <l_number; Counter++ )
		{
		    int l_nNum = l_Selections.Item(Counter);

			l_SelectionItem=m_pListBoxIn->GetString(l_nNum);
			m_pListBoxOut->Insert(l_SelectionItem, m_pListBoxOut->GetCount());

		}		
	}

}

void
SP_DLG_ColColoredPlaceSelection::OnRightToLeft(wxCommandEvent& p_cEvent )
{


	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	int l_number=m_pListBoxOut->GetSelections(l_Selections);
	if(l_number>0)
	{
		for(int Counter = l_number-1; Counter >=0; Counter-- )
		{
		    int l_nNum = l_Selections.Item(Counter);
			m_pListBoxOut->Delete(l_nNum);
		}
	}

}

wxArrayString SP_DLG_ColColoredPlaceSelection::GetStrings()
{
	return m_ArrayString;
}
