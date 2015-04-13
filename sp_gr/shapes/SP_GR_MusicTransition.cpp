//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/07/08 10:20:00 $
// Short Description: symbol of music transition
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_MusicTransition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MusicTransition::SP_GR_MusicTransition(SP_DS_Node* p_pcParent)
:SP_GR_DrawnShape(p_pcParent)
{
	SetWidth(20);
	SetHeight(20);
}

SP_GR_MusicTransition::~SP_GR_MusicTransition()
{
}

SP_Graphic*
SP_GR_MusicTransition::Clone(SP_Data* p_pcParent)
{
	if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
		return NULL;

	SP_GR_MusicTransition* l_pcNew = new SP_GR_MusicTransition(dynamic_cast<SP_DS_Node*>(p_pcParent));

	CloneBase(l_pcNew);

	return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_MusicTransition::Draw()
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

	m_pcPrimitive->DrawRectangle(wxRect(wxPoint(-l_nX, -l_nY),
		wxSize(l_nX * 2, l_nY * 2)));

	if (GetParent()->GetElementType() == SP_ELEMENT_NODE)
	{
		SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(GetParent());

		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("Sound")));

		if (l_pcAttr)
		{
				wxString l_sValue = wxString::Format(l_pcAttr->GetCell(l_pcAttr->GetActiveList(), wxT("Sound")));
				if (!l_sValue.IsEmpty())
				{
					//draw stem
					m_pcPrimitive->DrawLine(wxPoint((int)(l_nX * 0.4), (int)(-l_nY * 0.7)),
						wxPoint((int)(l_nX * 0.4), (int)(l_nY * 0.6)));

					m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
					//m_pcPrimitive->SetDrawnPen(wxBLACK_PEN, TRUE);
					//draw head
					m_pcPrimitive->DrawEllipse(wxRect(
						wxPoint((int)(-l_nX * 0.40), (int)(l_nY * 0.25)),
						wxPoint((int)(l_nX * 0.4), (int)(l_nY * 0.75)))
						);

				}
		}
	}

	m_pcPrimitive->CalculateSize();

	return TRUE;
}



