//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

#include "sp_core/base/SP_Type.h"
#include "sp_core/SP_Core.h"
#include "sp_defines.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Graph.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Canvas.h"

IMPLEMENT_CLASS(SP_GUI_Canvas, wxShapeCanvas)

BEGIN_EVENT_TABLE(SP_GUI_Canvas, wxShapeCanvas)
EVT_MOUSE_EVENTS(SP_GUI_Canvas::OnMouseEvent)
EVT_MOUSEWHEEL(SP_GUI_Canvas::OnMouseEvent)
EVT_PAINT(SP_GUI_Canvas::OnPaint)
EVT_KEY_DOWN(SP_GUI_Canvas::OnKeyEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
SP_GUI_Canvas::SP_GUI_Canvas(SP_MDI_View* p_pcView,
		SP_GUI_Childframe *p_pcFrame, wxWindowID p_nId, const wxPoint& p_cPos,
		const wxSize& p_cSize, const long p_nStyle) :
	wxShapeCanvas(p_pcFrame, p_nId, p_cPos, p_cSize, p_nStyle),
			m_pcView(p_pcView), m_pcParentframe(p_pcFrame),
			m_pcEditElement(NULL), m_pcControlPoints(NULL),
			m_nLastPosX(0),m_nLastPosY(0)
{
	m_bStartup = TRUE;
	SetView(p_pcView);

	SetBackgroundColour(wxColour(wxT("WHITE")));
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetCursor(wxCursor(wxCURSOR_HAND));

	SetScrollRate(SP_DEFAULT_GRID_SPACING, SP_DEFAULT_GRID_SPACING);
	SetVirtualSize(SP_DEFAULT_GRID_SIZE, SP_DEFAULT_GRID_SIZE);
	EnableScrolling(true,true);
}

SP_GUI_Canvas::~SP_GUI_Canvas()
{
}

void SP_GUI_Canvas::AddShape(wxShape* p_pcShape, wxShape *addAfter)
{
	if(addAfter)
	{
		if (GetDiagram() && p_pcShape)
			if(GetDiagram()->GetShapeList())
			{
				auto nodeAfter = addAfter->GetShapeListIterator();
				if(!nodeAfter)
					nodeAfter = GetDiagram()->GetShapeList()->Find(addAfter);
				decltype(nodeAfter) pos = NULL;
				if (nodeAfter)
				{
					if (nodeAfter->GetNext())
						pos = GetDiagram()->GetShapeList()->Insert(nodeAfter->GetNext(), p_pcShape);
					else
						pos = GetDiagram()->GetShapeList()->Append(p_pcShape);
				}
				else
				{
					pos = GetDiagram()->GetShapeList()->Append(p_pcShape);
				}
				p_pcShape->SetCanvas(GetDiagram()->GetCanvas());
				p_pcShape->SetShapeListIterator(pos);
			}
	}
	else
	{
		AddShape(p_pcShape);
	}
}

void SP_GUI_Canvas::AddShape(wxShape* p_pcShape)
{
	if (GetDiagram() && p_pcShape)
		if(GetDiagram()->GetShapeList())
		{
			auto pos = GetDiagram()->GetShapeList()->Append(p_pcShape);
			p_pcShape->SetCanvas(GetDiagram()->GetCanvas());
			p_pcShape->SetShapeListIterator(pos);
		}
}

void SP_GUI_Canvas::InsertShape(wxShape* p_pcShape)
{
	if (GetDiagram() && p_pcShape)
		GetDiagram()->InsertShape(p_pcShape);
}

void SP_GUI_Canvas::RemoveShape(wxShape *p_pcShape)
{
	if (GetDiagram())
		GetDiagram()->RemoveShape(p_pcShape);
}

void SP_GUI_Canvas::OnPaint(wxPaintEvent &p_cEvent)
{
	if (!GetDiagram())
		return;

	SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
	if(l_pcDoc)
	{
		int l_nSize = wxGetApp().GetCanvasPrefs()->GetGridSize(l_pcDoc->GetNetclassName());
		SetVirtualSize(l_nSize*GetScaleX(), l_nSize*GetScaleY());
/*
		SetScrollbars(SP_DEFAULT_GRID_SPACING,
					SP_DEFAULT_GRID_SPACING,
					l_nSize/SP_DEFAULT_GRID_SPACING*GetScaleX(),
					l_nSize/SP_DEFAULT_GRID_SPACING*GetScaleY(),
					GetScrollPos(wxHORIZONTAL),
					GetScrollPos(wxVERTICAL));
*/
	}

	// new paint implementation
	wxAutoBufferedPaintDC dc(this);
#if wxUSE_GRAPHICS_CONTEXT
	wxGCDC gcdc(dc);
	DoPrepareDC(gcdc);
	gcdc.SetUserScale(GetScaleX(), GetScaleY()); //needed for Zoom
	gcdc.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
	gcdc.Clear();
	GetDiagram()->Redraw(gcdc);
	gcdc.SetUserScale(1.0, 1.0); //needed for Zoom
#else
	DoPrepareDC(dc);
	dc.SetUserScale(GetScaleX(), GetScaleY()); //needed for Zoom
	dc.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
	dc.Clear();
	GetDiagram()->Redraw(dc);
	dc.SetUserScale(1.0, 1.0); //needed for Zoom
#endif
}

bool SP_GUI_Canvas::UnSelectAll(int p_nKeys)
{
	if (p_nKeys == KEY_SHIFT)
		return FALSE;

	if (GetView())
	{
		GetView()->SelectAll(FALSE);
		RefreshRects();
	}

	return TRUE;
}

void SP_GUI_Canvas::RefreshRects(bool p_bErase)
{
	Refresh(p_bErase);
	m_Overlay.Reset();
}

SP_Type* SP_GUI_Canvas::SetEditElement(SP_Type* p_pcElement)
{
	if (m_pcEditElement == p_pcElement)
		return m_pcEditElement;

	SP_LOGDEBUG(wxString::Format(wxT("SP_GUI_Canvas::SetEditElement(%p);"), p_pcElement));

	if (m_pcEditElement && (m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS))
	{
		if (m_pcControlPoints)
		{
			SP_Graphic* l_pcGr = static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->GetStartShape();
			if(l_pcGr)
			{
				wxShape *tmp = l_pcGr->GetPrimitive();
				SP_LOGDEBUG(wxString::Format(wxT("SP_GUI_Canvas::SetEditElement()")
								wxT(" delete control points, shape %p"), tmp));
				if (tmp)
					tmp->ClearPointList(*m_pcControlPoints);
				wxDELETE(m_pcControlPoints);
				ReleaseMouse();
				Refresh();
			}
		}
		static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->ResetTempShapes();
	}

	if (p_pcElement)
	{
		UnSelectAll();
	}

	m_pcEditElement = p_pcElement;
	return m_pcEditElement;
}

unsigned int SP_GUI_Canvas::GetNetnumber()
{
	if (!m_pcView)
		return 0;

	return m_pcView->GetNetnumber();
}

void SP_GUI_Canvas::DrawOutline(double p_nX1, double p_nY1,
		double p_nX2, double p_nY2)
{
	wxClientDC l_cDc(this);
	int l_nX1,l_nY1,l_nX2,l_nY2;
//TODO: check if neccessary in wx3.0
#if defined(__WXMAC__) && wxABI_VERSION < 30000
	CalcScrolledPosition(WXROUND(p_nX1), WXROUND(p_nY1), &l_nX1, &l_nY1);
	CalcScrolledPosition(WXROUND(p_nX2), WXROUND(p_nY2), &l_nX2, &l_nY2);
#else
	DoPrepareDC(l_cDc);
	l_nX1 = WXROUND(p_nX1);
	l_nY1 = WXROUND(p_nY1);
	l_nX2 = WXROUND(p_nX2);
	l_nY2 = WXROUND(p_nY2);
#endif

    wxDCOverlay overlaydc( m_Overlay, &l_cDc);
    overlaydc.Clear();

	wxPen* dottedPen = wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxDOT);
	l_cDc.SetPen(*dottedPen);
    l_cDc.SetBrush( *wxTRANSPARENT_BRUSH );
	l_cDc.DrawRectangle(l_nX1, l_nY1, (l_nX2 - l_nX1), (l_nY2 - l_nY1));
}

