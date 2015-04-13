//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Attribute.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_core/SP_Core.h"

SP_DS_Edge::SP_DS_Edge(SP_DS_Edgeclass* p_pcEdgeclass, unsigned int p_nNetnumber)
:SP_Data(SP_ELEMENT_EDGE),
m_bCoarseCompleted(FALSE)
{
    m_pcEdgeclass = p_pcEdgeclass;
    SetNetnumber(p_nNetnumber);

    m_pcSource = (SP_DS_Node*)NULL;
    m_pcTarget = (SP_DS_Node*)NULL;
}

SP_DS_Edge::~SP_DS_Edge()
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
		wxDELETE(l_pcAttr);
    }
    m_lAttributes.clear();

    RemoveAllGraphics(true);
}

SP_DS_Edge*
SP_DS_Edge::Clone(bool p_bCloneGr)
{
    SP_DS_Edge* l_pcNewEdge = new SP_DS_Edge(GetEdgeclass(), GetNetnumber());

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_pcNewEdge->AddAttribute(l_pcAttr->Clone(p_bCloneGr));
    }
    l_pcNewEdge->SetLogical(GetLogical());

    if (m_pcCoarse)
        l_pcNewEdge->SetCoarse(m_pcCoarse->Clone());

    // don't clone the list of graphics, too
    if (!p_bCloneGr)
        return l_pcNewEdge;

    SP_ListGraphic* l_plGraphic = GetGraphics();
    SP_Graphic* l_pcTemp;
    if (l_plGraphic)
    {
        // check all graphics for having the proper type
        for (SP_Graphic* l_pcGr : *l_plGraphic)
        {
            if (l_pcGr->GetGraphicType() == SP_GRAPHIC_EDGE)
            {
                l_pcTemp = l_pcGr->Clone(l_pcNewEdge);
                if (l_pcTemp)
                    l_pcTemp->ShowAttributeGraphic();

                l_pcNewEdge->AddGraphic(l_pcTemp);
            }
            else
            {
                // found a wrong associated graphic
                // should never happen, really
                wxASSERT_MSG(FALSE, wxT("Something went terribly wrong in SP_DS_Edge::Clone()"));
                wxDELETE(l_pcNewEdge);
                SetLastError(wxT("Wrong type for the graphic associated in SP_DS_Node"));
                return NULL;
            }
        }
    }

    return l_pcNewEdge;
}

/**	\brief	AddToCopyMap function

    \param  p_ppcGraph  reference to the graph we are copied to
    \param	p_pmGraphic2Graphic	a parameter of type SP_Graphic*>*
	\param	p_pmData2Data	map of objects from copied one to copies
	\param	p_pcGraphic	    the graphic, we are the parent of

	\return	SP_Data* the copied object or NULL in case of an error

	This method is called from within SP_Graphic::AddToCopyMap with
    ourself as caller (GetParent() of the graphic).
    We need to check the p_pcGraphic parameter for the list of children
    and add clones for everyone of them.
*/
SP_Data*
SP_DS_Edge::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                         SP_Graphic* p_pcGraphic,
                         SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                         SP_MapData2Data* p_pmData2Data)
{
    CHECK_POINTER(p_ppcGraph, return NULL);
    CHECK_POINTER(p_pmData2Data, return NULL);

    // try to find the mapping to myself in the parameter
    SP_DS_Edge* l_pcNewEdge = NULL;
    SP_Data* l_pcSource = NULL;
    SP_Data* l_pcTarget = NULL;
	SP_MapData2Data::const_iterator l_it = p_pmData2Data->find(this);
	if (l_it == p_pmData2Data->end())
    {
        // if not found, clone a new one and insert it into the map
        // clone the node without any graphics, because there can be
        // much more than we have selected. Check p_pcGraphic afterwards.
        l_pcNewEdge = this->Clone(FALSE);
        // new element insert into the copy graph
        (*p_ppcGraph)->AddElement(l_pcNewEdge);
        p_pmData2Data->insert(make_pair(this, l_pcNewEdge));
        // search the old source and target in the map
        // and take its values as new source and target
        l_it = p_pmData2Data->find(m_pcSource);
        if (l_it != p_pmData2Data->end())
        {
        	l_pcSource = (*l_it).second;
        }
        l_it = p_pmData2Data->find(m_pcTarget);
        if (l_it != (p_pmData2Data->end()))
        {
        	l_pcTarget = (*l_it).second;
        }

        // no DS_Edge starts or ends on a coarse-node, so that call should be safe
        l_pcNewEdge->SetNodes(l_pcSource, l_pcTarget);
    }
    else
    {
    	l_pcNewEdge = dynamic_cast<SP_DS_Edge*>((*l_it).second);
    }

    return SynchronizeChildren(l_pcNewEdge, p_pcGraphic, p_pmGraphic2Graphic);
}

