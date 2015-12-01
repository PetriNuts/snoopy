//////////////////////////////////////////////////////////////////////
// $Source: kwinder$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/07 11:55:00 $
// Short Description: scan logical expression
//
//////////////////////////////////////////////////////////////////////
#ifndef __SP_FTREE_TERMSCANNER_H__
#define __SP_FTREE_TERMSCANNER_H__

#include "sp_core/base/SP_Error.h"

#include <wx/ffile.h>

using namespace std;

enum SP_BOOL_SCANNER
{
	SP_SCANNER_EOF = 110,
	SP_SCANNER_TEXT = 150,
	SP_SCANNER_UNDEFINED = 160,
	SP_SCANNER_COLON = 170,
	SP_SCANNER_NORMAL = 220,
	SP_SCANNER_PLUS = 240,
	SP_SCANNER_MINUS = 250

};


class SP_FTree_TermScanner: public SP_Error
{
private:
	wxString m_sText;

	int m_nLastToken;
	int m_nLastChar;
	int m_nNoLastToken;

	//file to be read
	wxFFile m_fFile;

	//position in a line
	long pos;

	/**
	\brief Read next token in the file

	\returns	c	next token
	*/
	int Read();

protected:
public:
    SP_FTree_TermScanner();
    virtual ~SP_FTree_TermScanner();


	/**
	\brief The test whether the file is open

	\param		p_sFile		file to open
	\returns	TRUE, FALSE
	*/
	bool Open(const wxString& p_sFile);

	/**
	\brief The test whether the file is closed

	\returns	TRUE, FALSE
	*/
	bool Close();

	/**
	\brief Returns determined text

	\returns	m_sText		read token
	*/
	wxString GetText();

	/**
	\brief Returns next token in the line

	\returns	enum SP_BOOL_SCANNER	type of next token
	*/
	int GetNextToken();

	/**
	\brief Returns the position in a line

	\returns	pos		position in the line
	*/
	long GetPos();

	/**
	\brief Set the position in a line

	\param newPos	actual position in the line
	*/
	void SetPos(long newPos);

	/**
	\brief The test whether the token is a white space

	\returns	TRUE, FALSE
	*/
	bool IsWhitespace(wxChar p_cCharacter );

	/**
	\brief The test whether the token is a digit

	\returns	TRUE, FALSE
	*/
	bool IsDigit(wxChar p_cCharacter );

	/**
	\brief The test whether the token is an operation or an identifier

	\returns	TRUE, FALSE
	*/
	bool IsOperationOrIdentifier(wxChar p_cCharacter);

	/**
	\brief The test whether the token is a letter

	\returns	TRUE, FALSE
	*/
	bool IsLetter(wxChar p_cCharacter );


};

#endif // __SP_FTREE_TERMSCANNER_H__
