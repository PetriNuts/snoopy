//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

// cos, sin, fabs
#include <math.h>

#include "snoopy.h"

SP_Graphic::SP_Graphic(SP_GRAPHIC_TYPE p_eType)
:m_eGraphicType(p_eType),
m_bShow(TRUE),
m_eGraphicState(SP_STATE_NORMAL),
m_pcDefaultBrush(p_eType == SP_GRAPHIC_EDGE ? wxTheBrushList->FindOrCreateBrush(wxColour(0,0,0)) : wxTheBrushList->FindOrCreateBrush(wxColour(255,255,255))),
m_pcDefaultPen(wxThePenList->FindOrCreatePen(wxColour(0,0,0), 1)),
m_pcBrush(NULL),
m_pcPen(NULL),
m_bBrushSet(FALSE),
m_bPenSet(FALSE),
m_nThickness(1),
m_pcParent(NULL),
m_sTextColor(wxT("BLACK"))
{
    m_pcGraphicParent = this;
}

SP_Graphic::~SP_Graphic()
{
}

bool
SP_Graphic::CloneBase(SP_Graphic* p_pcCopy)
{
    if (!p_pcCopy)
        return FALSE;

	p_pcCopy->SetTextColor(this->GetTextColor());
    p_pcCopy->SetShow(GetShow());
    p_pcCopy->SetGraphicState(GetGraphicState());
    p_pcCopy->SetNetnumber(GetNetnumber());
    // brush and pen
    if (m_pcDefaultBrush)
        p_pcCopy->SetDefaultBrush(m_pcDefaultBrush);
    if (m_pcDefaultPen)
        p_pcCopy->SetDefaultPen(m_pcDefaultPen);
    if (m_pcBrush)
        p_pcCopy->SetBrush(m_pcBrush);
    if (m_pcPen)
        p_pcCopy->SetPen(m_pcPen);

    return TRUE;
}

bool
SP_Graphic::Equals(SP_Graphic* p_pcVal, const int p_eFlag)
{
    if (!p_pcVal || !GetParent())
        return FALSE;

    if (((p_eFlag &~ SP_EQUALTEST_GRAPHICTYPE) || (m_eGraphicType == p_pcVal->GetGraphicType())) &&
        ((p_eFlag &~ SP_EQUALTEST_GRAPHICSTATE) || (m_eGraphicState == p_pcVal->GetGraphicState())) )
        return GetParent()->Equals(p_pcVal->GetParent());

    return FALSE;
}

bool
SP_Graphic::ResetGraphicState()
{
    return TRUE;
}

SP_Data*
SP_Graphic::SetDataParent(SP_Data* p_pcParent)
{
    if (!p_pcParent)
        return NULL;

    // just insert the new kid on the block
    // maybe pointers are already reused (after a call to delete)
    // but the entry was never removed here
    // that shouldn't happen at all, but it can ... maybe ... sometimes
    m_pcParent = p_pcParent;

    return m_pcParent;
}

SP_Data*
SP_Graphic::GetParent()
{
	return m_pcParent;
}

void
SP_Graphic::Select(bool p_bVal, wxDC* p_pcDC)
{
	for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
	  l_pcGrChild->Select(p_bVal, p_pcDC);
	}

    if (GetPrimitive() && GetPrimitive()->Selected() != p_bVal)
    {
        // GetPrimitive()->Show(m_bShow);
        GetPrimitive()->Select(p_bVal, p_pcDC);
    }
}

wxBrush*
SP_Graphic::GetBrush()
{
	if(m_pcBrush)
	{
		return m_pcBrush;
	}
	else if(m_pcDefaultBrush)
	{
		return m_pcDefaultBrush;
	}
	else
	{
		return wxGetApp().GetCanvasNormalBrush();
	}
}

wxColour
SP_Graphic::GetBrushColour()
{
	return GetBrush()->GetColour();
}

wxPen*
SP_Graphic::GetPen()
{
	if(m_pcPen)
	{
		return m_pcPen;
	}
	else if(m_pcDefaultPen)
	{
		return m_pcDefaultPen;
	}
	else
	{
		return wxGetApp().GetCanvasNormalPen();
	}
}

wxColour 
SP_Graphic::GetPenColour()
{
	return GetPen()->GetColour();
}

