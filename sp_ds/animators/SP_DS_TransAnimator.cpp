//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/animators/SP_DS_PlaceAnimator.h"
#include "sp_ds/extensions/SP_DS_PedAnimation.h"
// Added by Erik Jongsma
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
// Added by Steffen Laarz
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


SP_DS_TransAnimator::SP_DS_TransAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL)
{
    SetAnimatorType(SP_DS_ANIMATOR_TRANS);

    // Added by Erik Jongsma
    m_nMultiplicity = 1;

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_TransAnimator::~SP_DS_TransAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_TransAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_TransAnimator* l_pcNew = new SP_DS_TransAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}


bool
SP_DS_TransAnimator::SimulateFiring(bool p_bAutoConcurrency)
{
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_PlaceAnimator* l_pcAnim;
	if (m_pcAnimation->GetDirection() == FORWARD) 
	{
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim = dynamic_cast<SP_DS_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					if (p_bAutoConcurrency)
					{
						l_pcAnim->DecrementSimValueAutoConcurrency(this);
					}
					else
					{
						l_pcAnim->DecrementSimValue(this);
					}
				}
			}
		}
	} 
	else 
	{
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim = dynamic_cast<SP_DS_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					if (p_bAutoConcurrency)
					{
						l_pcAnim->DecrementSimValueAutoConcurrency(this);
					}
					else
					{
						l_pcAnim->DecrementSimValue(this);
					}
				}
			}
		}
	}
	return true;
}

bool
SP_DS_TransAnimator::HasRealPrePlace()
{
	if (m_mlPrePlaces.size() == 0)
		return false;

    map<SP_DS_PlaceAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
	{
		SP_ListEdge* l_plEdges = &(l_Iter->second);
		for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
		{
			if( (*l_itE)->GetClassName() == wxT("Edge"))
			{
				return true;
			}
		}
	}
	return false;
}


