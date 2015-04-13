// SP_DLG_IAConfigure.cpp: Implementierung der Klasse SP_DLG_IAConfigure.
//
//////////////////////////////////////////////////////////////////////

#include "SP_DLG_IAConfigure.h"

#include "snoopy.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/dialogs/SP_DLG_IACommandConfigure.h"
#include "sp_gui/interaction/SP_IA_XmlWriter.h"
#include "sp_gui/interaction/SP_IA_XmlReader.h"

#include <wx/statline.h>
#include <wx/filename.h>

enum { //control id's
	SP_ID_LISTBOX_OPENNETS,
	SP_ID_LISTBOX_CONNECTEDNETS,
	SP_ID_BUTTON_CONNECT,
	SP_ID_BUTTON_DISCONNECT,
	SP_ID_LISTBOX_REGCMDS,
	SP_ID_LISTBOX_ACTCMDS,
	SP_ID_BUTTON_CMDCONFIG,
	SP_ID_BUTTON_ACTIVATE,
	SP_ID_BUTTON_DEACTIVATE,
	SP_ID_BUTTON_LOAD,
	SP_ID_BUTTON_SAVE
};


IMPLEMENT_CLASS(SP_DLG_IAConfigure, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_IAConfigure, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_IAConfigure::OnDlgClose)
EVT_BUTTON(SP_ID_BUTTON_CONNECT, SP_DLG_IAConfigure::OnConnect)
EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_OPENNETS, SP_DLG_IAConfigure::OnConnect)
EVT_BUTTON(SP_ID_BUTTON_DISCONNECT, SP_DLG_IAConfigure::OnDisconnect)
EVT_LISTBOX(SP_ID_LISTBOX_CONNECTEDNETS, SP_DLG_IAConfigure::OnSelectTarget)
EVT_BUTTON(SP_ID_BUTTON_ACTIVATE, SP_DLG_IAConfigure::OnActivate)
EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_REGCMDS, SP_DLG_IAConfigure::OnActivate)
EVT_LISTBOX(SP_ID_LISTBOX_REGCMDS, SP_DLG_IAConfigure::ShowDescription)
EVT_BUTTON(SP_ID_BUTTON_DEACTIVATE, SP_DLG_IAConfigure::OnDeactivate)
EVT_BUTTON(SP_ID_BUTTON_CMDCONFIG, SP_DLG_IAConfigure::OnCommandConfig)
EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_ACTCMDS, SP_DLG_IAConfigure::OnCommandConfig)
EVT_LISTBOX(SP_ID_LISTBOX_ACTCMDS, SP_DLG_IAConfigure::OnSelectActiveCommand)
EVT_BUTTON(SP_ID_BUTTON_LOAD, SP_DLG_IAConfigure::OnLoadConfig)
EVT_BUTTON(SP_ID_BUTTON_SAVE, SP_DLG_IAConfigure::OnSaveConfig)

END_EVENT_TABLE()


