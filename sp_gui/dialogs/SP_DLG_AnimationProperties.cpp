//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>

#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/extensions/SP_DS_Animation.h"

IMPLEMENT_CLASS(SP_DLG_AnimationProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_AnimationProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_AnimationProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_AnimationProperties::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_AnimationProperties::SP_DLG_AnimationProperties(SP_DS_Animation* p_pcAnim,
                                                       wxWindow* p_pcParent,
                                                       const wxString& p_sTitle,
                                                       long p_nStyle)
  : wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, 
	     p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
    m_pcError(NULL),
    m_pcAnimation(p_pcAnim)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);
    
    // animation properties come from the animation, of course
    m_pcAnimation->AddToDialog(this, m_pcSizer);

    m_pcError = new wxStaticText(this, -1, wxT("                                                  "));
    m_pcError->SetForegroundColour(*wxRED);
    m_pcSizer->Add(m_pcError, 0, wxALL | wxEXPAND, 5);

    /* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

    SetSizerAndFit(m_pcSizer);
    
    Centre(wxCENTER_ON_SCREEN);
}

//------------------------------------------------------------------------
void
SP_DLG_AnimationProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if (!m_pcAnimation->OnDialogOk())
        {
            if (m_pcError)
            {
                m_pcError->SetLabel(m_pcAnimation->GetLastError());
                m_pcError->Refresh();
            }
            return;
        }

        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
		    SetReturnCode(wxID_OK);
		    this->Show(FALSE);
        }
	}
}

void
SP_DLG_AnimationProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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

