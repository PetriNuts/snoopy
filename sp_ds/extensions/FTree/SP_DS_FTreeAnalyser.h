//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEANALYSER_H__
#define __SP_DS_FTREEANALYSER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCutSetsCreater.h"
#include "sp_utilities.h"



 /**
	\brief make function available which will be used for both system types (repairable
	and irreparable systems. Following parameters will be calculated:
	- Failure Rate
	- Probability of Failure
*/

class SP_DS_FTreeAnalyser
{
private:


protected:

	/**
	\brief Check whether a literal is negated.
	Literals Xi and negated literals  -Xi will be different by setting a flag (0,1),
	for example literal X1 ->" X01" (negated) and "X11" (not negated).
	The function checks on the basis of the flag, if the literal is negated or not.
	
	\param	name	name of the literal
	\return	TRUE, FALSE	
	*/
	bool LiteralIsNegated(const wxString& name);
	
	/**
	\brief Minimize a term on the basis of the "Idempotenz-rule"
	The Idempotenz-rule predicate that the following expression can be minimised by X*X = X
	and X*-X = 0.
	
	\param		term	the term to check
	\returns	term	minimised term
	*/
	SP_ListString* UseIdemPotenzRule(SP_ListString* term);
	
	/**
	\brief Separate literals
	After the multiplication of terms, the term consists of literals in form of X1X2...Xn. But to calculate
	the parameters, we need the terms in form of X1, X2, ..., Xn.
	
	\param		n_pcList						term of Literals and multiplicater
	\returns	pseudoFormWithSeparateLiterals	term of separate literals
	*/
	list<SP_ListString*>* CutLiterals(SP_ListString* n_pcList);

	/**
	\brief Calculation of the Failure Rate
	Calculate the failure rate of Intermediate Events and Top Events.
	
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateFailureRate(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of the probability of failure
	Calculate the probability of failure for all events.
	
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateProbabilityOfFailure(SP_DS_Node* p_pcNode);
	
	/**
	\brief Returns the value of the desired attribute type
	
	\param		p_pcNode	the node
	\param		attrType	attribute type of the node
	\returns	value		value of the attribute
	*/
	double GetValue(SP_DS_Node* p_pcNode, const wxString& attrType);

public:
    SP_DS_FTreeAnalyser();

    ~SP_DS_FTreeAnalyser();
	
	//logical expression of the fault tree
	NodeMap logExpression;
	//logical expression after application of de Morgan
	LiteralMap logExprAfterDeMorgan;

	/**
	\brief Reset all calculated values of the node
		
	\param	p_pcNode	component (event), which values should be reseted.
	*/
	void ResetValues(SP_DS_Node* p_pcNode);
};

#endif // __SP_DS_FTREEANALYSIS_H__


