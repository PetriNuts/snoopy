//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/tglbtn.h>

#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"
#include "snoopy.h"

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
//#include "bitmaps/anim_backstep.xpm"
#include "bitmaps/anim_pause.xpm"
#include "bitmaps/anim_play_bwd.xpm"
#include "bitmaps/anim_play_fwd.xpm"
//#include "bitmaps/anim_play.xpm"
#include "bitmaps/anim_reset.xpm"
#include "bitmaps/anim_step_bwd.xpm"
#include "bitmaps/anim_step_fwd.xpm"
//#include "bitmaps/anim_step.xpm"


#endif


IMPLEMENT_CLASS(SP_DLG_Animation, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_Animation, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_Animation::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_Animation::OnDlgCancel)
EVT_CLOSE(SP_DLG_Animation::OnMyClose)
EVT_BUTTON(SP_ID_BUTTON_ANIM_PLAYBWD, SP_DLG_Animation::OnAnimPlayBwd)
EVT_BUTTON(SP_ID_BUTTON_ANIM_STEPBWD, SP_DLG_Animation::OnAnimStepBwd)
EVT_BUTTON(SP_ID_BUTTON_ANIM_RESET, SP_DLG_Animation::OnReset)
EVT_BUTTON(SP_ID_BUTTON_ANIM_STEPFWD, SP_DLG_Animation::OnAnimStepFwd)
EVT_BUTTON(SP_ID_BUTTON_ANIM_PLAYFWD, SP_DLG_Animation::OnAnimPlayFwd)

EVT_BUTTON(SP_MENU_ITEM_ANIMOPTIONS, SP_DLG_Animation::OnOptions)

EVT_UPDATE_UI(SP_ID_BUTTON_ANIM_STEPBWD, SP_DLG_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_ANIM_STEPFWD, SP_DLG_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_ANIM_RESET, SP_DLG_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_ANIM_PLAYBWD, SP_DLG_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_ANIM_PLAYFWD, SP_DLG_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_MENU_ITEM_ANIMOPTIONS, SP_DLG_Animation::OnUpdateUI)
END_EVENT_TABLE()

SP_DLG_Animation::SP_DLG_Animation(SP_DS_Animation* p_pcAnim,
                                   SP_GUI_Mainframe* p_pcParent)
:wxDialog(p_pcParent, -1, wxT("Animation"), wxDefaultPosition, wxDefaultSize,
	  wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxCAPTION),
m_pcAnimation(p_pcAnim),
m_pcParent(p_pcParent),
m_bBackStepping(false)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

#if defined(__WXMSW__)
    m_bitmaps[0] = wxBitmap(wxT("BITMAP_ANIM_PLAY_BWD"), wxBITMAP_TYPE_BMP_RESOURCE);
    m_bitmaps[1] = wxBitmap(wxT("BITMAP_ANIM_STEP_BWD"), wxBITMAP_TYPE_BMP_RESOURCE);
    m_bitmaps[2] = wxBitmap(wxT("BITMAP_ANIM_RESET"), wxBITMAP_TYPE_BMP_RESOURCE);
    m_bitmaps[3] = wxBitmap(wxT("BITMAP_ANIM_STEP_FWD"), wxBITMAP_TYPE_BMP_RESOURCE);
    m_bitmaps[4] = wxBitmap(wxT("BITMAP_ANIM_PLAY_FWD"), wxBITMAP_TYPE_BMP_RESOURCE);
    m_bitmaps[5] = wxBitmap(wxT("BITMAP_ANIM_PAUSE"), wxBITMAP_TYPE_BMP_RESOURCE);
#else // !WIN
    m_bitmaps[0] = wxBitmap(anim_play_bwd_xpm);
    m_bitmaps[1] = wxBitmap(anim_step_bwd_xpm);
    m_bitmaps[2] = wxBitmap(anim_reset_xpm);
    m_bitmaps[3] = wxBitmap(anim_step_fwd_xpm);
    m_bitmaps[4] = wxBitmap(anim_play_fwd_xpm);
    m_bitmaps[5] = wxBitmap(anim_pause_xpm);

