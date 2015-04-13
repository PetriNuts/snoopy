//////////////////////////////////////////////////////////////////////
// $Author: R2$
// $Version: 0.1 $
// $Date: 2007/02/06 $
// Short Description: export to ina tmd
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportTMD.h"
#include "sp_defines.h"


#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

SP_ExportTMD::SP_ExportTMD()
{
}



SP_ExportTMD::~SP_ExportTMD()
{
}


bool
SP_ExportTMD::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (cName == SP_DS_TIMEPN_CLASS);
}


bool
SP_ExportTMD::DoWrite()
{
  WriteTransitions();



  return !m_bError;
}

bool
SP_ExportTMD::WriteTransitions()
{


  TransitionIdMap::const_iterator tIt;
  SP_Transition* t;
  for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++) {
    t = (*tIt).second;

    SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("Duration")));
	wxString l_strDuration = l_pcAttr->GetCell(l_pcAttr->GetActiveList(),1);

	m_file.Write(wxString::Format(wxT("   %ld:  %s;\n"),
				  t->m_id,l_strDuration.c_str()));
	}//endfor
    m_file.Write(wxT("@\n"));
  return TRUE;
}


