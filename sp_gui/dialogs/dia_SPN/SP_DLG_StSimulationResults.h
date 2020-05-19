//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.40 $
// $Date: 2007/08/14 11:20:00 $
/* @change M.Herajy 10.6.2010
 * @Description
 * Simulation result dialog for SPN,Colored PN, CSPN
 */
//////////////////////////////////////////////////////////////////////
#ifndef SP_DLG_STSIMULATIONRESULTS__
#define SP_DLG_STSIMULATIONRESULTS__

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

//F. Liu 2010.12.4
#include "sp_ds/extensions/ext_SPN/SP_DS_ApFormula.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_ApParser.h"
 
class SP_DS_ThreadEvent;
class SP_DS_SimulatorThreadStochastic;

class SP_DLG_StSimulationResults: public SP_DLG_Simulation
{
private:

protected:
	wxChoice* m_pcSimulatorComboBox;

	SP_DS_SimulatorThreadStochastic* m_pcWorkerThread;

	SP_DS_Metadata* m_pcMainTableMetadata;

protected:
	SP_DS_ApParser* m_pcApParser;
	SP_VectorApFormula m_aApFormulas;
	SP_VectorString m_sApFormulas;
	SP_VectorBool m_aApFormulaHappend;
	SP_VectorDouble m_aApResults;

	SP_VectorLong m_anCurrentMarking;
	



	void CheckApFormulae(wxCommandEvent& p_cEvent);

protected:

	virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow);
	virtual bool IsMarkingSetNameExist(const wxString& p_sName);

	virtual void UpdateViewer(SP_DS_Metadata* p_pcView = nullptr);

	void UpdateSimulationMatrix(SP_DS_Metadata* p_pcView = nullptr);

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

	virtual void UpdateXAxisValues();

	virtual void LoadPlaces();

	virtual void LoadTransitions();

	virtual void LoadConnections();

	virtual void LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType);

	virtual void DoStartSimulation();

	virtual wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);
	virtual bool IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue);

	virtual bool IsEvaluatedArcWeight(const wxString& p_sArcWeight, double& p_dVal);

	virtual double GetSimulatorProgress();

	virtual bool InitializeSimulator();

	virtual void LoadPlacesOfType(const wxString& p_sPlaceType, unsigned long& p_nPosition);

	virtual spsim::Simulator* CreateSimulator(const int& p_nSimulatorType);
	
	virtual int GetCureentSelectedSimulator()
	{
		return m_pcSimulatorComboBox->GetCurrentSelection();
	}
	

protected:
	DECLARE_EVENT_TABLE()

	virtual void LoadNetInformation();

	virtual void LoadSets();

	virtual void LoadSimulatorData();

	virtual void LoadColAuxResults()
	{
	}

	void OnSimulatorSelect(wxCommandEvent& p_cEvent);

	virtual void OnModifyMarkingSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyFunctionSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyParameterSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyDelaySets(wxCommandEvent& p_cEvent); //By Liu
	virtual void OnModifyScheduleSets(wxCommandEvent& p_cEvent); //By Liu
	virtual void OnModifyWeightSets(wxCommandEvent& p_cEvent); //By Liu
	virtual void OnLabelRightDClick(wxGridEvent& p_cEvent);

	virtual void OnCheckFormulaMenu(wxCommandEvent& p_cEvent);

	virtual void OnPrint(wxCommandEvent& p_cEvent);

	virtual void OnTimer(wxTimerEvent& event);

	virtual void OnLoadApFormulae(wxCommandEvent& p_cEvent);
	virtual void OnEnterApFormulae(wxCommandEvent& p_cEvent);
public:
	DECLARE_CLASS (SP_DLG_StSimulationResults)
	SP_DLG_StSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), const wxString& p_sTitle = wxT("Simulation results"), long p_nStyle =
			wxDEFAULT_DIALOG_STYLE);

	virtual ~SP_DLG_StSimulationResults();
	virtual void DirectExportToCSV();
	virtual void DirectSingleExportToCSV(long p_nSimulatinRunNumber);
	virtual void DirectSingleExactExportToCSV(long p_nSimulatinRunNumber, double p_nTimepoint, const SP_VectorDouble& p_anMarking, const SP_VectorDouble& p_anHazardValues, long p_nCurrentTimePoint);
	virtual void LoadResults();
	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	virtual void ComputeAverageAuxiliaryVars(long p_nSimulationRunNumber, long p_nSimulationRunCount)
	{
	}
	virtual bool ComputeAuxiliaryVars();

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues);

	virtual void OnSimulatorThreadEvent(SP_DS_ThreadEvent& event);

	virtual void DoSingleExport(const unsigned long& p_nCurrentRunNumber);

	virtual void   OnConstantsSetChanged(wxCommandEvent& p_cEvent);//by george

};

#endif // __SP_DLG_StSimulationResults__
