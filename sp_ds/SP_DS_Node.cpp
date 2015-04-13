//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Attribute.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_core/SP_Core.h"

SP_DS_Node::SP_DS_Node(SP_DS_Nodeclass* p_pcNodeclass, unsigned int p_nNetnumber) :
	SP_Data(SP_ELEMENT_NODE)
{
	m_pcNodeclass = p_pcNodeclass;
	SetNetnumber(p_nNetnumber);
}

SP_DS_Node::~SP_DS_Node()
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		wxDELETE(l_pcAttr);
	}
	m_lAttributes.clear();

	RemoveAllGraphics(true);
}

SP_DS_Node* SP_DS_Node::Clone(bool p_bCloneGr)
{
	SP_DS_Node* l_pcNewNode = new SP_DS_Node(GetNodeclass(), GetNetnumber());

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcNewNode->AddAttribute(l_pcAttr->Clone(p_bCloneGr));
	}
	l_pcNewNode->SetLogical(GetLogical());

	if (m_pcCoarse)
		l_pcNewNode->SetCoarse(m_pcCoarse->Clone());

	// don't clone the list of graphics, too
	if (!p_bCloneGr)
		return l_pcNewNode;

	SP_ListGraphic* l_plGraphic = GetGraphics();
	SP_Graphic* l_pcTemp;
	if (l_plGraphic)
	{
		// check all graphics for having the proper type
		for (SP_Graphic* l_pcGr : *l_plGraphic)
		{
			if (l_pcGr->GetGraphicType() == SP_GRAPHIC_NODE)
			{
				l_pcTemp = l_pcGr->Clone(l_pcNewNode);
				if (l_pcTemp)
					l_pcTemp->ShowAttributeGraphic();

				l_pcNewNode->AddGraphic(l_pcTemp);
			}
			else
			{
				// found a wrong associated graphic
				// should never happen, really
				wxASSERT_MSG(FALSE, wxT("Something went wrong in SP_DS_Node::Clone()"));
				wxDELETE(l_pcNewNode);
				SetLastError(wxT("Wrong type for the graphic associated in SP_DS_Node"));
				return NULL;
			}
		}
	}

	return l_pcNewNode;
}

bool SP_DS_Node::Equals(SP_Data* p_pcVal)
{
	if (!p_pcVal || (p_pcVal->GetElementType() != m_eElementType)
			|| !GetNodeclass())
		return FALSE;

	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(p_pcVal);

	if (!l_pcNode->GetNodeclass())
		return FALSE;

	if (GetNodeclass()->GetName()== l_pcNode->GetNodeclass()->GetName())
		return TRUE;

	return FALSE;
}

SP_Data* SP_DS_Node::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
		SP_Graphic* p_pcGraphic,
		SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
		SP_MapData2Data* p_pmData2Data)
{
	CHECK_POINTER(p_ppcGraph, return NULL);
	CHECK_POINTER(p_pmData2Data, return NULL);

	SP_DS_Node* l_pcNewNode;
	// try to find the mapping to myself in the parameter
	SP_MapData2Data::const_iterator l_it = p_pmData2Data->find(this);
	if (l_it == p_pmData2Data->end())
	{
		// if not found, clone a new one and insert it into the map
		// clone the node without any graphics, because there can be
		// much more than we have selected. Check p_pcGraphic afterwards.
		l_pcNewNode = this->Clone(FALSE);
		// new element insert into the copy graph
		(*p_ppcGraph)->AddElement(l_pcNewNode);
		p_pmData2Data->insert(make_pair(this,l_pcNewNode));
	}
	else
	{
		l_pcNewNode = dynamic_cast<SP_DS_Node*>((*l_it).second);
	}

	if (m_pcCoarse)
	{
		wxASSERT(l_pcNewNode->GetCoarse());
		SP_DS_Graph* l_pcGraph = l_pcNewNode->GetCoarse()->GetGraph();
		m_pcCoarse->AddToCopyMap(&l_pcGraph, p_pcGraphic, p_pmGraphic2Graphic,
				p_pmData2Data);
	}

	return SynchronizeChildren(l_pcNewNode, p_pcGraphic, p_pmGraphic2Graphic);
}

bool SP_DS_Node::SynchronizeAttributes()
{
	if (!m_bLogical)
		return TRUE;

	// trick or treat
	// we cloned ourself with all the logic stuff set, but we need to
	// ensure, that we aren't the node, to be found while in SP_DS_Graph::JoinElements
	m_bLogical = FALSE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LOGIC)
		{
			// immediately exit after this call, because we can be deleted appearently
			return dynamic_cast<SP_DS_LogicAttribute*>(l_pcAttr)->JoinElements();
		}
	}

	// if we didn't found an attribute, reset ourself
	m_bLogical = TRUE;

	return FALSE;
}

