//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/06/15 13:49:00 $
// Short Description: event animator
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeGateAnimator.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


SP_DS_FTreeEventAnimator::SP_DS_FTreeEventAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL),
m_pcNode(NULL),
m_sAttributeName(p_pchAttributeName),
m_pcAttribute(NULL),
m_nDefaultValue(0)
{
    SetAnimatorType(SP_DS_ANIMATOR_EVENT);

    if (p_pcParent){
        m_pcNode = p_pcParent;
        m_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>(p_pcParent->GetAttribute(p_pchAttributeName));
        if (m_pcAttribute)
            m_nDefaultValue = m_pcAttribute->GetValue();
        m_pcProtoshape = new SP_GR_MarkAnimator();
    }
}

SP_DS_FTreeEventAnimator::~SP_DS_FTreeEventAnimator()
{
    if (m_pcNode){
  	try{
		SP_DS_Attribute* l_pcAttr = m_pcNode->GetAttribute(wxT("Propability of Failure (%)"));
		if(l_pcAttr) l_pcAttr->SetShow(FALSE);
	}
	catch (...)
    {
        m_sMessage << wxT("An error occurred by setting Attribute") << wxT("\n");

    }
	}
	if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_FTreeEventAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_FTreeEventAnimator* l_pcNew = new SP_DS_FTreeEventAnimator(m_sAttributeName, p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

unsigned int
SP_DS_FTreeEventAnimator::SumCandidates()
{
    unsigned int l_nRet = 0;
    map<SP_DS_FTreeGateAnimator*, SP_ListEdge >::iterator l_Iter;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
        l_nRet += SumEdges(&((*l_Iter).second));

    return l_nRet;
}

unsigned int
SP_DS_FTreeEventAnimator::SumEdges(const SP_ListEdge* p_plEdges)
{
    if (!p_plEdges)
        return 0;

    unsigned int l_nRet = 0;
    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = p_plEdges->begin(); l_itEdge != p_plEdges->end(); ++l_itEdge)
		l_nRet += 1;

    return l_nRet;
}

bool
SP_DS_FTreeEventAnimator::TestMarking()
{
    if (!m_pcNode || !m_pcAttribute || (m_mlCandidates.size() == 0))
        return FALSE;


		map<SP_DS_FTreeGateAnimator*, SP_ListEdge >::iterator l_Iter;

		for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter) {
			SP_DS_Node* gate = l_Iter->first->GetNode();


			if (gate->GetClassName() == wxT("AND")
				|| gate->GetClassName() ==  wxT("COND")
				|| gate->GetClassName() ==  wxT("OR")) {
			// tell target animator to be enabled by me if we have enough markings
				if ((long)(SumEdges(&(l_Iter->second))) <= m_pcAttribute->GetValue())
					l_Iter->first->Enable(this);
			}

			//NEG-gate is always ready to switch
			if (gate->GetClassName()== wxT("NEG"))
			// tell target animator to be enabled by me if we have enough markings
				l_Iter->first->Enable(this);

			//first of all, the gate is set of wxT("Enable").
			if (gate->GetClassName() == wxT("XOR") ||
				gate->GetClassName()== wxT("M-OF-N"))
					l_Iter->first->Enable(this);
		}

		return (m_mlCandidates.size() > 0);
}

bool
SP_DS_FTreeEventAnimator::SetEnable()
{

    map<SP_DS_FTreeGateAnimator*, SP_ListEdge >::iterator l_Iter;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter)
        l_Iter->first->SetEnabled(TRUE);

    return TRUE;
}

// animate
bool
SP_DS_FTreeEventAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{

	if (!m_bEnabled || !m_pcNode || !m_pcAttribute || !m_pcAnimation ||
		(m_mlCandidates.size() == 0)){
		return FALSE;
	}else{
		const SP_ListEdge* edges = m_pcNode->GetSourceEdges();
		SP_ListEdge::const_iterator eIt;
		bool hasNeg = FALSE;
		for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
			SP_DS_Node* targetNode = (SP_DS_Node*) (*eIt)->GetTarget();
			if (targetNode->GetClassName()== wxT("NEG"))
				hasNeg = TRUE;

		}
		if (!hasNeg)
			if (m_pcAttribute->GetValue() == 0)
					return FALSE;
	}
	SP_ListEdge::const_iterator eIt;

    m_nSteps = p_nSteps;
    SP_ListGraphic* l_plGraphic;
    map<SP_DS_FTreeGateAnimator*, SP_ListEdge >::iterator l_Iter;
    SP_ListEdge::const_iterator l_itEdge;
    SP_GR_MarkAnimator* l_pcAnim;
    bool l_bReturn = FALSE;

    for (l_Iter = m_mlCandidates.begin(); l_Iter != m_mlCandidates.end(); ++l_Iter){
        if (!l_Iter->first->GetEnabled())
            continue;
        l_bReturn = TRUE;
        for (l_itEdge = l_Iter->second.begin(); l_itEdge != l_Iter->second.end(); ++l_itEdge){
            l_plGraphic = (*l_itEdge)->GetGraphics();
            if (l_plGraphic)
            	for (SP_Graphic* l_pcGraphic : *l_plGraphic){
                    l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));
                    if (l_pcAnim && l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir)){
                        AddGraphic(l_pcAnim);
                        m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
                    }
                    else
                        if (l_pcAnim)
                            wxDELETE(l_pcAnim);
                }
		}
    }
    return l_bReturn;
}

