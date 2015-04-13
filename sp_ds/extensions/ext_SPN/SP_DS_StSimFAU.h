//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////

#ifndef __SP_DS_STSIMFAU_H__
#define __SP_DS_STSIMFAU_H__

#include <wx/string.h>

#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimPlace.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimTransition.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StAnimationRunElem.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimTimeEvent.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimTimeEventTimeline.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimImmediateTransitionList.h"

#include "sp_ds/extensions/ext_SPN/SP_DS_ApFormula.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_ApParser.h"

#if 1
#include <boost/unordered_map.hpp>
typedef boost::unordered_map<SP_VectorLong, long> mapType;
#else
typedef map<SP_VectorLong, long> mapType;
#endif

typedef SP_VectorLong state;

// This structure represent a state.
// [s] store the number of each of the NC components of the state.
// [prob] is the proba of this state.
// [acc] is used during the propagate phase.
// [ctmc] is the ctmc probability of this state (after averaging with the poisson process)
// [sum] is just the sum of the rates of the outgoing edges
// [rate] store the rate of each edges (One per reaction, that is NR).
// [next] is the index of the states reached by such an edges, -1 if unknown.
enum NodeState
{
	STATE_NOT_CHECKED,
	STATE_ABSORB,
	STATE_NOT_ABSORB
};

struct node {
    state s;
    bool vanish;
	NodeState absorb;
    double prob;
    double acc;
    double ctmc;
    double sum;
    SP_VectorDouble rate;
    SP_VectorLong next;
    node(): vanish(false), absorb(STATE_NOT_CHECKED), prob(0), acc(0), ctmc(0), sum(0) {}
};

typedef vector<node> StateSpaceType;

class SP_DLG_StSimulationResults;

class SP_DS_StSimFAU: public SP_DS_Simulation
{

  // data

  private :

	StateSpaceType m_StateSpace;
	mapType m_mMapping;
	SP_SetLong m_aAvailable;

	unsigned long m_nMaxSize;
	unsigned long m_nStateSpaceSize;

	//cutting delta
	double m_nDelta;
	//cutting thershold for the Embedded DTMC and for the Embedded DTMC of the Birth Process
	double m_nDeltaB;
	//stopping criteria
	double m_nTolerance;
	//Poisson process accuracy - in the Fox Glynn algorithm or alike.
	double m_nEpsilon;

	double m_nMaxLambda;

	unsigned m_nCurrentResultPoint;
	bool m_ResetStates;

	//Controls for Properties Dlg
	wxTextCtrl* m_pcMaxLambdaTextCtrl;
	wxTextCtrl* m_pcDeltaTextCtrl;
	wxTextCtrl* m_pcDeltaBTextCtrl;
	wxTextCtrl* m_pcTolerenceTextCtrl;
	wxTextCtrl* m_pcEpsilonTextCtrl;

	long m_nLMin;
	long m_nRMax;
	long m_nLB;
	long m_nRB;
	long m_nNbActive;
	long m_nNbDelete;
	double m_nPNew;
	SP_VectorDouble m_anW;
	SP_VectorDouble m_anCrtState;



    SP_DS_StParser* m_cParser;

    SP_DS_StSimImmediateTransitionList* m_pImmediateTransitionList;
    SP_DS_StSimTimeEventTimeline* m_qpcSimTimeEventTimeline;

	SP_VectorString m_msTransitionFunctions;

	SP_VectorSimPlace m_apcSimPlaces;
	SP_VectorSimTransition m_apcSimTransitions;

	state m_anInitialNetMarking;

	SP_Vector2DDouble m_aanResults;
	SP_Vector2DDouble m_aanResultsSD;
    SP_Vector2DDouble m_aanSingleResults;

    SP_Vector2DDouble m_aanResultTransitionFireCount;
    SP_Vector2DDouble m_aanSingleTransitionFireCount;

	SP_DS_ApParser* m_pcApParser;
	SP_VectorApFormula m_aApFormulas;
	SP_VectorString m_sApFormulas;
	SP_VectorBool m_aApFormulaHappend;
	SP_VectorDouble m_aApResults;
	SP_VectorLong m_aApResultsCount;

  protected :


  public :


  // methods

  public :

    SP_DS_StSimFAU(SP_DS_Graph* p_pcGraph);
    virtual ~SP_DS_StSimFAU();

//	virtual bool AddToSimulation(SP_DLG_StSimulationResults* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool OnSimulationPropertiesOk();

    void StartSimulation();

    bool Initialise();

    const SP_Vector2DDouble& GetResultMatrix() { return m_aanResults; }
    const SP_Vector2DDouble& GetSingleResultMatrix() { return m_aanSingleResults; }
	const SP_Vector2DDouble& GetResultSDMatrix() { return m_aanResultsSD; }
    const SP_Vector2DDouble& GetResultFireCount() { return m_aanResultTransitionFireCount; }
    const SP_Vector2DDouble& GetSingleResultFireCount() { return m_aanSingleTransitionFireCount; }

    void LoadPlaces();
    void LoadTransitions();

    void Debug_ShowSimPlaces();
    void Debug_ShowDeltaMatrix();
    void Debug_ShowSimTransitions();

    void ShowParseError(const wxString& p_sNodeType);
  protected :


  private :

    double Solution(double p_nStepTime);

    void Propagate(double p_nLambda, double p_nCoeff, long p_nIteration);
    double Collect(long p_nIteration);
    double BirthProcessState(double p_nA, double p_nB, long p_nK);
    double BirthCoeffCompress(double p_nA, double p_nB, long p_nK);
    long NewNode(const state& p_aState);
    void RemoveNode(long p_nNode);

    void resetStates();

    void FoxGlynn_w(double l, double epsilon);
    int FoxGlynn_Right(double l, double epsilon);
    int FoxGlynn_Left(double l, double epsilon);

    void CheckVanishing(long p_nIteration);

    void CheckApFormulae();
	void CheckNodeState(node& p_Node);

    void SetDeltaVector( SP_DS_Node* p_pcTransNode, long p_nArrayPos );

    wxString GetHazardFunctionTypeAsString( SP_DS_StSimHazardFunctionType p_eType );

    bool InitSimPlaces();
    bool InitSimTransitions();

    SP_DS_StSimPlace* CreateSimPlace( long p_nPlaceArrayPos );
    SP_DS_StSimTransition* CreateSimTransition( long p_nTransitionArrayPos );

    void SimulateRun();

    void GenerateResultPoint( long p_nCurrentResultPoint );

	void WriteResults();

	void ExecuteTimeEvent( SP_DS_StSimTimeEvent* p_pcTimeEvent );

    void ComputeRate(node& p_Node);
    double CalculateHazardValue(long p_nTransitionNumber, const state& p_aState);
    bool IsTransitionEnabled(long p_nTransitionNumber, const state& p_aState);
    void FireTransition(long p_nTransNumber, state& p_aState);

    void SetToUpdateTransitionPosOfPlaces( long p_nPlaceNumber );
    void SetToUpdateTransitionPosOfTransitions( long p_nTransNumber );

    void InitialiseTimeline();

    void SetMemoryFree();

    void SetMemoryFreeTransitions();
    void SetMemoryFreePlaces();

	// only for colored stochastic Petri Nets
	void LoadData();
	//

};

#endif
