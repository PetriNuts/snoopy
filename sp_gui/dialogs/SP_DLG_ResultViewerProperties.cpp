/*
 * SP_DLG_ResultViewerProperties.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///=================================================
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_gui/dialogs/SP_DLG_ResultViewerProperties.h"

BEGIN_EVENT_TABLE(SP_DLG_ResultViewerProperties,wxDialog)

 //EVT_BUTTON(wxID_OK,SP_DLG_ResultViewerProperties::OnUpdateViewerAttributes)
 EVT_BUTTON( wxID_OK, SP_DLG_ResultViewerProperties::OnDlgOk )
 EVT_BUTTON( wxID_CANCEL, SP_DLG_ResultViewerProperties::OnDlgCancel )

END_EVENT_TABLE()

SP_DLG_ResultViewerProperties::SP_DLG_ResultViewerProperties(SP_DS_ResultViewer* p_pcResultViewer,wxWindow* p_pcParent)
:wxDialog(p_pcParent, wxID_ANY,wxT("Result Viewer Properties"),wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
m_pcResultViewer(p_pcResultViewer)
{
	wxSizer* l_pcMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pcNoteBook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

	if(m_pcResultViewer==NULL || m_pcResultViewer->GetAttributes()==NULL)
	{
		return;
	}

	//update attributes values
	m_pcResultViewer->LoadCurrentAttributeValues();

	for(auto l_Category : m_pcResultViewer->GetAttributeCategories())
	{
		  wxPanel* l_pcPage = new wxPanel(m_pcNoteBook);
		  wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);
		  l_pcPage->SetSizer(l_pcSizer);

		  SP_ViewerAttributeList l_aCatAttributes = m_pcResultViewer->GetAttributesByCategory(l_Category);

		  for(auto l_Attribute : l_aCatAttributes)
		  {
			  l_Attribute->Create(l_pcPage,l_pcSizer);
		  }
		  m_pcNoteBook->AddPage(l_pcPage,l_Category);
	}

	l_pcMainSizer->Add(m_pcNoteBook, 1, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

	wxBoxSizer* l_pcBottonsSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcMainSizer->Add(l_pcBottonsSizer, 0, wxEXPAND | wxALIGN_CENTER | wxALL, 5);

	l_pcBottonsSizer->Add(new wxButton( this, wxID_OK, wxT("&Ok") ), 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	l_pcBottonsSizer->Add(new wxButton( this, wxID_CANCEL, wxT("&Cancel") ), 0, wxEXPAND | wxALL|wxALIGN_CENTER, 5);

	SetSizerAndFit(l_pcMainSizer);
	Centre();
}

SP_DLG_ResultViewerProperties::~SP_DLG_ResultViewerProperties()
{

}

void SP_DLG_ResultViewerProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow())
	{
		for(auto l_Attribute : *m_pcResultViewer->GetAttributes())
		{
			l_Attribute->Update();
		}

		m_pcResultViewer->UpdateAttributes();

		if (IsModal())
		{
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_ResultViewerProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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
