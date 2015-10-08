//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Mark (PED marking) attribute implementation with only numbers
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_MarkNumberAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"

#include "sp_core/SP_Core.h"



//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MarkNumberAttribute::SP_GR_MarkNumberAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth,
        double p_nHeight)
:SP_GR_MarkAttribute(p_pcParent, p_nWidth, p_nHeight)

{

}

SP_Graphic*
SP_GR_MarkNumberAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_MarkNumberAttribute* l_pcNew = new SP_GR_MarkNumberAttribute(l_pcParent,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}


bool
SP_GR_MarkNumberAttribute::Update(bool p_bLocalOnly)
{
	CHECK_POINTER(m_pcPrimitive, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);
    CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());

	wxString l_sValue = wxEmptyString;
	if(l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
	{
		l_sValue = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr)->GetActiveCellValue(1);
	}
	else if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING)
	{
		if (SP_Core::Instance()->GetAnimMode())
		{
			l_sValue << dynamic_cast<SP_DS_DoubleMarkingAttribute*>(l_pcAttr)->GetValue();
		}
		else
		{
			l_sValue = l_pcAttr->GetValueString();
		}
	}
	else
    {
		l_sValue = l_pcAttr->GetValueString();
    }
	return DrawValueString(l_sValue);
}


//////////////////////////////////////////////////////////////////////
bool
SP_GR_MarkNumberAttribute::DrawValueString(const wxString& p_nValue)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

	m_pcPrimitive->Show(m_bShow && !p_nValue.IsEmpty() && p_nValue != wxT("0"));

	wxString l_sValue = SP_FormatWithComma(p_nValue);

	if(m_pcPrimitive->CalculateDimensions(l_sValue))
	{
		double l_nX = m_pcPrimitive->GetWidth();
		double l_nY = m_pcPrimitive->GetHeight();
		m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));
	}

    m_pcPrimitive->CalculateSize();

    return TRUE;
}


