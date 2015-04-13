//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:05:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/continuous/SP_DS_EquationParser.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

SP_DS_EquationParser::SP_DS_EquationParser(SP_DS_Node* p_pcNode,
		const wxString& p_sEquation):
	m_TokenClass(SP_DS_EQUATIONPARSER_TYPE_EOSTREAM), m_sTokenValue(wxT("")),
			m_nInputPos(0), m_pcNode(p_pcNode), m_bParseError(FALSE),
			m_sEquation(p_sEquation), m_pcTop(NULL)
{
	m_pcTop = Parse();
	m_pcVariableList.clear();
	GetVariables(m_pcTop);
}

SP_DS_EquationParser::~SP_DS_EquationParser()
{
	m_pcVariableList.clear();
	if (m_pcTop)
	{
		wxDELETE(m_pcTop);
	}
	m_pcTop = NULL;
}

SP_DS_EquationParser*
SP_DS_EquationParser::Clone(SP_DS_Node* p_pcNode, const wxString& p_sEquation)
{
	return new SP_DS_EquationParser(p_pcNode, p_sEquation);
}

list<SP_DS_EquationParserVariable*> SP_DS_EquationParser::GetVariables()
{
	return m_pcVariableList;
}

void SP_DS_EquationParser::GetVariables(SP_DS_EquationParserObject* p_pcCurr)
{
	if (!p_pcCurr)
	{
		return;
	}
	if (SP_DS_EQUATIONPARSEROBJECT_VARIABLE == p_pcCurr->GetType())
	{
		SP_DS_EquationParserVariable* l_pcCurr =
				dynamic_cast<SP_DS_EquationParserVariable*> (p_pcCurr);
		m_pcVariableList.push_back(l_pcCurr);
	}
	else if (SP_DS_EQUATIONPARSEROBJECT_OPERATOR == p_pcCurr->GetType())
	{
		SP_DS_EquationParserOperator* l_pcCurr =
				dynamic_cast<SP_DS_EquationParserOperator*> (p_pcCurr);
		GetVariables(l_pcCurr->GetLeft());
		GetVariables(l_pcCurr->GetRight());
	}
	else if (SP_DS_EQUATIONPARSEROBJECT_CONSTANT == p_pcCurr->GetType())
	{
	}
	else if (SP_DS_EQUATIONPARSEROBJECT_NONE == p_pcCurr->GetType())
	{
	}
	return;
}

long double SP_DS_EquationParser::Calculate()
{
	CHECK_POINTER(m_pcTop, return 0.0);
	long double l_nResult = m_pcTop->Calculate();
	return l_nResult;
}

long double SP_DS_EquationParser::Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable)
{
	CHECK_POINTER(m_pcTop, return 0.0);
	long double l_nResult = m_pcTop->Calculate(p_pcVariable);
	return l_nResult;
}

/*
 *  old version
 */
/*
 long double
 SP_DS_EquationParser::Calculate(list<SP_DS_EquationParserObject*> p_pcVariable)
 {
 CHECK_POINTER(m_pcTop, return 0.0);
 long double l_nResult = m_pcTop->Calculate(p_pcVariable);
 return l_nResult;
 }
 */

const wxString SP_DS_EquationParser::ToString()
{
	CHECK_POINTER(m_pcTop, return wxT(""));
	return m_pcTop->ToString();
}

const wxString SP_DS_EquationParser::ToTexString(bool p_bGreek,
		bool p_bParentheses, bool p_bConvSpecialChar)
{
	CHECK_POINTER(m_pcTop, return wxT(""));
	return m_pcTop->ToTexString(p_bGreek, p_bParentheses, p_bConvSpecialChar);
}

bool SP_DS_EquationParser::GetParseError()
{
	CHECK_POINTER(m_pcTop, return TRUE);
	return m_bParseError;
}

/**
 * ===========================================================================
 *
 * Parsing Section
 *
 * ===========================================================================
 */
