//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONPARSERVARIABLE_H__
#define __SP_DS_EQUATIONPARSERVARIABLE_H__

#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParserObject.h"

class SP_DS_EquationParser;

class SP_DS_EquationParserVariable: public SP_DS_EquationParserObject
{
private:
  wxString m_sName;
  long m_nId;
  long double m_nValue;
  bool m_bConnected;
  bool m_bCalcState;
  SP_DS_Node* m_pcNode;
  SP_DS_EquationParser* m_pcEquationParser;
  SP_DS_EQUATIONPARSERVARIABLE_TYPE m_VarType;

  wxString FormatTexString(const wxString& p_sName, bool p_bConvSpecialChar);
  wxString ConvertGreek(const wxString& p_sName, bool p_bConvSpecialChar);
protected:
public:
  SP_DS_EquationParserVariable(const wxString& p_sName = wxT(""),
                               long p_nId = -1);
  virtual ~SP_DS_EquationParserVariable();

  SP_DS_EquationParserVariable* Clone(const wxString& p_sName = wxT(""),
                                      long p_nId = -1);

  const wxString GetName();
  long GetID();
  bool GetCalcState();
  void SetCalcState(bool p_bCalcState);
  bool GetConnected();
  void SetConnected(bool p_bConnected);
  long double GetValue();
  void SetValue(double p_nValue);

  SP_DS_Node* GetNode();
  void SetNode(SP_DS_Node* p_pcNode);
  SP_DS_EquationParser* GetEquationParser();
  void SetEquationParser(SP_DS_EquationParser* p_pcEquationParser);
  SP_DS_EQUATIONPARSERVARIABLE_TYPE GetVarType();
  void SetVarType(SP_DS_EQUATIONPARSERVARIABLE_TYPE p_VarType);


  virtual const wxString ToString();
  virtual const wxString ToTexString(bool p_bGreek = FALSE,
                                  bool p_bParentheses = FALSE,
								  bool p_bConvSpecialChar= FALSE);

  virtual long double Calculate();
  virtual long double Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable);
//  virtual long double Calculate(list<SP_DS_EquationParserObject*> p_pcVariable);

};

#endif // __SP_DS_EQUATIONPARSERVARIABLE_H__


