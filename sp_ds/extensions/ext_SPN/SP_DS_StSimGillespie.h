//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////

#ifndef __SP_DS_STSIMGILLESPIE_H__
#define __SP_DS_STSIMGILLESPIE_H__

#include <wx/string.h>
#include <wx/thread.h>

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


class SP_DLG_StSimulationResults;
class MTRand;

class SP_DS_StSimGillespie: public SP_DS_Simulation
{

	// data

protected :

	double m_nCombinedHazardValue;

	SP_DS_StParser* m_cParser;

	SP_DS_StSimImmediateTransitionList* m_pImmediateTransitionList;
	SP_DS_StSimTimeEventTimeline* m_qpcSimTimeEventTimeline;

	SP_VectorLong m_anCurrentMarking;
	SP_VectorDouble m_anLastMarking;
	SP_VectorDouble m_anHazardValues;
	SP_VectorDouble m_anLastHazardValues;

	SP_VectorString m_msTransitionFunctions;

	SP_VectorSimPlace m_apcSimPlaces;
	SP_VectorSimTransition m_apcSimTransitions;

	SP_VectorLong m_anInitialNetMarking;

	SP_Vector2DDouble m_aanResults;
	SP_Vector2DDouble m_aanResultsSD;
	SP_Vector2DDouble m_aanSingleResults;

	SP_VectorLong m_anCurrentFireCount;
	SP_Vector2DDouble m_aanResultTransitionFireCount;
	SP_Vector2DDouble m_aanSingleTransitionFireCount;

	SP_DS_ApParser* m_pcApParser;
	SP_VectorApFormula m_aApFormulas;
	SP_VectorString m_sApFormulas;
	SP_VectorBool m_aApFormulaHappend;
	SP_VectorDouble m_aApResults;

	wxChoice* m_pcThreadCountComboBox;
	unsigned long m_nThreadCount;
	unsigned long m_nThreadFinished;
	// crit section
	wxCriticalSection m_critsect;


	// methods

public :

	SP_DS_StSimGillespie(SP_DS_Graph* p_pcGraph);
	virtual ~SP_DS_StSimGillespie();

	virtual bool AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer);
	virtual bool OnSimulationPropertiesOk();

	SP_VectorLong& GetCurrentMarking() { return m_anCurrentMarking; }
	//    SP_VectorLong& GetCurrentFireCount() { return m_anCurrentFireCount; }

	void StartSimulation();

	bool Initialise();

	const SP_Vector2DDouble& GetResultMatrix() { return m_aanResults; }
	const SP_Vector2DDouble& GetSingleResultMatrix() { return m_aanSingleResults; }
	const SP_Vector2DDouble& GetResultSDMatrix() { return m_aanResultsSD; }
	const SP_Vector2DDouble& GetResultFireCount() { return m_aanResultTransitionFireCount; }
	const SP_Vector2DDouble& GetSingleResultFireCount() { return m_aanSingleTransitionFireCount; }

	void LoadPlaces();
	void LoadTransitions();

	SP_DS_StAnimationRunElem* GetNextAnimationRunElem();

	void Debug_ShowSimPlaces();
	void Debug_ShowDeltaMatrix();
	void Debug_ShowSimTransitions();

	void ShowParseError(const wxString& p_sNodeType);

	//thread functions
	void ThreadFinished();
	void ThreadAddResult(const SP_Vector2DDouble& p_Result);
	void ThreadAddResultSD(const SP_Vector2DDouble& p_Result);
	void ThreadAddFireCount(const SP_Vector2DDouble& p_FireCount);

	void ThreadAddApResult(const SP_VectorDouble& p_Result);


	void SetMemoryFree();

