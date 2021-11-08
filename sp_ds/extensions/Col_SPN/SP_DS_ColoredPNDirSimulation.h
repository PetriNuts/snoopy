
#ifndef __SP_DS_ColoredPNDirSimulation_H__
#define __SP_DS_ColoredPNDirSimulation_H__



#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Coloring.h"



class SP_DS_ColoredPNDirSimulation
{

protected:
	//places
	SP_DS_ColPN_Coloring* m_pcColoredNet;

	
	SP_VectorStdString m_msUncolPlaceNames;
	SP_VectorStdString m_msColoredPlaceNames;

	vector<SP_DS_Node*> m_vcolplaceNode;
	SP_VectorStdString m_msColSetPlaceNames;
	SP_VectorStdString m_msUncoloredPlaceNames;
	SP_VectorStdString m_msuncoloredplaces;
	map<long, SP_CPN_ColCountInterval> m_pnsPlaceCountById;
	SP_VectorStdString m_msColorsOfPlace;

	//transitions
	
	SP_VectorStdString m_msColoredTransitionNames;
	SP_MapString2String m_msTransitionNamesById;
	map<long, SP_CPN_ColCountInterval> m_pnsTransitionCountById;
	
	SP_VectorString m_anNetFunctions;
	// arcs
	
	SP_VectorDouble m_anNetMarkings;
	SP_MatrixLong* m_aanDeltaIncMatrix;
	SP_MatrixBool* m_aanResetIncMatrix;
	SP_MatrixPairLong* m_aanPreIncMatrix;

	map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;

public:
	SP_DS_ColoredPNDirSimulation(SP_DS_ColPN_Coloring* p_pcColordedNet);
	virtual ~SP_DS_ColoredPNDirSimulation();

public:
	SP_DS_ColPN_Coloring* GetColoredNet() { return m_pcColoredNet; }


protected:
	virtual bool LoadColoringData();
	
	virtual void GetColPlaceResults(const SP_Vector2DDouble& p_aanResults, SP_Vector2DDouble& p_aanColPlaceResults);
	virtual void GetColTransResults(const SP_Vector2DDouble& p_aanResults, SP_Vector2DDouble& p_aanColTransResults);

};

#endif // __SP_DS_ColoredPNDirSimulation_H__


