//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// @ Updated: George Assaf
// @ Date:02/02/2020
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
#include <wx/textfile.h>//by george

#include"spsim/spsim.h"
#include "sp_ds/extensions/SP_DS_CPN_StAnimExport.h"//by george
#include "sp_ds/extensions/SP_DLG_ColStAnim_Import.h"//by george
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColSimParser.h"

#include <wx/gauge.h>
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/filedlg.h>
#include <fstream>
#include <wx/stopwatch.h>
#include <random>
#include "sp_core/tools/MersenneTwister.h"
#include "sp_ds/extensions/SP_DS_ColSPN_PlaceSelection.h"
#include "sp_gui/dialogs/SP_DLG_ShowAllModelView.h"
#include "sp_ds/animators/ani_ColSPN/SP_DLG_ColSimViewerWindow.h"


#include <fstream>
class ExportStochCPN;
class ImportColAnim;
class SP_DS_ResultViewer;
class SP_DLG_ShowAllModelView;
//class MTRand;

struct ColPlace2InstancesStates {
	wxString colPlace;
	SP_MapString2String color2Marking;
};
 

struct StepState {

	double timepoint;
	std::vector<ColPlace2InstancesStates> instancePlaces;

};

class SP_DS_ColStAnimation: public SP_DS_Animation
{
private:
	DECLARE_EVENT_TABLE()
	unsigned int m_nStepState;
	wxRadioBox* m_pcChoice;
	wxRadioBox* m_pcBindingChoice;
	int m_nBindingChoice;
	wxCheckBox *m_cbKeep;
	wxWindow* m_pcPropertyWindowPropertySizercolSimAnim;
	wxCheckBox *m_cbSimMode;
	wxSizer* l_pcRowSizer2;
	wxButton* m_pcBtn;
	SP_Vector2DDouble m_vvResultMatrix;

	//result viewer
	wxArrayString m_ArrayColPlaces;

	//MyFrame* frame;
	//wxScrolledWindow* m_pcScrolledWindow;


	
	//Input parameters for simulation
	wxTextCtrl* m_pcIntervalStartTextCtrl;
	wxTextCtrl* m_pcIntervalEndTextCtrl;
	wxTextCtrl* m_pcResultPointCountTextCtrl;
	wxTextCtrl* m_pcSeedTextCtrl;
	wxGauge* gauge;
	wxStaticText* m_pcHint;
	wxStaticText* m_pcPercentage;
	wxStaticText* m_pcHintTime;
	SP_DS_Animator* m_pcSingleStep;
	bool m_IsDestructor;//by george
	bool m_bIsKeepClicked;
	int m_nIsClose;

	wxStaticText* m_pcOutputLabelStaticText;//by george
	wxStaticText *m_pcStepCounterText; //For counter
	wxStaticText *m_pcStepCounterValue; //For counter
	wxSizer* m_pcStepCounter;//by george
	wxFilePickerCtrl* filebrowse;
	SP_DS_ColSPN_PlaceSelection* ps;
	wxString m_ExportFilename;// by george
	bool m_bExport; // by george
	bool m_bExportComplete;// by george
	long m_nStart; //Marking start
	long m_nEvery; //Marking step
	long m_nStop; //Marking stop
	int m_nMarkingOption;
	wxTextFile m_ExportTextfile;
	wxTextFile m_ExportRandom;//testing, writing generated random numbers 
	wxTextFile m_ExportRandomDisc;//testing
	wxTextFile m_ExportBindings;//testing
	SP_VectorString m_vSelectedPlaces;
	SP_VectorDouble m_vxAxisVieverDate;
	std::map<wxString, long> m_mselectedPlace2Index;
	bool m_bAutoConcurrency;
	int m_nStepCount;
	wxString m_sTriggiredUnfoldedTrans;
	int m_nColoringGroupCurrentSelectedValue;
	wxStopWatch m_stopwatch;
	//For import feature
	bool m_bImport;
	bool m_bInvalid;
	wxString m_ImportFilename;
	unsigned int m_nLineCount;
	wxTextFile m_ImportTextfile;
	SP_MapInt2String m_mTransID2Color;
	list<list<SP_DS_ColStTransAnimator*> > m_llHistoryTransAnimators;

	SP_DLG_Animation *m_pcDialog;

	bool m_bRestartAnimationFlag;

	

	wxCheckBox *m_cbBindingDialogue;
	

	wxCheckBox *m_cbChooseRandomColor;
	bool m_bChooseRandomColor;
	
	spsim::Gillespie* m_pcSimulator;

