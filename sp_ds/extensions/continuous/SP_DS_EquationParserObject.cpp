//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/continuous/SP_DS_EquationParserObject.h"

SP_DS_EquationParserObject::SP_DS_EquationParserObject(SP_DS_EQUATIONPARSEROBJECT_TYPE pType):
mType(pType)
{

}

SP_DS_EquationParserObject::~SP_DS_EquationParserObject()
{
}

SP_DS_EquationParserObject*
SP_DS_EquationParserObject::Clone(SP_DS_EQUATIONPARSEROBJECT_TYPE pType)
{
  return new SP_DS_EquationParserObject(pType);
}

SP_DS_EQUATIONPARSEROBJECT_TYPE 
SP_DS_EquationParserObject::GetType()
{ 
  return mType;
}

const wxString 
SP_DS_EquationParserObject::ToString()
{
  return wxT("");
}

const wxString 
SP_DS_EquationParserObject::ToString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType, 
                                     bool p_bRight)
{
  return ToString();
}

const wxString 
SP_DS_EquationParserObject::ToTexString(bool p_bGreek, 
                                        bool p_bParentheses,
										bool p_bConvSpecialChar)
{
  return wxT("");
}

const wxString 
SP_DS_EquationParserObject::ToTexString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType, 
                                        bool p_bRight, 
                                        bool p_bGreek, 
                                        bool p_bParentheses,
										bool p_bConvSpecialChar)
{
  return ToTexString(p_bGreek, p_bParentheses);
}

long double 
SP_DS_EquationParserObject::Calculate()
{
  return 0.0;
}

long double 
SP_DS_EquationParserObject::Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable)
{
  return 0.0;
}

/*
 *  old version
 */
/*
long double 
SP_DS_EquationParserObject::Calculate(list<SP_DS_EquationParserObject*> p_pcVariable)
{
  return 0.0;
}
 */