bool
SP_Graphic::AddChildToCanvas(SP_Graphic* p_pcChild)
{
    if (!GetPrimitive() ||
        !p_pcChild ||
        !p_pcChild->GetPrimitive())
        return FALSE;

    // I have to report some sort of 'attachement point' for my children
    // (they should be attributes in most (all?) cases, that's why the
    // points are reported by GetPosAttribute methods)

	double tmpX = GetPosAttributesX() + p_pcChild->GetOffsetX();//george: attribute position
	 

    if (tmpX < 0) {
      tmpX = GetPosAttributesX() + 5;
      SP_LOGDEBUG(wxString::Format(wxT("refused to assign a negative x position (%f)")
				     wxT(" to an attribute with offset (%f), set x to %f"),
				     GetPosAttributesX() + p_pcChild->GetOffsetX(),
				     p_pcChild->GetOffsetX(),
				     tmpX));
    }

    double tmpY =  GetPosAttributesY() + p_pcChild->GetOffsetY();
    if (tmpY < 0) {
      tmpY = GetPosAttributesY() + 5;
      SP_LOGDEBUG(wxString::Format(wxT("refused to assign a negative y position (%f)")
				     wxT(" to an attribute with offset (%f), set y to %f"),
				     GetPosAttributesY() + p_pcChild->GetOffsetY(),
				     p_pcChild->GetOffsetY(),
				     tmpY));
    }

    p_pcChild->SetPosXY(tmpX, tmpY);

    if (!AddGraphicChildren(p_pcChild))
        return FALSE;

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas());
    if (!l_pcCanvas)
        return FALSE;

    // if not already added
    if (!p_pcChild->GetPrimitive()->GetCanvas())
    {
    	// METADATA is inserted at the beginning of the shape list,
    	// so we have to look for it and add the child right after it,
    	// instead of just appending them
    	if(GetGraphicType() == SP_GRAPHIC_METADATA)
    	{
    		l_pcCanvas->AddShape(p_pcChild->GetPrimitive(), GetPrimitive());
    	}
    	else
    	{
    		l_pcCanvas->AddShape(p_pcChild->GetPrimitive());
    	}
        // p_pcChild->GetPrimitive()->Show(p_pcChild->GetShow());
    }

    return TRUE;
}