SP_Data*
SP_DS_Edge::SynchronizeChildren(SP_DS_Edge* p_pcEdge,
                                SP_Graphic* p_pcGraphic,
                                SP_MapGraphic2Graphic* p_pmGraphic2Graphic)
{
    CHECK_POINTER(p_pcEdge, return NULL);
    CHECK_POINTER(p_pcGraphic, return NULL);
    CHECK_POINTER(p_pcGraphic->GetGraphicChildren(), return NULL);
    CHECK_POINTER(p_pmGraphic2Graphic, return NULL);

    SP_Data* l_pcData = NULL;
    SP_DS_Attribute* l_pcAttr = NULL;
    SP_DS_Attribute* l_pcNewAttr = NULL;
    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = p_pcGraphic->GetGraphicChildren()->begin();
    l_Iter != p_pcGraphic->GetGraphicChildren()->end(); ++l_Iter)
    {
        l_pcData = (*l_Iter)->GetParent();
        if (l_pcData->GetElementType() != SP_ELEMENT_ATTRIBUTE)
            continue;

        l_pcAttr = dynamic_cast<SP_DS_Attribute*>(l_pcData);
        l_pcNewAttr = p_pcEdge->GetAttribute(l_pcAttr->GetName());
        if (l_pcNewAttr)
        {
            // insert the graphic association in the graphic map
            (*p_pmGraphic2Graphic)[(*l_Iter)] = l_pcNewAttr->AddGraphic((*l_Iter)->Clone(l_pcNewAttr));
        }
    }

    return p_pcEdge;
}

bool
SP_DS_Edge::ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
    CHECK_POINTER(p_pmNet2Net, return FALSE);
    CHECK_POINTER(p_pcToGraph, return FALSE);

    bool l_bReturn = TRUE;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
    }

    return (SP_Data::ResetNetnumbers(p_pmNet2Net, p_pcToGraph) && l_bReturn);
}

SP_Graphic*
SP_DS_Edge::NewGraphicInSubnet(unsigned int p_nNetnumber)
{
    SP_Graphic* l_pcOwn = SP_Data::NewGraphicInSubnet(p_nNetnumber);
    CHECK_POINTER(l_pcOwn, return NULL);

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_pcOwn->AddGraphicChildren(l_pcAttr->NewGraphicInSubnet(p_nNetnumber));
    }

    return l_pcOwn;
}

SP_DS_Edgeclass*
SP_DS_Edge::GetEdgeclass()
{
    return m_pcEdgeclass;
}

bool
SP_DS_Edge::ClassNameChanged(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return FALSE;

    SP_WDG_DialogBase* l_pcWdg;
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
		l_pcWdg = SP_Core::Instance()->GetDialogWidget(l_pcAttr);
		if (l_pcWdg)
			SP_Core::Instance()->RegisterDialogWidget(wxString::Format(wxT("%s|%s|%s|%d"),
					GetClassObject()->GetParentGraph()->GetNetclass()->GetName().c_str(),
					p_pchName.c_str(),
					l_pcAttr->GetName().c_str(),
					l_pcAttr->GetAttributeType()), l_pcWdg);
    }

    return TRUE;
}

