//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: colored PN animation class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColStAnimation_H__
#define __SP_DS_ColStAnimation_H__

#include <wx/wx.h>
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStPlaceAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStTransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStCoarseTransAnimator.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimGillespie.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"


#include"spsim/spsim.h"

class SP_DS_ColStAnimation: public SP_DS_Animation
{
private:
	DECLARE_EVENT_TABLE()
	unsigned int m_nStepState;
	wxRadioBox* m_pcChoice;
	wxRadioBox* m_pcBindingChoice;
	int m_nBindingChoice;
	wxCheckBox *m_cbKeep;
	SP_DS_Animator* m_pcSingleStep;

	list<list<SP_DS_ColStTransAnimator*> > m_llHistoryTransAnimators;

	SP_DLG_Animation *m_pcDialog;

	bool m_bRestartAnimationFlag;

	

	wxCheckBox *m_cbBindingDialogue;
	

	wxCheckBox *m_cbChooseRandomColor;
	bool m_bChooseRandomColor;
	
	spsim::Gillespie* m_pcSimulator;

	vector<SP_DS_ColListAttribute*> m_apcColListAttr;
	vector<wxChoice*> m_apcComboBoxes;

    wxStaticText* m_pcOutputLabelStaticText;


	map<long, SP_CPN_CountInterval> m_pnsTransitionCountById ;
	map<long, SP_CPN_CountInterval> m_nsPlaceCountById;
	SP_MapString2String m_msTransitionNamesById;
	SP_VectorString m_msColoredTransitionNames;

	SP_MapString2Long m_mnPlaceArrayPos;

	vector<SP_VectorLong> m_anNetMarkings;

	SP_VectorLong m_anCurrentMarking;

	SP_VectorString m_msColorsOfPlace;

	SP_VectorString m_msColoredPlaceNames;

	//speical to colored petir nets

	SP_CPN_ValueAssign m_cValueAssign;	
	SP_CPN_ColorSetClass m_cColorSetClass;

	SP_DS_ColPN_Unfolding* m_pcUnfolding;

	bool m_bFirstFolding;


	void OnModifyMarkingSets( wxCommandEvent& p_cEvent );
    void OnModifyFunctionSets( wxCommandEvent& p_cEvent );
    void OnModifyParameterSets( wxCommandEvent& p_cEvent );
	
	void OnModifyWeightSets( wxCommandEvent& p_cEvent );
	void OnModifyDelaySets( wxCommandEvent& p_cEvent );
	void OnModifyScheduleSets( wxCommandEvent& p_cEvent );

	void OnSetsChanged( wxCommandEvent& p_cEvent );

    void OnMarkingSetChanged( wxCommandEvent& p_cEvent );
	void OnFunctionSetChanged( wxCommandEvent& p_cEvent );
	void OnDelaySetChanged( wxCommandEvent& p_cEvent );
	void OnWeightSetChanged( wxCommandEvent& p_cEvent );
	void OnScheduleSetChanged( wxCommandEvent& p_cEvent );

	void LoadSets();
	int ChooseTransition();
	bool ChooseColTransitions();
	bool ReduceColTransitions();
	void ChooseArcMulitplicity(SP_DS_Node* p_pcNode,wxString p_sTransitionName);

	void LoadCurrentMarking();
	void SetCurrentMarking();
	bool LoadUnfoldingInformation();

	void LoadPlaces();

    void LoadTransitions();

    void LoadConnections();

    void LoadParameters();

    spsim::TransitionType GetTransitionType(const wxString& p_sTransitionType);

    spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);

protected:
	bool PreStep();
	bool Step();
	bool PostStep();

	// special to ped
	virtual bool ReduceTransitions();
	void KeepMarking();

	list<SP_DS_ColStPlaceAnimator*> m_lAllPlaceAnimators;
	list<SP_DS_ColStTransAnimator*> m_lAllTransAnimators;

	list<SP_DS_ColStTransAnimator*> m_lPossibleTransAnimators;

	list<SP_DS_ColStPlaceAnimator*> m_lStepPlaceAnimators;
	list<SP_DS_ColStTransAnimator*> m_lStepTransAnimators;

	map< int, SP_DS_ColStTransAnimator* > m_mpcTransitionAnimators;
    map< int, SP_DS_ColStPlaceAnimator* > m_mpcPlaceAnimators;

public:
	SP_DS_ColStAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
	virtual ~SP_DS_ColStAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	virtual bool StartTimer();

	virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);
	virtual bool OnDialogOk();

	void OnOpenSimulation( wxCommandEvent& p_cEvent );

	// special to ped animation
	virtual void OnReset();
	void OnSet(wxCommandEvent& p_cEvent);
	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

	void SetSingleStep(SP_DS_Animator* p_pcVal) { m_pcSingleStep = p_pcVal; }
	SP_DS_Animator* GetSingleStep() const { return m_pcSingleStep; }

	//speical to colored petri nets
	SP_CPN_ColorSetClass* GetColorSetClass() 
	{ 
		m_cValueAssign.InitializeColorset(m_cColorSetClass);
		return &m_cColorSetClass; 
	}

	int GetBindingDialogueShowFlag(){return m_nBindingChoice;}
	bool GetChooseRandomColorFlag(){return m_bChooseRandomColor;}

};

#endif // __SP_DS_ColStAnimation_H__


