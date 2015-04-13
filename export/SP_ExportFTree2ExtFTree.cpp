//////////////////////////////////////////////////////////////////////
// $source: alextov $
// $author (modification): akurth$
// $Version: 0.1 $
// $Date: 2005/06/23 $
// Short Description: writes Extended FaultTree from FaultTree
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportFTree2ExtFTree.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportFTree2ExtFTree::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName  == SP_DS_FAULTTREE_CLASS);
}

bool
SP_ExportFTree2ExtFTree::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool
SP_ExportFTree2ExtFTree::WriteNetclass(SP_DS_Netclass* p_pcVal,
				  wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return false);
  CHECK_POINTER(p_pcRoot, return false);
  

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
  l_pcElem->AddAttribute(wxT("name"), SP_DS_EXTFAULTTREE_CLASS);
  p_pcRoot->AddChild(l_pcElem);

  return true;
}