bool
SP_Graphic::CheckIntegrity()
{

  if (!GetPrimitive())
		return FALSE;

	double tmpX, tmpY;

	SP_Graphic* l_pcParent = GetGraphicParent();
	if (!l_pcParent || l_pcParent == this)
	{
		tmpX = GetPosX();
		tmpY = GetPosY();
		if (tmpX < 0)
		{
			SP_LOGDEBUG(wxString::Format(wxT("attempt to assign a negative x position (%f)")
							wxT(" set x to %f"), tmpX, -tmpX));
			SetPosX(-tmpX);
			return FALSE;
		}
		if (tmpY < 0)
		{
			SP_LOGDEBUG(wxString::Format(wxT("attempt to assign a negative y position (%f)")
							wxT(" set y to %f"), tmpY, -tmpY));
			SetPosY(-tmpY);
			return FALSE;
		}
	}
	else
	{
		if(l_pcParent->GetNetnumber() != GetNetnumber())
		{
			SetNetnumber(l_pcParent->GetNetnumber(), GetNetnumber());
		}
	}

	//   SP_LOGMESSAGE(
	// 	 wxString::Format(wxT("%ld gr type:%d"), GetId(), GetGraphicType()));

	double parX = GetPosAttributesX();
	double parY = GetPosAttributesY();
	double newYOffset = wxGetApp().getStdOffset();
	bool xMoved;
	for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		wxString l_sMsg = l_pcGrChild->GetGraphicParent()->GetParent()->GetClassName();
		l_sMsg << wxT(" - ") << l_pcGrChild->GetParent()->GetClassName();
		l_sMsg << wxT(": ");

		if (l_pcGrChild->GetPrimitive())
		{
			xMoved = FALSE;

			// saved in buggy version of snoopy
			if (fabs(parX + l_pcGrChild->GetOffsetX() - l_pcGrChild->GetPosX()) > 2)
			{
				// x is ok, offset is wrong
				if (l_pcGrChild->GetPosX() > 0 && fabs(l_pcGrChild->GetPosX() - parX)
						< wxGetApp().getDelta())
				{
					l_pcGrChild->SetOffsetX(l_pcGrChild->GetPosX() - parX);
					SP_LOGDEBUG(l_sMsg + wxT("wrong x offset"));
				}
				else if (parX + l_pcGrChild->GetOffsetX() > 0 && // offset looks not bad
						fabs(l_pcGrChild->GetOffsetX()) < wxGetApp().getDelta())
				{
					l_pcGrChild->SetPosX(parX + l_pcGrChild->GetOffsetX());
					SP_LOGDEBUG(l_sMsg + wxT("wrong x position"));
				}
				else
				{ // both x and offset seem to be bad
					l_pcGrChild->SetOffsetX(wxGetApp().getStdOffset());
					l_pcGrChild->SetPosX(parX + wxGetApp().getStdOffset());
					l_pcGrChild->Select(TRUE);
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("possibly wrong x coordinate met, moved to parent, x=%f"),
									parX + wxGetApp().getStdOffset()));
					xMoved = TRUE;
				}
			}
			else
			{ // position and offset are intact
				if (l_pcGrChild->GetPosX() < 0)
				{
					l_pcGrChild->SetOffsetX(wxGetApp().getStdOffset());
					l_pcGrChild->SetPosX(parX + wxGetApp().getStdOffset());
					l_pcGrChild->Select(TRUE);
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("negative x coordinate met, moved to parent, x=%f"),
									parX + wxGetApp().getStdOffset()));
					xMoved = TRUE;
					l_pcGrChild->Select(TRUE);
				}
				else if (fabs(l_pcGrChild->GetOffsetX()) > wxGetApp().getDelta())
				{
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("looks like child x coordinate %f ")
									wxT("is quite away from the parent x coordinate %f"),
									l_pcGrChild->GetPosX(), parX));

					if(l_pcGrChild->GetOffsetX() > 0)
					{
						l_pcGrChild->SetOffsetX(wxGetApp().getStdOffset());
						l_pcGrChild->SetPosX(parX + wxGetApp().getStdOffset());
					}
					else
					{
						l_pcGrChild->SetOffsetX(-wxGetApp().getStdOffset());
						l_pcGrChild->SetPosX(parX - wxGetApp().getStdOffset());
					}
					l_pcGrChild->Select(TRUE);
				}
			}

			// now almost the same for y, but assigned offset is not a const

			// saved in buggy version of snoopy or created by a converter
			if (fabs(parY + l_pcGrChild->GetOffsetY() - l_pcGrChild->GetPosY()) > 2)
			{
				// y is ok, offset is wrong
				if (l_pcGrChild->GetPosY() > 0 && fabs(l_pcGrChild->GetPosY() - parY)
						< wxGetApp().getDelta())
				{
					l_pcGrChild->SetOffsetY(l_pcGrChild->GetPosY() - parY);
					SP_LOGDEBUG(l_sMsg + wxT("wrong y offset"));
				}
				else if (parY + l_pcGrChild->GetOffsetY() > 0 && // offset looks not bad
						fabs(l_pcGrChild->GetOffsetY()) < wxGetApp().getDelta())
				{
					l_pcGrChild->SetPosY(parY + l_pcGrChild->GetOffsetY());
					SP_LOGDEBUG(l_sMsg + wxT("wrong y position"));
				}
				else
				{ // both y and offset seem to be bad
					l_pcGrChild->SetOffsetY(newYOffset);
					l_pcGrChild->SetPosY(parY + newYOffset);
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("possibly wrong y coordinate met, moved to parent, y=%f"),
									parY + newYOffset));
					if (xMoved)
					{
						newYOffset += wxGetApp().getStdOffset();
					}

					l_pcGrChild->Select(TRUE);
				}
			}
			else
			{ // position and offset are intact
				if (l_pcGrChild->GetPosY() < 0)
				{
					l_pcGrChild->SetOffsetY(newYOffset);
					l_pcGrChild->SetPosY(parY + newYOffset);
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("negative y coordinate met, moved to parent, y=%f"),
									parY + newYOffset));
					if (xMoved)
					{
						newYOffset += wxGetApp().getStdOffset();
					}

					l_pcGrChild->Select(TRUE);
				}
				else if (fabs(l_pcGrChild->GetOffsetY()) > wxGetApp().getDelta())
				{
					SP_LOGDEBUG(l_sMsg + wxString::Format(wxT("looks like child y coordinate %f ")
									wxT("is quite away from the parent y coordinate %f"),
									l_pcGrChild->GetPosY(), parY));

					if(l_pcGrChild->GetOffsetY() > 0)
					{
						l_pcGrChild->SetOffsetY(wxGetApp().getStdOffset());
						l_pcGrChild->SetPosY(parY + wxGetApp().getStdOffset());
					}
					else
					{
						l_pcGrChild->SetOffsetY(-wxGetApp().getStdOffset());
						l_pcGrChild->SetPosY(parY - wxGetApp().getStdOffset());
					}
					l_pcGrChild->Select(TRUE);
				}
			}
		}

		if(l_pcGrChild->GetNetnumber() != GetNetnumber())
		{
			l_pcGrChild->SetNetnumber(GetNetnumber(), l_pcGrChild->GetNetnumber());
		}

		if (l_pcGrChild->GetGraphicChildren()->size())
		{
			SP_LOGDEBUG(l_sMsg + wxString(wxT("several levels of graphics parentship")));
			l_pcGrChild->CheckIntegrity();
		}
	}

	return TRUE;
}

