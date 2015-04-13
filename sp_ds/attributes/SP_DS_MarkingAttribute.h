/*
 * SP_DS_MarkingAttribute.h
 *
 *  Created on: 05.12.2012
 *      Author: Steffen Laarz
 */

#ifndef SP_DS_MARKINGATTRIBUTE_H_
#define SP_DS_MARKINGATTRIBUTE_H_

#include "sp_ds/attributes/SP_DS_ExpressionAttribute.h"

class  SP_DS_MarkingAttribute: public SP_DS_ExpressionAttribute
{
private:
	 long m_nValue;					//for numeric value in animation

protected:

    virtual bool UpdateValue();

public:
    SP_DS_MarkingAttribute(const wxString& p_pchName, const wxString& p_pchVal);
    SP_DS_MarkingAttribute(const wxString& p_pchName, long p_nVal = 0);
    virtual ~SP_DS_MarkingAttribute() { }

    virtual bool SetValue(long l_nValue);
    virtual long GetValue(bool p_bUpdate = false);

    virtual wxString GetValueString();
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);


    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	virtual bool CheckIntegrity();

};


#endif /* SP_DS_MARKINGATTRIBUTE_H_ */
