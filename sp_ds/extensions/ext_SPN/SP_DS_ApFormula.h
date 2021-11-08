/*
 * SP_DS_ApFormula.h
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */

#ifndef SP_DS_ILFORMULA_H_
#define SP_DS_ILFORMULA_H_

#include "sp_ds/extensions/SP_DS_Formula.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"


class SP_DS_ApFormula: public SP_DS_Formula
{
public:
	SP_DS_ApFormula(SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree);
	SP_DS_ApFormula(SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree);

	virtual ~SP_DS_ApFormula();

	virtual bool Initialise();

	virtual double Evaluate(const SP_VectorLong* p_Marking);

	virtual double PostEvaluate(const SP_VectorDouble* p_Marking);


protected:
	SP_MapString2Long m_Placename2Id;
	SP_VectorStdString m_Id2Placename;
	SP_MapString2Long m_Parametername2Id;
	SP_VectorString m_Id2Parametername;

	const SP_VectorLong* m_CurrentMarking;
	SP_VectorDouble m_Parameter;

	SP_VectorLong m_NewMarking;
	SP_VectorDouble m_PostNewMarking;
	const SP_VectorDouble* m_PostCurrentMarking;
	bool m_bPostEval;

	
	SP_DS_ColPN_Unfolding* m_pcUnfoldedNet;
	SP_VectorStdString m_msColoredPlaceNames;
	map<long, SP_CPN_CountInterval> m_pnsPlaceCountById;

private:
	void LoadPlaces();
	void LoadUnfoldedPlaces();
	void LoadParameters();

	void GetNewMarking();
	void GetPostNewMarking();
	double Eval(const rfdag::RFDAG& p_Expression);

};

typedef vector<SP_DS_ApFormula> SP_VectorApFormula;

#endif /* SP_DS_ILFORMULA_H_ */
