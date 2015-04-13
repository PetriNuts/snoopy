//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONPARSEROPERATOR_H__
#define __SP_DS_EQUATIONPARSEROPERATOR_H__

#include "sp_ds/extensions/continuous/SP_DS_EquationParserObject.h"

class SP_DS_EquationParserOperator: public SP_DS_EquationParserObject
{
private:
  SP_DS_EQUATIONPARSEROPERATOR_TYPE m_OpType;
  SP_DS_EquationParserObject* m_pcLeft;
  SP_DS_EquationParserObject* m_pcRight;
protected:
public:
  SP_DS_EquationParserOperator(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType = SP_DS_EQUATIONPARSEROPERATOR_NONE, 
                               SP_DS_EquationParserObject* p_pcLeft = NULL, 
                               SP_DS_EquationParserObject* p_pcRight = NULL);
  virtual ~SP_DS_EquationParserOperator();

  SP_DS_EquationParserOperator* Clone(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType = SP_DS_EQUATIONPARSEROPERATOR_NONE, 
                                      SP_DS_EquationParserObject* p_pcLeft = NULL, 
                                      SP_DS_EquationParserObject* p_pcRight = NULL);

  SP_DS_EQUATIONPARSEROPERATOR_TYPE GetOpType();
  SP_DS_EquationParserObject* GetLeft();
  SP_DS_EquationParserObject* GetRight();
  bool SetOpType(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType);
  bool SetLeft(SP_DS_EquationParserObject* p_pcLeft);
  bool SetRight(SP_DS_EquationParserObject* p_pcRight);
  virtual const wxString ToString();
  virtual const wxString ToString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType, 
                               bool p_bRight = FALSE);
  virtual const wxString ToTexString(bool p_bGreek = FALSE, 
                                  bool p_bParentheses = FALSE,
								  bool p_bConvSpecialChar = FALSE);
  virtual const wxString ToTexString(SP_DS_EQUATIONPARSEROPERATOR_TYPE p_OpType, 
                                  bool p_bRight = FALSE, 
                                  bool p_bGreek = FALSE, 
                                  bool p_bParentheses = FALSE,
								  bool p_bConvSpecialChar = FALSE);

  virtual long double Calculate();
  virtual long double Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable);
//  virtual long double Calculate(list<SP_DS_EquationParserObject*> p_pcVariable);

};

#endif // __SP_DS_EQUATIONPARSEROPERATOR_H__


