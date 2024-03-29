#ifndef __SP_DS_FSPNSIMRESULTS_H__
#define __SP_DS_FSPNSIMRESULTS_H__
#include "FuzzyReasoning.h"
#include <wx/string.h>
#include <wx/thread.h>
#include "wx/busyinfo.h"
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "FuzzyBasics.h"
#include "FuzzyReasoning.h"
#include "TriangularFN.h"
#include "SP_DS_FspnSimulThread.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_SamplingProperties.h"

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
	wxBusyInfo*                 m_info;
	long                        m_lruncounter;
protected:
	SP_Vector2DDouble            m_paramMatrix;
	std::map<wxString, wxString> m_mTransParamNames;
	fuzzification::FuzzyReasoning *              m_fr;
	std::vector<int>             m_fuzzyParamPositions;
	std::vector<double>          m_vdCurrentSample;
	ResultFuzzyBand              m_ResultFBand;
	std::map<wxString, int>      m_mFuzzyParam2Position;
	std::vector<wxString>        m_fuzzyParams;
	bool                         m_bIsAbort;
	wxStopWatch m_stopWatch;
	std::vector<SP_DS_FspnSimulThread*> m_fspnThreadVector;
	SP_DS_Metadata* m_pcFuzzySimProp;//fuzzy properties
protected:


	virtual bool InitializeFuzzySetting();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual std::vector<double>  GetCurentSamples(int iteration);

	virtual std::vector<TriangularFN> LoadParams();

	virtual void   LoadUsedParams();

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> asTFNParams);


	virtual void  OnSimulatorSelect(wxCommandEvent& p_cEvent);

	virtual bool InitializeSimulator(unsigned long iteration, double alpha);

	virtual void LoadParameters(unsigned long, double alpha);

	virtual void InitializeParamMatrix();

	virtual void* DoBasicSamplingFspnSimulThread();

	virtual void * DoReducedSamplingFspnSimulThread();


	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	virtual wxString  GetKParameter(const wxString& func);


	virtual void OnModifyConstantSets(wxCommandEvent& p_cEvent);


	virtual void SetSimulationProgressText(long& p_nValue);



	virtual void  SetSimulationProgressGaugeRange(long p_nRangeValue);

	void SetSimulationProgressGauge(long p_nValue);

	void LoadTransitions();

	virtual void     OnSimulatorThreadEvent(SP_DS_ThreadEvent& event);

	virtual wxString GetCurrentSelectedSet()
	{
		wxString vset;
		int x = m_apcComboBoxes[4]->GetCurrentSelection();

		vset = m_apcComboBoxes[4]->GetString(x);

		return vset;
	}

	virtual void   OnDlgCancel(wxCommandEvent& p_cEvent);

	virtual wxString  GetFuzzySettingAttribute(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	void  SaveFuzzySimulatorProperties();

	virtual void OnSamplingAlgoChanged(wxCommandEvent& p_evt);

	virtual fuzzification::SAMPLING_TYPE GetSelectedSamplingMethod();

	virtual  void OnSamplingProperties(wxCommandEvent& p_cEvent);

public:
	DECLARE_CLASS(SP_DLG_FspnSimResult)
	SP_DLG_FspnSimResult(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), const wxString& p_sTitle = wxT("Fuzzy Stochastic Simulation"), long p_nStyle =
		wxDEFAULT_DIALOG_STYLE);

	virtual ~SP_DLG_FspnSimResult();
	virtual void DirectExportToCSV();

	wxDECLARE_EVENT_TABLE();

};

#endif // __SP_DS_FSPNSIMRESULTS_H__
