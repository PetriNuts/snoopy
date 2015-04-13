//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: create pseudo polynomial form to calculate unavailability/unreliabaility of the system
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/FTree/SP_DS_FTreeExprOfSuccessTreeCreater.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCutSetsCreater.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "snoopy.h"

SP_DS_FTreeExprOfSuccessTreeCreater::SP_DS_FTreeExprOfSuccessTreeCreater()
{
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

	m_pcGraph = l_pcDoc->GetGraph();

}

SP_DS_FTreeExprOfSuccessTreeCreater::~SP_DS_FTreeExprOfSuccessTreeCreater()
{

}


SP_ListString*
SP_DS_FTreeExprOfSuccessTreeCreater::createExpression (LiteralMap mMap)
{
	//Application of the wxT("Shannonschen Inversionsatz"):
	//in each case, every wxT("AND" and "OR") is interchanged
	mMap = InterchangeAndOr(mMap);

	SP_DS_FTreeCutSetsCreater* dnfCreater = new SP_DS_FTreeCutSetsCreater(m_pcGraph);
	//create disjunctive normalform of the negated logical expression
	SP_ListString* dnfForm = new SP_ListString();
	dnfForm = dnfCreater->CreateDNF(mMap);

	SP_ListString::iterator nIt;

	list<SP_ListString*>* tempDNF = new list<SP_ListString*>();
	SP_ListString* tempList = new SP_ListString();

	//Only the OR's have to be evaluated, because the disjunctive normalform
	//consists of a disjunction of terms. But the terms can be consist of negated
	//or not negated variables. First, negated variables are replaced by (1 - X).
	//This replacement takes place now, so that all variables of each term have to
	//be multiplied with each other.

	for (nIt = dnfForm->begin(); nIt != dnfForm->end(); ++nIt){
		wxString& l_rString = *nIt;
		if (wxStrstr(l_rString, wxT("X")))
			tempList->push_back((*nIt));
		if ((*nIt)== wxT("OR")){
			//first the variables of a term are multiplied with each other
			SP_ListString* newList = DoConjunction(tempList);
			tempList->clear();
			tempDNF->push_back(newList);

		}
	}
	//last expression of the disjunctive normalform doesn't yet evaluated. So, this have to be still done.
	if (!tempList->empty()) {
		tempList = DoConjunction(tempList);
		tempDNF->push_back(tempList);
	}
	//Replacement of the terms with ORs by 1 - (1 - X)(1 - X)....
	//The new expression is the pseudo polynom form of the success tree.
	return (DoDisjunction(tempDNF));
}

LiteralMap
SP_DS_FTreeExprOfSuccessTreeCreater::InterchangeAndOr(LiteralMap vMap)
{
	LiteralMap::iterator pIt;

	//Interchancing of ANDs and ORs
	for(pIt = vMap.begin(); pIt != vMap.end(); pIt++) {

		if ((*pIt).second== wxT("OR"))
			(*pIt).second.Replace(wxT("OR"), wxT("AND"), TRUE);
		else
			if ((*pIt).second== wxT("AND"))
				(*pIt).second.Replace(wxT("AND"), wxT("OR"), TRUE);
	}
	return vMap;
}

SP_ListString*
SP_DS_FTreeExprOfSuccessTreeCreater::DoDisjunction(list<SP_ListString*>* NodeList)
{

	SP_ListString* term1 = new SP_ListString();
	SP_ListString* term2 = new SP_ListString();

	list<SP_ListString*>::iterator aIt;
	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){

		// Each variable X or a conjunction of variables will be replaced by (1 - X). Finally, variables
		// of one term will be multiplied with each other.

		if (term1->empty()) {
			SP_ListString* tmp = new SP_ListString();
			tmp->push_back(wxString(wxT("-1")));
			term1 = DoMultiplicate(tmp, *aIt);
			term1->push_front(wxString::Format(wxT("+1")));

		}else{
			if (term2->empty()){
				SP_ListString* tmp = new SP_ListString();
				tmp->push_back(wxString(wxT("-1")));
				term2 = DoMultiplicate(tmp, *aIt);
				term2->push_front(wxString::Format(wxT("+1")));
			}
		}
		//multiplication of two expressions
		if (!term1->empty() && !term2->empty()) {
			term1 = DoMultiplicate(term1, term2);
			term2->clear();
		}
	}
	//Till now, the variables are replaced by (1 - X) and are multiplied with each other.
	//So as the last, the new expression has to be evaluated like 1 - (1 - X)(1 - Y)...
	term2->push_back(wxString::Format(wxT("-1")));
	term1 = DoMultiplicate(term2, term1);
	term1->push_front(wxString(wxT("+1")));

	return term1;

}

SP_ListString*
SP_DS_FTreeExprOfSuccessTreeCreater::DoConjunction(SP_ListString* NodeList)
{
	SP_ListString* term1 = new SP_ListString();
	SP_ListString* term2 = new SP_ListString();

	SP_ListString::iterator aIt;

	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){
		wxString& l_rString = *aIt;
		// Variables of one term will be multiplied with each other like X1* ... * Xn.
		// Variables which are negated, will be replaced by (1 - X).
		if (term1->empty()) {
			if (l_rString.Mid(1,1)== wxT("0")){

				l_rString.Replace(wxT("X0"), wxT("X1"), TRUE);
				term1->push_back(wxString::Format(wxT("+1")));
				term1->push_back(wxString::Format(wxT("-") + l_rString));
			}else{
				if (l_rString.Mid(0,1) !=  wxT("+") &&
					l_rString.Mid(0,1) !=  wxT("-"))
					term1->push_back(wxString::Format(wxT("+") + l_rString));
				else
					term1->push_back(l_rString);
			}
			//If a term consists of only one variable, this term has not to be evaluated.

			if (NodeList->size() == 1)
				return term1;

		}else{
			if (term2->empty()){
				if (l_rString.Mid(1,1)== wxT("0")){
					l_rString.Replace(wxT("X0"), wxT("X1"), TRUE);
					term2->push_back(wxString::Format(wxT("+1")));
					term2->push_back(wxString::Format(wxT("-") + l_rString));
				}else{
					if (l_rString.Mid(0,1) !=  wxT("+") &&
						l_rString.Mid(0,1) !=  wxT("-"))
						term2->push_back(wxString::Format(wxT("+") + l_rString));
					else
						term2->push_back(l_rString);
				}
			}
		}
		//multiplication of two variables
		if (!term1->empty() && !term2->empty()) {
			term1 = DoMultiplicate(term1, term2);
			term2->clear();
		}
	}

	return term1;
}





