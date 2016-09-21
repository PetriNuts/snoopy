//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/27 16:37:00 $
// Short Description:  drawn circle implementation
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_Core.h"

#include <math.h>

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedCircle::SP_GR_ExtendedCircle(SP_DS_Node* p_pcParent,double p_nR, SP_EXTENDED_TYPE p_nType, size_t p_nThickness,wxColour p_pcColur)
:SP_GR_ExtendedDrawnShape(p_pcParent, p_nType,p_nThickness)
{
  SetWidth(p_nR);
  SetPenColour(p_pcColur);
}

SP_GR_ExtendedCircle::~SP_GR_ExtendedCircle()
{
	if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ExtendedCircle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
	{
        return NULL;
	}
    SP_GR_ExtendedCircle* l_pcNew = new SP_GR_ExtendedCircle(dynamic_cast<SP_DS_Node*>(p_pcParent),this->GetWidth(),GetType(),GetThickness(),GetPenColour());
    CloneBase(l_pcNew);
    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_ExtendedCircle::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();

  int l_nX = wxRound(GetWidth() / 2.0);
  int l_nY = l_nX;
  SP_EXTENDED_TYPE l_nType = m_nType;
/* 
GTK have problems with both designs. to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic, SP_GR_ExtendedEdge, shapes/SP_GR_Extended*, SP_GPR_Elements
*/
#ifdef linux
	//redirect from net drawable styles
	if(l_nType==SP_EXTENDED_TYPE_DOTTED_DASHED || l_nType==SP_EXTENDED_TYPE_DASHED)
	{
		l_nType = SP_EXTENDED_TYPE_DOTTED;
	}
#endif
  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
  size_t l_nPenthickness = GetThickness();

  if (SP_EXTENDED_TYPE_TWO == l_nType) {
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_TWO);
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawEllipse(wxRect(-l_nX, -l_nY, wxRound(GetWidth()),wxRound(GetWidth())));
	m_pcPrimitive->DrawEllipse(wxRect(-wxRound(l_nX *0.5),-wxRound(l_nY *0.5),wxRound( l_nX ) ,wxRound( l_nY )));
  }
  else if (SP_EXTENDED_TYPE_DOTTED == l_nType) {
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED);
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawEllipse(wxRect(-l_nX,-l_nY,l_nX * 2,l_nY * 2));
  }
  else if (SP_EXTENDED_TYPE_DOTTED_DASHED == l_nType) {
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED_DASHED);
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawEllipse(wxRect(-l_nX,-l_nY,l_nX * 2,l_nY * 2));
  }
  else if (SP_EXTENDED_TYPE_DASHED == l_nType) {
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DASHED);
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawEllipse(wxRect(-l_nX,-l_nY,l_nX * 2,l_nY * 2));
  }
  else
  {
	//SP_EXTENDED_TYPE_DEFAULT
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DEFAULT);
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawEllipse(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
  }

  if (GetParent()->GetElementType() == SP_ELEMENT_NODE)
  {
    SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(GetParent());
    SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Fixed"));
    if (l_pcAttr)
    {
      if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL == l_pcAttr->GetAttributeType())
      {
        SP_DS_BoolAttribute* l_pcBoolAttr = dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttr);
        if (l_pcBoolAttr->GetValue())
        {
          m_pcPrimitive->SetDrawnPen(m_pcPrimitive->GetPen());
          int l_nSinX = wxRound(l_nX * sin(0.6 * 3.1415927 * 0.5));
          int l_nSinY = wxRound(l_nY * cos(0.6 * 3.1415927 * 0.5));
          m_pcPrimitive->DrawLine(wxPoint(-l_nSinX,  l_nSinY),
                                  wxPoint( l_nSinX,  l_nSinY));
          m_pcPrimitive->DrawLine(wxPoint(-l_nSinX, -l_nSinY - 1),
                                  wxPoint( l_nSinX, -l_nSinY - 1));
        }
      }
    }
  }
  m_pcPrimitive->CalculateSize();

  return TRUE;
}
