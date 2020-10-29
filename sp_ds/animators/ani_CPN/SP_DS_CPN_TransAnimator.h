//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_TRANSANIMATOR_H__
#define __SP_DS_TRANSANIMATOR_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_Binding.h"

class SP_DS_CPN_PlaceAnimator;

class SP_DS_CPN_TransAnimator: public SP_DS_Animator
{
protected:
	//special to colored petri nets	
	vector<SP_CPN_ExpressionInfo> m_ExprInfoList;
	SP_CPN_ExpressionInfo m_cExprInfo;
	SP_DS_ColListAttribute* m_pcColList;
	bool m_bSingleClick;
	int m_nBindingChoice;

	//newly added
	map<wxString, map<SP_DS_Edge*, map<wxString, int> > > m_mmmBind2Edge2Mult2Color;
	vector<wxString> m_vsSelBindings;

protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    list<SP_DS_CPN_PlaceAnimator*> m_lCandidates;
    map<SP_DS_CPN_PlaceAnimator*, SP_ListEdge > m_mlPrePlaces;

public:
    SP_DS_CPN_TransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_CPN_TransAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);
    // special to transitions
    virtual bool InformPrePlaces(const wxString& p_sColor=wxT(""));
    virtual SP_IMPORT_STATE CheckColor(const wxString& p_sColor);//by george
    void Enable(SP_DS_CPN_PlaceAnimator* p_pcAnimator);
    bool TestConcession();
    bool Reset();

    virtual bool OnLeftClick();
	virtual bool OnRightClick();
	void ResetSingleClick(bool p_bSingleClick){m_bSingleClick = p_bSingleClick;}

	SP_DS_Node* GetParentNode(){return m_pcNode;}

	map<wxString, map<SP_DS_Edge*, map<wxString, int> > >* GetBind2Edge2Mult2Color(){ return &m_mmmBind2Edge2Mult2Color;}

	void SetSelBindings(vector<wxString> p_vsSelBindings ) 
	{
		m_vsSelBindings.clear();
		m_vsSelBindings = p_vsSelBindings;
	}

	vector<wxString>* GetSelBindings() { return &m_vsSelBindings; }
	
};

#endif // __SP_DS_CPN_TransAnimator_H__


