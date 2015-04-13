//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/continuous/SP_DS_EquationParserOperator.h"
#include <math.h>


SP_DS_EquationParserOperator::SP_DS_EquationParserOperator(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType,
                                                           SP_DS_EquationParserObject* p_pcLeft,
                                                           SP_DS_EquationParserObject* p_pcRight):
SP_DS_EquationParserObject(SP_DS_EQUATIONPARSEROBJECT_OPERATOR),
m_OpType(p_OpType),
m_pcLeft(p_pcLeft),
m_pcRight(p_pcRight)
{

}

SP_DS_EquationParserOperator::~SP_DS_EquationParserOperator()
{
  if (m_pcLeft)
  {
    wxDELETE(m_pcLeft);
  }
  m_pcLeft = NULL;
  if (m_pcRight)
  {
    wxDELETE(m_pcRight);
  }
  m_pcRight = NULL;
}

SP_DS_EquationParserOperator*
SP_DS_EquationParserOperator::Clone(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType,
                                    SP_DS_EquationParserObject* p_pcLeft,
                                    SP_DS_EquationParserObject* p_pcRight)
{
  return new SP_DS_EquationParserOperator(p_OpType, p_pcLeft, p_pcRight);
}

SP_DS_EQUATIONPARSEROPERATOR_TYPE
SP_DS_EquationParserOperator::GetOpType()
{
  return m_OpType;
}

SP_DS_EquationParserObject*
SP_DS_EquationParserOperator::GetLeft()
{
  return m_pcLeft;
}

SP_DS_EquationParserObject*
SP_DS_EquationParserOperator::GetRight()
{
  return m_pcRight;
}

bool
SP_DS_EquationParserOperator::SetOpType(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType)
{
  m_OpType = p_OpType;
  return TRUE;
}

bool
SP_DS_EquationParserOperator::SetLeft(SP_DS_EquationParserObject* p_pcLeft)
{
  CHECK_POINTER(p_pcLeft, return FALSE);
  m_pcLeft = p_pcLeft;
  return TRUE;
}

bool
SP_DS_EquationParserOperator::SetRight(SP_DS_EquationParserObject* p_pcRight)
{
  CHECK_POINTER(p_pcRight, return FALSE);
  m_pcRight = p_pcRight;
  return TRUE;
}

const wxString
SP_DS_EquationParserOperator::ToString()
{
  return ToString(m_OpType);
}

const wxString
SP_DS_EquationParserOperator::ToString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType,
                                       bool p_bRight)
{
  // define constants for operator
  const wxString OPENPAR_OP = wxT("(");
  const wxString CLOSEPAR_OP = wxT(")");
  const wxString PLUS_OP = wxT("+");
  const wxString MINUS_OP = wxT("-");
  const wxString MULT_OP = wxT("*");
  const wxString DIV_OP = wxT("/");
  const wxString EXP_OP = wxT("^");

  wxString result = wxT("");
  if (m_pcLeft)
  {
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if ((m_pcRight) &&
          (m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType) || (p_bRight))
         )
      {
        result.append(OPENPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if ((m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) || (p_bRight))
         )
      {
        result.append(OPENPAR_OP);
      }
      if (!m_pcRight)
      {
        result.append(MINUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType))
      {
        result.append(OPENPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if ((m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MULT != p_OpType) || (p_bRight))
         )
      {
        result.append(OPENPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      break;
    default:
      if (m_OpType != p_OpType)
      {
        result.append(OPENPAR_OP);
      }
      break;
    } // switch
    result.append(m_pcLeft->ToString(m_OpType));
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if (m_pcRight)
      {
        result.append(PLUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if (m_pcRight)
      {
        result.append(MINUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      result.append(MULT_OP);
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      result.append(DIV_OP);
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      result.append(EXP_OP);
      break;
    default:
      break;
    } // switch
    if (m_pcRight)
    {
      result.append(m_pcRight->ToString(m_OpType, TRUE));
    } // if (m_pcRight)
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if ((m_pcRight) &&
          (m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType) || (p_bRight))
         )
      {
        result.append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if ((m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) || (p_bRight))
         )
      {
        result.append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType))
      {
        result.append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if ((m_OpType != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MULT != p_OpType) || (p_bRight))
         )
      {
        result.append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      break;
    default:
      if (m_OpType != p_OpType)
      {
        result.append(CLOSEPAR_OP);
      }
      break;
    } // switch
  } // if (m_pcLeft)
  return result;
}


const wxString
SP_DS_EquationParserOperator::ToTexString(bool p_bGreek,
                                          bool p_bParentheses,
										  bool p_bConvSpecialChar)
{
  return ToTexString(m_OpType, FALSE, p_bGreek, p_bParentheses, p_bConvSpecialChar);
}

const wxString
SP_DS_EquationParserOperator::ToTexString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType,
                                          bool p_bRight,
                                          bool p_bGreek,
                                          bool p_bParentheses,
										  bool p_bConvSpecialChar)
{
  // define constants for operator
  const wxString OPENPAR_OP = wxT("\\left(");
  const wxString CLOSEPAR_OP = wxT("\\right)");
  const wxString PLUS_OP = wxT("+\n");
  const wxString MINUS_OP = wxT("-\n");
  const wxString MULT_OP = wxT("*");
  const wxString OPENDIV_OP = wxT("\\frac{");
  const wxString DIV_OP = wxT("}{");
  const wxString CLOSEDIV_OP = wxT("}");
  const wxString EXP_OP = wxT("^{");
  const wxString CLOSEEXP_OP = wxT("}");

  wxString result = wxT("");
  if (m_pcLeft)
  {
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if ((m_pcRight) &&
          (m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType) || (p_bRight))
         )
      {
        result.Append(OPENPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) || (p_bRight))
         )
      {
        result.Append(OPENPAR_OP);
      }
      if (!m_pcRight)
      {
        result.Append(MINUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType))
      {
        result.Append(OPENPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if (SP_DS_EQUATIONPARSEROPERATOR_EXP == p_OpType)
      {
        result.Append(OPENPAR_OP);
      }
      result.Append(OPENDIV_OP);
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      break;
    default:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType))
      {
        result.Append(OPENPAR_OP);
      }
      break;
    } // switch
    result.Append(m_pcLeft->ToTexString(m_OpType, FALSE, p_bGreek, p_bParentheses));
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if (m_pcRight)
      {
        result.Append(PLUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if (m_pcRight)
      {
        result.Append(MINUS_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      result.Append(MULT_OP);
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      result.Append(DIV_OP);
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      result.Append(EXP_OP);
      break;
    default:
      break;
    } // switch
    if (m_pcRight)
    {
      result.Append(m_pcRight->ToTexString(m_OpType, TRUE, p_bGreek, p_bParentheses));
    } // if (m_pcRight)
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if ((m_pcRight) &&
          (m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType) || (p_bRight))
         )
      {
        result.Append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          ((SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) || (p_bRight))
         )
      {
        result.Append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_PLUS != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_MINUS != p_OpType))
      {
        result.Append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      result.Append(CLOSEDIV_OP);
      if (SP_DS_EQUATIONPARSEROPERATOR_EXP == p_OpType)
      {
        result.Append(CLOSEPAR_OP);
      }
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      result.Append(CLOSEEXP_OP);
      break;
    default:
      if ((m_OpType != p_OpType) &&
          (SP_DS_EQUATIONPARSEROPERATOR_DIV != p_OpType))
      {
        result.Append(CLOSEPAR_OP);
      }
      break;
    } // switch
  } // if (m_pcLeft)
  return result;
}

