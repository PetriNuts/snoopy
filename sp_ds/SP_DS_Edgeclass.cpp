//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gr/SP_GR_Edge.h"

#include "sp_core/base/SP_Graphic.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/dialogs/SP_DLG_GraphProperties.h"

#define CHECK_ENV(RET) \
    if (!m_pcPrototype) \
        RET;

SP_DS_Edgeclass::SP_DS_Edgeclass(SP_DS_Graph* p_pcGraph, const wxString& p_pchName, LongPtr p_nIdCounter)
:SP_ElementClass(SP_ELEMENT_EDGECLASS, p_pchName, p_nIdCounter),
m_pcStartShape(NULL), m_pcEndShape(NULL)
{
    m_pcPrototype = new SP_DS_Edge(this);
    m_pcGraph = p_pcGraph;
}

SP_DS_Edgeclass::~SP_DS_Edgeclass()
{
    if (m_pcPrototype)
        wxDELETE(m_pcPrototype);

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); l_itElement++)
    {
        SP_DS_Edge* l_pcEdge = (*l_itElement);
        wxDELETE(l_pcEdge);
    }
    m_lElements.clear();
}

SP_DS_Edge*
SP_DS_Edgeclass::GetPrototype()
{
    return m_pcPrototype;
}

SP_DS_Edge*
SP_DS_Edgeclass::NewElement(unsigned int p_nNetnumber, SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget)
{
    CHECK_ENV(return NULL);

    // if we are here, the requirements should be already checked,
    // so we just create a new object an add it to the list.
    SP_DS_Edge* l_pcNewEdge = m_pcPrototype->Clone();
    l_pcNewEdge->SetNetnumber(p_nNetnumber);

    AddElement(l_pcNewEdge);

    // while interactive drawing, we always get the connected graphical
    // objects passed through
    if (p_pcSource && p_pcTarget)
    {
    	SP_Graphic* l_pcNewGr = l_pcNewEdge->SetNodes(p_pcSource, p_pcTarget);
    	if(l_pcNewGr)
    	{
    		l_pcNewEdge = dynamic_cast<SP_DS_Edge*>(l_pcNewGr->GetParent());
    	}
    }

    return l_pcNewEdge;
}

bool
SP_DS_Edgeclass::AddElement(SP_DS_Edge* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return TRUE);

	if (p_pcVal->GetEdgeclass() != this)
	{
		p_pcVal->GetEdgeclass()->RemoveElement(p_pcVal);
		p_pcVal->SetEdgeclass(this);
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
	}    // whether we already had it in the list or not, now its in
    return TRUE;
}

bool
SP_DS_Edgeclass::RemoveElement(SP_DS_Edge* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return TRUE);

    p_pcVal->DecreaseCountLists();
    m_lElements.remove(p_pcVal);

    return TRUE;
}

bool
SP_DS_Edgeclass::RemoveElements(unsigned int p_nNetnumber, SP_ListData* p_plCollect)
{
    CHECK_POINTER(p_plCollect, return FALSE);

    SP_ListEdge l_lDeadElements;
    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
    	SP_DS_Edge* l_pcEdge = *l_itElement;
        if (l_pcEdge->RemoveGraphic(p_nNetnumber) == SP_NO_MORE_OBJECTS)
        {
            p_plCollect->push_back(dynamic_cast<SP_Data*>(l_pcEdge));
            l_lDeadElements.push_back(l_pcEdge);
        }
    }
    // clean up the local element list
    for (l_itElement = l_lDeadElements.begin(); l_itElement != l_lDeadElements.end(); ++l_itElement)
        RemoveElement((*l_itElement));

    return TRUE;
}

bool
SP_DS_Edgeclass::ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg)
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
SP_DS_Edgeclass::OnGlobalOptions()
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
SP_DS_Edgeclass::SetName(const wxString& p_pchName)
{
    if (m_pcPrototype)
        m_pcPrototype->ClassNameChanged(p_pchName);

    return SP_Name::SetName(p_pchName);
}

bool
SP_DS_Edgeclass::SqueezeIdAttributes()
{
    // only squeeze, if we really have an id attribute
    bool l_bReturn = HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);

    // start at new id
    ResetIdCount();

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_bReturn && l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= ((*l_itElement)->SqueezeIdAttribute() && (*l_itElement)->Update(TRUE));
    }

    return l_bReturn;
}

// protected method
bool
SP_DS_Edgeclass::SynchronizeShowFlags(SP_DS_Attribute* p_pcAttr)
{
    if (!p_pcAttr)
        return FALSE;

    SP_DS_Attribute* l_pcAttr;
    bool l_bShow = p_pcAttr->GetShow();
    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_pcAttr = (*l_itElement)->GetAttribute(p_pcAttr->GetName());
        if (l_pcAttr)
            l_pcAttr->SetShow(l_bShow);
    }
    return TRUE;
}

