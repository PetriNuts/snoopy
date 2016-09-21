//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_core/SP_GPR_Elements.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_TextAttribute::SP_GR_TextAttribute(SP_DS_Attribute* p_pcParent,
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

    SP_DS_Attribute* l_pcTextAttr = dynamic_cast<SP_DS_Attribute*>(p_pcParent);
	m_sPropertyName = l_pcTextAttr->GetName();
	SP_Data* l_pcDataNode = l_pcTextAttr->GetParent();

	if( l_pcDataNode )
	{
		if(l_pcDataNode->GetClassObject() )
		{
			m_sNodeClassName = l_pcDataNode->GetClassObject()->GetName();

			if(l_pcDataNode->GetClassObject()->GetParentGraph())
			{
				if(l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass())
				{
					m_sNetClassName = l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass()->GetName();
				}
			}
		}
	}
}

SP_GR_TextAttribute::~SP_GR_TextAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_TextAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_TextAttribute* l_pcNew = new SP_GR_TextAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_TextAttribute::FormatText()
{
    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");


    wxString l_pchVal = dynamic_cast<SP_DS_Attribute*>(GetParent())->GetValueString();
    if ( 200 < l_pchVal.Len() )
    {
       l_pchVal[196] = ' ';
       l_pchVal[197] = '.';
       l_pchVal[198] = '.';
       l_pchVal[199] = '.';
       l_pchVal[200] = '\0';
    }

    if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (l_pchVal.Len() != 0));

    if(l_pchVal.IsEmpty())
    {
    	return wxT("");
    }
    else
    {
		wxString l_sVal = m_sFormat;
		l_sVal.Replace(wxT("%"), l_pchVal);

		return l_sVal;
    }
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_TextAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    wxString l_sVal = FormatText();

	wxString l_sTextColor;

	if ((m_sNetClassName.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ))
	{
		if( m_sPropertyName == SP_DS_CPN_COLORSETNAME)
		{
			if(m_sNetClassName.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 )
				l_sTextColor = wxGetApp().GetElementPrefs()->GetColorSetColor(m_sNetClassName,wxT("Continuous Place"));
			else
				l_sTextColor = wxGetApp().GetElementPrefs()->GetColorSetColor(m_sNetClassName,m_sNodeClassName);
		}

	}

	if( l_sTextColor == wxT(""))
		l_sTextColor = m_sTextColor;

	m_pcPrimitive->SetTextColour(l_sTextColor);
	m_pcPrimitive->AddText(l_sVal);
	m_pcPrimitive->CalculateDimensions(l_sVal);

    return TRUE;
}

wxShape*
SP_GR_TextAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

