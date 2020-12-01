//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

SP_GR_MarkAnimator::SP_GR_MarkAnimator():
SP_GR_Animator(),
m_pcGrParent(NULL)
{
    m_pcPrimitive = new SP_GR_BaseCircle(10);
    m_pcPrimitive->SetBrush(wxRED_BRUSH);
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive, m_pcPrimitive));
    // no drag and drop, no select, nothing
    m_pcPrimitive->SetSensitivityFilter(0);
}

SP_GR_MarkAnimator::~SP_GR_MarkAnimator()
{
    RemoveFromCanvas();

    if (m_pcPrimitive)
        wxDELETE(m_pcPrimitive);

    m_pcPrimitive = NULL;
    ClearPath();
}

bool
SP_GR_MarkAnimator::SetPath(SP_Graphic* p_pcPath, unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!p_pcPath || p_pcPath->GetGraphicType() != SP_GRAPHIC_EDGE)
        return FALSE;

    m_pcGrParent = p_pcPath;
    bool l_bReturn = TRUE;
    l_bReturn &= SetNetnumber(p_pcPath->GetNetnumber());
	if(l_bReturn)
	    l_bReturn &= CalculatePath(dynamic_cast<SP_GR_Edge*>(p_pcPath), p_nSteps, p_nDir);
	if(l_bReturn)
		if ((m_lWaypoints.size()) == 0)  return l_bReturn;//george bugfix
	    l_bReturn &= SetPosXY((*m_lWaypoints.begin())->x, (*m_lWaypoints.begin())->y);
    return l_bReturn;
}

bool
SP_GR_MarkAnimator::CalculatePath(SP_GR_Edge* p_pcEdge, unsigned int p_nSteps, SP_DIRECTION_T p_nDir)
{
    if (!p_pcEdge || !p_pcEdge->GetPrimitive())
        return FALSE;

    wxLineShape* l_pcEdge = dynamic_cast<wxLineShape*>(p_pcEdge->GetPrimitive());
    double l_nLength = p_pcEdge->GetLength();

    if (l_nLength == 0.0)
        return FALSE;

    ClearPath();
    // ammount of length per line segment
    double l_nPartition;
    // number of steps per line segment, counter
    int l_nPartSteps, i;
    double l_nStepX, l_nStepY;
    wxNode* l_pcNode1 = l_pcEdge->GetLineControlPoints()->GetFirst();
    wxNode* l_pcNode2 = l_pcNode1->GetNext();
    wxRealPoint* l_pcPoint1, *l_pcPoint2;
    while (l_pcNode1 && l_pcNode2)
    {
        l_pcPoint1 = (wxRealPoint*)l_pcNode1->GetData();
        l_pcPoint2 = (wxRealPoint*)l_pcNode2->GetData();

        l_nPartition = SP_CalculateDistance(l_pcPoint1, l_pcPoint2) / l_nLength;
        l_nPartSteps = (int) (p_nSteps * l_nPartition);
        l_nStepX = (l_pcPoint2->x - l_pcPoint1->x) / (double)l_nPartSteps;
        l_nStepY = (l_pcPoint2->y - l_pcPoint1->y) / (double)l_nPartSteps;
        for (i = 0; i < l_nPartSteps; i++)
		{
			//this separation is necessary because 'Step(unsigned int p_nStep)' doesn't rely on given step parameter
			//instead it just walks the calculated path, advancing once every time it is called
			if (p_nDir == FORWARD)
			{
				m_lWaypoints.push_back(new wxRealPoint(l_pcPoint1->x + l_nStepX * i, l_pcPoint1->y + l_nStepY * i));
			} else
			{
				m_lWaypoints.push_front(new wxRealPoint(l_pcPoint1->x + l_nStepX * i, l_pcPoint1->y + l_nStepY * i));
			}
		}
        l_pcNode1 = l_pcNode2;
        l_pcNode2 = l_pcNode2->GetNext();
    }

    m_itPath = m_lWaypoints.begin();

    return TRUE;
}

SP_Graphic*
SP_GR_MarkAnimator::Clone(SP_Data* p_pcParent)
{
    return new SP_GR_MarkAnimator();
}

// animate
bool
SP_GR_MarkAnimator::PreStep(unsigned int p_nSteps)
{
    return TRUE;
}

bool
SP_GR_MarkAnimator::Step(unsigned int p_nStep)
{
    if (m_itPath == m_lWaypoints.end())
        return TRUE;

    AddToCanvas(NULL);

    m_pcPrimitive->SetX((*m_itPath)->x);
    m_pcPrimitive->SetY((*m_itPath)->y);

    m_itPath++;

    return TRUE;
}

bool
SP_GR_MarkAnimator::PostStep()
{
    return TRUE;
}

void
SP_GR_MarkAnimator::ClearPath()
{
    list<wxRealPoint*>::iterator l_Iter;
    for (l_Iter = m_lWaypoints.begin(); l_Iter != m_lWaypoints.end(); l_Iter++)
    {
        wxRealPoint* l_pcPoint = (*l_Iter);
        wxDELETE(l_pcPoint);
    }
	m_lWaypoints.clear();
}

bool
SP_GR_MarkAnimator::AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX, double p_nY, int p_nKeys)
{
    if (SP_Graphic::GetCanvas())
        return TRUE;

    if (!p_pcCanvas && m_pcGrParent)
        p_pcCanvas = m_pcGrParent->GetCanvas();

    // just wouldn't draw me, but that's no BAD state at all
    if (!p_pcCanvas || (p_pcCanvas->GetNetnumber() != GetNetnumber()))
        return TRUE;

    if (!GetPrimitive()) return FALSE;

    if (GetPrimitive()->GetCanvas())
        return TRUE;

    if ((p_nX != -1) && (p_nY != -1))
        SetPosXY(p_nX, p_nY);

    p_pcCanvas->AddShape(GetPrimitive());
    GetPrimitive()->Show(TRUE);

    return TRUE;
}

SP_GUI_Canvas*
SP_GR_MarkAnimator::GetCanvas()
{
    if (SP_Graphic::GetCanvas())
        return SP_Graphic::GetCanvas();

    if (m_pcGrParent)
        return m_pcGrParent->GetCanvas();

    return NULL;
}
