/*
* SP_DLG_FCPNSimulationResults.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 27.01.2019
* @Description
* Simulation result dialog forFuzzy Continuous PN
*/
//=====================================================================================

#ifndef SP_DLG_FCPNSIMULATIONRESULTS_H_
#define SP_DLG_FCPNSIMULATIONRESULTS_H_

/*
* This class implements the simulation dialog for Fuzzy continuous PN
*/
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include"sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include "FuzzyReasoning.h"
#include "SP_DS_FcpnSimulThread.h"
#include "SP_DS_FuzzyResult_Thread.h"
#include<wx/busyinfo.h>
#include "sp_gui/dialogs/dia_FPN/SP_DLG_SamplingProperties.h"

typedef std::vector<TriangularFN> TFN_List;

class SP_DLG_FCPNSimulationResults : public SP_DLG_CPNSimulationResults
{
protected:
	wxTextCtrl* m_lalphaLevels;
	wxTextCtrl* m_lSamplePoints;
	wxChoice*   m_pcSamplingchoices;
	unsigned long m_lpCount;
	unsigned long m_lnFuzzyNum;
	bool         m_initialRun;
	wxBusyInfo* m_info;
	long m_lruncounter;
	SP_DS_Metadata* m_pcFuzzySimProp;//fuzzy properties

protected:
	SP_Vector2DDouble           m_paramMatrix;
	std::map<wxString, wxString> m_mTransParamNames;
	std::map<wxString, int>  m_mFuzzyParam2Position;
	std::vector<wxString>    m_fuzzyParams;
	std::vector<int>         m_fuzzyParamPositions;
	std::vector<double>      m_vCurrentSample;
	ResultFuzzyBand          m_vResultFBand;
	fuzzification::FuzzyReasoning *        m_fr;
	long                     m_lSamplingStrategyselection;
	long                     m_lSimTim;
	bool                     m_bIsAbort;
	long                     m_lTotalSimRuns;
	wxStopWatch              m_stopWatch;
	//std::map<wxString, wxString>  m_mSimOptions;
	bool render_loop_on;
	std::vector<SP_DS_FcpnSimulThread*> m_fcpnThreadVector;

protected:


	//call the special csv export dialog
	virtual void OnExportToCSV();

protected:

	virtual void InitializeParamMatrix();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> vTriangasTFNParams);

	virtual bool InitializeFuzzySetting();

	virtual std::vector<double> GetCurentSamples(int iIteration);

	virtual std::vector<TriangularFN> LoadParams();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual void LoadParameters(unsigned long, double dAlpha);

	virtual void  LoadTransitions();


	virtual bool InitializeSimulator(unsigned long ulite, double dalpha);

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual void   LoadUsedParams();

	virtual void*  RunBasicSamplingFcpnThreadSim();

	virtual void*  RunReducedSamplingFCPNThreadSim();

	virtual wxString GetKParameter(const wxString& strfunc);

	virtual void  SetSimulationProgressText(long& p_nValue);

	virtual void  SetSimulationProgressGauge(long p_nValue);

	virtual void  SetSimulationProgressGaugeRange(long p_nRangeValue);

	virtual void   OnSimulatorThreadEvent(SP_DS_ThreadEvent& event);


	virtual void   OnDlgCancel(wxCommandEvent& p_cEvent);

	virtual wxString  GetFuzzySettingAttribute(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	virtual void  SaveFuzzySimulatorProperties();

	virtual void OnSamplingProperties(wxCommandEvent& p_cEvent);//1/2021

	virtual void OnSamplingAlgoChanged(wxCommandEvent& p_cEvent);

	virtual fuzzification::SAMPLING_TYPE GetSelectedSamplingMethod();

	void onIdle(wxIdleEvent& evt);
public:

	void activateRenderLoop(bool on);
	DECLARE_CLASS(SP_DLG_FCPNSimulationResults)
	SP_DLG_FCPNSimulationResults(
		SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent,
		wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Fuzzy Continuous Simulation"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	~SP_DLG_FCPNSimulationResults();
	//virtual void SetSimulationCurrentTime(const double & p_nTime) { m_pcCurrentSimulationTimeTxtCtrl->SetLabel(wxString::Format(wxT("%.3f units"), (float)p_nTime)); }

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);
	//virtual void OnStartAbortSimulation1();

	virtual void DirectExportToCSV();

	wxDECLARE_EVENT_TABLE();
};

#endif /* SP_DLG_FCPNSIMULATIONRESULTS_H_ */
