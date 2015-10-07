// SP_DLG_IACommandConfigure.cpp: Implementierung der Klasse SP_DLG_IACommandConfigure.
//
//////////////////////////////////////////////////////////////////////

#include "SP_DLG_IACommandConfigure.h"
#include "sp_gui/interaction/commands/SP_IA_Command_Colorize.h"
#include "sp_gui/interaction/commands/SP_IA_Command_Insert.h"

#include <wx/statline.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(SP_DLG_IACommandConfigure, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_IACommandConfigure, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_IACommandConfigure::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_IACommandConfigure::OnDlgCancel)

//work around: 
//just couldn't get event handling by SP_IA_Command_Colorize itself to work
//problem: couldn't get access to the button bitmap via wxMemoryDC
EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOR, SP_DLG_IACommandConfigure::OnChooseColor)
//similar thing
//when event is handled command object is gone somehow!? maybe same problem above?
EVT_BUTTON(SP_ID_BUTTON_SETNODE, SP_DLG_IACommandConfigure::OnSetCurrentNode)

END_EVENT_TABLE()


SP_DLG_IACommandConfigure::SP_DLG_IACommandConfigure(wxWindow *p_pcParent, SP_IA_Command *p_pcCmd)
: wxDialog(p_pcParent, -1, wxT("Command Configuration"), wxDefaultPosition, wxDefaultSize, 
		   wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
m_pcCommand(p_pcCmd)
{
	
	m_pcSizer = new wxBoxSizer(wxVERTICAL);
	
	if (m_pcCommand) m_pcCommand->AddToConfigDialog(this, m_pcSizer);
	
	m_pcSizer->Add(new wxStaticLine(this, -1), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
	wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 10);
	
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);
	
	SetSizerAndFit(m_pcSizer);
	
	Centre(wxBOTH);
	
}


void
SP_DLG_IACommandConfigure::OnDlgOk(wxCommandEvent& p_cEvent) {
	
	if (m_pcCommand) m_pcCommand->OnConfigDialogOk();
	
	while (GetEventHandler() != this) {
		PopEventHandler();
	}
	
	if (IsModal())
		EndModal(wxID_OK);
	else {
		SetReturnCode(wxID_OK);
		this->Show(FALSE);
	}
	
}


void
SP_DLG_IACommandConfigure::OnDlgCancel(wxCommandEvent& p_cEvent) {
	
	while (GetEventHandler() != this) {
		PopEventHandler();
	}
	
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
SP_DLG_IACommandConfigure::OnChooseColor(wxColourPickerEvent& p_cEvent) {
	
	dynamic_cast<SP_IA_Command_Colorize*>(m_pcCommand)->OnChooseColor(p_cEvent);
}


void
SP_DLG_IACommandConfigure::OnSetCurrentNode(wxCommandEvent& p_cEvent) {
	
	dynamic_cast<SP_IA_Command_Insert*>(m_pcCommand)->OnSetCurrentNode(p_cEvent);
}
