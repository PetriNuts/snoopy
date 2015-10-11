//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Edge Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/base/SP_GR_BaseEdge.h"
#include "sp_gr/eventhandler/SP_GRM_EdgeHandler.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"

#include "sp_core/base/SP_Graphic.h"

#include "sp_utilities.h"

#include <wx/wx.h>
#include <wx/ogl/basicp.h>
#include <wx/ogl/linesp.h>

IMPLEMENT_DYNAMIC_CLASS(SP_GR_BaseEdge, wxLineShape)

SP_GR_BaseEdge::SP_GR_BaseEdge()
:wxLineShape(),
m_pcSavedPen(NULL)
{
    Initialise();
    SetEventHandler(new SP_GRM_EdgeHandler(this, this));
}

SP_GR_BaseEdge::~SP_GR_BaseEdge()
{
}

void
SP_GR_BaseEdge::Select(bool p_bSelect, wxDC* p_pcDC)
{
    wxLineShape::Select(p_bSelect, p_pcDC);

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic)
    {
        l_pcGraphic->Select(p_bSelect, p_pcDC);
    }
}

void
SP_GR_BaseEdge::GetBoundingBoxMin(double* p_pnWidth, double* p_pnHeight)
{
    if (!p_pnWidth || !p_pnHeight || !m_lineControlPoints)
        return;

    // call original
    // wxLineShape::GetBoundingBoxMin
    double l_nX1 = 10000;
    double l_nY1 = 10000;
    double l_nX2 = -10000;
    double l_nY2 = -10000;

    wxNode *l_pcNode = m_lineControlPoints->GetFirst();
    while (l_pcNode)
    {
        wxRealPoint *l_pcPoint = (wxRealPoint *)l_pcNode->GetData();

        if (l_pcPoint->x < l_nX1) l_nX1 = l_pcPoint->x;
        if (l_pcPoint->y < l_nY1) l_nY1 = l_pcPoint->y;
        if (l_pcPoint->x > l_nX2) l_nX2 = l_pcPoint->x;
        if (l_pcPoint->y > l_nY2) l_nY2 = l_pcPoint->y;

        l_pcNode = l_pcNode->GetNext();
    }

    *p_pnWidth = (double)(l_nX2 - l_nX1);
    *p_pnHeight = (double)(l_nY2 - l_nY1);
    // end code of
    // wxLineShape::GetBoundingBoxMin

    // Well, the protected wxShape::m_xpos and wxShape::m_ypos
    // are more or less useless for line shapes, but the are used
    // to calculate the refresh rects.
    // So, here's the best place to set them to a proper value
    // to act as intended, while calculating the rectangles.
    m_xpos = (l_nX1 + *p_pnWidth / 2.0);
    m_ypos = (l_nY1 + *p_pnHeight / 2.0);
}


bool
SP_GR_BaseEdge::InsertControlPoint(double p_nX, double p_nY)
{
    // we are sure X and Y are on a line segment
    // otherwise, we wouldn't have entered here
    // from OnLeftClick ^^
    wxList l_lOldPoints;
    bool l_bInserted = FALSE;
    double l_nX1 = 0.0, l_nY1 = 0.0, l_nX2 = 0.0, l_nY2 = 0.0;
    // copy the values of the existing points over and insert the new one
    wxNode* l_pcNode = GetLineControlPoints()->GetFirst();
    while (l_pcNode)
    {
        wxRealPoint* l_pcPoint1 = (wxRealPoint*)l_pcNode->GetData();
        l_lOldPoints.Append((wxObject*)new wxRealPoint(l_pcPoint1->x, l_pcPoint1->y));

        if (!l_bInserted && l_pcNode->GetNext())
        {
            wxRealPoint* l_pcPoint2 = (wxRealPoint*)l_pcNode->GetNext()->GetData();
            l_nX1 = l_pcPoint1->x - p_nX;
            l_nY1 = l_pcPoint1->y - p_nY;
            l_nX2 = l_pcPoint2->x - p_nX;
            l_nY2 = l_pcPoint2->y - p_nY;
            if (SP_RectAroundOrigin(l_nX1, l_nY1, l_nX2, l_nY2))
            {
                l_bInserted = TRUE;
                l_lOldPoints.Append((wxObject*)new wxRealPoint(p_nX, p_nY));
            }

        }

        l_pcNode = l_pcNode->GetNext();
    }

    // calls OGL method to remove all points and
    // insert the new ones
    MakeLineControlPoints(l_lOldPoints.GetCount());

    // reposition the new nodes
    l_pcNode = GetLineControlPoints()->GetFirst();
    wxNode* l_pcNewNode = l_lOldPoints.GetFirst();
    while (l_pcNode && l_pcNewNode)
    {
        wxRealPoint* l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
        wxRealPoint* l_pcNewPoint = (wxRealPoint*)l_pcNewNode->GetData();

        l_pcPoint->x = l_pcNewPoint->x;
        l_pcPoint->y = l_pcNewPoint->y;

        l_pcNode = l_pcNode->GetNext();
        l_pcNewNode = l_pcNewNode->GetNext();
        // is this safe?
        wxDELETE(l_pcNewPoint);
    }

    static_cast<SP_GUI_Canvas*>(GetCanvas())->MoveShape(GetFrom(), 0, 0);
    static_cast<SP_GUI_Canvas*>(GetCanvas())->MoveShape(GetTo(), 0, 0);

    return TRUE;
}
