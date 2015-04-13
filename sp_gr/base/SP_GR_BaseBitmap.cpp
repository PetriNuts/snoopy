//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Drawn Shape Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/base/SP_GR_BaseBitmap.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gr/eventhandler/SP_GRM_AttributeHandler.h"

#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_defines.h"

#include "snoopy.h"

SP_GR_BaseBitmap::SP_GR_BaseBitmap()
:wxBitmapShape(),
 m_bHideDimensions(FALSE)
{
	SetEventHandler(new SP_GRM_AttributeHandler(this, this));
}

SP_GR_BaseBitmap::~SP_GR_BaseBitmap()
{
}

void
SP_GR_BaseBitmap::Select(bool p_bSelect, wxDC* p_pcDC)
{
	wxBitmapShape::Select(p_bSelect, p_pcDC);

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic)
    {
        l_pcGraphic->Select(p_bSelect, p_pcDC);
    }
}

void
SP_GR_BaseBitmap::MakeControlPoints()
{
    if (m_bHideDimensions)
        return;

    if(GetCanvas())
		wxBitmapShape::MakeControlPoints();
}

void
SP_GR_BaseBitmap::DeleteControlPoints(wxDC* p_pcDC)
{
    if (m_bHideDimensions)
        return;

   	wxBitmapShape::DeleteControlPoints(p_pcDC);
}

void
SP_GR_BaseBitmap::ResetControlPoints()
{
    if (m_bHideDimensions)
        return;

	wxBitmapShape::ResetControlPoints();
}

void
SP_GR_BaseBitmap::OnDrawOutline(wxDC& p_cDC, double p_nX, double p_nY, double p_nW, double p_nH)
{
    if (m_bHideDimensions)
        return;

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic &&
        !l_pcGraphic->GetShow())
    {
        return;
    }
    wxBitmapShape::OnDrawOutline(p_cDC, p_nX, p_nY, p_nW, p_nH);
}

bool
SP_GR_BaseBitmap::HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance)
{
    if (!m_bHideDimensions)
        return wxBitmapShape::HitTest(p_nX, p_nY, p_pnAttach, p_pnDistance);

    return FALSE;
}
