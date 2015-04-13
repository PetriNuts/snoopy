//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MULTIPLICITYATTRIBUTE_H__
#define __SP_DS_MULTIPLICITYATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_ExpressionAttribute.h"

class SP_DS_MultiplicityAttribute: public SP_DS_ExpressionAttribute
{
private:

protected:
	long m_nValue;					//for numeric value in animation

    virtual bool UpdateValue();

public:
    SP_DS_MultiplicityAttribute(const wxString& p_pchName, const wxString& p_pchVal);
    SP_DS_MultiplicityAttribute(const wxString& p_pchName, long p_nVal = 1);
    virtual ~SP_DS_MultiplicityAttribute() { }

    virtual bool SetValue(long l_nValue);
    virtual long GetValue(bool p_bUpdate = false);

    virtual wxString GetValueString();
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	virtual bool CheckIntegrity();

};


#endif // __SP_DS_MULTIPLICITYATTRIBUTE_H__

