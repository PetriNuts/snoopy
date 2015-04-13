//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: create disjunctive normalform
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEDNFCREATER_H__
#define __SP_DS_FTREEDNFCREATER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeAlgebraicEquationCreater.h"
#include <wx/wx.h>

	/**
	\brief Create disjunctive normalform on the basis
		of the logical expression of the fault tree.
	*/

class SP_DS_FTreeDNFCreater: public SP_DS_FTreeAlgebraicEquationCreater
{
private:
 

protected:

	/**
	\brief Transform expressions that consist of ANDs
	Expressions of the form X1 AND X2 AND ... AND Xn are replaced by
	X1X2...Xn.
	
	\param	var		label (variable) which belongs to the expression
	\param	list	the expression
	*/
	void DoConjunction(const wxString& var, SP_ListString* list);
	
	/**
	\brief Transform of expressions that consist of ORs
	Expressions which consist of a disjunction of
	variables are inserted first in a list.
	
	\param	var		label (variable) which belongs to the expression
	\param	list	the expression
	*/
	void DoDisjunction(const wxString& var, SP_ListString* list);

	/**
	\brief Create disjunctive normalform
	To create the disjunctive normalform, the terms will be multiplicate which each other
	(ANDs and ORs will be used like  + and *). 
	
	 \param		term1		first term
	 \param		term2		second term
	 \returns	newTerm		new term
	 */
	 SP_ListString* CombineTerms(SP_ListString* term1, SP_ListString* term2);

	/**
	\brief Minimize a term on the basis of the "Idempotenz-rule"
	The Idempotenz-rule predicate that the following expression can be minimised by X*X = X
	and X*-X = 0.
	
	*/
	void UseIdemPotenzRule();

	/**
	\brief Minimize a term on the basis of the absorptions law
	The absorptions law predicate that the following expression can be minimised by 
	X + -XY = X + Y and X * XY = X.

	*/
	void UseAbsorptionLaw();

	/**
	\brief Separate literals
	After the multiplication of terms, the term consists of literals in form of X1X2...Xn. But to calculate
	the parameters, we need the terms in form of X1, X2, ..., Xn.
	
	*/
	void CutLiterals();

public:
    SP_DS_FTreeDNFCreater(SP_DS_Graph* p_pcGraph);

    ~SP_DS_FTreeDNFCreater();

	/**
	\brief Select all cut sets of the fault tree
	
	\param		mMap		logical expression
	\returns	dnfForm		disjunctive normalform
	*/
	SP_ListString* CreateDNF(LiteralMap mMap);
	
	//fault tree
	SP_DS_Graph* m_pcGraph;
	//disjunctive normalform
	SP_ListString* dnfForm;

};

#endif // __SP_DS_FTREEDNFCREATER_H__


