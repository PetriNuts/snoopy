//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/09/03 $
// Short Description: writes SimplePed from Modulo Nets
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportModuloNets2Ped.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportModuloNets2Ped::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName == SP_DS_MODULOPN_CLASS);
}


bool
SP_ExportModuloNets2Ped::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  m_changeColour = false;
  return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}


bool
SP_ExportModuloNets2Ped::WriteNetclass(SP_DS_Netclass* p_pcVal,
				  wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return false);
  CHECK_POINTER(p_pcRoot, return false);
  

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
  l_pcElem->AddAttribute(wxT("name"), SP_DS_PN_CLASS);
  p_pcRoot->AddChild(l_pcElem);

  return true;
}

bool
SP_ExportModuloNets2Ped::WriteClasses(SP_DS_Graph* p_pcVal,
				wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListNodeclass::const_iterator l_NIter;
    const SP_ListNodeclass* l_plNC = p_pcVal->GetNodeclasses();
    SP_ListEdgeclass::const_iterator l_EIter;
    const SP_ListEdgeclass* l_plEC = p_pcVal->GetEdgeclasses();
    wxXmlNode*  l_pcElem;

    CHECK_POINTER(l_plNC, return FALSE);
    CHECK_POINTER(l_plEC, return FALSE);
    int l_nSize = 0;

    l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclasses"));
    p_pcRoot->AddChild(l_pcElem);

    for (l_NIter = l_plNC->begin(); l_NIter != l_plNC->end(); ++l_NIter)
    {
    	if((*l_NIter)->GetName() != wxT("Modulo"))
    	{
    		WriteNodeclass((*l_NIter), l_pcElem);
    		l_nSize++;
    	}
    }
    l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_nSize));

    l_nSize = 0;
    l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclasses"));
    p_pcRoot->AddChild(l_pcElem);

    for (l_EIter = l_plEC->begin(); l_EIter != l_plEC->end(); ++l_EIter)
    {
    	if((*l_EIter)->GetName() != wxT("Undirected Edge"))
    	{
    		WriteEdgeclass((*l_EIter), l_pcElem);
    		l_nSize++;
    	}
    }
    l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_nSize));

    return TRUE;
}


bool
SP_ExportModuloNets2Ped::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
				wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListNode::const_iterator l_Iter;
    const SP_ListNode* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    int l_nSize = 0;
    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
    {
    	if(p_pcVal->GetName() == wxT("Place"))
    	{
			bool l_bContinue = true;
			SP_DS_Node* l_pcNode = (*l_Iter);
			SP_ListEdge::const_iterator l_EIter = l_pcNode->GetSourceEdges()->begin();
			while(l_bContinue && l_EIter != l_pcNode->GetSourceEdges()->end())
			{
				if((*l_EIter)->GetEdgeclass()->GetName() == wxT("Undirected Edge"))
				{
					l_bContinue = false;
				}
				l_EIter++;
			}
			l_EIter = l_pcNode->GetTargetEdges()->begin();
			while(l_bContinue && l_EIter != l_pcNode->GetTargetEdges()->end())
			{
				if((*l_EIter)->GetEdgeclass()->GetName() == wxT("Undirected Edge"))
				{
					l_bContinue = false;
				}
				l_EIter++;
			}
			if(l_bContinue)
			{
				WriteNode((*l_Iter), l_pcElem);
				l_nSize++;
			}
    	}
    	else
		{
    		WriteNode((*l_Iter), l_pcElem);
    		l_nSize++;
    	}
    }

    l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_nSize));
    return TRUE;
}

