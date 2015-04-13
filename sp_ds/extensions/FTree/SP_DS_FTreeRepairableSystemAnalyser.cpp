//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeRepairableSystemAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreePseudoFormCreater.h"

#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"


SP_DS_FTreeRepairableSystemAnalyser::SP_DS_FTreeRepairableSystemAnalyser()

{
	//pseudo polynom form to calculate the unavailability of an event
	pseudoForm = new SP_ListString();
	fFrequency = 0;
}

SP_DS_FTreeRepairableSystemAnalyser::~SP_DS_FTreeRepairableSystemAnalyser()
{

	pseudoForm->clear();
}

void
SP_DS_FTreeRepairableSystemAnalyser::DoAnalyseRepairableSystem(SP_DS_Node* p_pcNode)
{
	logExpression.clear();
	logExprAfterDeMorgan.clear();
	CalculateUnavailability(p_pcNode);
	CalculateAvailability(p_pcNode);
	CalculateMTTF(p_pcNode);
	CalculateMTTR(p_pcNode);


	if (p_pcNode->GetClassName() !=  wxT("Basic Event")
		&& p_pcNode->GetClassName() !=  wxT("Undeveloped Event")) {
		CalculateFailureRate(p_pcNode);
		CalculateRepairRate(p_pcNode);
	}

	CalculateMTBF(p_pcNode);
	CalculateProbabilityOfFailure(p_pcNode);
}

