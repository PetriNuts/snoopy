//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: methods to create algebraic equations
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/FTree/SP_DS_FTreeAlgebraicEquationCreater.h"


SP_DS_FTreeAlgebraicEquationCreater::SP_DS_FTreeAlgebraicEquationCreater()
{

}

SP_DS_FTreeAlgebraicEquationCreater::~SP_DS_FTreeAlgebraicEquationCreater()
{

}


void
SP_DS_FTreeAlgebraicEquationCreater::CreateForm (LiteralMap vMap)
{
	LiteralMap::iterator pIt;
	wxString element;
	SP_ListString* Term = new SP_ListString();
	StackMap stack;

	long sId = 0;
	long idB = -1;
	long idE = -1;
	LiteralMap l_pcTemp;
	LiteralMap::iterator it;

	//The method is used to interpret the logical expression in the right sequence:
	//In each case, the most inner expression is first interpreted. At first, the method
	//looks for the most inner expression and replaces this expression in the main logical
	//expression with a variable.
	//The expression is saved in a stack in addition with the new variable.

	//First: the bracket pairs of expressions, that consist of only one variable, are deleted.
	vMap = EliminateBracketPair(vMap);
	while (vMap.size() > 1) {

		for(pIt = vMap.begin(); pIt != vMap.end(); pIt++) {

			 element = (*pIt).second;

			 if (element ==  wxT("(")) {
				 if (!Term->empty()){
					Term = new SP_ListString();
					//sId = 0;
					idB = (*pIt).first;
				 }

				 Term->push_back(element);
			 } else {
				 //the most inner expression was found. Now, this expression is saved
				 //in a stack on the basis of a identifier-name called wxT("Y") + ID (this is
				 //the new variable in the main logical expression).

				 if (element ==  wxT(")")) {

					Term->push_back(element);
					wxString l_sVal;
					l_sVal.Printf(wxT("%ld"), (sId + 1) );
					tMap[wxT("Y" + l_sVal)] = Term;
					idE = (*pIt).first;
					Term = new SP_ListString();
					sId++;
					break;

				}else{

					Term->push_back(element);
				}
			 }
		 }
		 long nId = 0;
		 // The position of the most inner expression that was found in the main logical
		 //expression, is replaced with the new variable of the typ wxT("Y") + ID.
		 for (it = vMap.begin(); it != vMap.end(); ++it) {
			if ((*it).first < idB || (*it).first > idE){
				nId++;
				l_pcTemp[nId] = (*it).second;
			}
			if ((*it).first == idB){
				nId++;
				wxString l_sVal;
				l_sVal.Printf(wxT("%ld"), sId );
				l_pcTemp.insert(SP_PairLongString
											(nId, wxT("Y") + l_sVal));
			}
		}

		vMap.clear();
		//before looking the next most inner expression:
		// the bracket pairs of expressions, that consist of only one variable, are deleted.
		vMap = EliminateBracketPair(l_pcTemp);
		l_pcTemp.clear();

	}
}

LiteralMap
SP_DS_FTreeAlgebraicEquationCreater::EliminateBracketPair(LiteralMap termMap)
{
	//looks for expression that consists of only one variable. The brackets of these
	//expressions are deleted.
	LiteralMap::iterator pIt;
	wxString element;
	long idBegin = 0;
	long idEnd = 0;
	bool mustDelete = FALSE;
	int id = 0;
	for(pIt = termMap.begin(); pIt != termMap.end(); ++pIt) {
		id++;
		element = (*pIt).second;
		//remember open bracket
		if (element ==  wxT("("))
			idBegin = id;
		//remember closed bracket
		if (element ==  wxT(")")) {
			idEnd = id;
			//expressions with only one variable, are marked. These brackets
			//can be deleted.
			if ((idBegin + 2) == idEnd) {
				mustDelete = TRUE;
				break;
			}
		}
	}
	//delete bracket pair
	if (mustDelete){
		id = 0;
		LiteralMap tmpMap;
		for(pIt = termMap.begin(); pIt != termMap.end(); ++pIt) {
			id++;
			if (id != idBegin && id != idEnd)
				tmpMap.insert(SP_PairLongString((*pIt).first, (*pIt).second));
		}
		termMap.clear();
		termMap = tmpMap;
		EliminateBracketPair(termMap);
	}


    return termMap;
}

