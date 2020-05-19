/*
 * SP_DLG_CPNSimulationResults.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 31.5.2010
 * @Description
 * Simulation result dialog for Continuous PN
 */
 //======================================================================================

#ifndef SP_DLG_CPNSIMULATIONRESULTS_H_
#define SP_DLG_CPNSIMULATIONRESULTS_H_

/*
 * This class implements the simulation dialog for continuous PN
 */
#include "sp_gui/dialogs/SP_DLG_Simulation.h"

#include "sp_ds/extensions/SP_DS_SimulatorThread.h"

//class SP_DS_ThreadEvent;
class SP_DLG_CPNSimulationResults : public SP_DLG_Simulation
{
 protected:
	      // replace values less than  l_nReplacedVaue
          bool  m_bReplaceValue;

          double m_nReplacedVaue;
 protected:
		wxChoice* m_pcMarkingSetComboBox;
	    wxChoice* m_pcFunctionSetComboBox;
	    wxChoice* m_pcParameterSetComboBox;

	    //simulator type: bio or server semantics
	    wxChoice* m_pcsemanticsComboBox;

	    //ODE solver: Euler, RK, ... etc
	    wxChoice* m_pcSolverComboBox;


	   // wxRadioBox * m_pcSolverType;
	    wxButton * m_pcSimulationProperites;

	    //ODE properties button
	    wxButton * m_pcODESolverProperites;

	    wxStaticText* m_pcCurrentSimulationTimeTxtCtrl;

	    unsigned long m_nLastUpdateTime;
	    
	    bool m_bIsSimulatorInitialized;

	    bool m_nRedraw;

	    //continuous simulation semantics (bio or server)
	    unsigned long m_nSimulationSemantics;

 protected:
	    DECLARE_EVENT_TABLE();
	    virtual void OnModifyMarkingSets( wxCommandEvent& p_cEvent );

	    virtual void OnModifyFunctionSets( wxCommandEvent& p_cEvent );

	    virtual void OnModifyParameterSets( wxCommandEvent& p_cEvent );

	    virtual void OnMarkingSetChanged( wxCommandEvent& p_cEvent );

	    virtual void OnParameterSetChanged( wxCommandEvent& p_cEvent );

		virtual void OnConstantsSetChanged(wxCommandEvent& p_cEvent);

	    virtual void OnFunctionSetChanged( wxCommandEvent& p_cEvent );

	    //call the special csv export dialog
	    virtual void OnExportToCSV();

protected:
	//virtual void     OnSimulatorThreadEvent(SP_DS_ThreadEvent& evt);
	    //on ODE solver changed
	    virtual void OnSolverChanged(wxCommandEvent& p_cEven);

	    //on semantics changed
	    virtual void OnSemanticsChanged(wxCommandEvent& p_cEven);

	    virtual void  LoadSets();

	    virtual void SaveODE(wxCommandEvent& p_cEvent);

	    virtual void LoadTransitions();
	    virtual void LoadConnections();
	    virtual void LoadConnectionOfType(const wxString& p_sArcType,const spsim::ConnectionType& p_ArcType);

	    virtual wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);
	    virtual bool IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue);

		virtual bool IsEvaluatedArcWeight(const wxString& p_sArcWeight, double& p_dVal);//by george

	    virtual void LoadPlaces();

	    virtual void DoStartSimulation();
         
	    //Check if we need to update the simulation dialog
	    virtual void UpdateSimulationDialog(const unsigned long& p_nGeneratedResultPointCount);

	    virtual void LoadColAuxResults(){}

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
	    virtual  spsim::Simulator* CreateSimulator(const int& p_nSimulatorType);

	    virtual int LoadSimulatorProperties();

	     virtual void SaveSimulatorProperties();

	     //add solver names
	     void AddODESoverNameToComboBox();

	     //destroy the Old ODE and create a new one
	     void ChangeODESolver();
 public:

	    DECLARE_CLASS( SP_DLG_CPNSimulationResults )
	    SP_DLG_CPNSimulationResults(
	    		SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent,
	    		wxString p_sHelpText = wxT(""),wxString p_sTitle = wxT("Continuous Simulation results"),
	    									long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	virtual void SetSimulationCurrentTime(const double & p_nTime){m_pcCurrentSimulationTimeTxtCtrl->SetLabel(wxString::Format(wxT("%.3f units"), (float)p_nTime));}

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);

	~SP_DLG_CPNSimulationResults();

	virtual void DirectExportToCSV();
	
	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView,SP_VectorDouble& p_anXAxisValues);
	 
};

#endif /* SP_DLG_CPNSIMULATIONRESULTS_H_ */
