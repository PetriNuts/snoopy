//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: Create pseudo-polynomform of an event
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEPSEUDOFORMCREATER_H__
#define __SP_DS_FTREEPSEUDOFORMCREATER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeAlgebraicEquationCreater.h"
#include <wx/wx.h>

/**
	\brief Create pseudo-polynomform.
	*/

class SP_DS_FTreePseudoFormCreater: public SP_DS_FTreeAlgebraicEquationCreater
{
private:
 
	SP_DS_Graph* m_pcGraph;
	SP_ListString* pseudoForm;

protected:
	
	/**
	\brief Transform terms which consist of ANDs
	Terms of the form X1 AND X2 AND ... AND Xn will be replaced by
	X1X2...Xn.
	
	\param	var		label which belongs to the term
	\param	list	the term
	*/
	void DoConjunction(const wxString& var, SP_ListString* list);

	/**
	\brief Transform of terms which consist ORs
	Terms of the form X1 OR X2 OR ... OR Xn will be replaced by
	1 - (1 - X1)*(1 - X2)*...*(1 - Xn).

	\param	var		label which belongs to the term
	\param	list	the term
	*/
	void DoDisjunction(const wxString& var, SP_ListString* list);

public:
    SP_DS_FTreePseudoFormCreater();

    ~SP_DS_FTreePseudoFormCreater();

	/**
	\brief Create pseudo-polynomform of an event

	\param	mMap		logical expression to replace
	\param	pseudoForm	pseudo polynomal form of the logical expression
	*/
	//
	SP_ListString* createPseudoPolynomalForm(LiteralMap mMap);
};

#endif // __SP_DS_FTREEPSEUDOFORMCREATER_H__