bool
SP_Graphic::AddGraphicChildren(SP_Graphic* p_pcChild)
{
    if (!p_pcChild)
        return FALSE;

    // every item has set itself as m_pcGraphicParent member.
    // so, only if the parameter isn't NULL and not attached to any parent,
    // AND not to a parent, that is marked for deletion
    // we continue
    if (p_pcChild->GetGraphicParent() && p_pcChild->GetGraphicParent() != p_pcChild)
    {
    	if ((p_pcChild->GetGraphicParent() != this) &&
            (!p_pcChild->GetGraphicParent()->GetDelete()))
        {
        	return FALSE;
        }
    }
    // remember this pointer to be bound to me,
    // so, moving etc. should affect this child, too.
    if (SP_Find(m_lGraphicChildren, p_pcChild) == m_lGraphicChildren.end())
    {
    	m_lGraphicChildren.push_back(p_pcChild);
    }
    // and remember me as graphic parent for the param
    p_pcChild->SetGraphicParent(this);

    return TRUE;
}

SP_ListGraphic*
SP_Graphic::GetGraphicChildren()
{
	return &m_lGraphicChildren;
}

bool
SP_Graphic::RemoveGraphicChildren(SP_Graphic* p_pcChild)
{
    if (!p_pcChild)
        return FALSE;

    // every item has set itself as m_pcGraphicParent member.
    // so, only if the parameter isn't NULL and not attached to any parent,
    // AND not to a parent, that is marked for deletion
    // we continue
    if (p_pcChild->GetGraphicParent() && p_pcChild->GetGraphicParent() != p_pcChild)
    {
    	if ((p_pcChild->GetGraphicParent() != this) &&
            (!p_pcChild->GetGraphicParent()->GetDelete()))
    	{
    		return FALSE;
    	}
    }

    // remember this pointer to be bound to me,
    // so, moving etc. should affect this child, too.
    if (SP_Find(m_lGraphicChildren, p_pcChild) != m_lGraphicChildren.end())
    {
    	m_lGraphicChildren.remove(p_pcChild);
    }
    // and remember me as graphic parent for the param
    p_pcChild->SetGraphicParent(p_pcChild);
    // removing children of p_pcChild from canvas
    for(SP_Graphic* l_pcGraphicChild : *(p_pcChild->GetGraphicChildren())) {
    			l_pcGraphicChild->RemoveFromCanvas();
	}

    return TRUE;
}

bool
SP_Graphic::AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter)
{
    if (!GetPrimitive() || !GetParent())
        return FALSE;

    if (GetDelete())
        return TRUE;

    for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		// recursive add all children to the queue
		l_pcGrChild->AddToDeleteQueue(p_eFilter);
	}

    GetPrimitive()->Select(FALSE);

    return SP_Core::Instance()->AddToDeleteQueue(this);
}

bool
SP_Graphic::AddAllToDeleteQueue(SP_GRAPHIC_STATE p_eFilter)
{
    if (!GetPrimitive() || !GetParent())
        return FALSE;

    bool l_bReturn = TRUE;
	for (SP_Graphic* l_pcGraphic : *GetParent()->GetGraphics())
	{
		l_bReturn &= l_pcGraphic->AddToDeleteQueue(p_eFilter);
	}

    return l_bReturn;
}

