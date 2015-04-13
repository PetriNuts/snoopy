//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckuege2 $
// $Version: 0.0 $
// $Date: 2009/07/21 
// Short Description:  drawn circle implementation
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedRectangle::SP_GR_ExtendedRectangle(SP_DS_Node* p_pcParent,
                                                 double p_nWidth,
                                                 double p_nHeight,
						 bool p_bSquare,
                                                 SP_EXTENDED_TYPE p_nType,
						 size_t p_nThickness,wxColour p_pcColur)
:SP_GR_ExtendedDrawnShape(p_pcParent, p_nType,p_nThickness)
{
  m_bSquare=p_bSquare;
  SetWidth(p_nWidth);
  SetHeight(p_nHeight);
  SetPenColour(p_pcColur);
}

SP_GR_ExtendedRectangle::~SP_GR_ExtendedRectangle()
{
	if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ExtendedRectangle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_ExtendedRectangle* l_pcNew = new SP_GR_ExtendedRectangle(dynamic_cast<SP_DS_Node*>(p_pcParent),
        GetWidth(), GetHeight(),m_bSquare,GetType(),GetThickness(),GetPenColour());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_ExtendedRectangle::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();

  int l_nX = wxRound(GetWidth() / 2.0);
  int l_nY=0;
  if(m_bSquare) {
    l_nY = wxRound(GetWidth()/ 2.0);
  } else
  {
    l_nY = wxRound(GetHeight()/ 2.0);
  }
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
  size_t l_nPenthickness = GetThickness();
  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);

  if (SP_EXTENDED_TYPE_TWO == l_nType) 
  {
 	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_TWO);  
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawRectangle(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
	m_pcPrimitive->DrawRectangle(wxRect(-wxRound( l_nX * 0.5),-wxRound(l_nY * 0.5), l_nX, l_nY));
  }
  else
  {
 	if (SP_EXTENDED_TYPE_DOTTED == l_nType) 
	{
		SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED);	  
		m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
		m_pcPrimitive->DrawRectangle(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
	}
	else
	{
		if (SP_EXTENDED_TYPE_DOTTED_DASHED == l_nType) 
		{
			SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED_DASHED);  
			m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
			m_pcPrimitive->DrawRectangle(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
		}
		else
		{
			if (SP_EXTENDED_TYPE_DASHED == l_nType) 
			{
				SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DASHED);	  
				m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
				m_pcPrimitive->DrawRectangle(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
			}
			else
			{
				 //SP_EXTENDED_TYPE_DEFAULT
				SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DEFAULT);  
				m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
				m_pcPrimitive->DrawRectangle(wxRect(-l_nX,-l_nY, l_nX * 2, l_nY * 2));
			}
		}
	}
}

  m_pcPrimitive->CalculateSize();

  return TRUE;
}



