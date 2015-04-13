//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/06/12 11:55:00 $
// Short Description: ped animation of fault trees
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEPEDANIMATION_H__
#define __SP_DS_FTREEPEDANIMATION_H__

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeRepairableSystemAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeIrreparableSystemAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCutSetsCreater.h"
#include "sp_gui/dialogs/SP_DLG_ChooseFTreeCutSet.h"
#include "sp_core/tools/SP_FTreeNodeColourizer.h"


class SP_Graphic;


class SP_DS_FTreePedAnimation: public SP_DS_Animation
{
private:
    DECLARE_EVENT_TABLE()
    unsigned int m_nStepState;
	unsigned int m_SystemType;
	unsigned int m_AnalysisType;
    wxRadioBox* m_pcChoice;
	wxRadioBox* m_pcAnalysisChoice;
	wxRadioBox* m_pcSystemType;
	wxButton* m_showCutSets;

	SP_DLG_ChooseFTreeCutSet* m_ChooseCutSetDialog;

	wxColourPickerCtrl* m_pcButtonColourSim;

	wxCheckBox* m_pcBoolCtrl;

    SP_DS_FTreeGateAnimator* m_pcSingleStep;
	SP_DS_FTreeIrreparableSystemAnalyser* ftAnalysisIrreparable;
	SP_DS_FTreeRepairableSystemAnalyser* ftAnalysisRepairable;

	list<list<SP_DS_FTreeGateAnimator*>*> m_llHistoryGateAnimators;

	SP_DLG_Animation* m_pcDialog;
	bool haveNoValues;

	NodeMap logExpression;
	LiteralMap vMap;

	SP_DS_FTreeCutSetsCreater* pDNF;
	SP_DS_Graph* m_pcGraph;
	

protected:
	
	/**
	\brief Check of NAND- or NOR-functions

	Combinations of gates which represent a NAND- or NOR-function
	cannot be still represented correctly. It has to be use de Morgan
	rules.
	
	\returns	TRUE, FALSE
	*/
	bool IsAnimationExecutable();
	
	bool PreStep();
    bool Step();
    bool PostStep();

    // special to ped
    bool ReduceGates();

    list<SP_DS_FTreeEventAnimator*> m_lAllNodeAnimators;
    list<SP_DS_FTreeGateAnimator*> m_lAllGateAnimators;
	

    list<SP_DS_FTreeGateAnimator*> m_lPossibleGateAnimators;

    list<SP_DS_FTreeEventAnimator*> m_lStepNodeAnimators;
    list<SP_DS_FTreeGateAnimator*> m_lStepGateAnimators;


	SP_FTreeNodeColourizer* m_nodeColourizer;

	void RefreshFrame();
	void SetSystemAttributes();

	void OnShow(wxCommandEvent& p_cEvent);
	void OnKlick(wxCommandEvent& p_cEvent);

	void OnColourSim(wxColourPickerEvent& p_cEvent);
	void RefreshColourSim();

public:
    SP_DS_FTreePedAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
    ~SP_DS_FTreePedAnimation();

    bool Initialise(SP_DS_Graph* p_pcGraph);

    SP_DS_Animation* Clone();
    
    bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
    bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);
    bool OnDialogOk();

    // special to ped animation
    void OnReset();

    void SetSingleStep(SP_DS_FTreeGateAnimator* p_pcVal) { m_pcSingleStep = p_pcVal; }
    SP_DS_FTreeGateAnimator* GetSingleStep() const { return m_pcSingleStep; }

	
};

#endif // __SP_DS_FTREEPEDANIMATION_H__


