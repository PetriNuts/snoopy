//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: export to maria
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportMaria.h"
#include "sp_defines.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


SP_ExportMaria::SP_ExportMaria()
{
}



SP_ExportMaria::~SP_ExportMaria()
{
}

bool
SP_ExportMaria::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (	cName == SP_DS_PN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS );

}



bool
SP_ExportMaria::DoWrite()
{
  m_file.Write(wxT("typedef struct {} t;\n")
	       wxT("t t = <t;\n\n"));

  WritePlaces();
  WriteTransitions();



  m_file.Write(wxT("\n"));

  return !m_bError;
}

bool
SP_ExportMaria::WritePlaces()
{

  PlaceIdMap::iterator pIt;
  SP_Place* p;
  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    p = (*pIt).second;
    if (p->m_marking > 0) {
      m_file.Write(wxString::Format(wxT("place \"%s\" t:%u#t;\n"),
				    p->m_name.c_str(), p->m_marking));
    } else {
      m_file.Write(wxString::Format(wxT("place \"%s\" t;\n"),
				    p->m_name.c_str()));
    }
  }

  m_file.Write(wxT("\n"));
  return TRUE;
}

bool
SP_ExportMaria::WriteTransitions()
{

  TransitionIdMap::iterator tIt;
  SP_Transition* t;
  list<SP_DiscrArc*>* aList;
  list<SP_DiscrArc*>::iterator aIt;
  SP_DiscrArc* a;

  for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++) {
    t = (*tIt).second;
    m_file.Write(wxString::Format(wxT("trans \"%s\"\n"),
				  t->m_name.c_str()));

    aList = m_preTrMap[t->m_id];
    if (aList) {
      m_file.Write(wxT("in {\n"));
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	m_file.Write(wxString::Format(wxT("    place \"%s\":%u#t;\n"),
				      (m_placeIdMap[a->m_id])->m_name.c_str(),
				      a->m_mult));
      }
      m_file.Write(wxT("}\n"));
    }
    aList = m_postTrMap[t->m_id];
    if (aList) {
      m_file.Write(wxT("out {\n"));
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	m_file.Write(wxString::Format(wxT("    place \"%s\":%u#t;\n"),
				      (m_placeIdMap[a->m_id])->m_name.c_str(),
				      a->m_mult));
      }
      m_file.Write(wxT("}\n"));
    }
    m_file.Write(wxT(";\n"));
  }

  m_file.Write(wxT("\n"));
  return TRUE;
}

