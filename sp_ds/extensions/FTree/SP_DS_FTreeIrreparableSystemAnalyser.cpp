//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: Calculate reliability parameters of irreparable systems
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeIrreparableSystemAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreePseudoFormCreater.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeExprOfSuccessTreeCreater.h"

#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"


SP_DS_FTreeIrreparableSystemAnalyser::SP_DS_FTreeIrreparableSystemAnalyser()
: SP_DS_FTreeAnalyser()
{
	//pseudo polynom form to calculate the unrealiability of an event
	pseudoForm = new SP_ListString();
}

SP_DS_FTreeIrreparableSystemAnalyser::~SP_DS_FTreeIrreparableSystemAnalyser()
{

	pseudoForm->clear();
}

void
SP_DS_FTreeIrreparableSystemAnalyser::DoAnalyseIrreparableSystem(SP_DS_Node* p_pcNode)
{
	logExpression.clear();
	logExprAfterDeMorgan.clear();
	CalculateUnreliability(p_pcNode);
	CalculateReliability(p_pcNode);
	CalculateMTTF(p_pcNode);


	if (p_pcNode->GetClassName() !=  wxT("Basic Event")
		&& p_pcNode->GetClassName() !=  wxT("Undeveloped Event")) {
		CalculateFailureRate(p_pcNode);
	}
	CalculateProbabilityOfFailure(p_pcNode);
}

void
SP_DS_FTreeIrreparableSystemAnalyser::CalculateMTTF(SP_DS_Node* p_pcNode)
{

	double valueMTTF = 0.0;
	wxString sVal;

	if (p_pcNode->GetClassName()== wxT("Basic Event")
			|| p_pcNode->GetClassName()== wxT("Undeveloped Event")) {

			SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>
								(p_pcNode->GetAttribute(wxT("Failure Rate")));

			SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

			double fRate = l_pcFAttr->GetValueDouble();
			valueMTTF = 1.0 / fRate;
		}else{
			//to calculate the MTTF of intermediate events or the top event, first it
			//has to be determined the combination of basic events/undeveloped events,
			//which cause the event. On the basis of the logical expression, it will be created
			//the negated logical expression (this is the logical expression of the success tree)
			//and on the basis of this expression the pseudo polynom form
			SP_DS_FTreeLogicalExpressionReader* readExp = new SP_DS_FTreeLogicalExpressionReader();
			logExpression = readExp->FindLogicalExpression(p_pcNode);
			logExprAfterDeMorgan.clear();
			logExprAfterDeMorgan = readExp->DoDeMorgan();

			SP_DS_FTreeExprOfSuccessTreeCreater* pExpr = new SP_DS_FTreeExprOfSuccessTreeCreater();
			pseudoForm = pExpr->createExpression(logExprAfterDeMorgan);

			//the pseudo polynom form consists of strings of terms. The variables of a term
			//are in the form of X1X2X3... But to evaluate these variables, they have to be separated
			//of each other.
			list<SP_ListString*>* pseudoFormWithSeparateLiterals = CutLiterals(pseudoForm);
			//calculation of the MTTF
			valueMTTF = GetMTTF(pseudoFormWithSeparateLiterals);
		}
	sVal.Printf(wxT("%.16G"), valueMTTF);
	p_pcNode->GetAttribute(wxT("MTTF"))->SetValueString(sVal);
}

void
SP_DS_FTreeIrreparableSystemAnalyser::CalculateReliability(SP_DS_Node* p_pcNode)
{

	double value = 0.0;
		wxString u_Val = p_pcNode->GetAttribute(wxT("Irreparable System - Unreliability"))->GetValueString();
		double uValue;
		u_Val.ToDouble(&uValue);

		value = 1 - uValue;
		wxString r_Val;
		r_Val.Printf(wxT("%.16G"), value);
		p_pcNode->GetAttribute(wxT("Irreparable System - Reliability"))->SetValueString(r_Val);
}

void
SP_DS_FTreeIrreparableSystemAnalyser::CalculateUnreliability(SP_DS_Node* p_pcNode)
{
	double uValue = 0.0;

		if (p_pcNode->GetClassName()== wxT("Basic Event")
		|| p_pcNode->GetClassName()== wxT("Undeveloped Event")) {

		SP_DS_ListAttribute* l_pcListFAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Failure Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListFAttr);

		double fRate = l_pcFAttr->GetValueDouble();

		uValue = fRate;

		}else{

			//to calculate the unreliability of an intermediate event or the top event, first
			//it has to be determined the combination of basic events/undeveloped events,
			//which cause the event. On the basis of the logical expression, it will be created
			//the pseudo polynom form to determine the unreliability of an event.
			SP_DS_FTreeLogicalExpressionReader* readExp = new SP_DS_FTreeLogicalExpressionReader();
			logExpression = readExp->FindLogicalExpression(p_pcNode);
			logExprAfterDeMorgan = readExp->DoDeMorgan();

			SP_DS_FTreePseudoFormCreater* pForm = new SP_DS_FTreePseudoFormCreater();
			pseudoForm = pForm->createPseudoPolynomalForm(logExprAfterDeMorgan);

			//the pseudo polynom form consists of strings of terms. The variables of a term
			//are in the form of X1X2X3... But to evaluate these variables, they have to be separated
			//of each other.
			list<SP_ListString*>* pseudoFormWithSeparateLiterals = CutLiterals(pseudoForm);
			//calculation of the unreliability
			uValue = GetUnreliability(pseudoFormWithSeparateLiterals);
		}
		wxString sVal;
		sVal.Printf(wxT("%.16G"), uValue);
		p_pcNode->GetAttribute(wxT("Irreparable System - Unreliability"))->SetValueString(sVal);

}