SP_DLG_IAConfigure::SP_DLG_IAConfigure(wxWindow *p_pcParent, SP_IA_Manager *p_pcMgr)
  : wxDialog(p_pcParent, -1, wxT("Configure Interaction"), wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
		m_pcManager = p_pcMgr;

    //dialog's main sizer and sizer for top section
    m_pcMainSizer = new wxBoxSizer(wxVERTICAL);

		wxBoxSizer *l_pcHelpSizer, *l_pcHelpSizer2;
		l_pcHelpSizer = new wxBoxSizer(wxHORIZONTAL);
		m_lbOpenNets = new wxListBox(this, SP_ID_LISTBOX_OPENNETS, wxDefaultPosition, wxSize(-1, 100),
																	0, 0, wxLB_HSCROLL | wxLB_NEEDED_SB);
		m_lbOpenNets->Set(GetOpenNets());
		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		l_pcHelpSizer2->Add(new wxStaticText(this, -1, wxT("Currently open nets:")), 0, wxLEFT | wxTOP, 10);
		l_pcHelpSizer2->Add(m_lbOpenNets, 0, wxEXPAND | wxALL, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 1, wxEXPAND);

		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		l_pcHelpSizer2->Add(new wxButton(this, SP_ID_BUTTON_CONNECT, wxT("Connect")), 0, wxLEFT | wxRIGHT | wxTOP, 10);
		l_pcHelpSizer2->Add(new wxButton(this, SP_ID_BUTTON_DISCONNECT, wxT("Disconnect")), 0, wxALL, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 0, wxALIGN_BOTTOM);

		m_lbConnectedNets = new wxListBox(this, SP_ID_LISTBOX_CONNECTEDNETS, wxDefaultPosition, wxSize(-1, 100),
																	0, 0, wxLB_HSCROLL | wxLB_NEEDED_SB);
		m_lbConnectedNets->Set(m_pcManager->GetConnectedNets(GetSourceName()));
		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		l_pcHelpSizer2->Add(new wxStaticText(this, -1, wxT("Connected target nets:")), 0, wxLEFT | wxTOP, 10);
		l_pcHelpSizer2->Add(m_lbConnectedNets, 0, wxEXPAND | wxALL, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 1, wxEXPAND);

		m_pcMainSizer->Add(l_pcHelpSizer, 0, wxEXPAND);
		m_pcMainSizer->Add(new wxStaticLine(this, -1), 0, wxEXPAND | wxALL, 10);

		l_pcHelpSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcHelpSizer->Add(new wxStaticText(this, -1, wxT("Source Net: ")), 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_tcSourceName = new wxTextCtrl(this, -1, GetSourceName(), wxDefaultPosition, wxDefaultSize,	wxTE_READONLY);
		l_pcHelpSizer->Add(m_tcSourceName, 1, wxLEFT | wxRIGHT | wxEXPAND, 10);
		m_pcMainSizer->Add(l_pcHelpSizer, 0, wxTOP | wxEXPAND, 10);

		l_pcHelpSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcHelpSizer->Add(new wxStaticText(this, -1, wxT("Selected Target Net: ")), 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_tcTargetName = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize,	wxTE_READONLY);
		l_pcHelpSizer->Add(m_tcTargetName, 1, wxLEFT | wxRIGHT | wxEXPAND, 10);
		m_pcMainSizer->Add(l_pcHelpSizer, 0, wxTOP | wxEXPAND, 10);

		l_pcHelpSizer = new wxBoxSizer(wxHORIZONTAL);
		m_lbRegCommands = new wxListBox(this, SP_ID_LISTBOX_REGCMDS, wxDefaultPosition, wxSize(-1, 100),
																	0, 0, wxLB_HSCROLL | wxLB_NEEDED_SB);
		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		l_pcHelpSizer2->Add(new wxStaticText(this, -1, wxT("Registered Commands:")), 0, wxLEFT | wxTOP, 10);
		l_pcHelpSizer2->Add(m_lbRegCommands, 0, wxEXPAND | wxALL, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 1, wxEXPAND);

		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		m_buCmdConfig = new wxButton(this, SP_ID_BUTTON_CMDCONFIG, wxT("Configure..."));
		m_buCmdConfig->Enable(false);
		l_pcHelpSizer2->Add(m_buCmdConfig, 0, wxALL | wxALIGN_TOP, 10);
		l_pcHelpSizer2->Add(new wxButton(this, SP_ID_BUTTON_ACTIVATE, wxT("Activate")), 0, wxLEFT | wxRIGHT, 10);
		l_pcHelpSizer2->Add(new wxButton(this, SP_ID_BUTTON_DEACTIVATE, wxT("Deactivate")), 0, wxALL | wxALIGN_BOTTOM, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 0, wxALIGN_BOTTOM);

		m_lbActCommands = new wxListBox(this, SP_ID_LISTBOX_ACTCMDS, wxDefaultPosition, wxSize(-1, 100),
																	0, 0, wxLB_HSCROLL | wxLB_NEEDED_SB);
		l_pcHelpSizer2 = new wxBoxSizer(wxVERTICAL);
		l_pcHelpSizer2->Add(new wxStaticText(this, -1, wxT("Activated Commands:")), 0, wxLEFT | wxTOP, 10);
		l_pcHelpSizer2->Add(m_lbActCommands, 0, wxEXPAND | wxALL, 10);
		l_pcHelpSizer->Add(l_pcHelpSizer2, 1, wxEXPAND);
		m_pcMainSizer->Add(l_pcHelpSizer, 0, wxEXPAND | wxTOP, 10);

		m_pcMainSizer->Add(new wxStaticText(this, -1, wxT("Command Description:")), 0, wxLEFT, 10);
		m_tcCmdDescription = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize,	wxTE_MULTILINE | wxTE_READONLY);
		m_pcMainSizer->Add(m_tcCmdDescription, 1, wxALL | wxEXPAND, 10);
		m_pcMainSizer->Add(new wxStaticLine(this, -1), 0, wxEXPAND | wxALL, 10);

		l_pcHelpSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcHelpSizer->Add(new wxButton(this, SP_ID_BUTTON_LOAD, wxT("Load...")), 0, wxALL, 10);
		l_pcHelpSizer->Add(new wxButton(this, SP_ID_BUTTON_SAVE, wxT("Save...")), 0, wxALL, 10);
		l_pcHelpSizer->Add(new wxButton(this, wxID_OK, wxT("Close")), 0, wxALL | wxALIGN_RIGHT, 10);
		m_pcMainSizer->Add(l_pcHelpSizer, 0, wxBOTTOM | wxALIGN_RIGHT, 15);

	//seems to be a reasonable dimension
	SetSizeHints(700, 650);
	SetSizerAndFit(m_pcMainSizer);

	Centre(wxBOTH);

}


