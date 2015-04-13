
//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruge2 $
// $Version: 0.0 $
// $Revision:  $
// $Date: 2009/07/21 $
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Actor.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Actor::SP_GR_Actor(SP_DS_Node* p_pcParent)
:SP_GR_DrawnShape(p_pcParent)
{
  SetWidth(30);
  SetHeight(30);
}

SP_GR_Actor::~SP_GR_Actor()
{
	if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_Actor::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Actor* l_pcNew = new SP_GR_Actor(dynamic_cast<SP_DS_Node*>(p_pcParent));

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_Actor::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();

  int l_nX = wxRound (GetWidth() / 2.0);
  int l_nY = wxRound (GetHeight() / 2.0);

  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
  wxPoint points[9];
  points[0] = wxPoint(0, -wxRound(l_nY * 0.65)); //Hals
  points[1] = wxPoint(0, wxRound(l_nY * 0.35));//schritt
  points[2] = wxPoint(-l_nX , l_nY ); //fuss1
  points[3] = wxPoint(l_nX , l_nY);
  points[4] = wxPoint(l_nX, 0); //hand
  points[5] = wxPoint(-l_nX, 0); 
  points[6] = wxPoint(0,-l_nY); //kopf oben
  points[7] = wxPoint(wxRound(l_nX * 0.35),-wxRound(l_nY * 0.8)); //ohr
  points[8] = wxPoint(-wxRound(l_nX * 0.35),-wxRound(l_nY * 0.8)); //ohr

  m_pcPrimitive->SetDrawnPen(GetPen());
  m_pcPrimitive->DrawLine(points[0],points[1]); //body
  m_pcPrimitive->DrawLine(points[1],points[2]); //leg
  m_pcPrimitive->DrawLine(points[1],points[3]); //leg
  m_pcPrimitive->DrawLine(points[0],points[4]); //arm
  m_pcPrimitive->DrawLine(points[0],points[5]); //arm
  m_pcPrimitive->DrawLine(points[0],points[7]); //kopf
  m_pcPrimitive->DrawLine(points[0],points[8]); //kopf
  m_pcPrimitive->DrawLine(points[7],points[6]); //kopf
  m_pcPrimitive->DrawLine(points[8],points[6]); //kopf
  m_pcPrimitive->CalculateSize();

  return TRUE;
}
