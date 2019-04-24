#ifndef __SP_DS_FSPNSIMRESULTS_H__
#define __SP_DS_FSPNSIMRESULTS_H__
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

typedef std::vector<TriangularFN>  TFN_List;
class SP_DS_ThreadEvent;
class SP_DS_SimulatorThreadStochastic;


class SP_DLG_FspnSimResult : public SP_DLG_StSimulationResults
{
private:
protected:
	wxTextCtrl*                 m_pSamplePints;
	wxTextCtrl*                 m_alphaLevels1;
	wxChoice*                   m_pcSamplingchoices;
	bool                        m_initialRun;
	unsigned long               m_lpCount;
	unsigned long               m_lnFuzzyNum;
	long                        m_lSamplingStrategyselection;
	long                        m_lTotalSimRuns;
protected:
	SP_Vector2DDouble            m_paramMatrix;
	std::map<wxString, wxString> m_mTransParamNames;
	FuzzyReasoning               m_fr;
	std::vector<int>             m_fuzzyParamPositions;
	std::vector<double>          m_vdCurrentSample;
	ResultFuzzyBand              m_ResultFBand;
	std::map<wxString, int>      m_mFuzzyParam2Position;
	std::vector<wxString>        m_fuzzyParams;
	bool                         m_bIsAbort;
	std::clock_t             m_clock;
protected:


	virtual bool InitializeFuzzySetting();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual std::vector<double>  GetCurentSamples(int iteration);

	virtual std::vector<TriangularFN> LoadParams();

	virtual void   LoadUsedParams();

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams);

	//virtual spsim::Simulator*  CreateSimulator(const int& p_nSimulatorType);

	virtual void  OnSimulatorSelect(wxCommandEvent& p_cEvent);

	virtual bool InitializeSimulator(unsigned long iteration, double alpha);

	virtual void LoadParameters(unsigned long, double alpha);

	virtual void InitializeParamMatrix();

	virtual void* DoFspnSimulation();

	virtual spsim::Matrix2DDouble DoNormalFSPN();

	virtual void* DoFSPwithNormalSampling();

	virtual spsim::Matrix2DDouble DOneSpnSimulation(unsigned long iteration, double alpha);

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	virtual wxString  GetKParameter(const wxString& func);

	virtual void   OnCloseWindow(wxCloseEvent& p_cEvent);

	virtual void OnModifyConstantSets(wxCommandEvent& p_cEvent);

	virtual void SetSimulationProgressText(long& p_nValue);

	virtual void  InitProgress();

	virtual void  SetSimulationProgressGaugeRange(long p_nRangeValue);

	void SetSimulationProgressGauge(long p_nValue);

	virtual wxString GetCurrentSelectedSet()
	{
		wxString vset;
		int x = m_apcComboBoxes[4]->GetCurrentSelection();

		vset = m_apcComboBoxes[4]->GetString(x);

		return vset;
	}

public:
	DECLARE_CLASS(SP_DLG_FspnSimResult)
	SP_DLG_FspnSimResult(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), const wxString& p_sTitle = wxT("Fuzzy Stochastic Simulation"), long p_nStyle =
		wxDEFAULT_DIALOG_STYLE);

	virtual ~SP_DLG_FspnSimResult();
	virtual void DirectExportToCSV();

};

#endif // __SP_DS_FSPNSIMRESULTS_H__