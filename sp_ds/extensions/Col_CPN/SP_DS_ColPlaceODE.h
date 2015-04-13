/*
 * SP_DS_PlaceODE.h
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 2.6.2010
 * Short Description:
 */
 //======================================================================================
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

#ifndef SP_DS_ColPlaceODE_H_
#define SP_DS_ColPlaceODE_H_


class SP_DS_ColPlaceODE : public SP_DS_PlaceODE
{



private:
	SP_DS_ColPN_Unfolding* m_pUnfoldedNet;



private:
	bool LoadPlacesOfType(const wxString& p_sPlacesType);
	bool LoadTransitions();
	bool LoadConnections();

	ConnectionType GetConnectionType(wxString p_sArcType);


public:

	   //Constructor	   
	   SP_DS_ColPlaceODE(SP_DS_Graph* p_pcGraph,SP_DS_ColPN_Unfolding* p_pUnfoldedNet);       //for colored continuous Petri Nets


/*
private:
	 long m_nPlaceNodeID;

	 SP_DS_ColPN_Unfolding* m_pUnfoldedNet;

private:
	map<long, vector<long> >* m_pmvPlaceID_PreTransitionID;
	map<long, vector<long> >* m_pmvPlaceID_PostTransitionID;
	vector<SP_VectorString>* m_panNetFunctions;

	map< long, SP_MapString2String >* m_pmPrePlaceToArcsOfTransitionNode;
	map< long, SP_MapString2String >* m_pmPostPlace2ArcsOfTransitionNode; 
	SP_VectorString* m_pmsPlaceNames;

	map<long, vector<wxString> >* m_pmvTransitionID2SpecialArcFunction;


	//void SetUnfoldedNet(SP_DS_ColPN_Unfolding* p_pUnfoldedNet) {  m_pUnfoldedNet = p_pUnfoldedNet; }

 public:

	   //Constructor	   
	   SP_DS_ColPlaceODE(SP_DS_Graph* p_pcGraph,SP_DS_ColPN_Unfolding* p_pUnfoldedNet);       //for colored continuous Petri Nets
	  
private:

	   wxString EquationEdge(bool p_bPreTrans, long p_nTransitionID);
	   void LoadUnfoldedNet();

public:
	//void SetPlaceID_PreTransitionID(map<long, vector<long> >* p_pmvPlaceID_PreTransitionID){ m_pmvPlaceID_PreTransitionID = p_pmvPlaceID_PreTransitionID;}
	//void SetPlaceID_PostTransitionID(map<long, vector<long> >* p_mvPlaceID_PostTransitionID){ m_mvPlaceID_PostTransitionID = p_mvPlaceID_PostTransitionID;}
	//void SetNetRateFunctions( vector<SP_VectorString>* p_panNetFunctions){ m_panNetFunctions = p_panNetFunctions;}
*/	

};

#endif /* SP_DS_ColPlaceODE_H_ */