bool
SP_Graphic::CollectDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
    if (!p_ptmAttributes)
        return FALSE;

    SP_Data* l_pcParent = this->GetParent();
    if (l_pcParent)
    {
        // ask every node to add it's attributes to the map
        l_pcParent->AddDialogAttributes(p_ptmAttributes);
    }

    return TRUE;
}

bool
SP_Graphic::Merge(SP_Graphic* p_pcTarget)
{
    if (!p_pcTarget || (p_pcTarget == this) || !Equals(p_pcTarget))
        return FALSE;

    // if the target is in normal state, this node can change this setting
    if (p_pcTarget->GetGraphicState() == SP_STATE_NORMAL)
        p_pcTarget->SetGraphicState(GetGraphicState());

    return AddToDeleteQueue();
}

bool
SP_Graphic::Rotate(double p_nX, double p_nY, double p_nAngle)
{
    /*
         cos(a)  sin(a)       m_X - p_nX       (m_X - p_nX) *  cos(a) + (m_Y - p_nY) * sin(a)
       (                ) * (            ) = (                                                )
        -sin(a)  cos(a)       m_Y - p_nY       (m_X - p_nX) * -sin(a) + (m_Y - p_nY) * cos(a)
    */
    double l_nAngle = p_nAngle * SP_PI / 180.0;

    double l_nCos = cos(l_nAngle);
    double l_nSin = sin(l_nAngle);

    double l_nX = (GetPosX() - p_nX) * l_nCos + (GetPosY() - p_nY) * l_nSin;
    double l_nY = (GetPosX() - p_nX) * (0 - l_nSin) + (GetPosY() - p_nY) * l_nCos;

    return Translate((l_nX + p_nX) - GetPosX(), (l_nY + p_nY) - GetPosY());
}

bool
SP_Graphic::Flip(double p_nX)
{
    return Translate((p_nX - GetPosX()) * 2, 0);
}

bool
SP_Graphic::Mirror(double p_nY)
{
    return Translate(0, (p_nY - GetPosY()) * 2);
}

bool
SP_Graphic::Translate(double p_nX, double p_nY)
{
    bool l_bReturn = SetPosXY(GetPosX() + p_nX, GetPosY() + p_nY);

    for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		// recursive add all children to the queue
		l_bReturn &= l_pcGrChild->Translate(p_nX, p_nY);
	}

    return l_bReturn;
}

bool
SP_Graphic::Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes)
{
    if (!GetParent())
        return FALSE;

    return GetParent()->Coarse(p_nNewNet, this, p_pcCoarseGr, p_plShapes);
}

bool
SP_Graphic::OnCoarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes)
{
	for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		// recursive Coarse the children
		l_pcGrChild->Coarse(p_nNewNet, p_pcCoarseGr, p_plShapes);
	}
    return RemoveFromCanvas() && SetNetnumber(p_nNewNet);
}

bool
SP_Graphic::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    if ((p_nFromNet == p_nToNet) ||
        (GetNetnumber() != p_nFromNet) ||
        (GetNetnumber() == p_nToNet))
        return TRUE;

    if (p_plGraphics)
        p_plGraphics->push_back(this);

    return RemoveFromCanvas() && SetNetnumber(p_nToNet);
}

bool
SP_Graphic::AddToCopyMaps(SP_DS_Graph** p_ppcGraph,
                          SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                          SP_MapData2Data* p_pmData2Data)
{
    if (!p_ppcGraph || !p_pmGraphic2Graphic || !p_pmData2Data || !GetParent())
        return FALSE;

    // coarseborderdown will only be copied, if the parent is already queued for copy
    if ((GetGraphicState() == SP_STATE_COARSEBORDERDOWN) &&
        (p_pmData2Data->find(GetParent()) == p_pmData2Data->end()))
        return FALSE;

    // get the new data element or create one
    SP_Data* l_pcNewData = GetParent()->AddToCopyMap(p_ppcGraph, this, p_pmGraphic2Graphic, p_pmData2Data);
    if (!l_pcNewData)
        return FALSE;

    SP_Graphic* l_pcNewGraphic = Clone(l_pcNewData);
    l_pcNewGraphic->SetPosXY(GetPosX(), GetPosY());
    l_pcNewData->AddGraphic(l_pcNewGraphic);
    (*p_pmGraphic2Graphic)[this] = l_pcNewGraphic;

    return SynchronizeChildren(p_pmGraphic2Graphic);
}

