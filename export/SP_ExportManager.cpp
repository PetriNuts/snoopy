//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: manager class for export routines
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportManager.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include "sp_ds/extensions/SP_DS_Transformer.h"

#include <wx/choicdlg.h>
#include <wx/filename.h>

SP_ExportManager::SP_ExportManager()
{
}

SP_ExportManager::~SP_ExportManager()
{
	for (SP_ExportRoutine* expR : m_lRoutines)
	{
		wxDELETE(expR);
	}
	m_lRoutines.clear();
}

SP_ExportRoutine* SP_ExportManager::GetExportRoutine(const wxString& p_sName, SP_MDI_Doc* p_pcDoc)
{
	for (SP_ExportRoutine* expR : m_lRoutines)
	{
		if( !p_pcDoc || expR->AcceptsDoc(p_pcDoc))
		{
			if (expR->GetName() == p_sName)
			{
				return expR;
			}
		}
	}
	return NULL;
}

bool SP_ExportManager::AddExportRoutine(SP_ExportRoutine* p_Routine)
{
	CHECK_POINTER(p_Routine, return false);
	if (GetExportRoutine(p_Routine->GetName()))
	{
		SetLastError(wxString::Format(
		wxT("Export routine with name '%s' is already added to the manager"),
		(p_Routine->GetName()).c_str()));
	}
	m_lRoutines.push_back(p_Routine);
	return true;
}

void SP_ExportManager::RemoveExportRoutine(SP_ExportRoutine* p_Routine)
{
	CHECK_POINTER(p_Routine, return);
	m_lRoutines.remove(p_Routine);
}

bool SP_ExportManager::DoExport(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);

	int eCnt = 0;

	for (SP_ExportRoutine* expR : m_lRoutines)
	{
		if (expR->AcceptsDoc(p_doc))
			eCnt++;
	}

	if (eCnt < 1)
	{ // no export routinese for p_doc
		SP_MESSAGEBOX(wxT("There are no export routines available for this ")
				+ wxString(p_doc->GetNetclassName()), wxT("Export"), wxOK | wxICON_ERROR);
		return true;
	}

	wxString *choices = new wxString[eCnt];

	size_t i = 0;
	for (SP_ExportRoutine* expR : m_lRoutines)
	{
		if (expR->AcceptsDoc(p_doc))
		{
			choices[i] = expR->GetName();
			i++;
		}
	}

	wxArrayInt sel;
	int sCount = wxGetSelectedChoices(sel,
			_T("Select export routine\n"
			wxT("(multi-choice is also accepted)")),
			_T("Export"), eCnt, choices);

	bool res = true;
	if (sCount > 0)
	{
		p_doc->GetGraph()->SqueezeIdAttributes();
		SP_DS_Transformer transform;
		transform.CheckUniqueNames(p_doc->GetGraph());
		wxFileName fn = wxFileName(p_doc->GetFilename());
		for (size_t n = 0; n < sCount; n++)
		{
			SP_ExportRoutine* expR = GetExportRoutine(choices[sel[n]], p_doc);
			SP_DLG_ExportProperties* l_pcDlg = new SP_DLG_ExportProperties(expR, NULL, p_doc, fn.GetPath(wxPATH_GET_SEPARATOR) + fn.GetName());
			if(l_pcDlg->ShowModal() == wxID_OK)
			{
				res &= true;
			}
			else
			{
				res &= false;
			}
			l_pcDlg->Destroy();
		}
	}

	wxDELETEA(choices);
	return res;
}