SP_DS_Attribute*
SP_DS_Edge::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
    CHECK_POINTER(p_pcAttr, return NULL);
    // alert, parameter already is associated to another data-element
    CHECK_BOOL((p_pcAttr->GetParent() == NULL), return NULL);

	//if p_pcAttr already exists, don't add it again
	bool l_bAdd = true;
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes) {
        if ((*p_pcAttr).Equals(l_pcAttr)) l_bAdd = false;
    }
		if (!l_bAdd) return 0;

    p_pcAttr->SetDataParent(this);
    //p_pcAttr->SetNetnumber(GetNetnumber());

    m_lAttributes.push_back(p_pcAttr);

    return p_pcAttr;
}

SP_DELETE_STATE
SP_DS_Edge::RemoveGraphic(SP_ListGraphic* p_plGraphic, bool p_bDelete)
{
    CHECK_POINTER(p_plGraphic, return SP_DELETE_ERROR);

    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = p_plGraphic->begin(); l_Iter != p_plGraphic->end(); ++l_Iter)
    {
        SP_Graphic* l_pcGraphic = (*l_Iter);
        if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
        {
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseSource();
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseTarget();
        }
    }
    return SP_Data::RemoveGraphic(p_plGraphic, p_bDelete);
}

bool SP_DS_Edge::MergeData(SP_Data* p_pcElement)
{
	CHECK_POINTER(p_pcElement, return FALSE);
	CHECK_BOOL((p_pcElement->GetElementType() == SP_ELEMENT_EDGE), return FALSE);

	bool l_bReturn= TRUE;
	SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(p_pcElement);
	SP_ListAttribute::const_iterator l_AttrIter;
	for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
	{
		SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
		l_bReturn
				&= l_pcAttribute->MergeData(l_pcEdge->GetAttribute(l_pcAttribute->GetName()));
	}
	SetLogical(true);
	l_bReturn &= SP_Data::MergeData(l_pcEdge);

	return l_bReturn && l_pcEdge->Remove();
}

SP_DS_Edge*
SP_DS_Edge::Split(SP_Graphic* p_pcGraphic)
{
	SP_DS_Edge* l_pcNew = NULL;
	SP_ListAttribute::const_iterator l_AttrIter;

	CHECK_POINTER(p_pcGraphic, return this);
	CHECK_POINTER(GetEdgeclass(), return NULL);

	// if this node has only one graphic left, we don't need to create a new one
	// or it is the last ordinary node and all other graphics are coarse ones
	if (GetGraphics())
	{
		SP_ListGraphic::iterator l_Iter;
		int l_nOrdinary = 0;
		for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
			if ((*l_Iter)->GetGraphicState() == SP_STATE_NORMAL)
				l_nOrdinary++;
		if (l_nOrdinary <= 1)
		{
			SetLogical(FALSE);
			return this;
		}
	}

	// clone new node based on this, but without any graphics
	l_pcNew = this->Clone(FALSE);
	for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
	{
		SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
		l_pcAttribute->Split(l_pcNew->GetAttribute(l_pcAttribute->GetName()),
				p_pcGraphic->GetGraphicChildren());
	}
	
	SP_Data::RemoveGraphic(p_pcGraphic, FALSE);
	l_pcNew->AddGraphic(p_pcGraphic);
	p_pcGraphic->SetDataParent(l_pcNew);
	// the value of the attribute was taken care of by OnSplit
	// now, reflect the state in the new node
	l_pcNew->SetLogical(FALSE);

	GetEdgeclass()->AddElement(l_pcNew);

	return l_pcNew;
}

SP_DS_Attribute*
SP_DS_Edge::GetAttribute(const wxString& p_pchName)
{
    if(p_pchName.IsEmpty()) return NULL;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if (p_pchName.Cmp(l_pcAttr->GetName()) == 0)
            return l_pcAttr;
    }

    return NULL;
}

SP_DS_Attribute*
SP_DS_Edge::RemoveAttribute(SP_DS_Attribute* p_pcVal)
{
    if (!p_pcVal || (SP_Find(m_lAttributes, p_pcVal) == m_lAttributes.end()))
        return NULL;

    p_pcVal->SetDataParent(NULL);
    m_lAttributes.remove(p_pcVal);

    return p_pcVal;
}

