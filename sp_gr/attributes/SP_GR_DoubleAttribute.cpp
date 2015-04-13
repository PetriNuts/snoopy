//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/05 14:35:00 $
// Short Description: Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
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
SP_GR_DoubleAttribute::SP_GR_DoubleAttribute(SP_DS_Attribute* p_pcParent,
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

SP_GR_DoubleAttribute::~SP_GR_DoubleAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_DoubleAttribute::Clone(SP_Data* p_pcParent)
{
    CHECK_POINTER(p_pcParent, return NULL);
    CHECK_BOOL((p_pcParent->GetElementType() == SP_ELEMENT_ATTRIBUTE), return NULL);

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_DoubleAttribute* l_pcNew = new SP_GR_DoubleAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_DoubleAttribute::FormatText()
{
    CHECK_POINTER(GetParent(), return wxT(""));
    CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return wxT(""));

    double value = 0;
	wxString l_pchVal = wxT("");
    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());


    if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST == l_pcAttr->GetAttributeType())
    {
      SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>(l_pcAttr);
      if (SP_ATTRIBUTELIST_DOUBLE == l_pcListAttr->GetAttributeListType())
      {
        SP_DS_DoubleListAttribute* l_pcAttribute = dynamic_cast<SP_DS_DoubleListAttribute*>((l_pcListAttr));
        value = l_pcAttribute->GetValueDouble();
		if (value > 0) {
			l_pchVal.Printf(wxT("%.16G"), value);
		}
      }
    } else {
      if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE){
		SP_DS_DoubleAttribute* l_pcAttribute = dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttr);
		value = l_pcAttribute->GetValue();
		if (value > 0) {
			l_pchVal.Printf(wxT("%.16G"), value);
		}
	  }
	}


    //int l_nLength = wxStrlen(l_pchVal);

    if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (value > 0));

    SP_FormatWithComma(l_pchVal);

    wxString l_sVal = m_sFormat;
    l_sVal.Replace(wxT("%"), l_pchVal);

    return l_sVal;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_DoubleAttribute::Update(bool p_bLocalOnly)
{
    if (!m_pcPrimitive)
        return FALSE;

    wxString l_sVal = FormatText();
    m_pcPrimitive->AddText(l_sVal);
    m_pcPrimitive->CalculateDimensions(l_sVal);
    return TRUE;
}

wxShape*
SP_GR_DoubleAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

