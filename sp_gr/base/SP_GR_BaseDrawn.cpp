//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Drawn Shape Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_defines.h"

#include "snoopy.h"

SP_GR_BaseDrawn::SP_GR_BaseDrawn()
:wxDrawnShape(),
m_pcSavedPen(NULL),
m_pcLastSetPen(NULL),
m_bHideDimensions(FALSE)
{
}

SP_GR_BaseDrawn::~SP_GR_BaseDrawn()
{
}

void
SP_GR_BaseDrawn::Select(bool p_bSelect, wxDC* p_pcDC)
{
    wxDrawnShape::Select(p_bSelect, p_pcDC);

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic)
    {
        l_pcGraphic->Select(p_bSelect, p_pcDC);
        l_pcGraphic->Update(TRUE);
    }
}

void
SP_GR_BaseDrawn::MakeControlPoints()
{
    bool l_bFixedX, l_bFixedY;

    if (m_bHideDimensions)
        return;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
	{
		if(GetCanvas())
	        wxDrawnShape::MakeControlPoints();
	}
    else
    {
        SetShadowMode(SHADOW_RIGHT, false);
    }
}

void
SP_GR_BaseDrawn::DeleteControlPoints(wxDC* p_pcDC)
{
    bool l_bFixedX, l_bFixedY;

    if (m_bHideDimensions)
        return;

    GetFixedSize(&l_bFixedX, &l_bFixedY);

    // if resizable in any direction, show standard control points
    if (!l_bFixedX || !l_bFixedY)
        wxDrawnShape::DeleteControlPoints(p_pcDC);
    else
    {
        SetShadowMode(SHADOW_NONE, false);
    }
}

void
SP_GR_BaseDrawn::ResetControlPoints()
{
    if (m_bHideDimensions)
        return;

    wxDrawnShape::ResetControlPoints();
}

bool
SP_GR_BaseDrawn::HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance)
{
    if (!m_bHideDimensions)
        return wxDrawnShape::HitTest(p_nX, p_nY, p_pnAttach, p_pnDistance);

    return FALSE;
}

void
SP_GR_BaseDrawn::GetBoundingBoxMin(double* p_pnWidth, double* p_pnHeight)
{
    wxDrawnShape::GetBoundingBoxMin(p_pnWidth, p_pnHeight);
}

bool
SP_GR_BaseDrawn::CalculateDimensions(const wxString& p_sVal)
{
    if (!GetCanvas())
        return FALSE;

    wxClientDC l_cDC(GetCanvas());
    GetCanvas()->DoPrepareDC(l_cDC);

    l_cDC.SetFont(*(GetFont()));

    wxSize l_cSize = l_cDC.GetMultiLineTextExtent(p_sVal);
    SetSize(l_cSize.GetX(), l_cSize.GetY());

    return TRUE;
}