#endif // !WIN
/*
    m_bitmaps[0].SetMask(new wxMask(m_bitmaps[0], *wxBLUE));
    m_bitmaps[1].SetMask(new wxMask(m_bitmaps[1], *wxBLUE));
    m_bitmaps[2].SetMask(new wxMask(m_bitmaps[2], *wxBLUE));
    m_bitmaps[3].SetMask(new wxMask(m_bitmaps[3], *wxBLUE));
    m_bitmaps[4].SetMask(new wxMask(m_bitmaps[4], *wxBLUE));
    m_bitmaps[5].SetMask(new wxMask(m_bitmaps[5], *wxBLUE));
*/

    wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		m_buPlayBwd = new wxBitmapButton(this, SP_ID_BUTTON_ANIM_PLAYBWD,
												m_bitmaps[0], wxDefaultPosition, wxDefaultSize);
		m_buPlayBwd->SetToolTip(wxT("Play backward / Pause"));
		l_pcSizer->Add(m_buPlayBwd, 0, wxALL, 3);

		m_buStepBwd = new wxBitmapButton(this, SP_ID_BUTTON_ANIM_STEPBWD,
												m_bitmaps[1], wxDefaultPosition, wxDefaultSize);
		m_buStepBwd->SetToolTip(wxT("Step backward"));
		l_pcSizer->Add(m_buStepBwd, 0, wxALL, 3);

		m_buReset = new wxBitmapButton(this, SP_ID_BUTTON_ANIM_RESET,
												m_bitmaps[2],	wxDefaultPosition, wxDefaultSize);
		m_buReset->SetToolTip(wxT("Reset Marking"));
		l_pcSizer->Add(m_buReset, 0, wxALL, 3);

    m_buStepFwd = new wxBitmapButton(this, SP_ID_BUTTON_ANIM_STEPFWD,
												m_bitmaps[3], wxDefaultPosition, wxDefaultSize);
		m_buStepFwd->SetToolTip(wxT("Step forward"));
		l_pcSizer->Add(m_buStepFwd, 0, wxALL, 3);

		m_buPlayFwd = new wxBitmapButton(this, SP_ID_BUTTON_ANIM_PLAYFWD,
												m_bitmaps[4], wxDefaultPosition, wxDefaultSize);
		m_buPlayFwd->SetToolTip(wxT("Play forward / Pause"));
		l_pcSizer->Add(m_buPlayFwd, 0, wxALL, 3);

		int x, y;
		m_buPlayFwd->GetSize(&x, &y);
    l_pcSizer->Add(new wxButton(this, SP_MENU_ITEM_ANIMOPTIONS, wxT("Options"),
												wxDefaultPosition, wxSize(-1, y)), 1, wxALL, 3);
    m_pcSizer->Add(l_pcSizer, 1, wxALL | wxEXPAND, 2);

    if (m_pcAnimation)
        m_pcAnimation->AddToControl(this, m_pcSizer);

    SetSizerAndFit(m_pcSizer);

	GetSize(&x, &y);
	wxWindow *l_cMF = wxGetApp().GetMainframe();
	wxRect r = l_cMF->GetRect();
	Move(r.GetLeft() + 10, r.GetBottom() - y - 10);

}

bool
SP_DLG_Animation::CleanUp()
{
    while (GetEventHandler() && GetEventHandler() != this)
        PopEventHandler();

    return TRUE;
}

//------------------------------------------------------------------------
void
SP_DLG_Animation::OnMyClose(wxCloseEvent& p_cEvent)
{
	m_pcAnimation->StopTimer();
	SP_MDI_Doc* l_pcMyDoc = SP_Core::Instance()->GetRootDocument();
	CHECK_POINTER(l_pcMyDoc, return );
	SP_DS_Graph* l_pcMyGraph = l_pcMyDoc ->GetGraph();
	CHECK_POINTER(l_pcMyGraph, return );
	wxNode* l_pcNode = wxGetApp().GetDocmanager()->GetDocuments().GetFirst();
	while(l_pcNode)
	{
		SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcNode->GetData());
		SP_Core::Instance()->ManageDocChange(l_pcDoc);
		SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

		if (l_pcGraph)
			l_pcGraph->OnToggleAnim(FALSE);

		l_pcNode = l_pcNode->GetNext();
	}
	SP_Core::Instance()->ManageDocChange(l_pcMyDoc);
}

void
SP_DLG_Animation::OnOptions(wxCommandEvent& p_cEvent)
{
    if (m_pcAnimation)
    {
        SP_DLG_AnimationProperties* l_pcDlg = new SP_DLG_AnimationProperties(m_pcAnimation, this);

        l_pcDlg->ShowModal();
        l_pcDlg->Destroy();

        //by george
        //required to notify all animator, that this animator is the  last one, whose option changed
        SP_MDI_Doc* l_pcMyDoc = SP_Core::Instance()->GetRootDocument();
        CHECK_POINTER(l_pcMyDoc, return);
        SP_DS_Graph* l_pcMyGraph = l_pcMyDoc->GetGraph();
        SP_Core::Instance()->ActivateAnimBAsedOnOptionSet(l_pcMyGraph, m_pcAnimation->GetRefreshFrequ(), m_pcAnimation->GetStepDuration());

    }
}

