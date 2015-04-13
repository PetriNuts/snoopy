//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Animator.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/dialogs/SP_DLG_GraphProperties.h"

#define CHECK_ENV(RET) \
    if (!m_pcPrototype) \
        RET;

SP_DS_Nodeclass::SP_DS_Nodeclass(SP_DS_Graph* p_pcGraph, const wxString& p_pchName, LongPtr p_nIdCounter)
:SP_ElementClass(SP_ELEMENT_NODECLASS, p_pchName, p_nIdCounter),
m_pcPrototype(NULL)
{
    m_pcPrototype = new SP_DS_Node(this);
    m_pcGraph = p_pcGraph;
}

SP_DS_Nodeclass::~SP_DS_Nodeclass()
{
    if (m_pcPrototype)
        wxDELETE(m_pcPrototype);

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); l_itElement++)
    {
        SP_DS_Node* l_pcNode = (*l_itElement);
        wxDELETE(l_pcNode);
    }
    m_lElements.clear();
}

SP_DS_Node*
SP_DS_Nodeclass::GetPrototype()
{
    return m_pcPrototype;
}

SP_DS_Node*
SP_DS_Nodeclass::NewElement(unsigned int p_nNetnumber, SP_DS_Node* p_pcCloneRef)
{
    CHECK_ENV(return NULL);

    SP_DS_Graph* l_pcGraph = this->GetParentGraph();
    CHECK_POINTER(l_pcGraph, return NULL);

    SP_DS_Netclass* l_pcNetclass = l_pcGraph->GetNetclass();
    CHECK_POINTER(l_pcNetclass, return NULL);

    if (!p_pcCloneRef)
        p_pcCloneRef = m_pcPrototype;

    SP_DS_Node* l_pcNewNode = p_pcCloneRef->Clone();
    l_pcNewNode->SetNetnumber(p_nNetnumber);

    // check the NodeRequirement of the Netclass
    if (!l_pcNetclass->NodeRequirement(l_pcNewNode))
    {
        wxDELETE(l_pcNewNode);
        return NULL;
    }

    AddElement(l_pcNewNode);

    return l_pcNewNode;
}

bool
SP_DS_Nodeclass::AddElement(SP_DS_Node* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return TRUE);
	if (p_pcVal->GetNodeclass() != this)
	{
		p_pcVal->GetNodeclass()->RemoveElement(p_pcVal);
		p_pcVal->SetNodeclass(this);
	}
	if(p_pcVal->GetCountLists() == 0)
	{
		p_pcVal->IncreaseCountLists();
		m_lElements.push_back(p_pcVal);
	}
	else
	{
		wxString l_sMsg;
		l_sMsg << p_pcVal->GetClassName() << wxT(" with ID: ") << p_pcVal->GetId() << wxT(" was already in the list ");
		SP_LOGDEBUG(l_sMsg);
	}
    // whether we already had it in the list or not, now its in
    return TRUE;
}

bool
SP_DS_Nodeclass::RemoveElement(SP_DS_Node* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return TRUE);

    p_pcVal->DecreaseCountLists();
    m_lElements.remove(p_pcVal);

    return TRUE;
}

bool
SP_DS_Nodeclass::RemoveElements(unsigned int p_nNetnumber, SP_ListData* p_plCollect)
{
    CHECK_POINTER(p_plCollect, return FALSE);

    SP_ListNode l_lDeadElements;
    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
    	SP_DS_Node* l_pcNode = *l_itElement;
        if (l_pcNode->RemoveGraphic(p_nNetnumber) == SP_NO_MORE_OBJECTS)
        {
            p_plCollect->push_back(dynamic_cast<SP_Data*>(l_pcNode));
            l_lDeadElements.push_back(l_pcNode);
        }
    }
    // clean up the local element list
    for (l_itElement = l_lDeadElements.begin(); l_itElement != l_lDeadElements.end(); ++l_itElement)
        RemoveElement((*l_itElement));

    return TRUE;
}

