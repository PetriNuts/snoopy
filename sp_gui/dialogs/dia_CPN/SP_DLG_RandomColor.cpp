//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: dialog to choose a master net
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/dia_CPN/SP_DLG_RandomColor.h"
#include <wx/spinctrl.h>


IMPLEMENT_CLASS(SP_DLG_RandomColor, wxDialog)

enum
{	
	SP_ID_COLORSETTXTCTRL,
	SP_ID_TYPECHOICE	
};

BEGIN_EVENT_TABLE(SP_DLG_RandomColor, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_RandomColor::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_RandomColor::OnDlgClose)
	EVT_TEXT(SP_ID_COLORSETTXTCTRL, SP_DLG_RandomColor::OnTextChanged)
END_EVENT_TABLE()

SP_DLG_RandomColor::SP_DLG_RandomColor(vector<wxString>* p_pvColors, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER)
{

	m_pvColors = p_pvColors;

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);	

	//Set color set and variable
	wxBoxSizer* l_pcColorSetSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Choose a Color:") ), 0, wxEXPAND | wxALL, 5);
	wxArrayString l_arDataTypes;
	for(unsigned i = 0; i < m_pvColors->size(); i++)
	{
		l_arDataTypes.Add( (*m_pvColors)[i]);
	}

	m_pcColorsComboBoxCtrl = new wxChoice(this, SP_ID_TYPECHOICE, wxDefaultPosition, wxDefaultSize, l_arDataTypes);
	m_pcColorsComboBoxCtrl->SetStringSelection((*m_pvColors)[0]);
	l_pcRowSizer->Add(m_pcColorsComboBoxCtrl, 1, wxEXPAND | wxALL, 5);	

	l_pcColorSetSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a number:") ), 0, wxEXPAND | wxALL, 5);
	m_pcNumberTextCtrl = new wxTextCtrl(this, SP_ID_COLORSETTXTCTRL, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcNumberTextCtrl, 1, wxEXPAND | wxALL, 5);

	l_pcColorSetSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	topSizer->Add(l_pcColorSetSizer, 0, wxALL | wxEXPAND, 1);	

	topSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_RandomColor::~SP_DLG_RandomColor()
{

}

void SP_DLG_RandomColor::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
	
		
		// 1 -- color set type
		m_sColors = m_pcColorsComboBoxCtrl->GetStringSelection();

		// 2 -- color set colors
		m_sNumber = m_pcNumberTextCtrl->GetValue();		

		long l_nNumber;
		if(!m_sNumber.ToLong(&l_nNumber) )
		{
			SP_MESSAGEBOX(wxT("Please input an integer."), wxT("Number input error"), wxOK | wxICON_ERROR);
			return;
		}


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



void SP_DLG_RandomColor::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}

void SP_DLG_RandomColor::OnTextChanged(wxCommandEvent& p_cEvent)
{
	/*
	m_sNumber = m_pcNumberTextCtrl->GetValue();		

	long l_nNumber;
	if(!m_sNumber.ToLong(&l_nNumber) )
	{
		SP_MESSAGEBOX(wxT("Please input an integer."), wxT("Number input error"), wxOK | wxICON_ERROR);
	}
	*/

	
}


