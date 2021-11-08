/*
 * SP_DS_ContinuousPlaceFormula.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 4.6.2010
 * Short Description:
 */
 //======================================================================================

#ifndef SP_DS_ColCPNPlaceFormula_H_
#define SP_DS_ColCPNPlaceFormula_H_

#include "sp_core/tools/rfdag.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

//using namespace rfdag;

class SP_DS_ColCPNPlaceFormula: public SP_DS_ContinuousPlaceFormula
{

private:
	map< long, SP_MapString2Long >* m_pmmPrePlaceArrayPosOfTransitionNode;
	map< long, SP_MapString2String >* m_pmPrePlaceToArcsOfTransitionNode;
	//map< long, SP_MapString2String >* m_pmPostPlace2ArcsOfTransitionNode; //for later considering post arcs

	SP_MapString2Long* m_pmnPlaceArrayPos;
    SP_VectorStdString* m_pmsPlaceNames;

	SP_VectorStdString* m_pmsColoredPlaceNames;

	long m_nTransitionID;

	bool m_bFirstTime;

	SP_DS_ColPN_Unfolding* m_pUnfoldedNet;

	bool m_bPreArcFlag;
 

 public:
	 SP_DS_ColCPNPlaceFormula(bool p_bFirstTime, const wxString& l_sFormula,const wxString & p_sPlaceType=SP_DS_CONTINUOUS_PLACE);

     virtual ~SP_DS_ColCPNPlaceFormula();

	 //load places and parameter
	 virtual bool Initialise(SP_DS_ColPN_Unfolding* p_pUnfoldedNet);

	//  virtual bool Parse();

	 void SetPreArcFlag(bool p_bPreArcFlag){ m_bPreArcFlag = p_bPreArcFlag;}


     //Substitute patterns of a transition with it's equivalent equation  
	 virtual wxString ReplacePatternInTransitionFormula(long p_nTransitionID);

 private:
 private:
 	
 	void SetNamesColored(rfdag::RFDAG& p_Expression);

	void SetPlaceName(rfdag::RFDAG& p_Expression);
 	

	void SetPlacesColored() {  }  //Set unfolded place instances to current class

	void LoadUnfoldedPlace();
	void LoadColoredPlace();
	void LoadUnfoldedNet();


};

#endif /* SP_DS_ColCPNPlaceFormula_H_ */