void
SP_DLG_Animation::OnAnimPlayBwd(wxCommandEvent& p_cEvent)
{
    if (!m_pcAnimation)
        return;

    if (m_pcAnimation->GetRunning()) {
        m_pcAnimation->StopTimer();
				m_buPlayBwd->SetBitmapLabel(m_bitmaps[0]);
				m_buPlayBwd->Refresh();
		} else {
				m_pcAnimation->SetDirection(BACKWARD);
				m_pcAnimation->StartTimer();
				m_buPlayBwd->SetBitmapLabel(m_bitmaps[5]);
				m_buPlayBwd->Refresh();
    }
}

void
SP_DLG_Animation::OnAnimStepBwd(wxCommandEvent& p_cEvent)
{
    if (!m_pcAnimation)
        return;

		m_pcAnimation->SetDirection(BACKWARD);
    m_pcAnimation->SetOneShot(TRUE);
    m_pcAnimation->StartTimer();
}

void
SP_DLG_Animation::OnReset(wxCommandEvent& p_cEvent) {
	m_pcAnimation->OnReset();
}

void
SP_DLG_Animation::OnAnimStepFwd(wxCommandEvent& p_cEvent)
{
    if (!m_pcAnimation)
        return;

		m_pcAnimation->SetDirection(FORWARD);
    m_pcAnimation->SetOneShot(TRUE);
    m_pcAnimation->StartTimer();
}

void
SP_DLG_Animation::OnAnimPlayFwd(wxCommandEvent& p_cEvent)
{
    if (!m_pcAnimation)
        return;

    if (m_pcAnimation->GetRunning()) {
        m_pcAnimation->StopTimer();
				m_buPlayFwd->SetBitmapLabel(m_bitmaps[4]);
				m_buPlayFwd->Refresh();
		} else {

        m_pcAnimation->SetDirection(FORWARD);

        if( m_pcAnimation->StartTimer() ) {

				  m_buPlayFwd->SetBitmapLabel(m_bitmaps[5]);
  				m_buPlayFwd->Refresh();

        }

    }

}

void
SP_DLG_Animation::OnDlgOk(wxCommandEvent& p_cEvent)
{
}

void
SP_DLG_Animation::OnDlgCancel(wxCommandEvent& p_cEvent)
{

}

void
SP_DLG_Animation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
    if (!m_pcAnimation)
    {
        p_cEvent.Enable(FALSE);
        return;
    }
    switch (p_cEvent.GetId())
    {
		case SP_ID_BUTTON_ANIM_PLAYBWD:
				p_cEvent.Enable(!(m_pcAnimation->GetRunning() && m_pcAnimation->GetDirection() == FORWARD)
													&& !m_pcAnimation->GetOneShot() && m_bBackStepping);
				break;
    case SP_ID_BUTTON_ANIM_PLAYFWD:
				p_cEvent.Enable(!(m_pcAnimation->GetRunning() && m_pcAnimation->GetDirection() == BACKWARD)
												&& !m_pcAnimation->GetOneShot());
				break;
		case SP_ID_BUTTON_ANIM_STEPBWD:
        p_cEvent.Enable(!m_pcAnimation->GetRunning() && m_bBackStepping);
        break;
		case SP_ID_BUTTON_ANIM_RESET:
    case SP_ID_BUTTON_ANIM_STEPFWD:
    case SP_MENU_ITEM_ANIMOPTIONS:
        p_cEvent.Enable(!m_pcAnimation->GetRunning());
        break;
    };
}

void
SP_DLG_Animation::ResetPlayButtons() {
	m_buPlayBwd->SetBitmapLabel(m_bitmaps[0]);
	m_buPlayBwd->Refresh();
	m_buPlayFwd->SetBitmapLabel(m_bitmaps[4]);
	m_buPlayFwd->Refresh();
}


void SP_DLG_Animation::EnableAnimMode() {
	//by george for enabling anim mode when color simulation is selected
	m_buPlayBwd->Enable(true);
	m_buPlayFwd->Enable(true);
	m_buStepBwd->Enable(true);
	m_buReset->Enable(true);
	m_buStepFwd->Enable(true);
}

void SP_DLG_Animation::DisapleAnimMode() {
	//by george for diabling anim mode when color simulation is selected
	m_buPlayBwd->Enable(false);
	m_buPlayFwd->Enable(false);
	m_buStepBwd->Enable(false);
	m_buReset->Enable(false);
	m_buStepFwd->Enable(false);
	
		 
}
wxWindow*
SP_DLG_Animation::getWindow()
{

    return m_pcParent;
}


