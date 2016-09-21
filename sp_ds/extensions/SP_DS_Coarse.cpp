//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Extension for netelements (only nodes actually)
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "snoopy.h"

#include "sp_utilities.h"


SP_DS_Coarse::SP_DS_Coarse(const wxString& p_pchName,
                           const wxString& p_pchInnerClass,
                           SP_DS_Graph* p_pcContent,
                           bool p_bClone)
  :
  SP_Name(p_pchName),
  m_bClone(p_bClone),
  m_bUpdate(FALSE),
  m_pcGraph(p_pcContent),
  m_pcParent(NULL),
  m_pcGraphic(NULL),
  m_sInnerClass(p_pchInnerClass),
  m_sLabelAttribute(wxT("")),
  m_nNetnumber(0),
  m_pcDocument(NULL)
{
  if (m_pcGraph) {
    if (m_pcGraph->GetNetclass())
      m_pcGraph->GetNetclass()->CreateCoarse();
  }
}

SP_DS_Coarse::~SP_DS_Coarse()
{
	SP_LOGDEBUG(wxString::Format(wxT("DTOR SP_DS_Coarse: %d"), m_nNetnumber));
    // having a parent means, that we are child of some real
    // element. the prototype of every class has set its
    // m_pcParent to NULL, to reflect its special state.
    // see SP_DS_Nodeclass::SetCoarse for an example
    if (m_pcParent)
    {
		// first clean up all subelements, if any
        // only if we have cloned the graph, delete it
        // otherwise just remove the elements that belong to us
    	if (m_bClone)
    	{
    		wxDELETE(m_pcGraph);
    		m_pcGraph = NULL;
        } else
        {
        	m_pcGraph->RemoveElements(m_nNetnumber);
        }
        // remove the entry in the hierarchy view afterwards
        if (m_pcGraph && !m_pcGraph->GetIsCopy() && m_nCoarseId.IsOk())
        {
        	// delete will remove the children too
        	// make sure, they will not try to delete self once again
        	wxTreeItemId l_nId = m_nCoarseId;
        	m_pcGraph->CreateCoarseTree()->InvalidateAllChildren(m_nCoarseId);
        	m_pcGraph->CreateCoarseTree()->Delete(l_nId);
        }
    }
    // manage the document and its view
    if (m_pcDocument)
    {
        m_pcDocument->SetClose(TRUE);
        m_pcDocument->Modify(FALSE);
        m_pcDocument->Close();
        // deletes the document, if there are no more views
        m_pcDocument->DeleteAllViews();
    }
}

SP_DS_Coarse*
SP_DS_Coarse::Clone()
{
    CHECK_POINTER(m_pcGraph, return NULL);
    CHECK_POINTER(m_pcGraph->GetNetclass(), return NULL);

    SP_DS_Graph* l_pcGraph = m_pcGraph;
    SP_DS_Coarse* l_pcNew = NULL;

    if (m_bClone)
    {
        l_pcGraph = new SP_DS_Graph();
        m_pcGraph->GetNetclass()->CreateGraph(l_pcGraph);
    }

    l_pcNew = new SP_DS_Coarse(GetName(), m_sInnerClass, l_pcGraph, m_bClone);
    // don't call GetNetnumber() because it will increment the member variable
    // what isn't desired here, because this can be a call to the prototype
    l_pcNew->SetNetnumber(m_nNetnumber);
    l_pcNew->SetLabelAttribute(GetLabelAttribute());

    return l_pcNew;
}

void
SP_DS_Coarse::SetCoarseId(wxTreeItemId p_nId)
{
    m_nCoarseId = p_nId;
}

SP_Graphic*
SP_DS_Coarse::GetGraphic()
{
    if (!m_pcGraphic)
    {
        if ((m_pcParent->GetGraphics()) &&
            (m_pcParent->GetGraphics()->size() > 0))
        {
            m_pcGraphic = *(m_pcParent->GetGraphics()->begin());
        }
    }
    return m_pcGraphic;
}

