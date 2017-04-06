//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_PLACEANIMATOR_H__
#define __SP_DS_PLACEANIMATOR_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

// added by sl
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"


class SP_DS_TransAnimator;

class SP_DS_PlaceAnimator: public SP_DS_Animator
{
private:
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    wxString m_sAttributeName;
    SP_DS_MarkingAttribute* m_pcAttribute;

    // special to places
    map<SP_DS_TransAnimator*, SP_ListEdge > m_mlCandidates;

    // Added by Erik Jongsma, needed for simulation when resolving conflicts
    long m_nSimValue;

    long m_nDefaultValue;
    virtual bool DecrementMark();
    unsigned int SumCandidates();
    unsigned int SumEdges(const SP_ListEdge* p_plEdges);
    virtual unsigned int SumEdge(SP_DS_Edge* p_pcEdge);

    //by sl
    virtual bool MarkingEdge(SP_DS_Edge* p_pcEdge);



public:
    SP_DS_PlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_PlaceAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    // special to places
    bool AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_TransAnimator* p_pcAnimator);
    bool RemoveCandidate(SP_DS_TransAnimator* p_pcAnimator);
    virtual bool TestMarking();

    virtual bool ResolveConflicts();
    bool ResolveConflicts1();

    virtual bool BuildConflictList (list<SP_DS_TransAnimator*> &p_lConflictTransAnimators);
    virtual bool BuildAutoConflictList (list<SP_DS_TransAnimator*> &p_lConflictTransAnimators);
    virtual bool DecrementSimValue (SP_DS_TransAnimator* p_pcTrans);
    virtual bool DecrementSimValueAutoConcurrency (SP_DS_TransAnimator* p_pcTrans);


    virtual bool ResetMarking();
    bool SetMarking();

    bool Enable(SP_DS_TransAnimator* p_pcAnim);

    virtual bool IncrementMark(SP_DS_Edge* p_pcEdge);

    SP_DS_Node* GetNode() { return m_pcNode; }


    void SetMarking( long p_nMarking );
    void SetDefaultMarking( long p_nMarking );

    void InitCanvas();

    long GetMarking();

    bool OnLeftClick();
    virtual bool OnRightClick();

    //by sl
    bool MarkingSourceEdges();
    bool MarkingTargetEdges();

    //by Avinash
    wxString GetAttributeName() { return m_sAttributeName; }

};

#endif // __SP_DS_PLACEANIMATOR_H__


