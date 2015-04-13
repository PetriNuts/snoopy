//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored PN coarse transiton class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColStCoarseTransAnimator_H__
#define __SP_DS_ColStCoarseTransAnimator_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

class SP_DS_ColStPlaceAnimator;

class SP_DS_ColStCoarseTransAnimator: public SP_DS_Animator
{
private:
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    list<SP_DS_ColStPlaceAnimator*> m_lCandidates;
    map<SP_DS_ColStPlaceAnimator*, SP_ListEdge > m_mlPrePlaces;
public:
    SP_DS_ColStCoarseTransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_ColStCoarseTransAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir){ return true; }
    bool Step(unsigned int p_nStep){ return true; }
    bool PostStep(){ return true; }

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    // special to transitions
    bool InformPrePlaces();
    void Enable(SP_DS_ColStPlaceAnimator* p_pcAnimator);
    bool TestConcession();
    bool Reset();

    bool OnLeftClick();
	bool OnRightClick();
};

#endif // __SP_DS_ColStCoarseTransAnimator_H__


