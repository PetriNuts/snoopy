/*
 * SP_DLG_HybridSimulationResults.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 2.8.2010
 * Short Description:
 */
//==============================================================================
#ifndef SP_DLG_HYBRIDSIMULTIONRESULTS_H_
#define SP_DLG_HYBRIDSIMULTIONRESULTS_H_

/*
 * This class implements the simulation dialog for Hybrid PN
 */
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

class SP_DS_ThreadEvent;
class SP_DS_SimulatorThreadHybrid;
class SP_DLG_HybridSimulationResults: public SP_DLG_Simulation
{

private:

protected:
	bool m_bIsSimulatorInitialized;
	SP_VectorDouble m_anCurrentMarking;

	double m_nLastUpdateTime;

	bool m_nRedraw;

	SP_DS_SimulatorThreadHybrid* m_pcWorkerThread;

	wxString m_sSimulatorType;

	//currently selected ODE solver
	int m_nCurrentODESimulator;

	//number of continuous places
	unsigned long m_nContinuousPlaceCount;

protected:
	wxChoice* m_pcMarkingSetComboBox;
	wxChoice* m_pcFunctionSetComboBox;
	wxChoice* m_pcStochasticRateSetComboBox;
	wxChoice* m_pcImmediateSetComboBox;
	wxChoice* m_pcDelaySetComboBox;
	wxChoice* m_pcScheduledSetComboBox;

	wxChoice* m_pcParameterSetComboBox;

	wxChoice* m_pcContinuousSolver;
	wxChoice* m_pcTimeSyncComboBox;
	wxButton * m_pcSimulationProperites;

protected:
DECLARE_EVENT_TABLE()
	;

	virtual void OnModifyMarkingSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyFunctionSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyStochasicSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyImmediateSets(wxCommandEvent& p_cEvent);
	virtual void OnModifyDelaySets(wxCommandEvent& p_cEvent);
	virtual void OnModifyScheduledSets(wxCommandEvent& p_cEvent);

	virtual void OnMarkingSetChanged(wxCommandEvent& p_cEvent);
	virtual void OnFunctionSetChanged(wxCommandEvent& p_cEvent);
	virtual void OnStochasticRateChanged(wxCommandEvent& p_cEvent);

	virtual void OnImmediateSetChanged(wxCommandEvent& p_cEvent);
	virtual void OnDelaySetChanged(wxCommandEvent& p_cEvent);
	virtual void OnScheduledSetChanged(wxCommandEvent& p_cEvent);


	virtual void OnODESolverProperties(wxCommandEvent& p_cEvent);

	//Synchronization method
	virtual void OnTimeSyncTypeChanged(wxCommandEvent& p_cEven);

	virtual void OnSolverChanged(wxCommandEvent& p_cEven);

	virtual void SaveODE(wxCommandEvent& p_cEvent);

	virtual void OnTimer(wxTimerEvent& event);

protected:

	virtual void LoadPlacesOfType(const wxString& p_sPlaceType, unsigned long& p_nStartPos);

	virtual void ChangeODESolver();

	virtual void UpdateViewer();

	virtual void UpdateSimulationMatrix();

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

	virtual void LoadNetInformation();

	virtual void UpdateXAxisValues();

	virtual void LoadSets();

	virtual bool InitializeSimulator();

	virtual void LoadPlaces();

	virtual void LoadTransitions();

	virtual void LoadConnections();

	virtual void LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType);

	virtual void DoStartSimulation();

	virtual wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);
	virtual bool IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue);

	virtual double GetSimulatorProgress();

	virtual spsim::Simulator* CreateSimulator(const int& p_nSimulatorType);

	virtual int GetCureentSelectedSimulator()
	{
		return m_pcTimeSyncComboBox->GetSelection();
	}

	virtual int LoadSimulatorProperties();

	virtual void SaveSimulatorProperties();

public:

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);DECLARE_CLASS( SP_DLG_HybridSimulationResults )
	SP_DLG_HybridSimulationResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Hybrid Simulation results"), long p_nStyle =
			wxDEFAULT_DIALOG_STYLE);
	virtual ~SP_DLG_HybridSimulationResults();

	virtual void DirectExportToCSV();

	virtual void OnSimulatorThreadEvent(SP_DS_ThreadEvent& event);

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues);
};

#endif /* SP_DLG_HYBRIDSIMULTIONRESULTS_H_ */
