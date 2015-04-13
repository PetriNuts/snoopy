//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Attribute Event Handler
//////////////////////////////////////////////////////////////////////
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include "sp_core/SP_Core.h"

#define UPWARD4(Func, P1, P2, P3, P4) \
    if (!GetShape()) \
        return; \
 \
    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape()); \
 \
    if (!l_pcGraphic || \
        !l_pcGraphic->GetGraphicParent() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()) \
        return; \
 \
    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->Func(P1, P2, P3, P4);
//    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->##Func##(P1, P2, P3, P4);

#define UPWARD5(Func, P1, P2, P3, P4, P5) \
    if (!GetShape()) \
        return; \
 \
    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape()); \
 \
    if (!l_pcGraphic || \
        !l_pcGraphic->GetGraphicParent() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()) \
        return; \
 \
    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->Func(P1, P2, P3, P4, P5);
//    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->##Func##(P1, P2, P3, P4, P5);

#define UPWARD6(Func, P1, P2, P3, P4, P5, P6) \
    if (!GetShape()) \
        return; \
 \
    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape()); \
 \
    if (!l_pcGraphic || \
        !l_pcGraphic->GetGraphicParent() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive() || \
        !l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()) \
        return; \
 \
    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->Func(P1, P2, P3, P4, P5, P6);
//    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->##Func##(P1, P2, P3, P4, P5, P6);

SP_GRM_UpwardHandler::SP_GRM_UpwardHandler(wxShapeEvtHandler* p_pcPrev, wxShape* p_pcShape)
:SP_GRM_EventHandler(p_pcPrev, p_pcShape)
{
}

void
SP_GRM_UpwardHandler::OnLeftClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnLeftClick, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnLeftDoubleClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnLeftDoubleClick, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnRightClick(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnRightClick, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnBeginDragLeft, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnDragLeft(bool p_bDraw, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD5(OnDragLeft, p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnEndDragLeft(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnEndDragLeft, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnBeginDragRight(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnBeginDragRight, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnDragRight(bool p_bDraw, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD5(OnDragRight, p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnEndDragRight(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD4(OnEndDragRight, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnSizingBeginDragLeft(wxControlPoint* p_pcCP, 
                                            double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD5(OnSizingBeginDragLeft, p_pcCP, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnSizingDragLeft(wxControlPoint* p_pcCP, bool p_bDraw,
                                      double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD6(OnSizingDragLeft, p_pcCP, p_bDraw, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnSizingEndDragLeft(wxControlPoint* p_pcCP, 
                                         double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    UPWARD5(OnSizingEndDragLeft, p_pcCP, p_nX, p_nY, p_nKeys, p_nAttach);
}

void
SP_GRM_UpwardHandler::OnSize(double p_nX, double p_nY)
{
    if (!GetShape())
        return;

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(GetShape());

    if (!l_pcGraphic ||
        !l_pcGraphic->GetGraphicParent() ||
        !l_pcGraphic->GetGraphicParent()->GetPrimitive())
        return;

    l_pcGraphic->GetGraphicParent()->GetPrimitive()->GetEventHandler()->OnSize(p_nX, p_nY);
}
