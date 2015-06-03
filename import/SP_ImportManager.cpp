//////////////////////////////////////////////////////////////////////
// $source: alextov $
// $author (modification): akurth$
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: manager class for import routines
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportManager.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Transformer.h"
#include "sp_core/tools/SP_StopWatch.h"

#include <wx/filedlg.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>


SP_ImportManager::SP_ImportManager()
{
}

SP_ImportManager::~SP_ImportManager()
{
  for (list<SP_ImportRoutine*>::iterator rIt = m_lRoutines.begin(); 
		rIt != m_lRoutines.end(); rIt++) 
  {
    SP_ImportRoutine* impR = (*rIt);
    wxDELETE(impR);
  }
  m_lRoutines.clear();
}

SP_ImportRoutine* 
SP_ImportManager::GetImportRoutine(const wxString& p_Name) {
  list<SP_ImportRoutine*>::iterator rIt;
  SP_ImportRoutine* impR;
  for (rIt = m_lRoutines.begin(); rIt != m_lRoutines.end(); rIt++) {
    impR = (*rIt);
    if (impR->GetName() == p_Name) return impR;
  }
  return NULL;
}


bool
SP_ImportManager::AddImportRoutine(SP_ImportRoutine* p_Routine)
{
  CHECK_POINTER(p_Routine, return false);
  if (GetImportRoutine(p_Routine->GetName())) {
    SetLastError(wxString::Format(
      wxT("Import routine with name '%s' is already added to the manager"),
      (p_Routine->GetName()).c_str()));
    return false;
  }
  m_lRoutines.push_back(p_Routine);
  return true;
}


void
SP_ImportManager::RemoveImportRoutine(SP_ImportRoutine* p_Routine)
{
    CHECK_POINTER(p_Routine, return);
    m_lRoutines.remove(p_Routine);
}

bool
SP_ImportManager::DoImport() 
{
  

  list<SP_ImportRoutine*>::const_iterator rIt;
  SP_ImportRoutine* impR;

  size_t eCnt = 0;
  
  for (rIt = m_lRoutines.begin(); rIt != m_lRoutines.end(); rIt++) {
    impR = (*rIt);
    eCnt++;
  }
  
  wxString *choices = new wxString[eCnt];

  size_t i = 0;
  for (rIt = m_lRoutines.begin(); rIt != m_lRoutines.end(); rIt++) {
    impR = (*rIt);
    choices[i] = impR->GetName();
      i++;
  }

  wxArrayInt sel;
  size_t sCount = wxGetMultipleChoices(sel,
				       _T("Select import routine\n"
					  wxT("(multi-choice is also accepted)")),
				       _T("Import"),				       
				       eCnt, choices);

  bool res = true;
  if (sCount > 0) 
  {
	wxString fileName;
	  
	wxString l_sSelName = wxT("");
	wxString l_sFileExt = wxT("");
	wxString l_sFileExtFilter = wxT("");
	wxString l_sSelDir = wxT("");
	for ( size_t n = 0; n < sCount; n++ ) 
	{
		impR = GetImportRoutine(choices[sel[n]]);
		l_sFileExtFilter = impR->GetExtension();
		l_sFileExtFilter << wxT("|All files (*.*)|*.*");
		fileName = wxFileSelector(impR->GetDescr(), 
				 l_sSelDir,
				 l_sSelName,
				 l_sFileExt,
				 l_sFileExtFilter,
				 wxFD_OPEN);
		if(fileName.empty())
		{
			return FALSE ;
		}
		SP_StopWatch sw(wxString::Format(wxT("Doing %s: %s"),
							impR->GetName().c_str(),
							fileName.c_str()),
						wxT("Finished import in"));

		res &= impR->ReadFile(fileName);

		if(res)
		{
			SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
			l_pcDoc->SetFilename(fileName + wxT(".") + l_pcDoc->GetGraph()->GetNetclass()->GetExtension(), true);
			l_pcDoc->Modify(true);
			SP_DS_Transformer transform;
			transform.Check(l_pcDoc->GetGraph());
		}
    }
  }

  wxDELETEA(choices);
  return res;
}