bool
SP_DS_Edge::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    bool l_bReturn = TRUE;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->SetNetnumber(p_nNewVal, p_nOldVal);
    }

    return (l_bReturn && SP_Data::SetNetnumber(p_nNewVal, p_nOldVal));
}

SP_DELETE_STATE
SP_DS_Edge::RemoveGraphic(unsigned int p_nNetnumber)
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_pcAttr->RemoveGraphic(p_nNetnumber);
    }

    return SP_Data::RemoveGraphic(p_nNetnumber);
}

bool
SP_DS_Edge::CanConnectNodes(SP_Data* p_pcSource, SP_Data* p_pcTarget)
{
    // if one of the parameters is NULL, we take, what we have
    // already stored.
    // Such form is used from within SP_GR_Node::ConsumeXxxxxxEdge
    if (!p_pcSource)
        p_pcSource = m_pcSource;
    if (!p_pcTarget)
        p_pcTarget = m_pcTarget;

    CHECK_POINTER(p_pcSource, return FALSE);
    CHECK_POINTER(p_pcTarget, return FALSE);

    CHECK_POINTER(m_pcEdgeclass, return FALSE);

    SP_DS_Graph* l_pcGraph = m_pcEdgeclass->GetParentGraph();
    CHECK_POINTER(l_pcGraph, return FALSE);

    SP_DS_Netclass* l_pcNetclass = l_pcGraph->GetNetclass();
    CHECK_POINTER(l_pcNetclass, return FALSE);

    return l_pcNetclass->EdgeRequirement(GetEdgeclass(), p_pcSource, p_pcTarget);
}

SP_DS_Edge*
SP_DS_Edge::SetNodes(SP_Data* p_pcSource, SP_Data* p_pcTarget)
{
    CHECK_POINTER(p_pcSource, return NULL);
    CHECK_POINTER(p_pcTarget, return NULL);

    // in case of any of this has the m_pcCoarse member set
    // we don't really get the SP_DS_Node to either of the
    // graphics we passed through the parameters
    m_pcSource = p_pcSource->AddSourceEdge(this);
    m_pcTarget = p_pcTarget->AddTargetEdge(this);

    CHECK_POINTER(m_pcSource, return NULL);
    CHECK_POINTER(m_pcTarget, return NULL);

    return this;
}

SP_Graphic*
SP_DS_Edge::SetNodes(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcGraphic)
{
    CHECK_POINTER(p_pcSource, return NULL);
    CHECK_POINTER(p_pcTarget, return NULL);

    if (!SetNodes(p_pcSource->GetParent(), p_pcTarget->GetParent()))
        return NULL;

    if (!p_pcGraphic)
    {
        CHECK_POINTER(GetGraphics(), return NULL);

        p_pcGraphic = (*GetGraphics()->begin());

        CHECK_POINTER(p_pcGraphic, return NULL);
    }

    if (p_pcSource->AddSourceEdge(p_pcGraphic, m_pcSource) &&
        p_pcTarget->AddTargetEdge(p_pcGraphic, m_pcTarget))
    {
        SP_Graphic* l_pcReturn = CompleteCoarse(p_pcSource, p_pcTarget, p_pcGraphic);
        m_bCoarseCompleted = FALSE;
        return l_pcReturn;
    }

    return NULL;
}

