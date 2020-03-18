
/*
* SP_DLG_ColFCPNSimulationResults.h
* $Author: G. Assaf
* $Version: 0.0 $
* $Revision : 0.0 $
* $Date: 3.03.2020
* @Description
* Simulation result dialog for Colored Fuzzy Continuous PN
* 
*/
#ifndef SP_DLG_ColFCPNSimulationResults_H_
#define SP_DLG_ColFCPNSimulationResults_H_

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <math.h>
#include <wx/ffile.h>
#include <wx/textfile.h>

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNSimulationResults.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

//Special for colored PN
#include "sp_ds/extensions/Col_CPN/SP_DS_ColPlaceODE.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"

#include "spsim/spsim.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColTraceAnalyzer.h"

#include <wx/regex.h>


//by george for constants harmonizing
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"//constants harmonizing by george


#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FCPNSimulationResults.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNSimulation.h"


class SP_DLG_ColFCPNSimulationResults : public SP_DS_ColoredPNSimulation, public SP_DLG_FCPNSimulationResults
{
private:
	SP_SetString m_choicesForColPNs;//by george
	int m_iModifyCount;//by george, this data member prevents load doublicate two identical CollistAttributes
	int m_nGroupCounts;//by george, this member counts the extra comboboxes (extra constants groups)
private:
	DECLARE_EVENT_TABLE();
	virtual void OnModifyMarkingSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyFunctionSets(wxCommandEvent& p_cEvent);

	//virtual void OnModifyParameterSets(wxCommandEvent& p_cEvent);

	virtual void OnMarkingSetChanged(wxCommandEvent& p_cEvent);

	//virtual void OnParameterSetChanged(wxCommandEvent& p_cEvent);

	virtual void OnFunctionSetChanged(wxCommandEvent& p_cEvent);

	virtual bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces);

	virtual void  LoadSets();

	virtual void  LoadConstantsSetsForColPN();

	virtual void SaveODE(wxCommandEvent& p_cEvent);

    virtual void LoadColAuxResults();

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

private:

	virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow);
	virtual bool IsMarkingSetNameExist(const wxString& p_sName);

	virtual void LoadTransitions();
	virtual void LoadConnections();
	//virtual void LoadParameters();
	virtual void LoadPlaces();
	virtual void UpdateChoices();//by george for constant harmonizing
	 
	void OnModifyConstants(wxCommandEvent& p_cEvent);//by george

	virtual void LoadNetInformation() {}

	virtual void UpdateXAxisValues();

	spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);
public:
	DECLARE_CLASS(SP_DLG_ColFCPNSimulationResults)
	SP_DLG_ColFCPNSimulationResults(SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
		wxString p_sTitle = wxT("Colored fuzzy continuous simulation results"), long p_nStyle = wxDEFAULT_DIALOG_STYLE);


	~SP_DLG_ColFCPNSimulationResults();

	virtual bool ComputeAuxiliaryVars();

	virtual void DirectExportToCSV();

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues);
};

#endif /* SP_DLG_ColFCPNSimulationResults_H_ */