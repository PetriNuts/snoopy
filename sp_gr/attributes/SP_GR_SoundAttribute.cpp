//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Mark (PED marking) attribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_SoundAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
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
SP_GR_SoundAttribute::SP_GR_SoundAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth,
        double p_nHeight)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseDrawn();
    m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_SoundAttribute::~SP_GR_SoundAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_SoundAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    CHECK_BOOL((l_pcParent->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST), return NULL);

    SP_GR_SoundAttribute* l_pcNew = new SP_GR_SoundAttribute(l_pcParent,
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
SP_GR_SoundAttribute::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);
    CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE), return FALSE);

    SP_DS_Attribute* l_pcAttr1 = dynamic_cast<SP_DS_Attribute*>(GetParent());

    CHECK_BOOL((l_pcAttr1->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST), return FALSE);

	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr1->GetAttribute(wxT("Sound")));

	if (l_pcAttr)
	{
		wxString l_sValue = wxString::Format(l_pcAttr->GetCell(l_pcAttr->GetActiveList(), wxT("Sound")));
		if (!l_sValue.IsEmpty())
		{
			//return Draw();
			return true;
		}
	}
	return false;

}

wxShape*
SP_GR_SoundAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

//////////////////////////////////////////////////////////////////////
bool
SP_GR_SoundAttribute::Draw()
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

	int l_nX = (int) (GetWidth() / 2.0);
	int l_nY = (int) (GetHeight() / 2.0);

	m_pcPrimitive->SetDrawnBrush(GetBrush(), FALSE);
	m_pcPrimitive->SetDrawnPen(wxBLACK_PEN);

    //draw stem
	m_pcPrimitive->DrawLine(wxPoint((int)(l_nX * 0.4), (int)(-l_nY * 0.7)),
		wxPoint((int)(l_nX * 0.4), (int)(l_nY * 0.6)));

	m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
	//m_pcPrimitive->SetDrawnPen(wxBLACK_PEN, TRUE);
	//draw head
	m_pcPrimitive->DrawEllipse(wxRect(
		wxPoint((int)(-l_nX * 0.40), (int)(l_nY * 0.25)),
		wxPoint((int)(l_nX * 0.4), (int)(l_nY * 0.75))));

    m_pcPrimitive->CalculateSize();

    return TRUE;
}