double
SP_DS_FTreeIrreparableSystemAnalyser::GetMTTF(list<SP_ListString*>* pseudoFormWithSeparateLiterals)
{

	double value = 0.0;
	double fRate;
	SP_ListString::iterator sIt;
	list<SP_ListString*>::iterator mIt;
	for (mIt = pseudoFormWithSeparateLiterals->begin(); mIt != pseudoFormWithSeparateLiterals->end(); ++mIt){
		fRate = 0.0;

		double multVal = 1;
		sIt = (*mIt)->begin();
		wxString& l_rString = *sIt;
		wxString arithSign = l_rString.SubString(0,0);
		wxString multNum;
		if (l_rString.size() > 1){
			multNum = l_rString.SubString(1,l_rString.length());
			multNum.ToDouble(&multVal);
		}
		//Evaluation of the terms of the pseudo polynom form on the basis of
		//the wxT("idempotenz-rule" and the "orthonality-rule"): expressions like X*X..*X
		//are shortened. Terms that consist of expressions like X*-X are deleted.
		SP_ListString* nList =	UseIdemPotenzRule((*mIt));

		//each term of the pseudo polynom form are calculated
		if (!nList->empty()){

			fRate = 0.0;
			SP_ListString::iterator tIt;
			for (tIt = nList->begin(); tIt != nList->end(); ++tIt){
				NodeMap::iterator pIt;

				for (pIt = logExpression.begin(); pIt != logExpression.end(); ++pIt) {
					wxString id1 = ((*pIt).second->m_name).SubString(2, ((*pIt).second->m_name).length());
					wxString& l_rT = *tIt;
					wxString id2 = l_rT.SubString(2, l_rT.length());
					//to each variable in the term, it is determined the node in the fault tree
					if (id1== id2){

						SP_DS_Node* node = (*pIt).second->m_node;
						fRate += GetValue(node, wxT("Failure Rate"));
						break;
					}
				}
			}
			//last build the sum
			if (arithSign== wxT("+")){
				value += (multVal / fRate);
			}else{
				value -= (multVal / fRate);
			}
		}
	}
	return value;
}

double
SP_DS_FTreeIrreparableSystemAnalyser::GetUnreliability(list<SP_ListString*>* pseudoFormWithSeparateLiterals)
{

	double value = 0.0;

	SP_ListString::iterator sIt;
	list<SP_ListString*>::iterator mIt;
	for (mIt = pseudoFormWithSeparateLiterals->begin(); mIt != pseudoFormWithSeparateLiterals->end(); ++mIt){

		sIt = (*mIt)->begin();
		wxString& l_rString = *sIt;
		wxString arithSign = l_rString.SubString(0,0);
		wxString multNum;
		double multVal = 1;
		if (l_rString.size() > 1){
			multNum = l_rString.SubString(1,l_rString.length());
			multNum.ToDouble(&multVal);
		}
		//Evaluation of the terms of the pseudo polynom form on the basis of
		//the wxT("idempotenz-rule" and the "orthonality-rule"): expressions like X*X..*X
		//are shortened. Terms that consist of expressions like X*-X are deleted.
		SP_ListString* nList =	UseIdemPotenzRule((*mIt));

		//for each term of the pseudo polynom form, the product of the unrealiabilties are
		//calculated.

		if (!nList->empty()){
			SP_ListString::iterator tIt;
			double sValue = 1;
			for (tIt = nList->begin(); tIt != nList->end(); ++tIt){
				NodeMap::iterator pIt;
				double uValue = 0.0;

				for (pIt = logExpression.begin(); pIt != logExpression.end(); ++pIt) {

					wxString id1 = ((*pIt).second->m_name).SubString(2, ((*pIt).second->m_name).length());
					wxString l_rT = *tIt;
					wxString id2 = l_rT.SubString(2, l_rT.length());
					//to each variable in the term, it is determined the node in the fault tree
					if (id1== id2){
						SP_DS_Node* node = (*pIt).second->m_node;
						//Is the variable negated, then the variable is replaced by the value
						//of reliability. Is the variable not negated, we use the value
						//of the unreliability of the event
						if (LiteralIsNegated((*tIt))){
							uValue = GetValue(node, wxT("Irreparable System - Reliability"));
						}else{
							uValue = GetValue(node, wxT("Irreparable System - Unreliability"));
						}
						break;
					}
				}
				sValue *= uValue;
			}
			if (arithSign== wxT("+")){
				value += (multVal * sValue);
			}else{
				value -= (multVal * sValue);
			}
		}
	}
	return value;
}









