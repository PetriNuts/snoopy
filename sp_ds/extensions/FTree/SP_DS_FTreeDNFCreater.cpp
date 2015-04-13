//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: create disjunctive normalform of a logical expression
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeDNFCreater.h"


SP_DS_FTreeDNFCreater::SP_DS_FTreeDNFCreater(SP_DS_Graph* p_pcGraph)
: m_pcGraph(p_pcGraph)
{
	//disjunctive normalform
	dnfForm = new SP_ListString();
}

SP_DS_FTreeDNFCreater::~SP_DS_FTreeDNFCreater()
{
	dnfForm->clear();
}

SP_ListString*
SP_DS_FTreeDNFCreater::CreateDNF (LiteralMap mMap)
{
	//Preparation to interprets the logical expression in the right sequence:
	//In each case, the most inner expression of a logical expression is always
	//interpreted at first.
	CreateForm(mMap);

	TermMap::iterator termIt;

	for (termIt = tMap.begin(); termIt != tMap.end(); ++termIt) {
		SP_ListString* list = (*termIt).second;
		SP_ListString::iterator sIt;
 		for (sIt = list->begin(); sIt != list->end(); ++sIt){

			if ((*sIt) ==  wxT("OR") ) {
				//interpretation of expressions that consist of a disjunction of variables
				DoDisjunction((*termIt).first, list);
				break;
			}
			if ((*sIt) ==  wxT("AND")){
				//interpretation of expressions that consist of a conjunction of variables
				DoConjunction((*termIt).first, list);
				break;
			}
		}
	}

	//Hint: the disjunctive normalform is a string of expressions. So, the variables in one
	//of the expressions form one string like X1X2X3..., but to analyse the disjunctive
	//normalform, the variables have to be stand alone like X1, X2, ...
	CutLiterals();

	//Application of the wxT("idempotenz-rule" and the "orthogonality-rule"):
	//expressions in form of X*X*...*X are shortened by X,
	//expressions in form of X*-X = 0 are deleted
	UseIdemPotenzRule();

	//Application of the wxT("absorption law"): Unneeded expressions are deleted.
	UseAbsorptionLaw();

	return dnfForm;
}


SP_ListString*
SP_DS_FTreeDNFCreater::CombineTerms(SP_ListString* term1, SP_ListString* term2)
{
	//Creating of the disjunctive normalform of the expression
	SP_ListString::iterator t1It;
	SP_ListString::iterator t2It;
	SP_ListString* newTerm = new SP_ListString();

	//each term of the logical expression are multiplicied with
	//each other
	for (t1It = term1->begin(); t1It != term1->end(); ++t1It) {
		for (t2It = term2->begin(); t2It != term2->end(); ++t2It) {
			if ((*t1It) != wxT("OR") && (*t2It) !=  wxT("OR")){
				if (!newTerm->empty())
					newTerm->push_back(wxString(wxT("OR")));
				newTerm->push_back((*t1It) + (*t2It));
			}
		}
	}
	return newTerm;
}

void
SP_DS_FTreeDNFCreater::DoDisjunction(const wxString& varName, SP_ListString* NodeList)
{
	SP_ListString* term1 = new SP_ListString();
	SP_ListString::iterator aIt;

	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){
		wxString& l_rsNode = *aIt;
		//the variables of an expression are saved at first in a list
		if (wxStrstr(l_rsNode, wxT("X")) || l_rsNode ==  wxT("OR") )
			term1->push_back(l_rsNode);

		// If an expression consists of a variable, that represents a substitution
		// of an expression, the expression is looked for to this variable in the list.
		if (wxStrstr(l_rsNode, wxT("Y"))){
			SP_ListString* term = new SP_ListString();
			TermMap::iterator tIt;
			for (tIt = tMap.begin(); tIt != tMap.end(); ++tIt) {
				if ((*tIt).first ==  l_rsNode) {
					term = (*tIt).second;
					break;
				}
			}
			if (!term->empty()){
				SP_ListString::iterator nIt;
				for (nIt = term->begin(); nIt != term->end(); ++nIt)
					term1->push_back(*nIt);
			}
		}
	}
	tMap[varName] = term1;
	dnfForm = term1;
}

