//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/07/01
// Short Description: Edge with multi linestyles and arrowstyles
//////////////////////////////////////////////////////////////////////
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/base/SP_GR_BaseEdge.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_GPR_Elements.h"
#include "snoopy.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedEdge::SP_GR_ExtendedEdge(SP_DS_Edge* p_pcParent, SP_EXTENDED_TYPE_EDGE p_nType, size_t p_nThickness,
									   SP_EXTENDED_ARROW_TYPE_EDGE p_nATypeLeft, double p_sizeLeft,
									   SP_EXTENDED_ARROW_TYPE_EDGE p_nATypeRight, double p_sizeRight,wxColour p_pcColur)
									   :SP_GR_Edge(p_pcParent)
{
	/* changed by ckruege2 17.06.2009 Initial */
	m_pcCanvasModPen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
	m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(wxColour(0, 0, 0), 1);
	/* changing end */
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseEdge();
    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);

    // because of bugs in ogl SetIgnoreOffsets must be set to true,
    // so offsets can not be used :(
    m_pcPrimitive->SetIgnoreOffsets(true);
	m_pcPrimitive->AddArrow(p_nATypeLeft, 0, p_sizeLeft);
	m_pcPrimitive->AddArrow(p_nATypeRight, 1, p_sizeRight);
	SetType(p_nType);
	SetThickness(p_nThickness);
	SetArrowTypeLeft(p_nATypeLeft);
	SetArrowLeftThickness(p_sizeLeft);
	SetArrowTypeRight(p_nATypeRight);
	SetArrowRightThickness(p_sizeRight);
	SetPenColour(p_pcColur);
	Update(TRUE);

}

SP_GR_ExtendedEdge::~SP_GR_ExtendedEdge()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic* SP_GR_ExtendedEdge::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_EDGE)
        return NULL;

    SP_GR_ExtendedEdge* newAE;
    wxArrowHead* ah;

    wxNode* node = m_pcPrimitive->GetArrows().GetFirst();
    if (! node) 
	{ // strange, we don't have any arrowheads?
      newAE = new SP_GR_ExtendedEdge(dynamic_cast<SP_DS_Edge*>(p_pcParent));
    } else 
	{
      ah = dynamic_cast<wxArrowHead*>(node->GetData());
      newAE = new SP_GR_ExtendedEdge(dynamic_cast<SP_DS_Edge*>(p_pcParent),GetType(), GetThickness(), GetArrowTypeLeft(),GetArrowLeftThickness(),GetArrowTypeRight(),GetArrowRightThickness(),GetPenColour());
      node = node->GetNext();
      while (node)  {
		ah = dynamic_cast<wxArrowHead*>(node->GetData());
		newAE->AddAnotherArrow(ah->_GetType(), ah->GetArrowEnd(),ah->GetSize());
		node = node->GetNext();
      }
    }
    CloneBase(newAE);
    return newAE;
}

void SP_GR_ExtendedEdge::AddAnotherArrow(WXTYPE p_type, int p_end, double p_size)
{
  m_pcPrimitive->AddArrow(p_type, p_end, p_size);
}


//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
wxShape* SP_GR_ExtendedEdge::GetPrimitive()
{
    return m_pcPrimitive;
}

bool SP_GR_ExtendedEdge::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(GetPrimitive(), return FALSE);

	SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
	if(GetParent()->GetElementType() == SP_ELEMENT_EDGE)
	{
		SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(GetParent());
		SP_DS_Attribute* l_pcAttr = l_pcEdge->GetAttribute(wxT("Spline"));
		bool l_bDrawSpline = false;
		if(l_pcAttr)
		{
			if(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL == l_pcAttr->GetAttributeType())
			{
				SP_DS_BoolAttribute* l_pcBoolAttr = dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttr);
				if(l_pcDoc)
				{
					wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(GetPrimitive());
					if(l_pcLineShape->GetLineControlPoints() &&	l_pcLineShape->GetLineControlPoints()->GetCount() > 2)
					{
						//local definition over global
						l_bDrawSpline = l_pcBoolAttr->GetValue();
					}
					if(l_bDrawSpline) 
					{
						l_pcLineShape->SetSpline(TRUE);
					} 
					else 
					{
						l_pcLineShape->SetSpline(FALSE);
					}
				}
			}
		}
		else
		{
			//no attribute, only preference check
			if(l_pcDoc)
			{
				wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(GetPrimitive());
				if(l_pcLineShape->GetLineControlPoints() &&	l_pcLineShape->GetLineControlPoints()->GetCount() > 2)
				{
					wxString l_sNetclass = l_pcDoc->GetNetclassName();
					l_bDrawSpline = wxGetApp().GetElementPrefs()->GetEdgeSpline(l_sNetclass);
				}
				l_pcLineShape->SetSpline(l_bDrawSpline);
			}
		}
	}
	SP_EXTENDED_TYPE_EDGE l_nType = m_nType;
