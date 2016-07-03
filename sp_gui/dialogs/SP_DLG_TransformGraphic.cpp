//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>

#include "sp_gui/dialogs/SP_DLG_TransformGraphic.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_View.h"

IMPLEMENT_CLASS(SP_DLG_TransformGraphic, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_TransformGraphic, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_TransformGraphic::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_TransformGraphic::OnDlgCancel)
EVT_RADIOBOX(SP_TRANSFORM_RADIO_ID, SP_DLG_TransformGraphic::OnRadiobox)
END_EVENT_TABLE()

SP_DLG_TransformGraphic::SP_DLG_TransformGraphic(SP_GUI_Mainframe* p_pcParent,
                                                 SP_MDI_View* p_pcView,
                                                 SP_ListGraphic* p_plGraphics,
                                                 wxRealPoint p_cPosition,
                                                 wxSize p_cSize,
                                                 const wxString& p_sTitle,
                                                 long p_nStyle)
  :wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
	    p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
   m_pcRadiusCtrl(NULL),
   m_nRadius(90),
   m_cPosition(p_cPosition),
   m_cSize(p_cSize),
   m_plGraphics(p_plGraphics),
   m_pcView(p_pcView)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcRadiusCtrl = new wxTextCtrl(this, -1, wxT("90"),
        wxDefaultPosition, wxDefaultSize, 0,
        wxGenericValidator(&m_nRadius));

    wxString l_asChoices[] = { wxT("Rotate"), wxT("Flip (vertical)"), wxT("Mirror (horizontal)") };
    m_pcChoice = new wxRadioBox(
        this,
        SP_TRANSFORM_RADIO_ID,
        wxT("Action"),
        wxDefaultPosition,
        wxDefaultSize,
        3,
        l_asChoices,
        1,
        wxRA_SPECIFY_COLS);

    m_pcSizer->Add(m_pcChoice, 0, wxEXPAND | wxALL, 5);
    l_pcSizer->Add(new wxStaticText(this, -1, wxT("Angle:")), 0, wxRIGHT | wxALIGN_CENTER, 5);
    l_pcSizer->Add(m_pcRadiusCtrl, 1);
    m_pcSizer->Add(l_pcSizer, 0, wxEXPAND | wxALL, 5);

    /* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

    SetSizerAndFit(m_pcSizer);

    Centre(wxBOTH);
}

void
SP_DLG_TransformGraphic::DoAction()
{
    if (!m_pcChoice || !m_pcView)
        return;

    switch (m_pcChoice->GetSelection())
    {
    // rotate
    case 0:
        m_pcView->DoRotate(m_plGraphics, m_cPosition.x, m_cPosition.y, m_nRadius);
        break;
    // flip (vertical)
    case 1:
        m_pcView->DoFlip(m_plGraphics, m_cPosition.x);
        break;
    // mirror (horizontal)
    case 2:
        m_pcView->DoMirror(m_plGraphics, m_cPosition.y);
        break;
    }
}

//------------------------------------------------------------------------
void
SP_DLG_TransformGraphic::OnDlgOk(wxCommandEvent& p_cEvent)
{
    if (Validate() && TransferDataFromWindow())
    {
        DoAction();
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

void
SP_DLG_TransformGraphic::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}

void
SP_DLG_TransformGraphic::OnRadiobox(wxCommandEvent& p_cEvent)
{
    wxString l_sText = p_cEvent.GetString();

    if (m_pcRadiusCtrl)
        m_pcRadiusCtrl->Enable(l_sText.Contains(wxT("Rotate")));
		m_pcChoice->Enable(true);
}


