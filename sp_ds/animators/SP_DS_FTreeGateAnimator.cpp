//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/06/15 13:55:00 $
// Short Description: gate animator
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_FTreeGateAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/extensions/SP_DS_FTreePedAnimation.h"

SP_DS_FTreeGateAnimator::SP_DS_FTreeGateAnimator(SP_DS_Animation* p_pcAnim, SP_DS_Node* p_pcParent):
SP_DS_Animator(p_pcParent, p_pcAnim),
m_pcProtoshape(NULL)
{
    SetAnimatorType(SP_DS_ANIMATOR_GATE);

    if (p_pcParent)
    {
        m_pcNode = p_pcParent;
        m_pcProtoshape = new SP_GR_MarkAnimator();
	}
}

SP_DS_FTreeGateAnimator::~SP_DS_FTreeGateAnimator()
{

	if (m_pcProtoshape)
        wxDELETE(m_pcProtoshape);

    m_pcProtoshape = NULL;
}

SP_DS_Animator*
SP_DS_FTreeGateAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_DS_FTreeGateAnimator* l_pcNew = new SP_DS_FTreeGateAnimator(p_pcAnim, dynamic_cast<SP_DS_Node*>(p_pcParent));

    return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}

bool
SP_DS_FTreeGateAnimator::InformPreNodes()
{
    Reset();
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    // i inform all nodes at my incoming edges of wanting their markings
    SP_ListEdge::const_iterator l_itEdges;
    SP_DS_FTreeEventAnimator* l_pcAnim;
	if (m_pcAnimation->GetDirection() == FORWARD) {
		if (m_pcNode->GetTargetEdges()->size() == 0) {
			m_bEnabled = TRUE;
			return TRUE;
		}
		for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges){
			if ((*l_itEdges)->GetSource()){
				l_pcAnim = dynamic_cast<SP_DS_FTreeEventAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_EVENT));
				if (l_pcAnim){
					l_pcAnim->AddCandidate((*l_itEdges), this);
					m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));
				}
			}
		}
	}
    return TRUE;
}

bool
SP_DS_FTreeGateAnimator::TestConcession()
{

	if (!m_nodeColourizer->IsNodeRestore(m_pcNode)) {

	if (m_pcNode->GetClassName() == wxT("AND") ||
		m_pcNode->GetClassName()== wxT("COND"))
	{
		if (m_mlPrePlaces.size() <= m_lCandidates.size())
		{
			return TRUE;
		}
	}
	if (m_pcNode->GetClassName()== wxT("OR") ) {

		map<SP_DS_FTreeEventAnimator*, SP_ListEdge >::iterator l_Iter;
		for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter) {
			long mark = (l_Iter)->first->GetValue();
			if (mark != 0 ) {
				return TRUE;
			}
		}
	}
	if (m_pcNode->GetClassName()== wxT("XOR") ) {
		unsigned int e_Candidates = 0;
		map<SP_DS_FTreeEventAnimator*, SP_ListEdge >::iterator l_Iter;
		for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter) {
			long mark = (l_Iter)->first->GetValue();
			if (mark != 0 )
				e_Candidates++;
		}
		if (e_Candidates == 1) {
			return TRUE;
		}
	}

	if (m_pcNode->GetClassName()== wxT("M-OF-N") ) {
		long e_Candidates = 0;
		map<SP_DS_FTreeEventAnimator*, SP_ListEdge >::iterator l_Iter;
		for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter) {
			long mark = (l_Iter)->first->GetValue();
			if (mark != 0 )
				e_Candidates++;
		}
		wxString mValue = m_pcNode->GetAttribute(wxT("M"))->GetValueString();
		long m;
		mValue.ToLong(&m);
		if (e_Candidates >= m)
			return TRUE;
	}

	if (m_pcNode->GetClassName()== wxT("NEG") ) {
		const SP_ListEdge* edges = m_pcNode->GetTargetEdges();
		SP_ListEdge::const_iterator eIt;
		eIt = edges->begin();
		SP_DS_Node* sourceNode = (SP_DS_Node*)(*eIt)->GetSource();
		if (sourceNode->GetClassName()== wxT("Intermediate Event")){
			SP_DS_NumberAttribute* n_pcAttr = dynamic_cast<SP_DS_NumberAttribute*>
												(sourceNode->GetAttribute(wxT("Failure-Status")));
			if (n_pcAttr->GetValue() != 0)
				return TRUE;
		}else{
			if (sourceNode->GetClassName() !=  wxT("Basic Event")
				&& sourceNode->GetClassName() !=  wxT("Undeveloped Event")){
				if (m_nodeColourizer->IsNodeRestore(sourceNode))
					return TRUE;
			}

		}
		if (sourceNode->GetClassName()== wxT("Basic Event")
			|| sourceNode->GetClassName()== wxT("Undeveloped Event"))
			return TRUE;
		}
	}

    // no concession
    map<SP_DS_FTreeEventAnimator*, SP_ListEdge >::iterator l_Iter;
    for (l_Iter = m_mlPrePlaces.begin(); l_Iter != m_mlPrePlaces.end(); ++l_Iter)
        l_Iter->first->RemoveCandidate(this);

    return FALSE;
}

