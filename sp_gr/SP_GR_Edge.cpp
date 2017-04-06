//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Node-Base Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Elements.h"
//#include "snoopy.h"

// cos and sin
#include <math.h>

#include "snoopy.h"

SP_GR_Edge::SP_GR_Edge(SP_DS_Edge* p_pcDataParent) :
	SP_Graphic(SP_GRAPHIC_EDGE), m_pcSource(nullptr), m_pcTarget(nullptr)
{
	SetDataParent(p_pcDataParent);
}

SP_GR_Edge::~SP_GR_Edge()
{
}

bool SP_GR_Edge::CloneBase(SP_Graphic* p_pcCopy)
{
	if (!p_pcCopy)
		return FALSE;

	return SP_Graphic::CloneBase(p_pcCopy);
}

bool SP_GR_Edge::Update(bool p_bLocalOnly)
{
	CHECK_POINTER(GetPrimitive(), return FALSE);

	SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
	if(l_pcDoc)
	{
		bool l_bDrawSpline = false;
		wxLineShape* l_pcLineShape = dynamic_cast<wxLineShape*>(GetPrimitive());
		if(l_pcLineShape->GetLineControlPoints() &&
			l_pcLineShape->GetLineControlPoints()->GetCount() > 2)
		{
			wxString l_sNetclass = l_pcDoc->GetNetclassName();
			l_bDrawSpline = wxGetApp().GetElementPrefs()->GetEdgeSpline(l_sNetclass);
		}
		l_pcLineShape->SetSpline(l_bDrawSpline);
	}

	if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
        auto l_Pen = wxThePenList->FindOrCreatePen(
                        wxGetApp().GetCanvasCoarsePen()->GetColour(),
                        GetThickness(),
                        wxGetApp().GetCanvasCoarsePen()->GetStyle());
        GetPrimitive()->SetPen(l_Pen);
		GetPrimitive()->SetBrush(wxGetApp().GetCanvasCoarseBrush());
	}
	else
	{
        auto l_Pen = wxThePenList->FindOrCreatePen(
                        GetPen()->GetColour(),
                        GetThickness(),
                        GetPen()->GetStyle());
        GetPrimitive()->SetPen(l_Pen);
		GetPrimitive()->SetBrush(GetBrush());
	}

	ToggleHide(GetHide());

	return TRUE;
}

SP_GR_Edge* SP_GR_Edge::SetSource(SP_Graphic* p_pcSource)
{
	m_pcSource = p_pcSource;

	SynchronizePrim();

	return this;
}

SP_GR_Edge* SP_GR_Edge::SetTarget(SP_Graphic* p_pcTarget)
{
	m_pcTarget = p_pcTarget;

	SynchronizePrim();

	return this;
}

bool SP_GR_Edge::ReleaseSource()
{
	// if we haven't a source set, everything is fine
	if (!m_pcSource)
		return TRUE;

	//SP_Graphic* l_pcSource = m_pcSource;

	if (m_pcSource->RemoveSourceEdge(this))
	{
		m_pcSource = NULL;
	}

	return TRUE;
	// changed by rr:  problems when merging nodes that are connected with coarse nodes
	//    return ResetState(l_pcSource);
}

bool SP_GR_Edge::ReleaseTarget()
{
	// if we haven't a target set, everything is fine
	if (!m_pcTarget)
		return TRUE;

	//SP_Graphic* l_pcTarget = m_pcTarget;

	if (m_pcTarget->RemoveTargetEdge(this))
	{
		m_pcTarget = NULL;
	}

	return TRUE;
	//changed by rr: problems when merging nodes that are connected with coarse nodes
	//    return ResetState(l_pcTarget);
}

