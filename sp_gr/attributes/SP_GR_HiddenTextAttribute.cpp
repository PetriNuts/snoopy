//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_HiddenTextAttribute::SP_GR_HiddenTextAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat,
        double p_nWidth,
        double p_nHeight)
:SP_GR_TextAttribute(p_pcParent, p_pchFormat, p_nWidth, p_nHeight)
{
}

SP_GR_HiddenTextAttribute::~SP_GR_HiddenTextAttribute()
{
}

SP_Graphic*
SP_GR_HiddenTextAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_HiddenTextAttribute* l_pcNew = new SP_GR_HiddenTextAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_HiddenTextAttribute::FormatText()
{
    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");

    wxString l_pchVal = dynamic_cast<SP_DS_Attribute*>(GetParent())->GetValueString();

    if (m_pcPrimitive)
        m_pcPrimitive->Show(FALSE);

    wxString l_sVal = m_sFormat;
    l_sVal.Replace(wxT("%"), l_pchVal);

    return l_sVal;
}

/*
//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_HiddenTextAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    wxString l_sVal = FormatText();

    if (m_pcPrimitive->CalculateDimensions(l_sVal))
    {
        wxClientDC l_cDC(GetCanvas());
        GetCanvas()->DoPrepareDC(l_cDC);
        m_pcPrimitive->Erase(l_cDC);
        m_pcPrimitive->FormatText(l_cDC, l_sVal);
        m_pcPrimitive->Move(l_cDC, m_pcPrimitive->GetX(), m_pcPrimitive->GetY(), TRUE);
        m_pcPrimitive->Draw(l_cDC);
    }
    return TRUE;
}

wxShape*
SP_GR_HiddenTextAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

*/