protected :

	void SetDeltaVector( SP_DS_Node* p_pcTransNode, long p_nArrayPos );

	wxString GetHazardFunctionTypeAsString( SP_DS_StSimHazardFunctionType p_eType );

	bool InitSimPlaces();
	bool InitSimTransitions();

	SP_DS_StSimPlace* CreateSimPlace( long p_nPlaceArrayPos );
	SP_DS_StSimTransition* CreateSimTransition( long p_nTransitionArrayPos );

	void SimulateRun();

	void GenerateResultPoints(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint);
	void GenerateResultPoint( long p_nCurrentResultPoint );

	void CheckTimeEventTimeLine(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint);
	void ExecuteTimeEventForAnimation( SP_DS_StSimTimeEvent* p_pcTimeEvent, SP_DS_StAnimationRunElem* p_pcRunElem );

	double GenerateRandomVariableExpDistr( double p_nLambda );
	long GenerateRandomVariableDiscrete( double p_nSum );

	void CalculateHazardValue( long p_nTransitionNumber );
	void CalculateAllHazardValues();
	double SumHazardValues();
	bool IsTransitionEnabled( long p_nTransitionNumber );
	void FireTransition( long p_nTransNumber );

	void DirectSingleExactExport();
	void WriteResults();
	void AddToMatrix(SP_Vector2DDouble& p_LHS, const SP_Vector2DDouble& p_RHS);
	void AddToVector(SP_VectorDouble& p_LHS, const SP_VectorDouble& p_RHS);

	void SetToUpdateTransitionPosOfPlaces( long p_nPlaceNumber );
	void SetToUpdateTransitionPosOfTransitions( long p_nTransNumber );

	void CheckImmediateTransitions();

	void InitialiseTimeline();

	void SetMemoryFreeTransitions();
	void SetMemoryFreePlaces();

	// only for colored stochastic Petri Nets
	void LoadData();
	//

	void CheckApFormulae(SP_DS_StSimTransition* p_pcSimTrans);

#if 1
	class SimulationThread : public wxThread
	{
	public:
		SimulationThread(unsigned long p_nThreadId,
			SP_DS_StSimGillespie* p_pcSimulation,
			SP_DS_Graph* p_pcGraph,
			double p_nIntervalStart,
			double p_nIntervalEnd,
			double p_nIntervalSize,
			unsigned long p_nResultPointCount,
			unsigned long p_nPlaceCount,
			unsigned long p_nTransitionCount,
			SP_VectorString p_msTransitionFunctions,
			SP_VectorSimPlace p_apcSimPlaces,
			SP_VectorSimTransition p_apcSimTransitions,
			SP_VectorLong p_anInitialNetMarking,
			SP_MatrixPairDouble* p_aanPreIncMatrix,
			unsigned long p_nSimulationRunCount,
			SP_VectorApFormula p_aApFormulae);
		~SimulationThread();

		unsigned long GetCurrentSimulationRunCount();
		// thread execution starts here
		virtual void *Entry();

	protected:
		void SimulateRun();

		void GenerateResultPoints(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint);
		void GenerateResultPoint( long p_nCurrentResultPoint );

		void CheckTimeEventTimeLine(double& p_nOutputTime, unsigned long& p_nCurrentResultPoint);

		double GenerateRandomVariableExpDistr( double p_nLambda );
		long GenerateRandomVariableDiscrete( double p_nSum );

		void CalculateHazardValue( long p_nTransitionNumber );
		void CalculateAllHazardValues();
		double SumHazardValues();
		bool IsTransitionEnabled( long p_nTransitionNumber );
		void FireTransition( long p_nTransNumber );

		void CheckImmediateTransitions();

		void InitialiseTimeline();

		void CheckApFormulae(SP_DS_StSimTransition* p_pcSimTrans);

	protected:
		unsigned long m_nThreadId;

		MTRand* m_pcMTRand;
		SP_DS_StParser* m_cParser;

		SP_DS_StSimGillespie* m_pcSimulation;
		SP_DS_Graph* m_pcGraph;

		double m_nIntervalStart;
		double m_nIntervalEnd;
		double m_nIntervalSize;

		unsigned long m_nResultPointCount;

		unsigned long m_nPlaceCount;
		unsigned long m_nTransitionCount;

		double m_nCurrentTime;
		double m_nLastTime;
		double m_nCombinedHazardValue;

		SP_DS_StSimImmediateTransitionList* m_pImmediateTransitionList;
		SP_DS_StSimTimeEventTimeline* m_qpcSimTimeEventTimeline;

		SP_VectorLong m_anCurrentMarking;
		SP_VectorDouble m_anLastMarking;
		SP_VectorDouble m_anHazardValues;

		SP_VectorString m_msTransitionFunctions;

		SP_VectorSimPlace m_apcSimPlaces;
		SP_VectorSimTransition m_apcSimTransitions;

		SP_VectorLong m_anInitialNetMarking;

		SP_Vector2DDouble m_aanResults;
		SP_Vector2DDouble m_aanResultsSD;

		SP_VectorLong m_anCurrentFireCount;
		SP_Vector2DDouble m_aanResultTransitionFireCount;

		SP_MatrixPairDouble* m_aanPreIncMatrix;

		unsigned long m_nSimulationRunCount;
		unsigned long m_nCurrentSimulationRunCount;

		SP_VectorApFormula m_aApFormulas;
		SP_VectorBool m_aApFormulaHappend;
		SP_VectorDouble m_aApResults;

	};
#endif

};



#endif
