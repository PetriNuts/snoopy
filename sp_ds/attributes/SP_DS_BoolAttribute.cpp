//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

SP_DS_BoolAttribute::SP_DS_BoolAttribute(const wxString& p_pchName,
                                         bool p_bVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL),
m_bValue(p_bVal)
{
}

SP_DS_Attribute*
SP_DS_BoolAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_BoolAttribute* l_pcAttr = new SP_DS_BoolAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}
