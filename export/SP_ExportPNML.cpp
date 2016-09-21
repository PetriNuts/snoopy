//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/06/05 11:55:00 $
// Short Description: PNML Export
//////////////////////////////////////////////////////////////////////
#include <wx/file.h>
#include <wx/textfile.h>
#include "export/SP_ExportPNML.h"
#include "sp_utilities.h"

#include "sp_revision.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Metadata.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

SP_ExportPNML::SP_ExportPNML()
:m_pcGraph(NULL)
{
    if (!m_bError)
        Init();
}

void
SP_ExportPNML::Init()
{
}

SP_ExportPNML::~SP_ExportPNML()
{
}

bool
SP_ExportPNML::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (	cName == SP_DS_PN_CLASS );

}

bool
SP_ExportPNML::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  return Write(p_doc->GetGraph(), p_fileName);
}

bool
SP_ExportPNML::Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_BOOL((!p_sFile.IsEmpty()), return FALSE);

    m_pcGraph = p_pcGraph;
    m_ArcId = 0;

    wxXmlDocument* l_pcDoc = new wxXmlDocument();

    wxXmlNode* l_pcRootElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("pnml"));
    l_pcRootElem->AddAttribute(wxT("xmlns"), wxT("http://www.pnml.org/version-2009/grammar/pnml"));
    l_pcDoc->SetRoot(l_pcRootElem);

    wxXmlNode*  l_pcNetElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("net"));
    l_pcNetElem->AddAttribute(wxT("id"), wxT("net0"));
    l_pcNetElem->AddAttribute(wxT("type"), wxT("http://www.pnml.org/version-2009/grammar/ptnet"));
	l_pcRootElem->AddChild(l_pcNetElem);
	SP_DS_Metadata* l_pcMeta = m_pcGraph->GetMetadataclass(wxT("General"))->GetElements()->front();
	wxString netName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();
	if(netName.IsEmpty())
	{
		netName = m_pcGraph->GetParentDoc()->GetUserReadableName();
	}
	WriteName(SP_NormalizeString(netName), l_pcNetElem);

    wxXmlNode*  l_pcPageElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("page"));
    l_pcPageElem->AddAttribute(wxT("id"), wxT("page0"));
	l_pcNetElem->AddChild(l_pcPageElem);
	WriteName(wxT("DefaultPage"), l_pcPageElem);

//	WriteNetclass(p_pcGraph->GetNetclass(), l_pcRootElem);
	WriteClasses(p_pcGraph, l_pcPageElem);

    if(!l_pcDoc->Save(p_sFile))
    {
        m_sMessage << wxT("An error occurred creating the document") << wxT("\n");
        m_bError = TRUE;
    }
    wxDELETE(l_pcDoc);

    m_pcGraph = NULL;
    return !m_bError;
}

bool
SP_ExportPNML::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    return TRUE;
}

bool
SP_ExportPNML::WriteClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListNodeclass::const_iterator l_NIter;
    const SP_ListNodeclass* l_plNC = p_pcVal->GetNodeclasses();
    SP_ListEdgeclass::const_iterator l_EIter;
    const SP_ListEdgeclass* l_plEC = p_pcVal->GetEdgeclasses();

    CHECK_POINTER(l_plNC, return FALSE);
    CHECK_POINTER(l_plEC, return FALSE);

    for (l_NIter = l_plNC->begin(); l_NIter != l_plNC->end(); ++l_NIter)
        WriteNodeclass((*l_NIter), p_pcRoot);

    for (l_EIter = l_plEC->begin(); l_EIter != l_plEC->end(); ++l_EIter)
        WriteEdgeclass((*l_EIter), p_pcRoot);

    return TRUE;
}

bool
SP_ExportPNML::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListNode::const_iterator l_Iter;
    const SP_ListNode* l_plElements = p_pcVal->GetElements();

    CHECK_POINTER(l_plElements, return FALSE);

    wxString l_sNodeclass = wxT("");
    if(p_pcVal->GetName() == SP_DS_DISCRETE_PLACE)
    {
    	l_sNodeclass = wxT("place");
    }
    else if(p_pcVal->GetName() == SP_DS_DISCRETE_TRANS)
    {
    	l_sNodeclass = wxT("transition");
    }

    if(!l_sNodeclass.IsEmpty())
    {
    	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, l_sNodeclass);
			WriteNode((*l_Iter), l_pcElem);
			p_pcRoot->AddChild(l_pcElem);
		}
    }
    return TRUE;
}

