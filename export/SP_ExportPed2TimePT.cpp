//////////////////////////////////////////////////////////////////////
// $Author: R2 $
// $Version: 0.1 $
// $Date: 2007/02/06 $
// Short Description: writes Time PT net from SimplePed
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportPed2TimePT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportPed2TimePT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName  == SP_DS_PN_CLASS);
}

bool
SP_ExportPed2TimePT::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool
SP_ExportPed2TimePT::WriteNetclass(SP_DS_Netclass* p_pcVal,
				  wxXmlNode* p_pcRoot)
{
  CHECK_POINTER(p_pcVal, return false);
  CHECK_POINTER(p_pcRoot, return false);
  

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
  l_pcElem->AddAttribute(wxT("name"), SP_DS_TIMEPN_CLASS);
  p_pcRoot->AddChild(l_pcElem);

  return true;
}

