//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

SP_DS_NumberAttribute::SP_DS_NumberAttribute(const wxString& p_pchName,
                                             long p_nVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NUMBER), m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_NumberAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_NumberAttribute* l_pcAttr = new SP_DS_NumberAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

wxString
SP_DS_NumberAttribute::GetValueString()
{
    wxString l_sVal;
    l_sVal.Printf(wxT("%ld"), GetValue());
    return l_sVal;
}

bool
SP_DS_NumberAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
    long l_nVal;
    if (p_pchVal.ToLong(&l_nVal))
    {

        return SetValue(l_nVal);
    }
    return FALSE;
}

