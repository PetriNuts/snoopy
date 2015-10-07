// SP_DLG_IA_RegCmdList.cpp: Implementierung der Klasse SP_DLG_IA_RegCmdList.
//
//////////////////////////////////////////////////////////////////////

#include "SP_DLG_IARegCmdList.h"
#include <wx/statline.h>

enum { //control id's
	SP_ID_LISTBOX_REGCMDS,
};

IMPLEMENT_CLASS(SP_DLG_IARegCmdList, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_IARegCmdList, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_IARegCmdList::OnDlgClose)
//EVT_LISTBOX(SP_ID_LISTBOX_REGCMDS, SP_DLG_IARegCmdList::ShowDescription)

END_EVENT_TABLE()


SP_DLG_IARegCmdList::SP_DLG_IARegCmdList(wxWindow *p_pcParent, SP_IA_Manager *p_pcMgr)
  : wxDialog(p_pcParent, -1, wxT("List of Registered Commands"), wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
		m_pcManager = p_pcMgr;

    //dialog's main sizer and sizer for top section
    m_pcMainSizer = new wxBoxSizer(wxVERTICAL);

		m_lbRegCommands = new wxListBox(this, SP_ID_LISTBOX_REGCMDS, wxDefaultPosition, wxSize(-1, 100),
																	0, 0, wxLB_HSCROLL | wxLB_NEEDED_SB);
		wxArrayString l_cCmds = m_pcManager->GetRegisteredCommandNames();
		l_cCmds.Sort();
		m_lbRegCommands->Set(l_cCmds);
		m_pcMainSizer->Add(new wxStaticText(this, -1, wxT("Registered Commands:")), 0, wxLEFT | wxTOP, 10);
		m_pcMainSizer->Add(m_lbRegCommands, 1, wxEXPAND | wxALL, 10);
/*
		m_pcMainSizer->Add(new wxStaticText(this, -1, wxT("Command Description:")), 0, wxLEFT, 10);
		m_tcCmdDescription = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize,	wxTE_MULTILINE | wxTE_READONLY);
		m_pcMainSizer->Add(m_tcCmdDescription, 1, wxALL | wxEXPAND, 10);
*/		m_pcMainSizer->Add(new wxStaticLine(this, -1), 0, wxEXPAND | wxALL, 10);

		/* Buttons in the lower right hand corner */
    m_pcMainSizer->Add(new wxButton(this, wxID_OK, wxT("Close")), 0, wxALL | wxALIGN_RIGHT, 10);

	//seems to be a reasonable dimension
	SetSizeHints(350, 500);
    SetSizerAndFit(m_pcMainSizer);
	Centre(wxBOTH);
}


void
SP_DLG_IARegCmdList::OnDlgClose(wxCommandEvent& p_cEvent) {

	if (IsModal())
		EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(FALSE);
	}
}


void
SP_DLG_IARegCmdList::ShowDescription(wxCommandEvent& p_cEvent) {
	SP_IA_Command *l_pcCmd = m_pcManager->GetRegisteredCommand(SP_DS_PN_CLASS,SP_DS_REACHABILITY_CLASS, m_lbRegCommands->GetStringSelection());
	if (l_pcCmd) {
		m_tcCmdDescription->SetValue(CreateDescription(l_pcCmd));
	}
}


wxString
SP_DLG_IARegCmdList::CreateDescription(SP_IA_Command *p_pcCmd) {

	if (!p_pcCmd) return wxString();

	wxString l_sSourceString = wxString();
	SP_ListData *l_pcSE = p_pcCmd->GetTriggerEvent()->GetSourceElements();
	for (SP_ListData::iterator it = l_pcSE->begin(); it != l_pcSE->end(); ++it) {
		if (!l_sSourceString.empty())	l_sSourceString += wxT(", ");
		l_sSourceString += (*it)->GetClassName();
	}
	wxString l_sDescription = wxString(wxT("TRIGGER\nType: ") + p_pcCmd->GetTriggerEvent()->GetTriggerTypeString()
																			+ wxT("\nElement(s): ") + l_sSourceString
																			+ wxT("\n\nACTION:\n") + p_pcCmd->GetDescription());

	return l_sDescription;
}
