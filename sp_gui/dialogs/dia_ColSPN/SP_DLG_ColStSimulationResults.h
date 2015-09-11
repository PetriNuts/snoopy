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

#ifndef SP_DLG_ColStSimulationResults_H_
#define SP_DLG_ColStSimulationResults_H_

#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNSimulation.h"


class  SP_DLG_ColStSimulationResults : public SP_DS_ColoredPNSimulation, public SP_DLG_StSimulationResults
{
 private :

	      wxString m_sOutputType;

		  SP_Vector2DDouble m_aanTempAverageAuxPLVarsResults;

private:
	
	   virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName,const int& p_nRow);
	   virtual bool IsMarkingSetNameExist(const wxString& p_sName);


      virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

protected:

       virtual void LoadTransitions();
       virtual void LoadConnections();
       virtual void LoadParameters();
       virtual void LoadPlaces();

       spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);

       spsim::TransitionType GetTransitionType(const wxString& p_sTransitionType);

protected :
	   DECLARE_EVENT_TABLE()

      virtual bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer,SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces);
      
      virtual void LoadSets();

	  virtual void LoadSimulatorData();

      virtual void OnModifyMarkingSets( wxCommandEvent& p_cEvent );
      virtual void OnModifyFunctionSets( wxCommandEvent& p_cEvent );
      
	  virtual void OnModifyDelaySets( wxCommandEvent& p_cEvent ); //By Liu
	  virtual void OnModifyScheduleSets( wxCommandEvent& p_cEvent ); //By Liu
	  virtual void OnModifyWeightSets( wxCommandEvent& p_cEvent ); //By Liu

	  virtual void OnModifyParameterSets(wxCommandEvent& p_cEvent);

	  virtual void LoadResults();

	  virtual void OnLoadApFormulae(wxCommandEvent& p_cEvent);
	  virtual void OnEnterApFormulae(wxCommandEvent& p_cEvent);

       virtual void CheckApFormulae(wxCommandEvent& p_cEvent);

	  virtual void LoadColAuxResults();

	  virtual void UpdateXAxisValues();
	 
  public:
	DECLARE_CLASS( SP_DLG_ColStSimulationResults )
    SP_DLG_ColStSimulationResults( SP_DS_Graph* p_pcGraph,
    		                    SP_DS_ColPN_Unfolding* p_pcUnfoldedNet,
								wxWindow* p_pcParent,
								wxString p_sHelpText = wxT(""),
								const wxString& p_sTitle = wxT("Simulation results"),
								long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    ~SP_DLG_ColStSimulationResults();

    virtual void DirectSingleExportToCSV(long p_nSimulatinRunNumber);
    virtual void DirectSingleExactExportToCSV(long p_nSimulatinRunNumber , double p_nTimepoint, const SP_VectorDouble& p_anMarking, const SP_VectorDouble& p_anHazardValues, long p_nCurrentTimePoint);
   // virtual void LoadResults();
    
	virtual bool ComputeAuxiliaryVars();
	virtual void ComputeAverageAuxiliaryVars(long p_nSimulationRunNumber, long p_nSimulationRunCount);
	virtual void ComputeEachSimulationAuxiliaryVars();
	
	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView,SP_VectorDouble& p_anXAxisValues);
};

#endif // __SP_DLG_ColStSimulationResults__

