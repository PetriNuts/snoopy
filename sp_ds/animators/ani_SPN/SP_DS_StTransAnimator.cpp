//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StTransAnimator.h"

SP_DS_StTransAnimator::SP_DS_StTransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_TransAnimator( p_pcAnim, p_pcParent)
{
}

SP_DS_StTransAnimator::~SP_DS_StTransAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_StTransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_DS_StTransAnimator* l_pcNew = new SP_DS_StTransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*> (p_pcParent));

	return l_pcNew;
}

