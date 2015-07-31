//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Readr, writes graph structures actually
//////////////////////////////////////////////////////////////////////

#include "sp_core/vector/SP_VectorScanner.h"
#include "sp_core/vector/SP_VectorParser.h"

#include "sp_defines.h"

SP_VectorParser::SP_VectorParser(SP_VectorScanner& p_pcScanner) :
	m_pcScanner(p_pcScanner)
{
}

SP_VectorParser::~SP_VectorParser()
{
}

SP_VECTOR_TYPE SP_VectorParser::ReadVectorTypeInformation()
{
	int token = m_pcScanner.GetNextToken();
	SP_VECTOR_TYPE type = SP_NO_NODE_VECTOR;
	wxString trans = wxT("transition");
	wxString plac = wxT("place");
	while (token != SP_SCANNER_EOF)
	{

		if (m_pcScanner.GetText() == trans)
		{
			type = SP_TRANSITION_TYPE;
			break;
		}
		else if (m_pcScanner.GetText() == plac)
		{
			type = SP_PLACE_TYPE;
			break;
		}
		token = m_pcScanner.GetNextToken();

	}

	while (token != SP_SCANNER_EOF)
	{
		if (token == SP_SCANNER_EQ)
		{
			break;
		}
		token = m_pcScanner.GetNextToken();
	}

	return type;
}

bool SP_VectorParser::ReadNodeData(long& p_nNodeNumber, wxString& p_sNodeName,
		long& p_nVal)
{
	int l_nToken = m_pcScanner.GetNextToken();
	if (l_nToken == SP_SCANNER_COMMA || l_nToken == SP_SCANNER_BAR)
	{
		if (l_nToken == SP_SCANNER_COMMA)
		{
			l_nToken = m_pcScanner.GetNextToken();
			if (l_nToken != SP_SCANNER_BAR)
			{
				return FALSE;
			}
		}
		l_nToken = m_pcScanner.GetNextToken();
		if (l_nToken == SP_SCANNER_NUMBER)
		{
			m_pcScanner.GetNumber(p_nNodeNumber);
		}
		l_nToken = m_pcScanner.GetNextToken();
		if (l_nToken == SP_SCANNER_DOT)
		{
			if (!ReadNodeName(p_sNodeName))
			{
				return FALSE;
			}
		}
		else if (l_nToken == SP_SCANNER_COLON)
		{
			p_sNodeName = wxT("");
		}

		if (m_pcScanner.GetNextToken() == SP_SCANNER_NUMBER)
		{
			m_pcScanner.GetNumber(p_nVal);
		}
		return TRUE;
	}
	else if ((l_nToken == SP_SCANNER_NUMBER) || (l_nToken == SP_SCANNER_AT))
	{
		m_pcScanner.PushBackToken(l_nToken);
	}
	return FALSE;
}

bool SP_VectorParser::ReadNodeName(wxString& p_sName)
{
	p_sName = wxT("");
	int l_nToken = m_pcScanner.GetNextToken();
	while (l_nToken != SP_SCANNER_COLON)
	{
		if (l_nToken == SP_SCANNER_NUMBER)
		{
			long num;
			if (m_pcScanner.GetNumber(num))
			{
				p_sName = wxString::Format(wxT("%s%d"), p_sName.c_str(), num);
			}
		}
		else if (l_nToken == SP_SCANNER_TEXT)
		{
			wxString s = m_pcScanner.GetText();
			p_sName = wxString::Format(wxT("%s%s"), p_sName.c_str(), s.c_str() );

		}
		l_nToken = m_pcScanner.GetNextToken();
	}
	return true;
}

bool SP_VectorParser::ReadVectorInformation(wxString &p_sName, wxString& p_sText)
{
	m_pcScanner.SetModus(SP_SCANNER_TEXT_MODUS);
	int l_nToken;
	if (m_pcScanner.GetNextToken() == SP_SCANNER_BAR)
	{
		l_nToken = m_pcScanner.GetNextToken();
		p_sName = wxT("");
		if (l_nToken != SP_SCANNER_BAR)
		{
			p_sName = p_sName + m_pcScanner.GetText();
		}
		else
		{
			m_pcScanner.PushBackToken(SP_SCANNER_BAR);
		}

	}

	if (m_pcScanner.GetNextToken() == SP_SCANNER_BAR)
	{
		l_nToken = m_pcScanner.GetNextToken();
		p_sText = wxT("");
		if (l_nToken != SP_SCANNER_BAR)
		{
			p_sText = p_sText + m_pcScanner.GetText();
		}
		else
		{
			m_pcScanner.PushBackToken(SP_SCANNER_BAR);
		}
	}

	if ( ! (m_pcScanner.GetNextToken() == SP_SCANNER_BAR))
	{
		return FALSE;
	}
	m_pcScanner.SetModus(SP_SCANNER_NORMAL);
	return TRUE;
}

bool SP_VectorParser::ReadVectorNumber(long &p_nNumber)
{
	int l_nToken = m_pcScanner.GetNextToken();
	if (l_nToken == SP_SCANNER_NUMBER)
	{
		return m_pcScanner.GetNumber(p_nNumber);
	}
	return FALSE;
}

wxString SP_VectorParser::GetVectorSetInformation()
{
	return m_pcScanner.GetStringUntilFirst(wxT('='));
}
