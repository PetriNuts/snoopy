//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/08/11 15:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"

SP_DS_DoubleListAttribute::SP_DS_DoubleListAttribute(const wxString& p_pchName,
                                                     const wxString& p_sVal)
:SP_DS_ListAttribute(p_pchName, p_sVal)
{
  SetAttributeListType(SP_ATTRIBUTELIST_DOUBLE);
}

SP_DS_Attribute*
SP_DS_DoubleListAttribute::Clone(bool p_bCloneGr)
{
  SP_DS_DoubleListAttribute* l_pcAttr = new SP_DS_DoubleListAttribute(GetName(), GetValueString());

  // just clone the graphic stuff, too
  return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_DoubleListAttribute::AddValueDouble(long double p_nVal)
{
  double l_nValDouble = (double) p_nVal;
  wxString l_sVal;
  l_sVal.Printf(wxT("%.16G"), l_nValDouble);
  m_pcValues[m_nNumOfValues] = l_sVal;
  m_nNumOfValues++;
  return TRUE;
}

bool
SP_DS_DoubleListAttribute::SetValueDouble(long double p_nVal)
{
  if (m_nChoosenValueIndex < m_nNumOfValues)
  {
    return SetValueDouble(p_nVal, m_nChoosenValueIndex);
  } else
  {
    return SetValueDouble(p_nVal, 0);
  }
}

bool
SP_DS_DoubleListAttribute::SetValueDouble(long double p_nVal, long p_nIndex)
{
  if (p_nIndex < m_nNumOfValues)
  {
    double l_nValDouble = (double) p_nVal;
    wxString l_sVal;
    l_sVal.Printf(wxT("%.16G"), l_nValDouble);
    m_pcValues[p_nIndex] = l_sVal;
    return TRUE;
  } else
  {
    return FALSE;
  }
}

long double
SP_DS_DoubleListAttribute::GetValueDouble()
{
  if (m_nChoosenValueIndex < m_nNumOfValues)
  {
    return GetValueDouble(m_nChoosenValueIndex);
  } else if (0 < m_nNumOfValues)
  {
    return GetValueDouble(0);
  } else
  {
    return 0.0;
  }
}

long double
SP_DS_DoubleListAttribute::GetValueDouble(long p_nIndex)
{
  if (p_nIndex < m_nNumOfValues)
  {
    double l_nVal;
    wxString l_sVal = m_pcValues[p_nIndex];
    l_sVal.ToDouble(&l_nVal);
    return l_nVal;
  } else
  {
    return 0.0;
  }
}
