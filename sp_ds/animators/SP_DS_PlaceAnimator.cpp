//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_PlaceAnimator.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"



SP_DS_PlaceAnimator::SP_DS_PlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL),
m_pcNode(NULL),
m_sAttributeName(p_pchAttributeName),
m_pcAttribute(NULL),
m_nDefaultValue(0)
{
    SetAnimatorType(SP_DS_ANIMATOR_PLACE);

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcAttribute = dynamic_cast<SP_DS_MarkingAttribute*>(p_pcParent->GetAttribute(p_pchAttributeName));

        if (m_pcAttribute)
            m_nDefaultValue = m_pcAttribute->GetValue(true);
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_PlaceAnimator::~SP_DS_PlaceAnimator()
{
    if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_PlaceAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_PlaceAnimator* l_pcNew = new SP_DS_PlaceAnimator(m_sAttributeName, p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

unsigned int
SP_DS_PlaceAnimator::SumCandidates()
{
    unsigned int l_nRet = 0;
    map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
    {
    	// Added by Erik Jongsma: multiplication added
        l_nRet += (SumEdges(&((*l_Iter).second)) * l_Iter->first->GetMultiplicity());

    }

    return l_nRet;
}

unsigned int
SP_DS_PlaceAnimator::SumEdges(const SP_ListEdge* p_plEdges)
{
    if (!p_plEdges)
        return 0;

    unsigned int l_nRet = 0;
    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = p_plEdges->begin(); l_itEdge != p_plEdges->end(); ++l_itEdge)
    {
        l_nRet += SumEdge((*l_itEdge));
    }

    return l_nRet;
}

unsigned int
SP_DS_PlaceAnimator::SumEdge(SP_DS_Edge* p_pcEdge)
{
    if (!p_pcEdge)
        return 0;

    unsigned int l_nRet = 0;
    SP_DS_Attribute* l_pcAttr;
    l_pcAttr = p_pcEdge->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
    if (l_pcAttr)
        l_nRet += dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();

    return l_nRet;
}




/*
 * set new edge value
 */
bool
SP_DS_PlaceAnimator::MarkingEdge(SP_DS_Edge* p_pcEdge)
{
    if (!p_pcEdge)
        return false;

   SP_DS_Attribute* l_pcAttr;

   l_pcAttr = p_pcEdge->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
    if (l_pcAttr)
    {
    	dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue(true);
    }

    //update the edge
    p_pcEdge->Update();

    return true;
}

/*
 * set new edge value
 */
bool
SP_DS_PlaceAnimator::MarkingSourceEdges()
{
	SP_ListEdge::const_iterator l_itEdges;
	for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
	{
		MarkingEdge((*l_itEdges));
	}

	return true;
}


bool
SP_DS_PlaceAnimator::MarkingTargetEdges()
{
	SP_ListEdge::const_iterator l_itEdges;
	for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
	{
		MarkingEdge((*l_itEdges));
	}

    return true;
}



bool
SP_DS_PlaceAnimator::TestMarking()
{
    if (!m_pcNode || !m_pcAttribute || (m_mlCandidates.size() == 0))
        return FALSE;

    // Added by Erik Jongsma, initialize the SimValue for later conflict resolution
    m_nSimValue = m_pcAttribute->GetValue();

    map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
	{
		// tell target animator to be enabled by me if we have enough markings
		if ((long)(SumEdges(&(l_Iter->second))) <= m_pcAttribute->GetValue())
		{
			l_Iter->first->Enable(this);
		}
	}

	return (m_mlCandidates.size() > 0);
}

bool
SP_DS_PlaceAnimator::ResolveConflicts()
{




	return TRUE;
	/* Old conflict resolving, bugged..
    map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
    while(m_pcAttribute->GetValue() <  (long) (SumCandidates()))
    {
        unsigned long l_nDelete = SP_RandomLong(m_mlCandidates.size());
        l_Iter = m_mlCandidates.begin();
        for(unsigned long i = 0; i < l_nDelete; i++)
            l_Iter++;
        l_Iter->first->Reset();
    }
    for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
        l_Iter->first->SetEnabled(TRUE);

    return TRUE;
    */
}

bool
SP_DS_PlaceAnimator::BuildConflictList(list<SP_DS_TransAnimator*> &p_lConflictTransAnimators)
{
	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator l_Iter;
	if (m_pcAttribute->GetValue() < (long)SumCandidates())
	{
		for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
		{
			p_lConflictTransAnimators.push_back(l_Iter->first);
		}
	}

	return true;
}

bool
SP_DS_PlaceAnimator::BuildAutoConflictList(list<SP_DS_TransAnimator*> &p_lConflictTransAnimators)
{
	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator l_Iter;
	if (m_mlCandidates.size() > 1)
		for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
		   	p_lConflictTransAnimators.push_back(l_Iter->first);

	return true;
}

bool
SP_DS_PlaceAnimator::DecrementSimValue(SP_DS_TransAnimator* p_pcTrans)
{
	map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first == p_pcTrans)
			m_nSimValue -= (long)(SumEdges(&(l_Iter->second)));
	}
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first != p_pcTrans)
			if ((long)(SumEdges(&(l_Iter->second))) > m_nSimValue)
				l_Iter->first->Disable(this);
	}
	return true;
}