bool
SP_DS_FTreeEventAnimator::Step(unsigned int p_nStep)
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
SP_DS_FTreeEventAnimator::PostStep()
{
    SP_ListGraphic* l_plGraphics = GetGraphics();
    // clean up temps
    m_mlCandidates.clear();
    m_bEnabled = FALSE;

    if (!l_plGraphics)
        return TRUE;

    SP_ListGraphic::iterator l_Iter;
    SP_GR_MarkAnimator* l_pcGr;
    for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); l_Iter = l_plGraphics->begin()){
    	SP_Graphic* l_pcGrIt = *l_Iter;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter);
        wxDELETE(l_pcGr);
    }

    return RemoveAllGraphics();
}

bool
SP_DS_FTreeEventAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

	if (m_pcNode->GetClassName()== wxT("Basic Event")
		|| m_pcNode->GetClassName()== wxT("Top Event")
		|| m_pcNode->GetClassName()== wxT("Undeveloped Event")
		|| m_pcNode->GetClassName()== wxT("Intermediate Event")
		|| m_pcNode->GetClassName()== wxT("Coarse Event")
		|| m_pcNode->GetClassName()== wxT("COND-Parameter")) {
		if (m_pcAttribute->GetValue() != 1) {
			m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);
			m_pcNode->Update(TRUE);
			SP_Core::Instance()->GetRootDocument()->Modify(TRUE);


		}
	}

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::OnRightClick()
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);

    if (m_pcNode->GetClassName()== wxT("Basic Event")
		|| m_pcNode->GetClassName()== wxT("Top Event")
		|| m_pcNode->GetClassName()== wxT("Undeveloped Event")
		|| m_pcNode->GetClassName()== wxT("Intermediate Event")
		|| m_pcNode->GetClassName()== wxT("Coarse Event")
		|| m_pcNode->GetClassName()== wxT("COND-Parameter")) {
		if (m_pcAttribute->GetValue() > 0){
			m_pcAttribute->SetValue(m_pcAttribute->GetValue() - 1);
			m_pcNode->Update(TRUE);
			SP_Core::Instance()->GetRootDocument()->Modify(TRUE);


		}
	}

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::Enable(SP_DS_FTreeGateAnimator* p_pcAnim)
{
    if (m_mlCandidates.find(p_pcAnim) != m_mlCandidates.end())
        m_bEnabled = TRUE;

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::IncrementMark(SP_DS_Edge* p_pcEdge)
{
    CHECK_POINTER(m_pcAttribute, return FALSE);
    CHECK_POINTER(m_pcNode, return FALSE);


	if (m_pcAttribute->GetValue() == 0 ) {
		m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);

		SP_ListGraphic::iterator l_Iter;
		SP_ListGraphic* l_pcGraphic = m_pcNode->GetGraphics();
		if (l_pcGraphic){
			for (l_Iter = l_pcGraphic->begin(); l_Iter != l_pcGraphic->end(); ++l_Iter)
				m_pcAnimation->AddCanvas((*l_Iter)->GetCanvas());
		}
		m_pcNode->Update(TRUE);
	}

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_FTreeGateAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcEdge, return FALSE);
    CHECK_POINTER(p_pcAnimator, return FALSE);

    m_mlCandidates[p_pcAnimator].push_back(p_pcEdge);
    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::RemoveCandidate(SP_DS_FTreeGateAnimator* p_pcAnimator)
{
    if (m_mlCandidates.find(p_pcAnimator) != m_mlCandidates.end())
        m_mlCandidates.erase(p_pcAnimator);

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::ResetMarking()
{
    if (m_pcAttribute && m_pcNode)
    {
        m_pcAttribute->SetValue(m_nDefaultValue);

		if(  m_pcNode->GetClassName()== wxT("Basic Event") ||
			(m_pcNode->GetClassName()== wxT("Undeveloped Event") && m_pcAttribute->GetValue() != 0))
		{
			m_pcNode->GetAttribute(wxT("Repairable System - Unavailability"))->SetShow(FALSE);
		}
		m_pcNode->Update(TRUE);
    }

    return TRUE;
}

bool
SP_DS_FTreeEventAnimator::SetMarking()
{
    if (m_pcAttribute)
        m_nDefaultValue = m_pcAttribute->GetValue();

    return TRUE;
}

