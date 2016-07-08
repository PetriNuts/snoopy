

#ifndef SP_DLG_CPNColDirSIMRESULTS_H_
#define SP_DLG_CPNColDirSIMRESULTS_H_

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Coloring.h"
#include "sp_gui/dialogs/SP_DLG_DirColorSimulation.h"

class SP_DLG_CPNColDirSimResults : public SP_DLG_DirColorSimulation
{
private:

protected:
	wxChoice* m_pcMarkingSetComboBox;
	wxChoice* m_pcFunctionSetComboBox;
	wxChoice* m_pcParameterSetComboBox;
	wxChoice* m_pcSolver;
	wxRadioBox * m_pcSolverType;
	wxButton * m_pcSimulationProperites;

	wxStaticText* m_pcCurrentSimulationTimeTxtCtrl;

	unsigned long m_nLastUpdateTime;

	bool m_bIsSimulatorInitialized;

	bool m_nRedraw;

	int m_nContinuousSimulator;
	wxString m_sContinuousSimulator;
protected:
	DECLARE_EVENT_TABLE();
	//SP_DS_ColPN_Coloring* m_pcColoredNet;
	virtual void OnModifyMarkingSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyFunctionSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyParameterSets(wxCommandEvent& p_cEvent);

	virtual void OnMarkingSetChanged(wxCommandEvent& p_cEvent);

	virtual void OnParameterSetChanged(wxCommandEvent& p_cEvent);

	virtual void OnFunctionSetChanged(wxCommandEvent& p_cEvent);

	
	virtual void OnSolverTypeChanged(wxCommandEvent& p_cEven);
	//virtual int LoadSimulatorProperties();

protected:
	
	virtual void OnSolverChanged(wxCommandEvent& p_cEven);

	virtual void  LoadSets();

	virtual void SaveODE(wxCommandEvent& p_cEvent);

	virtual void LoadTransitions();
	virtual void LoadConnections();
	virtual void LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType);

	virtual wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);
	virtual bool IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue);

	virtual void LoadPlaces();

	virtual void DoStartSimulation();

	//Check if we need to update the simulation dialog
	virtual void UpdateSimulationDialog(const unsigned long& p_nGeneratedResultPointCount);

	virtual void LoadColAuxResults() {}

	virtual void UpdateViewer(SP_DS_Metadata* p_pcView = nullptr);

	//select the rate or marking matrix
	virtual void UpdateSimulationMatrix(SP_DS_Metadata* p_pcView = nullptr);

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

	virtual void LoadNetInformation();

	virtual void UpdateXAxisValues();

	virtual bool InitializeSimulator();

	//get the current combo box selection
	virtual int GetCureentSelectedSimulator();

	//create a simulator based on the child class type
	//virtual  spsim::Simulator* CreateColSimulator(const int& p_nSimulatorType);
	virtual  spsim::ColorSimulator* CreateSimulator(const int& p_nSimulatorType);
	
	
public:

	DECLARE_CLASS(SP_DLG_CPNColDirSimResults)
	SP_DLG_CPNColDirSimResults(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent,
		wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Direct Colored Continuous  Simulation"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	virtual void SetSimulationCurrentTime(const double & p_nTime) { m_pcCurrentSimulationTimeTxtCtrl->SetLabel(wxString::Format(wxT("%.3f units"), (float)p_nTime)); }

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	~SP_DLG_CPNColDirSimResults();

	virtual void DirectExportToCSV();

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues);
};

#endif /* SP_DLG_CPNColDirSIMRESULTS_H_ */
