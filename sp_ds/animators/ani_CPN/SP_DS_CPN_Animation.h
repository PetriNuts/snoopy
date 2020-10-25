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

//by george for constants harmonizing
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"
#include "sp_ds/extensions/SP_DS_CPN_AnimExport.h"//by george
#include "sp_ds/extensions/SP_DLG_ColStAnim_Import.h"//by george
class ImportColAnim;
class ExportCPN;

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

	SP_MapInt2String m_mTransID2Color;
	//speical to colored petir nets
	SP_CPN_ValueAssign m_cValueAssign;	
	SP_CPN_ColorSetClass m_cColorSetClass;
	bool m_IsDestructor;//by george
	SP_SetString  m_choices;//by george
	vector<wxChoice*> m_apcComboBoxes1;// by george
	vector<SP_DS_ColListAttribute*> m_apcColListAttrForConstants;// by george
	wxStaticText* m_pcOutputLabelStaticText;//by george
	wxStaticText *m_pcStepCounterText; //For counter
	wxStaticText *m_pcStepCounterValue; //For counter
	wxSizer* m_pcStepCounter;//by george
	int m_nIsClose;
	bool m_bIsKeepClicked;// by george

	wxString m_ExportFilename;// by george
	bool m_bExport; // by george
	bool m_bExportComplete;// by george
	long m_nStart; //Marking start
	long m_nEvery; //Marking step
	long m_nStop; //Marking stop
	int m_nMarkingOption;
	wxTextFile m_ExportTextfile;
	bool m_bAutoConcurrency;
	int m_nStepCount;

	//For import feature
	bool m_bImport;
	bool m_bInvalid;
	wxString m_ImportFilename;
	unsigned int m_nLineCount;
	wxTextFile m_ImportTextfile;


private:
	void UpdateChoices();//by george
	void LoadSets();//by george
	void OnModifyConstants(wxCommandEvent& p_cEvent);//by george
	void OnColConstantSetsChanged(wxCommandEvent& p_cEvent);//george
	void UpdateColMarking();//by george
	void LoadDefaultConstantsGroups();//by george
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
	void OnExport(wxCommandEvent &p_pc_Event);//george
	void ExportDetailsCPN(ExportCPN *export_frame);//george
	void ImportDetails(ImportColAnim *);//george
	void OnImport(wxCommandEvent &p_pcEvent);//george

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

	void UpdateMarkingPlaces();//by george
	void ExportMarkings();//by george
	void ExportStepSequences();
	void SetStepCounter();//by george
	void ImportStepSequences();//by george
	void ResetTransSequenceFile();//by george

};

#endif // __SP_DS_CPN_Animation_H__