	vector<SP_DS_ColListAttribute*> m_apcColListAttr;
	vector<wxChoice*> m_apcComboBoxes;
	

   // wxStaticText* m_pcOutputLabelStaticText;
	SP_SetString  m_choices;//by george
	vector<wxChoice*> m_apcComboBoxes1;// by george
	vector<SP_DS_ColListAttribute*> m_apcColListAttrForConstants;// by george
	SP_MapString2Int m_mGroup2Pos;//by george

	map<long, SP_CPN_CountInterval> m_pnsTransitionCountById ;
	map<long, SP_CPN_CountInterval> m_nsPlaceCountById;
	SP_MapString2String m_msTransitionNamesById;
	SP_VectorStdString m_msColoredTransitionNames;
	SP_MapInt2String   m_mColTransID2Name;

	SP_MapString2Long m_mnPlaceArrayPos;

	vector<SP_VectorLong> m_anNetMarkings;

	SP_VectorLong m_anCurrentMarking;

	SP_VectorStdString m_msColorsOfPlace;

	SP_VectorStdString m_msColoredPlaceNames;

	//speical to colored petir nets

	SP_CPN_ValueAssign m_cValueAssign;	
	SP_CPN_ColorSetClass m_cColorSetClass;

	SP_DS_ColPN_Unfolding* m_pcUnfolding;

	std::vector<StepState> Traces;
	std::map<wxString, wxString> m_mPlaceInstance2Marking;
	SP_VectorString l_vIsolated;

	bool m_bFirstFolding;


	void OnModifyMarkingSets( wxCommandEvent& p_cEvent );
    void OnModifyFunctionSets( wxCommandEvent& p_cEvent );
    void OnModifyParameterSets( wxCommandEvent& p_cEvent );
	
	void OnModifyWeightSets( wxCommandEvent& p_cEvent );
	void OnModifyDelaySets( wxCommandEvent& p_cEvent );
	void OnModifyScheduleSets( wxCommandEvent& p_cEvent );
	void OnClose(wxCommandEvent& p_cEvent);

	void OnSetsChanged( wxCommandEvent& p_cEvent );

    void OnMarkingSetChanged( wxCommandEvent& p_cEvent );
	void OnFunctionSetChanged( wxCommandEvent& p_cEvent );
	void OnDelaySetChanged( wxCommandEvent& p_cEvent );
	void OnWeightSetChanged( wxCommandEvent& p_cEvent );
	void OnScheduleSetChanged( wxCommandEvent& p_cEvent );
	void OnColConstantSetsChanged(wxCommandEvent& p_cEvent);//george
	void OnModifyConstants(wxCommandEvent& p_cEvent);//by george
	void UpdateColMarking();//george
	void UpdateChoices();//george
	void LoadDefaultConstantsGroups();//by george
	void LoadSets();
	int  ChooseTransition();
	bool ChooseColTransitions();
	
	bool ChooseColTransGillespiesSSA(int p_nTransIndex);
	bool ReduceColTransitions();
	void ChooseArcMulitplicity(SP_DS_Node* p_pcNode,wxString p_sTransitionName);
	public:
	void LoadCurrentMarking();
	void SetCurrentMarking();
	bool LoadUnfoldingInformation();
	void LoadMarkingForcolorSimulation(map<long, SP_CPN_CountInterval> p_vColPlacesInterval, SP_VectorStdString p_ColorofPlacesVector, vector<SP_VectorLong>  p_ColorVector);
	bool  IsTransitionEnabled(SP_DS_Node* m_pcNode);
	void LoadPlaces();

    void LoadTransitions();

    void LoadConnections();

    void LoadParameters();

    spsim::TransitionType GetTransitionType(const wxString& p_sTransitionType);

    spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);

	void OnSimulationStart(wxCommandEvent &p_pc_Event);//george
	void OnExportSimTraces(wxCommandEvent &p_pc_Event);//george
	void OnChoosePlaces(wxCommandEvent &p_pc_Event);//george
	void OnOpenSelectedGraphViews(wxCommandEvent &p_pc_Event);
	bool  LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces);
	void ComputeResultMatrix();
	void DoExport( );//george


    void SetStepCounter();//by george
    void OnExport(wxCommandEvent &p_pc_Event);//george
    void ExportMarkings();//by george
	void OutputStateAt(double p_dTime);
	void RemoveIsolatedPlaces();
    void ExportStepSequences();//by george
    void OnImport(wxCommandEvent &event);//by george
    bool ImportStepSequences();//by george
    void ResetTransSequenceFile();//by george
	int TokenNumerOfAnimatorPlace(wxString l_sName, SP_VectorString p_vchosenbinding);