wxString
SP_DLG_IAConfigure::GetSourceName() {
	wxDocument *l_pcCD = wxGetApp().GetDocmanager()->GetCurrentDocument();
	wxString l_sName = wxT("");
	if (l_pcCD) l_sName = l_pcCD->GetFilename();

	return l_sName;
}


wxArrayString
SP_DLG_IAConfigure::GetOpenNets() {
	wxArrayString l_asOpenNets;
	l_asOpenNets.Clear();
	wxNode *l_pcNode = wxGetApp().GetDocmanager()->GetDocuments().GetFirst();
	SP_MDI_Doc *l_pcDoc;
	wxString l_sFilename;

	while (l_pcNode) {
		l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcNode->GetData());
		l_sFilename = l_pcDoc->GetFilename();
		//using the opportunity to fill the lookup map
		m_mFilename2Netclass[l_sFilename] = l_pcDoc->GetNetclassName();

		if (l_asOpenNets.Index(l_sFilename, true, true) == wxNOT_FOUND) {
				//&& l_sFilename.Cmp(GetSourceName()) != 0) {
			l_asOpenNets.Add(l_sFilename);
		}
		l_pcNode = l_pcNode->GetNext();
	}
	return l_asOpenNets;
}

void
SP_DLG_IAConfigure::OnDlgClose(wxCommandEvent& p_cEvent) {

	if (IsModal())
		EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(FALSE);
	}
	SP_MDI_Doc *l_pcCD = dynamic_cast<SP_MDI_Doc*>(wxGetApp().GetDocmanager()->GetCurrentDocument());
	if (l_pcCD) {
		dynamic_cast<SP_GUI_Canvas*>(l_pcCD->GetDiagram()->GetCanvas())->GetView()->UpdateIASubMenu();
	}

	m_mBlockedCmds.clear();
}


