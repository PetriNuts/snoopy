//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_PEDANIMATION_H__
#define __SP_DS_PEDANIMATION_H__

#include "sp_ds/extensions/SP_DS_Export.h"
#include "sp_ds/extensions/SP_DS_Import.h"
#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_PlaceAnimator.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/animators/SP_DS_CoarseTransAnimator.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class Export;
class Import;

class SP_DS_PedAnimation: public SP_DS_Animation
{
private:
	DECLARE_EVENT_TABLE()
	unsigned int m_nStepState;
	bool m_bAutoConcurrency;
	wxRadioBox* m_pcChoice;
	wxCheckBox *m_cbKeep;
	wxCheckBox *m_cbAutoConcurrency;
	wxBoxSizer *m_pcExportImportSizer; // Added for export & import options in animation window
	wxStaticText *m_pcStepCounterText; //For counter
	wxStaticText *m_pcStepCounterValue; //For counter
	wxBoxSizer *m_pcStepCounter; //For counter
	SP_DS_Animator* m_pcSingleStep;

	//For export feature
	wxString m_ExportFilename;
	bool m_bExport;
	bool m_bExportComplete;
	long int m_nStepCount; //Keeps a count of step number
	long m_nStart; //Marking start
	long m_nEvery; //Marking step
	long m_nStop; //Marking stop
	int m_nMarkingOption;
	wxTextFile m_ExportTextfile;

	//For import feature
	bool m_bImport;
	bool m_bInvalid;
	wxString m_ImportFilename;
	unsigned int m_nLineCount;
	wxTextFile m_ImportTextfile;

	list<list<SP_DS_TransAnimator*> > m_llHistoryTransAnimators;

	SP_DLG_Animation *m_pcDialog;

	//bysl
	vector<wxChoice*> m_apcChoices;
	vector<SP_DS_ColListAttribute*> m_apcColListAttr;
	wxArrayString  m_choices;

	void LoadSets();
	void LoadGroups();
	void LoadCurrentMarking();
	//by sl
	void LoadInitialMarking();

protected:
	bool PreStep();
	bool Step();
	bool PostStep();

	// special to ped
	virtual bool ReduceTransitions();
	// Added by Erik Jongsma, needed for conflict resolution
	virtual bool SolveConflicts();
	// Needed for autoconcurrency
	virtual bool MaximizeFirings();

	void KeepMarking();

	void SetStepCounter();

	list<SP_DS_PlaceAnimator*> m_lAllPlaceAnimators;
	list<SP_DS_TransAnimator*> m_lAllTransAnimators;

	list<SP_DS_TransAnimator*> m_lPossibleTransAnimators;

	list<SP_DS_PlaceAnimator*> m_lStepPlaceAnimators;
	list<SP_DS_TransAnimator*> m_lStepTransAnimators;

	// Added by Erik Jongsma, needed for conflict resolution
	list<SP_DS_TransAnimator*> m_lConflictTransAnimators;


public:
	SP_DS_PedAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
	virtual ~SP_DS_PedAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);
	virtual bool OnDialogOk();

	// special to ped animation
	virtual void OnReset();
	void OnSet(wxCommandEvent& p_cEvent);
	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

	void SetSingleStep(SP_DS_Animator* p_pcVal) { m_pcSingleStep = p_pcVal; }
	SP_DS_Animator* GetSingleStep() const { return m_pcSingleStep; }


	void OnModifyMarkingSets(wxCommandEvent& p_cEvent);
	void OnModifyConstantSets(wxCommandEvent& p_cEvent);
	void OnConstantSetsChanged(wxCommandEvent& p_cEvent);

	//Export & Import options - Extra methods added
	void OnExport(wxCommandEvent &p_pcEvent);
	void OnImport(wxCommandEvent &p_pcEvent);
	void ExportDetails(Export *);
	void ExportMarkings();
	void ExportStepSequences();
	void ImportDetails(Import *);
	bool ImportStepSequences();
	void ResetTransSequenceFile();

};

#endif // __SP_DS_PEDANIMATION_H__


