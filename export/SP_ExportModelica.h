/*
 * SP_ExportModelica.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 11.8.2010
 * Short Description:
 */
 //==============================================================================

#ifndef SP_EXPORTMODELICA_H_
#define SP_EXPORTMODELICA_H_

#include "export/SP_ExportPT.h"

class SP_ExportModelica : public SP_ExportPT
{
 private:
	   wxString m_sConnections;
	   SP_MapLong2Long l_mNodeInConnection;
	   SP_MapLong2Long l_mNodeOutConnection;
 private:
	   //places transformation
	   bool TransformPlaces();
	   //Transitions transformations
	   bool TransformTransitions();
	   //edges transformation
	   bool TransformEdges();
	   bool TransformParameters();
	   wxString MakeModelicaPlace(const wxString & p_sPlaceType, SP_DS_Node* p_pcPlace);

	   wxString MakeModelicaContinuousTransition(SP_DS_Node* p_pcTrans);
	   wxString MakeModelicaStochasticTransition(SP_DS_Node* p_pcTrans);
	   wxString MakeModelicaDiscreteTransition(SP_DS_Node* p_pcTrans);

       void AddConnection(SP_DS_Edge* p_pcEdge);
	   long GetNodeSourceEdgesCount(SP_DS_Node* p_pcNode);
	   long GetNodeTargetEdgesCount(SP_DS_Node* p_pcNode);

	   //Check for existing normal edge between Two nodes
	   bool CheckForExistingEdge(SP_DS_Node* p_pSourceNodes, SP_DS_Node* p_pcDestNode);

	   //Get the Arcs' weights
	   wxString GetArcWeight(SP_DS_Edge* p_pcEdge,const wxString& p_sArcAddedWeight);

	   //Construct Edges information
	   wxString GetNodeArcsWeights(SP_DS_Node* p_pcNode);

	   //Get the extended edge multiplicity
	   wxString GetExtendedArcsMultiplicity(long p_nArcPosition,SP_DS_Node* p_pSourceNodes,SP_DS_Node* p_pcDestNode);

	   wxString MakeConnector(SP_DS_Edge* pcEdge,long p_nOutIndex,long p_nInIndex);

	   wxString GetNodeName(SP_DS_Node* p_pcNode);

	   wxCheckBox *m_GenerateSimulationFile;

	   wxTextCtrl* m_pcStartSimulationTimeCtrl,*m_pcEndSimulationTimeCtrl,*m_pcSimulationInterval,*m_pcTolerance;


  public:

	    bool DoWrite();
	    bool AcceptsDoc(SP_MDI_Doc* p_doc);

	    inline virtual wxString GetName() { return wxT("Export to Modelica ");}
	    inline virtual wxString GetDescr() { return wxT("Export a Petri Net in Modelica Format");}
	    virtual wxString GetExtension(){ return wxT("mo");}
	    virtual wxString GetFileDescr(){ return wxT("Modelica Files");}

	    virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);
	    bool WriteSimulationFile(const wxString& p_sFileName);
	    bool ValidateSimulationValues();

	    SP_ExportModelica();
	    virtual ~SP_ExportModelica();

};

#endif /* SP_EXPORTMODELICA_H_ */
