//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ak $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/07 11:55:00 $
// Short Description: logical expression reader
//////////////////////////////////////////////////////////////////////

#include "sp_core/FTree/SP_FTree_TermScanner.h"
#include "sp_core/FTree/SP_FTree_TermParser.h"

#include "sp_defines.h"

Term::Term(const wxString& name, bool isNegatived)
  : m_name(name),
	isNeg(isNegatived)
{}

ScannerText::ScannerText(const wxString& name, long pos)
  : m_name(name),
	m_pos(pos)
{}

SP_FTree_TermParser::SP_FTree_TermParser(SP_FTree_TermScanner* p_pcScanner):
m_pcScanner(p_pcScanner),
variableList(NULL),
identifierList(NULL),
allTermMap(NULL)
{
	//counter for list of variables
	variableId = 0;
	//counter for list of identifiers (negations)
	identifierId = 0;
	stopToken = 0;

	//list of names of variables (one element of this list
	//consists of the name of the variable and the position
	//of the variable in the file)
	variableList = new map<long, ScannerText*>();

	//list of negations (one element of this list consists of
	//the negation-identifier and the position of the negation-
	//identifier in the file)
	identifierList = new map<long, ScannerText*>();

	//list of all variables of the disjunctive normalform
	//(one element of this list consists of the name of the variable and if the variable
	//is negated)
	allTermMap = new map<long, Term*>();

}


SP_FTree_TermParser::~SP_FTree_TermParser()
{
	variableList->clear();
	identifierList->clear();
}


void
SP_FTree_TermParser:: ReadUntilBeginExpression(){

	variableList->clear();
	identifierList->clear();
	allTermMap->clear();

	int token = m_pcScanner->GetNextToken();

	//First, the file is read until wxT(":") is reached.
	//Only then the real disjunctive normalfom begins.
	while(token != SP_SCANNER_EOF){
		if(token == SP_SCANNER_COLON){
			break;
		}
		token = m_pcScanner->GetNextToken();
	}
	m_pcScanner->SetPos(0);

}

bool
SP_FTree_TermParser:: ReadNodeData(wxString& p_sNodeName)
{
	//next token in the file is read
	int l_nToken = m_pcScanner->GetNextToken();

	//if wxT("+" or "-") are read, it is not a disjunctive normalform,
	//but a pseudo polynom form
	if (l_nToken == SP_SCANNER_PLUS || l_nToken == SP_SCANNER_MINUS){
		stopToken = l_nToken;
		return FALSE;
	}

	//elements of the disjunctive normalform are read
	if(l_nToken == SP_SCANNER_TEXT){
		if(! ReadNodeName(p_sNodeName))
			return FALSE;
	}

	//end of the file is reached
	if(l_nToken == SP_SCANNER_EOF){
		stopToken = l_nToken;
		return FALSE;
	}

	return TRUE;
}

bool
SP_FTree_TermParser :: ReadNodeName(wxString& p_sName){

	//reads the elements of the disjunctive normalform
	p_sName = wxT("");
	wxString s = m_pcScanner->GetText();

	//If the next token begins with a wxT("X"), it is about a variable.
	//The name of the variable as well as the position of the variable
	//in the line of the file is saved in a list. The variable is interpreted
	//as a basic event.
	if(s.StartsWith(wxT("X")))
	{
		p_sName = wxString::Format(wxT("%s"), s.c_str() );
		long pos = m_pcScanner->GetPos();
		variableId++;
		ScannerText* name = new ScannerText(s, pos);
		variableList->insert(pair< long, ScannerText*>(variableId, name));

		return TRUE;
	}

	//If the next token is an identifier wxT("_"), it is about a negation of a variable.
	//The token is marked as a NEG. The token as well as the position of the token
	//in the line of the file is saved in a separate list. The NEG is interpreted as
	//a NEG-gate.
	if (s == wxT("_")){
		p_sName = wxString::Format(wxT("NEG"));
		long pos = m_pcScanner->GetPos();
		identifierId++;
		ScannerText* name = new ScannerText(wxT("NEG"), pos);
		identifierList->insert(pair< long, ScannerText*>(identifierId, name));

		return TRUE;
	}

	//If the next token is a wxT("v"), then the token is interpreted as an OR-gate
	if (s == wxT("v")){
		p_sName = wxString::Format(wxT("OR"));
	}else{
		stopToken = 0;
		return FALSE;
	}
	return true;
}

bool
SP_FTree_TermParser::CheckVariableOfIsNegated()
{
	//If no wxT("_") has been read in the file, no negated variable exists in
	//the disjunctive normalform.
	if (identifierList->empty())
		return FALSE;

	map<long, ScannerText*>::iterator vIt;
	map<long, ScannerText*>::iterator iIt;
	bool isNeg = FALSE;
	long id = 0;

	//Apart from that, check which variable is negated
	for (vIt = variableList->begin(); vIt != variableList->end(); ++vIt)
	{
		//reads the position of the variable in the line of the file
		ScannerText* variable = (*vIt).second;
		long posVar = variable->m_pos;
		for (iIt = identifierList->begin(); iIt != identifierList->end(); ++iIt)
		{
			//reads the position of the identifier in the line of the file
			ScannerText* identifier = (*iIt).second;
			long posIdent = identifier->m_pos;
			//Compare both positions. If the position of the variable in the line is the same
			//as the position of the negation-identifier, so we know the variable is negated.
			if (posVar == (posIdent + 1)){

				id++;
				Term* node = new Term(variable->m_name, TRUE);
				//The variable is inserted as a negated variable in a list...
				allTermMap->insert(pair<long, Term*>(id, node));

				//...and is marked as negated
				isNeg = TRUE;
				break;
			}
		}

		//If the variable is negated, the position of the negation-identifier is deleted in the list.
		//(The reason is: in every line, the position begins always with wxT("0").)
		if (isNeg){
			identifierList->erase((*iIt).first);
			isNeg = FALSE;
		}else{
			//If there is no negation-identifier for this variable, the variable is marked
			//as not negated.
			id++;
			Term* node = new Term(variable->m_name, FALSE);

			allTermMap->insert(pair<long, Term*>(id, node));
		}
	}
	return TRUE;
}

bool
SP_FTree_TermParser::IsNegated(const wxString& name)
{
	map<long, Term*>::iterator tIt;
	bool isNeg = FALSE;
	int id = -1;

	//To the name of each variable, it is looked for name of this variable in the list.
	//It is checked whether the variable is negated.
	for (tIt = allTermMap->begin(); tIt != allTermMap->end(); ++tIt)
	{
		Term* node = (*tIt).second;

		if (node->m_name == name){
			id = (*tIt).first;
			isNeg = node->isNeg;
			break;
		}

	}
	//After the check, the variable is deleted in the list.
	//(The reason is: The variable can exist repeatedly in the list.
	//In addition, this variable can be negated or not negated.
	//So to check the right variable, variables which are checked,
	//should be deleted now.)
	if(id != -1) allTermMap->erase(id);
	return isNeg;
}

bool
SP_FTree_TermParser::IsEOF()
{
	//checks whether the end of the file is reached
	if (stopToken == SP_SCANNER_EOF)
		return TRUE;

	return FALSE;
}
