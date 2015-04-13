//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/29 $
// Short Description: place animator for Extended PT nets
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"

SP_DS_ExtPTPlaceAnimator::SP_DS_ExtPTPlaceAnimator(const wxString& p_pchAttributeName,
						   SP_DS_Animation* p_pcAnim,
						   SP_DS_Node* p_pcParent)
  : SP_DS_PlaceAnimator(p_pchAttributeName, p_pcAnim, p_pcParent)
{
}

SP_DS_ExtPTPlaceAnimator::~SP_DS_ExtPTPlaceAnimator()
{
}

SP_DS_Animator*
SP_DS_ExtPTPlaceAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_DS_ExtPTPlaceAnimator* l_pcNew = new SP_DS_ExtPTPlaceAnimator(m_sAttributeName, p_pcAnim, dynamic_cast<SP_DS_Node*> (p_pcParent));

	return l_pcNew;
}

unsigned int SP_DS_ExtPTPlaceAnimator::SumEdge(SP_DS_Edge* p_edge)
{
	if (!p_edge)
		return 0;
	if (p_edge->GetClassName() == wxT("Edge"))
	{
	    unsigned int l_nRet = 0;
	    SP_DS_Attribute* l_pcAttr;

	    l_pcAttr = p_edge->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
	    if (l_pcAttr)
	    	l_nRet += dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(l_pcAttr)->GetValue();

	    return l_nRet;
	}
	return 0;
}

/*
 * set new edge value
 */
bool
SP_DS_ExtPTPlaceAnimator::MarkingEdge(SP_DS_Edge* p_pcEdge)
{
    if (!p_pcEdge)
        return false;

   SP_DS_Attribute* l_pcAttr;

   l_pcAttr = p_pcEdge->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
    if (l_pcAttr)
    {
    	dynamic_cast<SP_DS_MarkingDependentMultiplicity*>(l_pcAttr)->GetValue(true);
    }

    //update the edge
    p_pcEdge->Update();

    return true;
}

bool SP_DS_ExtPTPlaceAnimator::TestMarking(const SP_ListEdge* p_edges, long p_tokens)
{
	if (!p_edges)
		return false;

    // Added by Erik Jongsma, initialize the SimValue for later conflict resolution
    m_nSimValue = m_pcAttribute->GetValue();

	SP_DS_Attribute* attr;
	SP_ListEdge::const_iterator eIt;
	long tokensMin = 0;
	long tokensRead = 0;
	long tokensReq = 0;
	bool inhibitor = false;
	bool equaledge = false;

	for (eIt = p_edges->begin(); eIt != p_edges->end(); ++eIt)
	{
		attr = (*eIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
	    CHECK_POINTER (attr, continue);

		if ((*eIt)->GetClassName() == wxT("Edge"))
		{
			tokensMin += dynamic_cast<SP_DS_MarkingDependentMultiplicity*> (attr)->GetValue();
		}
		else if ((*eIt)->GetClassName() == wxT("Read Edge"))
		{
			tokensRead = dynamic_cast<SP_DS_MarkingDependentMultiplicity*> (attr)->GetValue();
		}
		else if ((*eIt)->GetClassName() == wxT("Inhibitor Edge"))
		{
			tokensReq = dynamic_cast<SP_DS_MarkingDependentMultiplicity*> (attr)->GetValue();
			inhibitor = true;
		}
		else if ((*eIt)->GetClassName() == wxT("Equal Edge"))
		{
			tokensReq = dynamic_cast<SP_DS_MarkingDependentMultiplicity*> (attr)->GetValue();
			equaledge = true;
		}
		else
		{
			continue;
		}
	}
	CHECK_BOOL (! (inhibitor && equaledge), return false);

	if (inhibitor)
	{
		return (p_tokens < tokensReq && tokensMin <= p_tokens && tokensRead <= p_tokens);
	}

	if (equaledge)
		return (p_tokens == tokensReq && tokensMin <= p_tokens && tokensRead <= p_tokens);

	return (tokensMin <= p_tokens && tokensRead <= p_tokens);
}


bool SP_DS_ExtPTPlaceAnimator::TestMarking()
{
	if (!m_pcNode || !m_pcAttribute || (m_mlCandidates.size() == 0))
		return FALSE;

	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
	{
		if (TestMarking(&(l_Iter->second), m_pcAttribute->GetValue()))
			l_Iter->first->Enable(this);
	}
	return (m_mlCandidates.size() > 0);
}

bool
SP_DS_ExtPTPlaceAnimator::ResolveConflicts()
{
    map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	list<SP_DS_TransAnimator*> l_aPossibleConflict;
    for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); l_Iter++)
    {
    	SP_DS_TransAnimator* l_pcTA = l_Iter->first;
    	SP_ListEdge* l_plEdges = &(l_Iter->second);
    	for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
    	{
    		if( (*l_itE)->GetClassName() == wxT("Edge"))
    		{
				l_aPossibleConflict.push_back(l_pcTA);
				break;
    		}
    	}
    }
	while(!l_aPossibleConflict.empty() && m_pcAttribute->GetValue() < (long) (SumCandidates()))
    {
		unsigned long l_nDelete = SP_RandomLong(l_aPossibleConflict.size());
		list<SP_DS_TransAnimator*>::iterator l_itC = l_aPossibleConflict.begin();
        for (unsigned long i = 0; i < l_nDelete; i++)
		{
			l_itC++;
		}
		//Remove from m_mlCandidates and disable Transition
        (*l_itC)->Reset();
    }
    for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
        l_Iter->first->SetEnabled(TRUE);

    return TRUE;
}

