/*
 * SP_DS_TypeAttribute.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Steffen Laarz
 */
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"


SP_DS_TypeAttribute::SP_DS_TypeAttribute(const wxString& p_pchName, const wxString& p_sType):
	SP_DS_TextAttribute(p_pchName, p_sType)
{
	SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_TEXT);
	AddPossibleValue(p_sType);
}

SP_DS_Attribute*
SP_DS_TypeAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_TypeAttribute* l_pcAttr = new SP_DS_TypeAttribute(GetName(), GetValue());

    for (SP_SetString::iterator it = m_sTypes.begin(); it != m_sTypes.end(); it++)
    {
    	l_pcAttr->AddPossibleValue(*it);
    }


    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

/*
 * add new datatype
 */
void
SP_DS_TypeAttribute::AddPossibleValue(const wxString& p_pchVal)
{
	m_sTypes.insert(p_pchVal);
}

/*
 * set new datatype for constant
 * and check iff the datatype is allowed
 */
bool
SP_DS_TypeAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	SP_SetString::iterator it;
	it = m_sTypes.find(p_pchVal);

	if (it == m_sTypes.end())
	{
		SP_LOGERROR( wxT("datatype ") + p_pchVal + wxT(" not allowed"));
		return false;
	}

	return SetValue(p_pchVal);
}
