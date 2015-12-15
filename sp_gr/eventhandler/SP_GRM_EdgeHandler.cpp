//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Edge handler Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/eventhandler/SP_GRM_EdgeHandler.h"
#include "sp_gr/base/SP_GR_BaseEdge.h"
#include "sp_gr/SP_GR_Edge.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include "snoopy.h"
#include "sp_defines.h"
#include "sp_gui/interaction/SP_IA_Manager.h"

#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

SP_GRM_EdgeHandler::SP_GRM_EdgeHandler(wxShapeEvtHandler* p_pcPrev, wxShape* p_pcShape)
:SP_GRM_EventHandler(p_pcPrev, p_pcShape)
{
}

void
SP_GRM_EdgeHandler::OnLeftClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    if (p_nKeys == KEY_CTRL)
    {
			SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
			SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcCanvas->GetView()->GetDocument());
			l_pcDoc = l_pcDoc->GetParentDoc();


			if (GetShape() && GetShape()->IsKindOf(CLASSINFO(wxLineShape)))
			{
				DoSnap(&p_nX, &p_nY, p_nKeys);
				SP_GUI_Canvas* l_pcCanvas = UnSelectAll(0);
				l_pcCanvas->Modify(TRUE);

				GetShape()->Select();
				dynamic_cast<SP_GR_BaseEdge*>(GetShape())->InsertControlPoint(p_nX, p_nY);
				GetShape()->Select(FALSE);
				SP_Core::Instance()->ResolveExtern(GetShape())->Update();
				l_pcCanvas->Refresh();
				GetShape()->Select();
				return;
			}
    }

    SP_GRM_EventHandler::OnLeftClick(p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EdgeHandler::OnSizingBeginDragLeft(wxControlPoint* p_pcCP,
                                          double p_nX, double p_nY,
                                          int p_nKeys, int p_nAttach)
{
    if (!GetShape() || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    if (!l_pcCanvas)
        return;
    l_pcCanvas->Modify(TRUE);

    wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetShape());
    wxLineControlPoint* l_pcCP = dynamic_cast<wxLineControlPoint*>(p_pcCP);

    wxClientDC l_cDC(l_pcCanvas);
    l_pcCanvas->DoPrepareDC(l_cDC);

    wxDCOverlay overlaydc(l_pcCanvas->GetOverlay(), &l_cDC);
    overlaydc.Clear();

    l_pcCP->m_originalPos = *(l_pcCP->m_point);
    l_pcCanvas->Snap(&p_nX, &p_nY);

    l_pcShape->Erase(l_cDC);

    // Redraw start and end objects because we've left holes
    // when erasing the line
    l_pcShape->GetFrom()->OnDraw(l_cDC);
    l_pcShape->GetFrom()->OnDrawContents(l_cDC);
    l_pcShape->GetTo()->OnDraw(l_cDC);
    l_pcShape->GetTo()->OnDrawContents(l_cDC);

    l_pcShape->SetDisableLabel(TRUE);

    l_pcCP->SetX(p_nX); l_pcCP->SetY(p_nY);
    l_pcCP->m_point->x = p_nX; l_pcCP->m_point->y = p_nY;

    wxPen *l_pcOldPen = l_pcShape->GetPen();
    wxBrush *l_pcOldBrush = l_pcShape->GetBrush();

    wxPen l_cPen(wxColour(0, 0, 0), 1, wxPENSTYLE_DOT);
    l_pcShape->SetPen(&l_cPen);
    l_pcShape->SetBrush(wxTRANSPARENT_BRUSH);

    if (l_pcCP->m_type == CONTROL_POINT_LINE)
        l_pcShape->GetEventHandler()->OnMoveLink(l_cDC, FALSE);
    else
        l_pcShape->Draw(l_cDC);

    l_pcShape->SetPen(l_pcOldPen);
    l_pcShape->SetBrush(l_pcOldBrush);
}

void
SP_GRM_EdgeHandler::OnSizingDragLeft(wxControlPoint* p_pcCP, bool p_bDraw,
                                     double p_nX, double p_nY,
                                     int p_nKeys, int p_nAttach)
{
    if (!GetShape() || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxLineControlPoint* l_pcCP = dynamic_cast<wxLineControlPoint*>(p_pcCP);
    wxLineShape *l_pcShape = dynamic_cast<wxLineShape*>(GetShape());

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcShape->GetCanvas());
    if (!l_pcCanvas)
        return;

    wxClientDC l_cDC(l_pcCanvas);
    l_pcCanvas->DoPrepareDC(l_cDC);

    wxDCOverlay overlaydc(l_pcCanvas->GetOverlay(), &l_cDC);
    overlaydc.Clear();

    wxPen l_cPen(wxColour(0, 0, 0), 1, wxPENSTYLE_DOT);
    l_cDC.SetPen(l_cPen);
    l_cDC.SetBrush((* wxTRANSPARENT_BRUSH));

    l_pcCanvas->Snap(&p_nX, &p_nY);

    l_pcCP->SetX(p_nX); l_pcCP->SetY(p_nY);
    l_pcCP->m_point->x = p_nX; l_pcCP->m_point->y = p_nY;

    wxPen *l_pcOldPen = l_pcShape->GetPen();
    wxBrush *l_pcOldBrush = l_pcShape->GetBrush();

    l_pcShape->SetPen(& l_cPen);
    l_pcShape->SetBrush(wxTRANSPARENT_BRUSH);

    if (l_pcCP->m_type == CONTROL_POINT_LINE)
        l_pcShape->GetEventHandler()->OnMoveLink(l_cDC, FALSE);
    else
        l_pcShape->Draw(l_cDC);

    l_pcShape->SetPen(l_pcOldPen);
    l_pcShape->SetBrush(l_pcOldBrush);
}

void
SP_GRM_EdgeHandler::OnSizingEndDragLeft(wxControlPoint* p_pcCP,
                                          double p_nX, double p_nY,
                                          int p_nKeys, int p_nAttach)
{
    if (!GetShape() || !p_pcCP || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxLineControlPoint* l_pcCP = dynamic_cast<wxLineControlPoint*>(p_pcCP);
    wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetShape());
    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcShape->GetCanvas());
    if (!l_pcCanvas)
        return;

    l_pcCanvas->Modify(TRUE);

    if (l_pcCP->m_type != CONTROL_POINT_LINE)
    {
        // moved an endpoint of the line
        OnMoveLineEndPoint(l_pcCP, p_nX, p_nY);

        wxShape* l_pcSource = l_pcShape->GetFrom();
        wxShape* l_pcTarget = l_pcShape->GetTo();

        l_pcCanvas->MoveShape(l_pcSource, 0, 0);
        l_pcCanvas->MoveShape(l_pcTarget, 0, 0);
    }
    else
    {
        // maybe we have to merge some line nodes
        OnMoveMiddleControlPoint(l_pcCP, p_nX, p_nY);
        wxShapeEvtHandler::OnSizingEndDragLeft(p_pcCP, p_nX, p_nY, p_nKeys, p_nAttach);
    }

    l_pcCanvas->Refresh();
}