bool
SP_ExportPNML::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListEdge::const_iterator l_Iter;
    const SP_ListEdge* l_plElements = p_pcVal->GetElements();

    CHECK_POINTER(l_plElements, return FALSE);

    wxString l_sEdgeclass = wxT("");
    if(p_pcVal->GetName() == SP_DS_EDGE)
    {
    	l_sEdgeclass = wxT("arc");
    }

    if(!l_sEdgeclass.IsEmpty())
    {
    	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
    	{
    		wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, l_sEdgeclass);
    		WriteEdge((*l_Iter), l_pcElem);
			p_pcRoot->AddChild(l_pcElem);
    	}
    }
    return TRUE;
}

bool
SP_ExportPNML::WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), p_pcRoot);

    return WriteData(p_pcVal, p_pcRoot);
}

bool
SP_ExportPNML::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();

    CHECK_POINTER(l_plAttributes, return FALSE);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), p_pcRoot);

    p_pcRoot->AddAttribute(wxT("id"), wxString::Format(wxT("a%ld"), m_ArcId));
    ++m_ArcId;

    if (p_pcVal->GetSource())
    {
    	wxString l_sId = p_pcVal->GetSource()->GetClassName().Lower()[0];
    	l_sId += p_pcVal->GetSource()->GetAttribute(wxT("ID"))->GetValueString();
    	p_pcRoot->AddAttribute(wxT("source"), l_sId);
    }
    if (p_pcVal->GetTarget())
    {
    	wxString l_sId = p_pcVal->GetTarget()->GetClassName().Lower()[0];
    	l_sId += p_pcVal->GetTarget()->GetAttribute(wxT("ID"))->GetValueString();
    	p_pcRoot->AddAttribute(wxT("target"), l_sId);
    }

    return WriteData(p_pcVal, p_pcRoot);
}

bool
SP_ExportPNML::WriteAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    if(p_pcVal->GetName() == wxT("ID"))
    {
    	p_pcRoot->AddAttribute(wxT("id"), p_pcVal->GetParent()->GetClassName().Lower()[0] + p_pcVal->GetValueString());
    }
    else if(p_pcVal->GetName() == wxT("Name"))
    {
    	WriteName(p_pcVal->GetValueString(), p_pcRoot);
    }
    else if(p_pcVal->GetName() == wxT("Marking"))
    {
    	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("initialMarking"));
    	wxString l_sVal;
    	l_sVal << dynamic_cast<SP_DS_MarkingAttribute*>(p_pcVal)->GetValue(true);
    	WriteText(l_sVal, l_pcElem);
    	p_pcRoot->AddChild(l_pcElem);
    }
    else if(p_pcVal->GetName() == wxT("Multiplicity"))
    {
    	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("inscription"));
    	wxString l_sVal;
    	l_sVal << dynamic_cast<SP_DS_MultiplicityAttribute*>(p_pcVal)->GetValue(true);
    	WriteText(l_sVal, l_pcElem);
    	p_pcRoot->AddChild(l_pcElem);
    }

    // do something

    return WriteData(p_pcVal, p_pcRoot);
}

bool
SP_ExportPNML::WriteData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = p_pcVal->GetGraphics();

    // do something

    if (l_plGraphics)
        for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
            WriteGraphic((*l_Iter), p_pcRoot);

    return TRUE;
}

bool
SP_ExportPNML::WriteGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_EDGE)
        return WriteEdgeGraphic(dynamic_cast<SP_GR_Edge*>(p_pcVal), p_pcRoot);

    // do something

    return TRUE;
}

bool
SP_ExportPNML::WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxList* l_pcList = dynamic_cast<wxLineShape*>(p_pcVal->GetPrimitive())->GetLineControlPoints();
    if (l_pcList)
    {
        wxNode* l_pcNode = l_pcList->GetFirst();
        wxRealPoint* l_pcPoint;
        while (l_pcNode)
        {
            l_pcPoint = (wxRealPoint*)(l_pcNode->GetData());
            // value in l_pcPoint->x and l_pcPoint->y
            // do something
            l_pcNode = l_pcNode->GetNext();
        }
    }

    return TRUE;
}

bool
SP_ExportPNML::WriteText(const wxString& p_sText, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
    //wxXmlNode* l_pcText =
    		new wxXmlNode(l_pcElem, wxXML_TEXT_NODE, wxT(""), p_sText);
    p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool
SP_ExportPNML::WriteName(const wxString& p_sName, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("name"));
    WriteText(p_sName, l_pcElem);
    p_pcRoot->AddChild(l_pcElem);

	return true;
}