long double
SP_DS_EquationParserOperator::Calculate()
{
  long double result = 0.0;
  if (m_pcLeft)
  {
    result = m_pcLeft->Calculate();
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if (m_pcRight)
      {
        result = result + m_pcRight->Calculate();
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if (m_pcRight)
      {
        result = result - m_pcRight->Calculate();
      } else {
        result = (-1) *  result;
      } // if-else (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if (m_pcRight)
      {
        result = result * m_pcRight->Calculate();
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if (m_pcRight)
      {
        result = result / m_pcRight->Calculate();
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      if (m_pcRight)
      {
        result = powl(result, m_pcRight->Calculate());
      } // if (m_pcRight)
      break;
    default:
      break;
    } // switch
  } // if (m_pcLeft)
  return result;
}

long double
SP_DS_EquationParserOperator::Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable)
{
  long double result = 0.0;
  if (m_pcLeft)
  {
    result = m_pcLeft->Calculate(p_pcVariable);
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if (m_pcRight)
      {
        result = result + m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if (m_pcRight)
      {
        result = result - m_pcRight->Calculate(p_pcVariable);
      } else {
        result = (-1) *  result;
      } // if-else (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if (m_pcRight)
      {
        result = result * m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if (m_pcRight)
      {
        result = result / m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      if (m_pcRight)
      {
        result = powl(result, m_pcRight->Calculate(p_pcVariable));
      } // if (m_pcRight)
      break;
    default:
      break;
    } // switch
  } // if (m_pcLeft)
  return result;
}

/*
 *  old version
 */
/*
long double
SP_DS_EquationParserOperator::Calculate(list<SP_DS_EquationParserObject*> p_pcVariable)
{
  long double result = 0.0;
  if (m_pcLeft)
  {
    result = m_pcLeft->Calculate(p_pcVariable);
    switch(m_OpType)
    {
    case SP_DS_EQUATIONPARSEROPERATOR_PLUS:
      if (m_pcRight)
      {
        result = result + m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MINUS:
      if (m_pcRight)
      {
        result = result - m_pcRight->Calculate(p_pcVariable);
      } else {
        result = (-1) *  result;
      } // if-else (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_MULT:
      if (m_pcRight)
      {
        result = result * m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_DIV:
      if (m_pcRight)
      {
        result = result / m_pcRight->Calculate(p_pcVariable);
      } // if (m_pcRight)
      break;
    case SP_DS_EQUATIONPARSEROPERATOR_EXP:
      if (m_pcRight)
      {
        result = powl(result, m_pcRight->Calculate(p_pcVariable));
      } // if (m_pcRight)
      break;
    default:
      break;
    } // switch
  } // if (m_pcLeft)
  return result;
}
 */