// protected, only to be used from within SetNodes(SP_Graphic*, SP_Graphic*, SP_Graphic*)
SP_Graphic*
SP_DS_Edge::CompleteCoarse(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcSelf)
{
    if (m_bCoarseCompleted)
        return p_pcSelf;

    CHECK_POINTER(p_pcSource, return NULL);
    CHECK_POINTER(p_pcTarget, return NULL);
    CHECK_POINTER(p_pcSource->GetParent(), return NULL);
    CHECK_POINTER(p_pcTarget->GetParent(), return NULL);
    CHECK_POINTER(p_pcSelf, return NULL);

    SP_Graphic* l_pcTarget = p_pcTarget;
    SP_Graphic* l_pcSource = p_pcSource;
    SP_Graphic* l_pcSelf = p_pcSelf;
    // the m_pcTarget member isn't the one, that belongs to p_pcTarget
    if (p_pcTarget->GetParent()->GetCoarse())
    {
        SP_LOGDEBUG(wxT("p_pcTarget->GetParent()->GetCoarse()"));
        p_pcTarget->SetGraphicState(SP_STATE_COARSECOARSE);
        l_pcTarget = (*(m_pcTarget->GetGraphics()->begin()));
        l_pcTarget->SetPosXY((p_pcTarget->GetPosX() + p_pcSource->GetPosX()) / 2.0,
            (p_pcTarget->GetPosY() + p_pcSource->GetPosY()) / 2.0);

        l_pcSource = p_pcSource->GetParent()->AddGraphicInSubnet(m_pcTarget->GetNetnumber());
        if ((l_pcSource->GetPosX() == 0.0) && (l_pcSource->GetPosY() == 0.0))
        {
            l_pcSource->SetPosXY(p_pcSource->GetPosX(), p_pcSource->GetPosY());
            l_pcSource->ShowAttributeGraphic();
        }
        if(p_pcSource->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
        	p_pcSource->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        l_pcSource->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

        l_pcSelf = AddGraphicInSubnet(m_pcTarget->GetNetnumber());
        dynamic_cast<SP_GR_Edge*>(l_pcSelf)->SetControlPoints((wxList*)NULL);
       	if(p_pcSelf->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
       		p_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        l_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

        m_bCoarseCompleted = TRUE;

        // do it again, maybe we just had more coarse on the other end?
        SetNodes(l_pcSource, l_pcTarget, l_pcSelf);
    }
    // the m_pcSource member isn't the one, that belongs to p_pcSource
    if (p_pcSource->GetParent()->GetCoarse())
    {
        SP_LOGDEBUG(wxT("p_pcSource->GetParent()->GetCoarse()"));
        p_pcSource->SetGraphicState(SP_STATE_COARSECOARSE);
        l_pcSource = (*(m_pcSource->GetGraphics()->begin()));
        l_pcSource->SetPosXY((p_pcSource->GetPosX() + p_pcTarget->GetPosX()) / 2.0,
            (p_pcSource->GetPosY() + p_pcTarget->GetPosY()) / 2.0);

        l_pcTarget = p_pcTarget->GetParent()->AddGraphicInSubnet(m_pcSource->GetNetnumber());
        if ((l_pcTarget->GetPosX() == 0.0) && (l_pcTarget->GetPosY() == 0.0))
        {
            l_pcTarget->SetPosXY(p_pcTarget->GetPosX(), p_pcTarget->GetPosY());
            l_pcTarget->ShowAttributeGraphic();
        }
        if(p_pcTarget->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
        	p_pcTarget->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        l_pcTarget->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

        l_pcSelf = AddGraphicInSubnet(m_pcSource->GetNetnumber());
        dynamic_cast<SP_GR_Edge*>(l_pcSelf)->SetControlPoints((wxList*)NULL);
       	if(p_pcSelf->GetGraphicState() != SP_STATE_COARSEBORDERDOWN)
       		p_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        l_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

        m_bCoarseCompleted = TRUE;

        // do it again, maybe we just had more coarse on the other end?
        SetNodes(l_pcSource, l_pcTarget, l_pcSelf);
    }
    if (p_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN ||
    	p_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
    {
    	l_pcSelf = CompleteCoarseBorder(p_pcSource, p_pcTarget, p_pcSelf);
    }
    return l_pcSelf;
}

// protected, only to be used from within CompleteCoarse(SP_Graphic*, SP_Graphic*, SP_Graphic*)
SP_Graphic*
SP_DS_Edge::CompleteCoarseBorder(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcSelf)
{
    if (m_bCoarseCompleted)
        return p_pcSelf;

    CHECK_POINTER(p_pcSource, return NULL);
    CHECK_POINTER(p_pcTarget, return NULL);
    CHECK_POINTER(p_pcSource->GetParent(), return NULL);
    CHECK_POINTER(p_pcTarget->GetParent(), return NULL);
    CHECK_POINTER(p_pcSelf, return NULL);

    SP_Graphic* l_pcTarget = p_pcTarget;
    SP_Graphic* l_pcSource = p_pcSource;
    SP_Graphic* l_pcSelf = p_pcSelf;
    if (p_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
    {
        SP_LOGDEBUG(wxT("p_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN"));
        SP_DS_Graph* l_pcGraph = dynamic_cast<SP_DS_Node*>(m_pcSource)->GetNodeclass()->GetParentGraph();
        unsigned int l_nNr = p_pcSource->GetNetnumber();
        SP_Data* l_pcCoarse = l_pcGraph->GetCoarseNode(l_nNr);

        l_pcSource = m_pcSource->AddGraphicInSubnet(l_pcCoarse->GetNetnumber());
        l_pcTarget = l_pcCoarse->GetCoarse()->GetGraphic();
        l_pcSelf = AddGraphicInSubnet(l_pcCoarse->GetNetnumber());
        dynamic_cast<SP_GR_Edge*>(l_pcSelf)->SetControlPoints((wxList*)NULL);
        l_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        p_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
        l_pcSource->AddSourceEdge(l_pcSelf, m_pcSource);
        l_pcTarget->AddTargetEdge(l_pcSelf, m_pcTarget);

        l_pcSelf->AddToCanvas(dynamic_cast<SP_GUI_Canvas*>(l_pcTarget->GetPrimitive()->GetCanvas()));

        if(l_pcSource->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
        {
        	CompleteCoarseBorder(l_pcSource, l_pcTarget, l_pcSelf);
        }
    }
    if (p_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
    {
        SP_LOGDEBUG(wxT("p_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN"));
        SP_DS_Graph* l_pcGraph = dynamic_cast<SP_DS_Node*>(m_pcTarget)->GetNodeclass()->GetParentGraph();
        unsigned int l_nNr = p_pcTarget->GetNetnumber();
        SP_Data* l_pcCoarse = l_pcGraph->GetCoarseNode(l_nNr);

        l_pcTarget = m_pcTarget->AddGraphicInSubnet(l_pcCoarse->GetNetnumber());

        l_pcSource = l_pcCoarse->GetCoarse()->GetGraphic();

        l_pcSelf = AddGraphicInSubnet(l_pcCoarse->GetNetnumber());
        dynamic_cast<SP_GR_Edge*>(l_pcSelf)->SetControlPoints((wxList*)NULL);
        l_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERTOP);
        p_pcSelf->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
        l_pcSource->AddSourceEdge(l_pcSelf, m_pcSource);
        l_pcTarget->AddTargetEdge(l_pcSelf, m_pcTarget);

        l_pcSelf->AddToCanvas(dynamic_cast<SP_GUI_Canvas*>(l_pcTarget->GetPrimitive()->GetCanvas()));

        if(l_pcTarget->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
        {
        	CompleteCoarseBorder(l_pcSource, l_pcTarget, l_pcSelf);
        }
    }
    return l_pcSelf;
}

bool
SP_DS_Edge::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if (l_pcAttr->GetAttributeType() == p_eVal)
            return TRUE;
    }
    return FALSE;
}

SP_DS_Attribute*
SP_DS_Edge::GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal)
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if (l_pcAttr->GetAttributeType() == p_eVal)
            return l_pcAttr;
    }
    return NULL;
}

bool
SP_DS_Edge::SqueezeIdAttribute()
{
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
            return dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr)->Squeeze();
    }
    return FALSE;
}

