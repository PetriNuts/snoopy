//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Vector Reader, reads vectors of transitions
// 		or places
//////////////////////////////////////////////////////////////////////
#ifndef __SP_VECTORSCANNER_H__
#define __SP_VECTORSCANNER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Visitor.h"
#include "sp_core/base/SP_Error.h"

#include <wx/ffile.h>

using namespace std;

enum SP_VECTOR_SCANNER
{
	SP_SCANNER_EOF = 110,
	SP_SCANNER_AT = 120,
	SP_SCANNER_NUMBER = 130,
	SP_SCANNER_STAR = 140,
	SP_SCANNER_TEXT = 150,
	SP_SCANNER_UNDEFINED = 160,
	SP_SCANNER_COLON = 170,
	SP_SCANNER_COMMA = 180,
	SP_SCANNER_BAR = 190,
	SP_SCANNER_DOT = 200,
	SP_SCANNER_TEXT_MODUS = 210,
	SP_SCANNER_NORMAL = 220,
	SP_SCANNER_EQ = 230,
	SP_SCANNER_PLUS = 240

};


class SP_VectorScanner: public SP_Error
{
private:
	wxString m_sText;
	wxString m_sNumber;

	int m_nModus;
	int m_nLastToken;
	int m_nLastChar;
	int m_nNoLastToken;
	wxFFile m_fFile;
	wxString m_sFile;
	size_t m_nFilePos;

	bool Read(int& p_sToken);

protected:
public:
    SP_VectorScanner();
    virtual ~SP_VectorScanner();

	bool Open(const wxString& p_sFile);
	bool Close();

	bool GetNumber(long & p_nNumber);
	wxString GetText();

	int GetNextToken();

	void PushBackToken(int p_nToken);

	void SetModus(SP_VECTOR_SCANNER p_nModus);

	bool IsWhitespace(int p_cCharacter );
	bool IsDigit(int p_cCharacter );
	bool IsOperationOrIdentifier(int p_cCharacter);
	bool IsLetter(int p_cCharacter );

  wxString GetStringUntilFirst(int p_char);
};

#endif // __SP_VECTORSCANNER_H__
