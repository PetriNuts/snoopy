//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/06/30 11:55:00 $
// Short Description: Animation class for musical petri nets
//////////////////////////////////////////////////////////////////////

#include <wx/statline.h>
#include "sp_ds/extensions/SP_DS_MusicPedAnimation.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"


BEGIN_EVENT_TABLE(SP_DS_MusicPedAnimation, SP_DS_PedAnimation)

EVT_CHECKBOX(SP_ID_TOGGLEMUTE, SP_DS_MusicPedAnimation::OnToggleMute)

EVT_BUTTON(SP_ID_BUTTON_ANIM_PLAYFWD, SP_DS_MusicPedAnimation::OnAnimPlayFwd)
EVT_BUTTON(SP_ID_BUTTON_ANIM_PLAYBWD, SP_DS_MusicPedAnimation::OnAnimPlayBwd)
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SOUND_SETS, SP_DS_MusicPedAnimation :: OnModifySoundSets)

EVT_COMBOBOX( SP_ID_COMBOBOX_SOUND_SETS, SP_DS_MusicPedAnimation::OnSoundSetChanged)

EVT_COMMAND_SCROLL(SP_ID_SLIDER, SP_DS_MusicPedAnimation::OnVolumeChanged)

EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_SOUND_SETS, SP_DS_MusicPedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_COMBOBOX_SOUND_SETS, SP_DS_MusicPedAnimation::OnUpdateUI)

END_EVENT_TABLE()

SP_DS_MusicPedAnimation::SP_DS_MusicPedAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
SP_DS_PedAnimation(p_nRefresh, p_nDuration, p_eStepping),
m_cbMuteBox(0)
{
	//initialize sound device
	m_pSoundEngine = irrklang::createIrrKlangDevice();
}

SP_DS_MusicPedAnimation::~SP_DS_MusicPedAnimation()
{
	if (m_pSoundEngine)
	{
		m_pSoundEngine->stopAllSounds();
		//delete sound files
		m_pSoundEngine->removeAllSoundSources();
		//delete sound device
		m_pSoundEngine->drop();
		m_pSoundEngine = NULL;
	}
	if (m_cbMuteBox)
		wxDELETE((m_cbMuteBox));
}


SP_DS_Animation*
SP_DS_MusicPedAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_MusicPedAnimation(
		m_nRefreshFrequ,
		m_nStepDuration,
		SP_ANIM_STEP_T(m_nStepState)));
}


bool
SP_DS_MusicPedAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
	bool l_bReturn = SP_DS_PedAnimation::Initialise(p_pcGraph);
	SP_ListAnimator::iterator l_Iter;
	m_lAllPlaceAnimators.clear();
	m_lAllTransAnimators.clear();
	if (l_bReturn)
	{
		for (l_Iter = m_lAllAnimators.begin(); l_Iter != m_lAllAnimators.end(); ++l_Iter)
		{
			SP_DS_Animator* l_pcAnimator = *l_Iter;
			if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_PLACE)
				m_lAllPlaceAnimators.push_back(dynamic_cast<SP_DS_PlaceAnimator*>(l_pcAnimator));
			else if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_MUSIC_TRANS)
				m_lAllTransAnimators.push_back(dynamic_cast<SP_DS_TransAnimator*>(l_pcAnimator));
		}
	}

	return l_bReturn;
}

irrklang::ISoundEngine*
SP_DS_MusicPedAnimation::GetSoundEngine()
{
	return m_pSoundEngine;
}

void
SP_DS_MusicPedAnimation::LoadSoundSets()
{
	m_pcSoundSetComboBox->Clear();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(wxT("Transition"));

	if (l_pcNodeclass->GetElements()->empty())
		return;
	SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("Sound")));

	for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++) {

		wxString l_sSetName = l_pcAttr->GetCell(i,0);
		m_pcSoundSetComboBox->Append( l_sSetName );

		if (i == l_pcAttr->GetActiveList())
		{
			//preselect active set
			m_pcSoundSetComboBox->Select(i);
		}
	}
}



