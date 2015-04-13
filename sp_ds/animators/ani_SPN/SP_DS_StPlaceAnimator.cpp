//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "SP_DS_StPlaceAnimator.h"
#include "SP_DS_StTransAnimator.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

SP_DS_StPlaceAnimator::SP_DS_StPlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_ExtPTPlaceAnimator( p_pchAttributeName, p_pcAnim, p_pcParent)
{
}

SP_DS_StPlaceAnimator::~SP_DS_StPlaceAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_StPlaceAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_DS_StPlaceAnimator* l_pcNew = new SP_DS_StPlaceAnimator(m_sAttributeName, p_pcAnim, dynamic_cast<SP_DS_Node*> (p_pcParent));

	return l_pcNew;
}

bool SP_DS_StPlaceAnimator::ResetMarking()
{
	if (m_pcAttribute && m_pcNode)
	{

		m_pcAttribute->SetValue(m_nDefaultValue);
		m_pcNode->Update(TRUE);
	}

	return TRUE;
}
