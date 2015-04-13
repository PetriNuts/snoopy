//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Node-Base Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"
#include "snoopy.h"

SP_GR_Node::SP_GR_Node(SP_DS_Node* p_pcDataParent)
    :SP_Graphic(SP_GRAPHIC_NODE),
    m_bFixedSize(FALSE)
{
    SetDataParent(p_pcDataParent);
}

SP_GR_Node::~SP_GR_Node()
{
}

bool
SP_GR_Node::CloneBase(SP_Graphic* p_pcCopy)
{
    if (!p_pcCopy)
        return FALSE;

    dynamic_cast<SP_GR_Node*>(p_pcCopy)->SetFixedSize(m_bFixedSize);

    return SP_Graphic::CloneBase(p_pcCopy);
}

bool
SP_GR_Node::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(GetPrimitive(), return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);

    ResetGraphicState();

    if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
    {
        GetPrimitive()->SetPen(wxGetApp().GetCanvasCoarsePen());
    }
    else
    {
        GetPrimitive()->SetPen(GetPen());
    }

    // special logical, set the style to cross hatch
	if (GetParent()->GetLogical() && (GetBrush() != wxGetApp().GetCanvasLogicBrush()))
	{
		if (GetBrush()->GetColour() != *wxWHITE)
		{
			GetPrimitive()->SetBrush(wxTheBrushList->FindOrCreateBrush(GetBrush()->GetColour(), wxCROSSDIAG_HATCH));
		}
		else
		{
			GetPrimitive()->SetBrush(wxGetApp().GetCanvasLogicBrush());
		}
	}
	else
	{
		GetPrimitive()->SetBrush(GetBrush());
	}

	ToggleHide(GetHide());

	if (GetParent()->GetCoarse())
		GetParent()->GetCoarse()->Update(FALSE, p_bLocalOnly);

	return TRUE;
}

bool
SP_GR_Node::ResetGraphicState()
{
    if (!GetPrimitive())
        return FALSE;

    if (GetGraphicState() == SP_STATE_COARSECOARSE)
        return TRUE;

    if (GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
        if (!HasUndeletedEdges())
            return AddToDeleteQueue(SP_STATE_NONE);

    wxNode* l_pcNode = GetPrimitive()->GetLines().GetFirst();
    wxLineShape* l_pcLine;
    SP_GR_Edge* edge;
    int l_nState = 0;
    bool skipIt;
    while (l_pcNode)
    {
        l_pcLine = (wxLineShape*)l_pcNode->GetData();
        edge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern(l_pcLine));

		// both source and target nodes can not be in SP_STATE_COARSEBORDERDOWN state
		skipIt = false;
		if ((edge->GetSource() == this) &&
			(edge->GetTarget()->GetGraphicState() & SP_STATE_COARSEBORDERDOWN))
		{
		  skipIt = true;
		}
		if ((edge->GetTarget() == this) &&
			(edge->GetSource()->GetGraphicState() & SP_STATE_COARSEBORDERDOWN))
		{
		  skipIt = true;
		}
		if (! skipIt)
		{
			l_nState |= edge->GetGraphicState();
		}

		l_pcNode = l_pcNode->GetNext();
    }
    if (l_nState & SP_STATE_COARSEBORDERTOP)
        return SetGraphicState(SP_STATE_COARSEBORDERTOP);
    if (l_nState & SP_STATE_COARSEBORDERDOWN)
        return SetGraphicState(SP_STATE_COARSEBORDERDOWN);

    return SetGraphicState(SP_STATE_NORMAL);
}

bool
SP_GR_Node::OnDoubleClick(int p_nKeys)
{
    if (SP_Core::Instance()->GetAnimRunning())
        return FALSE;

    if (GetParent() && GetParent()->OnDoubleClick(this, p_nKeys))
    {
        return TRUE;
    }

    return EditProperties();
}

bool
SP_GR_Node::EditProperties()
{
    // map of list of attributes
    SP_MapString2ListAttribute l_tmAttributes;

    // get the attributes to show in the dialog
    // implemented in base class 'SP_Graphic'
    CollectDialogAttributes(&l_tmAttributes);

    // construct the dialog
    SP_DLG_ShapeProperties l_cDialog(NULL, wxT("Edit Properties"));
    // add this to the list of affected nodes
    l_cDialog.AddDialogGraphic(this);

    if (l_cDialog.ShowDialogAttributes(&l_tmAttributes))
    {
        // show and forget about the dlg
        // the dialog manages the events (OK, CANCEL) by itself
        l_cDialog.ShowModal();

        if (GetPrimitive())
        {
			dynamic_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas())->Modify(TRUE);
        }
    }
    else
    {
        SP_LOGDEBUG(wxT("No attributes on this node."));
    }

    l_cDialog.Destroy();

    if (GetParent())
    {
        return GetParent()->Update(TRUE);
    }

    return Update(TRUE);
}