bool
SP_DS_FTreeGateAnimator::Reset()
{
    map<SP_DS_FTreeEventAnimator*, SP_ListEdge >::iterator l_Iter;
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

    for (l_Iter2 = l_plGraphics->begin(); l_Iter2 != l_plGraphics->end(); l_Iter2 = l_plGraphics->begin()){
    	SP_Graphic* l_pcGrIt = *l_Iter2;
        l_pcGr = dynamic_cast<SP_GR_MarkAnimator*>(l_pcGrIt);
        l_plGraphics->erase(l_Iter2);
        wxDELETE(l_pcGr);
    }
    return TRUE;
}

// animate
bool
SP_DS_FTreeGateAnimator::PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!m_pcNode || !m_pcAnimation)
        return FALSE;

    if (!m_bEnabled)
        return FALSE;

    list<SP_DS_FTreeEventAnimator*>::iterator l_Iter;
    for (l_Iter = m_lCandidates.begin(); l_Iter != m_lCandidates.end(); ++l_Iter) {
        (*l_Iter)->SetEnabled(TRUE);
	}

    m_nSteps = p_nSteps;
    SP_ListEdge::const_iterator l_itEdges;
	SP_ListEdge::const_iterator l_outEdges;
    SP_GR_MarkAnimator* l_pcAnim;
	SP_DS_NumberAttribute* m_pcAttribute;
	bool l_bReturn = false;

	if (p_nDir == FORWARD) {
		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges){

			if ((*l_itEdges)->GetGraphics()) {
				SP_DS_Node* targetNode = (SP_DS_Node*) (*l_itEdges)->GetTarget();

				for (SP_Graphic* l_pcGraphic : *(*l_itEdges)->GetGraphics()){
					l_pcAnim = dynamic_cast<SP_GR_MarkAnimator*>(m_pcProtoshape->Clone(this));

					if (targetNode->GetClassName()== wxT("Top Event")
						|| targetNode->GetClassName()== wxT("Intermediate Event")
						|| targetNode->GetClassName()== wxT("Coarse Event") ) {
						if (m_pcNode->GetClassName() !=  wxT("NEG")) {

							if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir)){
								AddGraphic(l_pcAnim);
								m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
							}

						}else{
							l_outEdges = m_pcNode->GetTargetEdges()->begin();

							SP_DS_Node* sourceNode = (SP_DS_Node*) (*l_outEdges)->GetSource();

							m_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
								(sourceNode->GetAttribute(wxT("Failure-Status")));

							if (m_pcAttribute->GetValue() == 0 ) {
								if (l_pcAnim->SetPath(l_pcGraphic, m_nSteps, p_nDir)){
									AddGraphic(l_pcAnim);
									m_pcAnimation->AddCanvas(l_pcAnim->GetCanvas());
									m_pcNode->Update(TRUE);
								}

							} else
								wxDELETE(l_pcAnim);
							m_nodeColourizer->RestoreColourNode(m_pcNode);
						}
					}else{

						//handle the token of a NEG-gate
						if (m_pcNode->GetClassName()== wxT("NEG")) {
							l_outEdges = m_pcNode->GetTargetEdges()->begin();

							SP_DS_Node* sourceNode = (SP_DS_Node*) (*l_outEdges)->GetSource();

							SP_DS_NumberAttribute* n_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
												(sourceNode->GetAttribute(wxT("Failure-Status")));
							m_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
												(m_pcNode->GetAttribute(wxT("Failure-Status")));
							if (n_pcAttribute->GetValue() == 0 && m_pcAttribute->GetValue() == 0) {
								m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);
								m_pcNode->Update(TRUE);
							}
							m_nodeColourizer->RestoreColourNode(m_pcNode);

						}else{
							m_pcAttribute = dynamic_cast<SP_DS_NumberAttribute*>
									(m_pcNode->GetAttribute(wxT("Failure-Status")));
							if (m_pcAttribute->GetValue() == 0 )
								m_pcAttribute->SetValue(m_pcAttribute->GetValue() + 1);
								m_nodeColourizer->RestoreColourNode(m_pcNode);
							}

						}//if

					}//ofr
				}//if
			}//ofr

			l_bReturn = m_pcNode->GetSourceEdges()->size() == 0;
		}

    return (l_bReturn || (GetGraphics() && (GetGraphics()->size() > 0)));
}