const wxString
SP_DS_Coarse::GetLabelAttributeLabel()
{
    wxString l_sVal = wxT("Coarse");
    SP_DS_Attribute* l_pcVal = NULL;
    if (m_pcParent)
    {
        switch (m_pcParent->GetElementType())
        {
        case SP_ELEMENT_NODE:
            l_pcVal = static_cast<SP_DS_Node*>(m_pcParent)->GetAttribute(m_sLabelAttribute);
            break;
        case SP_ELEMENT_EDGE:
            l_pcVal = static_cast<SP_DS_Edge*>(m_pcParent)->GetAttribute(m_sLabelAttribute);
            break;
	default:
	  break;
        }
    }
    if (l_pcVal)
    {
        wxString l_pchVal = l_pcVal->GetValueString();

        if (l_pchVal.Len())
            l_sVal = l_pchVal;
    }

    l_sVal << wxT(" (") << GetNetnumber() << wxT(")");
    if (m_pcDocument)
        m_pcDocument->SetNameSuffix(l_sVal);

    return l_sVal;
}

SP_Data*
SP_DS_Coarse::SetParent(SP_Data* p_pcVal)
{
    m_pcParent = p_pcVal;

    return m_pcParent;
}

unsigned int
SP_DS_Coarse::GetNetnumber()
{
    wxASSERT(m_pcGraph);

    if ((m_nNetnumber == 0) && m_pcGraph)
        m_nNetnumber = m_pcGraph->GetNewNetnumber();

    return m_nNetnumber;
}

bool
SP_DS_Coarse::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal, bool p_bRecursive)
{
    CHECK_POINTER(m_pcGraph, return FALSE);
    CHECK_BOOL((p_nOldVal == 0 || p_nOldVal == m_nNetnumber), return TRUE);

    bool l_bReturn = TRUE;

    if (p_bRecursive)
        l_bReturn &= m_pcGraph->SetNetnumbers(p_nNewVal, p_nOldVal);

    m_nNetnumber = p_nNewVal;
    m_pcGraph->TestNetnumber(p_nNewVal);

    return l_bReturn;
}

bool
SP_DS_Coarse::Flatten(unsigned int p_nNewNet, SP_ListGraphic* p_plGraphics)
{
    if (!m_pcGraph)
        return FALSE;

    if (p_nNewNet == m_nNetnumber)
        return TRUE;

    bool l_bReturn = m_pcGraph->OnFlatten(p_nNewNet, m_nNetnumber, p_plGraphics);

    return l_bReturn;
}

bool
SP_DS_Coarse::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet)
{
    if (!m_pcGraph || !m_pcParent)
        return FALSE;

    if (m_pcParent->GetNetnumber() != p_nFromNet)
        return FALSE;

    return m_pcGraph->CreateCoarseTree()->InvalidateAllChildren(m_nCoarseId);
}

bool
SP_DS_Coarse::OnCoarse(unsigned int p_nToNet)
{
    if (!m_pcGraph)
        return FALSE;

    // save the id, to delete it afterwards, as InvalidateAllChildren sets it to 0
    wxTreeItemId l_nId = m_nCoarseId;
    m_pcGraph->CreateCoarseTree()->InvalidateAllChildren(m_nCoarseId);
    m_pcGraph->CreateCoarseTree()->Delete(l_nId);

    return TRUE;
}

