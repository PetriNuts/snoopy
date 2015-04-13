//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Steffen Laarz $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2011/12/08 12:37:00 $
// Short Description: to handle expression and constants for places
//////////////////////////////////////////////////////////////////////
#ifndef SP_DS_EXPRESSIONATTRIBUE_H_
#define SP_DS_EXPRESSIONATTRIBUE_H_

#include "sp_ds/SP_DS_Attribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"

class SP_DS_ExpressionAttribute: public SP_DS_Attribute
{
private:
protected:
	SP_FunctionPtr m_Function;
	wxString m_sValue;				//for arithmetic expressions
	wxString m_sValueStandard;		//to remember the standard value

	bool parseString(const wxString& p_sVal, SP_DS_Node* p_pcTrans);

	virtual bool UpdateValue() = 0;

public:
	SP_DS_ExpressionAttribute(const wxString& p_pchName, const wxString& p_pchVal,  SP_ATTRIBUTE_TYPE p_eType );
	virtual ~SP_DS_ExpressionAttribute() { }
};


#endif /* SP_DS_EXPRESSIONATTRIBUE_H_ */
