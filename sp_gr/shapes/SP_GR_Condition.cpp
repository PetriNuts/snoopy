//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/03 09:28:00 $
// Short Description: Condition-Gate implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Condition.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Condition::SP_GR_Condition(SP_DS_Node* p_pcParent, double p_nWidth, double p_nHeight)
:SP_GR_DrawnShape(p_pcParent)
{
  SetWidth(p_nWidth);
  SetHeight(p_nHeight);
}

SP_GR_Condition::~SP_GR_Condition()
{
}

SP_Graphic*
SP_GR_Condition::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Condition* l_pcNew = new SP_GR_Condition(dynamic_cast<SP_DS_Node*>(p_pcParent), GetWidth(), GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_Condition::Draw()
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

  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);

  m_pcPrimitive->SetDrawnPen(GetPen());

   wxPoint points[6];

   points[2] = wxPoint((int)(l_nX * 2.0), (int)(l_nY /1.2));
   points[1] = wxPoint((int)(l_nX * 2.0), (int)(-l_nY ));
   points[3] = wxPoint((int) (l_nX / 30.0 ), (int)(l_nY * 2.0));
   points[5] = wxPoint((int)(-l_nX * 2.0), (int)(-l_nY));
   points[4] = wxPoint((int)(-l_nX * 2.0), (int)(l_nY / 1.2));
   points[0] = wxPoint((int)(l_nX / 30.0  ), (int)(-l_nY * 2.0));

   m_pcPrimitive->DrawPolygon(6, points);


   m_pcPrimitive->CalculateSize();
   m_pcPrimitive->SetSize((int)(l_nX * 2.0 ) ,  (int)(l_nY * 2.0));

  return TRUE;
}



