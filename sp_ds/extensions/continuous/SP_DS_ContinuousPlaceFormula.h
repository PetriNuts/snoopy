/*
 * SP_DS_ContinuousPlaceFormula.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 4.6.2010
 * Short Description:
 */
 //======================================================================================
#ifndef SP_DS_ContinuousPlaceFormula_H_
#define SP_DS_ContinuousPlaceFormula_H_

#include "sp_core/tools/rfdag.h"
#include "sp_ds/SP_DS_Graph.h"

//using namespace rfdag;

class SP_DS_ContinuousPlaceFormula
{
 protected:

	//formula string
	wxString m_sFormula;

	//parsed formula
	rfdag::RFDAG* m_Expression;

	//place type (discrete or continuous)
	wxString m_sPlaceType;

	//current marking
	const SP_VectorDouble *m_CurrentMarking;
	const double* m_pcCurrentMarking;

	SP_VectorDouble m_Parameter;
	SP_DS_Graph* m_pcGraph;
	SP_MapString2Long m_Placename2Id;
	SP_VectorStdString m_Id2Placename;
	SP_MapString2Long m_Parametername2Id;
	SP_VectorString m_Id2Parametername;

	

	//used in the case of transition formula
	SP_VectorString m_vsTransitionPrePlaces;
	SP_MapString2String m_mPlaceTransitionMultiplicity;

 public:
	 SP_DS_ContinuousPlaceFormula(const wxString& l_sFormula,const wxString & p_sPlaceType=SP_DS_CONTINUOUS_PLACE);

     virtual ~SP_DS_ContinuousPlaceFormula();

     //Get/Set
     rfdag::RFDAG  GetParsedExp(){return *m_Expression;}
     void SetParsedExp(rfdag::RFDAG* p_Expression){m_Expression=p_Expression;}

     //Current Marking
     void SetCurrentMarking(const SP_VectorDouble* p_pcCurrentMarking){m_CurrentMarking=p_pcCurrentMarking;}
     const SP_VectorDouble* GetCurrentMarking(){return m_CurrentMarking;}

     //
     void SetCurrentMarking(const double* p_pcCurrentMarking){m_pcCurrentMarking=p_pcCurrentMarking;}

	 //load places and parameter
	 virtual bool Initialise();

	 //parse the formula string
     virtual bool Parse();

     //evaluate the formula expression
     virtual double Evaluate(const SP_VectorDouble *p_pcCurrentMarking);

     inline virtual double Evaluate(rfdag::RFDAG* p_Expression)
     {
    	 return Eval(*p_Expression);
     }

     virtual double EvaluateA(rfdag::RFDAG* p_Expression);


     inline virtual double Evaluate(rfdag::RFDAG* p_Expression,const SP_VectorDouble *p_pcCurrentMarking)
     {
    	 m_Expression=p_Expression;
    	 return Evaluate(p_pcCurrentMarking);
     }

     //Return the formula equation string
      virtual wxString GetFormulaInString(){return m_sFormula;}

     //Substitute patter of a transition with it's equivalent equation
     virtual wxString ReplacePatternInTransitionFormula(SP_DS_Node * p_pcTransNode, const bool& p_bUseModelicaNaming=false);

	 virtual wxString ReplacePatternInTransitionFormula(long p_nTransitionID){return wxT("");}

	 virtual void SetODEString(const wxString& p_sFormulaStr){m_sFormula=p_sFormulaStr;}


 protected:
 	void LoadPlaces();
 	void LoadPlacesOfType(const wxString& p_sPlaceType, unsigned& p_nPos);
 	void LoadParameters();
 	void SetNames(rfdag::RFDAG& p_Expression);

	void ClearclientContinuousPlaces();
	void AddclientContinuousPlaces(const wxCharBuffer p_sPlaceName, unsigned p_nPos);
	bool ParseforColCPN();


protected:
 	double Eval(const rfdag::RFDAG& p_Expression);
 	double EvalA(const rfdag::RFDAG& p_Expression);
 	wxString EliminatePattern(const rfdag::RFDAG& p_Expression,const bool& p_bUseModelicaNaming=false);
	

};

#endif /* SP_DS_ContinuousPlaceFormula_H_ */
