//////////////////////////////////////////////////////////////////////
// $Author: dbayer $
// $Version: 0.1 $
// $Date: 2008/09/08 $
// Short Description: writes SimplePed from Music Petri Net
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportMusicPed2Ped.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


bool
SP_ExportMusicPed2Ped::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName  == SP_DS_MUSICPN_CLASS);
}

bool
SP_ExportMusicPed2Ped::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  CHECK_BOOL((!p_fileName.IsEmpty()), return false);

  return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool
SP_ExportMusicPed2Ped::WriteNetclass(SP_DS_Netclass* p_pcVal,
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
SP_ExportMusicPed2Ped::WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if ((*l_Iter)->GetName() != wxT("Sound") &&
			(*l_Iter)->GetName() != wxT("Play Sound when Transition fires"))
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}

	}
    return WriteData(p_pcVal, l_pcElem);
}