bool
SP_GR_Node::AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX, double p_nY, int p_nKeys)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);

    // just wouldn't draw me, but that's no BAD state at all
    if (p_pcCanvas->GetNetnumber() != GetNetnumber())
        return TRUE;

    CHECK_POINTER(GetPrimitive(), return FALSE);

    if (GetPrimitive()->GetCanvas())
        return TRUE;

    if ((p_nX != -1) && (p_nY != -1))
        SetPosXY(p_nX, p_nY);

    p_pcCanvas->AddShape(GetPrimitive());
    GetPrimitive()->Show(GetShow());

    ShowAttributeGraphic();

    return TRUE;
}

bool
SP_GR_Node::AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter)
{
    if (!GetPrimitive() || !GetParent())
        return FALSE;

    if (GetDelete())
        return TRUE;

    if (((p_eFilter & SP_STATE_COARSEBORDERDOWN) == SP_STATE_COARSEBORDERDOWN) &&
        (GetGraphicState() == SP_STATE_COARSEBORDERDOWN))
        return FALSE;

    wxNode* l_pcNode = GetPrimitive()->GetLines().GetFirst();
    while (l_pcNode)
    {
        wxLineShape* l_pcLine = dynamic_cast<wxLineShape*>(l_pcNode->GetData());
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcLine);
        if (l_pcGraphic)
        {
            // enqueue the lines, too
            if ((p_eFilter != SP_STATE_SINGLE))
                l_pcGraphic->AddAllToDeleteQueue(p_eFilter);
            // the edge AND either end will be removed! we can't ensure, that
            // the edge isn't removed before any of their nodes, so we just
            // tell source and target that there are'nt any more lines connected
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseSource();
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseTarget();

            l_pcNode = GetPrimitive()->GetLines().GetFirst();
        }
        else
            l_pcNode = l_pcNode->GetNext();
    }

    return SP_Graphic::AddToDeleteQueue(p_eFilter);
}

bool
SP_GR_Node::HasUndeletedEdges()
{
    wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
    while (l_pcListNode)
    {
        SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern((wxLineShape*)l_pcListNode->GetData()));

        if (l_pcEdge && !l_pcEdge->GetDelete())
            return TRUE;

        l_pcListNode = l_pcListNode->GetNext();
    }
    return FALSE;
}

bool
SP_GR_Node::ShowAttributeGraphic()
{
    SP_Data* l_plParent = this->GetParent();
    if (l_plParent)
    {
        return dynamic_cast<SP_DS_Node*>(l_plParent)->ShowAttributeGraphic(this);
    }
    return FALSE;
}

SP_Graphic*
SP_GR_Node::AddSourceEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcSource)
{
    CHECK_POINTER(p_pcEdge, return NULL);
 wxASSERT_MSG(GetNetnumber() == p_pcEdge->GetNetnumber(), wxT("Net numbers conflict in SP_GR_Node::AddSourceEdge"));

    if (p_pcEdge->GetGraphicType() == SP_GRAPHIC_EDGE)
    {
        return dynamic_cast<SP_GR_Edge*>(p_pcEdge)->SetSource(this);
    }

    SP_LOGDEBUG(wxT("SP_GR_Node::AddSourceEdge doesn't actually know how to connect two nodes!"));

    return NULL;
}

SP_Graphic*
SP_GR_Node::AddTargetEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcTarget)
{
     CHECK_POINTER(p_pcEdge, return NULL);

    wxASSERT_MSG(GetNetnumber() == p_pcEdge->GetNetnumber(), wxT("Net numbers conflict in SP_GR_Node::AddSourceEdge"));

    if (p_pcEdge->GetGraphicType() == SP_GRAPHIC_EDGE)
    {
        dynamic_cast<SP_GR_Edge*>(p_pcEdge)->SetTarget(this);

        return p_pcEdge;
    }

    SP_LOGDEBUG(wxT("SP_GR_Node::AddTargetEdge doesn't actually know how to connect two nodes!"));

    return NULL;
}

