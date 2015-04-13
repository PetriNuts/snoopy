//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Mark (PED marking) attribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//by sl
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/extensions/SP_DS_Animation.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MarkAttribute::SP_GR_MarkAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth,
        double p_nHeight)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseDrawn();
    m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));
    m_pcPrimitive->SetHideDimensions(TRUE);
    m_pcPrimitive->SetSensitivityFilter(OP_ALL &~ OP_DRAG_LEFT);
	m_bColorNet = false;

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_MarkAttribute::~SP_GR_MarkAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_MarkAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);


    SP_GR_MarkAttribute* l_pcNew = new SP_GR_MarkAttribute(l_pcParent,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_MarkAttribute::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);
    CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());

	// deal with colored markings
	SP_Data* l_pcDataNode = l_pcAttr->GetParent();
	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{
		return FALSE;
	}

	wxString l_sNetClass = l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass()->GetName();
	if(l_sNetClass == SP_DS_COLEPN_CLASS 
		|| l_sNetClass == SP_DS_COLSPN_CLASS 
		|| l_sNetClass == SP_DS_COLPN_CLASS 
		|| l_sNetClass == SP_DS_COLCPN_CLASS )
	{
		m_bColorNet = true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wxString l_sValue = wxEmptyString;
	if(l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST)
	{
		l_sValue = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr)->GetActiveCellValue(1);
	}
	else if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)
	{
		if (SP_Core::Instance()->GetAnimMode())
		{
			l_sValue << dynamic_cast<SP_DS_MarkingAttribute*>(l_pcAttr)->GetValue();
		}
		else
		{
			l_sValue = l_pcAttr->GetValueString();
		}
	}
	else
	{
		CHECK_BOOL((l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NUMBER), return FALSE);
		l_sValue = l_pcAttr->GetValueString();
	}
	return DrawValueString(l_sValue);
}

wxShape*
SP_GR_MarkAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
SP_GR_MarkAttribute::DrawValueString(const wxString& p_nValue)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

	m_pcPrimitive->Show(m_bShow && !p_nValue.IsEmpty() && p_nValue != wxT("0"));

	if (p_nValue == wxT("0"))
	{
		// nothing to draw
	}
	else if(p_nValue == wxT("1"))
	{
		m_pcPrimitive->DrawEllipse(wxRect(-3, -3, 6, 6));
	}
	else if(p_nValue == wxT("2"))
	{
		m_pcPrimitive->DrawEllipse(wxRect(-7, -3, 6, 6));
		m_pcPrimitive->DrawEllipse(wxRect(1, -3, 6, 6));
	}
	else if(p_nValue == wxT("3"))
	{
		m_pcPrimitive->DrawEllipse(wxRect(-7, -5, 6, 6));
		m_pcPrimitive->DrawEllipse(wxRect(1, -5, 6, 6));
		m_pcPrimitive->DrawEllipse(wxRect(-3, 1, 6, 6));
	}
	else
	{
		wxString l_sValue = p_nValue;
		SP_FormatWithComma(l_sValue);

		if(m_pcPrimitive->CalculateDimensions(l_sValue))
		{
			double l_nX = m_pcPrimitive->GetWidth();
			double l_nY = m_pcPrimitive->GetHeight();
			m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));
		}
	}

    m_pcPrimitive->CalculateSize();

    return TRUE;
}

