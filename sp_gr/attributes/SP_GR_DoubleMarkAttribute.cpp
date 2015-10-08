//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/05 14:08:00 $
// Short Description: Mark (PED marking) attribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_DoubleMarkAttribute::SP_GR_DoubleMarkAttribute(SP_DS_Attribute* p_pcParent, double p_nWidth, double p_nHeight) :
	SP_GR_Attribute(p_pcParent), m_pcPrimitive(NULL)
{
	// instance for displaying on the canvas
	m_pcPrimitive = new SP_GR_BaseDrawn();
	m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));
	m_pcPrimitive->SetHideDimensions(TRUE);
	m_pcPrimitive->SetSensitivityFilter(OP_ALL & ~OP_DRAG_LEFT);

	SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_DoubleMarkAttribute::~SP_GR_DoubleMarkAttribute()
{
	if (m_pcPrimitive)
	{
		SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
		wxDELETE(m_pcPrimitive);
	}
	m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_DoubleMarkAttribute::Clone(SP_Data* p_pcParent)
{
	if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
		return NULL;

	SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*> (p_pcParent);
	SP_GR_DoubleMarkAttribute* l_pcNew = new SP_GR_DoubleMarkAttribute(l_pcParent, this->GetWidth(), this->GetHeight());

	CloneBase(l_pcNew);

	return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool SP_GR_DoubleMarkAttribute::Update(bool p_bLocalOnly)
{
	CHECK_POINTER(m_pcPrimitive, return FALSE);
	CHECK_POINTER(GetParent(), return FALSE);
	CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

	SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*> (GetParent());

	if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE)
	{
		return DrawValue(dynamic_cast<SP_DS_DoubleAttribute*> (l_pcAttr)->GetValue());
	}
	else
		if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST)
		{
			if (dynamic_cast<SP_DS_ListAttribute*> (l_pcAttr)->GetAttributeListType() == SP_ATTRIBUTELIST_DOUBLE)
			{
				return DrawValue(dynamic_cast<SP_DS_DoubleListAttribute*> (l_pcAttr)->GetValueDouble());
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}

}

wxShape*
SP_GR_DoubleMarkAttribute::GetPrimitive()
{
	return m_pcPrimitive;
}

//////////////////////////////////////////////////////////////////////
bool SP_GR_DoubleMarkAttribute::DrawValue(double p_nValue)
{
	CHECK_POINTER(m_pcPrimitive, return FALSE);

	// Clear Content
	// because of GetMetaFile() returns a reference object,
	// we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
	// so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

	m_pcPrimitive->Show(m_bShow && (p_nValue > 0));
    wxString l_sValue;
    l_sValue << p_nValue;
    l_sValue = SP_FormatWithComma(l_sValue);
    if(m_pcPrimitive->CalculateDimensions(l_sValue))
    {
		double l_nX = m_pcPrimitive->GetWidth();
		double l_nY = m_pcPrimitive->GetHeight();
	    m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));

	    m_pcPrimitive->CalculateSize();
    }

	return TRUE;
}

