//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to ina
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportTIM.h"
#include "sp_defines.h"


#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

SP_ExportTIM::SP_ExportTIM()
{
}



SP_ExportTIM::~SP_ExportTIM()
{
}


bool
SP_ExportTIM::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName == SP_DS_TIMEPN_CLASS);
}


bool
SP_ExportTIM::DoWrite()
{
  WriteTransitions();



  return !m_bError;
}

bool
SP_ExportTIM::WriteTransitions()
{


  TransitionIdMap::const_iterator tIt;
  SP_Transition* t;
  for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++) {
    t = (*tIt).second;

    SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("Interval")));

	wxString l_strEFT= l_pcAttr->GetCell(l_pcAttr->GetActiveList(),1);
	wxString l_strLFT= l_pcAttr->GetCell(l_pcAttr->GetActiveList(),2);

	m_file.Write(wxString::Format(wxT("  %ld:%s,%s\n"),
				  t->m_id,l_strEFT.c_str(),l_strLFT.c_str()));
	}//endfor

  return TRUE;
}

