//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_BOOLATTRIBUTE_H__
#define __SP_DS_BOOLATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_BoolAttribute: public SP_DS_Attribute
{
private:
protected:
    bool m_bValue;
public:
    SP_DS_BoolAttribute(const wxString& p_pchName,
                       bool p_bVal = TRUE);
    virtual ~SP_DS_BoolAttribute() { }

    virtual bool SetValue(bool p_bVal) { m_bValue = p_bVal; return TRUE; }
    virtual bool GetValue() { return m_bValue; }

    virtual wxString GetValueString() { return (m_bValue?wxT("1"):wxT("0")); }
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true) { return SetValue(p_pchVal.IsSameAs(wxT("1"))); }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_BOOLATTRIBUTE_H__
