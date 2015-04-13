/*
 * SP_DS_ExtPTTransAnimator.cpp
 *
 *  Created on: 31.08.2012
 *      Author: dell-notebook
 */
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_ExtPTTransAnimator.h"
#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"



#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"

//#include "sp_core/SP_GPR_Animation.h"
//#include <wx/filename.h>
//#include "snoopy.h"

SP_DS_ExtPTTransAnimator::SP_DS_ExtPTTransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_TransAnimator(p_pcAnim, p_pcParent)
{
	SetAnimatorType(SP_DS_ANIMATOR_TRANS);
}

SP_DS_ExtPTTransAnimator::~SP_DS_ExtPTTransAnimator()
{

}

SP_DS_Animator*
SP_DS_ExtPTTransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_DS_ExtPTTransAnimator* l_pcNew = new SP_DS_ExtPTTransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

	return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}



bool
SP_DS_ExtPTTransAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
	return SP_DS_TransAnimator::PreStep(p_nSteps, p_nDir);
}



bool
SP_DS_ExtPTTransAnimator::Step(unsigned int p_nStep)
{
    return SP_DS_TransAnimator::Step(p_nStep);
}

//////////////////////////////////////////////////////////
/** Called after transition fired*/
/*!
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_ExtPTTransAnimator::PostStep()
{
	SP_DS_TransAnimator::PostStep();

	return true;
}

//////////////////////////////////////////////////////////
/** Invoked on left clicking on a transition */
/*!
  Executes a single step in forward direction
  \return always TRUE
 */
//////////////////////////////////////////////////////////
bool
SP_DS_ExtPTTransAnimator::OnLeftClick()
{

	SP_DS_TransAnimator::OnLeftClick();

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
SP_DS_ExtPTTransAnimator::OnRightClick()
{

	SP_DS_TransAnimator::OnRightClick();

	return TRUE;
}

void
SP_DS_ExtPTTransAnimator::CalculateMultiplicity()
{
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_ExtPTPlaceAnimator* l_pcAnim;

	m_nMultiplicity = LONG_MAX;

	if (m_pcAnimation->GetDirection() == FORWARD) {
		if (m_pcNode->GetTargetEdges()->size() > 0) {
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetClassName() == wxT("Edge"))
				{
					SP_DS_Attribute* l_pcAttr;
					l_pcAttr = (*l_itEdges)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
					if (l_pcAttr)
					{
						if ((*l_itEdges)->GetSource())
						{
							l_pcAnim = dynamic_cast<SP_DS_ExtPTPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								long l_nMarking = l_pcAnim->GetMarking();
								long l_nMultiplicity = dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(l_pcAttr)->GetValue();
								long l_nNewMultiplicity = l_nMarking / l_nMultiplicity;
								if (l_nNewMultiplicity < m_nMultiplicity)
									m_nMultiplicity = l_nNewMultiplicity;
							}
						}
					}
				}
			}
		}
	} else {
		if (m_pcNode->GetSourceEdges()->size() > 0) {
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
				if((*l_itEdges)->GetClassName() == wxT("Edge"))
				{
					SP_DS_Attribute* l_pcAttr;
					l_pcAttr = (*l_itEdges)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
					if (l_pcAttr)
					{
						if ((*l_itEdges)->GetTarget())
						{
							l_pcAnim = dynamic_cast<SP_DS_ExtPTPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								long l_nMarking = l_pcAnim->GetMarking();
								long l_nMultiplicity = dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(l_pcAttr)->GetValue();
								long l_nNewMultiplicity = l_nMarking / l_nMultiplicity;
								if (l_nNewMultiplicity < m_nMultiplicity)
									m_nMultiplicity = l_nNewMultiplicity;
							}
						}
					}
				}
			}
		}
	}

	// TODO: Probably need some error reporting here.
	if (m_nMultiplicity == LONG_MAX) {
		m_nMultiplicity = 1;
	}
}