bool
SP_GR_Node::SetNewSource(SP_Graphic* p_pcSource)
{

	if (!p_pcSource || (p_pcSource == this) )
        return FALSE;

    bool l_bReturn = TRUE;

	wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
	while(l_pcListNode && l_bReturn)
	{
		wxLineShape *l_pcLine = dynamic_cast<wxLineShape*>(l_pcListNode->GetData());
		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern(l_pcLine));
		CHECK_POINTER(l_pcEdge, return FALSE);

		if (l_pcEdge->GetSource() == this)
		{
			l_bReturn = l_pcEdge->SetSource(p_pcSource);
		}
		l_pcListNode = l_pcListNode->GetNext();
    }
    return l_bReturn;
}

bool
SP_GR_Node::SetNewTarget(SP_Graphic* p_pcTarget)
{

	if (!p_pcTarget || (p_pcTarget == this) )
        return FALSE;

    bool l_bReturn = TRUE;

	wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
	while(l_pcListNode && l_bReturn)
	{
		wxLineShape *l_pcLine = dynamic_cast<wxLineShape*>(l_pcListNode->GetData());
		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern(l_pcLine));
		CHECK_POINTER(l_pcEdge, return FALSE);

		if (l_pcEdge->GetTarget() == this)
		{
			l_bReturn = l_pcEdge->SetTarget(p_pcTarget);
		}

		l_pcListNode = l_pcListNode->GetNext();
    }
    return l_bReturn;
}

bool
SP_GR_Node::RemoveSourceEdge(SP_Graphic* p_pcEdge, bool p_bInDs)
{
    CHECK_POINTER(p_pcEdge, return TRUE);
    CHECK_POINTER(GetPrimitive(), return FALSE);
    CHECK_POINTER(p_pcEdge->GetPrimitive(), return FALSE);
	CHECK_POINTER(GetParent(), return FALSE);

    // clean up the wx' stuff
    GetPrimitive()->RemoveLine(dynamic_cast<wxLineShape*>(p_pcEdge->GetPrimitive()));
    if (!p_bInDs)
        return ResetGraphicState();

    // ask the DS
    SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(p_pcEdge->GetParent());
    // ask the DS to remove the edge.
    return ResetGraphicState() && GetParent()->RemoveSourceEdge(l_pcEdge);
}

bool
SP_GR_Node::RemoveTargetEdge(SP_Graphic* p_pcEdge, bool p_bInDs)
{
    CHECK_POINTER(p_pcEdge, return TRUE);
    CHECK_POINTER(GetPrimitive(), return FALSE);
    CHECK_POINTER(p_pcEdge->GetPrimitive(), return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);
    // clean up the wx' stuff
    GetPrimitive()->RemoveLine(dynamic_cast<wxLineShape*>(p_pcEdge->GetPrimitive()));

    if (!p_bInDs)
        return ResetGraphicState();

    // ask the DS
    SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(p_pcEdge->GetParent());
    // ask the DS to remove the edge.
    return ResetGraphicState() && GetParent()->RemoveTargetEdge(l_pcEdge);
}