/* 
GTK have problems with both designs. to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic, SP_GR_ExtendedEdge, shapes/SP_GR_Extended*, SP_GPR_Elements
*/
#ifdef linux
	//redirect from net drawable styles
	if(l_nType==SP_EXTENDED_TYPE_DOTTED_DASHED_EDGE || l_nType==SP_EXTENDED_TYPE_DASHED_EDGE)
	{
		l_nType =SP_EXTENDED_TYPE_DOTTED_EDGE;
	}
#endif
	SP_EXTENDED_ARROW_TYPE_EDGE l_nArrowTypeLeft = m_nArrowTypeLeft;
	SP_EXTENDED_ARROW_TYPE_EDGE l_nArrowTypeRight =m_nArrowTypeRight;
	GetPrimitive()->SetBrush(GetBrush());
	size_t l_nPenthickness = GetThickness();
	
	
		if (SP_EXTENDED_TYPE_DOTTED_EDGE == l_nType) 
		{
			SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED_EDGE);	  
			GetPrimitive()->SetPen(GetCanvasModPen());
		}
		else
		{
			if (SP_EXTENDED_TYPE_DOTTED_DASHED_EDGE == l_nType) 
			{
				SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DOTTED_DASHED_EDGE);  
				GetPrimitive()->SetPen(GetCanvasModPen());
			}
			else
			{
				if (SP_EXTENDED_TYPE_DASHED_EDGE == l_nType) 
				{
					SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DASHED_EDGE);	  
					GetPrimitive()->SetPen(GetCanvasModPen());
				}
				else
				{
					//SP_EXTENDED_TYPE_DEFAULT_EDGE
					SetCanvasModPenStyle(GetPenColour(),l_nPenthickness,SP_EXTENDED_TYPE_DEFAULT_EDGE);  
					GetPrimitive()->SetPen(GetCanvasModPen());
				}
			}
		}
	
	
	//Arrowheads
	m_pcPrimitive->ClearArrowsAtPosition();
	if(l_nArrowTypeLeft==SP_EXTENDED_ARROW_TYPE_DOUBLE_ARROW_EDGE) 
	{
		m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 0, GetArrowLeftThickness());
		AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 0, GetArrowLeftThickness());
	}
	else
	{
		if(l_nArrowTypeLeft==SP_EXTENDED_ARROW_TYPE_DOUBLE_CIRCLE_EDGE) 
		{
			m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 0, GetArrowLeftThickness());
			AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 0, GetArrowLeftThickness());
		}
		else
		{
			if(l_nArrowTypeLeft==SP_EXTENDED_ARROW_TYPE_CIRCLE_ARROW_EDGE) 
			{
				m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 0, GetArrowLeftThickness());
				AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 0, GetArrowLeftThickness());
			}
			else
			{
				if(l_nArrowTypeLeft== SP_EXTENDED_ARROW_TYPE_ARROW_EDGE ||
					l_nArrowTypeLeft== SP_EXTENDED_ARROW_TYPE_HOLLOW_CIRCLE_EDGE ||
					l_nArrowTypeLeft== SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE) {
					m_pcPrimitive->AddArrow(l_nArrowTypeLeft, 0, GetArrowLeftThickness());
				} else
				{
					//last is none arrowheads ;)
				}
			}
		}
	}
	if(l_nArrowTypeRight==SP_EXTENDED_ARROW_TYPE_DOUBLE_ARROW_EDGE) 
	{
		m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 1, GetArrowRightThickness());
		AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 1, GetArrowRightThickness());
	}
	else
	{
		if(l_nArrowTypeRight==SP_EXTENDED_ARROW_TYPE_DOUBLE_CIRCLE_EDGE) 
		{
			m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 1, GetArrowRightThickness());
			AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 1, GetArrowRightThickness());
		}
		else
		{
			if(l_nArrowTypeRight==SP_EXTENDED_ARROW_TYPE_CIRCLE_ARROW_EDGE) 
			{
				m_pcPrimitive->AddArrow(SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE, 1, GetArrowRightThickness());
				AddAnotherArrow(SP_EXTENDED_ARROW_TYPE_ARROW_EDGE, 1, GetArrowRightThickness());
			}
			else
			{
				if(l_nArrowTypeRight== SP_EXTENDED_ARROW_TYPE_ARROW_EDGE ||
					l_nArrowTypeRight== SP_EXTENDED_ARROW_TYPE_HOLLOW_CIRCLE_EDGE ||
					l_nArrowTypeRight== SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE) {
					m_pcPrimitive->AddArrow(l_nArrowTypeRight, 1, GetArrowRightThickness());
				} else
				{
					//last is none arrowheads ;)
				}
			}
		}
	}
	ToggleHide(GetHide());

    if (!GetPrimitive()->GetCanvas())
        return TRUE;

    return TRUE;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------

