//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"

#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_NumberAttribute::SP_GR_NumberAttribute(SP_DS_Attribute* p_pcParent,
                                             const wxString& p_pchFormat,
                                             double p_nWidth,
                                             double p_nHeight)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL),
m_sFormat(p_pchFormat)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseText(FormatText(), p_nWidth, p_nHeight);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_NumberAttribute::~SP_GR_NumberAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_NumberAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_NumberAttribute* l_pcNew = new SP_GR_NumberAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_NumberAttribute::FormatText()
{
    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");


    wxString l_pchVal = dynamic_cast<SP_DS_Attribute*>(GetParent())->GetValueString();


	if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (l_pchVal.Len() != 0));

    wxString l_sVal = m_sFormat;
    l_sVal.Replace(wxT("%"), SP_FormatWithComma(l_pchVal));

    return l_sVal;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_NumberAttribute::Update(bool p_bLocalOnly)
{
	if (!m_pcPrimitive)
        return FALSE;

    wxString l_sVal = FormatText();

    m_pcPrimitive->AddText(l_sVal);
    m_pcPrimitive->CalculateDimensions(l_sVal);

    return TRUE;
}

wxShape*
SP_GR_NumberAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

