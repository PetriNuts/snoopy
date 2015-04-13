//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.03 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEALGEBRAICEQUATIONCREATER_H__
#define __SP_DS_FTREEALGEBRAICEQUATIONCREATER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include <wx/wx.h>

/**
	\brief make standard function available to transform a logical expression 
	in an algebraic equation ( with only +,- and *).	
	*/

class SP_DS_FTreeAlgebraicEquationCreater
{
private:
	
	//logical expression
	LiteralMap vMap;

protected:

	//list of most inner expressions
	TermMap tMap;

	/**
	\brief Elimination of unneeded bracket pairs
	Eliminate bracket pairs which include only one literal.
		
	\param		mMap	list of elements of the logical expression
	\returns	termMap	new list of elements of the logical expression
	*/
	LiteralMap EliminateBracketPair(LiteralMap mMap);

	/**
	\brief Multiplication of terms
	Multiplicate terms with each other.
		
	\param		term1	first term to multiplicate with...
	\param		term2	second term
	\returns	newTerm	result of the multiplication
	*/
	SP_ListString* DoMultiplicate(SP_ListString* term1, SP_ListString* term2);

	/**
	\brief Set new arithmetic sign
	Returns new arithmetic sign after multiplication of two terms.
		
	\param		sVal1		arithmetic sign of the first term
	\param		sVal1		arithmetic sign of the second term
	\returns	arithSign	result of the multiplication
	*/
	wxString CheckArithmeticSign(const wxString& sVal1, const wxString& sVal2);

	/**
	\brief Check, if a literal is negated.
	Literals Xi and negated literals  -Xi will be different by setting a flag (0,1),
	for example literal X1 ->" X01" (negated) and "X11" (not negated).
	The function checks on the basis of the flag, if the literal is negated or not.
	
	\param	name	name of the literal
	\return	TRUE, FALSE	
	*/
	bool LiteralIsNegated(const wxString& name);

public:
    SP_DS_FTreeAlgebraicEquationCreater();

    ~SP_DS_FTreeAlgebraicEquationCreater();

	//transform a logical expression in an algebraic equation
	void CreateForm(LiteralMap mMap);

	//fault tree
	SP_DS_Graph* m_pcGraph;
};

#endif // __SP_DS_FTREECREATEALGEBRAICEQUATION_H__


