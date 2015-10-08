//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/01 14:50:00 $
// Short Description: Multiplicity graphic implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_DoubleMultiplicityAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_ds/attributes/SP_DS_DoubleMultiplicityAttribute.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_DoubleMultiplicityAttribute::SP_GR_DoubleMultiplicityAttribute(SP_DS_Attribute* p_pcParent,
                                                                     const wxString& p_pchFormat,
                                                                     double p_nWidth,
                                                                     double p_nHeight)
:SP_GR_DoubleAttribute(p_pcParent, p_pchFormat, p_nWidth, p_nHeight)
{
}

SP_Graphic*
SP_GR_DoubleMultiplicityAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_DoubleMultiplicityAttribute* l_pcNew = new SP_GR_DoubleMultiplicityAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_DoubleMultiplicityAttribute::FormatText()
{
    if (!GetParent() ||
        (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE) ||
        (dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType() != SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE &&
        dynamic_cast<SP_DS_Attribute*>(GetParent())->GetAttributeType()!=SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMULTIPLICITY))
        return wxT("");

    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());
    double l_nVal = dynamic_cast<SP_DS_DoubleMultiplicityAttribute*>(l_pcAttr)->GetValue();
    wxString l_pchVal = l_pcAttr->GetValueString();

    if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (l_pchVal.Len() != 0) && ((1 < l_nVal) || (1 > l_nVal)));

    wxString l_sVal = m_sFormat;
    l_sVal.Replace(wxT("%"), SP_FormatWithComma(l_pchVal));

    return l_sVal;
}

bool
SP_GR_DoubleMultiplicityAttribute::Update(bool p_bLocalOnly)
{
    if (!m_pcPrimitive)
        return FALSE;

    wxString l_sVal = FormatText();
    m_pcPrimitive->AddText(l_sVal);
    m_pcPrimitive->CalculateDimensions(l_sVal);

    return TRUE;
}
