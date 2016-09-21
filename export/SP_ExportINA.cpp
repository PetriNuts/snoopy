//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to ina
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportINA.h"
#include "sp_utilities.h"


#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"


SP_ExportINA::SP_ExportINA()
{
}



SP_ExportINA::~SP_ExportINA()
{
}


bool
SP_ExportINA::DoWrite()
{
  WritePlPrePost();
  WritePlaces();
  WriteTransitions();



  m_file.Write(wxT("\n"));

  return !m_bError;
}

bool
SP_ExportINA::WritePlaces()
{
  PlaceIdMap::const_iterator pIt;
  SP_Place* p;

  m_file.Write(wxT("place nr.             name capacity time\n"));


  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    p = (*pIt).second;

    wxString nName = NormalizeString(p->m_name);
    if (p->m_name != nName) {
      SP_LOGWARNING(
	     wxString::Format(wxT("place name %s was changed to %s"),
			      p->m_name.c_str(), nName.c_str()));
    }
    m_file.Write(wxString::Format(wxT("     %ld: %s                  65535    0\n"),
				  p->m_id, nName.c_str()));
  }
  m_file.Write(wxT("@\n"));
  return TRUE;
}


bool
SP_ExportINA::WriteTransitions()
{

  m_file.Write(wxT("trans nr.             name priority time\n"));

  TransitionIdMap::const_iterator tIt;
  SP_Transition* t;
  for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++) {
    t = (*tIt).second;

    wxString nName = NormalizeString(t->m_name);
    if (t->m_name != nName) {
      SP_LOGWARNING(
	     wxString::Format(wxT("tr name %s was changed to %s"),
			      t->m_name.c_str(), nName.c_str()));
    }
    m_file.Write(wxString::Format(wxT("    %ld: %s                      0    0\n"),
				  t->m_id, nName.c_str()));
  }

  m_file.Write(wxT("@\n"));
  return TRUE;
}

bool
SP_ExportINA::WritePlPrePost()
{
  m_file.Write(wxT("P   M   PRE,POST  NETZ 0:t\n"));

  PlaceIdMap::const_iterator pIt;
  SP_Place* p;
  list<SP_DiscrArc*>* aList;
  list<SP_DiscrArc*>::iterator aIt;
  SP_DiscrArc* a;

  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    p = (*pIt).second;

    m_file.Write(wxString::Format(wxT("%ld %u"), p->m_id, p->m_marking));

    aList = m_prePlMap[p->m_id];
    if (aList) {
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	if (a->m_mult > 1) {
	  m_file.Write(wxString::Format(wxT(" %ld:%u"), a->m_id, a->m_mult));
	} else {
	  m_file.Write(wxString::Format(wxT(" %ld"), a->m_id));
	}
      }
    }

    aList = m_postPlMap[p->m_id];
    if (aList) {
      m_file.Write(wxT(" ,"));
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	if (a->m_mult > 1) {
	  m_file.Write(wxString::Format(wxT(" %ld:%u"), a->m_id, a->m_mult));
	} else {
	  m_file.Write(wxString::Format(wxT(" %ld"), a->m_id));
	}
      }
    }
    m_file.Write(wxT("\n"));
  }
  m_file.Write(wxT("@\n"));
  return TRUE;
}



bool
SP_ExportINA::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (	cName == SP_DS_PN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS );
}