bool
SP_DS_MusicPedAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer)
{
	if (!SP_DS_PedAnimation::AddToControl(p_pcCtrl, p_pcSizer))
		return FALSE;

	m_pcDialog = p_pcCtrl;
	wxSizer* l_pcSetsSizer = new wxBoxSizer( wxVERTICAL );

	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcRowSizer->Add( new wxStaticText( p_pcCtrl, -1, wxT("Sound Set:") ), 1, wxALL | wxEXPAND, 5 );

	m_pcSoundSetComboBox = new wxComboBox( p_pcCtrl, SP_ID_COMBOBOX_SOUND_SETS, _T( "" ), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );

	l_pcRowSizer->Add( m_pcSoundSetComboBox, 0, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( p_pcCtrl, SP_ID_BUTTON_MODIFY_SOUND_SETS, wxT( "Overview" ) ), 0, wxALL, 5 );

	l_pcSetsSizer->Add( l_pcRowSizer, 0, wxEXPAND);

	p_pcSizer->Add( new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND);
	p_pcSizer->Add( l_pcSetsSizer, 0, wxALL, 5 );

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	m_cbMuteBox = new wxCheckBox(p_pcCtrl, SP_ID_TOGGLEMUTE, wxT("Mute Sound"));
	l_pcRowSizer->Add(m_cbMuteBox, 0, wxALL, 5);

	l_pcRowSizer->Add( new wxStaticText( p_pcCtrl, -1, wxT("Volume:") ), 0, wxALL, 5 );

	m_pcSlider = new wxSlider(p_pcCtrl, SP_ID_SLIDER,100,0,100);
	l_pcRowSizer->Add(m_pcSlider, 1, wxALL | wxEXPAND , 5);
	p_pcSizer->Add( l_pcRowSizer, 0, wxEXPAND);

	m_pcSoundSetComboBox->Enable(true);

	LoadSoundSets();

	return TRUE;
}


void
SP_DS_MusicPedAnimation::OnToggleMute(wxCommandEvent& p_cEvent)
{
	float l_nMax = float(m_pcSlider->GetValue())/100.0f;
	m_pSoundEngine->setSoundVolume(m_cbMuteBox->IsChecked() ? 0 : l_nMax);
}


void
SP_DS_MusicPedAnimation::OnReset()
{
	SP_DS_PedAnimation::OnReset();
	m_pSoundEngine->stopAllSounds();
}


void
SP_DS_MusicPedAnimation::OnAnimPlayFwd(wxCommandEvent& p_cEvent)
{
	m_pcDialog->OnAnimPlayFwd(p_cEvent);
	m_pSoundEngine->setAllSoundsPaused(!GetRunning());
}


void
SP_DS_MusicPedAnimation::OnAnimPlayBwd(wxCommandEvent& p_cEvent)
{
	m_pcDialog->OnAnimPlayBwd(p_cEvent);
	m_pSoundEngine->setAllSoundsPaused(!GetRunning());
}


void
SP_DS_MusicPedAnimation::OnModifySoundSets( wxCommandEvent& p_cEvent )
{
	unsigned int l_nCurrentSoundSet = m_pcSoundSetComboBox->GetSelection();

	SP_DLG_ColListOverview l_cDlg(wxT("Sound"), m_pcDialog);

	if ( l_cDlg.ShowModal() == wxID_OK ) {

		LoadSoundSets();
		if( l_nCurrentSoundSet <= m_pcSoundSetComboBox->GetCount() ) {
			m_pcSoundSetComboBox->SetSelection(l_nCurrentSoundSet);
		}
	}
	l_cDlg.Destroy();
}


void
SP_DS_MusicPedAnimation::OnSoundSetChanged( wxCommandEvent& p_cEvent )
{

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass( wxT("Transition") );
	if (l_pcNodeclass->GetElements()->empty())
		return;

	SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();

	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("Sound")));
	l_pcAttr->SetActiveList(m_pcSoundSetComboBox->GetSelection());

	RefreshFrame();
}

void
SP_DS_MusicPedAnimation::OnVolumeChanged(wxScrollEvent& p_cEvent)
{
	//do nothing if sound is muted to prevent artifacts
	if (m_cbMuteBox->IsChecked())
		return;
	// scale the value of the slider to one
	float l_nVol = float(m_pcSlider->GetValue())/100.0f;
	m_pSoundEngine->setSoundVolume(l_nVol);
}

void SP_DS_MusicPedAnimation::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}
