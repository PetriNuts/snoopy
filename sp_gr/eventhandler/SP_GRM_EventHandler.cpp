//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Event handler baseclass
//////////////////////////////////////////////////////////////////////
#include "sp_gr/eventhandler/SP_GRM_EventHandler.h"
#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_gui/interaction/SP_IA_Manager.h"

#include "sp_core/SP_Core.h"
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

// clicks
#include "sp_ds/extensions/SP_DS_Animation.h"

SP_GRM_EventHandler::SP_GRM_EventHandler(wxShapeEvtHandler* p_pcPrev, wxShape* p_pcShape)
:wxShapeEvtHandler(p_pcPrev, p_pcShape)
{
}

SP_GUI_Canvas*
SP_GRM_EventHandler::UnSelectAll(int p_nKeys)
{
    SP_GUI_Canvas* l_pcCanvas = NULL;

    if (GetShape())
        l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());

    if (p_nKeys == KEY_SHIFT)
        return l_pcCanvas;

    if (l_pcCanvas && GetShape() && !GetShape()->Selected())
    {
        SP_MDI_View* l_pcView = l_pcCanvas->GetView();
        l_pcView->SelectAll(FALSE);
        l_pcCanvas->Refresh();
    }

    return l_pcCanvas;
}

void
SP_GRM_EventHandler::OnLeftClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
	if(SP_Core::Instance()->GetSimulationMode())
		return;

    if (SP_Core::Instance()->GetAnim())
    {
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape());
        if (l_pcGraphic)
            SP_Core::Instance()->GetAnim()->OnLeftClick(l_pcGraphic->GetParent());
        if (GetShape()->GetCanvas())
            dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas())->Refresh();
        return;
    }

    SP_GUI_Canvas* l_pcCanvas = UnSelectAll(p_nKeys);

    // let the canvas be able to vote the dealing with this event
    if (!l_pcCanvas ||
        l_pcCanvas->OnClickOnShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
        return;

    wxClientDC l_cDC(l_pcCanvas);
    l_pcCanvas->DoPrepareDC(l_cDC);

    bool l_bSelected = GetShape()->Selected();

    l_bSelected = !l_bSelected;

    GetShape()->Select(l_bSelected, &l_cDC);

    l_pcCanvas->Refresh(); // Redraw because bits of objects will be missing

		if (GetShape()->IsKindOf(CLASSINFO(wxLineShape))) return;

		//trigger interaction event
		SP_MDI_View* l_pcView = l_pcCanvas->GetView();
		SP_MDI_Doc *l_pcDoc = dynamic_cast<SP_MDI_Doc*>(l_pcView->GetDocument());
		if (l_pcDoc && wxGetApp().GetIAManager()) {
			if (p_nKeys == KEY_CTRL && wxGetApp().GetIAManager()->IAModeEnabled(l_pcDoc->GetGraph())) {
				SP_ListData l_lSource;
				l_lSource.clear();
				l_pcView->FindSelectedElements(l_lSource);
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&l_lSource, SP_IA_EVT_SINGLE));
			}
		}

}

void
SP_GRM_EventHandler::OnLeftDoubleClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape());
    SP_GUI_Canvas* l_pcCanvas = UnSelectAll(p_nKeys);

    // let the canvas be able to veto the dealing with this event
    if (!l_pcGraphic || !l_pcCanvas ||
        l_pcCanvas->OnClickOnShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
        return;

    l_pcGraphic->OnDoubleClick(p_nKeys);

    l_pcCanvas->Refresh();
}

