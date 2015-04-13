//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/09/01 $
// Short Description: writes Modulo Nets net from SimplePed
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportPed2ModuloNets.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportPed2ModuloNets::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName  == SP_DS_PN_CLASS);
}

bool
SP_ExportPed2ModuloNets::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool
SP_ExportPed2ModuloNets::WriteNetclass(SP_DS_Netclass* p_pcVal,
				  wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return false);
  CHECK_POINTER(p_pcRoot, return false);
  

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
  l_pcElem->AddAttribute(wxT("name"), SP_DS_MODULOPN_CLASS);
  p_pcRoot->AddChild(l_pcElem);

  return true;
}