void
SP_DS_FTreeDNFCreater::DoConjunction(const wxString& varName, SP_ListString* NodeList)
{
	SP_ListString* term1 = new SP_ListString();
	SP_ListString* term2 = new SP_ListString();
	SP_ListString::iterator aIt;

	for (aIt = NodeList->begin(); aIt != NodeList->end(); ++aIt){
		wxString& l_rsNode = *aIt;
		//each expression is multiplicied with each other
		if (wxStrstr(l_rsNode, wxT("X"))){

			if (term1->empty())
				term1->push_back(l_rsNode);
			else if (term2->empty())
				term2->push_back(l_rsNode);

			if (!term1->empty() && !term2->empty()){
				term1 = CombineTerms(term1, term2);
				term2->clear();
			}
		}
		// If an expression consists of a variable, that represents a substitution
		// of an expression, the expression is looked for to this variable in the list.
		if (wxStrstr(l_rsNode, wxT("Y"))){
			SP_ListString* term = new SP_ListString();
			TermMap::iterator tIt;
			for (tIt = tMap.begin(); tIt != tMap.end(); ++tIt) {
				if ((*tIt).first ==  l_rsNode){
					term = (*tIt).second;
					break;
				}
			}
			if (term1->empty())
				term1 = term;
			else{
				if (term2->empty())
					term2 = term;
			}
			//multiplication of two expressions
			if (!term1->empty() && !term2->empty()) {
				term1 = CombineTerms(term1, term2);
				term2->clear();
			}
		}
	}
	tMap[varName] = term1;
	dnfForm = term1;
}


void
SP_DS_FTreeDNFCreater::UseIdemPotenzRule()
{
	// Expressions of the form X * ...* X are shortened by X.
	// The method uses the application of the orthogonality -law at the same time.
	// Terms, that consist of variables in form of X * -X , are deleted completely.

	SP_ListString::iterator nIt;
	SP_ListString::iterator mIt;
	SP_ListString* newList = new SP_ListString();
	SP_ListString* tempList = new SP_ListString();
	bool stillExists = FALSE;

	//identifier whether an expression is taken away in the
	//disjunctive normalform or not
	bool possibleTerm = TRUE;


	for (nIt = dnfForm->begin(); nIt != dnfForm->end(); ++nIt){
		stillExists = FALSE;
		wxString& l_rsDNF = *nIt;
		if (l_rsDNF !=  wxT("OR")){
			if (tempList->empty() && possibleTerm){
				tempList->push_back(wxString::Format(l_rsDNF));
			}else{
				for (mIt = tempList->begin(); mIt != tempList->end(); ++mIt){
					wxString& l_rsMString = *mIt;
					wxString id1 = l_rsDNF.SubString(2, l_rsDNF.length());
					wxString id2 = l_rsMString.SubString(2, l_rsMString.length());
					//it was found a variable that already exits in this expression
					if (id1 ==  id2){
						//the other variable is negated
						if (LiteralIsNegated(l_rsDNF) != LiteralIsNegated(l_rsMString)){
								tempList->clear();
								//identify, that this expression will be taken away in the disjunctive normalform
								possibleTerm = FALSE;
						//the other variable is not negated
						}else
							stillExists = TRUE;
						break;
					}
				}
				if (!stillExists && !tempList->empty())
					tempList->push_back(wxString::Format(l_rsDNF));
			}
		}else{
			possibleTerm = TRUE;
			//expression is only been attached to the disjunctive normalform,
			//if it only still has variables after the check
			if (!tempList->empty()){
				for (mIt = tempList->begin(); mIt != tempList->end(); ++mIt){
					wxString literal = (*mIt);
					newList->push_back(literal);
				}
				newList->push_back(wxString(wxT("OR")));
				tempList->clear();
			}
		}
	}
	//last expression of the disjunctive normalform doesn't yet list into the
	//new disjunctive normalform. So, this have to be still done.
	if (!tempList->empty()){
		for (mIt = tempList->begin(); mIt != tempList->end(); ++mIt){
			wxString literal = (*mIt);
			newList->push_back(literal);
		}
	}

	//disjunctive normalform is replaced by the shortened disjunctive normalform
	dnfForm->clear();
	dnfForm = newList;
}

