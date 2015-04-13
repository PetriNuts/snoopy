//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/16 15:35:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/continuous/SP_DS_EquationParserConstant.h"

SP_DS_EquationParserConstant::SP_DS_EquationParserConstant(long double p_dValue):
SP_DS_EquationParserObject(SP_DS_EQUATIONPARSEROBJECT_CONSTANT),
m_dValue(p_dValue)
{

}

SP_DS_EquationParserConstant::~SP_DS_EquationParserConstant()
{
}

SP_DS_EquationParserConstant*
SP_DS_EquationParserConstant::Clone(long double p_dValue)
{
  return new SP_DS_EquationParserConstant(p_dValue);
}

long double 
SP_DS_EquationParserConstant::GetValue() 
{
  return m_dValue;
}

const wxString 
SP_DS_EquationParserConstant::ToString()
{
  wxString result = wxT("");
  result.Printf(wxT("%.19G"), (double) m_dValue);
  return result;
}

const wxString 
SP_DS_EquationParserConstant::ToTexString(bool p_bGreek, 
                                          bool p_bParentheses, 
										  bool p_bConvSpecialChar)
{
  return ToString();
}

long double 
SP_DS_EquationParserConstant::Calculate()
{
  return m_dValue;
}

long double 
SP_DS_EquationParserConstant::Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable)
{
  return m_dValue;
}

/*
 *  old version
 */
/*
long double 
SP_DS_EquationParserConstant::Calculate(list<SP_DS_EquationParserObject*> p_pcVariable)
{
  return m_dValue;
}
 */
