//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/04/29 13:43:00 $
// Short Description: Rhombus implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Rhombus.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Rhombus::SP_GR_Rhombus(SP_DS_Node* p_pcParent, double p_nWidth, double p_nHeight)
:SP_GR_DrawnShape(p_pcParent)
{
  SetWidth(p_nWidth);
  SetHeight(p_nHeight);
}

SP_GR_Rhombus::~SP_GR_Rhombus()
{
}

SP_Graphic*
SP_GR_Rhombus::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Rhombus* l_pcNew = new SP_GR_Rhombus(dynamic_cast<SP_DS_Node*>(p_pcParent), GetWidth(), GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_Rhombus::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();

  int l_nX = wxRound(GetWidth() / 2.0);
  int l_nY = wxRound(GetHeight() / 2.0);

  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);

  m_pcPrimitive->SetDrawnPen(GetPen());

   wxPoint points[4];
   points[0] = wxPoint( -l_nX, wxRound( -l_nY / 30.0)); //links mitte unterer punkt
   points[1] = wxPoint(wxRound( l_nX / 30.0) , l_nY);//mitte unten
   points[2] = wxPoint(l_nX, wxRound( -l_nY / 30.0)); //rechts mitte
   points[3] = wxPoint(wxRound( l_nX / 30.0 ), -l_nY );

   m_pcPrimitive->DrawPolygon(4, points);

   return TRUE;
}


bool
SP_GR_Rhombus::Merge(SP_Graphic* p_pcTarget)
{
	SP_DS_Node* targetNode = dynamic_cast<SP_DS_Node*>(p_pcTarget->GetParent());

	if (!targetNode->GetLogical()){
		SP_MESSAGEBOX(wxT("No merging possible for undependent undeveloped events."));
		return FALSE;
	}

	return SP_GR_Node::Merge(p_pcTarget);

}
