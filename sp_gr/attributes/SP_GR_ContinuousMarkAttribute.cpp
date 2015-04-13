//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Mark (PED marking) attribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_ContinuousMarkAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ContinuousMarkAttribute::SP_GR_ContinuousMarkAttribute(SP_DS_Attribute* p_pcParent,
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

SP_GR_ContinuousMarkAttribute::~SP_GR_ContinuousMarkAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ContinuousMarkAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);    

    SP_GR_ContinuousMarkAttribute* l_pcNew = new SP_GR_ContinuousMarkAttribute(l_pcParent,
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
SP_GR_ContinuousMarkAttribute::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);
    CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

    SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>(GetParent());

	///
	// deal with colored markings
	SP_Data* l_pcDataNode = l_pcAttr->GetParent();
	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{
		return FALSE;
	}

	wxString l_sNetClass = l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass()->GetName();
	if( l_sNetClass == SP_DS_COLCPN_CLASS ||
		l_sNetClass == SP_DS_COLHPN_CLASS)
	{
		m_bColorNet = true;
	}
	////

	if(l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_TEXT)
	{
		wxString l_longvalue  = dynamic_cast<SP_DS_TextAttribute*>(l_pcAttr)->GetValue();
		
		return DrawValue(l_longvalue );
	}

	return true;
	
}

wxShape*
SP_GR_ContinuousMarkAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

//////////////////////////////////////////////////////////////////////
bool
SP_GR_ContinuousMarkAttribute::DrawValue(wxString p_sValue)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

    m_pcPrimitive->Show(m_bShow && (p_sValue != wxT("0")));

	if(m_bColorNet) // By Liu
	{
		wxString l_sValue = p_sValue;
		SP_FormatWithComma(l_sValue);
        if(m_pcPrimitive->CalculateDimensions(l_sValue))
        {
    		double l_nX = m_pcPrimitive->GetWidth();
    		double l_nY = m_pcPrimitive->GetHeight();
    	    m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));			
		}
	}
	/*
	else
	{
		switch (p_nValue)
		{
		case 0:
			// nothing to draw
			break;
		case 1:
		  m_pcPrimitive->DrawEllipse(wxRect(-3, -3, 6, 6));
		  break;
		case 2:
		  m_pcPrimitive->DrawEllipse(wxRect(-7, -3, 6, 6));
		  m_pcPrimitive->DrawEllipse(wxRect(1, -3, 6, 6));
		  break;
		case 3:
		  m_pcPrimitive->DrawEllipse(wxRect(-7, -5, 6, 6));
		  m_pcPrimitive->DrawEllipse(wxRect(1, -5, 6, 6));
		  m_pcPrimitive->DrawEllipse(wxRect(-3, 1, 6, 6));
		  break;
		case 4:
		default:
			wxString l_sValue;
			l_sValue << p_nValue;
			if(m_pcPrimitive->CalculateDimensions(l_sValue))
			{
				double l_nX = m_pcPrimitive->GetWidth();
				double l_nY = m_pcPrimitive->GetHeight();
				m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));
			}
			break;
		};
	}
	*/

    m_pcPrimitive->CalculateSize();

    return TRUE;
}



