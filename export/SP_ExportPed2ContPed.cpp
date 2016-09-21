//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/05/30 12:20:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportPed2ContPed.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"


bool
SP_ExportPed2ContPed::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  m_doc = p_doc;
  m_graph = m_doc->GetGraph();
  m_fileName = p_fileName;
  return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool
SP_ExportPed2ContPed::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return FALSE);
  CHECK_POINTER(p_pcRoot, return FALSE);
  

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
  l_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUSPED_CLASS);
  p_pcRoot->AddChild(l_pcElem);

  return TRUE;
}

bool
SP_ExportPed2ContPed::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return FALSE);
  CHECK_POINTER(p_pcRoot, return FALSE);
  

  SP_ListNode::const_iterator l_Iter;
  const SP_ListNode* l_plElements = p_pcVal->GetElements();
  CHECK_POINTER(l_plElements, return FALSE);
  wxString l_sNodeclassName = p_pcVal->GetName();
  wxString l_pcNodeclassName;

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
  l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));

  if (wxT("Place") == l_sNodeclassName) {
    l_pcNodeclassName = SP_DS_CONTINUOUS_PLACE;
  } else if (wxT("Transition") == l_sNodeclassName) {
    l_pcNodeclassName = SP_DS_CONTINUOUS_TRANS;
  } else if (wxT("Coarse Place") == l_sNodeclassName) {
    l_pcNodeclassName = SP_DS_COARSE_PLACE;
  } else if (wxT("Coarse Transition") == l_sNodeclassName) {
    l_pcNodeclassName = SP_DS_COARSE_TRANSITION;
  } else if (wxT("Comment") == l_sNodeclassName) {
    l_pcNodeclassName = wxT("Comment");
  }
  l_pcElem->AddAttribute(wxT("name"), l_pcNodeclassName);
  p_pcRoot->AddChild(l_pcElem);

  for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
    WriteNode((*l_Iter), l_pcElem);

  return TRUE;
}

bool
SP_ExportPed2ContPed::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return FALSE);
  CHECK_POINTER(p_pcRoot, return FALSE);
  

  SP_ListEdge::const_iterator l_Iter;
  const SP_ListEdge* l_plElements = p_pcVal->GetElements();
  CHECK_POINTER(l_plElements, return FALSE);
  wxString l_sEdgeclassName = p_pcVal->GetName();
  wxString l_pcEdgeclassName;

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
  l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
  if (wxT("Edge") == l_sEdgeclassName) {
    l_pcEdgeclassName = SP_DS_EDGE;
  }
  l_pcElem->AddAttribute(wxT("name"), l_pcEdgeclassName);
  p_pcRoot->AddChild(l_pcElem);

  for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
    WriteEdge((*l_Iter), l_pcElem);

  return TRUE;
}

bool
SP_ExportPed2ContPed::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format( wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format( wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
    {
       WriteAttribute((*l_Iter), l_pcElem);
    }

    return WriteData(p_pcVal, l_pcElem);
}

bool
SP_ExportPed2ContPed::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (SP_DS_PN_CLASS == cName);
}