bool
SP_DS_PlaceAnimator::DecrementSimValueAutoConcurrency(SP_DS_TransAnimator* p_pcTrans)
{
	map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first == p_pcTrans)
			m_nSimValue -= (long)(SumEdges(&(l_Iter->second)));
	}
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if ((long)(SumEdges(&(l_Iter->second))) > m_nSimValue)
			l_Iter->first->SetAutoEnabled(false);
	}
	return true;
}


bool
SP_DS_PlaceAnimator::ResolveConflicts1()
{

  map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;

  for( l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter ) {

    l_Iter->first->SetEnabled( TRUE );

  }

  return TRUE;

}

// animate
bool
SP_DS_PlaceAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAttribute || !m_pcAnimation)
    {
    	return FALSE;
    }
    if (!m_bEnabled || (m_mlCandidates.size() == 0))
    {
    	return FALSE;
    }

    m_nSteps = p_nSteps;
    SP_ListGraphic* l_plGraphic;
    SP_ListGraphic::iterator l_itGraphic;
    map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListEdge::const_iterator l_itEdge;
    SP_GR_MarkAnimator* l_pcAnim;
    bool l_bReturn = FALSE;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
    {
        if (!l_Iter->first->GetEnabled())
            continue;
        l_bReturn = TRUE;
        for (l_itEdge = l_Iter->second.begin(); l_itEdge != l_Iter->second.end(); ++l_itEdge)
        {
            l_plGraphic = (*l_itEdge)->GetGraphics();
            if (l_plGraphic)
            	for (SP_Graphic* l_pcGraphic : *l_plGraphic)
                {
                    l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
                    if (l_pcAnim && l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir))
                    {
                        AddGraphic(l_pcAnim);
                        m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
                    }
                    else
                        if (l_pcAnim)
                            wxDELETE(l_pcAnim);
                }
        }
    }

	DecrementMark();

    return l_bReturn;
}

bool
SP_DS_PlaceAnimator::Step(unsigned int p_nStep)
{
    if (!GetGraphics() || (p_nStep >= m_nSteps))
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
SP_DS_PlaceAnimator::PostStep()
{
    SP_ListGraphic* l_plGraphics = GetGraphics();

    // clean up temps
    m_mlCandidates.clear();
    m_bEnabled = FALSE;

    if (!l_plGraphics)
        return TRUE;

    SP_ListGraphic::iterator l_Iter;
    SP_GR_MarkAnimator* l_pcGr;
    for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); l_Iter = l_plGraphics->begin())
    {
    	SP_Graphic* l_pcGrIt = *l_Iter;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter);
        wxDELETE(l_pcGr);
    }

    return RemoveAllGraphics();
}

bool
SP_DS_PlaceAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

    m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);
    m_pcNode->Update(TRUE);

	m_pcParent->Update(TRUE);
	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

	MarkingSourceEdges();
	MarkingTargetEdges();

	return TRUE;
}

bool
SP_DS_PlaceAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

    if (m_pcAttribute->GetValue() > 0)
    {
        m_pcAttribute->SetValue(m_pcAttribute->GetValue() - 1);
        m_pcNode->Update(TRUE);
		SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
    }

    MarkingSourceEdges();
    MarkingTargetEdges();

    return TRUE;
}

bool
SP_DS_PlaceAnimator::Enable(SP_DS_TransAnimator* p_pcAnim)
{
    if (m_mlCandidates.find(p_pcAnim) != m_mlCandidates.end())
        m_bEnabled = TRUE;

    return TRUE;
}