SP_Data* SP_DS_Node::SynchronizeChildren(SP_DS_Node* p_pcNode,
		SP_Graphic* p_pcGraphic,
		SP_MapGraphic2Graphic* p_pmGraphic2Graphic)
{
	CHECK_POINTER(p_pcNode, return NULL);
	CHECK_POINTER(p_pcGraphic, return NULL);
	CHECK_POINTER(p_pcGraphic->GetGraphicChildren(), return NULL);
	CHECK_POINTER(p_pmGraphic2Graphic, return NULL);

	SP_Data* l_pcData= NULL;
	SP_DS_Attribute* l_pcAttr= NULL;
	SP_DS_Attribute* l_pcNewAttr= NULL;
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = p_pcGraphic->GetGraphicChildren()->begin(); l_Iter != p_pcGraphic->GetGraphicChildren()->end(); ++l_Iter)
	{
		l_pcData = (*l_Iter)->GetParent();
		if (!l_pcData)
		{
			SP_LOGDEBUG(wxT("SP_DS_Node::SynchronizeChildren, ")
					wxT("SP_Graphic::GetParent() returned null"));
			continue;
		}
		if (l_pcData->GetElementType() != SP_ELEMENT_ATTRIBUTE)
			continue;

		l_pcAttr = dynamic_cast<SP_DS_Attribute*>(l_pcData);
		l_pcNewAttr = p_pcNode->GetAttribute(l_pcAttr->GetName());
		if (l_pcNewAttr)
		{
			// insert the graphic association in the graphic map
			(*p_pmGraphic2Graphic)[(*l_Iter)] = l_pcNewAttr->AddGraphic((*l_Iter)->Clone(l_pcNewAttr));
		}
	}

	return p_pcNode;
}

bool SP_DS_Node::ResetNetnumbers(
		SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
	CHECK_POINTER(p_pmNet2Net, return FALSE);
	CHECK_POINTER(p_pcToGraph, return FALSE);

	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
	}

	return (SP_Data::ResetNetnumbers(p_pmNet2Net, p_pcToGraph) && l_bReturn);
}

SP_Graphic* SP_DS_Node::NewGraphicInSubnet(unsigned int p_nNetnumber)
{
	SP_Graphic* l_pcOwn = SP_Data::NewGraphicInSubnet(p_nNetnumber);
	CHECK_POINTER(l_pcOwn, return NULL);

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcOwn->AddGraphicChildren(l_pcAttr->NewGraphicInSubnet(p_nNetnumber));
	}

	return l_pcOwn;
}

SP_DS_Nodeclass* SP_DS_Node::GetNodeclass()
{
	return m_pcNodeclass;
}

SP_DS_Nodeclass* SP_DS_Node::SetNodeclass(SP_DS_Nodeclass* p_pcVal)
{
	m_pcNodeclass = p_pcVal;

	// what about nodes with m_pcCoarse member set?
	// inform the m_pcCoarse member of the new nodeclass
	// that means, that we may have to reset the graph we point to
	if (m_pcCoarse)
		m_pcCoarse->ChangedClass(p_pcVal);

	return m_pcNodeclass;
}

bool SP_DS_Node::ClassNameChanged(const wxString& p_pchName)
{
	if(p_pchName.IsEmpty()) return FALSE;

	wxString l_sKey;
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

SP_DS_Attribute* SP_DS_Node::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
	CHECK_POINTER(p_pcAttr, return NULL);
	// alert, parameter already is associated to another data-element
	CHECK_BOOL((p_pcAttr->GetParent() == NULL), return NULL);

	//if p_pcAttr already exists, don't add it again
	bool l_bAdd = true;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if ((*p_pcAttr).Equals(l_pcAttr))
			l_bAdd = false;
	}
	if (!l_bAdd)
		return 0;

	p_pcAttr->SetDataParent(this);
	//p_pcAttr->SetNetnumber(GetNetnumber());

	m_lAttributes.push_back(p_pcAttr);

	return p_pcAttr;
}

SP_DS_Attribute* SP_DS_Node::GetAttribute(const wxString& p_pchName)
{
	if(p_pchName.IsEmpty()) return NULL;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (p_pchName.Cmp(l_pcAttr->GetName()) == 0)
			return l_pcAttr;
	}

	return NULL;
}

