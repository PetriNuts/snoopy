//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/16 15:35:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONPARSERCONSTANT_H__
#define __SP_DS_EQUATIONPARSERCONSTANT_H__

#include "sp_ds/extensions/continuous/SP_DS_EquationParserObject.h"

class SP_DS_EquationParserConstant: public SP_DS_EquationParserObject
{
private:
  long double m_dValue;
protected:
public:
  SP_DS_EquationParserConstant(long double p_dValue = 0.0);
  virtual ~SP_DS_EquationParserConstant();

  SP_DS_EquationParserConstant* Clone(long double p_dValue = 0.0);

  long double GetValue();
  virtual const wxString ToString();
  virtual const wxString ToTexString(bool p_bGreek = FALSE, 
                                  bool p_bParentheses = FALSE,
								  bool p_bConvSpecialChar = FALSE);

  virtual long double Calculate();
  virtual long double Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable);
//  virtual long double Calculate(list<SP_DS_EquationParserObject*> p_pcVariable);

};

#endif // __SP_DS_EQUATIONPARSERCONSTANT_H__