void
SP_GRM_EventHandler::OnRightClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
	if(SP_Core::Instance()->GetSimulationMode())
		return;

    if (SP_Core::Instance()->GetAnim())
    {
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape());
        if (l_pcGraphic)
            SP_Core::Instance()->GetAnim()->OnRightClick(l_pcGraphic->GetParent());
        if (GetShape()->GetCanvas())
            dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas())->Refresh();
        return;
    }

    SP_GUI_Canvas* l_pcCanvas = UnSelectAll(p_nKeys);
    // let the canvas be able to vote the dealing with this event
    if (!l_pcCanvas ||
        l_pcCanvas->OnClickOnShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
        return;

    wxShapeEvtHandler::OnRightClick(p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EventHandler::OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
 //   if (SP_Core::Instance()->GetAnimMode()) return;

    SP_GUI_Canvas* l_pcCanvas = UnSelectAll(p_nKeys);

    // let the canvas be able to vote the dealing with this event
    if (!l_pcCanvas || l_pcCanvas->OnClickOnShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
        return;

    l_pcCanvas->Modify(TRUE);
    GetShape()->Select(TRUE);

    g_nDragStartX = GetShape()->GetX();
    g_nDragStartY = GetShape()->GetY();

    g_nDragOffsetX = g_nDragStartX - p_nX;
    g_nDragOffsetY = g_nDragStartY - p_nY;

    double xx, yy;
    xx = p_nX + g_nDragOffsetX;
    yy = p_nY + g_nDragOffsetY;

    DoSnap(&xx, &yy, p_nKeys);

    double offsetX = xx - g_nDragStartX;
    double offsetY = yy - g_nDragStartY;

    l_pcCanvas->DrawOutlineShapes(offsetX, offsetY);

    l_pcCanvas->CaptureMouse();
}

void
SP_GRM_EventHandler::OnDragLeft(bool p_bDraw, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
//    if (SP_Core::Instance()->GetAnimMode()) return;

    double xx, yy;
    xx = p_nX + g_nDragOffsetX;
    yy = p_nY + g_nDragOffsetY;

    DoSnap(&xx, &yy, p_nKeys);

    double offsetX = xx - g_nDragStartX;
    double offsetY = yy - g_nDragStartY;

    SP_GUI_Canvas *l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    l_pcCanvas->DrawOutlineShapes(offsetX, offsetY);
}

void
SP_GRM_EventHandler::OnEndDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
//    if (SP_Core::Instance()->GetAnimMode()) return;

    SP_GUI_Canvas *l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    l_pcCanvas->ReleaseMouse();

    double xx = p_nX + g_nDragOffsetX;
    double yy = p_nY + g_nDragOffsetY;

    DoSnap(&xx, &yy, p_nKeys);

    double offsetX = xx - g_nDragStartX;
    double offsetY = yy - g_nDragStartY;

    // do the real moving
    l_pcCanvas->MoveShapes(offsetX, offsetY);

    l_pcCanvas->Modify(TRUE);
    l_pcCanvas->Refresh();
}

void
SP_GRM_EventHandler::OnBeginDragRight(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnBeginDragRight(p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EventHandler::OnDragRight(bool p_bDraw, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnDragRight(p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EventHandler::OnEndDragRight(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnEndDragRight(p_nX, p_nY, p_nKeys, p_nAttach);
}


void
SP_GRM_EventHandler::OnSizingBeginDragLeft(wxControlPoint* p_pcCP,
                                           double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnSizingBeginDragLeft(p_pcCP, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EventHandler::OnSizingDragLeft(wxControlPoint* p_pcCP, bool p_bDraw,
                                      double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnSizingDragLeft(p_pcCP, p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_EventHandler::OnSizingEndDragLeft(wxControlPoint* p_pcCP,
                                         double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnSizingEndDragLeft(p_pcCP, p_nX, p_nY, p_nKeys, p_nAttach);
}


void
SP_GRM_EventHandler::OnSize(double p_nX, double p_nY)
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
        return;

    wxShapeEvtHandler::OnSize(p_nX, p_nY);
}


bool
SP_GRM_EventHandler::CheckSensitivity(int p_nOperation)
{
    if (!GetShape())
        return FALSE;

    if ((GetShape()->GetSensitivityFilter() & p_nOperation) != p_nOperation)
        return FALSE;

    return TRUE;
}

void
SP_GRM_EventHandler::DoSnap(double* p_pnX, double* p_pnY, int p_nKeys)
{
    if (GetShape() &&
        GetShape()->GetCanvas())
    {
        GetShape()->GetCanvas()->Snap(p_pnX, p_pnY);
    }
}