SP_DS_Attribute* SP_DS_Node::RemoveAttribute(SP_DS_Attribute* p_pcVal)
{
	if (!p_pcVal || (SP_Find(m_lAttributes, p_pcVal) == m_lAttributes.end()))
		return NULL;

	p_pcVal->SetDataParent(NULL);
	m_lAttributes.remove(p_pcVal);

	return p_pcVal;
}

bool SP_DS_Node::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->SetNetnumber(p_nNewVal, p_nOldVal);
	}

	return (l_bReturn && SP_Data::SetNetnumber(p_nNewVal, p_nOldVal));
}

SP_DELETE_STATE SP_DS_Node::RemoveGraphic(unsigned int p_nNetnumber)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_pcAttr->RemoveGraphic(p_nNetnumber);
	}

	return SP_Data::RemoveGraphic(p_nNetnumber);
}

SP_Data* SP_DS_Node::GetSibling(const wxString& p_pchRefAttribute)
{
	CHECK_POINTER(GetNodeclass(), return NULL);

	SP_DS_Node* l_pcNode = GetNodeclass()->GetRelative(this, p_pchRefAttribute, TRUE);

	return l_pcNode;
}

bool SP_DS_Node::MergeData(SP_Data* p_pcElement)
{
	CHECK_POINTER(p_pcElement, return FALSE);
	CHECK_BOOL((p_pcElement->GetElementType() == SP_ELEMENT_NODE), return FALSE);

	bool l_bReturn= TRUE;
	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(p_pcElement);
	SP_ListAttribute::const_iterator l_AttrIter;
	for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
	{
		SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
		l_bReturn
				&= l_pcAttribute->MergeData(l_pcNode->GetAttribute(l_pcAttribute->GetName()));
	}
	// edges
	l_bReturn &= l_pcNode->SetNewSource(this);
	l_bReturn &= l_pcNode->SetNewTarget(this);

	l_bReturn &= SP_Data::MergeData(l_pcNode);

	return l_bReturn && l_pcNode->GetNodeclass()->RemoveElement(l_pcNode);
}

bool SP_DS_Node::Split(SP_ListGraphic* p_plGraphics)
{
	CHECK_POINTER(p_plGraphics, return TRUE);
	CHECK_POINTER(GetGraphics(), return TRUE);
	CHECK_POINTER(GetNodeclass(), return FALSE);

	bool l_bReturn= TRUE;
	SP_ListGraphic* l_plGraphics = GetGraphics();
	SP_ListGraphic::iterator l_Iter;

	for (l_Iter = p_plGraphics->begin(); l_Iter != p_plGraphics->end(); ++l_Iter)
	{
		SP_ListGraphic::iterator l_itGr = SP_Find(*l_plGraphics, *l_Iter);
		if (l_itGr != l_plGraphics->end())
		{
			l_bReturn &= GetNodeclass()->AddElement(Split(*l_itGr));
		}
	}

	return l_bReturn;
}