void
SP_DLG_IAConfigure::OnLoadConfig(wxCommandEvent& p_cEvent) {

	if (m_tcTargetName->GetValue().IsNull()) {
		SP_MESSAGEBOX(wxT("Please specify a target net."), wxT("Missing target..."), wxOK | wxICON_EXCLAMATION);
		return;
	}

	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("IA-Config Files (*.iac)|*.iac"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxString p_sFilename = l_cDlg.GetFilename();
		wxString l_sSource = m_tcSourceName->GetValue();
		wxString l_sTarget = m_tcTargetName->GetValue();
		SP_IA_XMLReader r;
		if (r.Read(p_sFilename)) {
			//true, if net_1 of the config-file corresponds to source
			bool source = true;
			wxString netclass1 = r.GetNet(source).first;
			wxString netclass2 = r.GetNet(!source).first;
			bool l_bNoErrors = true;
			if ((m_mFilename2Netclass[l_sSource].Cmp(netclass2) == 0)
				&& (m_mFilename2Netclass[l_sTarget].Cmp(netclass1) == 0)) {
				//change source and target
				wxString tmp = netclass1;
				netclass1 = netclass2;
				netclass2 = tmp;
				source = false;
			}
			if ((m_mFilename2Netclass[l_sSource].Cmp(netclass1) == 0)
				&& (m_mFilename2Netclass[l_sTarget].Cmp(netclass2) == 0)) {
				//setting up configuration
				//first setting up backward-direction
				wxArrayString cmds = r.GetCommandNames(!source);
				m_pcManager->DeactivateCommand(l_sTarget, l_sSource);
				if (!cmds.IsEmpty()) m_pcManager->ConnectNet(l_sTarget, l_sSource);
				for (unsigned int i = 0; i < cmds.GetCount(); i++) {
					l_bNoErrors &= m_pcManager->ActivateCommand(l_sTarget, l_sSource, cmds[i]);
				}
				//now setting up forward-direction
				cmds = r.GetCommandNames(source);
				m_pcManager->DeactivateCommand(l_sSource,l_sTarget);
				m_lbRegCommands->Set(m_pcManager->GetRegisteredCommandNames(
															m_mFilename2Netclass[l_sSource], m_mFilename2Netclass[l_sTarget]));
				m_lbActCommands->Clear();
				for (unsigned int i = 0; i < cmds.GetCount(); i++) {
					if (m_pcManager->ActivateCommand(l_sSource, l_sTarget, cmds[i])) {
						m_lbActCommands->Append(cmds[i]);
						BlockCommands(m_pcManager->GetRegisteredCommand(m_mFilename2Netclass[l_sSource],
																														m_mFilename2Netclass[l_sTarget],
																														cmds[i])->GetTriggerEvent());
					} else {
						l_bNoErrors = false;
					}
				}
			} else {
				SP_MESSAGEBOX(wxT("Couldn't set up configuration.\n(Netclasses don't match.)"), wxT("Error"), wxICON_ERROR);
			}
			if (!l_bNoErrors) {SP_LOGDEBUG(wxT("Warning: One or more commands couldn't be activated."));}
		}
	}
	l_cDlg.Destroy();

}


