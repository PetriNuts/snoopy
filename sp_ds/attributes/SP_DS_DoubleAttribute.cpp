//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/04/05 13:23:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"

SP_DS_DoubleAttribute::SP_DS_DoubleAttribute(const wxString& p_pchName,
                                             long double p_nVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE),
m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_DoubleAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_DoubleAttribute* l_pcAttr = new SP_DS_DoubleAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

wxString
SP_DS_DoubleAttribute::GetValueString()
{
	long double l_nVal = GetValue();
    wxString l_sVal;
    l_sVal.Printf(wxT("%.16LG"), l_nVal);
    return l_sVal;
}

bool
SP_DS_DoubleAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
    double l_nVal;
    if (p_pchVal.ToDouble(&l_nVal))
    {
        return SetValue(l_nVal);
    }

    return FALSE;
}
