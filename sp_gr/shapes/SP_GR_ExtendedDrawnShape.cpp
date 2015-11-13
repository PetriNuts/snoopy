//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/28 13:10:00 $
// Short Description: drawn shape implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_core/SP_GPR_Elements.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedDrawnShape::SP_GR_ExtendedDrawnShape(SP_DS_Node* p_pcParent, 
                                                   SP_EXTENDED_TYPE p_nType, size_t p_nThickness)
:SP_GR_DrawnShape(p_pcParent),
m_nType(p_nType),
m_nThickness(p_nThickness)
{
	/* changed by ckruege2 17.06.2009 Initial */
	m_pcCanvasModPen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
	m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
	/* changing end */
}

SP_GR_ExtendedDrawnShape::~SP_GR_ExtendedDrawnShape()
{
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_ExtendedDrawnShape::SetType(SP_EXTENDED_TYPE p_nType)
{
    m_nType = p_nType;
    return TRUE;
}

SP_EXTENDED_TYPE
SP_GR_ExtendedDrawnShape::GetType()
{
    return m_nType;
}

bool
SP_GR_ExtendedDrawnShape::SetDesignType(long p_nType)
{
    m_nType = (SP_EXTENDED_TYPE)p_nType;
    return TRUE;
}

long
SP_GR_ExtendedDrawnShape::GetDesignType()
{
    return (long)m_nType;
}

bool SP_GR_ExtendedDrawnShape::SetThickness(size_t p_nThickness)
{

    m_nThickness = p_nThickness;
    return TRUE;
}

size_t
SP_GR_ExtendedDrawnShape::GetThickness()
{
    return m_nThickness;
}

bool
SP_GR_ExtendedDrawnShape::SetDesignThickness(long p_nThickness)
{
	if(p_nThickness<0) {
		return(FALSE);
	}
    m_nThickness = (size_t)p_nThickness;
    return TRUE;
}

long
SP_GR_ExtendedDrawnShape::GetDesignThickness()
{
    return (long)m_nThickness;
}

SP_EXTENDED_TYPE SP_GR_ExtendedDrawnShape::ReadDefaultDesignType()
{
  return wxGetApp().GetElementPrefs()->GetNodeDesignType(SP_DS_CONTINUOUSPED_CLASS);
}

bool SP_GR_ExtendedDrawnShape::SetCanvasModPenStyle(wxColour colour,int thick,int style)
{
	if(style == SP_EXTENDED_TYPE_DEFAULT || style == SP_EXTENDED_TYPE_TWO)
	{
		m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick);
		m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick);
	}
	else if(style == SP_EXTENDED_TYPE_DOTTED)
	{
		m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT);
		m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT);
	}
	else if(style == SP_EXTENDED_TYPE_DOTTED_DASHED)
	{
		m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT_DASH);
		m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT_DASH);
	}
	else if(style == SP_EXTENDED_TYPE_DASHED)
	{
		m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_SHORT_DASH);
		m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_SHORT_DASH);
	}
	return true;
}
