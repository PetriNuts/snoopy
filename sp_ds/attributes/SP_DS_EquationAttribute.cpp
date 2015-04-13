//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/03 11:30:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

SP_DS_EquationAttribute::SP_DS_EquationAttribute(const wxString& p_pchName,
                                                 const wxString& p_pchVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_EQUATION),
m_sValue(p_pchVal)
{
}

SP_DS_Attribute*
SP_DS_EquationAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_EquationAttribute* l_pcAttr = new SP_DS_EquationAttribute(GetName(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_EquationAttribute::SetValue(const wxString& p_pchVal)
{
    wxString l_sTemp(p_pchVal);

    // hack to make empty lines (only newline) show up in the canvas
    // l_sTemp.Replace(wxT("\n\n"), wxT("\n \n"));

    m_sValue = l_sTemp;

    return TRUE;
}