bool
SP_GR_Node::ConsumeSourceEdge(SP_Graphic* p_pcEdge)
{

    CHECK_POINTER(p_pcEdge, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);

    wxASSERT_MSG(GetNetnumber() == p_pcEdge->GetNetnumber(), wxT("Net numbers conflict in SP_GR_Node::ConsumeSourceEdge"));
    CHECK_BOOL((p_pcEdge->GetGraphicType() == SP_GRAPHIC_EDGE), return FALSE);

    SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(p_pcEdge);

    // assuming l_pcEdge is really an edge
    SP_DS_Edge* l_pcData = dynamic_cast<SP_DS_Edge*>(l_pcEdge->GetParent());
    CHECK_POINTER(l_pcData, return FALSE);

    if (this->GetParent() != l_pcData->GetSource() &&
    		!l_pcData->CanConnectNodes(this->GetParent(), NULL))
        return FALSE;

    SP_Graphic* l_pcOldSource = l_pcEdge->GetSource();
	SP_Graphic* l_pcTarget = l_pcEdge->GetTarget();
	SP_Graphic* l_pcSubNetEdgeGraphic;
	SP_Graphic*	l_pcTempSubNetEdgeGraphic;

	//if Target is Coarse Node
	while (l_pcTarget->GetGraphicState() == SP_STATE_COARSECOARSE){
		unsigned int l_uiSubNetNumber = l_pcTarget->GetParent()->GetCoarse()->GetNetnumber();
		l_pcSubNetEdgeGraphic = l_pcData->GetGraphicInSubnet(l_uiSubNetNumber);
		SP_GR_Edge* l_pcSubNetEdge = dynamic_cast<SP_GR_Edge*>(l_pcSubNetEdgeGraphic);
		SP_Graphic*  l_pcSubNetTargetNode = l_pcSubNetEdge->GetTarget();


		l_pcTempSubNetEdgeGraphic = l_pcData->GetGraphicInSubnet(l_uiSubNetNumber);
		SP_GR_Edge* l_pcTempSubNetEdge = dynamic_cast<SP_GR_Edge*>(l_pcTempSubNetEdgeGraphic);
		//SP_Graphic*  l_pcTempSubNetSourceNode = l_pcTempSubNetEdge->GetSource();

		 l_pcTempSubNetEdge->ReleaseSource();

		SP_Graphic* l_pcSubNetBorderNodeGraphic = dynamic_cast<SP_DS_Node*>(GetParent())->AddGraphicInSubnet(l_uiSubNetNumber);
		l_pcSubNetBorderNodeGraphic->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		l_pcSubNetBorderNodeGraphic->SetPosXY(GetPosX(), GetPosY());

		l_pcTempSubNetEdge->SetSource(l_pcSubNetBorderNodeGraphic);
		l_pcTempSubNetEdge->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

		l_pcTarget = l_pcSubNetTargetNode;

	}

	// ask the edge to release its target
	//this will release the subnetnode too :-(
	if (!l_pcEdge->ReleaseSource())
	   return FALSE;


	AddSourceEdge(l_pcEdge, l_pcData);
	GetParent()->AddSourceEdge(l_pcData);

	//Clean up the unneeded stuff
	if ( dynamic_cast<SP_DS_Node*>(l_pcOldSource->GetParent())->GetTargetEdges()->size() == 0 &&
		dynamic_cast<SP_DS_Node*>(l_pcOldSource->GetParent())->GetSourceEdges()->size()== 0)
	{
	   l_pcOldSource->AddAllToDeleteQueue();
	   l_pcOldSource->GetParent()->Remove();
	}


    return TRUE;
}

bool
SP_GR_Node::ConsumeTargetEdge(SP_Graphic* p_pcEdge)
{

	CHECK_POINTER(p_pcEdge, return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);

    wxASSERT_MSG(GetNetnumber() == p_pcEdge->GetNetnumber(), wxT("Net numbers conflict in SP_GR_Node::ConsumeTargetEdge"));
    CHECK_BOOL((p_pcEdge->GetGraphicType() == SP_GRAPHIC_EDGE), return FALSE);

    SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(p_pcEdge);

    // assuming l_pcEdge is really an edge
    SP_DS_Edge* l_pcData = dynamic_cast<SP_DS_Edge*>(l_pcEdge->GetParent());
    CHECK_POINTER(l_pcData, return FALSE);

    if (this->GetParent() != l_pcData->GetTarget() &&
    		!l_pcData->CanConnectNodes(NULL, this->GetParent()))
        return FALSE;


	SP_Graphic* l_pcSource = l_pcEdge->GetSource();
	SP_Graphic* l_pcOldTarget = l_pcEdge->GetTarget();
	SP_Graphic* l_pcSubNetEdgeGraphic;
	SP_Graphic*	l_pcTempSubNetEdgeGraphic;

	//if Source is Coarse Node
    while (l_pcSource->GetGraphicState() == SP_STATE_COARSECOARSE){
		unsigned int l_uiSubNetNumber = l_pcSource->GetParent()->GetCoarse()->GetNetnumber();
		l_pcSubNetEdgeGraphic = l_pcData->GetGraphicInSubnet(l_uiSubNetNumber);
		SP_GR_Edge* l_pcSubNetEdge = dynamic_cast<SP_GR_Edge*>(l_pcSubNetEdgeGraphic);
		SP_Graphic*  l_pcSubNetSourceNode = l_pcSubNetEdge->GetSource();


		l_pcTempSubNetEdgeGraphic = l_pcData->GetGraphicInSubnet(l_uiSubNetNumber);
		SP_GR_Edge* l_pcTempSubNetEdge = dynamic_cast<SP_GR_Edge*>(l_pcTempSubNetEdgeGraphic);
		//SP_Graphic*  l_pcTempSubNetTargetNode = l_pcTempSubNetEdge->GetTarget();

		l_pcTempSubNetEdge->ReleaseTarget();

		SP_Graphic* l_pcSubNetBorderNodeGraphic = dynamic_cast<SP_DS_Node*>(GetParent())->AddGraphicInSubnet(l_uiSubNetNumber);
		l_pcSubNetBorderNodeGraphic->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
		l_pcSubNetBorderNodeGraphic->SetPosXY(GetPosX(), GetPosY());

		l_pcTempSubNetEdge->SetTarget(l_pcSubNetBorderNodeGraphic);
		l_pcTempSubNetEdge->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

		l_pcSource = l_pcSubNetSourceNode;

	}

	// ask the edge to release its target
	//this will release the subnetnode too :-(
	if (!l_pcEdge->ReleaseTarget())
	   return FALSE;


	AddTargetEdge(l_pcEdge,l_pcData);
	GetParent()->AddTargetEdge(l_pcData);

	//Clean up the unneeded stuff

    if ( dynamic_cast<SP_DS_Node*>(l_pcOldTarget->GetParent())->GetTargetEdges()->size() == 0 &&
		dynamic_cast<SP_DS_Node*>(l_pcOldTarget->GetParent())->GetSourceEdges()->size() == 0)
    {
		 l_pcOldTarget->AddAllToDeleteQueue();
		 l_pcOldTarget->GetParent()->Remove();
	}

	return TRUE;

}

