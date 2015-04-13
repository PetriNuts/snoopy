//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/06/30 11:55:00 $
// Short Description: Animation class for musical petri nets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MUSICPEDANIMATION_H__
#define __SP_DS_MUSICPEDANIMATION_H__

#include <irrKlang.h>
#include <wx/slider.h>
#include "sp_ds/extensions/SP_DS_PedAnimation.h"

class SP_DS_MusicPedAnimation: public SP_DS_PedAnimation
{
private:
	DECLARE_EVENT_TABLE()
	irrklang::ISoundEngine* m_pSoundEngine;
	wxCheckBox* m_cbMuteBox;
	wxComboBox* m_pcSoundSetComboBox;
	wxSlider* m_pcSlider;

	wxStaticText* m_pcOutputLabelStaticText;

	void LoadSoundSets();

	void OnModifySoundSets(wxCommandEvent& p_cEvent);
	void OnSoundSetChanged(wxCommandEvent& p_cEvent);
	void OnVolumeChanged(wxScrollEvent& p_cEvent);

	void RefreshFrame();

protected:


public:
	SP_DS_MusicPedAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
	virtual ~SP_DS_MusicPedAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);
	SP_DS_Animation* Clone();

	virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);

	irrklang::ISoundEngine* GetSoundEngine();

	/** \brief Invoked on clicking mute check box
		This toggles the volume between off and the
		setting of the volume slider
		\param p_cEvent the event object
	*/
	void OnToggleMute(wxCommandEvent& p_cEvent);

	/** \brief Invoked on clicking reset button
		This stops animation and sound playback
	*/
	void OnReset();
	void OnAnimPlayFwd(wxCommandEvent& p_cEvent);
	void OnAnimPlayBwd(wxCommandEvent& p_cEvent);


};

#endif // __SP_DS_MUSICPEDANIMATION_H__


