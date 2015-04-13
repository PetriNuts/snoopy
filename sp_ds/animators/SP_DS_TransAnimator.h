//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_TRANSANIMATOR_H__
#define __SP_DS_TRANSANIMATOR_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

class SP_DS_PlaceAnimator;

class SP_DS_TransAnimator: public SP_DS_Animator
{
private:
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    list<SP_DS_PlaceAnimator*> m_lCandidates;
    map<SP_DS_PlaceAnimator*, SP_ListEdge > m_mlPrePlaces;

	long m_nMultiplicity;
	bool m_bAutoEnabled;
public:
    SP_DS_TransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_TransAnimator();


    //virtuell ergänzt
    virtual bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    virtual bool Step(unsigned int p_nStep);
    virtual bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);
    // special to transitions
    virtual bool InformPrePlaces();
    virtual void Enable(SP_DS_PlaceAnimator* p_pcAnimator);
    virtual void Disable(SP_DS_PlaceAnimator* p_pcAnimator);

    virtual bool TestConcession();
    virtual bool Reset();

    virtual bool SimulateFiring(bool p_bAutoConcurrency);

    virtual bool OnLeftClick();
    virtual bool OnRightClick();

	// Added by Erik Jongsma
    virtual bool IncreaseMultiplicity();
    virtual long GetMultiplicity() { return m_nMultiplicity; }
    virtual void CalculateMultiplicity();
    virtual void SetAutoEnabled(bool p_bValue);
    virtual bool IsAutoEnabled() { return m_bAutoEnabled; }
    virtual bool HasRealPrePlace();

    SP_DS_Node* GetNode() { return m_pcNode; }

};

#endif // __SP_DS_TRANSANIMATOR_H__