bool
SP_GRM_EdgeHandler::OnMoveMiddleControlPoint(wxLineControlPoint* p_pcCP, double p_nX, double p_nY)
{
    if (!GetShape() || !p_pcCP || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return FALSE;

    wxRealPoint* l_pcTempPoint = NULL;
    wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetShape());
    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcShape->GetCanvas());
    if (!l_pcCanvas)
        return FALSE;

    bool l_bHit = FALSE;
    int l_nAttach;
    wxShape* l_pcHit = l_pcCanvas->FindShape(p_nX, p_nY, &l_nAttach, CLASSINFO(wxLineControlPoint), p_pcCP);
    wxLineControlPoint* l_pcCP = (wxLineControlPoint*)l_pcHit;

    l_pcCanvas->Modify(TRUE);

    if (!l_pcCP ||
        !l_pcShape->GetLineControlPoints()->Member((wxObject*)l_pcCP->m_point))
        return FALSE;

    wxNode *l_pcNode = l_pcShape->GetLineControlPoints()->GetFirst();
    while (l_pcNode)
    {
        l_pcTempPoint = (wxRealPoint*)l_pcNode->GetData();
        if (*l_pcTempPoint == *(l_pcCP->m_point))
        {
            l_bHit = TRUE;
            break;
        }

        l_pcNode = l_pcNode->GetNext();
    }

    if (l_bHit)
    {
        l_pcShape->GetLineControlPoints()->DeleteNode(l_pcNode);
        wxDELETE(l_pcTempPoint);
        l_pcShape->Select(FALSE);
        l_pcShape->Select(TRUE);
        return TRUE;
    }

    return FALSE;
}

bool
SP_GRM_EdgeHandler::OnMoveLineEndPoint(wxLineControlPoint* p_pcCP, double p_nX, double p_nY)
{
    if (!GetShape() || !p_pcCP || SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return FALSE;

    wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetShape());
    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(l_pcShape->GetCanvas());
    if (!l_pcCanvas)
        return FALSE;

    l_pcCanvas->Modify(TRUE);

    int l_nAttach;
    // we don't want the controlpoint
    wxShape* l_pcHit = l_pcCanvas->FindShape(p_nX, p_nY, &l_nAttach, NULL, p_pcCP);

    if (l_pcHit)
    {
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcShape);
        if (!l_pcGraphic)
            return FALSE;

        // only edges can connect elements, and so they can
        // only be rearranged
        if (l_pcGraphic->GetGraphicType() != SP_GRAPHIC_EDGE)
            return FALSE;

	   //Disabled by rr: TODO correct this
       // return dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->OnConsumeEdge(l_pcHit, p_pcCP->m_type);
    }

    return FALSE;
}