void SP_DS_EquationParser::NextToken()
{
	wxString l_sTemp = wxT("");
	m_sTokenValue = wxT("");
	m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_NONE;

	// define constants for operator
	const wxChar OPENPAR_OP = '(';
	const wxChar CLOSEPAR_OP = ')';
	const wxChar PLUS_OP = '+';
	const wxChar MINUS_OP = '-';
	const wxChar MULT_OP = '*';
	const wxChar DIV_OP = '/';
	const wxChar EXP_OP = '^';

	if (0 <= m_nInputPos)
	{
		wxChar l_chTemp = m_sEquation.at(m_nInputPos);
		while ((0 <= m_nInputPos) && ((' ' == l_chTemp) || ('\n' == l_chTemp)
				|| ('\t' == l_chTemp)))
		{
			// blanks, new lines and tabs read over
			m_nInputPos--;
			if (0 <= m_nInputPos)
			{
				l_chTemp = m_sEquation.at(m_nInputPos);
			} // if
		} // while (' ' || '\n' || '\t')
		switch (l_chTemp)
		{
		case PLUS_OP: // PLUS operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_PLUS;
			m_sTokenValue = PLUS_OP;
			m_nInputPos--;
			break;
		case MINUS_OP: // MINUS operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_MINUS;
			m_sTokenValue = PLUS_OP;
			m_nInputPos--;
			break;
		case MULT_OP: // MULT operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_MULT;
			m_sTokenValue = MULT_OP;
			m_nInputPos--;
			break;
		case DIV_OP: // DIV operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_DIV;
			m_sTokenValue = DIV_OP;
			m_nInputPos--;
			break;
		case EXP_OP: // EXP operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_EXP;
			m_sTokenValue = EXP_OP;
			m_nInputPos--;
			break;
		case OPENPAR_OP: // OPENPAR operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_OPENPAR;
			m_nInputPos--;
			break;
		case CLOSEPAR_OP: // CLOSEPAR operator
			m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_CLOSEPAR;
			m_nInputPos--;
			break;
		default:
			if (('_' == l_chTemp) || ('.' == l_chTemp) || (':' == l_chTemp)
					|| ('|' == l_chTemp) || ('$' == l_chTemp) || ('#'
					== l_chTemp) || ('@' == l_chTemp) || ('?' == l_chTemp)
					|| ('!' == l_chTemp) || (('0' <= l_chTemp) && ('9'
					>= l_chTemp)) || (('a' <= l_chTemp) && ('z' >= l_chTemp))
					|| (('A' <= l_chTemp) && ('Z' >= l_chTemp)))
			{
				if (('.' == l_chTemp) || (('0' <= l_chTemp)
						&& ('9' >= l_chTemp)))
				{
					m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_NUMBER;
				}
				else
				{
					m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_IDENT;
				}
				m_sTokenValue = l_chTemp;
				m_nInputPos--;
				if (0 <= m_nInputPos)
				{
					l_chTemp = m_sEquation.at(m_nInputPos);
					while (('_' == l_chTemp) || ('.' == l_chTemp) || (':'
							== l_chTemp) || ('|' == l_chTemp) || ('$'
							== l_chTemp) || ('#' == l_chTemp) || ('@'
							== l_chTemp) || ('?' == l_chTemp) || ('!'
							== l_chTemp) || (('a' <= l_chTemp) && ('z'
							>= l_chTemp)) || (('A' <= l_chTemp) && ('Z'
							>= l_chTemp)) || (('0' <= l_chTemp) && ('9'
							>= l_chTemp)))
					{
						if (('.' != l_chTemp) && (('0' > l_chTemp) || ('9'
								< l_chTemp)))
						{
							m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_IDENT;
						}
						m_sTokenValue = l_chTemp + m_sTokenValue;
						m_nInputPos--;
						if (0 <= m_nInputPos)
						{
							l_chTemp = m_sEquation.at(m_nInputPos);
						}
						else // if (m_nInputPos < m_sEquation.size())
						{
							l_chTemp = ' ';
						} // if-else (0 <= m_nInputPos)
					} // while
				} // if (0 <= m_nInputPos)

			}
			else // if (ident)
			{
				if (0 <= m_nInputPos)
				{
					wxString l_sError = wxT("");
					l_sError << wxT("Parse error in ")
							<< dynamic_cast<SP_DS_NameAttribute*>(m_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue()
							<< wxT("; Faulty Sign ")
							<< l_chTemp << wxT(" at position ") << m_nInputPos + 1
							<< wxT(" in equation\n") << m_sEquation << wxT("\ndetected.");
					SP_LOGDEBUG(wxT("SP_DS_EquationParser::NextToken(): ") + l_sError);
					SP_MESSAGEBOX(l_sError, wxT("Parse error"), wxOK | wxICON_ERROR);
					m_bParseError = TRUE;
				}
				else
				{
					m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_EOSTREAM;
				}
			} // if-else (ident)
			break;
		} // switch
	}
	else // if (0 <= m_nInputPos)
	{
		m_TokenClass = SP_DS_EQUATIONPARSER_TYPE_EOSTREAM;
	} // if-else (0 <= m_nInputPos)

}

SP_DS_EquationParserObject*
SP_DS_EquationParser::Parse()
{
	m_nInputPos = m_sEquation.size() - 1;
	NextToken();
	SP_DS_EquationParserObject* l_pcResult = Expression();
	if (l_pcResult)
	{
		NextToken();
		if (SP_DS_EQUATIONPARSER_TYPE_EOSTREAM != m_TokenClass)
		{
			wxString l_sError = wxT("");
			l_sError << wxT("Parse error in ")
					<< dynamic_cast<SP_DS_NameAttribute*>(m_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue()
					<< wxT("; Equation to long.");
			SP_LOGDEBUG(wxT("SP_DS_EquationParser::Parse(): ") + l_sError);
			SP_MESSAGEBOX(l_sError, wxT("Parse error"), wxOK | wxICON_ERROR);
			m_bParseError = TRUE;
		}
	}
	else
	{
		wxString l_sError = wxT("");
		wxChar l_chTemp = ' ';
		if (-1 <= m_nInputPos)
		{
			l_chTemp = m_sEquation.at(m_nInputPos + 1);
		}
		l_sError << wxT("Parse error in ")
				<< dynamic_cast<SP_DS_NameAttribute*>(m_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue()
				<< wxT("; Unexpected sign ") << l_chTemp << wxT(" at position ")
				<< m_nInputPos + 2 << wxT(" in equation\n") << m_sEquation
				<< wxT("\nfound.");
		SP_LOGDEBUG(wxT("SP_DS_EquationParser::Parse(): ") + l_sError);
		SP_MESSAGEBOX(l_sError, wxT("Parse error"), wxOK | wxICON_ERROR);
		m_bParseError = TRUE;
	}
	return l_pcResult;
}

SP_DS_EquationParserObject*
SP_DS_EquationParser::Expression()
{
	SP_DS_EquationParserObject* l_pcResult = NULL;
	SP_DS_EquationParserObject* l_pcTRight = Term();
	if (l_pcTRight)
	{
		SP_DS_EQUATIONPARSEROPERATOR_TYPE l_OpType = AddOperator();
		if (SP_DS_EQUATIONPARSEROPERATOR_NONE != l_OpType)
		{
			NextToken();
			SP_DS_EquationParserObject* l_pcTLeft = Expression();
			if (l_pcTLeft)
			{
				l_pcResult
						= new SP_DS_EquationParserOperator(l_OpType, l_pcTLeft, l_pcTRight);
			}
			else
			{
				l_pcResult
						= new SP_DS_EquationParserOperator(l_OpType, l_pcTRight, NULL);
			}
		}
		else
		{
			l_pcResult = l_pcTRight;
		} // if-else
	}
	return l_pcResult;
}

SP_DS_EquationParserObject*
SP_DS_EquationParser::Term()
{
	SP_DS_EquationParserObject* l_pcResult = NULL;
	SP_DS_EquationParserObject* l_pcTRight = Exponent();
	if (l_pcTRight)
	{
		SP_DS_EQUATIONPARSEROPERATOR_TYPE l_OpType = MulOperator();
		if (SP_DS_EQUATIONPARSEROPERATOR_NONE != l_OpType)
		{
			NextToken();
			SP_DS_EquationParserObject* l_pcTLeft = Term();
			if (l_pcTLeft)
			{
				l_pcResult
						= new SP_DS_EquationParserOperator(l_OpType, l_pcTLeft, l_pcTRight);
			}
		}
		else
		{
			l_pcResult = l_pcTRight;
		} // if-else
	}
	return l_pcResult;
}

SP_DS_EquationParserObject*
SP_DS_EquationParser::Exponent()
{
	SP_DS_EquationParserObject* l_pcResult = NULL;
	SP_DS_EquationParserObject* l_pcTRight = Factor();
	if (l_pcTRight)
	{
		SP_DS_EQUATIONPARSEROPERATOR_TYPE l_OpType = ExpOperator();
		if (SP_DS_EQUATIONPARSEROPERATOR_NONE != l_OpType)
		{
			NextToken();
			SP_DS_EquationParserObject* l_pcTLeft = Exponent();
			if (l_pcTLeft)
			{
				l_pcResult
						= new SP_DS_EquationParserOperator(l_OpType, l_pcTLeft, l_pcTRight);
			}
		}
		else
		{
			l_pcResult = l_pcTRight;
		} // if-else
	}
	return l_pcResult;
}

SP_DS_EquationParserObject*
SP_DS_EquationParser::Factor()
{
	SP_DS_EquationParserObject* l_pcResult = NULL;
	if (SP_DS_EQUATIONPARSER_TYPE_CLOSEPAR == m_TokenClass)
	{
		NextToken();
		SP_DS_EquationParserObject* l_pcTRight = Expression();
		if (SP_DS_EQUATIONPARSER_TYPE_OPENPAR == m_TokenClass)
		{
			l_pcResult = l_pcTRight;
			NextToken();
		}
	}
	else if (SP_DS_EQUATIONPARSER_TYPE_NUMBER == m_TokenClass)
	{
		double l_dValue;
		if (m_sTokenValue.ToDouble(&l_dValue))
		{
			l_pcResult = new SP_DS_EquationParserConstant(l_dValue);
		}
		else
		{
			l_pcResult = new SP_DS_EquationParserConstant(0.0);
		}
		NextToken();
	}
	else if (SP_DS_EQUATIONPARSER_TYPE_IDENT == m_TokenClass)
	{
		l_pcResult = new SP_DS_EquationParserVariable(m_sTokenValue);
		NextToken();
	} // if-else
	return l_pcResult;
}

SP_DS_EQUATIONPARSEROPERATOR_TYPE SP_DS_EquationParser::AddOperator()
{
	SP_DS_EQUATIONPARSEROPERATOR_TYPE l_pcResult =
			SP_DS_EQUATIONPARSEROPERATOR_NONE;
	if (SP_DS_EQUATIONPARSER_TYPE_PLUS == m_TokenClass)
	{
		l_pcResult = SP_DS_EQUATIONPARSEROPERATOR_PLUS;
	}
	else if (SP_DS_EQUATIONPARSER_TYPE_MINUS == m_TokenClass)
	{
		l_pcResult = SP_DS_EQUATIONPARSEROPERATOR_MINUS;
	}
	return l_pcResult;
}

SP_DS_EQUATIONPARSEROPERATOR_TYPE SP_DS_EquationParser::MulOperator()
{
	SP_DS_EQUATIONPARSEROPERATOR_TYPE l_pcResult =
			SP_DS_EQUATIONPARSEROPERATOR_NONE;
	if (SP_DS_EQUATIONPARSER_TYPE_MULT == m_TokenClass)
	{
		l_pcResult = SP_DS_EQUATIONPARSEROPERATOR_MULT;
	}
	else if (SP_DS_EQUATIONPARSER_TYPE_DIV == m_TokenClass)
	{
		l_pcResult = SP_DS_EQUATIONPARSEROPERATOR_DIV;
	}
	return l_pcResult;
}

SP_DS_EQUATIONPARSEROPERATOR_TYPE SP_DS_EquationParser::ExpOperator()
{
	SP_DS_EQUATIONPARSEROPERATOR_TYPE l_pcResult =
			SP_DS_EQUATIONPARSEROPERATOR_NONE;
	if (SP_DS_EQUATIONPARSER_TYPE_EXP == m_TokenClass)
	{
		l_pcResult = SP_DS_EQUATIONPARSEROPERATOR_EXP;
	}
	return l_pcResult;
}