bool
SP_DS_Coarse::ChangedClass(SP_Type* p_pcNewClass)
{
    CHECK_POINTER(p_pcNewClass, return FALSE);

    // if we are a coarse that clones it's own graph
    // everything is fine
    if (m_bClone)
        return TRUE;

    switch (p_pcNewClass->GetElementType())
    {
    case SP_ELEMENT_NODECLASS:
        m_pcGraph = static_cast<SP_DS_Nodeclass*>(p_pcNewClass)->GetParentGraph();
        break;
    case SP_ELEMENT_EDGECLASS:
        m_pcGraph = static_cast<SP_DS_Edgeclass*>(p_pcNewClass)->GetParentGraph();
        break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

bool
SP_DS_Coarse::Show(bool p_bShowWindow)
{
    CHECK_POINTER(m_pcGraph, return FALSE);
    CHECK_POINTER(m_pcGraph->GetNetclass(), return FALSE);
    CHECK_POINTER(wxGetApp().GetDocmanager(), return FALSE);

    if (!m_pcDocument)
    {
        SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
        wxString l_sName = wxString(m_pcGraph->GetNetclass()->GetName()) + wxT(" Coarse");

        SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(l_sName);
        if (l_pcTemplate)
        {
            m_pcDocument = dynamic_cast<SP_MDI_CoarseDoc*>(l_pcTemplate->CreateDocument(wxT(""), 0));
            if (m_pcDocument)
            {
				m_pcDocument->SetFilename(m_pcDocument->GetParentDoc()->GetFilename());
				// not needed on wxgtk
#ifdef __WXMSW__
				m_pcGraph->CreateCoarseTree()->SetCurrentDoc(m_pcDocument, FALSE);
#endif
			}
        }
    }

    return Update(p_bShowWindow, FALSE);
}

bool
SP_DS_Coarse::Update(bool p_bShowWindow, bool p_bLocalOnly)
{
    if (m_bUpdate)
        return TRUE;

    CHECK_POINTER(m_pcGraph, return FALSE);
    // display in the hierarchy view of the application
    if (!m_nCoarseId.IsOk())
        m_nCoarseId = m_pcGraph->CreateCoarseTree()->AppendFileItem(m_pcParent->GetNetnumber(), this);
    else
        m_pcGraph->CreateCoarseTree()->UpdateEntry(m_nCoarseId, this);
    // something didn't match the process
    if (!m_nCoarseId.IsOk())
        return FALSE;

    if (!m_pcDocument)
        return TRUE; //< not yet opened, but don't mind

    m_bUpdate = TRUE; //< avoid recursion
    m_pcDocument->SetGraph(m_pcGraph);
    m_pcDocument->SetNetnumber(GetNetnumber());
    if (!p_bLocalOnly)
        m_pcDocument->DrawAllElements(TRUE);

    if (p_bShowWindow)
    {
        m_pcDocument->ShowAll();
        SP_LOGDEBUG(wxString(wxT("Opening subnet number ")) << GetNetnumber());
    }
    m_bUpdate = FALSE;
    return TRUE;
}

bool
SP_DS_Coarse::AddToCopyMap(SP_DS_Graph** p_ppcGraph,
                           SP_Graphic* p_pcGraphic,
                           SP_MapGraphic2Graphic* p_pmGraphic2Graphic,
                           SP_MapData2Data* p_pmData2Data)
{
    CHECK_POINTER(m_pcGraph, return FALSE);

    return m_pcGraph->AddToCopyMap(p_ppcGraph, GetNetnumber(), p_pmGraphic2Graphic, p_pmData2Data);
}

SP_DS_Node*
SP_DS_Coarse::AddSourceEdge(SP_DS_Edge* p_pcEdge)
{
    CHECK_POINTER(p_pcEdge, return NULL);
    CHECK_POINTER(m_pcGraph, return NULL);

    SP_DS_Nodeclass* l_pcNC = m_pcGraph->GetNodeclass(m_sInnerClass);
    CHECK_POINTER(l_pcNC, return NULL);

    SP_DS_Node* l_pcNode = l_pcNC->NewElement(GetNetnumber());
    CHECK_POINTER(l_pcNode, return NULL);

    return l_pcNode->AddSourceEdge(p_pcEdge);
}

SP_DS_Node*
SP_DS_Coarse::AddTargetEdge(SP_DS_Edge* p_pcEdge)
{
    CHECK_POINTER(p_pcEdge, return NULL);
    CHECK_POINTER(m_pcGraph, return NULL);

    SP_DS_Nodeclass* l_pcNC = m_pcGraph->GetNodeclass(m_sInnerClass);
    CHECK_POINTER(l_pcNC, return NULL);

    SP_DS_Node* l_pcNode = l_pcNC->NewElement(GetNetnumber());
    CHECK_POINTER(l_pcNode, return NULL);

    return l_pcNode->AddTargetEdge(p_pcEdge);
}

void
SP_DS_Coarse::ShowDebug() {

	SP_LOGDEBUG(wxString::Format(wxT("Coarse: %p with CoarseId: %ld"), this, m_pcParent->GetId()));
	SP_LOGDEBUG(wxString::Format(wxT("Netnumber: %d"), m_nNetnumber));
	SP_LOGDEBUG(wxString::Format(wxT("Parent: %p Name: %s  in Graph: %p of Doc: %p"), m_pcParent, m_pcParent->GetAttribute(wxT("Name"))->GetValueString().c_str(),
																	m_pcParent->GetClassObject()->GetParentGraph(), m_pcParent->GetClassObject()->GetParentGraph()->GetParentDoc()));
	SP_LOGDEBUG(wxString::Format(wxT("CoarseGraph: %p of Doc: %p"), m_pcGraph, m_pcGraph->GetParentDoc()));

	SP_LOGDEBUG(wxString::Format(wxT("CoarseDoc: %p"), m_pcDocument));

}