wxString
SP_DS_FTreeAlgebraicEquationCreater::CheckArithmeticSign(const wxString& sVal1, const wxString& sVal2)
{

	wxString arithSign;
	//check arithemtic sign, if two expressions will be multiplied with each other
	if (wxStrstr(sVal1, wxT("-"))){
		if (wxStrstr(sVal2, wxT("-"))){
			arithSign = wxT("+");
		}else{
			arithSign = wxT("-");
		}
	}else{
		if (wxStrstr(sVal2, wxT("-"))){
			arithSign = wxT("-");
		}else{
			arithSign = wxT("+");
		}
	}
	return arithSign;
}

SP_ListString*
SP_DS_FTreeAlgebraicEquationCreater::DoMultiplicate(SP_ListString* term1, SP_ListString* term2)
{
	SP_ListString::iterator t1It;
	SP_ListString::iterator t2It;
	SP_ListString* newTerm = new SP_ListString();

	//Multiplication of two expressions
	for (t1It = term1->begin(); t1It != term1->end(); ++t1It)
	{
		wxString& l_rsT1 = *t1It;
		for (t2It = term2->begin(); t2It != term2->end(); ++t2It)
		{
			wxString& l_rsT2 = *t2It;
			wxString num1 = wxT("");
			wxString num2 = wxT("");
			wxString event1 = wxT("");
			wxString event2 = wxT("");
			wxString event;
			wxString num;
			long sNum1;
			long sNum2;

			//check the arithemtic sign of every expression
			wxString arithSign1 = l_rsT1.SubString(0,0);
			wxString arithSign2 = l_rsT2.SubString(0,0);


			//check every expression, if they consists of variables or of only a number without variables
			int posNum = l_rsT1.First(wxT("X"));
			//analyse the first expression
			if (posNum < 0 ){
				num1 = l_rsT1.SubString(1,1);
				num1.ToLong(&sNum1);
			}else{
				//check whether a multiple comes before the variables
				if ( posNum == 1){
					event1 = l_rsT1.SubString(1,l_rsT1.length());
					num1 = wxT("1");
					sNum1 = 1;
				}else{
					num1 = l_rsT1.SubString(1, (posNum - 1));
					num1.ToLong(&sNum1);
					event1 = l_rsT1.SubString(posNum, l_rsT1.length());
				}
			}
			//analyse the second expression
			posNum = l_rsT2.First(wxT("X"));
			if (posNum < 0 ){
				num2 = l_rsT2.SubString(1,1);
				num2.ToLong(&sNum2);
			}else{
				//check whether a multiple comes before the variables
				if ( posNum == 1){
					event2 = l_rsT2.SubString(1,l_rsT2.length());
					num2 = wxT("1");
					sNum2 = 1;
				}else{
					num2 = l_rsT2.SubString(1, (posNum - 1));
					num2.ToLong(&sNum2);
					event2 = l_rsT2.SubString(posNum, l_rsT2.length());
				}
			}

			//ckeck the arithmetic sign of the new expression
			wxString arithSign = CheckArithmeticSign(arithSign1, arithSign2);

			//new expression (result of the multiplication)
			long newVal = sNum1 * sNum2;
			num.Printf(wxT("%ld"), newVal);

			event = event1 + event2;

			newTerm->push_back(arithSign + num + event);
		}
	}
		return newTerm;
}

bool
SP_DS_FTreeAlgebraicEquationCreater::LiteralIsNegated(const wxString& name)
{
	//if the flag of the variable is wxT("0"), so the variable is negated
	if (name.SubString(1,1) ==  wxT("0"))
		return TRUE;
	return FALSE;
}




