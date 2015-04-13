//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/04/05 13:23:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_DOUBLEATTRIBUTE_H__
#define __SP_DS_DOUBLEATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_DoubleAttribute: public SP_DS_Attribute
{
private:
protected:
    long double m_nValue;
public:
    SP_DS_DoubleAttribute(const wxString& p_pchName, long double p_nVal = 0.0);
    virtual ~SP_DS_DoubleAttribute() { }

    virtual bool SetValue(long double p_nVal) { m_nValue = p_nVal; return TRUE; }
    virtual long double GetValue() { return m_nValue; }

    virtual wxString GetValueString();
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

};


#endif // __SP_DS_DOUBLEATTRIBUTE_H__

