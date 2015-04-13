//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author(modification): akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEGATEANIMATOR_H__
#define __SP_DS_FTREEGATEANIMATOR_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_gr/shapes/SP_GR_DrawnShape.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_core/tools/SP_FTreeNodeColourizer.h"


class SP_DS_FTreeEventAnimator;

class SP_DS_FTreeGateAnimator: public SP_DS_Animator
{
private:

	
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

	 SP_GR_BaseDrawn* act_pcPrimitive;

    SP_DS_Node* m_pcNode;
    list<SP_DS_FTreeEventAnimator*> m_lCandidates;
    map<SP_DS_FTreeEventAnimator*, SP_ListEdge > m_mlPrePlaces;
	SP_FTreeNodeColourizer* m_nodeColourizer;
	
	 

public:
    SP_DS_FTreeGateAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_FTreeGateAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);
    // special to transitions
    bool InformPreNodes();
    void Enable(SP_DS_FTreeEventAnimator* p_pcAnimator);
    bool TestConcession();
    bool Reset();

    bool OnLeftClick();

	SP_DS_Node* GetNode() const { return m_pcNode; }

	void SetNodeColourizer(SP_FTreeNodeColourizer* m_nodeColor);
};

#endif // __SP_DS_FTREEGATEANIMATOR_H__


