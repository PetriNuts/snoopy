//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to LoLA
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportLoLA.h"

#include "sp_defines.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"






SP_ExportLoLA::SP_ExportLoLA()
{
}



SP_ExportLoLA::~SP_ExportLoLA()
{
}

bool
SP_ExportLoLA::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (	cName == SP_DS_PN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS);

}



bool
SP_ExportLoLA::DoWrite()
{
  WritePlaces();
  WriteMarking();
  WriteTransitions();


  return !m_bError;
}

bool
SP_ExportLoLA::WritePlaces()
{
  PlaceIdMap::iterator pIt;
  SP_Place* p;
  unsigned written = 0;
  m_file.Write(wxT("PLACE "));
  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    if (written > 0 && (written % 10) == 0) m_file.Write(wxT("\n"));
    if (written > 0) {
      m_file.Write(wxT(", "));
    }
    p = (*pIt).second;
    m_file.Write(GetName(p).c_str());
    written++;
  }
  m_file.Write(wxT(";\n\n"));
  return TRUE;
}

bool
SP_ExportLoLA::WriteMarking()
{
  PlaceIdMap::iterator pIt;
  SP_Place* p;
  unsigned written = 0;
  m_file.Write(wxT("MARKING "));
  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    p = (*pIt).second;
    if (p->m_marking > 0) {
      if (written > 0 && (written % 10) == 0) m_file.Write(wxT("\n"));
      if (written > 0) m_file.Write(wxT(", "));
      m_file.Write(wxString::Format(wxT("%s:%u"), GetName(p).c_str(),
				    p->m_marking));
      written++;
    }
  }
  m_file.Write(wxT(";\n\n"));
  return TRUE;
}


bool
SP_ExportLoLA::WriteTransitions()
{

  TransitionMap::iterator tIt;
  SP_Transition* t;
  list<SP_DiscrArc*>* aList;
  list<SP_DiscrArc*>::iterator aIt;
  SP_DiscrArc* a;

  for (tIt = m_trNameMap.begin(); tIt != m_trNameMap.end(); tIt++) {
    t = (*tIt).second;
    m_file.Write(wxString::Format(wxT("TRANSITION %s\n"),
				  GetName(t).c_str()));

    m_file.Write(wxT("CONSUME "));
    aList = m_preTrMap[t->m_id];
    if (aList) {
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	if(aIt != aList->begin()) m_file.Write(wxT(", "));
	a = (*aIt);
	m_file.Write(wxString::Format(wxT("%s:%u"),
					GetName(m_placeIdMap[a->m_id]).c_str(),
				    a->m_mult));
      }
    }
    m_file.Write(wxT(";\n"));
    m_file.Write(wxT("PRODUCE "));
    aList = m_postTrMap[t->m_id];
    if (aList) {
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	if(aIt != aList->begin()) m_file.Write(wxT(", "));
	a = (*aIt);
	m_file.Write(wxString::Format(wxT("%s:%u"),
				      GetName(m_placeIdMap[a->m_id]).c_str(),
				      a->m_mult));
      }
    }
    m_file.Write(wxT(";\n\n"));
  }
  m_file.Write(wxT("\n"));
  return TRUE;
}

wxString
SP_ExportLoLA::GetName(SP_Place* p)
{
    wxString l_name = wxT("P");
    l_name << p->m_id;
    l_name << wxT(".") << p->m_name;
    return l_name;
}

wxString
SP_ExportLoLA::GetName(SP_Transition* t)
{
    wxString l_name = wxT("T");
    l_name << t->m_id;
    l_name << wxT(".") << t->m_name;
    return l_name;
}
