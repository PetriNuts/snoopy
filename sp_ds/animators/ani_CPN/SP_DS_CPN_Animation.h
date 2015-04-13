//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: colored PN animation class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_CPN_Animation_H__
#define __SP_DS_CPN_Animation_H__

#include <wx/wx.h>
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_PlaceAnimator.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_TransAnimator.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_CoarseTransAnimator.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

class SP_DS_CPN_Animation: public SP_DS_Animation
{
private:
	DECLARE_EVENT_TABLE()
	unsigned int m_nStepState;
	wxRadioBox* m_pcChoice;
	wxCheckBox *m_cbKeep;
	SP_DS_Animator* m_pcSingleStep;

	wxRadioBox* m_pcBindingChoice;
	int m_nBindingChoice;
	

	wxCheckBox *m_cbChooseRandomColor;
	bool m_bChooseRandomColor;

	list<list<SP_DS_CPN_TransAnimator*> > m_llHistoryTransAnimators;

	SP_DLG_Animation *m_pcDialog;

	//speical to colored petir nets
	SP_CPN_ValueAssign m_cValueAssign;	
	SP_CPN_ColorSetClass m_cColorSetClass;

protected:
	bool PreStep();
	bool Step();
	bool PostStep();

	// special to ped
	virtual bool ReduceTransitions();
	void KeepMarking();

	list<SP_DS_CPN_PlaceAnimator*> m_lAllPlaceAnimators;
	list<SP_DS_CPN_TransAnimator*> m_lAllTransAnimators;

	list<SP_DS_CPN_TransAnimator*> m_lPossibleTransAnimators;

	list<SP_DS_CPN_PlaceAnimator*> m_lStepPlaceAnimators;
	list<SP_DS_CPN_TransAnimator*> m_lStepTransAnimators;

public:
	SP_DS_CPN_Animation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
	virtual ~SP_DS_CPN_Animation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);
	virtual bool OnDialogOk();

	// special to ped animation
	void OnReset();
	void OnSet(wxCommandEvent& p_cEvent);
	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

	void SetSingleStep(SP_DS_Animator* p_pcVal) { m_pcSingleStep = p_pcVal; }
	SP_DS_Animator* GetSingleStep() const { return m_pcSingleStep; }

	//speical to colored petri nets
	SP_CPN_ColorSetClass* GetColorSetClass() 
	{ 
		if( m_cValueAssign.InitializeColorset(m_cColorSetClass) )
			return &m_cColorSetClass; 
		else
			return NULL;
	}

	int GetBindingDialogueShowFlag(){return m_nBindingChoice;}
	bool GetChooseRandomColorFlag(){return m_bChooseRandomColor;}

};

#endif // __SP_DS_CPN_Animation_H__


