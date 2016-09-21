//////////////////////////////////////////////////////////////////////
// $Source: kwinder$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/07 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/FTree/SP_FTree_TermScanner.h"
#include "sp_utilities.h"



SP_FTree_TermScanner::SP_FTree_TermScanner():

m_nLastToken(-1),
m_nLastChar(-1),
m_nNoLastToken(true)
{
	pos = 0;
}


SP_FTree_TermScanner::~SP_FTree_TermScanner()
{

}

bool
SP_FTree_TermScanner::Open(const wxString& p_sFile)
{
	m_fFile.Open(p_sFile.c_str());
	if(m_fFile.IsOpened()){
		return true;
	}
	return false;
}

bool
SP_FTree_TermScanner::Close()
{
	if(m_fFile.IsOpened()){
		m_fFile.Close();
		return true;
	}
	return false;
}
//Returns next token in the file
int
SP_FTree_TermScanner::Read()
{
	if(m_fFile.IsOpened()){
		wxChar* c = new wxChar[1];
		if(m_fFile.Read( c, 1) > 0){
			pos++;
			return 	(*c);
		}
	}
	return -1;
}
//Returns determined text
wxString
SP_FTree_TermScanner:: GetText()
{
	return m_sText;
}
//Returns actual position in the line
long
SP_FTree_TermScanner::GetPos()
{
	return pos;
}
//Set actual postition in the line
void
SP_FTree_TermScanner::SetPos(long newPos)
{
	pos = newPos;
}

//Read the next token in the file
int
SP_FTree_TermScanner:: GetNextToken()
{

	if (m_nLastToken != m_nNoLastToken) {
      int dummy = m_nLastToken;
      m_nLastToken = m_nNoLastToken;
      return dummy;
    }

	int l_nNextChar;

    if (m_nLastChar == -1 || IsWhitespace((wxChar)m_nLastChar)) {
      // ignore whitespaces
		while ((l_nNextChar = Read()) >= 0 && IsWhitespace((wxChar)l_nNextChar)){

		}
    } else {
      l_nNextChar = m_nLastChar;
      m_nLastChar = -1;
    }

    switch (l_nNextChar) {
      case -1:
        return SP_SCANNER_EOF;

      case ':':
        return SP_SCANNER_COLON;

      default:

        m_sText = wxT("");

        if (IsOperationOrIdentifier((wxChar)l_nNextChar)) {
          m_sText = m_sText+(wxChar)l_nNextChar;
        }
		//reads the tokens up to the next blanks
        while ((l_nNextChar = Read()) >= 0 &&
			IsOperationOrIdentifier((wxChar)l_nNextChar)) {
          m_sText = m_sText+(wxChar)l_nNextChar;
        }
        if (l_nNextChar >= 0) {
          m_nLastChar = l_nNextChar;
        }

        if (m_sText.length() == 0) {
          return SP_SCANNER_UNDEFINED;
        }
		return SP_SCANNER_TEXT;
    }
}

bool
SP_FTree_TermScanner::IsWhitespace(wxChar p_cCharacter ){
	switch(p_cCharacter) {

		case ' ': return true;
		case '\t': return true;
		case '\n': pos = 0;
					return true;
		case '\r':
			return true;
		default:
			return false;
	}
}

bool
SP_FTree_TermScanner::IsDigit(wxChar p_cCharacter ){
	switch(p_cCharacter) {

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
		default:
			return false;
	}
}

bool
SP_FTree_TermScanner::IsLetter(wxChar p_cCharacter ){
	if((p_cCharacter > 64 && p_cCharacter < 91) ||
       (p_cCharacter > 96 && p_cCharacter < 123))
	{
			return true;
	}else{
			return false;
	}
}


bool
SP_FTree_TermScanner::IsOperationOrIdentifier(wxChar p_cCharacter) {
    return IsLetter(p_cCharacter)
			|| p_cCharacter == '+'
			|| p_cCharacter == '-'
            || p_cCharacter == '*'
            || p_cCharacter == '_'
            || IsDigit(p_cCharacter);
}

