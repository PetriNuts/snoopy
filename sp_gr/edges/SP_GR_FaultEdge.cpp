//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/09/06 11:40:00 $
// Short Description: edge with arrow implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/edges/SP_GR_FaultEdge.h"
#include "sp_gr/base/SP_GR_BaseEdge.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Elements.h"
#include "snoopy.h"

// testing
#include "sp_gr/base/SP_GR_BaseText.h"

SP_GR_FaultEdge::SP_GR_FaultEdge(SP_DS_Edge* p_pcParent,
				 WXTYPE p_type, int p_end,
				 double p_size)
  :SP_GR_Edge(p_pcParent)
{

    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseEdge();
    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);

    // because of bugs in ogl SetIgnoreOffsets must be set to true,
    // so offsets can not be used :(
    m_pcPrimitive->SetIgnoreOffsets(true);


	//NEG-Gates: Edges with a specific design
	if (GetGraphicState() == SP_STATE_NEG) {
		m_pcPrimitive->ClearArrowsAtPosition();
		m_pcPrimitive->AddArrow(ARROW_HOLLOW_CIRCLE, ARROW_POSITION_START, 7);

	} else {

		SP_EXTENDED_ARC_TYPE l_nType = wxGetApp().GetElementPrefs()->GetEdgeDesignType();
		if ( SP_EXTENDED_TYPE_DIRECTED==l_nType ) {
			m_pcPrimitive->ClearArrowsAtPosition();
			m_pcPrimitive->AddArrow(ARROW_ARROW, ARROW_POSITION_END, 7);
		}
		if ( SP_EXTENDED_TYPE_UNDIRECTED==l_nType ) {
			m_pcPrimitive->ClearArrowsAtPosition();
			m_pcPrimitive->AddArrow(0, 0, 0);
		}
	}

    m_pcPrimitive->SetBrush(wxBLACK_BRUSH);
}

SP_GR_FaultEdge::~SP_GR_FaultEdge()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_FaultEdge::Clone(SP_Data* p_pcParent)
{

    if (p_pcParent->GetElementType() != SP_ELEMENT_EDGE)
        return NULL;

    SP_GR_FaultEdge* newAE;
    wxArrowHead* ah;
    wxNode* node = m_pcPrimitive->GetArrows().GetFirst();
    if (! node) { // strange, we don't have any arrowheads?
      newAE = new SP_GR_FaultEdge(dynamic_cast<SP_DS_Edge*>(p_pcParent));
    } else {
      ah = dynamic_cast<wxArrowHead*>(node->GetData());
      newAE = new SP_GR_FaultEdge(dynamic_cast<SP_DS_Edge*>(p_pcParent),
				  ah->_GetType(),
				  ah->GetArrowEnd(),
				  ah->GetSize());
      node = node->GetNext();
      while (node)  {
	ah = dynamic_cast<wxArrowHead*>(node->GetData());
	newAE->AddAnotherArrow(ah->_GetType(), ah->GetArrowEnd(),
			       ah->GetSize());
	node = node->GetNext();
      }
    }

    CloneBase(newAE);
    return newAE;
}

void
SP_GR_FaultEdge::AddAnotherArrow(WXTYPE p_type, int p_end, double p_size)
{
  m_pcPrimitive->AddArrow(p_type, p_end, p_size);
}


//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
wxShape*
SP_GR_FaultEdge::GetPrimitive()
{
    return m_pcPrimitive;
}

bool
SP_GR_FaultEdge::Update(bool p_bLocalOnly)
{

    SP_EXTENDED_ARC_TYPE l_nType = wxGetApp().GetElementPrefs()->GetEdgeDesignType();

	//NEG-Gates: Edges with a specific design
	if (GetGraphicState() == SP_STATE_NEG)
	{
		GetPrimitive()->SetPen(GetPen());
		GetPrimitive()->SetBrush(GetBrush());
		SP_GR_BaseEdge* act_pcPrimitive = dynamic_cast<SP_GR_BaseEdge*>(GetPrimitive());

		act_pcPrimitive->ClearArrowsAtPosition();
		act_pcPrimitive->AddArrow(ARROW_HOLLOW_CIRCLE, ARROW_POSITION_START, 7);
	}
	else
	{
		if ( SP_EXTENDED_TYPE_DIRECTED==l_nType )
		{
			GetPrimitive()->SetPen(GetPen());
			GetPrimitive()->SetBrush(GetBrush());
			SP_GR_BaseEdge* act_pcPrimitive = dynamic_cast<SP_GR_BaseEdge*>(GetPrimitive());

			act_pcPrimitive->ClearArrowsAtPosition();
			act_pcPrimitive->AddArrow(ARROW_ARROW, ARROW_POSITION_END, 7);
		}
		if ( SP_EXTENDED_TYPE_UNDIRECTED==l_nType )
		{
			GetPrimitive()->SetPen(GetPen());
			GetPrimitive()->SetBrush(GetBrush());
			SP_GR_BaseEdge* act_pcPrimitive = dynamic_cast<SP_GR_BaseEdge*>(GetPrimitive());
			act_pcPrimitive->ClearArrowsAtPosition();
			act_pcPrimitive->AddArrow(0, 0, 0);
		}
	}
    return TRUE;
}
