//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Circle Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/base/SP_GR_BaseRectangle.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_defines.h"

#include "snoopy.h"

SP_GR_BaseRectangle::SP_GR_BaseRectangle(double p_nW, double p_nH)
:wxRectangleShape(p_nW, p_nH),
m_pcSavedPen(NULL),
m_pcLastSetPen(NULL)
{
}

SP_GR_BaseRectangle::~SP_GR_BaseRectangle()
{
}

void
SP_GR_BaseRectangle::Select(bool p_bSelect, wxDC* p_pcDC)
{
    wxRectangleShape::Select(p_bSelect, p_pcDC);

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic)
    {
        l_pcGraphic->Select(p_bSelect, p_pcDC);
    }
}

void
SP_GR_BaseRectangle::MakeControlPoints()
{
    bool l_bFixedX, l_bFixedY;

    if (!GetCanvas())
        return;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
        wxRectangleShape::MakeControlPoints();
    else
    {
        SetShadowMode(SHADOW_RIGHT, false);
    }
}

void
SP_GR_BaseRectangle::DeleteControlPoints(wxDC* p_pcDC)
{
    bool l_bFixedX, l_bFixedY;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
        wxRectangleShape::DeleteControlPoints(p_pcDC);
    else
    {
        SetShadowMode(SHADOW_NONE, false);
    }
}

void
SP_GR_BaseRectangle::ResetControlPoints()
{
    wxRectangleShape::ResetControlPoints();
}

