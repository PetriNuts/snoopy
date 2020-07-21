//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/06/30 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "import/SP_ImportRoutine.h"

#include <wx/filename.h>
#include <wx/gbsizer.h>
#include <wx/busyinfo.h>

#include "sp_gui/dialogs/SP_DLG_ImportProperties.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/tools/SP_StopWatch.h"

IMPLEMENT_CLASS(SP_DLG_ImportProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ImportProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_ImportProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ImportProperties::OnDlgCancel)
END_EVENT_TABLE()

enum {
	SP_ID_DECLARATIONS_UPDATE,
	SP_ID_GRID_CONSTANTS,
	SP_ID_SELECTIVE_IMPORT,
	SP_ID_NEW_MODEL
};
SP_DLG_ImportProperties::SP_DLG_ImportProperties(SP_ImportRoutine* p_pcImport,
	wxWindow* p_pcParent,
	dssd::andl::Net_ptr p_Net,
	const wxString& p_sFilename,
	const wxString& p_sTitle,
	long p_nStyle)
	: wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
		p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
	m_pcImport(p_pcImport),
	m_pcDoc(p_Net)
{

	/* top level sizer */
	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	/* notebook to show multiple pages on */
	m_pcNotebook = new SP_WDG_Notebook(this, wxT("Import"), wxDefaultPosition, wxSize(400, 300));
	m_pcSizer->Add(m_pcNotebook, 1, wxEXPAND | wxALL, 5);

	m_pcCheckBoxSizer = new wxBoxSizer(wxVERTICAL);
	m_pcCheckBoxSizerAll = new wxBoxSizer(wxHORIZONTAL);
	m_pcCheckBoxSizerSelectAll= new wxBoxSizer(wxVERTICAL);

	m_pcCheckBoxSizerAll->Add(m_pcCheckBoxSizer);
	m_pcCheckBoxSizerAll->Add(m_pcCheckBoxSizerSelectAll);
	m_pcSizer->Add(m_pcCheckBoxSizerAll);
	//m_pcSizer->Add(m_pcCheckBoxSizerSelectAll);
	/* Buttons in the lower right hand corner */
	m_pcSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);

	// export properties come from the export, of course
	m_pcImport->AddToDialog(this, p_Net);

	SetSizerAndFit(m_pcSizer);
#if wxABI_VERSION > 30000
	m_pcSizer->FitInside(this);
#else
	m_pcSizer->SetVirtualSizeHints(this);
#endif

	Centre(wxBOTH);
}

void SP_DLG_ImportProperties::AddCheckBoxSelectAll(wxCheckBox* p_pcCheckBox)
{
	if (p_pcCheckBox&&m_pcCheckBoxSizerSelectAll)
	{
		m_pcCheckBoxSizerSelectAll->Add(p_pcCheckBox,0,0,1);
	}
}

void SP_DLG_ImportProperties::AddCheckBox(wxCheckBox* l_pcCheckBox)
{
	if (m_pcCheckBoxSizer&& l_pcCheckBox)
	{
		m_pcCheckBoxSizer->Add(l_pcCheckBox);
	}
}

SP_WDG_NotebookPage*
SP_DLG_ImportProperties::AddPage(const wxString& p_sLabel)
{
	if (m_pcNotebook)
		return m_pcNotebook->AddOrGetPage(p_sLabel);

	return NULL;
}
//------------------------------------------------------------------------
void
SP_DLG_ImportProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
 

		int l_nReturn = wxID_OK;

		if (IsModal())
			EndModal(l_nReturn);
		else
		{
			SetReturnCode(l_nReturn);
			this->Show(FALSE);
		}
	
	
}




void
SP_DLG_ImportProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(FALSE);
	}
}

 


 
void
SP_DLG_ImportProperties::SetNotebookSize(int width, int height)
{
	m_pcNotebook->SetInitialSize(wxSize(width, height));
}
 