void
SP_DS_FTreeDNFCreater::UseAbsorptionLaw()
{
	SP_ListString::iterator nIt;

	//disjunctive normalform consists of a disjunctions of expressions.
	//To use the absorption law, all terms are saved into a list at first,
	//to which the absorption-laws are finally applied.
	list<SP_ListString*>* literalList = new list<SP_ListString*>();
	SP_ListString* tmpList = new SP_ListString();

	for (nIt = dnfForm->begin(); nIt != dnfForm->end(); ++nIt){
		wxString& l_rsDNF = *nIt;
		if (wxStrstr(l_rsDNF, wxT("X")))
			tmpList->push_back(l_rsDNF);
		else{
			if (!tmpList->empty()){
				SP_ListString::iterator mIt;
				SP_ListString* nodeList = new SP_ListString();
				for (mIt = tmpList->begin(); mIt != tmpList->end(); ++mIt){
					wxString literal = (*mIt);
					nodeList->push_back(literal);
				}
				tmpList->clear();
				literalList->push_back(nodeList);
			}
		}
	}
	//last expression of the disjunctive normalform doesn't yet list into the
	//list. So, this have to be still done.
	if (!tmpList->empty())
		literalList->push_back(tmpList);

	//it is looked after expressions, that are minimized with help of the absorption-laws.
	list<SP_ListString*>::iterator mIt;
	list<SP_ListString*>::iterator wIt;
	mIt = literalList->begin();

	while (mIt != literalList->end()){
		SP_ListString* term1 = (*mIt);
		SP_ListString::iterator sIt;
		if (term1->size() == 1){
			wIt = literalList->begin();
			while (wIt != literalList->end()){
				if ((*mIt) != (*wIt)){
					SP_ListString* term2 = (*wIt);
					SP_ListString::iterator t1It = (*mIt)->begin();

					// rule X + XY = X and X + X = X
					if ((SP_Find(*term2, *t1It) != term2->end())
						&& !LiteralIsNegated((*t1It))){
							literalList->erase(wIt);
							wIt = literalList->begin();
					}else{
						// rule X + -XY = X + Y
						if (!LiteralIsNegated(*t1It)){
							wxString& l_rsT1 = *t1It;
							wxString literal = wxT("X0") + l_rsT1.SubString(2, l_rsT1.length());
							if (SP_Find(*term2, literal) != term2->end()
								&& term2->size() > 1){
								term2->remove(literal);

							}
						}
					}
				}
				++wIt;
			}
		}
		++mIt;
	}

	//replace old disjunctive normalform with the new shortened disjunctive normalform
	dnfForm->clear();
	for (mIt = literalList->begin(); mIt != literalList->end(); ++mIt){
		SP_ListString* term = (*mIt);
		if (mIt != literalList->begin())
			dnfForm->push_back(wxString(wxT("OR")));
		for (nIt = term->begin(); nIt != term->end(); ++nIt){
			wxString& l_rsDNF = *nIt;
			dnfForm->push_back(l_rsDNF);
		}
	}

}

/** After the multiplication, the disjunctive normalform consists of expressions in form
	of X1X2X3. But to analyse these expressions, the variables of each expression have to
	be divided into individual variables like X1, X2, X3
*/
void
SP_DS_FTreeDNFCreater::CutLiterals()
{
	SP_ListString::iterator lIt;
	SP_ListString* dnfFormWithSeparateLiterals = new SP_ListString();

	for (lIt = dnfForm->begin(); lIt != dnfForm->end(); ++lIt){
		wxString& l_rsDNF = *lIt;
		int posBegin;
		int posEnd;
		if (wxStrstr(l_rsDNF, wxT("X"))){
			if ((posBegin = l_rsDNF.Find(wxT("X"))) > -1){
				if ((posEnd = l_rsDNF.find(wxT("X"), (posBegin + 1))) > -1){
					dnfFormWithSeparateLiterals->push_back(l_rsDNF.SubString(posBegin, (posEnd - 1)));
					posBegin = posEnd;
					while ((posEnd = l_rsDNF.find(wxT("X"), (posBegin + 1))) > -1) {
						dnfFormWithSeparateLiterals->push_back(l_rsDNF.SubString(posBegin, (posEnd - 1)));
						posBegin = posEnd;
					}
				}
				dnfFormWithSeparateLiterals->push_back(l_rsDNF.SubString(posBegin, l_rsDNF.length()));
			}
		}else
			dnfFormWithSeparateLiterals->push_back(l_rsDNF);
	}
	dnfForm->clear();
	dnfForm = dnfFormWithSeparateLiterals;
}
