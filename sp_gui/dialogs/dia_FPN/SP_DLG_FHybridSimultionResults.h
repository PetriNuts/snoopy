/*
* SP_DLG_HybridSimulationResults.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 17.2.2019
* Short Description:
*/
//==============================================================================
#ifndef SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_
#define SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_

/*
* This class implements the simulation dialog for Fuzzy Hybrid PN
*/
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "FuzzyReasoning.h"
//class SP_DS_ThreadEvent;
///class SP_DS_SimulatorThreadHybrid;
typedef std::vector<TriangularFN> TFN_List;

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
	FuzzyReasoning               m_fr;
	long                         m_samplingStrategyselection;
	long                         m_lTotalSimRuns;
protected:
 

protected:

	virtual void  InitializeParamMatrix();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams);

	virtual bool InitializeFuzzySetting();


	virtual void LoadUsedParams();

	virtual std::vector<TriangularFN> LoadParams();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual wxString GetKParameter(const wxString& func);

	virtual void LoadParameters(unsigned long, double alpha);

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual bool InitializeSimulator(unsigned long ite, double a);

	virtual std::vector<double> GetCurentSamples(int iteration);

	virtual void* DoFHpnSimulation();

	virtual void* DoOneHPNSimulation(long iter, double alpha);

	virtual void* RunNormalHPN();

	virtual void* DoReducedFhpn();

	virtual void SetSimulationProgressText(long& p_nValue);

	virtual void InitProgress();

	virtual void SetSimulationProgressGauge(long p_nValue);

	virtual void SetSimulationProgressGaugeRange(long p_nRangeValue);

public:

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);
	DECLARE_CLASS(SP_DLG_FHybridSimulationResults)
		SP_DLG_FHybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Hybrid Simulation Results"), long p_nStyle =
			wxDEFAULT_DIALOG_STYLE);
	virtual ~SP_DLG_FHybridSimulationResults();

	virtual void DirectExportToCSV();

	 
};

#endif /* SP_DLG_FUZZYHYBRIDSIMULTIONRESULTS_H_*/