SP_DS_Node* SP_DS_Node::Split(SP_Graphic* p_pcGraphic)
{
	SP_DS_Node* l_pcNew= NULL;
	SP_ListAttribute::const_iterator l_AttrIter;

	CHECK_POINTER(p_pcGraphic, return this);
	CHECK_POINTER(GetNodeclass(), return NULL);

	// if this node has only one graphic left, we don't need to create a new one
	// or it is the last ordinary node and all other graphics are coarse ones
	SP_ListGraphic::iterator l_Iter;
	int l_nOrdinary = 0;
	int l_nCoarseBorderTop = 0;
	for (l_Iter = GetGraphics()->begin(); l_Iter != GetGraphics()->end(); ++l_Iter)
	{
		if ((*l_Iter)->GetGraphicState() == SP_STATE_NORMAL)
		{
			l_nOrdinary++;
		}
		else if ((*l_Iter)->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
		{
			l_nCoarseBorderTop++;
		}
	}
	if ((l_nOrdinary == 0 && l_nCoarseBorderTop == 1)
		|| (l_nOrdinary == 1 && GetGraphics()->size() == 1))
	{
		SetLogical(FALSE);
		return this;
	}

	// clone new node based on this, but without any graphics
	l_pcNew = this->Clone(FALSE);
	SP_ListGraphic l_lGraphics;
	GetCoupledGraphics(l_lGraphics, p_pcGraphic);
	for(l_Iter = l_lGraphics.begin(); l_Iter != l_lGraphics.end(); ++l_Iter)
	{
		SP_Graphic* l_pcGraphic = *l_Iter;
		for (l_AttrIter = m_lAttributes.begin(); l_AttrIter != m_lAttributes.end(); ++l_AttrIter)
		{
			SP_DS_Attribute* l_pcAttribute = (*l_AttrIter);
			l_pcAttribute->Split(l_pcNew->GetAttribute(l_pcAttribute->GetName()),
					l_pcGraphic->GetGraphicChildren());
		}
		// important to be called before p_pcGraphic->SetDataParent(l_pcNew);
		SplitEdges(l_pcGraphic, l_pcNew);

		SP_Data::RemoveGraphic(l_pcGraphic, FALSE);
		l_pcNew->AddGraphic(l_pcGraphic);
		l_pcGraphic->SetDataParent(l_pcNew);
	}
	// the value of the attribute was taken care of by OnSplit
	// now, reflect the state in the new node
	l_pcNew->SetLogical(FALSE);

	return l_pcNew;
}

void SP_DS_Node::GetCoupledGraphics(SP_ListGraphic& p_plGraphics, SP_Graphic* p_pcGraphic)
{
	p_plGraphics.push_back(p_pcGraphic);

	if(p_pcGraphic->GetGraphicState() != SP_STATE_NORMAL)
	{
		SP_ListEdge::const_iterator l_itEdge;
		for(l_itEdge = GetSourceEdges()->begin(); l_itEdge != GetSourceEdges()->end(); ++l_itEdge)
		{
			SP_DS_Edge* l_pcEdge = *l_itEdge;
			SP_ListGraphic::const_iterator l_itGr;
			bool l_bAddGraphics = false;
			for(l_itGr = l_pcEdge->GetGraphics()->begin(); l_itGr != l_pcEdge->GetGraphics()->end(); ++l_itGr)
			{
				SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>(*l_itGr);
				if(l_pcGrEdge->GetSource() == p_pcGraphic)
				{
					l_bAddGraphics = true;
				}
			}
			if(l_bAddGraphics)
			{
				for(l_itGr = l_pcEdge->GetGraphics()->begin(); l_itGr != l_pcEdge->GetGraphics()->end(); ++l_itGr)
				{
					SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>(*l_itGr);
					if(l_pcGrEdge->GetSource() != p_pcGraphic)
					{
						p_plGraphics.push_back(l_pcGrEdge->GetSource());
					}
				}
			}
		}
		for(l_itEdge = GetTargetEdges()->begin(); l_itEdge != GetTargetEdges()->end(); ++l_itEdge)
		{
			SP_DS_Edge* l_pcEdge = *l_itEdge;
			SP_ListGraphic::const_iterator l_itGr;
			bool l_bAddGraphics = false;
			for(l_itGr = l_pcEdge->GetGraphics()->begin(); l_itGr != l_pcEdge->GetGraphics()->end(); ++l_itGr)
			{
				SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>(*l_itGr);
				if(l_pcGrEdge->GetTarget() == p_pcGraphic)
				{
					l_bAddGraphics = true;
				}
			}
			if(l_bAddGraphics)
			{
				for(l_itGr = l_pcEdge->GetGraphics()->begin(); l_itGr != l_pcEdge->GetGraphics()->end(); ++l_itGr)
				{
					SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>(*l_itGr);
					if(l_pcGrEdge->GetTarget() != p_pcGraphic)
					{
						p_plGraphics.push_back(l_pcGrEdge->GetTarget());
					}
				}
			}
		}

	}
}

// protected method, only to be used from within
// SP_DS_Node::Split(SP_Graphic*)
bool SP_DS_Node::SplitEdges(SP_Graphic* p_pcGraphic, SP_DS_Node* p_pcNode)
{
	SP_GR_Node* l_pcGrNode= NULL;

	CHECK_POINTER(p_pcGraphic, return FALSE);
	CHECK_POINTER(p_pcNode, return FALSE);

	if (p_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE)
		l_pcGrNode = dynamic_cast<SP_GR_Node*>(p_pcGraphic);

	CHECK_POINTER(l_pcGrNode, return FALSE);

	return l_pcGrNode->SplitEdges(p_pcNode);
}

bool SP_DS_Node::HasEqualAttribute(SP_DS_Attribute* p_pcAttribute)
{
	CHECK_POINTER(p_pcAttribute, return FALSE);
	CHECK_POINTER(GetAttribute(p_pcAttribute->GetName()), return FALSE);

	wxString l_sFirst = GetAttribute(p_pcAttribute->GetName())->GetValueString();
	wxString l_sSecond = p_pcAttribute->GetValueString();

	bool l_bReturn = l_sFirst.IsSameAs(l_sSecond);

	return l_bReturn;
}

bool SP_DS_Node::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return TRUE;
	}
	return FALSE;
}