bool
SP_GUI_Canvas::DrawOutlineShapes(double p_nOffsetX, double p_nOffsetY)
{
    wxClientDC l_cDC(this);
    DoPrepareDC(l_cDC);

    wxDCOverlay overlaydc( m_Overlay, &l_cDC);
    overlaydc.Clear();

	wxPen* dottedPen = wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxDOT);
    l_cDC.SetPen(*dottedPen);
    l_cDC.SetBrush(*wxTRANSPARENT_BRUSH);

    double l_nWidth, l_nHeight;

    // Draw bounding box for selected shapes
    wxNode* l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
    while (l_pcNode)
    {
        wxShape* l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
        if (l_pcShape->Selected() && l_pcShape->IsShown() &&
            !l_pcShape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            l_pcShape->GetBoundingBoxMax(&l_nWidth, &l_nHeight);
            l_pcShape->OnDrawOutline(l_cDC, l_pcShape->GetX() + p_nOffsetX,
                l_pcShape->GetY() + p_nOffsetY, l_nWidth, l_nHeight);
        }
        l_pcNode = l_pcNode->GetNext();
    }
    return TRUE;
}

void SP_GUI_Canvas::OnLeftClick(double p_nX, double p_nY, int p_nKeys)
{
	if (!m_pcEditElement || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
	{
		UnSelectAll(p_nKeys);
	}
	else
	{
		Snap(&p_nX, &p_nY);

		Modify(TRUE);

		switch (m_pcEditElement->GetElementType())
		{
		case SP_ELEMENT_NODECLASS:
			static_cast<SP_DS_Nodeclass*>(m_pcEditElement)->OnLeftClickOnCanvas(this, p_nX, p_nY,
					p_nKeys);
			break;
		case SP_ELEMENT_EDGECLASS:
			static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->OnLeftClickOnCanvas(this, p_nX, p_nY,
					p_nKeys);
			break;
		case SP_ELEMENT_METADATACLASS:
			static_cast<SP_DS_Metadataclass*>(m_pcEditElement)->OnLeftClickOnCanvas(this, p_nX, p_nY,
					p_nKeys);
			break;
		default:
			break;
		}
	}

	RefreshRects();
}

bool SP_GUI_Canvas::DrawAllElements(SP_DS_Graph* p_pcGraph, bool p_bLocalOnly)
{
	if (!p_pcGraph)
		return FALSE;

	return p_pcGraph->ShowOnCanvas(this, p_bLocalOnly);
}

bool SP_GUI_Canvas::OnBeginDragLeftShape(wxShape* p_pcShape, double p_nX,
		double p_nY, int p_nKeys, int p_nAttach)
{
	if (!m_pcEditElement || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return FALSE;

	Modify(TRUE);

	p_pcShape->Select(FALSE);

	if (m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS)
	{
		return static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->
		OnStartElement(SP_Core::Instance()->ResolveExtern(p_pcShape), p_nX, p_nY, p_nKeys, p_nAttach);
	}

	return FALSE;
}

bool SP_GUI_Canvas::OnDragLeftShape(wxShape* p_pcShape, double p_nX,
		double p_nY, int p_nKeys, int p_nAttach)
{
	if (!m_pcEditElement || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return FALSE;

	Modify(TRUE);

	if (m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS)
	{
		return static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->
		OnDrawElement(p_nX, p_nY, p_nKeys, p_nAttach);
	}

	return FALSE;
}

bool SP_GUI_Canvas::OnEndDragLeftShape(wxShape* p_pcShape, double p_nX,
		double p_nY, int p_nKeys, int p_nAttach)
{
	Modify(TRUE);

	int l_nAttach;
	wxShape* l_pcHit = FindShape(p_nX, p_nY, &l_nAttach);
	if (m_pcEditElement && m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS)
	{
		bool l_bReturn= FALSE;
		Snap(&p_nX, &p_nY);

		if (l_pcHit)
		{
			ReleaseMouse();
			l_bReturn = static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->
					OnEndElement(SP_Core::Instance()->ResolveExtern(l_pcHit),
									p_nX, p_nY, p_nKeys, p_nAttach);

			if (m_pcControlPoints)
			{
				p_pcShape->ClearPointList(*m_pcControlPoints);
				wxDELETE(m_pcControlPoints);
				m_pcControlPoints = NULL;
			}

			SP_LOGDEBUG(wxT("SP_GUI_Canvas::OnEndDragLeftShape -- ReleaseMouse();"));
			RefreshRects();
			Refresh();

			return l_bReturn;
		}

		// no hit, so just end the line
		double l_nX = 0, l_nY = 0;
		if (p_pcShape)
			p_pcShape->GetAttachmentPosition(p_nAttach, &l_nX, &l_nY);

		// remember the position of the event
		AddControlPoint(l_nX, l_nY);
		AddControlPoint(p_nX, p_nY);

		return DrawTempEdge(p_nX, p_nY);
	}
	else if (l_pcHit && (l_pcHit != p_pcShape) && (!l_pcHit->Selected()))
	{
		// merge target and selected elements
		return OnEndDragLeftMerge(SP_Core::Instance()->ResolveExtern(l_pcHit));
	}

	return FALSE;
}

bool SP_GUI_Canvas::OnEndDragLeftMerge(SP_Graphic* p_pcShape)
{
	ReleaseMouse();

	if (!p_pcShape || !GetDiagram() || !GetDiagram()->GetShapeList())
		return TRUE;

	if (p_pcShape->GetGraphicType() != SP_GRAPHIC_NODE)
		return FALSE;

	while (p_pcShape && (p_pcShape->GetGraphicType() != SP_GRAPHIC_NODE)
			&& (p_pcShape != p_pcShape->GetGraphicParent()))
	{
		p_pcShape = p_pcShape->GetGraphicParent();
	}

	// will not merge coarse nodes
	if (!p_pcShape || (p_pcShape->GetGraphicState() == SP_STATE_COARSECOARSE))
		return TRUE;

	wxShape* l_pcShape= NULL;
	SP_Graphic* l_pcTarget = p_pcShape;
	SP_Graphic* l_pcGraphic= NULL;
	SP_ListGraphic l_lGraphics;
	wxNode* l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
		if (l_pcShape->Selected())
		{
			l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
			if ((!l_pcGraphic) || (l_pcGraphic->GetGraphicType()
					== SP_GRAPHIC_ATTRIBUTE) ||
			// SP_STATE_COARSEBORDERDOWN can only be target
					(l_pcGraphic->GetGraphicState()
							== SP_STATE_COARSEBORDERDOWN))
			{
				l_pcNode = l_pcNode->GetNext();
				continue;
			}

			if (l_pcGraphic != l_pcTarget && l_pcGraphic->Equals(l_pcTarget,
					SP_EQUALTEST_ALL &~SP_EQUALTEST_GRAPHICSTATE))
			{
				l_lGraphics.push_back(l_pcGraphic);
			}
		}
		l_pcNode = l_pcNode->GetNext();
	}

	return MergeAll(&l_lGraphics, l_pcTarget);
}

bool SP_GUI_Canvas::MergeAll(SP_ListGraphic* p_plShapes,
		SP_Graphic* p_pcTarget)
{
	if (!p_plShapes || (p_plShapes->size() == 0) || !p_pcTarget)
		return TRUE;

	// change selection
	UnSelectAll();
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		SP_Graphic* l_pcGr = (*l_Iter);
		l_pcGr->Select(TRUE);
	}
	p_pcTarget->Select(TRUE);
	Refresh();

	int l_nRet = SP_MESSAGEBOX(wxT("Do you want to merge all selected nodes to one?"), wxT("Merge"), wxYES_NO);
	if (l_nRet == wxNO)
		return TRUE;

	for (l_Iter = p_plShapes->begin(); l_Iter != p_plShapes->end(); ++l_Iter)
	{
		SP_Graphic* l_pcGr = (*l_Iter);
		l_pcGr->Merge(p_pcTarget);
	}

	SP_Core::Instance()->RemoveQueuedElements();

	if (p_pcTarget->GetPrimitive())
	{
		wxClientDC l_cDC(this);
		DoPrepareDC(l_cDC);
		p_pcTarget->GetPrimitive()->Move(l_cDC, p_pcTarget->GetPosX(), p_pcTarget->GetPosY());
	}

	RefreshRects();

	return TRUE;
}

bool SP_GUI_Canvas::OnClickOnShape(wxShape* p_pcShape, double p_nX,
		double p_nY, int p_nKeys, int p_nAttach)
{
	if (!m_pcEditElement || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return FALSE;

	bool l_bReturn= FALSE;
	if (m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS)
	{
		ReleaseMouse();

		// if the edge is only selected, but we are actually not finishing
		// drawing of an edge
		if (!static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->GetStartShape())
			return FALSE;

		Modify(TRUE);

		// last point to store in the list
		AddControlPoint(p_nX, p_nY);

		l_bReturn = static_cast<SP_DS_Edgeclass*>(m_pcEditElement)->
				OnEndElement(SP_Core::Instance()->ResolveExtern(p_pcShape), p_nX, p_nY, p_nKeys, p_nAttach);

		if (m_pcControlPoints)
		{
			p_pcShape->ClearPointList(*m_pcControlPoints);
			wxDELETE(m_pcControlPoints);
			m_pcControlPoints = NULL;
		}
		RefreshRects();
		Refresh();
	}

	return l_bReturn;
}

bool SP_GUI_Canvas::OnStartLine(wxShape* p_pcShape, double p_nX, double p_nY,
		int p_nKeys, int p_nAttach)
{
	if (!p_pcShape)
		return FALSE;

	Modify(TRUE);

	return TRUE;
}

bool SP_GUI_Canvas::OnDrawLine(wxShape* p_pcShape, double p_nX, double p_nY,
		int p_nKeys, int p_nAttach)
{
	wxClientDC l_cDC(this);

	double l_nX = g_nStartX, l_nY = g_nStartY;

	if (p_pcShape)
		p_pcShape->GetAttachmentPosition(p_nAttach, &l_nX, &l_nY);

	int l_nX1,l_nY1,l_nX2,l_nY2;
//TODO: check if neccessary in wx3.0
#if defined(__WXMAC__) && wxABI_VERSION < 30000
	CalcScrolledPosition(WXROUND(l_nX), WXROUND(l_nY), &l_nX1, &l_nY1);
	CalcScrolledPosition(WXROUND(p_nX), WXROUND(p_nY), &l_nX2, &l_nY2);
#else
	DoPrepareDC(l_cDC);
	l_nX1 = WXROUND(l_nX);
	l_nY1 = WXROUND(l_nY);
	l_nX2 = WXROUND(p_nX);
	l_nY2 = WXROUND(p_nY);
#endif

    wxDCOverlay overlaydc( m_Overlay, &l_cDC);
    overlaydc.Clear();

	wxPen* dottedPen = wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxDOT);
	l_cDC.SetPen(*dottedPen);

	l_cDC.DrawLine(l_nX1, l_nY1, l_nX2, l_nY2);

	return TRUE;
}

bool SP_GUI_Canvas::OnEndLine(wxShape* p_pcShape, double p_nX, double p_nY,
		int p_nKeys, int p_nAttach)
{
	Modify(TRUE);

	return TRUE;
}

void SP_GUI_Canvas::OnRightClick(double x, double y, int keys)
{
}

void SP_GUI_Canvas::OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys)
{
	if (m_pcControlPoints || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return;

	UnSelectAll(p_nKeys);

	g_nStartX = p_nX;
	g_nStartY = p_nY;

	DrawOutline(g_nStartX, g_nStartY, p_nX, p_nY);
	CaptureMouse();

	Modify(TRUE);
}

void SP_GUI_Canvas::OnDragLeft(bool p_bDraw, double p_nX, double p_nY,
		int p_nKeys)
{
	if (m_pcControlPoints || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return;

	DrawOutline(g_nStartX, g_nStartY, p_nX, p_nY);
}

void SP_GUI_Canvas::OnEndDragLeft(double p_nX, double p_nY, int p_nKeys)
{
	if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		return;

	if (m_pcControlPoints)
	{
		Snap(&p_nX, &p_nY);
		AddControlPoint(p_nX, p_nY);
		DrawTempEdge(p_nX, p_nY);

		Modify(TRUE);

		return;
	}

	ReleaseMouse();

	wxClientDC l_cDc(this);
	DoPrepareDC(l_cDc);

	// Select all images within the rectangle
	double l_nMinX, l_nMaxX, l_nMinY, l_nMaxY;
	l_nMinX = wxMin(p_nX, g_nStartX);
	l_nMaxX = wxMax(p_nX, g_nStartX);
	l_nMinY = wxMin(p_nY, g_nStartY);
	l_nMaxY = wxMax(p_nY, g_nStartY);

	wxNode* l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
	while (l_pcNode)
	{
		wxShape *l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());

		if (l_pcShape->GetParent() == NULL && !l_pcShape->IsKindOf(CLASSINFO(wxControlPoint)))
		{
			if(l_pcShape->IsKindOf(CLASSINFO(wxLineShape)))
			{
				wxNode* l_pcNode = dynamic_cast<wxLineShape*>(l_pcShape)->GetLineControlPoints()->GetFirst();
				bool l_bIsSelected = false;
				while (l_pcNode)
				{
					wxRealPoint* l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
					double l_nImageX = l_pcPoint->x;
					double l_nImageY = l_pcPoint->y;
					if (l_nImageX >= l_nMinX && l_nImageX <= l_nMaxX && l_nImageY
							>= l_nMinY && l_nImageY <= l_nMaxY)
					{
						l_bIsSelected = true;
					}
					else
					{
						l_bIsSelected = false;
						break;
					}
					l_pcNode = l_pcNode->GetNext();
				}
				if(l_bIsSelected)
					l_pcShape->Select(true, &l_cDc);
			}
			else
			{
				double l_nImageX = l_pcShape->GetX();
				double l_nImageY = l_pcShape->GetY();
				if (l_nImageX >= l_nMinX && l_nImageX <= l_nMaxX && l_nImageY
						>= l_nMinY && l_nImageY <= l_nMaxY)
				{
					l_pcShape->Select(true, &l_cDc);
				}
			}
		}
		l_pcNode = l_pcNode->GetNext();
	}
	RefreshRects();

	Modify(TRUE);
}

void SP_GUI_Canvas::OnBeginDragRight(double x, double y, int keys)
{
}

void SP_GUI_Canvas::OnDragRight(bool draw, double x, double y, int keys)
{
}

void SP_GUI_Canvas::OnEndDragRight(double x, double y, int keys)
{
}

void SP_GUI_Canvas::OnMouseEvent(wxMouseEvent& p_cEvent)
{
	if(p_cEvent.ButtonDown())
	{
		SetFocus();
	}
	double l_nScaleX = GetScaleX();
	double l_nScaleY = GetScaleY();
	if(p_cEvent.GetWheelRotation() > 0)
	{
		if(p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && !p_cEvent.AltDown())
		{
			SetScale(l_nScaleX * 1.10, l_nScaleY * 1.10);
			Refresh();
		}
		else if(!p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && !p_cEvent.AltDown())
		{
#if wxABI_VERSION > 30000
			if(p_cEvent.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			{
				int scroll = GetScrollPos(wxVERTICAL);
				if(scroll > 0)
					Scroll(-1, --scroll);
			}
			else if(p_cEvent.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL)
			{
				Scroll(GetScrollPos(wxHORIZONTAL)+1, -1);
			}
#else
			int scroll = GetScrollPos(wxVERTICAL);
			if(scroll > 0)
				Scroll(-1, --scroll);
#endif
		}

	}
	else if(p_cEvent.GetWheelRotation() < 0)
	{
		if(p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && !p_cEvent.AltDown())
		{
			SetScale(l_nScaleX / 1.10, l_nScaleY / 1.10);
			Refresh();
		}
		else if(!p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && !p_cEvent.AltDown())
		{
#if wxABI_VERSION > 30000
			if(p_cEvent.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			{
				Scroll(-1, GetScrollPos(wxVERTICAL)+1);
			}
			else if(p_cEvent.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL)
			{
				int scroll = GetScrollPos(wxHORIZONTAL);
				if(scroll > 0)
					Scroll(--scroll, -1);
			}
#else
			Scroll(-1, GetScrollPos(wxVERTICAL)+1);
#endif
		}
	}
	if(p_cEvent.Moving())
	{

		if(!p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && !p_cEvent.AltDown())
		{
			SetCursor(wxCURSOR_DEFAULT);
			m_nLastPosX = p_cEvent.GetX();
			m_nLastPosY = p_cEvent.GetY();
		}
		if(!p_cEvent.CmdDown() && !p_cEvent.ShiftDown() && p_cEvent.AltDown())
		{
			SetCursor(wxCURSOR_HAND);
			Scroll(GetScrollPos(wxHORIZONTAL)+(p_cEvent.GetX()-m_nLastPosX), GetScrollPos(wxVERTICAL)+(p_cEvent.GetY()-m_nLastPosY));
			m_nLastPosX = p_cEvent.GetX();
			m_nLastPosY = p_cEvent.GetY();
		}
	}
	if(m_pcEditElement && (m_pcEditElement->GetElementType() == SP_ELEMENT_EDGECLASS && m_pcControlPoints))
	{
		DrawTempEdge(p_cEvent.GetX(), p_cEvent.GetY());
	}
	wxShapeCanvas::OnMouseEvent(p_cEvent);
}

void SP_GUI_Canvas::OnKeyEvent(wxKeyEvent& p_cEvent)
{
/*
	wxString l_sMsg = "SP_GUI_Canvas::OnKeyEvent = ";
	l_sMsg << p_cEvent.GetUnicodeKey();
	SP_LOGDEBUG(l_sMsg);
*/
	if(p_cEvent.ShiftDown())
	{
		//check if are we in the edit mode?
		if (!m_pcControlPoints || !SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
		{
          double offsetX=0,offsetY=0;
		  switch(p_cEvent.GetKeyCode())
	      {
		   case WXK_LEFT:
			           offsetX=-GetDiagram()->GetGridSpacing();
			   	       break;
	       case WXK_RIGHT:
	    	            offsetX=GetDiagram()->GetGridSpacing();

	    	   		    break;
	       case WXK_DOWN:
	    	            offsetY=GetDiagram()->GetGridSpacing();
	    	            break;
	       case WXK_UP:
	    	           offsetY=-GetDiagram()->GetGridSpacing();
	    	   	       break;
	      }

		  if(offsetX!=0 || offsetY!=0)
          {

			  MoveLinePoints(offsetX, offsetY);
			  // maybe enough to hide the moving of every line point from the viewer?
			  Freeze();
			  // do the real moving
			  MoveShapes(offsetX, offsetY);
			  // if we freeze, we need to thaw, too
			  Thaw();

			  Modify(TRUE);
			  RefreshRects();

          }
		}
	}

	p_cEvent.Skip();

}

void SP_GUI_Canvas::AddControlPoint(double p_nX, double p_nY)
{
	if (!m_pcControlPoints)
		m_pcControlPoints = new wxList();

	g_nStartX = p_nX;
	g_nStartY = p_nY;
	m_pcControlPoints->Append((wxObject*)new wxRealPoint(p_nX, p_nY));
}

bool SP_GUI_Canvas::DrawTempEdge(double p_nX, double p_nY)
{
	wxClientDC l_cDC(this);
	DoPrepareDC(l_cDC);

    wxDCOverlay overlaydc( m_Overlay, &l_cDC);
    overlaydc.Clear();

	wxPen* l_cPen = wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxDOT);
	l_cDC.SetPen(*l_cPen);

	int l_nX1,l_nY1,l_nX2,l_nY2;
	wxNode* l_pcNode = m_pcControlPoints->GetFirst();
	wxRealPoint* l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
	l_nX1 = WXROUND(l_pcPoint->x);
	l_nY1 = WXROUND(l_pcPoint->y);
	for (l_pcNode = l_pcNode->GetNext(); l_pcNode; l_pcNode = l_pcNode->GetNext())
	{
		l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
		l_nX2 = WXROUND(l_pcPoint->x);
		l_nY2 = WXROUND(l_pcPoint->y);
		l_cDC.DrawLine(l_nX1, l_nY1, l_nX2, l_nY2);
		l_nX1 = l_nX2;
		l_nY1 = l_nY2;
	}
	l_nX2 = WXROUND(p_nX);
	l_nY2 = WXROUND(p_nY);
	l_cDC.DrawLine(l_nX1, l_nY1, l_nX2, l_nY2);

	return TRUE;
}

void SP_GUI_Canvas::Modify(bool p_bVal)
{
	if (m_pcView)
		m_pcView->Modify(p_bVal);
}

bool
SP_GUI_Canvas::MoveLinePoints(double p_nOffsetX, double p_nOffsetY)
{
    if (!GetDiagram())
        return FALSE;

    // Move line points
    wxNode* l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
    while (l_pcNode)
    {
        wxShape* l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
        // Only move the line point(s) if both ends move too
        if (l_pcShape->IsKindOf(CLASSINFO(wxLineShape)) &&
            dynamic_cast<wxLineShape*>(l_pcShape)->GetTo()->Selected() &&
            dynamic_cast<wxLineShape*>(l_pcShape)->GetFrom()->Selected())
        {
            wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(l_pcShape);

            if (l_pcLineShape->GetLineControlPoints()->GetCount() > 2)
            {
                wxNode *node1 = l_pcLineShape->GetLineControlPoints()->GetFirst();
                while (node1)
                {
                    wxRealPoint *point = (wxRealPoint*)node1->GetData();
                    point->x += p_nOffsetX;
                    point->y += p_nOffsetY;
                    node1 = node1->GetNext();
                }
            }
        }
        l_pcNode = l_pcNode->GetNext();
    }
    return FALSE;
}

bool
SP_GUI_Canvas::MoveShapes(double p_nOffsetX, double p_nOffsetY)
{
    if (!GetDiagram())
        return FALSE;

    wxClientDC l_cDC(this);
    DoPrepareDC(l_cDC);

    // Add selected node shapes, if any
    wxNode* l_pcNode = GetDiagram()->GetShapeList()->GetFirst();
    while (l_pcNode)
    {
        wxShape* l_pcShape = dynamic_cast<wxShape*>(l_pcNode->GetData());
        if (l_pcShape->Selected() &&
            !l_pcShape->IsKindOf(CLASSINFO(wxLineShape)))
        {
            // to get the line attachments right
            l_pcShape->Move(l_cDC,
            l_pcShape->GetX() + p_nOffsetX,
            l_pcShape->GetY() + p_nOffsetY);
        }
        l_pcNode = l_pcNode->GetNext();
    }
    return TRUE;
}



