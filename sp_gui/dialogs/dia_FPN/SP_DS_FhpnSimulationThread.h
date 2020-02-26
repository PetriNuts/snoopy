
/*

#ifndef _SP_DLG_FUZZYHYBRIDSIMULTIONTHREAD_H_
#define _SP_DLG_FUZZYHYBRIDSIMULTIONTHREAD_H_

#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "FuzzyReasoning.h"

class SP_DS_FhpnSimulationThread : public wxThread, public SP_DLG_HybridSimulationResults
{
public:
	SP_DS_FhpnSimulationThread(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent,
		wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Fuzzy Hybrid Simulation"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);
	~SP_DS_FhpnSimulationThread();

protected:
	void * Entry();
public:

	virtual  void SetReasoning(FuzzyReasoning fr) { m_fuzzyReasoning = fr; }
	virtual void SetAbortSim(bool b = true) {
		m_bIsAbort1 = b; m_fuzzyReasoning.SetAbort(b);
	}
	virtual ResultFuzzyBand GetResultBand() { return m_fuzzyBand; }
	virtual void SetResultBand(ResultFuzzyBand rfuzzyBand) { m_fuzzyBand = rfuzzyBand; }
	virtual void SetReducedSampling(bool bIsReduced=true) { m_isReducedSampling = bIsReduced; }
	virtual void  SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue = 0, void* p_pcUserData = NULL);
	virtual void OnSimulatorThreadEvent(SP_DS_ThreadEvent& event) {}
	//virtual void SetCurrentProgress(long lprogress) { m_lCurrentProgress = lprogress; }
//	virtual long GetCurrentProgress() { return m_lCurrentProgress; }l
	virtual void SetParentWin(wxWindow* win) { m_pcParentWin1 = win; }
	virtual void SetCurrentGraph(SP_DS_Graph* m_pcGraph1) { m_pcGraph1 = m_pcGraph1; }
	virtual void SetMainSimulator(spsim::Simulator* ss) { m_pcMainSimulatorth = ss; }
	virtual bool InitializeSimulator(unsigned long ulite, double dalpha);
	virtual void  LoadParameters(unsigned long lIteration, double dAlpha);
	virtual void SetParamNames(std::map<wxString, wxString> m_mTransParamNames1)
	{
		m_mTransParamNames = m_mTransParamNames1;

	}
	virtual std::vector<double> GetCombinationVectorForTopLevel();
	virtual void SetParamMatrix(SP_Vector2DDouble pmat) { m_paramMatrix = pmat; }
	virtual void SetCurrentSample(std::vector<double>      m_vCurrentSample1) {
		m_vCurrentSample = m_vCurrentSample;
	}
	virtual void SetFuzzyParamPos(std::map<wxString, int>  m_mFuzzyParam2Position1) {
		m_mFuzzyParam2Position = m_mFuzzyParam2Position1;
	}

private:
	wxWindow* m_pcParentWin1;
	spsim::Simulator* m_pcMainSimulatorth;
	FuzzyReasoning m_fuzzyReasoning;
	ResultFuzzyBand m_fuzzyBand;
	bool m_bIsAbort1;
	bool m_isReducedSampling;
	std::map<wxString, wxString> m_mTransParamNames;
	SP_Vector2DDouble           m_paramMatrix;
	std::vector<double>      m_vCurrentSample;
	std::map<wxString, int>  m_mFuzzyParam2Position;

};




#endif  
*/