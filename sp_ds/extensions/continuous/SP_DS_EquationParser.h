//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:05:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONPARSER_H__
#define __SP_DS_EQUATIONPARSER_H__

#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParserObject.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParserOperator.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParserVariable.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParserConstant.h"

enum SP_DS_EQUATIONPARSER_TYPES 
{
  SP_DS_EQUATIONPARSER_TYPE_NONE,     //  0
  SP_DS_EQUATIONPARSER_TYPE_EOSTREAM, //  1
  SP_DS_EQUATIONPARSER_TYPE_IDENT,    //  2
  SP_DS_EQUATIONPARSER_TYPE_NUMBER,   //  3
  SP_DS_EQUATIONPARSER_TYPE_OPENPAR,  //  4
  SP_DS_EQUATIONPARSER_TYPE_CLOSEPAR, //  5
  SP_DS_EQUATIONPARSER_TYPE_PLUS,     //  6
  SP_DS_EQUATIONPARSER_TYPE_MINUS,    //  7
  SP_DS_EQUATIONPARSER_TYPE_MULT,     //  8
  SP_DS_EQUATIONPARSER_TYPE_DIV,      //  9
  SP_DS_EQUATIONPARSER_TYPE_EXP      // 10
};

class SP_DS_EquationParser: public SP_Error
{
private:
  // variables and methods used to parse the Equation
  SP_DS_EQUATIONPARSER_TYPES m_TokenClass;
  wxString m_sTokenValue;
  int m_nInputPos;
  
  void NextToken();
  SP_DS_EquationParserObject* Parse();
  SP_DS_EquationParserObject* Expression();
  SP_DS_EquationParserObject* Term();
  SP_DS_EquationParserObject* Exponent();
  SP_DS_EquationParserObject* Factor();
  SP_DS_EQUATIONPARSEROPERATOR_TYPE  AddOperator();
  SP_DS_EQUATIONPARSEROPERATOR_TYPE  MulOperator();
  SP_DS_EQUATIONPARSEROPERATOR_TYPE  ExpOperator();
  const wxString UnaryOperator();

  // variables and methods used by other methods
  SP_DS_Node* m_pcNode;
  bool m_bParseError;
  wxString m_sEquation;
  SP_DS_EquationParserObject* m_pcTop;
  list<SP_DS_EquationParserVariable*> m_pcVariableList;
  void GetVariables(SP_DS_EquationParserObject* p_pcCurr);
protected:
public:
  SP_DS_EquationParser(SP_DS_Node* p_pcNode, const wxString& p_sEquation = wxT(""));
  virtual ~SP_DS_EquationParser();

  SP_DS_EquationParser* Clone(SP_DS_Node* p_pcNode, const wxString& p_sEquation = wxT(""));

  list<SP_DS_EquationParserVariable*> GetVariables();
  long double Calculate();
  long double Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable);
//  long double Calculate(list<SP_DS_EquationParserObject*> p_pcVariable);

  const wxString GetEquation() const { return m_sEquation; }
  const wxString ToString();
  const wxString ToTexString(bool p_bGreek = FALSE, 
                          bool p_bParentheses = FALSE,
						  bool p_bConvSpecialChar = FALSE);
  bool GetParseError();

};

#endif // __SP_DS_EQUATIONPARSER_H__


