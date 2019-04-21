
#ifndef __SP_DS_FSPN_SIM_TH_
#define __SP_DS_FSPN_SIM_TH_

#include "FuzzyReasoning.h"
#include <wx/string.h>
#include <wx/thread.h>
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
 

#include "sp_gui/dialogs/SP_DLG_Simulation.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "FuzzyBasics.h"
#include "FuzzyReasoning.h"
#include "TriangularFN.h"


class SP_DS_ThreadEvent;
class SP_DS_FspnThread :public wxThread, public  SP_DLG_StSimulationResults
{
public:
	SP_DS_FspnThread(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText, const wxString& p_sTitle, long p_nStyle);
	~SP_DS_FspnThread();


protected:
	void * Entry();
public:
	virtual  void SetReasoning(FuzzyReasoning fr) { m_fuzzyReasoning = fr; }
	virtual void SetAbortSim(bool b) {
		m_bIsAbort1 = b; m_fuzzyReasoning.SetAbort(b);
	}
	virtual ResultFuzzyBand GetResultBand() { return m_fuzzyBand; }
	virtual void SetResultBand(ResultFuzzyBand rfuzzyBand) { m_fuzzyBand = rfuzzyBand; }
	virtual spsim::Matrix2DDouble  DOneSpnSimulation(unsigned long iteration, double alpha);
	virtual bool  InitializeSimulator(unsigned long lIteration, double dAlpha);
	virtual 	void LoadParameters(unsigned long lIteration, double dAlpha);
	virtual 	void SetTransitionNames(std::map<wxString, wxString> transmap) { m_TransParamNames = transmap; }
	virtual 	void SetParam2Pos(std::map<wxString, int> pp) { m_mFuzzyParam2Position = pp; }
	virtual 	void SetCurrentSample(vector<double> sam) { std::vector<double> m_vdCurrentSample; }
	virtual 	vector<double> GetCurentSample() { return m_vdCurrentSample; }
	virtual 	std::vector<double> GetCombinationVectorForTopLevel();
	virtual void SetParamMatrix(SP_Vector2DDouble vv) { m_paramMatrix1 = vv; }
	virtual std::vector<double>  GetCurentSamples(int iteration);
	virtual void SetReducedSampling(bool bIsReduced) { m_isReducedSampling = bIsReduced; }
	virtual void* DoFSPwithNormalSampling();
	virtual void SetSimulationProgressGauge(long p_nValue);
	virtual void InitProgress();
	//virtual void SetSimulationProgressText(long& p_nValue);
	virtual void  SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue = 0, void* p_pcUserData = NULL);
	virtual void OnSimulatorThreadEvent(SP_DS_ThreadEvent& event) {}
	virtual void SetCurrentProgress(long lprogress) { m_lCurrentProgress = lprogress; }
	virtual long GetCurrentProgress() { return m_lCurrentProgress; }
	virtual void SetParentWin(wxWindow* win) { m_pcParentWin1 = win; }
	virtual void SetCurrentGraph(SP_DS_Graph* m_pcGraph1) { m_pcGraph1 = m_pcGraph1; }
	virtual void SetMainSimulator(spsim::Simulator* ss) { m_pcMainSimulatorth = ss; }
private:
	wxWindow* m_pcParentWin1;
	SP_DS_Graph* m_pcGraph1;
	spsim::Simulator* m_pcMainSimulatorth;
	FuzzyReasoning m_fuzzyReasoning;
	bool m_bIsAbort1;
	ResultFuzzyBand m_fuzzyBand;
	bool m_initialRun1;
	std::map<wxString, wxString> m_TransParamNames;
	std::map<wxString, int> m_mFuzzyParam2Position;
	std::vector<double> m_vdCurrentSample;
	SP_Vector2DDouble            m_paramMatrix1;
	long                         m_lTotalSimRuns;
	bool                         m_isReducedSampling;
	long                         m_lCurrentProgress;
	wxStopWatch                m_fspnstopWatch;
	//SP_Compressed_Fuzzy_Band*     m_compressedband;
};

#endif //__SP_DS_FSPN_SIM_TH_