bool
SP_GR_ExtendedEdge::SetType(SP_EXTENDED_TYPE_EDGE p_nType)
{
    m_nType = p_nType;
    return TRUE;
}

SP_EXTENDED_TYPE_EDGE SP_GR_ExtendedEdge::GetType()
{
    return m_nType;
}

bool
SP_GR_ExtendedEdge::SetDesignType(long p_nType)
{
    m_nType = (SP_EXTENDED_TYPE_EDGE)p_nType;
    return TRUE;
}

long SP_GR_ExtendedEdge::GetDesignType()
{
    return (long)m_nType;
}

bool SP_GR_ExtendedEdge::SetThickness(size_t p_nThickness)
{

    m_nThickness = p_nThickness;
    return TRUE;
}

size_t SP_GR_ExtendedEdge::GetThickness()
{
    return m_nThickness;
}

bool SP_GR_ExtendedEdge::SetDesignThickness(long p_nThickness)
{
	if(p_nThickness<0) {
		return(FALSE);
	}
    m_nThickness = (size_t)p_nThickness;
    return TRUE;
}

long SP_GR_ExtendedEdge::GetDesignThickness()
{
    return (long)m_nThickness;
}

bool SP_GR_ExtendedEdge::SetArrowTypeRight(SP_EXTENDED_ARROW_TYPE_EDGE p_nArrowTypeRight)
{
    m_nArrowTypeRight = p_nArrowTypeRight;
    return TRUE;
}

SP_EXTENDED_ARROW_TYPE_EDGE SP_GR_ExtendedEdge::GetArrowTypeRight()
{
    return m_nArrowTypeRight;
}


bool
SP_GR_ExtendedEdge::SetDesignArrowTypeRight(long p_nArrowTypeRight)
{
    m_nArrowTypeRight = (SP_EXTENDED_ARROW_TYPE_EDGE)p_nArrowTypeRight;
    return TRUE;
}

long SP_GR_ExtendedEdge::GetDesignArrowTypeRight()
{
    return (long)m_nArrowTypeRight;
}


bool SP_GR_ExtendedEdge::SetArrowTypeLeft(SP_EXTENDED_ARROW_TYPE_EDGE p_nArrowTypeLeft)
{
    m_nArrowTypeLeft = p_nArrowTypeLeft;
    return TRUE;
}

SP_EXTENDED_ARROW_TYPE_EDGE SP_GR_ExtendedEdge::GetArrowTypeLeft()
{
    return m_nArrowTypeLeft;
}


bool
SP_GR_ExtendedEdge::SetDesignArrowTypeLeft(long p_nArrowTypeLeft)
{
    m_nArrowTypeLeft = (SP_EXTENDED_ARROW_TYPE_EDGE)p_nArrowTypeLeft;
    return TRUE;
}

long
SP_GR_ExtendedEdge::GetDesignArrowTypeLeft()
{
    return (long)m_nArrowTypeLeft;
}

bool SP_GR_ExtendedEdge::SetArrowLeftThickness(size_t l_nArrowLeftSize) 
{
	m_nArrowLeftSize=l_nArrowLeftSize;
	return TRUE;
}
bool SP_GR_ExtendedEdge::SetArrowRightThickness(size_t l_nArrowRightSize) 
{
	m_nArrowRightSize = l_nArrowRightSize;
	return TRUE;
}
size_t SP_GR_ExtendedEdge::GetArrowLeftThickness() 
{
	return m_nArrowLeftSize;
}
size_t SP_GR_ExtendedEdge::GetArrowRightThickness() 
{
	return m_nArrowRightSize;
}

bool SP_GR_ExtendedEdge::SetCanvasModPenStyle(wxColour colour,int thick,int style)
{
	if(style == SP_EXTENDED_TYPE_DEFAULT_EDGE)
	{
		m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick);
		m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick);
	}
	else
	{
		if(style == SP_EXTENDED_TYPE_DOTTED_EDGE) 
		{
			m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT);
			m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT);
		}
		else
		{
			if(style == SP_EXTENDED_TYPE_DOTTED_DASHED_EDGE)
			{
				m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT_DASH);
				m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_DOT_DASH);
			}
			else
			{
				if(style == SP_EXTENDED_TYPE_DASHED_EDGE) 
				{
					m_pcCanvasModPen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_SHORT_DASH);
					m_pcCanvasModCoarsePen = wxThePenList->FindOrCreatePen(colour, thick, wxPENSTYLE_SHORT_DASH);
				}
			}
		}
	}
	return true;
}