bool
SP_GR_Node::Merge(SP_Graphic* p_pcTarget)
{

	if (!p_pcTarget || (p_pcTarget == this) || !Equals(p_pcTarget))
        return FALSE;

    bool l_bReturn = TRUE;

	wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
	while(l_pcListNode && l_bReturn)
	{
		wxLineShape *l_pcLine = dynamic_cast<wxLineShape*>(l_pcListNode->GetData());
		SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern(l_pcLine));
		CHECK_POINTER(l_pcEdge, return FALSE);

		if (l_pcEdge->GetTarget() == this)
		{
			l_bReturn &= p_pcTarget->ConsumeTargetEdge(l_pcEdge);
		}
		else if (l_pcEdge->GetSource() == this)
		{
			l_bReturn &= p_pcTarget->ConsumeSourceEdge(l_pcEdge);
		}
		l_pcListNode = GetPrimitive()->GetLines().GetFirst();
    }
    return l_bReturn && SP_Graphic::Merge(p_pcTarget);
}

bool
SP_GR_Node::SplitEdges(SP_DS_Node* p_pcNode)
{
    CHECK_POINTER(p_pcNode, return FALSE);
    CHECK_POINTER(GetPrimitive(), return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);

    bool l_bReturn = TRUE;
    wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
    while (l_pcListNode)
    {
        SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern((wxLineShape*)l_pcListNode->GetData()));

        CHECK_POINTER(l_pcEdge, return FALSE);

        l_bReturn &= l_pcEdge->Split(GetParent(), this, p_pcNode);

        l_pcListNode = l_pcListNode->GetNext();
    }
    return l_bReturn;
}

bool
SP_GR_Node::Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes)
{
    CHECK_POINTER(GetPrimitive(), return FALSE);

    wxNode* l_pcListNode = GetPrimitive()->GetLines().GetFirst();
    list<SP_GR_Edge*> l_lEdges;
    list<SP_GR_Edge*>::iterator l_Iter;
    while (l_pcListNode)
    {
        SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern((wxLineShape*)l_pcListNode->GetData()));
        if (l_pcEdge)
            l_lEdges.push_back(l_pcEdge);
        l_pcListNode = l_pcListNode->GetNext();
    }

    // really coarse this element
    bool l_bReturn = SP_Graphic::Coarse(p_nNewNet, p_pcCoarseGr, p_plShapes);

    for (l_Iter = l_lEdges.begin(); l_Iter != l_lEdges.end(); ++l_Iter)
        l_bReturn &= (*l_Iter)->OnCoarse(p_nNewNet, p_pcCoarseGr, p_plShapes);

    return l_bReturn;
}

void
SP_GR_Node::SetFixedSize(bool p_bVal)
{
    m_bFixedSize = p_bVal;

    if (GetPrimitive())
        GetPrimitive()->SetFixedSize(p_bVal, p_bVal);
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_Node::SetWidth(double p_nVal)
{
    double l_nHeight = GetHeight();

    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(p_nVal, l_nHeight);
    return TRUE;
}

double
SP_GR_Node::GetWidth()
{
    CHECK_POINTER(GetPrimitive(), return 0.0);

    double l_nWidth, l_nHeight;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nWidth;
}

bool
SP_GR_Node::SetHeight(double p_nVal)
{
    double l_nWidth = GetWidth();

    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(l_nWidth, p_nVal);
    return TRUE;
}

double
SP_GR_Node::GetHeight()
{
    CHECK_POINTER(GetPrimitive(), return 0.0);

    double l_nWidth, l_nHeight;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nHeight;
}
