//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: dialog to choose a master net
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ChooseMasterNet.h"
#include <wx/spinctrl.h>


IMPLEMENT_CLASS(SP_DLG_ChooseMasterNet, wxDialog)

enum
{		
	SP_ID_LISTCTRL_ELEMENT,
	SP_ID_RADIO,
	SP_ID_COLORSETTXTCTRL,
	SP_ID_VARIABLETXTCTRL
};

BEGIN_EVENT_TABLE(SP_DLG_ChooseMasterNet, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_ChooseMasterNet::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_ChooseMasterNet::OnDlgClose)
END_EVENT_TABLE()

SP_DLG_ChooseMasterNet::SP_DLG_ChooseMasterNet(wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER)
{

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* l_pcRowSizer = new wxBoxSizer( wxVERTICAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("File specifying subnets:") ), 0, wxEXPAND | wxALL, 5);

	m_pcFilePickerCtrl = new wxFilePickerCtrl( this, -1, wxT(""), wxT("Browse"), wxT("Text files(*.inv)|*.inv"), wxDefaultPosition,
			wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);
	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxEXPAND | wxALL, 5);

	topSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a color set Name:") ), 0, wxEXPAND | wxALL, 5);
	m_pcColorSetTextCtrl = new wxTextCtrl(this, SP_ID_COLORSETTXTCTRL, wxT("CS"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcColorSetTextCtrl, 1, wxEXPAND | wxALL, 5);

	topSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a variable name:") ), 0, wxEXPAND | wxALL, 5);
	m_pcVariableTextCtrl = new wxTextCtrl(this, SP_ID_VARIABLETXTCTRL, wxT("x"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcVariableTextCtrl, 1, wxEXPAND | wxALL, 5);

	topSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

	topSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);
	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_ChooseMasterNet::~SP_DLG_ChooseMasterNet()
{

}

void SP_DLG_ChooseMasterNet::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		m_sFilePath = m_pcFilePickerCtrl->GetPath();
		m_sColorSetName = m_pcColorSetTextCtrl->GetValue();
		m_sVariableName = m_pcVariableTextCtrl->GetValue();

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



void SP_DLG_ChooseMasterNet::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}