void
SP_DLG_IAConfigure::OnSaveConfig(wxCommandEvent& p_cEvent) {
	wxFileDialog l_cDlg(this, wxT(""), wxT(""), wxT(""), wxT("IA-Config Files (*.iac)|*.iac"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);
	if (l_cDlg.ShowModal() == wxID_OK) {
		wxFileName l_sFilename;
		l_sFilename.SetPath(l_cDlg.GetDirectory());
		l_sFilename.SetName(l_cDlg.GetFilename());
		if (l_sFilename.GetExt() != wxT("iac"))
		{
			l_sFilename.SetExt(wxT("iac"));
		}
		SP_IA_XMLWriter w;
		w.Write(SP_PairString(m_tcSourceName->GetValue(), m_tcTargetName->GetValue()), l_sFilename.GetFullPath());
	}
	l_cDlg.Destroy();
}


void
SP_DLG_IAConfigure::OnConnect(wxCommandEvent& p_cEvent) {
	wxString toConnect = m_lbOpenNets->GetStringSelection();

	if (m_lbOpenNets->GetSelection() != wxNOT_FOUND) {
		if (m_pcManager->ConnectNet(GetSourceName(), toConnect)) {
			m_lbConnectedNets->Append(toConnect);
		}
	}
}


void
SP_DLG_IAConfigure::OnDisconnect(wxCommandEvent& p_cEvent) {

	if (m_lbConnectedNets->GetSelection() != wxNOT_FOUND) {
		wxString toDisconnect = m_lbConnectedNets->GetStringSelection();

		if (m_pcManager->DisconnectNet(GetSourceName(), toDisconnect)) {
			m_lbConnectedNets->Delete(m_lbConnectedNets->GetSelection());
			if (toDisconnect.Cmp(m_tcTargetName->GetValue()) == 0) {
				m_tcTargetName->Clear();
				m_lbRegCommands->Clear();
				m_lbActCommands->Clear();
				m_tcCmdDescription->Clear();
				m_buCmdConfig->Enable(false);
			}
		}
	}
}


void
SP_DLG_IAConfigure::OnSelectTarget(wxCommandEvent& p_cEvent) {
	wxString l_sCmdName;
	wxString l_sSource = m_tcSourceName->GetValue();
	wxString l_sTarget = m_lbConnectedNets->GetStringSelection();

	m_tcTargetName->SetValue(l_sTarget);
	m_lbRegCommands->Set(m_pcManager->GetRegisteredCommandNames(
					m_mFilename2Netclass[l_sSource], m_mFilename2Netclass[l_sTarget]));
	m_lbActCommands->Set(m_pcManager->GetActiveCommandNames(l_sSource, l_sTarget));

	for (unsigned int i = 0; i < m_lbActCommands->GetCount(); i++) {
		BlockCommands(m_pcManager->GetRegisteredCommand(m_mFilename2Netclass[l_sSource],
																										m_mFilename2Netclass[l_sTarget],
																										m_lbActCommands->GetString(i))->GetTriggerEvent());
	}
}


void
SP_DLG_IAConfigure::OnActivate(wxCommandEvent& p_cEvent) {
	wxString l_sCmdName = m_lbRegCommands->GetStringSelection();
	wxString l_sSource = m_tcSourceName->GetValue();
	wxString l_sTarget = m_tcTargetName->GetValue();

	if (m_lbRegCommands->GetSelection() != wxNOT_FOUND) {
		if (m_pcManager->ActivateCommand(l_sSource, l_sTarget, l_sCmdName)) {
			m_lbActCommands->Append(l_sCmdName);
			//block all other commands with the same event
			BlockCommands(m_pcManager->GetRegisteredCommand(m_mFilename2Netclass[l_sSource],
																											m_mFilename2Netclass[l_sTarget],
																											l_sCmdName)->GetTriggerEvent());
		}
	}
}


void
SP_DLG_IAConfigure::OnDeactivate(wxCommandEvent& p_cEvent) {
	int selection = m_lbActCommands->GetSelection();
	wxString l_sCmdName = m_lbActCommands->GetStringSelection();
	wxString l_sSource = m_tcSourceName->GetValue();
	wxString l_sTarget = m_tcTargetName->GetValue();

	if (selection != wxNOT_FOUND) {
		if (m_pcManager->DeactivateCommand(l_sSource, l_sTarget, l_sCmdName)) {
			m_lbActCommands->Delete(selection);
			m_buCmdConfig->Enable(false);
			UnblockCommands(m_pcManager->GetRegisteredCommand(m_mFilename2Netclass[l_sSource],
																												m_mFilename2Netclass[l_sTarget],
																												l_sCmdName)->GetTriggerEvent());
		}
	}
}


void
SP_DLG_IAConfigure::OnSelectActiveCommand(wxCommandEvent& p_cEvent) {
	SP_IA_Command *l_pcCmd = m_pcManager->GetActiveCommand(m_tcSourceName->GetValue(),
												m_tcTargetName->GetValue(), m_lbActCommands->GetStringSelection());

	if (!l_pcCmd) return;

	m_tcCmdDescription->SetValue(CreateDescription(l_pcCmd));
	m_buCmdConfig->Enable(l_pcCmd->HasConfig());
}


void
SP_DLG_IAConfigure::OnCommandConfig(wxCommandEvent& p_cEvent) {
	SP_IA_Command *l_pcCmd = m_pcManager->GetActiveCommand(m_tcSourceName->GetValue(),
												m_tcTargetName->GetValue(), m_lbActCommands->GetStringSelection());
	if (!l_pcCmd) return;
	if (l_pcCmd->HasConfig()) {
		SP_DLG_IACommandConfigure l_cDialog(this, l_pcCmd);
		l_cDialog.ShowModal();
		l_cDialog.Destroy();
	}
}


void
SP_DLG_IAConfigure::ShowDescription(wxCommandEvent& p_cEvent) {
	SP_IA_Command *l_pcCmd = m_pcManager->GetRegisteredCommand(m_mFilename2Netclass[m_tcSourceName->GetValue()],
										m_mFilename2Netclass[m_tcTargetName->GetValue()], m_lbRegCommands->GetStringSelection());
	if (l_pcCmd) {
		m_tcCmdDescription->SetValue(CreateDescription(l_pcCmd));
	}
}


wxString
SP_DLG_IAConfigure::CreateDescription(SP_IA_Command *p_pcCmd) {

	if (!p_pcCmd) return wxString();

	wxString l_sSourceString = wxString();
	SP_ListData *l_pcSE = p_pcCmd->GetTriggerEvent()->GetSourceElements();
	for (SP_ListData::iterator it = l_pcSE->begin(); it != l_pcSE->end(); ++it) {
		if (!l_sSourceString.empty())	l_sSourceString += wxT(", ");
		l_sSourceString += (*it)->GetClassName();
	}
	wxString l_sDescription = wxString(wxT("COMMAND\nType: ") + p_pcCmd->GetCommandTypeString()
																			+ wxT("\n\nTRIGGER\nType: ") + p_pcCmd->GetTriggerEvent()->GetTriggerTypeString()
																			+ wxT("\nElement(s): ") + l_sSourceString
																			+ wxT("\n\nACTION ON TARGET:\n") + p_pcCmd->GetDescription());

	return l_sDescription;
}


SP_PairString*
SP_DLG_IAConfigure::GetSource() {
	SP_PairString *source = new SP_PairString();

	if (m_tcSourceName) {
		source->first = m_mFilename2Netclass[m_tcSourceName->GetValue()];
		source->second = m_tcSourceName->GetValue();
	}

	return source;
}


SP_PairString*
SP_DLG_IAConfigure::GetTarget() {
	SP_PairString *target = new SP_PairString();

	if (m_tcTargetName) {
		target->first = m_mFilename2Netclass[m_tcTargetName->GetValue()];
		target->second = m_tcTargetName->GetValue();
	}

	return target;
}


wxArrayString
SP_DLG_IAConfigure::GetActiveCmds() {
	wxArrayString cmds;
	cmds.Clear();

	for (unsigned int i = 0; i <	m_lbActCommands->GetCount(); i++) {
		cmds.Add(m_lbActCommands->GetString(i));
	}

	return cmds;
}


void
SP_DLG_IAConfigure::BlockCommands(SP_IA_Event *p_pcEvent) {
	wxString l_sCmdName;
	wxString l_sSource = m_mFilename2Netclass[m_tcSourceName->GetValue()];
	wxString l_sTarget = m_mFilename2Netclass[m_tcTargetName->GetValue()];
	wxArrayString l_asBlockedCmds;
	SP_IA_Command *l_pcCmd;

	//collecting all names of commands with the same event
	if (m_mBlockedCmds.find(p_pcEvent) == m_mBlockedCmds.end()) {
		for (unsigned int i = 0; i < m_lbRegCommands->GetCount(); i++) {
			l_sCmdName = m_lbRegCommands->GetString(i);
			l_pcCmd = m_pcManager->GetRegisteredCommand(l_sSource, l_sTarget, l_sCmdName);
			if (p_pcEvent->Equals(l_pcCmd->GetTriggerEvent(), false)) {
				l_asBlockedCmds.Add(l_sCmdName);
			}
		}
		//saving blocked commands for faster unblocking
		m_mBlockedCmds[p_pcEvent] = l_asBlockedCmds;
	} else {
		l_asBlockedCmds = m_mBlockedCmds[p_pcEvent];
	}
	//deleting all collected names from the list
	for (unsigned int i = 0; i < l_asBlockedCmds.GetCount(); i++) {
		m_lbRegCommands->Delete(m_lbRegCommands->FindString(l_asBlockedCmds[i]));
	}
}


void
SP_DLG_IAConfigure::UnblockCommands(SP_IA_Event *p_pcEvent) {

	if (m_mBlockedCmds.find(p_pcEvent) != m_mBlockedCmds.end()) {
		m_lbRegCommands->Append(m_mBlockedCmds[p_pcEvent]);
	}
}
