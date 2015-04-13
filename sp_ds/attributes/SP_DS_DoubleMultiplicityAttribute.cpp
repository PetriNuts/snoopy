//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/07 13:05:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_DoubleMultiplicityAttribute.h"
#include "sp_core/base/SP_ElementClass.h"

SP_DS_DoubleMultiplicityAttribute::SP_DS_DoubleMultiplicityAttribute(const wxString& p_pchName)
:SP_DS_DoubleAttribute(p_pchName, 1)
{
  SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMULTIPLICITY);
}

SP_DS_Attribute*
SP_DS_DoubleMultiplicityAttribute::Clone(bool p_bCloneGr)
{
  SP_DS_DoubleMultiplicityAttribute* l_pcAttr = new SP_DS_DoubleMultiplicityAttribute(GetName());
  l_pcAttr->SetValue(m_nValue);

  // just clone the graphic stuff, too
  return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_DoubleMultiplicityAttribute::SetValue(long double p_nVal)
{
  if (p_nVal > 0)
    m_nValue = p_nVal;

  return TRUE;
}
