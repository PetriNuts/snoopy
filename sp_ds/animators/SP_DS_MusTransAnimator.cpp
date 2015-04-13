//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/01 11:55:00 $
// Short Description: Animation class for transitions of music petri net that
//					  play a sound during stepping
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_MusTransAnimator.h"
#include "sp_ds/animators/SP_DS_PlaceAnimator.h"
#include "sp_ds/extensions/SP_DS_MusicPedAnimation.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_core/SP_GPR_Animation.h"

#include <wx/filename.h>

#include "snoopy.h"


SP_DS_MusTransAnimator::SP_DS_MusTransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_TransAnimator(p_pcAnim, p_pcParent),
m_bFirstStep(TRUE)
{
	SetAnimatorType(SP_DS_ANIMATOR_MUSIC_TRANS);
}

SP_DS_MusTransAnimator::~SP_DS_MusTransAnimator()
{

}

//////////////////////////////////////////////////////////
/** Returns a copy of this animator */
/*!
  \param p_pcParent
  \param p_pcAnim
  \return copy as SP_DS_Animator pointer
 */
//////////////////////////////////////////////////////////
SP_DS_Animator*
SP_DS_MusTransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_DS_MusTransAnimator* l_pcNew = new SP_DS_MusTransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

	return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

//////////////////////////////////////////////////////////
/** plays the sound file associated to the transition */
/*!
  Only plays back the sound if the bool option of a
  transition is set to do so.
  \return TRUE if played successfully, FALSE otherwise
 */
//////////////////////////////////////////////////////////
bool
SP_DS_MusTransAnimator::PlaySound()
{
	CHECK_POINTER(m_pcNode, return FALSE);
	CHECK_POINTER(m_pcAnimation, return FALSE);
	SP_DS_MusicPedAnimation* l_pcAnim = dynamic_cast<SP_DS_MusicPedAnimation*>(m_pcAnimation);

	irrklang::ISoundEngine* l_pSoundEngine = l_pcAnim->GetSoundEngine();

	// don't play sound if the respective option is disabled
	SP_DS_BoolAttribute* l_pcBool = dynamic_cast<SP_DS_BoolAttribute*>(m_pcNode->GetAttribute(wxT("Play Sound when Transition fires")));
	if (!l_pcBool || !l_pcBool->GetValue())
		return false;

	// get the attribute and play the sound file
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(m_pcNode->GetAttribute(wxT("Sound")));
	if (l_pcAttr)
	{
		//get path to sound file
		wxChar l_Separator = wxFileName::GetPathSeparator();
		wxString l_sSound = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS) + l_Separator;
		l_sSound += wxString::Format(l_pcAttr->GetCell(l_pcAttr->GetActiveList(), wxT("Sound")));
		if (!l_sSound.IsEmpty())
		{
			l_pSoundEngine->play2D(l_sSound.mb_str(wxConvUTF8));
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////
/** animation method for each step during the
  * firing process */
/*!
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_MusTransAnimator::Step(unsigned int p_nStep)
{
    if (m_bFirstStep)
	{
		m_bFirstStep = FALSE;
		PlaySound();
	}

	return SP_DS_TransAnimator::Step(p_nStep);
}

//////////////////////////////////////////////////////////
/** Called after transition fired*/
/*!
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_MusTransAnimator::PostStep()
{
    //reset bool for next firing
	m_bFirstStep = TRUE;

	return SP_DS_TransAnimator::PostStep();
}

//////////////////////////////////////////////////////////
/** Invoked on left clicking on a transition */
/*!
  Executes a single step in forward direction
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_MusTransAnimator::OnLeftClick()
{
	CHECK_POINTER(m_pcNode, return FALSE);
	CHECK_POINTER(m_pcAnimation, return FALSE);
	SP_DS_MusicPedAnimation* l_pcAnim = dynamic_cast<SP_DS_MusicPedAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(FORWARD);
	l_pcAnim->SetSingleStep(this);
	l_pcAnim->SetOneShot(TRUE);
	l_pcAnim->StartTimer();

	return TRUE;
}

//////////////////////////////////////////////////////////
/** Invoked on right clicking on a transition */
/*!
  Executes a single step in backward direction
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_MusTransAnimator::OnRightClick()
{
	CHECK_POINTER(m_pcNode, return FALSE);
	CHECK_POINTER(m_pcAnimation, return FALSE);
	SP_DS_MusicPedAnimation* l_pcAnim = dynamic_cast<SP_DS_MusicPedAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(BACKWARD);
	l_pcAnim->SetSingleStep(this);
	l_pcAnim->SetOneShot(TRUE);
	l_pcAnim->StartTimer();

	return TRUE;
}
