//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColStTransAnimator__
#define __SP_DS_ColStTransAnimator__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"



class SP_DS_ColStPlaceAnimator;

class SP_DS_ColStTransAnimator: public SP_DS_Animator
{
protected:
	//special to colored petri nets

	//SP_DS_Animation* m_pcAnimation;

	SP_DS_ColListAttribute* m_pcColList;

	bool m_bSingleClick;

	int m_nBindingChoice;

protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    list<SP_DS_ColStPlaceAnimator*> m_lCandidates;
    map<SP_DS_ColStPlaceAnimator*, SP_ListEdge > m_mlPrePlaces;	

private:
	SP_VectorString m_vChosenBinding;

public:
    SP_DS_ColStTransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_ColStTransAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);
    // special to transitions
    virtual bool InformPrePlaces();
    void Enable(SP_DS_ColStPlaceAnimator* p_pcAnimator);
    bool TestConcession();
    bool Reset();

    bool InformStPrePlaces(const wxString& p_sColor=wxT("") );
    bool CheckColour(const wxString& p_sColor);

    virtual bool OnLeftClick();
	virtual bool OnRightClick();
	void ResetSingleClick(bool p_bSingleClick){m_bSingleClick = p_bSingleClick;}

	SP_DS_Node* GetParentNode(){return m_pcNode;}
	
	SP_VectorString GetChosenBinding() { return m_vChosenBinding; }//by george

};

#endif // __SP_DS_ColStTransAnimator_H__


