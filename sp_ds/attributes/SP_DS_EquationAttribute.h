//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/03 11:30:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EQUATIONATTRIBUTE_H__
#define __SP_DS_EQUATIONATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_EquationAttribute: public SP_DS_Attribute
{
private:
protected:
    wxString m_sValue;
public:
    SP_DS_EquationAttribute(const wxString& p_pchName, const wxString& p_pchVal = wxT(""));
    virtual ~SP_DS_EquationAttribute() { }

    virtual bool SetValue(const wxString& p_pchVal);
    virtual wxString GetValue() { return m_sValue; }

    virtual wxString GetValueString() { return GetValue(); }
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true) { return SetValue(p_pchVal); }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_EQUATIONATTRIBUTE_H__