double
SP_DS_FTreeRepairableSystemAnalyser::GetAverageFailureFrequency()
{
	return fFrequency;
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateMTTF(SP_DS_Node* p_pcNode)
{
	//MTTF is calculated on the basis of the average failure frequency.
	double valueMTTF = 0.0;
	wxString sVal;
	if (p_pcNode->GetClassName() ==  wxT("Basic Event")
		|| p_pcNode->GetClassName() ==  wxT("Undeveloped Event")) {

		SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>
								(p_pcNode->GetAttribute(wxT("Failure Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

		double fRate = l_pcFAttr->GetValueDouble();

		valueMTTF = 1.0 / fRate;
	}else{
		if (fFrequency != 0){
			wxString sValue = p_pcNode->GetAttribute(wxT("Repairable System - Availability"))->GetValueString();
			double aValue;
			sValue.ToDouble(&aValue);
			valueMTTF = aValue / fFrequency;
		}
	}
	sVal.Printf(wxT("%.16G"), valueMTTF);
	p_pcNode->GetAttribute(wxT("MTTF"))->SetValueString(sVal);
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateMTTR(SP_DS_Node* p_pcNode)
{
	//MTTR is calculated on the basis of the average failure frequency.
	double valueMTTR = 0.0;
	wxString sVal;

	if (p_pcNode->GetClassName() ==  wxT("Basic Event")
		|| p_pcNode->GetClassName() ==  wxT("Undeveloped Event")) {

		SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>
								(p_pcNode->GetAttribute(wxT("Repair Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

		double rRate = l_pcFAttr->GetValueDouble();

		valueMTTR = 1.0 / rRate;

	}else{
		if (fFrequency != 0){
			wxString sValue = p_pcNode->GetAttribute(wxT("Repairable System - Unavailability"))->GetValueString();
			double uValue;
			sValue.ToDouble(&uValue);
			valueMTTR = uValue / fFrequency;
		}
	}
	sVal.Printf(wxT("%.16G"), valueMTTR);
	p_pcNode->GetAttribute(wxT("MTTR"))->SetValueString(sVal);
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateMTBF(SP_DS_Node* p_pcNode)
{

	double rRate;
	double fRate;
	if (p_pcNode->GetClassName() ==  wxT("Basic Event")
		|| p_pcNode->GetClassName() ==  wxT("Undeveloped Event")) {

		SP_DS_ListAttribute* l_pcListFAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Failure Rate")));

		SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListFAttr);

		fRate = l_pcFAttr->GetValueDouble();

		SP_DS_ListAttribute* l_pcListRAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Repair Rate")));

		SP_DS_DoubleListAttribute* l_pcRAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListRAttr);

		rRate = l_pcRAttr->GetValueDouble();
	}else{
		wxString fValue = p_pcNode->GetAttribute(wxT("Failure Rate"))->GetValueString();

		fValue.ToDouble(&fRate);

		wxString rValue = p_pcNode->GetAttribute(wxT("Repair Rate"))->GetValueString();

		rValue.ToDouble(&rRate);
	}

		double valueMTTF = (1.0 / fRate);
		double valueMTTR = (1.0 / rRate);
		double valueMTBF = valueMTTF + valueMTTR;
		wxString sVal;
		sVal.Printf(wxT("%.16G"), valueMTBF);
		p_pcNode->GetAttribute(wxT("MTBF"))->SetValueString(sVal);

}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateAvailability(SP_DS_Node* p_pcNode)
{

	double value = 0.0;
	wxString u_Val = p_pcNode->GetAttribute(wxT("Repairable System - Unavailability"))->GetValueString();
	double uValue;
	u_Val.ToDouble(&uValue);

	value = 1 - uValue;
	wxString a_Val;
	a_Val.Printf(wxT("%.16G"), value);
	p_pcNode->GetAttribute(wxT("Repairable System - Availability"))->SetValueString(a_Val);
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateUnavailability(SP_DS_Node* p_pcNode)
{
	double uValue = 0.0;

	if (p_pcNode->GetClassName() ==  wxT("Basic Event")
	|| p_pcNode->GetClassName() ==  wxT("Undeveloped Event")) {

	SP_DS_ListAttribute* l_pcListFAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Failure Rate")));

	SP_DS_DoubleListAttribute* l_pcFAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListFAttr);

	double fRate = l_pcFAttr->GetValueDouble();


	SP_DS_ListAttribute* l_pcListRAttr = dynamic_cast<SP_DS_ListAttribute*>
							(p_pcNode->GetAttribute(wxT("Repair Rate")));

	SP_DS_DoubleListAttribute* l_pcRAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListRAttr);


	double rRate = l_pcRAttr->GetValueDouble();
	uValue = fRate / (rRate + fRate);

	}else{
		//to calculate the unavailability of an intermediate event or the top event, first
		//it has to be determined the combination of basic events/undeveloped events,
		//which cause the event. On the basis of the logical expression, it will be created
		//the pseudo polynom form to determine the unavailability of an event.
		fFrequency = 0;
		SP_DS_FTreeLogicalExpressionReader* readExp = new SP_DS_FTreeLogicalExpressionReader();
		logExpression = readExp->FindLogicalExpression(p_pcNode);
		logExprAfterDeMorgan = readExp->DoDeMorgan();

		SP_DS_FTreePseudoFormCreater* pForm = new SP_DS_FTreePseudoFormCreater();
		pseudoForm = pForm->createPseudoPolynomalForm(logExprAfterDeMorgan);

		//the pseudo polynom form consists of strings of terms. The variables of a term
		//are in the form of X1X2X3... But to evaluate these variables, they have to be separated
		//of each other.
		list<SP_ListString*>* pseudoFormWithSeparateLiterals = CutLiterals(pseudoForm);
		CalculateAverageFailureFrequency(pseudoFormWithSeparateLiterals);

		//calculation of the unavailability of an event
		uValue = GetUnavailability(pseudoFormWithSeparateLiterals);
	}
	wxString sVal;
	sVal.Printf(wxT("%.16G"), uValue);
	p_pcNode->GetAttribute(wxT("Repairable System - Unavailability"))->SetValueString(sVal);
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateRepairRate(SP_DS_Node* p_pcNode)
{
	double rRate;
	wxString sRRate;
	double valueMTTR;
	wxString sMTTR = p_pcNode->GetAttribute(wxT("MTTR"))->GetValueString();
	sMTTR.ToDouble(&valueMTTR);
	rRate = 1 / valueMTTR;
	sRRate.Printf(wxT("%.16G"), rRate);
	p_pcNode->GetAttribute(wxT("Repair Rate"))->SetValueString(sRRate);
}

void
SP_DS_FTreeRepairableSystemAnalyser::CalculateAverageFailureFrequency(list<SP_ListString*>* pseudoFormWithSeparateLiterals)
{
	//Calculation of the average failure frequency. This is demanded to calculate around MTTF and MTTR.
	double m_nValue = 0;
	SP_ListString::iterator tIt;
	double value = 0.0;

	list<SP_ListString*>::iterator mIt;
	for (mIt = pseudoFormWithSeparateLiterals->begin(); mIt != pseudoFormWithSeparateLiterals->end(); ++mIt){
		m_nValue = 0;
		tIt = (*mIt)->begin();
		wxString& l_rString = *tIt;
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
		SP_ListString* nList = UseIdemPotenzRule((*mIt));

		if (!nList->empty()){

			double sValue = 1;
			SP_ListString::iterator sIt;
			for (sIt = nList->begin(); sIt != nList->end(); ++sIt){
				wxString& l_rSString = *sIt;
				NodeMap::iterator pIt;
				double uValue = 1;

				for (pIt = logExpression.begin(); pIt != logExpression.end(); ++pIt) {
					wxString nodeName = (*pIt).second->m_name;
					if (nodeName.SubString(2,nodeName.length()) == l_rSString.SubString(2,l_rSString.length())){
						SP_DS_Node* node = (*pIt).second->m_node;

						//Is the variable negated, then we use the value of availability and
						//the failure rate. Is the variable not negated, we use the value
						//of the unavailability and the repair rate of the event
						if (LiteralIsNegated(l_rSString)){
							uValue = GetValue(node, wxT("Repairable System - Availability"));
							m_nValue -= GetValue(node, wxT("Failure Rate"));
						}else{
							uValue = GetValue(node, wxT("Repairable System - Unavailability"));
							m_nValue += GetValue(node, wxT("Repair Rate"));
						}
					break;
					}
				}
				sValue *= uValue;
			}
			if (arithSign ==  wxT("+")){
				value += (multVal * sValue * m_nValue);
			}else{
				value -= (multVal * sValue * m_nValue);
			}
			fFrequency += value;
			value = 0.0;
		}
	}
}

double
SP_DS_FTreeRepairableSystemAnalyser::GetUnavailability(list<SP_ListString*>* pseudoFormWithSeparateLiterals)
{

	double value = 0.0;

	SP_ListString::iterator sIt;
	list<SP_ListString*>::iterator mIt;
	for (mIt = pseudoFormWithSeparateLiterals->begin(); mIt != pseudoFormWithSeparateLiterals->end(); ++mIt){

		sIt = (*mIt)->begin();
		wxString& l_rSString = *sIt;
		wxString arithSign = l_rSString.SubString(0,0);
		wxString multNum;
		double multVal = 1;
		if (l_rSString.size() > 1){
			multNum = l_rSString.SubString(1,l_rSString.length());
			multNum.ToDouble(&multVal);
		}

		//Evaluation of the terms of the pseudo polynom form on the basis of
		//the wxT("idempotenz-rule" and the "orthonality-rule"): expressions like X*X..*X
		//are shortened. Terms that consist of expressions like X*-X are deleted.
		SP_ListString* nList =	UseIdemPotenzRule((*mIt));

		//each term of the pseudo polynom form are calculated
		if (!nList->empty()){
			SP_ListString::iterator tIt;
			double sValue = 1;
			for (tIt = nList->begin(); tIt != nList->end(); ++tIt){
				wxString& l_rString = *tIt;
				NodeMap::iterator pIt;
				double uValue = 1;

				for (pIt = logExpression.begin(); pIt != logExpression.end(); ++pIt) {
					//to each variable in the term, it is determined the node in the fault tree
					wxString nodeName = (*pIt).second->m_name;
					if (nodeName.SubString(2,nodeName.length()) == l_rString.SubString(2,l_rString.length())){
						SP_DS_Node* node = (*pIt).second->m_node;
						//Is the variable negated, then the variable is replaced by the value
						//of availability. Is the variable not negated, we use the value
						//of the unavailability of the event
						if (LiteralIsNegated(l_rString)){
							uValue = GetValue(node, wxT("Repairable System - Availability"));
						}else{
							uValue = GetValue(node, wxT("Repairable System - Unavailability"));
						}
						break;
					}
				}
			sValue *= uValue;
		}
		if (arithSign ==  wxT("+")){
			value += (multVal * sValue);
		}else{
			value -= (multVal * sValue);
		}
		}
	}

	return value;
}