SP_DS_Attribute* SP_DS_Node::GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return l_pcAttr;
	}
	return NULL;
}

bool SP_DS_Node::SqueezeIdAttribute()
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
			return dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr)->Squeeze();
	}
	return FALSE;
}

bool SP_DS_Node::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly,
		double p_nX, double p_nY, int p_nKeys)
{
	CHECK_POINTER(p_pcCanvas, return FALSE);

	bool l_bReturn= TRUE;
	SP_ListGraphic* l_plGraphic = GetGraphics();
	if (l_plGraphic)
	{
		for (SP_Graphic* l_pcGr : *l_plGraphic)
		{
			if (m_pcCoarse)
				l_pcGr->SetGraphicState(SP_STATE_COARSECOARSE);
			l_bReturn &= l_pcGr->AddToCanvas(p_pcCanvas, p_nX, p_nY, p_nKeys);
		}
	}

	return l_bReturn && Update(p_bLocalOnly);
}

bool SP_DS_Node::CheckIntegrity()
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
			if(!GetNodeclass()->GetParentGraph()->GetCoarseNode(GetNetnumber()))
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

bool SP_DS_Node::GetGraphicsInNet(SP_ListGraphic* p_plGraphics,
		unsigned int p_nNet, const unsigned int p_eFilter)
{
	if (!p_plGraphics)
		return FALSE;

	if (!(GetElementType() & p_eFilter))
		return TRUE;

	bool l_bReturn= TRUE;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);
	}

	return SP_Data::GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter)
			&& l_bReturn;
}

bool SP_DS_Node::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet,
		SP_ListGraphic* p_plGraphics)
{
	bool l_bReturn= TRUE;
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
	}

	return SP_Data::OnFlatten(p_nToNet, p_nFromNet, p_plGraphics) && l_bReturn;
}

bool SP_DS_Node::ShowAttributeGraphic(SP_GR_Node* p_pcGraphic)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->ShowGraphic(p_pcGraphic);
	}

	return l_bReturn;
}

bool SP_DS_Node::AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
	// debug
	SP_LOGDEBUG(wxString::Format(wxT("Node %p with "), this) << m_lSourceEdges.size()
			<< wxT(" Source and ") << m_lTargetEdges.size() << wxT(" Target edges."));

	CHECK_POINTER(p_ptmAttributes, return FALSE);

	// iterate over the list of attributes and insert them into the value of
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

bool SP_DS_Node::Remove()
{
	bool l_bReturn= TRUE;
	// inform the edges
	SP_ListEdge::const_iterator l_itEdge = m_lSourceEdges.begin();
	while (l_itEdge != m_lSourceEdges.end())
	{
		l_bReturn &= RemoveSourceEdge((*l_itEdge));
		l_itEdge = m_lSourceEdges.begin();
	}

	l_itEdge = m_lTargetEdges.begin();
	while (l_itEdge != m_lTargetEdges.end())
	{
		l_bReturn &= RemoveTargetEdge((*l_itEdge));
		l_itEdge = m_lTargetEdges.begin();
	}
	// remove me from the NodeClass list
	if (m_pcNodeclass)
		l_bReturn &= m_pcNodeclass->RemoveElement(this);

	return l_bReturn;
}

SP_DS_Node* SP_DS_Node::AddSourceEdge(SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcEdge, return NULL);

	// check the m_pcCoarse member
	if (m_pcCoarse)
		return m_pcCoarse->AddSourceEdge(p_pcEdge);

	SP_ListEdge::iterator l_Iter = m_lSourceEdges.begin();
	for(; l_Iter != m_lSourceEdges.end(); ++l_Iter)
	{
		SP_DS_Edge* l_pcEdge = *l_Iter;
		if(l_pcEdge->GetTarget() == p_pcEdge->GetTarget()
			&& (l_pcEdge->GetTarget()->GetLogical()
					|| p_pcEdge->GetTarget()->GetLogical())
			&& l_pcEdge->GetEdgeclass() == p_pcEdge->GetEdgeclass()
			&& l_pcEdge != p_pcEdge)
		{
			if(l_pcEdge->MergeData(p_pcEdge))
			{
				//wxDELETE(p_pcEdge);
			}
			return this;
		}
	}

	if (SP_Find(m_lSourceEdges, p_pcEdge) == m_lSourceEdges.end())
		m_lSourceEdges.push_back(p_pcEdge);

	p_pcEdge->SetSource(this);

	return this;
}