bool
SP_DS_Edge::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly, double p_nX, double p_nY, int p_nKeys)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);

    bool l_bReturn = TRUE;
    SP_ListGraphic* l_plGraphic = GetGraphics();
    if (l_plGraphic)
    {
		for (SP_Graphic* l_pcGr : *l_plGraphic)
        {
            l_bReturn &= l_pcGr->AddToCanvas(p_pcCanvas, p_nX, p_nY, p_nKeys);
        }
    }

    return l_bReturn && Update(p_bLocalOnly);
}


bool
SP_DS_Edge::CheckIntegrity()
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= (l_pcAttr)->CheckIntegrity();
	}

	SP_ListGraphic* l_plGraphic = GetGraphics();
	SP_SetUInt l_Netnumbers;
	if (l_plGraphic)
	{
		if(GetNetnumber() != 1)
		{
			if(!GetEdgeclass()->GetParentGraph()->GetCoarseNode(GetNetnumber()))
			{
				SetNetnumber(1, GetNetnumber());
			}
		}
		for (SP_Graphic* l_pcGraphic : *l_plGraphic)
		{
			l_bReturn &= l_pcGraphic->CheckIntegrity();
			l_Netnumbers.insert(l_pcGraphic->GetNetnumber());
		}
	}

	if(!l_Netnumbers.empty())
	{
		if(l_Netnumbers.find(GetNetnumber()) == l_Netnumbers.end())
		{
			SetNetnumber(*(l_Netnumbers.begin()), GetNetnumber());
		}
	}

	return l_bReturn;
}