bool
SP_DS_ExtPTPlaceAnimator::BuildConflictList(list<SP_DS_TransAnimator*> &p_lConflictTransAnimators)
{
	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator l_Iter;
	list<SP_DS_TransAnimator*> l_aPossibleConflict;
	for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); l_Iter++)
	{
	   	SP_DS_TransAnimator* l_pcTA = l_Iter->first;
	   	SP_ListEdge* l_plEdges = &(l_Iter->second);
	   	for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
	   	{
	   		if( (*l_itE)->GetClassName() == wxT("Edge"))
	   		{
				l_aPossibleConflict.push_back(l_pcTA);
				break;
	   		}
	   	}
	}
	if (m_pcAttribute->GetValue() < (long)SumCandidates())
		for(list<SP_DS_TransAnimator*>::iterator l_itC = l_aPossibleConflict.begin(); l_itC != l_aPossibleConflict.end(); ++l_itC)
		   	p_lConflictTransAnimators.push_back((*l_itC));
	else
	    for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
	    	l_Iter->first->SetEnabled(true);

	return true;
}

bool
SP_DS_ExtPTPlaceAnimator::BuildAutoConflictList(list<SP_DS_TransAnimator*> &p_lConflictTransAnimators)
{
	int p_nCount = 0;
	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator l_Iter;
	for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); l_Iter++)
	{
	   	SP_ListEdge* l_plEdges = &(l_Iter->second);
	   	for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
	   	{
	   		if( (*l_itE)->GetClassName() == wxT("Edge"))
	   		{
				p_nCount++;
				break;
	   		}
	   	}
	}

	if (p_nCount > 1) {
		for(l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); l_Iter++)
		{
			SP_DS_TransAnimator* l_pcTA = l_Iter->first;
		   	SP_ListEdge* l_plEdges = &(l_Iter->second);
		   	for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
		   	{
		   		if( (*l_itE)->GetClassName() == wxT("Edge"))
		   		{
					p_lConflictTransAnimators.push_back(l_pcTA);
					break;
		   		}
		   	}
		}
	}
	return true;
}

bool
SP_DS_ExtPTPlaceAnimator::DecrementSimValue(SP_DS_TransAnimator* p_pcTrans)
{
	map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first == p_pcTrans)
		{
	    	SP_ListEdge* l_plEdges = &(l_Iter->second);
			for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
		   		if( (*l_itE)->GetClassName() == wxT("Edge"))
		   			m_nSimValue -= (long)(SumEdges(&(l_Iter->second)));
		}
	}
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first != p_pcTrans) {
			SP_ListEdge* l_plEdges = &(l_Iter->second);
			for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
				if( (*l_itE)->GetClassName() == wxT("Edge"))
					if ((long)(SumEdges(&(l_Iter->second))) > m_nSimValue)
						l_Iter->first->Disable(this);
		}
	}
	return true;
}

bool
SP_DS_ExtPTPlaceAnimator::DecrementSimValueAutoConcurrency(SP_DS_TransAnimator* p_pcTrans)
{
	map<SP_DS_TransAnimator*, SP_ListEdge >::iterator l_Iter;
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
		if (l_Iter->first == p_pcTrans)
		{
	    	SP_ListEdge* l_plEdges = &(l_Iter->second);
			for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
		   		if( (*l_itE)->GetClassName() == wxT("Edge"))
		   			m_nSimValue -= (long)(SumEdges(&(l_Iter->second)));
		}
	}
	for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
			SP_ListEdge* l_plEdges = &(l_Iter->second);
			for(SP_ListEdge::const_iterator l_itE = l_plEdges->begin(); l_itE != l_plEdges->end(); l_itE++)
				if( (*l_itE)->GetClassName() == wxT("Edge"))
					if ((long)(SumEdges(&(l_Iter->second))) > m_nSimValue)
						l_Iter->first->SetAutoEnabled(false);
	}
	return true;
}

long SP_DS_ExtPTPlaceAnimator::DecrementedMarking(const SP_ListEdge* p_edges, long p_tokens, SP_DS_TransAnimator* p_pcTrans)
{
	if (!p_edges)
		return p_tokens;

	SP_DS_Attribute* attr;
	SP_ListEdge::const_iterator eIt;

	for (eIt = p_edges->begin(); eIt != p_edges->end(); ++eIt)
	{
		if ((*eIt)->GetClassName() == wxT("Edge"))
		{
			attr = (*eIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
			if (attr)
			{
				p_tokens -= (dynamic_cast<SP_DS_MarkingDependentMultiplicity*> (attr)->GetValue() * p_pcTrans->GetMultiplicity());
			}
		}
		else if ((*eIt)->GetClassName() == wxT("Reset Edge"))
		{
			return 0;
		}
		else
		{
			continue;
		}
	}
	return p_tokens;
}

bool SP_DS_ExtPTPlaceAnimator::DecrementMark()
{
	CHECK_POINTER(m_pcAttribute, return FALSE);
	CHECK_POINTER(m_pcNode, return FALSE);

	long val = m_pcAttribute->GetValue();

	map<SP_DS_TransAnimator*, SP_ListEdge>::iterator it;
	for (it = m_mlCandidates.begin(); it != m_mlCandidates.end(); ++it)
	{
		val = DecrementedMarking(&((*it).second), val, (*it).first);
		if (val == 0)
			break;
	}

	m_pcAttribute->SetValue(val);
	m_pcNode->Update(TRUE);

	return TRUE;
}

bool
SP_DS_ExtPTPlaceAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
	return SP_DS_PlaceAnimator::PreStep(p_nSteps, p_nDir);

}



bool
SP_DS_ExtPTPlaceAnimator::OnRightClick()
{
	return SP_DS_PlaceAnimator::OnRightClick();
}

