//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: schroed $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/04/30 11:55:00 $
// Short Description: Dialog for SBML Import to extended Petrinets
//////////////////////////////////////////////////////////////////////

#include "SP_DLG_ImportSBML2extPN.h"
#include "sp_core/SP_Core.h"

IMPLEMENT_CLASS(SP_DLG_ImportSBML2extPN, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ImportSBML2extPN, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_ImportSBML2extPN::OnDlgOk)
	EVT_BUTTON(wxID_CANCEL, SP_DLG_ImportSBML2extPN::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_ImportSBML2extPN::SP_DLG_ImportSBML2extPN(SP_GUI_Mainframe*  p_pcParent,
										   const wxString& p_sTitle,
                                           long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
          p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

	m_cbHighlightReverseReaction = new wxCheckBox(this, -1, wxT("Highlight reversible reactions"));
	m_pcSizer->Add(m_cbHighlightReverseReaction, 0, wxALL, 10);
	m_cbCreateReactionReverseReaction = new wxCheckBox(this, -1, wxT("Create reverse reactions"));
	m_pcSizer->Add(m_cbCreateReactionReverseReaction, 0, wxALL, 10);

	/* Buttons in the lower right hand corner */
	wxStdDialogButtonSizer* l_pcButtons = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    m_pcSizer->Add(l_pcButtons, 0, wxALL, 10);

   SetSizerAndFit(m_pcSizer);

    Centre(wxBOTH);
}

bool
SP_DLG_ImportSBML2extPN::GetCreateReverseReactions()
{
	return( m_cbCreateReactionReverseReaction->IsChecked() );
}

bool
SP_DLG_ImportSBML2extPN::GetHighlightReverseReactions()
{
	return( m_cbHighlightReverseReaction->IsChecked() );
}

void
SP_DLG_ImportSBML2extPN::OnDlgOk(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}

void
SP_DLG_ImportSBML2extPN::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}