protected:
	bool PreStep(int p_nColTrans=-1, const std::vector<wxString>& values = std::vector<wxString>());//default arguemnet added on 3.05.2023
	bool Step();
	bool PostStep();
	void OnTimer(wxTimerEvent& p_cEvent);//2.5.2023
	void FireOneTransition(int p_nColTrans, const std::vector<wxString>& binding = std::vector<wxString>());//2.05.2023



	void DoColSimulation();
	double ComputeFunctionHazard(int l_nTransition);
	void  SetSimulationStopWatch(long p_nTime);
	void RemoveTranistionFromFiringSchedule(const wxString& l_sColTransName);
	void RemoveRemainingTranistionInstancesFromFiringSchedule(const wxString& l_sColTransName, const std::vector<wxString>& p_vBinding);
	wxString ColTransName(unsigned long p_nTranNum);
	double RateFunction(unsigned long p_nTranNumber, const wxString& b);
	bool EvaluatePredicate(const wxString& p_sPredicate, const wxString& p_sBinding);
	bool IsVariable(const wxString& p_sVar);
	wxString SubstituteToken(const wxString& expression, const wxString& token, const wxString& value);



	long  GetTransitionPosByName(const wxString& p_sName);

	bool  ExtractPlaceIds(const wxString& expression, SP_VectorString& p_vResultVector);
	wxString  ExtractInstanceMarking(const wxString& p_sColPlaceId, const wxString& color);
	std::string  ExtractSubstring(const std::string& lengthyString, const std::string& pattern);

	void  ExtractPlaceIntanceMarking(const wxString& p_sRate, SP_MapString2String& l_colplace2colMap);
	bool SubstituteRateFunction(const wxString& p_sRate,const wxString& colPlace, const wxString& p_vChosenBinding,wxString& substituted);
	double  GenerateRandomVariableExpDistr(double p_nLambda);
	long GenerateRandomVariableDiscrete(double p_nSum);
	long GenerateRandomVariableDiscreteBySeed(double p_nSum);
	double GenerateRandomVariableExpDistrBySeed(double p_nLambda);
	void SetSeed(unsigned long p_nSeed);

	wxString GetColorTransitionName(long p_nPos);

	double CalculateTotalHazard();

	void Simulate();

	bool InitiliseSimulator();

	double SimulateSingleStep(double p_dcurrentTime);

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

	//color simulation purpose
	SP_VectorDouble m_RowSum;
	unsigned m_RowLength;
	unsigned m_RowCount;
	unsigned m_nInstanceId;
	SP_VectorDouble m_anHazardValues;
	std::map<wxString, double> m_mInstance2HazardValue;
	double m_nIntervalStart;
	double m_nIntervalEnd;
	double m_nIntervalSize;
	long m_nNumberofPoints;
	double m_dCurrentTime;
	bool m_bStopSimulation;
	long m_nProgress = 0;
	wxString m_sHint;
	SP_VectorString m_vColPlaceNames;
	SP_VectorString m_vColPlaceNamesModified;
	std::map<long, SP_VectorString> m_vColTrans2Binding;
	wxArrayString ar_PlaceList;
	//for Gillespie's SSA
	std::mt19937 m_Generator;// for seed value
	unsigned long m_ulSeed = 0;
	MTRand* m_pcRandGen;// for Gillespie 
	//std::map<wxString, SP_VectorString> m_vColTrans2Binding;
	SP_VectorString m_vColTransInstances;
	std::map<wxString, double> m_mTransInstance2Hazard;
	std::map<wxString, unsigned> m_mTransInstance2Id;
	double m_nCombinedHazardValue;
	//wxFileOutputStream fileStream;
public:
	SP_DS_ColStAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping,bool p_bColSimMode=false);
	virtual ~SP_DS_ColStAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	virtual bool StartTimer();

	bool StartTimerForColSim();

	virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);
	virtual bool OnDialogOk();

	void OnOpenSimulation( wxCommandEvent& p_cEvent );

	// special to ped animation
	virtual void OnReset(bool p_bIsAnimMode=true);
	void OnSet(wxCommandEvent& p_cEvent);
	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);
	void OnSimMode(wxCommandEvent& event);

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

	void UpdateMarkingPlaces();//by george
	void LaodIntialNetState();//by george
	void ExportDetailsCPN(ExportStochCPN *export_frame);//george
	void ImportDetails(ImportColAnim *import_frame);//george

	//colorSimulation methods 
	//ToDo: decide later about scope acess, for now make them public
	void  CalculateHazardValue(long p_nTransitionNumber);


};



#endif // __SP_DS_ColStAnimation_H__