bool
SP_Graphic::SynchronizeChildren(SP_MapGraphic2Graphic* p_pmGraphic2Graphic)
{
    if (!p_pmGraphic2Graphic)
        return FALSE;

    SP_MapGraphic2Graphic::iterator l_itGr2Gr = p_pmGraphic2Graphic->find(m_pcGraphicParent);
    if(l_itGr2Gr != p_pmGraphic2Graphic->end())
    {
        SP_Graphic* l_pcOldParent = l_itGr2Gr->first;
        SP_Graphic* l_pcNewParent = l_itGr2Gr->second;

        for (SP_Graphic* l_pcGrChild : *l_pcOldParent->GetGraphicChildren())
        {
        	l_itGr2Gr = p_pmGraphic2Graphic->find(l_pcGrChild);
            if (l_itGr2Gr != p_pmGraphic2Graphic->end())
            {
            	SP_Graphic* l_pcGraphic1 = l_itGr2Gr->first;
            	SP_Graphic* l_pcGraphic2 = l_itGr2Gr->second;
                l_pcGraphic2->SetPosXY(l_pcGraphic1->GetPosX(), l_pcGraphic1->GetPosY());
                l_pcNewParent->AddGraphicChildren(l_pcGraphic2);
            }
        }
    }
    return TRUE;
}

bool
SP_Graphic::ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
    if (!p_pmNet2Net || !p_pcToGraph)
        return FALSE;

    SP_NetElement::SetNetnumber(GetPasteNetnumber(p_pmNet2Net, GetNetnumber(), p_pcToGraph));

    return TRUE;
}

bool
SP_Graphic::RemoveFromCanvas()
{
    if (!GetPrimitive() || !GetCanvas())
        return TRUE;

    GetCanvas()->RemoveShape(GetPrimitive());
    GetPrimitive()->SetCanvas(NULL);

    return TRUE;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_Graphic::SetPosX(double p_nVal)
{
    if (!GetPrimitive())
        return FALSE;

    if(GetPrimitive()->GetCanvas())
    {
    	wxSize l_VirtualSize = GetPrimitive()->GetCanvas()->GetVirtualSize();
    	if(l_VirtualSize.GetX() < p_nVal)
    	{
        	static_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas())->SetVirtualSizeX(WXROUND(p_nVal+20));
    	}
    }

    GetPrimitive()->SetX(p_nVal);
    return TRUE;
}

double
SP_Graphic::GetPosX()
{
    if (!GetPrimitive())
        return 0.0;
    return (GetPrimitive()->GetX());
}

bool
SP_Graphic::SetPosY(double p_nVal)
{
    if (!GetPrimitive())
        return FALSE;

	if(GetPrimitive()->GetCanvas())
    {
    	wxSize l_VirtualSize = GetPrimitive()->GetCanvas()->GetVirtualSize();
    	if(l_VirtualSize.GetY() < p_nVal)
    	{
        	static_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas())->SetVirtualSizeY(WXROUND(p_nVal+20));
    	}
    }

    GetPrimitive()->SetY(p_nVal);
    return TRUE;
}

double
SP_Graphic::GetPosY()
{
    if (!GetPrimitive())
        return 0.0;
    return (GetPrimitive()->GetY());
}

bool
SP_Graphic::SetPosXY(double p_nVal1, double p_nVal2)
{
    return (SetPosX(p_nVal1) && SetPosY(p_nVal2));
}

void
SP_Graphic::GetPosXY(double* p_pnVal1, double* p_pnVal2)
{
    *p_pnVal1 = GetPosX();
    *p_pnVal2 = GetPosY();
}

bool
SP_Graphic::SetThickness(int p_nThickness)
{
    if(p_nThickness <= 0)
    {
        m_nThickness = 1;
    }
    else
    {
        m_nThickness = p_nThickness;
    }
    return TRUE;
}

int
SP_Graphic::GetThickness()
{
    return m_nThickness;
}

bool
SP_Graphic::SetShow(bool p_bVal)
{
    if (m_bShow == p_bVal)
        return m_bShow;

    m_bShow = p_bVal;

    // if we have to hide, do it anyway, else let the update methods
    // of the graphics do the job
    if (GetPrimitive())
    	GetPrimitive()->Show(p_bVal);

    Update(TRUE);

    return m_bShow;
}

