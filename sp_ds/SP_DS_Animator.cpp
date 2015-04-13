//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Animator.h"
#include "sp_ds/extensions/SP_DS_Animation.h"

SP_DS_Animator::SP_DS_Animator(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim):
SP_Data(SP_ELEMENT_ANIMATOR),
SP_Animator(SP_ANIM_DS),
m_eAnimatorType(SP_DS_ANIMATOR_NULL),
m_pcParent(p_pcParent),
m_pcAnimation(p_pcAnim),
m_nSteps(0),
m_bEnabled(FALSE)
{
    if (m_pcAnimation)
        m_pcAnimation->AddAnimator(p_pcParent, this);
}

SP_DS_Animator::~SP_DS_Animator()
{
}