bool SP_GR_Edge::ResetState(SP_Graphic* p_pcGraphic)
{
	if (GetDelete())
		return TRUE;

	bool l_bDeleted= FALSE;
	// special for coarse edges
	if (GetGraphicState() == SP_STATE_COARSEBORDERTOP)
	{
		SP_ListGraphic::iterator l_Iter;
		;
		for (l_Iter = GetParent()->GetGraphics()->begin(); l_Iter != GetParent()->GetGraphics()->end(); ++l_Iter)
		{
			if ((*l_Iter)->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
			{
				(*l_Iter)->AddToDeleteQueue(SP_STATE_SINGLE);
				l_bDeleted = TRUE;
			}
		}
	}
	else if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
		SP_ListGraphic::iterator l_Iter;
		for (l_Iter = GetParent()->GetGraphics()->begin(); l_Iter != GetParent()->GetGraphics()->end(); ++l_Iter)
		{
			if ((*l_Iter)->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
			{
				(*l_Iter)->AddToDeleteQueue(SP_STATE_SINGLE);
				l_bDeleted = TRUE;
			}
		}
	}

	// whatever we were before, we are now 'normal'
	// until we get a new connection, so we determine
	// our new state then depending of the states of
	// our nodes
	if (l_bDeleted)
		SetGraphicState(SP_STATE_NORMAL);

	return TRUE;
}

bool SP_GR_Edge::Split(SP_Data* p_pcOldNode, SP_Graphic* p_pcGrNode,
		SP_Data* p_pcNewNode)
{
	CHECK_POINTER(p_pcOldNode, return FALSE);
	CHECK_POINTER(p_pcGrNode, return FALSE);
	CHECK_POINTER(p_pcNewNode, return FALSE);
	CHECK_POINTER(GetParent(), return FALSE);
	CHECK_BOOL((GetParent()->GetElementType() == SP_ELEMENT_EDGE), return FALSE);

	SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(GetParent());

	if (GetSource() == p_pcGrNode)
	{
		SP_DS_Edge* l_pcNewEdge = l_pcEdge->Split(this);
		if(l_pcEdge == l_pcNewEdge)
		{
			p_pcOldNode->RemoveSourceEdge(l_pcNewEdge);
		}
		else
		{
			l_pcEdge->GetTarget()->AddTargetEdge(l_pcNewEdge);
		}
		p_pcNewNode->AddSourceEdge(l_pcNewEdge);
	}
	else if (GetTarget() == p_pcGrNode)
	{
		SP_DS_Edge* l_pcNewEdge = l_pcEdge->Split(this);
		if(l_pcEdge == l_pcNewEdge)
		{
			p_pcOldNode->RemoveTargetEdge(l_pcNewEdge);
		}
		else
		{
			l_pcEdge->GetSource()->AddSourceEdge(l_pcNewEdge);
		}
		p_pcNewNode->AddTargetEdge(l_pcNewEdge);
	}
	else
		return FALSE;

	return TRUE;
}

bool SP_GR_Edge::OnConsumeEdge(wxShape* p_pcHit, int p_nType)
{
	CHECK_POINTER(p_pcHit, return FALSE);

	SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(p_pcHit);

	CHECK_POINTER(l_pcGraphic, return FALSE);

	// no resetting on attributes!
	while (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_ATTRIBUTE)
		l_pcGraphic = l_pcGraphic->GetGraphicParent();

	if (!l_pcGraphic || (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_NULL))
		return FALSE;

	SP_Graphic* l_pcOther = NULL;
	bool l_bReturn= FALSE;
	switch (p_nType)
	{
	case CONTROL_POINT_ENDPOINT_FROM:
		l_pcOther = GetSource();
		l_bReturn = l_pcGraphic->ConsumeSourceEdge(this);
		break;
	case CONTROL_POINT_ENDPOINT_TO:
		l_pcOther = GetTarget();
		l_bReturn = l_pcGraphic->ConsumeTargetEdge(this);
		break;
	};

	if (l_bReturn)
	{
		l_pcOther->Update();
		GetSource()->Update();
		GetTarget()->Update();
		return SP_Core::Instance()->RemoveQueuedElements() && Update(TRUE);
	}

	return FALSE;
}

bool SP_GR_Edge::OnDoubleClick(int p_nKeys)
{
	if (SP_Core::Instance()->GetAnimRunning())
		return FALSE;

	return EditProperties();
}

bool SP_GR_Edge::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
	if ((p_nFromNet == p_nToNet) || 
		(GetNetnumber() != p_nFromNet) || 
		(GetNetnumber() == p_nToNet))
		return TRUE;

	if (!GetParent() || !GetSource() || !GetTarget())
		return FALSE;

	// source is marked as 'dummy' in this subnet
	if (GetSource()->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
		// source was already processed by OnFlatten, so we just give it the old
		// number to be removed together with the coarse node later on
		GetSource()->SetNetnumber(p_nFromNet);
		// get the edge in the net above
		SP_Graphic* l_pcEdge = GetParent()->GetGraphicInSubnet(p_nToNet);
		if (!l_pcEdge)
			return FALSE;
		if(l_pcEdge->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
		{
			l_pcEdge->SetGraphicState(SP_STATE_NORMAL);
			dynamic_cast<SP_GR_Edge*>(l_pcEdge)->GetSource()->SetGraphicState(SP_STATE_NORMAL);
		}
		// the edge is conected to the coarse, so we release it there, but not in the DS
		dynamic_cast<SP_GR_Edge*>(l_pcEdge)->GetTarget()->RemoveTargetEdge(l_pcEdge, FALSE);
		dynamic_cast<SP_GR_Edge*>(l_pcEdge)->SetTarget(GetTarget());
		GetTarget()->RemoveTargetEdge(this, FALSE);
		return FALSE;
	}
	// source is marked as 'dummy' in this subnet
	if (GetTarget()->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
		// source was already processed by OnFlatten, so we just give it the old
		// number to be removed together with the coarse node later on
		GetTarget()->SetNetnumber(p_nFromNet);
		SetDelete(TRUE);
		// get the edge in the net above
		SP_Graphic* l_pcEdge = GetParent()->GetGraphicInSubnet(p_nToNet);
		if (!l_pcEdge)
			return FALSE;
		if(l_pcEdge->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
		{
			l_pcEdge->SetGraphicState(SP_STATE_NORMAL);
			dynamic_cast<SP_GR_Edge*>(l_pcEdge)->GetTarget()->SetGraphicState(SP_STATE_NORMAL);
		}
		// the edge is conected to the coarse, so we release it there, but not in the DS
		dynamic_cast<SP_GR_Edge*>(l_pcEdge)->GetSource()->RemoveSourceEdge(l_pcEdge, FALSE);
		dynamic_cast<SP_GR_Edge*>(l_pcEdge)->SetSource(GetSource());
		GetSource()->RemoveSourceEdge(this, FALSE);
		return FALSE;
	}

	return SP_Graphic::OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
}

bool SP_GR_Edge::OnCoarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr,
		SP_ListGraphic* p_plShapes)
{
	if (!p_pcCoarseGr || !p_plShapes || !GetSource() || !GetTarget()
			|| !GetSource()->GetParent() || !GetTarget()->GetParent() || !p_pcCoarseGr->GetParent()
			|| !p_pcCoarseGr->GetParent()->GetCoarse())
		return FALSE;

	// already in the right net
	if (GetNetnumber() == p_nNewNet)
		return TRUE;

	// source is a lose node, not queued already
	if (SP_Find(*p_plShapes, GetSource()) == p_plShapes->end())
	{
		// first test, whether we can connect the line to the coarsenode
		wxString l_pcharTargetName;

		//check if the border node is a coarse node
		if (GetTarget()->GetParent()->GetCoarse())
		{
			l_pcharTargetName = GetTarget()->GetParent()->GetCoarse()->GetInnerClass();
		}
		else
		{
			l_pcharTargetName = GetTarget()->GetParent()->GetClassName();
		}
		wxString l_pcharCoarseClassName = p_pcCoarseGr->GetParent()->GetCoarse()->GetInnerClass();

		//to deal with special transition types
		if( l_pcharCoarseClassName == SP_DS_STOCHASTIC_TRANS)
		{
			if( l_pcharTargetName == SP_DS_IMMEDIATE_TRANS || l_pcharTargetName == SP_DS_DETERMINISTIC_TRANS || l_pcharTargetName == SP_DS_SCHEDULED_TRANS )
			{
				l_pcharTargetName = SP_DS_STOCHASTIC_TRANS;
			}
		}

		//for hybrid Petri nets. TODO: find a better solution!
		if(l_pcharCoarseClassName == SP_DS_DISCRETE_PLACE&&
				l_pcharTargetName==SP_DS_CONTINUOUS_PLACE)
		{
			l_pcharTargetName=SP_DS_DISCRETE_PLACE;
		}

		if (l_pcharTargetName.Cmp(l_pcharCoarseClassName) != 0 /*||
		 (GetGraphicState() != SP_STATE_NORMAL)*/)
		{
			wxString l_sId;
			if(GetTarget()->GetParent()->GetAttribute(wxT("ID")))
				l_sId = GetTarget()->GetParent()->GetAttribute(wxT("ID"))->GetValueString();
			SP_MESSAGEBOX(wxString::Format(
					wxT("ATTENTION: Deleting arc(-s), because %s with id: %s ")
						wxT("is at the border of the coarse %s subnet"),
					l_pcharTargetName.c_str(), l_sId.c_str(), l_pcharCoarseClassName.c_str()),
					wxT("Alert"),
					wxICON_WARNING | wxOK);
			return AddAllToDeleteQueue();
		}

		SP_Graphic* l_pcSource = GetSource()->GetParent()->AddGraphicInSubnet(p_nNewNet);
		l_pcSource->SetPosXY(GetSource()->GetPosX(), GetSource()->GetPosY());
		SP_Graphic* l_pcTarget = GetTarget();
		// unfortunately, removes the line on both ends
		l_pcTarget->RemoveTargetEdge(this, FALSE);
		l_pcSource->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		if (GetSource()->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
			GetSource()->SetGraphicState(SP_STATE_COARSEBORDERTOP);

		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(GetParent()->AddGraphicInSubnet(p_nNewNet));
		// initialise control points member
		wxList* cp = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints();
		l_pcEdge->SetControlPoints(cp);
		l_pcEdge->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		l_pcSource->AddSourceEdge(l_pcEdge, l_pcSource->GetParent());
		l_pcTarget->AddTargetEdge(l_pcEdge, l_pcTarget->GetParent());

		p_pcCoarseGr->AddTargetEdge(this, p_pcCoarseGr->GetParent());
		GetSource()->AddSourceEdge(this, GetSource()->GetParent());
		if (GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
			SetGraphicState(SP_STATE_COARSEBORDERTOP);
	}
	// target is a lose node, not queued already
	else if (SP_Find(*p_plShapes, GetTarget()) == p_plShapes->end())
	{
		// first test, whether we can connect the line to the coarsenode
		wxString l_pcharSourceName;

		//check if the border node is a coarse node
		if (GetSource()->GetParent()->GetCoarse())
		{
			l_pcharSourceName = GetSource()->GetParent()->GetCoarse()->GetInnerClass();
		}
		else
		{
			l_pcharSourceName = GetSource()->GetParent()->GetClassName();
		}

		wxString l_pcharCoarseClassName = p_pcCoarseGr->GetParent()->GetCoarse()->GetInnerClass();

		//to deal with special transition types
		if( l_pcharCoarseClassName == SP_DS_STOCHASTIC_TRANS)
		{
			if( l_pcharSourceName == SP_DS_IMMEDIATE_TRANS || l_pcharSourceName == SP_DS_DETERMINISTIC_TRANS || l_pcharSourceName == SP_DS_SCHEDULED_TRANS )
			{
				l_pcharSourceName = SP_DS_STOCHASTIC_TRANS;
			}
		}

		//for hybrid Petri nets. TODO: find a better solution!
		if(l_pcharCoarseClassName == SP_DS_DISCRETE_PLACE&&
			l_pcharSourceName==SP_DS_CONTINUOUS_PLACE)
		{
			l_pcharSourceName=SP_DS_DISCRETE_PLACE;
		}

		if (l_pcharSourceName.Cmp(l_pcharCoarseClassName) != 0 /*||
		 (GetGraphicState() != SP_STATE_NORMAL)*/)
		{
			wxString l_sId;
			if(GetSource()->GetParent()->GetAttribute(wxT("ID")))
				l_sId = GetSource()->GetParent()->GetAttribute(wxT("ID"))->GetValueString();
			SP_MESSAGEBOX(wxString::Format(
					wxT("ATTENTION: Deleting arc(-s), because %s with id: %s ")
						wxT("is at the border of the coarse %s subnet"),
						l_pcharSourceName.c_str(), l_sId.c_str(), l_pcharCoarseClassName.c_str()),
					wxT("Alert"),
					wxICON_WARNING | wxOK);
			return AddAllToDeleteQueue();
		}

		SP_Graphic* l_pcTarget = GetTarget()->GetParent()->AddGraphicInSubnet(p_nNewNet);
		l_pcTarget->SetPosXY(GetTarget()->GetPosX(), GetTarget()->GetPosY());
		SP_Graphic* l_pcSource = GetSource();
		// unfortunately, removes the line on both ends
		l_pcSource->RemoveTargetEdge(this, FALSE);
		l_pcTarget->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		if (GetTarget()->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
			GetTarget()->SetGraphicState(SP_STATE_COARSEBORDERTOP);

		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(GetParent()->AddGraphicInSubnet(p_nNewNet));
		// initialise control points member
		wxList* cp = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints();
		l_pcEdge->SetControlPoints(cp);
		l_pcEdge->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		l_pcSource->AddSourceEdge(l_pcEdge, l_pcSource->GetParent());
		l_pcTarget->AddTargetEdge(l_pcEdge, l_pcTarget->GetParent());

		p_pcCoarseGr->AddSourceEdge(this, p_pcCoarseGr->GetParent());
		GetTarget()->AddTargetEdge(this, GetTarget()->GetParent());
		if (GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
			SetGraphicState(SP_STATE_COARSEBORDERTOP);
	}
	else
		return SP_Graphic::OnCoarse(p_nNewNet, p_pcCoarseGr, p_plShapes);
	return TRUE;
}

bool SP_GR_Edge::EditProperties()
{
	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;

	// get the attributes to show in the dialog
	// implemented in base class 'SP_Graphic'
	CollectDialogAttributes(&l_tmAttributes);

	// construct the dialog
	SP_DLG_ShapeProperties l_cDialog(NULL, wxT("Edit Properties"));
	// add this to the list of affected graphics
	l_cDialog.AddDialogGraphic(this);

	if (l_cDialog.ShowDialogAttributes(&l_tmAttributes))
	{
		// show and forget about the dlg
		// the dialog manages the events (OK, CANCEL) by itself
		l_cDialog.ShowModal();

		if (GetPrimitive()->GetCanvas())
			dynamic_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas())->Modify(TRUE);
	}
	else
	{
		SP_LOGDEBUG(wxT("No attributes on this edge"));
	}

	l_cDialog.Destroy();

	if (GetParent())
		return GetParent()->Update(TRUE);

	return Update(TRUE);
}

double SP_GR_Edge::GetPosAttributesX()
{
	CHECK_POINTER(GetPrimitive(),
			return SP_Graphic::GetPosX());

	wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetPrimitive());
	if (!l_pcShape->GetLineControlPoints())
		return SP_Graphic::GetPosX();

	int l_nCP = l_pcShape->GetLineControlPoints()->GetCount();
	wxNode* l_pcNode= NULL;
	int i = 0;
	l_pcNode = l_pcShape->GetLineControlPoints()->GetFirst();
	for (i = 0; i < (int)(l_nCP / 2); i++)
	{
		l_pcNode = l_pcNode->GetNext();
	}

	if (l_pcNode)
	{
		wxNode* l_pcNode2 = l_pcNode->GetPrevious();
		if (!l_pcNode2)
			return ((wxRealPoint*)l_pcNode->GetData())->x;

		return (((wxRealPoint*)l_pcNode->GetData())->x + ((wxRealPoint*)l_pcNode2->GetData())->x) / 2.0;
	}

	return SP_Graphic::GetPosX();
}

double SP_GR_Edge::GetPosAttributesY()
{
	CHECK_POINTER(GetPrimitive(),
			return SP_Graphic::GetPosY());

	wxLineShape* l_pcShape = dynamic_cast<wxLineShape*>(GetPrimitive());
	if (!l_pcShape->GetLineControlPoints())
		return SP_Graphic::GetPosY();

	int l_nCP = l_pcShape->GetLineControlPoints()->GetCount();
	wxNode* l_pcNode= NULL;
	int i = 0;
	l_pcNode = l_pcShape->GetLineControlPoints()->GetFirst();
	for (i = 0; i < (int)(l_nCP / 2); i++)
	{
		l_pcNode = l_pcNode->GetNext();
	}

	if (l_pcNode)
	{
		wxNode* l_pcNode2 = l_pcNode->GetPrevious();
		if (!l_pcNode2)
			return ((wxRealPoint*)l_pcNode->GetData())->y;
		return (((wxRealPoint*)l_pcNode->GetData())->y + ((wxRealPoint*)l_pcNode2->GetData())->y) / 2.0;
	}

	return SP_Graphic::GetPosY();
}

void SP_GR_Edge::GetPosAttributesXY(double* p_pnVal1, double* p_pnVal2)
{
	*p_pnVal1 = GetPosAttributesX();
	*p_pnVal2 = GetPosAttributesY();
}

double SP_GR_Edge::GetLength()
{
	CHECK_POINTER(GetPrimitive(), return 0.0);
	CHECK_POINTER(dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints(), return 0.0);

	wxNode* l_pcNode = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints()->GetFirst();
	wxRealPoint* l_pcPoint1= NULL;
	wxRealPoint* l_pcPoint2= NULL;
	double l_nLength = 0.0;
	double l_nXDist, l_nYDist;
	while (l_pcNode && l_pcNode->GetNext())
	{
		l_pcPoint1 = (wxRealPoint*)l_pcNode->GetData();
		l_pcPoint2 = (wxRealPoint*)l_pcNode->GetNext()->GetData();
		l_nXDist = l_pcPoint1->x - l_pcPoint2->x;
		l_nYDist = l_pcPoint1->y - l_pcPoint2->y;
		l_nXDist *= l_nXDist;
		l_nYDist *= l_nYDist;
		l_nLength += sqrt(l_nXDist + l_nYDist);

		l_pcNode = l_pcNode->GetNext();
	}
	return l_nLength;
}

bool SP_GR_Edge::AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX,
		double p_nY, int p_nKeys)
{
	CHECK_POINTER(p_pcCanvas, return FALSE);

	// just wouldn't draw me, but that's no BAD state at all
	if (p_pcCanvas->GetNetnumber() != GetNetnumber())
		return TRUE;

	CHECK_POINTER(GetPrimitive(), return FALSE);
	CHECK_POINTER(m_pcSource, return FALSE);
	CHECK_POINTER(m_pcTarget, return FALSE);

	if (GetPrimitive()->GetCanvas())
		return TRUE;

	if (m_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
		SetGraphicState(SP_STATE_COARSEBORDERDOWN);
	if (m_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
		SetGraphicState(SP_STATE_COARSEBORDERDOWN);

	SetPosXY(p_nX, p_nY);
	// special to wxLineShape
	wxLineShape* l_pcPrim = dynamic_cast<wxLineShape*>(GetPrimitive());

	SetControlPoints(p_pcCanvas);

	// SynchronizePrim();
	l_pcPrim->SetAttachments(0, 0);
	p_pcCanvas->AddShape(l_pcPrim);
	l_pcPrim->Show(GetShow());

	ShowAttributeGraphic();

	return TRUE;
}

bool SP_GR_Edge::AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter)
{
	if (!m_pcSource || !m_pcTarget)
		return FALSE;

	if (GetDelete())
		return TRUE;

	// this temporary storing is valid, because no element is really deleted
	// at this stage.
	SP_Graphic* l_pcSource = m_pcSource;
	SP_Graphic* l_pcTarget = m_pcTarget;

	// filter for the next step
	SP_GRAPHIC_STATE l_eFilter = (p_eFilter != SP_STATE_SINGLE) ? SP_STATE_NONE
			: SP_STATE_SINGLE;

	bool l_bReturn = SP_Graphic::AddToDeleteQueue(p_eFilter);

	if (l_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
		if (!l_pcSource->HasUndeletedEdges())
		{
			l_bReturn &= l_pcSource->AddToDeleteQueue(l_eFilter);
		}
	}
	if (l_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
	{
		if (!l_pcTarget->HasUndeletedEdges())
		{
			l_bReturn &= l_pcTarget->AddToDeleteQueue(l_eFilter);
		}
	}
	l_pcTarget->RemoveTargetEdge(this);
	l_pcSource->RemoveSourceEdge(this);
	// we are now queued for deletion, so we don't have any source nor target
	m_pcSource = NULL;
	m_pcTarget = NULL;
	return l_bReturn;
}

bool SP_GR_Edge::ShowAttributeGraphic()
{
	SP_Data* l_plParent = this->GetParent();
	if (l_plParent)
	{
		return dynamic_cast<SP_DS_Edge*>(l_plParent)->ShowAttributeGraphic(this);
	}
	return FALSE;
}

bool SP_GR_Edge::SetControlPoints(SP_GUI_Canvas* p_pcCanvas)
{
	CHECK_POINTER(p_pcCanvas, return SetControlPoints((wxList*)NULL));

	return SetControlPoints(p_pcCanvas->GetControlPoints());
}

bool SP_GR_Edge::SetControlPoints(wxList* p_pcPoints, double p_pnOffset)
{
	CHECK_POINTER(GetPrimitive(), return FALSE);
	CHECK_BOOL((GetPrimitive()->IsKindOf(CLASSINFO(wxLineShape))), return FALSE);

	wxLineShape* l_pcPrim = dynamic_cast<wxLineShape*>(GetPrimitive());

	if (!p_pcPoints || (p_pcPoints->GetCount() == 0))
	{
		// maybe we set the points already from loading
		if (!l_pcPrim->GetLineControlPoints())
			l_pcPrim->MakeLineControlPoints(2);
	}
	else
	{
		l_pcPrim->MakeLineControlPoints(p_pcPoints->GetCount());
		wxNode* l_pcNode1 = p_pcPoints->GetFirst();
		wxNode* l_pcNode2 = l_pcPrim->GetLineControlPoints()->GetFirst();
		while (l_pcNode1 && l_pcNode2)
		{
			wxRealPoint* l_pcPoint = (wxRealPoint*)l_pcNode1->GetData();
			wxRealPoint* l_pcPoint2 = (wxRealPoint*)l_pcNode2->GetData();

			l_pcPoint2->x = l_pcPoint->x + p_pnOffset;
			l_pcPoint2->y = l_pcPoint->y + p_pnOffset;

			l_pcNode1 = l_pcNode1->GetNext();
			l_pcNode2 = l_pcNode2->GetNext();
		}
	}
	return TRUE;
}

bool SP_GR_Edge::SynchronizePrim()
{
	if (!m_pcSource || !m_pcTarget)
		return FALSE;

	CHECK_POINTER(GetPrimitive(), return FALSE);
	CHECK_POINTER(m_pcSource->GetPrimitive(), return FALSE);
	CHECK_POINTER(m_pcTarget->GetPrimitive(), return FALSE);
	CHECK_BOOL((GetPrimitive()->IsKindOf(CLASSINFO(wxLineShape))), return FALSE);

	SP_GRAPHIC_STATE l_eState = GetGraphicState();
	if (m_pcSource->GetGraphicState() != SP_STATE_NORMAL)
	{
		if (m_pcSource->GetGraphicState() == SP_STATE_COARSECOARSE)
		{
			if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
				l_eState = SP_STATE_COARSEBORDERDOWN;
			else
				l_eState = SP_STATE_COARSEBORDERTOP;
		}
		else
			l_eState = m_pcSource->GetGraphicState();
	}
	else if (m_pcTarget->GetGraphicState() != SP_STATE_NORMAL)
	{
		if (m_pcTarget->GetGraphicState() == SP_STATE_COARSECOARSE)
		{
			if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
				l_eState = SP_STATE_COARSEBORDERDOWN;
			else
				l_eState = SP_STATE_COARSEBORDERTOP;
		}
		else
			l_eState = m_pcTarget->GetGraphicState();
	}

	SetGraphicState(l_eState);

	wxLineShape* l_pcPrim = dynamic_cast<wxLineShape*>(GetPrimitive());
	m_pcSource->GetPrimitive()->AddLine(l_pcPrim, m_pcTarget->GetPrimitive());
	return TRUE;
}

bool SP_GR_Edge::Rotate(double p_nX, double p_nY, double p_nAngle)
{
	/*
	 cos(a)  sin(a)       m_X - p_nX       (m_X - p_nX) *  cos(a) + (m_Y - p_nY) * sin(a)
	 (                ) * (            ) = (                                                )
	 -sin(a)  cos(a)       m_Y - p_nY       (m_X - p_nX) * -sin(a) + (m_Y - p_nY) * cos(a)
	 */
	double l_nAngle = p_nAngle * SP_PI / 180.0;

	double l_nCos = cos(l_nAngle);
	double l_nSin = sin(l_nAngle);
	double l_nX, l_nY, l_nDistX = 0, l_nDistY = 0;

	wxNode* l_pcNode = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints()->GetFirst();
	wxRealPoint* l_pcPoint= NULL;
	while (l_pcNode)
	{
		l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
		l_nX
				= ((l_pcPoint->x - p_nX) * l_nCos + (l_pcPoint->y - p_nY)
						* l_nSin) + p_nX;
		l_nY = ((l_pcPoint->x - p_nX) * (0 - l_nSin) + (l_pcPoint->y - p_nY)
				* l_nCos) + p_nY;
		l_nDistX = l_nX - l_pcPoint->x;
		l_nDistY = l_nY - l_pcPoint->y;
		l_pcPoint->x = l_nX;
		l_pcPoint->y = l_nY;
		l_pcNode = l_pcNode->GetNext();
	}

	if (GetGraphicChildren())
	{
		SP_ListGraphic::iterator l_itChild;
		for (l_itChild = GetGraphicChildren()->begin(); l_itChild != GetGraphicChildren()->end(); ++l_itChild)
		{
			// move all children, too
			(*l_itChild)->Translate(l_nDistX, l_nDistY);
		}
	}

	return TRUE;
}

bool SP_GR_Edge::Flip(double p_nX)
{
	if (!GetPrimitive())
		return FALSE;

	bool l_bReturn= TRUE;

	wxNode* l_pcNode = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints()->GetFirst();
	wxRealPoint* l_pcPoint= NULL;
	double l_nTemp;
	while (l_pcNode)
	{
		l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
		l_nTemp = l_pcPoint->x;
		l_pcPoint->x += (p_nX - l_nTemp) * 2;
		l_pcNode = l_pcNode->GetNext();
	}

	if (GetGraphicChildren())
	{
		SP_ListGraphic::iterator l_itChild;
		for (l_itChild = GetGraphicChildren()->begin(); l_itChild != GetGraphicChildren()->end(); ++l_itChild)
		{
			// recursive add all children to the queue
			l_bReturn &= (*l_itChild)->Translate((p_nX - (*l_itChild)->GetPosX()) * 2, 0);
		}
	}
	return l_bReturn;
}

bool SP_GR_Edge::Mirror(double p_nY)
{
	if (!GetPrimitive())
		return FALSE;

	bool l_bReturn= TRUE;

	wxNode* l_pcNode = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints()->GetFirst();
	wxRealPoint* l_pcPoint= NULL;
	double l_nTemp;
	while (l_pcNode)
	{
		l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
		l_nTemp = l_pcPoint->y;
		l_pcPoint->y += (p_nY - l_nTemp) * 2;
		l_pcNode = l_pcNode->GetNext();
	}

	if (GetGraphicChildren())
	{
		SP_ListGraphic::iterator l_itChild;
		for (l_itChild = GetGraphicChildren()->begin(); l_itChild != GetGraphicChildren()->end(); ++l_itChild)
		{
			// recursive add all children to the queue
			l_bReturn &= (*l_itChild)->Translate(0, (p_nY - (*l_itChild)->GetPosY()) * 2);
		}
	}
	return l_bReturn;
}

bool SP_GR_Edge::Translate(double p_nX, double p_nY)
{
	if (!GetPrimitive())
		return FALSE;

	bool l_bReturn= TRUE;

	wxNode* l_pcNode = dynamic_cast<wxLineShape*>(GetPrimitive())->GetLineControlPoints()->GetFirst();
	wxRealPoint* l_pcPoint= NULL;
	while (l_pcNode)
	{
		l_pcPoint = (wxRealPoint*)l_pcNode->GetData();
		l_pcPoint->x += p_nX;
		l_pcPoint->y += p_nY;
		l_pcNode = l_pcNode->GetNext();
	}

	if (GetGraphicChildren())
	{
		SP_ListGraphic::iterator l_itChild;
		for (l_itChild = GetGraphicChildren()->begin(); l_itChild != GetGraphicChildren()->end(); ++l_itChild)
		{
			// recursive add all children to the queue
			l_bReturn &= (*l_itChild)->Translate(p_nX, p_nY);
		}
	}
	return l_bReturn;
}

bool SP_GR_Edge::AddToCopyMaps(SP_DS_Graph** p_ppcGraph,
		SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
		SP_MapData2Data* p_pmData2Data)
{
	if (!p_pmGraphic2Graphic || !p_pmData2Data || !GetParent()
			|| !GetPrimitive())
		return FALSE;

	SP_GR_Edge* l_pcNewGraphic= NULL;
	wxLineShape* l_pcLine= NULL;
	SP_Graphic* l_pcSource= NULL;
	SP_Graphic* l_pcTarget= NULL;

	// get the new data element or create one
	SP_Data* l_pcNewData = GetParent()->AddToCopyMap(p_ppcGraph, this, p_pmGraphic2Graphic,	p_pmData2Data);
	if (!l_pcNewData)
		return FALSE;

	l_pcNewGraphic = dynamic_cast<SP_GR_Edge*>(Clone(l_pcNewData));
	SP_MapGraphic2Graphic::const_iterator l_it = p_pmGraphic2Graphic->find(m_pcSource);
	if (l_it != p_pmGraphic2Graphic->end())
	{
		l_pcSource = (*l_it).second;
	}
	l_it = p_pmGraphic2Graphic->find(m_pcTarget);
	if (l_it != p_pmGraphic2Graphic->end())
	{
		l_pcTarget = (*l_it).second;
	}

	l_pcNewGraphic->SetSource(l_pcSource);
	l_pcNewGraphic->SetTarget(l_pcTarget);

	l_pcLine = dynamic_cast<wxLineShape*>(GetPrimitive());
	l_pcNewGraphic->SetControlPoints(l_pcLine->GetLineControlPoints(), 0.0);

	l_pcNewData->AddGraphic(l_pcNewGraphic);
	(*p_pmGraphic2Graphic)[this] = l_pcNewGraphic;

	return SynchronizeChildren(p_pmGraphic2Graphic);
}

wxBrush* SP_GR_Edge::GetBrush()
{
	return (m_pcBrush) ? m_pcBrush : wxGetApp().GetCanvasEdgeBrush();
}

double SP_GR_Edge::GetPosX()
{
	return GetPosAttributesX();
}

double SP_GR_Edge::GetPosY()
{
	return GetPosAttributesY();
}