SP_DS_Node*
SP_DS_Nodeclass::GetRelative(SP_DS_Node* p_pcNode, const wxString& p_pchRefAttribute, bool p_bLogic)
{
    CHECK_POINTER(p_pcNode, return NULL);
    if(p_pchRefAttribute.IsEmpty()) return NULL;
    CHECK_POINTER(p_pcNode->GetAttribute(p_pchRefAttribute), return NULL);

    SP_DS_Attribute* l_pcAttribute = p_pcNode->GetAttribute(p_pchRefAttribute);

    SP_ListNode::const_iterator l_Iter;
    for (l_Iter = m_lElements.begin(); l_Iter != m_lElements.end(); ++l_Iter)
    {
        SP_DS_Node* l_pcNode = (*l_Iter);
        if (l_pcNode == p_pcNode)
            continue;

        if (l_pcNode->HasEqualAttribute(l_pcAttribute) &&
            (p_bLogic?l_pcNode->GetLogical():TRUE))
            return l_pcNode;
    }
    return NULL;
}

bool
SP_DS_Nodeclass::ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg)
{
    CHECK_POINTER(p_pcDlg, return FALSE);
    CHECK_POINTER(m_pcPrototype, return FALSE);

    // iterating through the list of attributes of the coarsenode
    const SP_ListAttribute* l_plAttr = m_pcPrototype->GetAttributes();
    wxSizer* l_pcSizer = NULL;
    if (!l_plAttr)
        return FALSE;

    for (SP_DS_Attribute* l_pcAttr : *l_plAttr)
    {
        if (l_pcAttr->GetGlobalShow())
        {
            if (!l_pcSizer)
                l_pcSizer = p_pcDlg->AddSection(GetDisplayName());

            p_pcDlg->AddFlagLine(l_pcSizer, l_pcAttr);
        }
    }

    return TRUE;
}

bool
SP_DS_Nodeclass::OnGlobalOptions()
{
    CHECK_POINTER(m_pcPrototype, return FALSE);

    // iterating through the list of attributes of the coarsenode
    const SP_ListAttribute* l_plAttr = m_pcPrototype->GetAttributes();

    if (!l_plAttr)
        return FALSE;

    for (SP_DS_Attribute* l_pcAttr : *l_plAttr)
    {
        if (l_pcAttr->GetGlobalShow())
        {
            SynchronizeShowFlags(l_pcAttr);
        }
    }

    return TRUE;
}

wxString&
SP_DS_Nodeclass::SetName(const wxString& p_pchName)
{
    if (m_pcPrototype)
        m_pcPrototype->ClassNameChanged(p_pchName);

    return SP_Name::SetName(p_pchName);
}

bool
SP_DS_Nodeclass::SqueezeIdAttributes()
{
    // only squeeze, if we really have an id attribute
    bool l_bReturn = HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);

    // start at new id
    ResetIdCount();

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_bReturn && l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= ((*l_itElement)->SqueezeIdAttribute() && (*l_itElement)->Update(TRUE));
    }

    return l_bReturn;
}

// protected method
bool
SP_DS_Nodeclass::SynchronizeShowFlags(SP_DS_Attribute* p_pcAttr)
{
    if (!p_pcAttr)
        return FALSE;

    SP_DS_Attribute* l_pcAttr;
    bool l_bShow = p_pcAttr->GetShow();
    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_pcAttr = (*l_itElement)->GetAttribute(p_pcAttr->GetName());
        if (l_pcAttr)
            l_pcAttr->SetShow(l_bShow);
        (*l_itElement)->Update(TRUE);
    }
    return TRUE;
}

SP_DS_Attribute*
SP_DS_Nodeclass::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
    CHECK_ENV(return NULL);
    CHECK_POINTER(p_pcAttr, return NULL);

    SP_DS_Attribute* l_pcAttr = m_pcPrototype->GetAttribute(p_pcAttr->GetName());

    if (!l_pcAttr)
        return m_pcPrototype->AddAttribute(p_pcAttr);

    SetLastError(wxString::Format(wxT("Attribute %s already exists"), p_pcAttr->GetName().c_str()));
    return NULL;
}