SP_DS_Attribute*
SP_DS_Edgeclass::AddAttribute(SP_DS_Attribute* p_pcAttr)
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
SP_DS_Edgeclass::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
    CHECK_POINTER(m_pcPrototype, return FALSE);

    // it's enough to check the prototype
    return m_pcPrototype->HasAttributeType(p_eVal);
}

SP_Graphic*
SP_DS_Edgeclass::SetGraphic(SP_Graphic* p_pcGraphic)
{
    CHECK_ENV(return NULL);

    // per definition:
    // 'set'ing a graphic means removing all formerly
    // added ones, if any!
    m_pcPrototype->RemoveAllGraphics(TRUE);

    return m_pcPrototype->AddGraphic(p_pcGraphic);
}

bool
SP_DS_Edgeclass::OnLeftClickOnCanvas(SP_GUI_Canvas* p_pcCanvas,
                                     double p_nX, double p_nY, int p_nKeys)
{
    CHECK_ENV(return FALSE);
    CHECK_POINTER(p_pcCanvas, return FALSE);

    // already drawing an edge
    if (m_pcStartShape)
    {
        p_pcCanvas->AddControlPoint(p_nX, p_nY);
        p_pcCanvas->DrawTempEdge(p_nX, p_nY);

        return TRUE;
    }
    return FALSE;
}

bool
SP_DS_Edgeclass::OnStartElement(SP_Graphic* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    CHECK_ENV(return FALSE);
    CHECK_POINTER(p_pcShape, return FALSE);
    CHECK_POINTER(p_pcShape->GetPrimitive(), return FALSE);

    // do we already started a new element?
    if (m_pcStartShape)
        return FALSE;

    // remember the shape, we started on
    m_pcStartShape = p_pcShape;
    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(p_pcShape->GetPrimitive()->GetCanvas());

    if (l_pcCanvas &&
        l_pcCanvas->OnStartLine(p_pcShape->GetPrimitive(), p_nX, p_nY, p_nKeys, p_nAttach))
        return TRUE;

    // failed to start the line
    m_pcStartShape = NULL;
    return FALSE;
}

bool
SP_DS_Edgeclass::OnDrawElement(double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    CHECK_ENV(return FALSE);
    // if we didn't start a line, what do we do here?
    CHECK_POINTER(m_pcStartShape, return FALSE);
    CHECK_POINTER(m_pcStartShape->GetPrimitive(), return FALSE);

    SP_GUI_Canvas* l_pcCanvas = dynamic_cast<SP_GUI_Canvas*>(m_pcStartShape->GetPrimitive()->GetCanvas());

    if (l_pcCanvas &&
        l_pcCanvas->OnDrawLine(m_pcStartShape->GetPrimitive(), p_nX, p_nY, p_nKeys, p_nAttach))
        return TRUE;

    return FALSE;
}

