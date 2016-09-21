//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONPARSEROBJECT_H__
#define __SP_DS_EQUATIONPARSEROBJECT_H__

#include "sp_core/SP_Map.h"
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_utilities.h"


// Equation Object Types
enum SP_DS_EQUATIONPARSEROBJECT_TYPE
{
    SP_DS_EQUATIONPARSEROBJECT_NONE,     // 0
    SP_DS_EQUATIONPARSEROBJECT_VARIABLE, // 1
    SP_DS_EQUATIONPARSEROBJECT_CONSTANT, // 2
    SP_DS_EQUATIONPARSEROBJECT_OPERATOR // 3
};

// Equation Operator Types
enum SP_DS_EQUATIONPARSEROPERATOR_TYPE
{
    SP_DS_EQUATIONPARSEROPERATOR_NONE,       // 0
    SP_DS_EQUATIONPARSEROPERATOR_PLUS,  // + // 1
    SP_DS_EQUATIONPARSEROPERATOR_MINUS, // - // 2
    SP_DS_EQUATIONPARSEROPERATOR_MULT,  // * // 3
    SP_DS_EQUATIONPARSEROPERATOR_DIV,   // / // 4
    SP_DS_EQUATIONPARSEROPERATOR_EXP   // ^ // 5
};

// Equation Operator Types
enum SP_DS_EQUATIONPARSERVARIABLE_TYPE
{
    SP_DS_EQUATIONPARSERVARIABLE_NONE,      // 0
    SP_DS_EQUATIONPARSERVARIABLE_VARIABLE,  // 1
    SP_DS_EQUATIONPARSERVARIABLE_PARAMETER // 2
};

class SP_DS_EquationParserObject: public SP_Error
{
private:
  SP_DS_EQUATIONPARSEROBJECT_TYPE mType;
protected:
public:
  SP_DS_EquationParserObject(SP_DS_EQUATIONPARSEROBJECT_TYPE pType = SP_DS_EQUATIONPARSEROBJECT_NONE);
  virtual ~SP_DS_EquationParserObject();

  SP_DS_EquationParserObject* Clone(SP_DS_EQUATIONPARSEROBJECT_TYPE pType = SP_DS_EQUATIONPARSEROBJECT_NONE);

  SP_DS_EQUATIONPARSEROBJECT_TYPE GetType();
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
								  bool p_bConvSpecialChar =FALSE);

  virtual long double Calculate();
  virtual long double Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable);
//  virtual long double Calculate(list<SP_DS_EquationParserObject*> p_pcVariable);

};

#endif // __SP_DS_EQUATIONPARSEROBJECT_H__


