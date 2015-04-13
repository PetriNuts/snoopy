//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/12 11:55:00 $
// Short Description: Bool Reader, read terms of logical expression
//
//////////////////////////////////////////////////////////////////////
#ifndef __SP_FTREE_TERMPARSER_H__
#define __SP_FTREE_TERMPARSER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/FTree/SP_FTree_TermScanner.h"
#include "snoopy.h"

struct Term
{
  Term(const wxString&, bool);
  wxString m_name; //name of the variable
  bool isNeg;	//is a variable negated or not negated

};

struct ScannerText
{
  ScannerText(const wxString&, long);
  wxString m_name;	//name of the element of the logical expression
  long m_pos;		//position in the line

};


class SP_FTree_TermParser: public SP_Error
{
private:
	SP_FTree_TermScanner* m_pcScanner;

	//list of variables and their positions
	map<long, ScannerText*>* variableList;

	//list of negations of variables and their positions
	map<long, ScannerText*>* identifierList;

	//list of variables (variables are marked as negated or not negated)
	map<long, Term*>*  allTermMap;

	//ID of the list of variables
	long variableId;

	//ID of the list of negations
	long identifierId;
	int stopToken;

protected:

	/**
	\brief Determine the name of the token
	*/
	bool ReadNodeName(wxString& p_sName);

public:
    SP_FTree_TermParser(SP_FTree_TermScanner* p_pcScanner);
    ~SP_FTree_TermParser();

	/**
	\brief Run through of the lines until the logical expression begins
	*/
	void ReadUntilBeginExpression();

	/**
	\brief Reading of the elements in the file
	Kind of the element is determined (whether it is a variable,
	a negation or a logic operation)

	\param		p_sNodeName		element to be read
	\returns	p_sNodeName		name of the element
	*/
	bool ReadNodeData(wxString& p_sNodeName);

	/**
	\brief The test which of the variables are presented as a negated variable

	\returns	TRUE, FALSE
	*/
	bool CheckVariableOfIsNegated();

	/**
	\brief The Test whether the variable is negated

	\param		name		variable to be check
	\returns	TRUE, FALSE
	*/
	bool IsNegated(const wxString& name);

	/**
	\brief The test whether the end of the file is reached

	\returns	TRUE, FALSE
	*/
	bool IsEOF();
};

#endif // __SP_FTREE_TERMPARSER_H__