bool
SP_DS_PlaceAnimator::DecrementMark()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

    long l_nTokens = m_pcAttribute->GetValue();
    long l_nSumCandidates = SumCandidates();
    m_pcAttribute->SetValue(l_nTokens - l_nSumCandidates);
    m_pcNode->Update(TRUE);

    return TRUE;
}

bool
SP_DS_PlaceAnimator::IncrementMark(SP_DS_Edge* p_pcEdge)
{

	// Added by Erik Jongsma
	long l_nMultiplicity = 0;
	SP_DS_TransAnimator* l_pcAnim;

    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);

	// Added by Erik Jongsma
    if (m_pcAnimation->GetDirection() == FORWARD) {
    	if (p_pcEdge->GetSource())
    	{
    		l_pcAnim = dynamic_cast<SP_DS_TransAnimator*>(m_pcAnimation->GetAnimator(p_pcEdge->GetSource(), SP_DS_ANIMATOR_TRANS));
    		if (l_pcAnim)
    		{
    			l_nMultiplicity = l_pcAnim->GetMultiplicity();
    		}
    	}
    } else {
    	if (p_pcEdge->GetTarget())
    	{
    		l_pcAnim = dynamic_cast<SP_DS_TransAnimator*>(m_pcAnimation->GetAnimator(p_pcEdge->GetTarget(), SP_DS_ANIMATOR_TRANS));
    		if (l_pcAnim)
    		{
    			l_nMultiplicity = l_pcAnim->GetMultiplicity();
    		}
    	}
    }

    if (l_nMultiplicity == 0)
    	return FALSE;

    // Added by Erik Jongsma: added multiplication with multiplicity
    m_pcAttribute->SetValue(m_pcAttribute->GetValue() + (l_nMultiplicity * SumEdge(p_pcEdge)));

    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_pcGraphic = m_pcNode->GetGraphics();
    if (l_pcGraphic)
    {
        for (l_Iter = l_pcGraphic->begin(); l_Iter != l_pcGraphic->end(); ++l_Iter)
            m_pcAnimation->AddCanvas((*l_Iter)->GetCanvas());
    }
    m_pcNode->Update(TRUE);

    return TRUE;
}

bool
SP_DS_PlaceAnimator::AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_TransAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcEdge, return FALSE);
    CHECK_POINTER(p_pcAnimator, return FALSE);

    m_mlCandidates[p_pcAnimator].push_back(p_pcEdge);
    return TRUE;
}

bool
SP_DS_PlaceAnimator::RemoveCandidate(SP_DS_TransAnimator* p_pcAnimator)
{
    if (m_mlCandidates.find(p_pcAnimator) != m_mlCandidates.end())
        m_mlCandidates.erase(p_pcAnimator);

    return TRUE;
}

bool
SP_DS_PlaceAnimator::ResetMarking()
{
    if (m_pcAttribute && m_pcNode)
    {
        m_pcAttribute->SetValue(m_nDefaultValue);
        m_pcNode->Update(TRUE);
    }

    return TRUE;
}

bool
SP_DS_PlaceAnimator::SetMarking()
{
    if (m_pcAttribute)
        m_nDefaultValue = m_pcAttribute->GetValue();

    return TRUE;
}

void
SP_DS_PlaceAnimator::SetMarking( long p_nMarking )
{

  if( m_pcAttribute && m_pcNode ) {

    m_pcAttribute->SetValue( p_nMarking );
    m_pcNode->Update( TRUE );

  }

}

void
SP_DS_PlaceAnimator::SetDefaultMarking( long p_nMarking )
{

  m_nDefaultValue = p_nMarking;

}

void
SP_DS_PlaceAnimator :: InitCanvas()
{

  SP_ListGraphic::iterator l_Iter;
  SP_ListGraphic* l_pcGraphic = m_pcNode->GetGraphics();

  if ( l_pcGraphic ) {

    for ( l_Iter = l_pcGraphic->begin(); l_Iter != l_pcGraphic->end(); ++l_Iter ) {

      m_pcAnimation->AddCanvas( ( *l_Iter )->GetCanvas() );

    }

  }

}

long
SP_DS_PlaceAnimator :: GetMarking()
{

  if( m_pcAttribute ) {

    return m_pcAttribute->GetValue();

  }

  return 0;

}