bool
SP_Graphic::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    if (GetDelete())
        return TRUE;

    bool l_bReturn = TRUE;
    for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		// recursive add all children to the queue
		l_bReturn &= l_pcGrChild->SetNetnumber(p_nNewVal, p_nOldVal);
	}

    return l_bReturn && SP_NetElement::SetNetnumber(p_nNewVal, p_nOldVal);
}

void
SP_Graphic::GetBoundingBox(double* p_pnX1, double* p_pnY1, double* p_pnX2, double* p_pnY2)
{
    if (!GetPrimitive() ||
        !p_pnX1 || !p_pnY1 || !p_pnX2 || !p_pnY2)
        return;

    double l_nWidth = 0, l_nHeight = 0;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);
    l_nWidth /=  2.0;
    l_nHeight /= 2.0;

    *p_pnX1 = GetPosX() - l_nWidth;
    *p_pnX2 = GetPosX() + l_nWidth;
    *p_pnY1 = GetPosY() - l_nHeight;
    *p_pnY2 = GetPosY() + l_nHeight;
}

bool
SP_Graphic::ShowDebug(unsigned int p_nTabs)
{
    SP_LOGDEBUG(wxString::Format(wxT("SP_Graphic  %p    at (%03f, %03f) with   %d")
				   wxT("children and    %p   as graphic parent and")
				   wxT("%p   as ds parent in net %u"),
				   this, GetPosX(), GetPosY(),
				   m_lGraphicChildren.size(),
				   GetGraphicParent(), GetParent(),
				   GetNetnumber()).Pad(p_nTabs, '\t', FALSE));

    for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		// recursive add all children to the queue
		l_pcGrChild->ShowDebug(4);
	}

    return TRUE;
}

bool
SP_Graphic::SetGraphicState(SP_GRAPHIC_STATE p_eVal)
{
//   wxString t = GetParent()->GetClassName();
//   SP_LOGDEBUG(wxString::Format(wxT("SetGraphicState(%d), %s"),
// 				 p_eVal, t.c_str()));
  m_eGraphicState = p_eVal;
  return TRUE;
}


bool
SP_Graphic::UpdateChildrenOffsets()
{
	bool res = TRUE;
	double posAX = GetPosAttributesX();
	double posAY = GetPosAttributesY();
	for (SP_Graphic* l_pcGrChild : m_lGraphicChildren)
	{
		if (l_pcGrChild->GetPosX() - posAX != l_pcGrChild->GetOffsetX())
		{
			res = FALSE;
			l_pcGrChild->SetOffsetX(l_pcGrChild->GetPosX() - posAX);
		}
		if (l_pcGrChild->GetPosY() - posAY != l_pcGrChild->GetOffsetY())
		{
			res = FALSE;
			l_pcGrChild->SetOffsetY(l_pcGrChild->GetPosY() - posAY);
		}
	}

	return res;
}

void SP_Graphic::ToggleHide(bool p_bHide)
{
	unsigned char l_chAlpha = GetPrimitive()->GetBrush()->GetColour().Alpha();
	if((p_bHide == true && l_chAlpha != 50) ||
		(p_bHide == false && l_chAlpha != 255))
	{
		l_chAlpha = p_bHide ? 50 : 255;
		auto l_nBrushStyle = GetPrimitive()->GetBrush()->GetStyle();
		unsigned char l_chRed = GetPrimitive()->GetBrush()->GetColour().Red();
		unsigned char l_chGreen = GetPrimitive()->GetBrush()->GetColour().Green();
		unsigned char l_chBlue = GetPrimitive()->GetBrush()->GetColour().Blue();
		GetPrimitive()->SetBrush(wxTheBrushList->FindOrCreateBrush(wxColour(l_chRed,l_chGreen,l_chBlue,l_chAlpha), l_nBrushStyle));

		int l_nWidth = GetPrimitive()->GetPen()->GetWidth();
		auto l_nPenStyle = GetPrimitive()->GetPen()->GetStyle();
		l_chRed = GetPrimitive()->GetPen()->GetColour().Red();
		l_chGreen = GetPrimitive()->GetPen()->GetColour().Green();
		l_chBlue = GetPrimitive()->GetPen()->GetColour().Blue();
		GetPrimitive()->SetPen(wxThePenList->FindOrCreatePen(wxColour(l_chRed,l_chGreen,l_chBlue,l_chAlpha), l_nWidth, l_nPenStyle));
	}
}
