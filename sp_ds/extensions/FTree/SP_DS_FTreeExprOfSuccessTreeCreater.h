//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: Create negative pseudo-polynomform of an event
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEEXPROFSUCCESSTREECREATER_H__
#define __SP_DS_FTREEEXPROFSUCCESSTREECREATER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeAlgebraicEquationCreater.h"
#include <wx/wx.h>


/**
	\brief Create negative pseudo-polynomform on the bais of the success tree.
	*/

class SP_DS_FTreeExprOfSuccessTreeCreater: public SP_DS_FTreeAlgebraicEquationCreater
{
private:
 
	SP_DS_Graph* m_pcGraph;
	

protected:

	/**
	\brief Transform expressions that consist of ANDs
	Expressions of the form X1 AND X2 AND ... AND Xn are replaced by
	X1X2...Xn. A negated literal is replaced by -X = (1 - X).
	
	\param		var		label (variable) which belongs to the expression
	\param		list	the expression
	\returns	term1	new expression after replacement
	*/
	SP_ListString* DoConjunction(SP_ListString* list);

	/**
	\brief Transform of expressions that consist of ORs
	Expressions in form of X1 OR X2 OR ... OR Xn are replaced by
	1 - (1 - X1)*(1 - X2)*...*(1 - Xn). A negated Literal is 
	replaced by -X = (1 - X).
	
	\param		var		label (variable) which belongs to the expression
	\param		list	the expression
	\returns	term1	new expression after replacement
	*/
	SP_ListString* DoDisjunction(list<SP_ListString*>* list);
	
	/**
	\brief Interchancing of the gates AND and OR
	To create a negated pseudo-polynomform, ANDs and ORs have to be interchanged
	(SHANNONSCHER Inversionssatz).
	
	\param		vMap	list of elements of the logical expression
	\returns	vMap	negated logical expression
	*/
	LiteralMap InterchangeAndOr(LiteralMap vMap);

public:
    SP_DS_FTreeExprOfSuccessTreeCreater();

    ~SP_DS_FTreeExprOfSuccessTreeCreater();

	//create negated pseudo-polynomform of an event
	SP_ListString* createExpression(LiteralMap mMap);
};

#endif // __SP_DS_FTREECREATEEXPROFSUCCESSTREE_H__


