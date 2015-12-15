//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Shape Handler Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_defines.h"

SP_GRM_ShapeHandler::SP_GRM_ShapeHandler(wxShapeEvtHandler* p_pcPrev, wxShape* p_pcShape)
:SP_GRM_EventHandler(p_pcPrev, p_pcShape)
{
}

void
SP_GRM_ShapeHandler::OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (!GetShape())
        return;

    SP_GUI_Canvas* l_pcCanvas = UnSelectAll(p_nKeys);//dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    if (l_pcCanvas &&
        l_pcCanvas->OnBeginDragLeftShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
    {
        l_pcCanvas->CaptureMouse();
        return;
    }

    SP_GRM_EventHandler::OnBeginDragLeft(p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_ShapeHandler::OnDragLeft(bool p_bDraw, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (!GetShape())
        return;

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    if (l_pcCanvas &&
        l_pcCanvas->OnDragLeftShape(GetShape(), p_nX, p_nY, p_nKeys, p_nAttach))
    {
        return;
    }

    SP_GRM_EventHandler::OnDragLeft(p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_ShapeHandler::OnEndDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    if (!GetShape())
        return;

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(GetShape()->GetCanvas());
    if (l_pcCanvas)
    {
        if (l_pcCanvas->OnEndDragLeftShape(GetShape(),
            p_nX, p_nY, p_nKeys, p_nAttach))
        {
            // unfortunately, OnEndDragLeft doesn't return anything, so we
            // can't pass the result of the canvas handler directly
            l_pcCanvas->Refresh();
            return;
        }
    }

    SP_GRM_EventHandler::OnEndDragLeft(p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_ShapeHandler::OnEndSize(double p_nX, double p_nY)
{
    if (!GetShape())
        return;

    GetShape()->OnEndSize(p_nX, p_nY);
}
