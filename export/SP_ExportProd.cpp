//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: export to prod
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportProd.h"
#include "sp_defines.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


SP_ExportProd::SP_ExportProd()
{
}



SP_ExportProd::~SP_ExportProd()
{
}

bool
SP_ExportProd::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return false);
  CHECK_POINTER(p_doc->GetGraph(), return false);
  wxString cName = p_doc->GetNetclassName();
  return (	cName == SP_DS_PN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS);

}



bool
SP_ExportProd::DoWrite()
{
  WritePlaces();
  WriteTransitions();


  return !m_bError;
}

bool
SP_ExportProd::WritePlaces()
{

  PlaceIdMap::iterator pIt;
  SP_Place* p;
  for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) {
    p = (*pIt).second;

    wxString nName = NormalizeString(p->m_name);
    if (p->m_name != nName) {
      SP_LOGWARNING(
	     wxString::Format(wxT("place name %s was changed to %s"),
			      p->m_name.c_str(), nName.c_str()));
    }

    if (p->m_marking > 0) {
      m_file.Write(wxString::Format(wxT("#place %s    lo(<.1.>) hi(<.1.>) mk(%u<.1.>)\n"),
				    nName.c_str(), p->m_marking));
    } else {
      m_file.Write(wxString::Format(wxT("#place %s    lo(<.1.>) hi(<.1.>)\n"),
				    nName.c_str()));
    }
  }

  m_file.Write(wxT("\n"));
  return TRUE;
}

bool
SP_ExportProd::WriteTransitions()
{

  TransitionIdMap::iterator tIt;
  SP_Transition* t;
  list<SP_DiscrArc*>* aList;
  list<SP_DiscrArc*>::iterator aIt;
  SP_DiscrArc* a;

  wxString nName;
  for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++) {
    t = (*tIt).second;

    nName = NormalizeString(t->m_name);
    if (t->m_name != nName) {
      SP_LOGWARNING(
	     wxString::Format(wxT("tr name %s was changed to %s"),
			      t->m_name.c_str(), nName.c_str()));
    }
    m_file.Write(wxString::Format(wxT("#trans %s\n"),
				  nName.c_str()));

    aList = m_preTrMap[t->m_id];
    if (aList) {
      m_file.Write(wxT("in {\n"));
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	nName = NormalizeString(m_placeIdMap[a->m_id]->m_name);
	m_file.Write(wxString::Format(wxT("    %s: %u <.d.>;\n"),
				      nName.c_str(),
				      a->m_mult));
      }
      m_file.Write(wxT("}\n"));
    }
    aList = m_postTrMap[t->m_id];
    if (aList) {
      m_file.Write(wxT("out {\n"));
      for (aIt = aList->begin(); aIt != aList->end(); aIt++) {
	a = (*aIt);
	nName = NormalizeString(m_placeIdMap[a->m_id]->m_name);
	m_file.Write(wxString::Format(wxT("    %s: %u <.d.>;\n"),
				      nName.c_str(),
				      a->m_mult));
      }
      m_file.Write(wxT("}\n"));
    }
    m_file.Write(wxT("#endtr\n"));
  }

  m_file.Write(wxT("\n"));
  return TRUE;
}

