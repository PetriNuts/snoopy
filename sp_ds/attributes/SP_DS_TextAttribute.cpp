//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


SP_DS_TextAttribute::SP_DS_TextAttribute(const wxString& p_pchName,
                                         const wxString& p_pchVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_TEXT),
m_sValue(p_pchVal)
{
}

SP_DS_Attribute*
SP_DS_TextAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_TextAttribute* l_pcAttr = new SP_DS_TextAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_TextAttribute::SetValue(const wxString& p_pchVal)
{
    wxString l_sTemp(p_pchVal);

    // special, if the param contains '*' as wildcard, we replace every occurance
    // of '*' with the content of the old member value
    l_sTemp.Replace(SP_WILDCARD, m_sValue);

    // hack to make empty lines (only newline) show up in the canvas
    // l_sTemp.Replace(wxT("\n\n"), wxT("\n \n"));

    m_sValue = l_sTemp;

    return TRUE;
}
