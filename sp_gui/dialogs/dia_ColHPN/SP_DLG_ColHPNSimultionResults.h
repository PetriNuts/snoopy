/*
 * SP_DLG_ColHPNSimultionResults.h
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 6.12.2011
 * $Updated: by George Assaf: constants harmonizing and supporting observers$
 * Short Description:
 */
 //==============================================================================

#ifndef SP_DLG_ColHPNSimultionResults_H_
#define SP_DLG_ColHPNSimultionResults_H_

/*
 * This class implements the simulation dialog for Colored Hybrid PN
 */

#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNSimulation.h"


class SP_DLG_ColHPNSimultionResults : public SP_DS_ColoredPNSimulation, public SP_DLG_HybridSimulationResults
{
 private://by george
	 SP_SetString m_choicesForColPNs;//by george
	 int m_iModifyCount;//by george, this data member prevents load doublicate two identical CollistAttributes
	 int m_nGroupCounts;//by george, this member counts the extra comboboxes (extra constants groups)
 protected:
	    DECLARE_EVENT_TABLE();
		virtual void OnModifyMarkingSets( wxCommandEvent& p_cEvent );
	    virtual void OnModifyFunctionSets( wxCommandEvent& p_cEvent );
	    virtual void OnModifyStochasicSets(wxCommandEvent& p_cEvent);
		virtual void OnModifyImmediateSets(wxCommandEvent& p_cEvent);
	    virtual void OnModifyDelaySets(wxCommandEvent& p_cEvent);
		virtual void OnModifyScheduledSets(wxCommandEvent& p_cEvent);
	    virtual void OnModifyParameterSets( wxCommandEvent& p_cEvent );

	    virtual void OnMarkingSetChanged( wxCommandEvent& p_cEvent );
		virtual void OnFunctionSetChanged( wxCommandEvent& p_cEvent );	    
	    virtual void OnStochasticRateChanged( wxCommandEvent& p_cEvent );
		virtual void OnImmediateSetChanged(wxCommandEvent& p_cEvent);
	    virtual void OnDelaySetChanged(wxCommandEvent& p_cEvent);
		virtual void OnScheduledSetChanged(wxCommandEvent& p_cEvent);
		virtual void OnParameterSetChanged( wxCommandEvent& p_cEvent );

		virtual void  LoadSets();

		bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer,SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces);		

		void SaveODE(wxCommandEvent& p_cEvent);

		virtual void LoadColAuxResults();   // to be implemented

		virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

		virtual void LoadObservers();//by george

		virtual void  UpdateUnfoldedObservers();//by george

		virtual void  UpdateObservers(const wxString& p_sObsersverType, unsigned int p_nRowCount);//by george

		virtual wxString  SubstituteConstants(const wxString& p_sExpression);

		virtual void ComputeCombinedMatrix(const wxString& p_sObserverBody, std::map<std::string, unsigned int>& p_mapvar2Pos);//by george

		virtual  SP_FunctionPtr RegesterFunctionVariables(const wxString& p_sObserverType, const wxString& p_sObserverName, wxString p_sExpression);//by george


 protected: 	  

		//virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName,const int& p_nRow); // to be 
		//virtual bool IsMarkingSetNameExist(const wxString& p_sName); // to be

		virtual void LoadPlaces();
	    
	    virtual void LoadTransitions();
	    
	    virtual void LoadConnections();

	    virtual void LoadParameters();

	    virtual void LoadNetInformation(){}

	    virtual void UpdateXAxisValues();

		spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);
	    
		virtual void UpdateChoices();//by george for constant harmonizing
		virtual void LoadConstantsSetsForColPN();//by george for constants harmo
		void OnModifyConstants(wxCommandEvent& p_cEvent);//by george
		void OnConstantsSetsChanged(wxCommandEvent& p_cEvent);//by george
		
 public:

	    //virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent);
	    DECLARE_CLASS( SP_DLG_HybridSimulationResults )
	    SP_DLG_ColHPNSimultionResults(
	    		SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Unfolding* p_pcUnfoldedNet,wxWindow* p_pcParent,
	    		wxString p_sHelpText = wxT(""),wxString p_sTitle = wxT("Colored Hybrid Simulation results"),
	    									long p_nStyle = wxDEFAULT_DIALOG_STYLE);
	  virtual ~SP_DLG_ColHPNSimultionResults();

	  virtual void DirectExportToCSV();

	  virtual bool ComputeAuxiliaryVars(); // to be 

	  virtual void LoadResults();
	  
	  virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView,SP_VectorDouble& p_anXAxisValues);
};


#endif /* SP_DLG_ColHPNSimultionResults_H_ */
