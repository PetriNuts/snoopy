//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored PN coarse transiton class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStCoarseTransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStPlaceAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"

SP_DS_ColStCoarseTransAnimator::SP_DS_ColStCoarseTransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL)
{
    SetAnimatorType(SP_DS_ANIMATOR_COARSE_TRANS);

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_ColStCoarseTransAnimator::~SP_DS_ColStCoarseTransAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_ColStCoarseTransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_ColStCoarseTransAnimator* l_pcNew = new SP_DS_ColStCoarseTransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}
#if 0
bool
SP_DS_ColStCoarseTransAnimator::InformPrePlaces()
{
    Reset();
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    // i inform all nodes at my incoming edges of wanting their markings
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_ColStPlaceAnimator* l_pcAnim;
		if (m_pcAnimation->GetDirection() == FORWARD) {
			if (m_pcNode->GetTargetEdges()->size() == 0) {
					m_bEnabled = TRUE;
					return TRUE;
			}
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetSource())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
									l_pcAnim->AddCandidate((*l_itEdges), this);
									m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
							}
					}
			}
		} else {
			if (m_pcNode->GetSourceEdges()->size() == 0) {
					m_bEnabled = TRUE;
					return TRUE;
			}
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
									l_pcAnim->AddCandidate((*l_itEdges), this);
									m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
							}
					}
			}
		}
    return TRUE;
}

bool
SP_DS_ColStCoarseTransAnimator::TestConcession()
{
    if (m_mlPrePlaces.size() <= m_lCandidates.size())
        return TRUE;
    // no concession
    map<SP_DS_ColStPlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    return FALSE;
}

bool
SP_DS_ColStCoarseTransAnimator::Reset()
{
	map<SP_DS_ColStPlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = GetGraphics();
    SP_ListGraphic::iterator l_Iter2;
    SP_GR_MarkAnimator* l_pcGr;

    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    m_lCandidates.clear();
    m_bEnabled = FALSE;
    m_mlPrePlaces.clear();

    if (!l_plGraphics)
        return TRUE;

    for (l_Iter2 = l_plGraphics->begin(); l_Iter2 != l_plGraphics->end(); l_Iter2 = l_plGraphics->begin())
    {
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(dynamic_cast<SP_GR_Animator*>((*l_Iter2)));
        l_plGraphics->erase(l_Iter2);
        wxDELETE(l_pcGr);
    }
    return TRUE;
}

// animate
bool
SP_DS_ColStCoarseTransAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    if (!m_bEnabled)
        return FALSE;

    list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter)
        (*l_Iter)->SetEnabled(TRUE);

    m_nSteps = p_nSteps;
    SP_ListEdge::const_iterator l_itEdges;
    SP_ListGraphic::iterator l_itGraphic;
    SP_GR_MarkAnimator* l_pcAnim;
		bool l_bReturn = false;

		if (p_nDir == FORWARD) {
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetGraphics())
							for (l_itGraphic = (*l_itEdges)->GetGraphics()->begin(); l_itGraphic != (*l_itEdges)->GetGraphics()->end(); ++l_itGraphic)
							{
									l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
									if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
									{
											AddGraphic(l_pcAnim);
											m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
									}
									else
											wxDELETE(l_pcAnim);
							}
			}

			l_bReturn = m_pcNode->GetSourceEdges()->size() == 0;

		} else {
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetGraphics())
							for (l_itGraphic = (*l_itEdges)->GetGraphics()->begin(); l_itGraphic != (*l_itEdges)->GetGraphics()->end(); ++l_itGraphic)
							{
									l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
									if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
									{
											AddGraphic(l_pcAnim);
											m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
									}
									else
											wxDELETE(l_pcAnim);
							}
			}

			l_bReturn = m_pcNode->GetTargetEdges()->size() == 0;

		}

    return (l_bReturn || GetGraphics() && (GetGraphics()->size() > 0));
}

bool
SP_DS_ColStCoarseTransAnimator::Step(unsigned int p_nStep)
{
    if (!GetGraphics())
        return TRUE;

    bool l_bReturn = TRUE;
    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
        l_bReturn &= dynamic_cast<SP_GR_Animator*>((*l_Iter))->Step(p_nStep);

    return l_bReturn;
}

bool
SP_DS_ColStCoarseTransAnimator::PostStep()
{
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_ColStPlaceAnimator* l_pcAnim;

    Reset();
		if (m_pcAnimation->GetDirection() == FORWARD) {
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetTarget())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
									l_pcAnim->IncrementMark((*l_itEdges));
					}
			}
		} else {
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
					if ((*l_itEdges)->GetSource())
					{
							l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
									l_pcAnim->IncrementMark((*l_itEdges));
					}
			}
		}

    return RemoveAllGraphics();
}
#endif
bool
SP_DS_ColStCoarseTransAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_ColStAnimation* l_pcAnim = dynamic_cast<SP_DS_ColStAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(FORWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

bool
SP_DS_ColStCoarseTransAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_ColStAnimation* l_pcAnim = dynamic_cast<SP_DS_ColStAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(BACKWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}
#if 0
void
SP_DS_ColStCoarseTransAnimator::Enable(SP_DS_ColStPlaceAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    m_lCandidates.push_back(p_pcAnimator);
}
#endif
