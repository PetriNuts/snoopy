//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2011/08/02 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColoredPNSimulation_H__
#define __SP_DS_ColoredPNSimulation_H__

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include <dssd/andl/andl_builder.h>//by george

class SP_DS_ColoredPNSimulation
{

protected:
	//net
	SP_DS_ColPN_Unfolding* m_pcUnfoldedNet;

	//net - places
	SP_MapString2String m_msPlaceNamesById_FromUnfolding;
    SP_MapString2Long m_mnPlaceArrayPos_FromUnfolding;
	SP_VectorStdString m_msPlaceNames;
    SP_VectorStdString m_msColoredPlaceNames;
	map<long, SP_CPN_CountInterval> m_pnsPlaceCountById;	      
	SP_VectorStdString m_msColorsOfPlace;

	//net - transitions
	SP_MapString2Long m_mnTransitionArrayPos;
    SP_VectorStdString m_msTransitionNames;
	SP_VectorStdString m_msColoredTransitionNames;
    SP_MapString2String m_msTransitionNamesById;
	map<long, SP_CPN_CountInterval> m_pnsTransitionCountById ;
  // 	map<wxString, SP_CPN_CountInterval> m_nsTransitionCountByNodeClass;

	vector<SP_VectorString> m_anNetFunctions;

	//net - arcs
	vector<SP_VectorLong> m_anNetMarkings;
	SP_MatrixLong* m_aanDeltaIncMatrix;
    SP_MatrixBool* m_aanResetIncMatrix;
    SP_MatrixPairLong* m_aanPreIncMatrix;

	map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;
	
public:
	SP_DS_ColoredPNSimulation(SP_DS_ColPN_Unfolding* p_pcUnfoldedNet);
	virtual ~SP_DS_ColoredPNSimulation();

public:
	SP_DS_ColPN_Unfolding* GetUnfoldedNet() { return m_pcUnfoldedNet; }
	 dssd::andl::Net_ptr CreateANDLNet(SP_DS_Graph* p_pcGraph, bool p_bshowLog = true);//by george
	

protected:
	 virtual bool LoadUnfoldingData();
	 bool LoadUnfoldedPlaces();
	 virtual void GetColPlaceResults(const SP_Vector2DDouble& p_aanResults,SP_Vector2DDouble& p_aanColPlaceResults);
	 virtual void GetColTransResults(const SP_Vector2DDouble& p_aanResults,SP_Vector2DDouble& p_aanColTransResults);
    
};

#endif // __SP_DS_ColoredPNSimulation_H__


