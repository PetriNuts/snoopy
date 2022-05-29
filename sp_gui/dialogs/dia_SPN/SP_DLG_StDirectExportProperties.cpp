//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/09/26 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"

#include <wx/filedlg.h>
#include <wx/filename.h>

IMPLEMENT_CLASS( SP_DLG_StDirectExportProperties, wxDialog )

BEGIN_EVENT_TABLE( SP_DLG_StDirectExportProperties, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_StDirectExportProperties :: OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_StDirectExportProperties :: OnDlgCancel )

END_EVENT_TABLE()

SP_DLG_StDirectExportProperties::SP_DLG_StDirectExportProperties(
		SP_DS_StExportCSVEditDlgType p_eType, wxDialog* p_pcParent,
		wxString* p_psFilename, int* p_pnSpacer, bool* p_pbCompressExact, bool* p_pbAlltraces,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
	m_eDlgType = p_eType;

	m_psFilename = p_psFilename;
	m_pnSpacer = p_pnSpacer;
	m_pbCompressExact = p_pbCompressExact;
	m_pbAlltraces = p_pbAlltraces;
	if (m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT)
	{

		SetTitle(wxT("Direct export properties"));

	}
	else if (m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT)
	{

		SetTitle(wxT("Export properties"));

	}

	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);

	 m_pcPropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( this, -1,
											wxT("Export properties for main table") ), wxVERTICAL );

	m_pcMainSizer->Add(m_pcPropertiesSizer, 0, wxALL | wxEXPAND, 5);

	// rows of SetsSizer

	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Filename:") ), 0, wxALL | wxEXPAND, 5);

	m_pcFilePickerCtrl = new wxFilePickerCtrl( this, -1, *m_psFilename, wxT("Browse"), wxT("CSV files (*.csv)|*.csv"), wxDefaultPosition,
			wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);

	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxALL | wxEXPAND, 5);

	m_pcPropertiesSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Spacer:") ), 1, wxALL | wxEXPAND, 5);

	wxString choices[4] = {wxT("Colon"), wxT("Comma"), wxT("SemiColon"), wxT("Tabulator")};
	m_pcSpacerChoice = new wxChoice( this, -1 , wxDefaultPosition, wxDefaultSize, 4, choices);

	l_pcRowSizer->Add(m_pcSpacerChoice, 1, wxALL | wxEXPAND, 5);

	m_pcPropertiesSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND);

	//if (m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT)
	///{
		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

		m_pcCompressCheckBox = new wxCheckBox( this, -1, wxT("Compress Output") );


		l_pcRowSizer->Add( m_pcCompressCheckBox, 1, wxALL | wxEXPAND, 5 );

		wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();///Added by G.A
		bool isFuzzyNet = netClass.Contains(wxT("Fuzzy")) ? true : false; 

		if (isFuzzyNet) {
			m_pcAllTracesesForFuzzySimulation = new wxCheckBox(this, -1, wxT("Export Entire Band"));
			l_pcRowSizer->Add(m_pcAllTracesesForFuzzySimulation, 1, wxALL | wxEXPAND, 5);
		}

		m_pcPropertiesSizer->Add( l_pcRowSizer, 1, wxALL | wxEXPAND );

	///}

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxButton* l_pcButton = new wxButton( this, wxID_OK, wxT("Ok") );

	if( m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT )
	{

		l_pcButton->SetLabel( wxT("Export") );

	}

	l_pcRowSizer->Add( l_pcButton, 1, wxLEFT | wxRIGHT | wxTOP, 5 );
	l_pcRowSizer->Add( new wxButton( this, wxID_CANCEL, wxT("Cancel") ), 1, wxLEFT | wxRIGHT | wxTOP, 5 );

	m_pcMainSizer->Add( l_pcRowSizer, 0, wxALL, 5 );

	LoadData();

	SetSizerAndFit( m_pcMainSizer );

}

void SP_DLG_StDirectExportProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		if (IsModal() )
		{
			if (SaveData() )
			{
				EndModal(wxID_OK);
			}
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}
}

void SP_DLG_StDirectExportProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_StDirectExportProperties::LoadData()
{

	m_pcFilePickerCtrl->SetPath( *m_psFilename);
	m_pcSpacerChoice->SetSelection( *m_pnSpacer);
	if (m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT)
		m_pcCompressCheckBox->SetValue(*m_pbCompressExact);

}

bool SP_DLG_StDirectExportProperties::SaveData()
{

	wxString l_sOutName = m_pcFilePickerCtrl->GetPath();

	if (wxFileName::FileExists(l_sOutName) )
	{
		int l_nAnswer = SP_MESSAGEBOX(wxT("File ") + l_sOutName
									+ wxT(" exists. Overwrite?"),
									wxT("Overwrite?"),
									wxYES_NO | wxICON_QUESTION);
		if (l_nAnswer == wxNO)
		{
			return false;
		}
	}

	*m_psFilename = l_sOutName;
	*m_pnSpacer = m_pcSpacerChoice->GetSelection();
	if (m_eDlgType == SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT)
		*m_pbCompressExact = m_pcCompressCheckBox->GetValue();

	wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();///Added by G.A
	bool isFuzzyNet = netClass.Contains(wxT("Fuzzy")) ? true : false;
	if (isFuzzyNet) {
		*m_pbAlltraces = m_pcAllTracesesForFuzzySimulation->GetValue();
	}

	return true;

}

