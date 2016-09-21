//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Circle Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/base/SP_GR_BaseCircle.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_utilities.h"


#include "snoopy.h"

SP_GR_BaseCircle::SP_GR_BaseCircle(double p_nR)
:wxCircleShape(p_nR),
m_pcSavedPen(NULL),
m_pcLastSetPen(NULL)
{
}

SP_GR_BaseCircle::~SP_GR_BaseCircle()
{
}

void
SP_GR_BaseCircle::Select(bool p_bSelect, wxDC* p_pcDC)
{
    wxCircleShape::Select(p_bSelect, p_pcDC);

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic)
    {
        l_pcGraphic->Select(p_bSelect, p_pcDC);
    }
}

void
SP_GR_BaseCircle::MakeControlPoints()
{
    bool l_bFixedX, l_bFixedY;

    if (!GetCanvas())
        return;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
        wxCircleShape::MakeControlPoints();
    else if (!m_pcSavedPen)
    {
        SetShadowMode(SHADOW_RIGHT, false);
    }
}

void
SP_GR_BaseCircle::DeleteControlPoints(wxDC* p_pcDC)
{
    bool l_bFixedX, l_bFixedY;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
        wxCircleShape::DeleteControlPoints(p_pcDC);
    else
    {
        SetShadowMode(SHADOW_NONE, false);
    }
}

void
SP_GR_BaseCircle::ResetControlPoints()
{
    wxCircleShape::ResetControlPoints();
}

void
SP_GR_BaseCircle::OnEndSize(double p_nX, double p_nY)
{
    SetSize(wxMax(p_nX, p_nY), wxMax(p_nX, p_nY));
}
