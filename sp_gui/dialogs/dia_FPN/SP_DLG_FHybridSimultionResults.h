/*
* SP_DLG_FHybridSimulationResults.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 17.2.2019
* Short Description:
*/
//==============================================================================
#ifndef SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_
#define SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_
#include<ctime>
/*
* This class implements the simulation dialog for Fuzzy Hybrid PN
*/
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "FuzzyReasoning.h"
#include"SP_DS_FhpnSimulThread.h"
#include "SP_DS_FuzzyResult_Thread.h"
#include "wx/busyinfo.h"
//class SP_DS_ThreadEvent;

typedef std::vector<TriangularFN> TFN_List;
class SP_DS_ThreadEvent;
class SP_DLG_FHybridSimulationResults : public SP_DLG_HybridSimulationResults
{

private:

protected:
	wxTextCtrl*   m_lalphaLevels;
	wxTextCtrl*   m_lSamplePoints;
	wxChoice*     m_samplingchoices;
	unsigned long m_lpCount;
	unsigned long m_lnFuzzyNum;
	bool          m_binitialRun;
	bool          m_bIsAbort;
protected:
	SP_Vector2DDouble            m_paramMatrix;
	std::map<wxString, wxString> m_mTransitionsParamNames;
	std::map<wxString, int>      m_mFuzzyParam2Position;
	std::vector<double>         m_vdCurrentSample;
	std::map<wxString, wxString> m_mTransParamNames;
	ResultFuzzyBand              m_ResultFBand;
	FuzzyReasoning *              m_fr;
	long                         m_samplingStrategyselection;
	long                         m_lTotalSimRuns;
	long                         m_lruncounter;
	wxBusyInfo*                  m_info;
	wxStopWatch                  m_simStopWatch;
	std::vector<SP_DS_FhpnSimulThread*> m_fhpnThreadVector;
protected:


protected:

	virtual void  InitializeParamMatrix();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams);

	virtual bool InitializeFuzzySetting();

	virtual void     OnSimulatorThreadEvent(SP_DS_ThreadEvent& event);

	virtual void        LoadUsedParams();
	virtual void LoadTransitions();

	virtual std::vector<TriangularFN> LoadParams();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual wxString GetKParameter(const wxString& func);

	virtual void LoadParameters(unsigned long, double alpha);

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual bool InitializeSimulator(unsigned long ite, double a);

	virtual std::vector<double> GetCurentSamples(int iteration);


	virtual void* DoBasicSamplingFhpnSimulThread();

	virtual void* DoReducedSamplingFhpnSimulThread();


	virtual void SetSimulationProgressText(long& p_nValue);


	virtual void SetSimulationProgressGauge(long p_nValue);

	virtual void SetSimulationProgressGaugeRange(long p_nRangeValue);

public:

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);
	DECLARE_CLASS(SP_DLG_FHybridSimulationResults)
	SP_DLG_FHybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), const  wxString p_sTitle = wxT("Fuzzy Hybrid Simulation"), long p_nStyle =
		wxDEFAULT_DIALOG_STYLE);
	virtual ~SP_DLG_FHybridSimulationResults();

	virtual void DirectExportToCSV();
	wxDECLARE_EVENT_TABLE();

};

#endif /* SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_*/
