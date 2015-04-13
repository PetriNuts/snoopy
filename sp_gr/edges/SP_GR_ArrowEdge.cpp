//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: edge with arrow implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/base/SP_GR_BaseEdge.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

// testing
#include "sp_gr/base/SP_GR_BaseText.h"
//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ArrowEdge::SP_GR_ArrowEdge(SP_DS_Edge* p_pcParent,
				 WXTYPE p_type, int p_end,
				 double p_size)
  :SP_GR_Edge(p_pcParent)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseEdge();
	m_nType = p_type;
    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);

    // because of bugs in ogl SetIgnoreOffsets must be set to true,
    // so offsets can not be used :(
    m_pcPrimitive->SetIgnoreOffsets(true);

    m_pcPrimitive->AddArrow(p_type, p_end, p_size);

    m_pcPrimitive->SetBrush(wxBLACK_BRUSH);

//     wxNode* l_pcNode = m_pcPrimitive->GetArrows().GetFirst();
//     while (l_pcNode)
//     {
//         dynamic_cast<wxArrowHead*>(l_pcNode->GetData())->SetWidthHeightRatio(4.0);
//         l_pcNode = l_pcNode->GetNext();
//     }
}

SP_GR_ArrowEdge::~SP_GR_ArrowEdge()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ArrowEdge::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_EDGE)
		return NULL;

	SP_GR_ArrowEdge* newAE;
	wxArrowHead* ah;
	wxNode* node = m_pcPrimitive->GetArrows().GetFirst();
	if (!node)
	{ // strange, we don't have any arrowheads?
		newAE = new SP_GR_ArrowEdge(dynamic_cast<SP_DS_Edge*> (p_pcParent));
	}
	else
	{
		ah = dynamic_cast<wxArrowHead*> (node->GetData());
		newAE = new SP_GR_ArrowEdge(dynamic_cast<SP_DS_Edge*> (p_pcParent),
				ah->_GetType(), ah->GetArrowEnd(), ah->GetSize());
		node = node->GetNext();
		while (node)
		{
			ah = dynamic_cast<wxArrowHead*> (node->GetData());
			newAE->AddAnotherArrow(ah->_GetType(), ah->GetArrowEnd(),
					ah->GetSize());
			node = node->GetNext();
		}
	}

	CloneBase(newAE);
	return newAE;
}

void
SP_GR_ArrowEdge::AddAnotherArrow(WXTYPE p_type, int p_end, double p_size)
{
  m_pcPrimitive->AddArrow(p_type, p_end, p_size);
}


//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
wxShape*
SP_GR_ArrowEdge::GetPrimitive()
{
    return m_pcPrimitive;
}



bool
SP_GR_ArrowEdge::SetDesignType(long p_nType)
{
    m_nType = (WXTYPE)p_nType;
    return TRUE;
}

long SP_GR_ArrowEdge::GetDesignType()
{
    return (long)m_nType;
}