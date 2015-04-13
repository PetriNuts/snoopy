//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: main class for analysing of repairable and not repairable systems
//////////////////////////////////////////////////////////////////////


#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreePseudoFormCreater.h"
#include <math.h>
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"


SP_DS_FTreeAnalyser::SP_DS_FTreeAnalyser()
{

}

SP_DS_FTreeAnalyser::~SP_DS_FTreeAnalyser()
{
    logExpression.clear();
	logExprAfterDeMorgan.clear();
}

void
SP_DS_FTreeAnalyser::CalculateProbabilityOfFailure(SP_DS_Node* p_pcNode)
{
	//Calculate the probability of failure of each event of a fault tree
	wxString probFailureVal;
	double fRate;

	if (p_pcNode->GetClassName()== wxT("Basic Event")
		|| p_pcNode->GetClassName()== wxT("Undeveloped Event")) {

		SP_DS_ListAttribute* l_pcListFAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Failure Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListFAttr);
		fRate = l_pcFAttr->GetValueDouble();
	}else{
		wxString fRateVal = p_pcNode->GetAttribute(wxT("Failure Rate"))->GetValueString();
		fRateVal.ToDouble(&fRate);
	}
	double probFailure = (1 - exp(-fRate))* 100;
	probFailureVal.Printf(wxT("%.3G"), probFailure);

	p_pcNode->GetAttribute(wxT("Probability of Failure (%)"))->SetValueString(probFailureVal);
	p_pcNode->GetAttribute(wxT("Probability of Failure (%)"))->SetShow(TRUE);
}

double
SP_DS_FTreeAnalyser::GetValue(SP_DS_Node* p_pcNode, const wxString& attrType)
{
	//returns the value of an attribute type of an event
	double value = 0.0;
	if (attrType== wxT("Failure Rate")
		|| attrType== wxT("Repair Rate")){
		SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(attrType));

		SP_DS_DoubleListAttribute* l_pcAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

		value = l_pcAttr->GetValueDouble();
	}else{
		wxString sVal = p_pcNode->GetAttribute(attrType)->GetValueString();
		sVal.ToDouble(&value);
	}
	return value;
}

void
SP_DS_FTreeAnalyser::CalculateFailureRate(SP_DS_Node* p_pcNode)
{
	//Calculate the failure rate of intermediate events and the top event
	//by 1 / MTTF
	double fRate;
	wxString sFRate;

	double valueMTTF;
	wxString vMTTF = p_pcNode->GetAttribute(wxT("MTTF"))->GetValueString();
	vMTTF.ToDouble(&valueMTTF);
	fRate = 1 / valueMTTF;

	sFRate.Printf(wxT("%.16G"), fRate);
	p_pcNode->GetAttribute(wxT("Failure Rate"))->SetValueString(sFRate);
}

void
SP_DS_FTreeAnalyser::ResetValues(SP_DS_Node* p_pcNode)
{
	//reset all calculated values of an event
	const SP_ListAttribute* attrList = p_pcNode->GetAttributes();
	SP_ListAttribute::const_iterator attrIt;

	for (attrIt = attrList->begin(); attrIt != attrList->end(); ++attrIt){
		if ((*attrIt)->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE)
			(*attrIt)->SetValueString(wxT("0"));
	}
}

bool
SP_DS_FTreeAnalyser::LiteralIsNegated(const wxString& name)
{
	//if a variable has the flag wxT("0"), the variable is negated
	if (name.SubString(1,1)== wxT("0"))
		return TRUE;
	return FALSE;
}

/** Idempotenzrule predicate, that terms in form of X1*X1 = X1
	and X1*-X1 = 0
*/
SP_ListString*
SP_DS_FTreeAnalyser::UseIdemPotenzRule(SP_ListString* term)
{
	// expressions of the form X * ...* X are shortened by X.
	// The method uses the application of the orthogonality -law at the same time.
	// Terms, that consist of variables in form of X * -X , are deleted completely.

	SP_ListString::iterator nIt;
	SP_ListString::iterator mIt;
	SP_ListString* tempList = new SP_ListString();
	bool wasFound = FALSE;

	for (nIt = term->begin(); nIt != term->end(); ++nIt){
		wxString& l_rsNString = *nIt;
		if (nIt != term->begin()){
			if (tempList->empty()){
				tempList->push_back(l_rsNString);
			}else{
				for (mIt = tempList->begin(); mIt != tempList->end(); ++mIt){
					wxString& l_rsMString = *mIt;
					wxString id1 = l_rsNString.SubString(2, l_rsNString.length());
					wxString id2 = l_rsMString.SubString(2, l_rsMString.length());
					//it was found a variable that already exits in this expression
					if (id1== id2){
						//the other variable is negated, so the expression will be deleted
						if (LiteralIsNegated(l_rsNString) != LiteralIsNegated(l_rsMString)){
							tempList->clear();
							return tempList;
						}else
							//the other variable is not negated
							wasFound = TRUE;
						break;
					}
				}
				//a variable is only been attached to the expression,
				//if it is not double or negated

				if (!wasFound)
					tempList->push_back(l_rsNString);
			}
			wasFound = FALSE;
		}
	}
	return tempList;
}

/** After the multiplication, the disjunctive normal form consists of expressions in form
	of X1X2X3. But to analyse these expressions, the variables of each expression have to
	be divided into individual variables like X1, X2, X3
*/

list<SP_ListString*>*
SP_DS_FTreeAnalyser::CutLiterals(SP_ListString* n_pcList)
{
	SP_ListString::iterator lIt;
	list<SP_ListString*>* pseudoFormWithSeparateLiterals = new list<SP_ListString*>();
	SP_ListString* termWithSeparateLiterals = new SP_ListString();

	for (lIt = n_pcList->begin(); lIt != n_pcList->end(); ++lIt){
		SP_ListString* tmpList = new SP_ListString();
		int posBegin;
		int posEnd;
		termWithSeparateLiterals = new SP_ListString();
		wxString& l_rsString = *lIt;
		if ((posBegin = l_rsString.Find(wxT("X"))) > -1){
			tmpList->push_back(l_rsString.SubString(0, (posBegin - 1)));
			if ((posEnd = l_rsString.find(wxT("X"), (posBegin + 1))) > -1){
				tmpList->push_back(l_rsString.SubString(posBegin, (posEnd - 1)));
				posBegin = posEnd;
				while ((posEnd = l_rsString.find(wxT("X"), (posBegin + 1))) > -1) {

					tmpList->push_back(l_rsString.SubString(posBegin, (posEnd - 1)));
					posBegin = posEnd;
				}
				tmpList->push_back(l_rsString.SubString(posBegin, l_rsString.length()));
			}else{
				tmpList->push_back(l_rsString.SubString(posBegin, l_rsString.length()));
			}
		}else{
			tmpList->push_back(l_rsString.SubString(0, l_rsString.length()));
		}
		SP_ListString::iterator nIt;
		for (nIt = tmpList->begin(); nIt != tmpList->end(); ++nIt){
			wxString expr = (*nIt);
			termWithSeparateLiterals->push_back(expr);
		}
		pseudoFormWithSeparateLiterals->push_back(termWithSeparateLiterals);
		tmpList->clear();
	}

	return pseudoFormWithSeparateLiterals;
}