bool
SP_DS_FTreeGateAnimator::Step(unsigned int p_nStep)
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
SP_DS_FTreeGateAnimator::PostStep()
{
    SP_ListEdge::const_iterator l_itEdges;
	SP_ListEdge::const_iterator l_outEdges;
    SP_DS_FTreeEventAnimator* l_pcAnim;

    Reset();
	if (m_pcAnimation->GetDirection() == FORWARD) {

		for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges){
			if ((*l_itEdges)->GetTarget()){
				l_pcAnim = dynamic_cast<SP_DS_FTreeEventAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_EVENT));
				if (l_pcAnim) {
					l_pcAnim->IncrementMark((*l_itEdges));

					//checks, if the combination of basic events/undeveloped events can cause the top event
					if (l_pcAnim->GetNode()->GetClassName()== wxT("Top Event"))
						SP_MESSAGEBOX(wxT("The combination of basic events can cause the top event."),
						wxT("Notification"), wxOK | wxICON_INFORMATION );
				}
			}
		}
	}
	m_nodeColourizer->RestoreColourNode(m_pcNode);
    return RemoveAllGraphics();
}

bool
SP_DS_FTreeGateAnimator::OnLeftClick()
{
    CHECK_POINTER(m_pcNode, return FALSE);
    CHECK_POINTER(m_pcAnimation, return FALSE);
    SP_DS_FTreePedAnimation* l_pcAnim = dynamic_cast<SP_DS_FTreePedAnimation*>(m_pcAnimation);

	l_pcAnim->SetDirection(FORWARD);
    l_pcAnim->SetSingleStep(this);
    l_pcAnim->SetOneShot(TRUE);
    l_pcAnim->StartTimer();

    return TRUE;
}

void
SP_DS_FTreeGateAnimator::Enable(SP_DS_FTreeEventAnimator* p_pcAnimator)
{
    CHECK_POINTER(p_pcAnimator, return);
    m_lCandidates.push_back(p_pcAnimator);
}

void
SP_DS_FTreeGateAnimator::SetNodeColourizer(SP_FTreeNodeColourizer* m_nodeColor)
{
	m_nodeColourizer = m_nodeColor;
}

