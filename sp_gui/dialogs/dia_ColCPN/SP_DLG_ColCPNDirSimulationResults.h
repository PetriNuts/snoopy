
#ifndef SP_DLG_ColCPNDirSimulationResults_H_
#define SP_DLG_ColCPNDirSimulationResults_H_


#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNColDirSimResults.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNDirSimulation.h"


class SP_DLG_ColCPNDirSimulationResults : public SP_DS_ColoredPNDirSimulation, public SP_DLG_CPNColDirSimResults
{
private:

private:
	DECLARE_EVENT_TABLE();
	virtual void OnModifyMarkingSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyFunctionSets(wxCommandEvent& p_cEvent);

	virtual void OnModifyParameterSets(wxCommandEvent& p_cEvent);

	virtual void OnMarkingSetChanged(wxCommandEvent& p_cEvent);

	virtual void OnParameterSetChanged(wxCommandEvent& p_cEvent);

	virtual void OnFunctionSetChanged(wxCommandEvent& p_cEvent);

	virtual bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces);

	virtual void  LoadSets();

	virtual void SaveODE(wxCommandEvent& p_cEvent);

	virtual void LoadColAuxResults();

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView);

	// to get ode string 
	wxString GetODEString(SP_DS_Node* p_pcPlaceNode);
	wxString FillInPostConnection(SP_DS_Node* p_pcPlaceNode);
	wxString FillInPreConnection(SP_DS_Node* p_pcPlaceNode);
	
	// Transition with guard
	wxString FillInPreConnguard(SP_DS_Node* p_pcPlaceNode, wxString p_sVariablval);
	wxString FillInPostConnguard(SP_DS_Node* p_pcPlaceNode, wxString p_sVariablval);
	wxString GetODETranswGuard(SP_DS_Node* p_pcPlaceNode);

	//wxString GetArcMultiplicity(wxString l_psArcExprs);
	wxString GetTransRate(wxString l_spTransrate);
private:
	//const wxString m_pcSimulatedclass;
	SP_VectorString m_msColTransitionNames;
	virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow);
	virtual bool IsMarkingSetNameExist(const wxString& p_sName);
	
	virtual void LoadTransitions();
	virtual void LoadConnections();
	virtual void LoadParameters();
	virtual void LoadPlaces();

	virtual void LoadNetInformation() {}

	virtual void UpdateXAxisValues();
	bool Testcolorsetofallplaces();
	//bool TestArcVariable();
	spsim::ConnectionType GetConnectionType(const wxString& p_sConnectionType);

protected: 
	wxString m_sNetClass;
	vector<double> m_adInitialNetMarking;
	vector<vector<double> > m_anColCPNNetMarkings;
	vector<wxString>  m_vsTransNodeType;
	SP_VectorString m_msColTransitionFunctions;
	vector<SP_VectorString> m_anColNetFunctions;
	SP_VectorString m_msColorSetOfPlacetest;
	SP_VectorString m_pcEdgeVariabletest;
	SP_VectorString* Getedgvariable() { return &m_pcEdgeVariabletest; }
	// to construct ode
	//pre Arcs
	vector<SP_DS_Edge*> m_pcpreEdgeVector;
	//post Arcs
	vector<SP_DS_Edge*> m_pcpostEdgeVector;
	SP_CPN_ColorSetClass m_ColorSetClass;
	SP_VectorString m_msColPreTransNames;
	SP_CPN_ValueAssign m_ValueAssign;
	SP_VectorString m_msColSetofPlace;
	wxString m_sColPlaceName;
	SP_MapString2Long m_mnPlaceArrayPos;
public:
	DECLARE_CLASS(SP_DLG_ColCPNSimulationResults)
	SP_DLG_ColCPNDirSimulationResults(SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Coloring* p_pcColoreddNet, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
		wxString p_sTitle = wxT("Direct colored continuous  simulation results"), long p_nStyle = wxDEFAULT_DIALOG_STYLE);


	~SP_DLG_ColCPNDirSimulationResults();

	virtual bool ComputeAuxiliaryVars();

	virtual void DirectExportToCSV();

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues);

	

	SP_VectorString* GetColoredTransitionNames() { return &m_msColTransitionNames; }
	
	unsigned int GetContinuousPlaceCount() { return m_anColCPNNetMarkings.size(); }
	vector<vector<double> > * GetColCPNNetMarkings() { return &m_anColCPNNetMarkings; }
	
	vector<wxString>* GetTransNodeType() { return &m_vsTransNodeType; }
	vector<SP_VectorString>* GetColNetFunctions() { return &m_anColNetFunctions; }
	void LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType);
	//wxString GetEdgeWeightE(SP_DS_Edge* p_pcEdge);
	//wxString GetArcVariable(SP_DS_Edge* p_pcEdge);
	//wxString ArcWeightAfterevaluateGuard(SP_DS_Node* l_pcSourceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* l_pcTargetNode);
};

#endif /* SP_DLG_ColCPNDirSimulationResults_H_ */