SP_DS_Edge*
SP_DS_Edgeclass::OnEndElement(SP_Graphic* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach)
{
    CHECK_ENV(return nullptr);
    // if we didn't start a line, what do we do here?
    CHECK_POINTER(m_pcStartShape, return nullptr);
    CHECK_POINTER(p_pcShape, return nullptr);
    CHECK_POINTER(GetParentGraph(), return nullptr);
    CHECK_POINTER(GetParentGraph()->GetNetclass(), return nullptr);
    // source and target should be in the same net
    CHECK_BOOL((m_pcStartShape->GetNetnumber() == p_pcShape->GetNetnumber()), return nullptr);
    SP_DS_Edge* l_pcEdge = nullptr;

    // never accept an attribute, try to get the parent
    while (p_pcShape->GetGraphicType() == SP_GRAPHIC_ATTRIBUTE)
    {
        p_pcShape = p_pcShape->GetGraphicParent();
    }

    if (m_pcStartShape->GetParent() && p_pcShape->GetParent())
    {
        if (GetParentGraph()->GetNetclass()->EdgeRequirement(
            this, m_pcStartShape->GetParent(), p_pcShape->GetParent()))
        {
            l_pcEdge = NewElement(m_pcStartShape->GetNetnumber(), m_pcStartShape, p_pcShape);

            {
				SP_GUI_Canvas* l_pcCanvas = m_pcStartShape->GetCanvas();
				l_pcEdge->ShowOnCanvas(l_pcCanvas, false, p_nX, p_nY, p_nKeys);

				p_pcShape->GetParent()->Update(false);
				m_pcStartShape->GetParent()->Update(false);
				//move nodes to get edge looking nice
                l_pcCanvas->MoveShape(m_pcStartShape->GetPrimitive(), 0, 0);
                l_pcCanvas->MoveShape(p_pcShape->GetPrimitive(), 0, 0);
            }
			if(m_pcStartShape->GetParent()->GetCoarse())
			{
				unsigned l_nNetNr = m_pcStartShape->GetParent()->GetCoarse()->GetNetnumber();
				SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcEdge->GetGraphicInSubnet(l_nNetNr));
				SP_Graphic* l_pcGrTarget = l_pcGrEdge->GetTarget();
	            SP_GUI_Canvas* l_pcCanvas = m_pcStartShape->GetCanvas();
	            l_pcEdge->ShowOnCanvas(l_pcCanvas, false, p_nX, p_nY, p_nKeys);
				p_pcShape->GetParent()->Update(false);
				m_pcStartShape->GetParent()->Update(false);
                l_pcCanvas->MoveShape(l_pcGrTarget->GetPrimitive(), 0, 0);
			}
			if(p_pcShape->GetParent()->GetCoarse())
			{
				unsigned l_nNetNr = p_pcShape->GetParent()->GetCoarse()->GetNetnumber();
				SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcEdge->GetGraphicInSubnet(l_nNetNr));
				SP_Graphic* l_pcGrSource = l_pcGrEdge->GetSource();
	            SP_GUI_Canvas* l_pcCanvas = m_pcStartShape->GetCanvas();
	            l_pcEdge->ShowOnCanvas(l_pcCanvas, false, p_nX, p_nY, p_nKeys);
				p_pcShape->GetParent()->Update(false);
				m_pcStartShape->GetParent()->Update(false);
                l_pcCanvas->MoveShape(l_pcGrSource->GetPrimitive(), 0, 0);
			}

        }
    }
    m_pcStartShape = nullptr;

    return l_pcEdge;
}

bool
SP_DS_Edgeclass::SetNetnumbers(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = m_lElements.begin(); l_itEdge != m_lElements.end(); ++l_itEdge)
    {
        l_bReturn &= (*l_itEdge)->SetNetnumber(p_nNewVal, p_nOldVal);
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph)
{
    CHECK_POINTER(p_pmNet2Net, return FALSE);
    CHECK_POINTER(p_pcToGraph, return FALSE);

    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = m_lElements.begin(); l_itEdge != m_lElements.end(); ++l_itEdge)
    {
        l_bReturn &= (*l_itEdge)->ResetNetnumbers(p_pmNet2Net, p_pcToGraph);
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->ShowOnCanvas(p_pcCanvas, p_bLocalOnly);
    }

    return l_bReturn;
}


bool
SP_DS_Edgeclass::CheckIntegrity()
{
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
      l_bReturn &= (*l_itElement)->CheckIntegrity();
    }
    return l_bReturn;
}


bool
SP_DS_Edgeclass::GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter)
{
    CHECK_POINTER(p_plGraphics, return FALSE);
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->GetGraphicsInNet(p_plGraphics, p_nNet, p_eFilter);
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
        l_bReturn &= (*l_itElement)->OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                              unsigned int p_nNetnumber,
                              SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                              SP_MapData2Data* p_pmData2Data)
{
    bool l_bReturn = TRUE;

    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = m_lElements.begin(); l_itEdge != m_lElements.end(); ++l_itEdge)
    {
    	SP_DS_Edge* l_pcEdge = *l_itEdge;
        l_bReturn &= dynamic_cast<SP_Data*>(l_pcEdge)->AddToCopyMap(p_ppcGraph, p_nNetnumber, p_pmGraphic2Graphic, p_pmData2Data);
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::ShowDebug()
{
    bool l_bReturn = TRUE;

    SP_LOGDEBUG(wxString::Format(wxT("\tEdgeclass: %s (%p) of Graph %s (%p)"),
				   GetName().c_str(), this, m_pcGraph->GetName().c_str(), m_pcGraph));

    SP_ListEdge::const_iterator l_itEdge;
    for (l_itEdge = m_lElements.begin(); l_itEdge != m_lElements.end(); ++l_itEdge)
    {
        l_bReturn &= (*l_itEdge)->ShowDebug();
    }

    return l_bReturn;
}

bool
SP_DS_Edgeclass::Update()
{
	bool l_bReturn = true;

    SP_ListEdge::const_iterator l_itElement;
    for (l_itElement = m_lElements.begin(); l_itElement != m_lElements.end(); ++l_itElement)
    {
    	l_bReturn &= (*l_itElement)->Update();
    }
    return l_bReturn;
}


