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

	m_cbShowReactionReverseReaction = new wxCheckBox(this, -1, wxT("Show reverse reactions"));
	m_pcSizer->Add(m_cbShowReactionReverseReaction, 0, wxALL, 10);

	/* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcButtonSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

   SetSizerAndFit(m_pcSizer);

    Centre(wxBOTH);
}

bool
SP_DLG_ImportSBML2extPN::GetShowReverseReactions()
{
	return( m_cbShowReactionReverseReaction->IsChecked() );
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


