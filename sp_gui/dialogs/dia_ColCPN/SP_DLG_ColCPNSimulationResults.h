/*
 * SP_DLG_ColCPNSimulationResults.h
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 31.5.2010
 * @Description
 * Simulation result dialog for Continuous PN
 */
 //======================================================================================

#ifndef SP_DLG_ColCPNSimulationResults_H_
#define SP_DLG_ColCPNSimulationResults_H_


#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNSimulation.h"


class SP_DLG_ColCPNSimulationResults : public SP_DS_ColoredPNSimulation, public SP_DLG_CPNSimulationResults
{
 private:
	 SP_SetString m_choicesForColPNs;//by george
	 int m_iModifyCount;//by george, this data member prevents load doublicate two identical CollistAttributes
	 int m_nGroupCounts;//by george, this member counts the extra comboboxes (extra constants groups)
 private:
	    DECLARE_EVENT_TABLE();
	    virtual void OnModifyMarkingSets( wxCommandEvent& p_cEvent );

	    virtual void OnModifyFunctionSets( wxCommandEvent& p_cEvent );

	    virtual void OnModifyParameterSets( wxCommandEvent& p_cEvent );

	    virtual void OnMarkingSetChanged( wxCommandEvent& p_cEvent );

	    virtual void OnParameterSetChanged( wxCommandEvent& p_cEvent );

	    virtual void OnFunctionSetChanged( wxCommandEvent& p_cEvent );

	    virtual bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer,SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces);
	   
	    virtual void  LoadSets();

	    virtual void SaveODE(wxCommandEvent& p_cEvent);

	     virtual void LoadColAuxResults();

	     virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

private:

	    virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName,const int& p_nRow);
	    virtual bool IsMarkingSetNameExist(const wxString& p_sName);

		virtual void LoadTransitions();
		virtual void LoadConnections();
		virtual void LoadParameters();
		virtual void LoadPlaces();
		virtual void UpdateChoices();//by george for constant harmonizing
		virtual void LoadConstantsSetsForColPN();//by george for constants harmo
		void OnModifyConstants(wxCommandEvent& p_cEvent);//by george

		virtual void LoadNetInformation(){}

		virtual void UpdateXAxisValues();

		spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);
 public:
	    DECLARE_CLASS( SP_DLG_ColCPNSimulationResults )
	    SP_DLG_ColCPNSimulationResults(	SP_DS_Graph* p_pcGraph,SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, wxWindow* p_pcParent,wxString p_sHelpText = wxT(""),
										wxString p_sTitle = wxT("Colored continuous simulation results"),long p_nStyle = wxDEFAULT_DIALOG_STYLE);


	~SP_DLG_ColCPNSimulationResults();

	virtual bool ComputeAuxiliaryVars();

	virtual void DirectExportToCSV();
	
	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView,SP_VectorDouble& p_anXAxisValues);
};

#endif /* SP_DLG_ColCPNSimulationResults_H_ */