SP_DS_Node* SP_DS_Node::AddTargetEdge(SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcEdge, return NULL);

	// check the m_pcCoarse member
	if (m_pcCoarse)
		return m_pcCoarse->AddTargetEdge(p_pcEdge);

	SP_ListEdge::iterator l_Iter = m_lTargetEdges.begin();
	for(; l_Iter != m_lTargetEdges.end(); ++l_Iter)
	{
		SP_DS_Edge* l_pcEdge = *l_Iter;
		if(l_pcEdge->GetSource() == p_pcEdge->GetSource()
			&& (l_pcEdge->GetSource()->GetLogical()
					|| p_pcEdge->GetSource()->GetLogical())
			&& l_pcEdge->GetEdgeclass() == p_pcEdge->GetEdgeclass()
			&& l_pcEdge != p_pcEdge)
		{
			if(l_pcEdge->MergeData(p_pcEdge))
			{
				//wxDELETE(p_pcEdge);
			}
			return this;
		}
	}

	if (SP_Find(m_lTargetEdges, p_pcEdge) == m_lTargetEdges.end())
		m_lTargetEdges.push_back(p_pcEdge);

	p_pcEdge->SetTarget(this);

	return this;
}

bool SP_DS_Node::RemoveSourceEdge(SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcEdge, return FALSE);

	p_pcEdge->SetSource(NULL);
	m_lSourceEdges.remove(p_pcEdge);

	return TRUE;
}

bool SP_DS_Node::RemoveTargetEdge(SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcEdge, return FALSE);

	p_pcEdge->SetTarget(NULL);
	m_lTargetEdges.remove(p_pcEdge);

	return TRUE;
}

bool SP_DS_Node::SetNewSource(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return TRUE);

	bool l_bReturn= TRUE;
	SP_ListEdge::const_iterator l_Iter = m_lSourceEdges.begin();
	while (l_Iter != m_lSourceEdges.end())
	{
		SP_DS_Edge* l_pcEdge = (*l_Iter);
		l_bReturn &= RemoveSourceEdge(l_pcEdge);
		l_bReturn &= (p_pcNode->AddSourceEdge(l_pcEdge) != NULL);

		l_Iter = m_lSourceEdges.begin();
	}
	return l_bReturn;
}

bool SP_DS_Node::SetNewTarget(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return TRUE);

	bool l_bReturn= TRUE;
	SP_ListEdge::const_iterator l_Iter = m_lTargetEdges.begin();
	while (l_Iter != m_lTargetEdges.end())
	{
		SP_DS_Edge* l_pcEdge = (*l_Iter);
		l_bReturn &= RemoveTargetEdge(l_pcEdge);
		l_bReturn &= (p_pcNode->AddTargetEdge(l_pcEdge) != NULL);

		l_Iter = m_lTargetEdges.begin();
	}
	return l_bReturn;
}

bool SP_DS_Node::IsSourceOf(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return TRUE);

	SP_ListEdge::const_iterator l_Iter = m_lSourceEdges.begin();
	while (l_Iter != m_lSourceEdges.end())
	{
		SP_DS_Edge* l_pcEdge = (*l_Iter);
		if(l_pcEdge->GetTarget() == p_pcNode)
		{
			return true;
		}
		l_Iter++;
	}
	return false;
}

bool SP_DS_Node::IsTargetOf(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return TRUE);

	SP_ListEdge::const_iterator l_Iter = m_lTargetEdges.begin();
	while (l_Iter != m_lTargetEdges.end())
	{
		SP_DS_Edge* l_pcEdge = (*l_Iter);
		if(l_pcEdge->GetSource() == p_pcNode)
		{
			return true;
		}
		l_Iter++;
	}
	return false;
}

const wxString SP_DS_Node::GetClassName()
{
	if (m_pcNodeclass)
		return m_pcNodeclass->GetName();
	return wxT("");
}

SP_ElementClass*
SP_DS_Node::GetClassObject()
{
	return m_pcNodeclass;
}

bool SP_DS_Node::Update(bool p_bLocalOnly)
{
	bool l_bReturn= TRUE;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		l_bReturn &= l_pcAttr->Update(p_bLocalOnly);
	}

	return l_bReturn && SP_Data::Update(p_bLocalOnly);
}