bool
SP_DS_Nodeclass::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
    CHECK_POINTER(m_pcPrototype, return FALSE);

    // it's enough to check the prototype
    return m_pcPrototype->HasAttributeType(p_eVal);
}

SP_DS_Coarse*
SP_DS_Nodeclass::SetCoarse(SP_DS_Coarse* p_pcVal)
{
    CHECK_ENV(return NULL);

    SP_DS_Coarse* l_pcExt = m_pcPrototype->SetCoarse(p_pcVal);
    // if we were successful, set the parent to NULL.
    // Indicating, that this is the prototype, which
    // should NEVER call delete on its m_pcContent member
    if (l_pcExt)
        l_pcExt->SetParent(NULL);

    return l_pcExt;
}

SP_Graphic*
SP_DS_Nodeclass::SetGraphic(SP_Graphic* p_pcGraphic)
{
    CHECK_ENV(return NULL);

    // per definition:
    // 'set'ing a graphic means removing all formerly
    // added ones, if any!
    m_pcPrototype->RemoveAllGraphics(TRUE);

    return m_pcPrototype->AddGraphic(p_pcGraphic);
}

bool
SP_DS_Nodeclass::OnLeftClickOnCanvas(SP_GUI_Canvas* p_pcCanvas,
                                     double p_nX, double p_nY, int p_nKeys)
{
    CHECK_ENV(return FALSE);

    if (!p_pcCanvas)
        return TRUE;

    SP_DS_Node* l_pcNode = NewElement(p_pcCanvas->GetNetnumber());

    if (!l_pcNode)
        return FALSE;

    return l_pcNode->ShowOnCanvas(p_pcCanvas, FALSE, p_nX, p_nY, p_nKeys);
}

SP_Data*
SP_DS_Nodeclass::GetCoarseNode(unsigned int p_nNetnumber)
{
    CHECK_POINTER(m_pcPrototype, return NULL);

    if (!m_pcPrototype->GetCoarse())
        return NULL;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        if ((*l_itElement)->GetCoarse() &&
            (*l_itElement)->GetCoarse()->GetNetnumber() == p_nNetnumber)
            return (*l_itElement);
    }
    return NULL;
}

bool
SP_DS_Nodeclass::SetNetnumbers(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->SetNetnumber(p_nNewVal, p_nOldVal);
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
    CHECK_POINTER(p_pmNet2Net, return FALSE);
    CHECK_POINTER(p_pcToGraph, return FALSE);

    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::ResetCoarseTree()
{
    CHECK_POINTER(m_pcPrototype, return FALSE);
    if (!m_pcPrototype->GetCoarse())
        return TRUE;

    bool l_bReturn = TRUE;
    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->ResetCoarseTree();
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->ShowOnCanvas(p_pcCanvas, p_bLocalOnly);
    }

    return l_bReturn;
}



bool
SP_DS_Nodeclass::CheckIntegrity()
{
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->CheckIntegrity();
    }

    return l_bReturn;
}


bool
SP_DS_Nodeclass::GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter)
{
    CHECK_POINTER(p_plGraphics, return FALSE);
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                              unsigned int p_nNetnumber,
                              SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                              SP_MapData2Data* p_pmData2Data)
{
    bool l_bReturn = TRUE;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
    	SP_DS_Node* l_pcNode = *l_itElement;
        l_bReturn &= dynamic_cast<SP_Data*>(l_pcNode)->AddToCopyMap(p_ppcGraph, p_nNetnumber, p_pmGraphic2Graphic, p_pmData2Data);
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::ShowDebug()
{
    bool l_bReturn = TRUE;

    SP_LOGDEBUG(wxString::Format(wxT("\tNodeclass: %s (%p) of Graph %s (%p)"),
				   GetName().c_str(), this, m_pcGraph->GetName().c_str(), m_pcGraph));

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->ShowDebug();
    }

    return l_bReturn;
}

bool
SP_DS_Nodeclass::Update()
{
	bool l_bReturn = true;

    SP_ListNode::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
    	l_bReturn &= (*l_itElement)->Update();
    }

    return l_bReturn;
}


