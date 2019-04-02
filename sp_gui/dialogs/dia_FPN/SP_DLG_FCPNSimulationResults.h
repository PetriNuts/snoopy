/*
* SP_DLG_FCPNSimulationResults.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 27.01.2019
* @Description
* Simulation result dialog forFuzzy Continuous PN
*/
//======================================================================================

#ifndef SP_DLG_FCPNSIMULATIONRESULTS_H_
#define SP_DLG_FCPNSIMULATIONRESULTS_H_

/*
* This class implements the simulation dialog for Fuzzy continuous PN
*/
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include"sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include "FuzzyReasoning.h"
#include<mutex>
typedef std::vector<TriangularFN> TFN_List;
class SP_DLG_FCPNSimulationResults : public SP_DLG_CPNSimulationResults
{
protected:
	wxTextCtrl* m_lalphaLevels;
	wxTextCtrl* m_lSamplePoints;
	wxChoice*   m_pcSamplingchoices;
	unsigned long m_lpCount;
	unsigned long m_lnFuzzyNum;
	bool m_initialRun;
	//SP_Compressed_Fuzzy_Band m_compressedFuzzyBand =  SP_Compressed_Fuzzy_Band();
protected:
	SP_Vector2DDouble           m_paramMatrix;
	std::map<wxString,wxString> m_mTransParamNames;
	std::map<wxString, int>  m_mFuzzyParam2Position;
	std::vector<wxString>    m_fuzzyParams;
	std::vector<int>         m_fuzzyParamPositions;
	std::vector<double>      m_vCurrentSample;
	ResultFuzzyBand          m_vResultFBand;
	FuzzyReasoning           m_fr;
	long                     m_lSamplingStrategyselection;
	long                     m_lSimTim;
	std::recursive_mutex   m_mutex;
	//std::condition_variable m_finished;
	//SP_SetString m_samplingStrategies;
protected:
	 

	//call the special csv export dialog
  virtual void OnExportToCSV();

protected:

	virtual void InitializeParamMatrix();

	virtual void  ConvertTFNListToParamMatrix(std::vector<TriangularFN> vTriangasTFNParams);

	virtual void InitializeFuzzySetting();

	virtual std::vector<double> GetCurentSamples(int iIteration);

	virtual std::vector<TriangularFN> LoadParams();

	virtual SP_VectorDouble  GetFNConstants(const wxString &val);

	virtual void LoadParameters(unsigned long, double dAlpha);

	virtual bool InitializeSimulator(unsigned long ulite, double dalpha);

	std::vector<double> GetCombinationVectorForTopLevel();

	virtual void LoadUsedParams();

	virtual wxString GetKParameter(const wxString& strfunc);

	virtual void* DoFcpnSimulation();

	virtual void* DoNormalFcpnSimulation();

 

	virtual void DoStartSimulation();

	 
public:

	DECLARE_CLASS(SP_DLG_FCPNSimulationResults)
	SP_DLG_FCPNSimulationResults(
		SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent,
		wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Fuzzy Continuous Simulation results"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	~SP_DLG_FCPNSimulationResults();
	//virtual void SetSimulationCurrentTime(const double & p_nTime) { m_pcCurrentSimulationTimeTxtCtrl->SetLabel(wxString::Format(wxT("%.3f units"), (float)p_nTime)); }

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	virtual void DirectExportToCSV();
 
	 
};

#endif /* SP_DLG_FCPNSIMULATIONRESULTS_H_ */