bool
SP_DS_TransAnimator::InformPrePlaces()
{
    Reset();
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    // i inform all nodes at my incoming edges of wanting their markings
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_PlaceAnimator* l_pcAnim;
	if (m_pcAnimation->GetDirection() == FORWARD)
	{
		if (m_pcNode->GetTargetEdges()->size() == 0)
		{
			m_bEnabled = TRUE;
			return TRUE;
		}
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges
				!= m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim
						= dynamic_cast<SP_DS_PlaceAnimator*> (m_pcAnimation->GetAnimator(
								(*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
				{
					l_pcAnim->AddCandidate((*l_itEdges), this);
					m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
				}
			}
		}
	}
	else
	{
		if (m_pcNode->GetSourceEdges()->size() == 0)
		{
			m_bEnabled = TRUE;
			return TRUE;
		}
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges
				!= m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim
						= dynamic_cast<SP_DS_PlaceAnimator*> (m_pcAnimation->GetAnimator(
								(*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
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
SP_DS_TransAnimator::TestConcession()
{
    if (m_mlPrePlaces.size() <= m_lCandidates.size())
    {
    	return TRUE;
    }
    // no concession
    map<SP_DS_PlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
    {
    	l_Iter->first->RemoveCandidate(this);
    }

    return FALSE;
}

bool
SP_DS_TransAnimator::Reset()
{
    map<SP_DS_PlaceAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = GetGraphics();
    SP_ListGraphic::iterator l_Iter2;
    SP_GR_MarkAnimator* l_pcGr;

    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
    {
    	l_Iter->first->RemoveCandidate(this);
    }

    m_lCandidates.clear();
    m_bEnabled = FALSE;
    m_mlPrePlaces.clear();
    m_bAutoEnabled = true;

    if (!l_plGraphics)
        return TRUE;

    for (l_Iter2 = l_plGraphics->begin(); l_Iter2 != l_plGraphics->end(); l_Iter2 = l_plGraphics->begin())
    {
    	SP_Graphic* l_pcGrIt = *l_Iter2;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter2);
        wxDELETE(l_pcGr);
    }
    return TRUE;
}

// animate
bool
SP_DS_TransAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    if (!m_bEnabled)
        return FALSE;

    list<SP_DS_PlaceAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter)
    {
        (*l_Iter)->SetEnabled(TRUE);
		//SP_LOGDEBUG(wxString::Format(wxT("Enabled candidate place %d"),(*l_Iter)->GetId()));
    }

    m_nSteps = p_nSteps;
    SP_ListEdge::const_iterator l_itEdges;
    SP_GR_MarkAnimator* l_pcAnim;
	bool l_bReturn = false;

	if (p_nDir == FORWARD)
	{
		for(l_itEdges = m_pcNode->GetSourceEdges()->begin();
			l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetGraphics())
			{
				for (SP_Graphic* l_pcGraphic : *(*l_itEdges)->GetGraphics())
				{
					l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*> (m_pcProtoshape->Clone(this));
					if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
					{
						AddGraphic(l_pcAnim);
						m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
					}
					else
					{
						wxDELETE(l_pcAnim);
					}
				}
			}
			l_bReturn = m_pcNode->GetSourceEdges()->size() == 0;
		}
	}
	else
	{
		for(l_itEdges = m_pcNode->GetTargetEdges()->begin();
			l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetGraphics())
			{
				for (SP_Graphic* l_pcGraphic : *(*l_itEdges)->GetGraphics())
				{
					l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
					if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
					{
						AddGraphic(l_pcAnim);
						m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
					}
					else
					{
						wxDELETE(l_pcAnim);
					}
				}
			}
		}
		l_bReturn = m_pcNode->GetTargetEdges()->size() == 0;
	}

    return (l_bReturn || (GetGraphics() && (GetGraphics()->size() > 0)));
}

bool
SP_DS_TransAnimator::Step(unsigned int p_nStep)
{
    if (!GetGraphics())
        return TRUE;

    bool l_bReturn = TRUE;
    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
    {
    	SP_Graphic* l_pcGr = *l_Iter;
        l_bReturn &= dynamic_cast<SP_GR_Animator*>(l_pcGr)->Step(p_nStep);
    }
    return l_bReturn;
}

bool
SP_DS_TransAnimator::PostStep()
{
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_PlaceAnimator* l_pcAnim;

    Reset();
	if (m_pcAnimation->GetDirection() == FORWARD)
	{
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges
				!= m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				l_pcAnim
						= dynamic_cast<SP_DS_PlaceAnimator*> (m_pcAnimation->GetAnimator(
								(*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->IncrementMark((*l_itEdges));
			}
		}
	}
	else
	{
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges
				!= m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				l_pcAnim
						= dynamic_cast<SP_DS_PlaceAnimator*> (m_pcAnimation->GetAnimator(
								(*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
				if (l_pcAnim)
					l_pcAnim->IncrementMark((*l_itEdges));
			}
		}
	}

	// Added by Erik Jongsma
	m_nMultiplicity = 1;

    return RemoveAllGraphics();
}

void
SP_DS_TransAnimator::CalculateMultiplicity()
{
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_PlaceAnimator* l_pcAnim;

	m_nMultiplicity = LONG_MAX;

	if (m_pcAnimation->GetDirection() == FORWARD) 
	{
		if (m_pcNode->GetTargetEdges()->size() > 0) 
		{
			for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetClassName() == wxT("Edge"))
				{
					SP_DS_Attribute* l_pcAttr;
					l_pcAttr = (*l_itEdges)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
					if (l_pcAttr)
					{
						if ((*l_itEdges)->GetSource())
						{
							l_pcAnim = dynamic_cast<SP_DS_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								long l_nMarking = l_pcAnim->GetMarking();
								long l_nMultiplicity = dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();
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
	else 
	{
		if (m_pcNode->GetSourceEdges()->size() > 0) 
		{
			for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
			{
				if((*l_itEdges)->GetClassName() == wxT("Edge"))
				{
					SP_DS_Attribute* l_pcAttr;
					l_pcAttr = (*l_itEdges)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
					if (l_pcAttr)
					{
						if ((*l_itEdges)->GetTarget())
						{
							l_pcAnim = dynamic_cast<SP_DS_PlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
							if (l_pcAnim)
							{
								long l_nMarking = l_pcAnim->GetMarking();
								long l_nMultiplicity = dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();
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

bool
SP_DS_TransAnimator::IncreaseMultiplicity()
{
	m_nMultiplicity++;
	return true;
}

bool
SP_DS_TransAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_PedAnimation* l_pcAnim = dynamic_cast<SP_DS_PedAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(FORWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

bool
SP_DS_TransAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_PedAnimation* l_pcAnim = dynamic_cast<SP_DS_PedAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(BACKWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

void
SP_DS_TransAnimator::Enable(SP_DS_PlaceAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    m_lCandidates.push_back(p_pcAnimator);
}

void
SP_DS_TransAnimator::Disable(SP_DS_PlaceAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    list<SP_DS_PlaceAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter)
	{
		if ((*l_Iter) == p_pcAnimator)
		{
			m_lCandidates.erase(l_Iter);
			break;
		}
	}
}

void
SP_DS_TransAnimator::SetAutoEnabled(bool p_bValue)
{
	m_bAutoEnabled = p_bValue;
}



