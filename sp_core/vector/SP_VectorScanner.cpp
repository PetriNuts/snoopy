//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/vector/SP_VectorScanner.h"
#include "sp_defines.h"

SP_VectorScanner::SP_VectorScanner() :

	m_nModus(SP_SCANNER_NORMAL), m_nLastToken(true), m_nLastChar(wxEOF),
			m_nNoLastToken(true), m_nFilePos(0)
{
}

SP_VectorScanner::~SP_VectorScanner()
{

}

bool SP_VectorScanner::Open(const wxString& p_sFile)
{
	m_fFile.Open(p_sFile);
	if (m_fFile.IsOpened())
	{
		m_fFile.ReadAll(&m_sFile);
		return true;
	}
	return false;
}

bool SP_VectorScanner::Close()
{
	if (m_fFile.IsOpened())
	{
		m_fFile.Close();
		m_sFile.Clear();
		m_nFilePos = 0;
		m_nLastToken = true;
		m_nLastChar = wxEOF;
		return true;
	}
	return false;
}

bool SP_VectorScanner::Read(int& p_sToken)
{
	if (m_fFile.IsOpened())
	{
		if(m_nFilePos < m_sFile.Len())
		{
			p_sToken = m_sFile[m_nFilePos];
			m_nFilePos++;
			return true;
		}
		else
		{
			p_sToken = wxEOF;
		}
	}
	return false;
}

bool SP_VectorScanner::GetNumber(long & p_nNumber)
{
	return m_sNumber.ToLong(&p_nNumber);
}

wxString SP_VectorScanner::GetText()
{
	return m_sText;
}

int SP_VectorScanner::GetNextToken()
{

	if (m_nLastToken != m_nNoLastToken)
	{
		int dummy = m_nLastToken;
		m_nLastToken = m_nNoLastToken;
		return dummy;
	}

	int l_nNextChar = 0;

	if (m_nLastChar == wxEOF || IsWhitespace(m_nLastChar))
	{
		// ignore whitespaces
		while (Read(l_nNextChar) && IsWhitespace(l_nNextChar))
			;
	}
	else
	{
		l_nNextChar = m_nLastChar;
		m_nLastChar = wxEOF;
	}

	switch (l_nNextChar)
	{
	case wxEOF:
		return SP_SCANNER_EOF;

	case wxT('-'):
	case wxT('0'):
	case wxT('1'):
	case wxT('2'):
	case wxT('3'):
	case wxT('4'):
	case wxT('5'):
	case wxT('6'):
	case wxT('7'):
	case wxT('8'):
	case wxT('9'):
		if (m_nModus == SP_SCANNER_TEXT_MODUS)
		{
			m_sText = wxT("");
			m_sText = m_sText + (wxChar)l_nNextChar;
			while (Read(l_nNextChar) && l_nNextChar != wxT('|'))
			{
				m_sText = m_sText + (wxChar)l_nNextChar;
			}
			if (l_nNextChar == wxT('|'))
			{
				m_nLastChar = l_nNextChar;
			}
			return SP_SCANNER_TEXT;
		}

		m_sNumber = wxT("");
		m_sNumber = m_sNumber + (wxChar)l_nNextChar;
		while (Read(l_nNextChar) && IsDigit(l_nNextChar))
		{
			m_sNumber = m_sNumber + (wxChar)l_nNextChar;
		}
		if (l_nNextChar >= 0)
		{
			m_nLastChar = l_nNextChar;
		}

		if (m_sNumber.length() == 0)
		{
			return SP_SCANNER_UNDEFINED;
		}
		return SP_SCANNER_NUMBER;

	case wxT('@'):
		return SP_SCANNER_AT;
	case wxT('*'):
		return SP_SCANNER_STAR;
	case wxT(':'):
		return SP_SCANNER_COLON;
	case wxT(','):
		return SP_SCANNER_COMMA;
	case wxT('='):
		return SP_SCANNER_EQ;
	case wxT('|'):
		return SP_SCANNER_BAR;
	case wxT('+'):
		return SP_SCANNER_PLUS;
	case wxT('.'):
		return SP_SCANNER_DOT;

	default:

		m_sText = wxT("");
		if (m_nModus == SP_SCANNER_TEXT_MODUS)
		{
			m_sText = m_sText + (wxChar)l_nNextChar;
			while (Read(l_nNextChar) && l_nNextChar != wxT('|'))
			{
				m_sText = m_sText + (wxChar)l_nNextChar;
			}
			if (l_nNextChar == wxT('|'))
			{
				m_nLastChar = l_nNextChar;
			}
			return SP_SCANNER_TEXT;
		}

		if (IsOperationOrIdentifier(l_nNextChar))
		{
			m_sText = m_sText + (wxChar)l_nNextChar;
		}
		while (Read(l_nNextChar) && IsOperationOrIdentifier(l_nNextChar))
		{
			m_sText = m_sText + (wxChar)l_nNextChar;
		}
		if (l_nNextChar >= 0)
		{
			m_nLastChar = l_nNextChar;
		}

		if (m_sText.length() == 0)
		{
			return SP_SCANNER_UNDEFINED;
		}

		return SP_SCANNER_TEXT;
	}

}

void SP_VectorScanner::PushBackToken(int p_nToken)
{
	m_nLastToken = p_nToken;
}

bool SP_VectorScanner::IsWhitespace(int p_cCharacter)
{
	return wxIsspace(p_cCharacter);
}

bool SP_VectorScanner::IsDigit(int p_cCharacter)
{
	return wxIsdigit(p_cCharacter);
}

bool SP_VectorScanner::IsLetter(int p_cCharacter)
{
	return wxIsalpha(p_cCharacter);
}

bool SP_VectorScanner::IsOperationOrIdentifier(int p_cCharacter)
{
	return IsLetter(p_cCharacter) ||
			p_cCharacter == wxT('+') ||
			p_cCharacter == wxT('-') ||
			p_cCharacter == wxT('*') ||
			p_cCharacter == wxT('/') ||
			p_cCharacter == wxT('<') ||
			p_cCharacter == wxT('>') ||
			p_cCharacter == wxT('!') ||
			p_cCharacter == wxT('&') ||
			p_cCharacter == wxT('_') ||
			p_cCharacter == wxT('(') ||
			p_cCharacter == wxT(')') ||
			IsDigit(p_cCharacter);
}

void SP_VectorScanner::SetModus(SP_VECTOR_SCANNER p_nModus)
{
	m_nModus = p_nModus;
}

wxString SP_VectorScanner::GetStringUntilFirst(int p_char)
{

	if (m_fFile.IsOpened())
	{
		wxString l_sText = wxT("");
		int l_read;
		while(Read(l_read))
		{
			if (l_read != wxT('='))
			{
				l_sText = wxString::Format(wxT("%s%c"), l_sText.c_str(), l_read);
			}
			else
			{
				l_sText = wxString::Format(wxT("%s%c\n\n"), l_sText.c_str(), l_read);
				break;
			}
		}
		m_nFilePos = 0;
		m_fFile.Seek(0);

		return l_sText;
	}
	else
	{
		return wxT("");
	}

	return wxT("transition invariants = \n\n");

}
