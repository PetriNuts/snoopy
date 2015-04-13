//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/base/SP_ElementClass.h"

SP_DS_IdAttribute::SP_DS_IdAttribute(const wxString& p_pchName)
:SP_DS_NumberAttribute(p_pchName, NOT_SET_YET)
{
    SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
}

SP_DS_Attribute*
SP_DS_IdAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_IdAttribute* l_pcAttr = new SP_DS_IdAttribute(GetName());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_IdAttribute::SetValue(long p_nVal)
{
    if (GetParent() && GetParent()->GetClassObject())
            GetParent()->GetClassObject()->TestIdCount(p_nVal);

    m_nValue = p_nVal;
    return TRUE;
}

long
SP_DS_IdAttribute::GetValue()
{
    if (m_nValue == NOT_SET_YET)
    {
        if (GetParent() && GetParent()->GetClassObject())
            m_nValue = GetParent()->GetClassObject()->GetNewIdCount();
    }
    return m_nValue;
}

bool
SP_DS_IdAttribute::Squeeze()
{
    if (GetParent() && GetParent()->GetClassObject())
    {
        m_nValue = GetParent()->GetClassObject()->GetNewIdCount();
        return TRUE;
    }
    return FALSE;
}