bool
SP_DS_Edge::GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter)
{
    if (!p_plGraphics)
        return FALSE;

    if (!(GetElementType() & p_eFilter))
        return TRUE;

    bool l_bReturn = TRUE;
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);
    }

    return SP_Data::GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter) && l_bReturn;
}

bool
SP_DS_Edge::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    bool l_bReturn = TRUE;
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
    }

    return SP_Data::OnFlatten(p_nToNet, p_nFromNet, p_plGraphics) && l_bReturn;
}

bool
SP_DS_Edge::ShowAttributeGraphic(SP_GR_Edge* p_pcGraphic)
{
    bool l_bReturn = TRUE;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->ShowGraphic(p_pcGraphic);
    }

    return l_bReturn;
}

bool
SP_DS_Edge::AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
    CHECK_POINTER(p_ptmAttributes, return FALSE);

    SP_LOGDEBUG(wxString::Format(wxT("Edge %p with source %p target %p"), this,
				   m_pcSource, m_pcTarget));

    // iterate over the list off attributes and insert them into the value of
    // the map, i.e. the list of attributes under the key, combined of
    // wxT("<name of the class>|<name of the attribute>|<type of the attribute>")
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
    	wxString l_sKey = SP_Core::Instance()->GetDialogWidgetKey(l_pcAttr);
        //(*p_ptmAttributes)[l_sKey].push_back(l_pcAttr);
        SP_ListAttribute& l_lAttr = (*p_ptmAttributes)[l_sKey];
        if(SP_Find(l_lAttr, l_pcAttr) == l_lAttr.end())
        {
        	l_lAttr.push_back(l_pcAttr);
        }
    }

    return TRUE;
}

bool
SP_DS_Edge::Remove()
{
    bool l_bReturn = TRUE;

    if (m_pcSource)
        m_pcSource->RemoveSourceEdge(this);

    if (m_pcTarget)
        m_pcTarget->RemoveTargetEdge(this);

    // remove me from the EdgeClass list
    if (m_pcEdgeclass)
        l_bReturn &= m_pcEdgeclass->RemoveElement(this);

    return l_bReturn;
}

const wxString
SP_DS_Edge::GetClassName()
{
    if (m_pcEdgeclass)
        return m_pcEdgeclass->GetName();
    return wxT("");
}

SP_ElementClass*
SP_DS_Edge::GetClassObject()
{
	return m_pcEdgeclass;
}

bool
SP_DS_Edge::Update(bool p_bLocalOnly)
{
    bool l_bReturn = TRUE;

    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
        l_bReturn &= l_pcAttr->Update(p_bLocalOnly);
    }

    return l_bReturn && SP_Data::Update(p_bLocalOnly);
}





