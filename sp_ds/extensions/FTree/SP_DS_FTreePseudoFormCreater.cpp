//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: create pseudo polynomial form to calculate unavailability/unreliabaility of the system
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/FTree/SP_DS_FTreePseudoFormCreater.h"


SP_DS_FTreePseudoFormCreater::SP_DS_FTreePseudoFormCreater()
{
	pseudoForm = new SP_ListString();
}

SP_DS_FTreePseudoFormCreater::~SP_DS_FTreePseudoFormCreater()
{
	pseudoForm->clear();
}


SP_ListString*
SP_DS_FTreePseudoFormCreater::createPseudoPolynomalForm (LiteralMap mMap)
{

	//Preparation to interprets the logical expression in the right sequence:
	//In each case, the most inner expression of a logical expression is always
	//interpreted at first.
	CreateForm(mMap);

	TermMap::iterator termIt;

	//Creating of the pseudo polynom form
	for (termIt = tMap.begin(); termIt != tMap.end(); ++termIt) {
		SP_ListString* list = (*termIt).second;
		SP_ListString::iterator sIt;
 		for (sIt = list->begin(); sIt != list->end(); ++sIt){

			if ((*sIt)== wxT("OR") ) {
				// Substitution of terms with OR in form of 1 - (1 - x1)*...*(1 - Xn)
				DoDisjunction((*termIt).first, list);
				break;
			}
			// Substitution of terms with AND in form of X1*X2*...*Xn
			if ((*sIt)== wxT("AND")){
				DoConjunction((*termIt).first, list);
				break;
			}
		}
	}
	return pseudoForm;
}


void
SP_DS_FTreePseudoFormCreater::DoDisjunction(const wxString& varName, SP_ListString* NodeList)
{
	SP_ListString* term1 = new SP_ListString();
	SP_ListString* term2 = new SP_ListString();
	SP_ListString::iterator aIt;

	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){
		wxString& l_rAString = *aIt;
		// variables Xi of one term will be replaced by (1 - Xi). Finally,
		// all variables will be multiplied with each other.
		if (wxStrstr(l_rAString, wxT("X")) ){

			if (term1->empty()) {
				term1->push_back(wxString::Format(wxT("+1")));
				term1->push_back(wxString::Format(wxT("-") + l_rAString));
			}else{
				if (term2->empty()){
					term2->push_back(wxString::Format(wxT("+1")));
					term2->push_back(wxString::Format(wxT("-") + l_rAString));
				}
			}
			//multiplication of two variables
			if (!term1->empty() && !term2->empty()) {
				term1 = DoMultiplicate(term1, term2);
				term2->clear();
			}
		}

		// If an expression contains a variable, that represents a substitution
		// of an expression, the expression is looked for to this variable in the list.
		// This expression is multiplied with the next possible variable.
		if (wxStrstr(l_rAString, wxT("Y"))){
			SP_ListString* term = new SP_ListString();
			TermMap::iterator tIt;
			for (tIt = tMap.begin(); tIt != tMap.end(); ++tIt) {
				if ((*tIt).first== l_rAString) {
					term = (*tIt).second;
					term2->push_back(wxString::Format(wxT("-1")));
					term = DoMultiplicate(term2, term);
					term2->clear();
					break;
				}
			}
			if (term1->empty()){
				term1 = term;
				term1->push_front(wxString::Format(wxT("+1")));
			}else{
				if (term2->empty()){
					term2 = term;
					term2->push_front(wxString::Format(wxT("+1")));
				}
			}
			//multiplication of two conjunctions of variables
			if (!term1->empty() && !term2->empty()) {
				term1 = DoMultiplicate(term1, term2);
				term2->clear();
			}
		}
	}

	// Variables, combines with OR-connections, now have to be replaced in form of (1 - X1)*...*(1 - Xn).
	// Finally, the new expression must be multiplied with -1 in form of 1 - (1 - X1)* ... * (1 - Xn).
	term2->push_back(wxString::Format(wxT("-1")));
	term1 = DoMultiplicate(term2, term1);
	// wxT("1") of 1 - (1 - X1)... is discontinued through evaluation of the expression
	term1->pop_front();
	tMap[varName] = term1;
	pseudoForm = term1;
}

void
SP_DS_FTreePseudoFormCreater::DoConjunction(const wxString& varName, SP_ListString* NodeList)
{
	SP_ListString* term1 = new SP_ListString();
	SP_ListString* term2 = new SP_ListString();
	SP_ListString::iterator aIt;

	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){
		wxString& l_rAString = *aIt;
		// variables of one term will be multiplied with each other like X1* ... * Xn.
		if (wxStrstr(l_rAString, wxT("X"))){

			if (term1->empty())
				term1->push_back(wxString::Format(wxT("+") + l_rAString));
			else{
				if (term2->empty())
					term2->push_back(wxString::Format(wxT("+") + l_rAString));
			}
			//multiplication of two variables
			if (!term1->empty() && !term2->empty()) {
				term1 = DoMultiplicate(term1, term2);
				term2->clear();
			}
		}
		// Before multiplication: If an expression contains a variable, that represents a substitution
		// of an expression, the expression is looked for to this variable in the list.
		if (wxStrstr(l_rAString, wxT("Y"))){
			SP_ListString* term = new SP_ListString();
			TermMap::iterator tIt;
			for (tIt = tMap.begin(); tIt != tMap.end(); ++tIt) {
				if ((*tIt).first== l_rAString){
					term = (*tIt).second;
					break;
				}
			}
			if (term1->empty()){
				term1 = term;
			}else{
				if (term2->empty())
					term2 = term;
			}
			//multiplication of two conjunctions of variables
			if (!term1->empty() && !term2->empty()) {
				term1 = DoMultiplicate(term1, term2);
				term2->clear();
			}
		}
	}
	//the new expression is actualized in the stack
	tMap[varName] = term1;
	pseudoForm = term1;
}





