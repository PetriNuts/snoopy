//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/06/15 13:43:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEEVENTANIMATOR_H__
#define __SP_DS_FTREEEVENTANIMATOR_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

class SP_DS_FTreeGateAnimator;

class SP_DS_FTreeEventAnimator: public SP_DS_Animator
{
private:
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    wxString m_sAttributeName;
    SP_DS_NumberAttribute* m_pcAttribute;

    // special to places
    map<SP_DS_FTreeGateAnimator*, SP_ListEdge > m_mlCandidates;

    long m_nDefaultValue;

    unsigned int SumCandidates();
    unsigned int SumEdges(const SP_ListEdge* p_plEdges);


public:
    SP_DS_FTreeEventAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_FTreeEventAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    // special to places
    bool AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_FTreeGateAnimator* p_pcAnimator);
    bool RemoveCandidate(SP_DS_FTreeGateAnimator* p_pcAnimator);
    virtual bool TestMarking();
    bool SetEnable();
    bool ResetMarking();
    bool SetMarking();

    bool Enable(SP_DS_FTreeGateAnimator* p_pcAnim);

    bool IncrementMark(SP_DS_Edge* p_pcEdge);

    bool OnLeftClick();
    bool OnRightClick();

	SP_DS_Node* GetNode() const { return m_pcNode; }

	long GetValue() const { return m_pcAttribute->GetValue(); }


};

#endif // __SP_DS_FTREEEVENTANIMATOR_H__


