//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 0 $
// $Date: 2009/06/17
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/shapes/SP_GR_ExtendedRhombus.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedRhombus::SP_GR_ExtendedRhombus(SP_DS_Node* p_pcParent, int p_nWidth, int p_nHeight, SP_EXTENDED_TYPE p_nType, size_t p_nThickness,wxColour p_pcColur)
:SP_GR_ExtendedDrawnShape(p_pcParent, p_nType,p_nThickness)
{
  SetWidth(p_nWidth);
  SetHeight(p_nHeight);
  SetPenColour(p_pcColur);
}

SP_GR_ExtendedRhombus::~SP_GR_ExtendedRhombus()
{
	if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ExtendedRhombus::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_ExtendedRhombus* l_pcNew = new SP_GR_ExtendedRhombus(dynamic_cast<SP_DS_Node*>(p_pcParent),GetWidth(),GetHeight(),GetType(),GetThickness(),GetPenColour());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_ExtendedRhombus::Draw()
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

  wxPoint points[4];
  points[0] = wxPoint(-l_nX					, -wxRound(-l_nY / 30.0) ); //links mitte unterer punkt
  points[1] = wxPoint(wxRound(l_nX / 30.0) , l_nY);//mitte unten
  points[2] = wxPoint(l_nX					, -wxRound(l_nY / 30.0)); //rechts mitte
  points[3] = wxPoint(wxRound(l_nX / 30.0 )	, -l_nY);

  wxPoint innerpoints[4];
  innerpoints[0] = wxPoint(-wxRound(l_nX * 0.75)	, -wxRound(l_nY / 30.0)); //links mitte unterer punkt
  innerpoints[1] = wxPoint(wxRound(l_nX / 30.0)		, wxRound(l_nY * 0.75));//mitte unten
  innerpoints[2] = wxPoint(wxRound(l_nX * 0.75)		, -wxRound(l_nY / 30.0)); //rechts mitte
  innerpoints[3] = wxPoint(wxRound(l_nX / 30.0 )	, -wxRound(l_nY * 0.75));


  if (SP_EXTENDED_TYPE_TWO == l_nType) {
	SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_TWO);  
	m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
	m_pcPrimitive->DrawPolygon(4, points);
	m_pcPrimitive->DrawPolygon(4, innerpoints);
  }
  else
	{
	if (SP_EXTENDED_TYPE_DOTTED == l_nType) {
		SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED);	  
		m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
		m_pcPrimitive->DrawPolygon(4, points);
	}
	else
	{
		if (SP_EXTENDED_TYPE_DOTTED_DASHED == l_nType) {
			SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED_DASHED);  
			m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
			m_pcPrimitive->DrawPolygon(4, points);
		}
		else
		{
			if (SP_EXTENDED_TYPE_DASHED == l_nType) {
				SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DASHED);
				m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
				m_pcPrimitive->DrawPolygon(4, points);
			}
			else
			{
				//SP_EXTENDED_TYPE_DEFAULT
				SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DEFAULT);  
				m_pcPrimitive->SetDrawnPen(GetCanvasModPen());
				m_pcPrimitive->DrawPolygon(4, points);
			}
		}
	}
  }
  m_pcPrimitive->CalculateSize();

  return TRUE;
}
