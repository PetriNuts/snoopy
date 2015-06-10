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

 EVT_BUTTON(wxID_OK,SP_DLG_ResultViewerProperties::OnUpdateViewerAttributes)

END_EVENT_TABLE()

SP_DLG_ResultViewerProperties::SP_DLG_ResultViewerProperties(SP_DS_ResultViewer* p_pcResultViewer,wxWindow* p_pcParent)
:wxDialog(p_pcParent, wxID_ANY,wxT("Result Viewer Properties"),wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE),
m_pcResultViewer(p_pcResultViewer)
{
SP_VectorString::const_iterator l_itCategory;
SP_ViewerAttributeList::const_iterator l_itAttribute;

	    wxSizer* l_pcMainSizer = new wxBoxSizer( wxVERTICAL );

	    m_pcNoteBook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize,wxNB_TOP);

	    if(m_pcResultViewer==NULL || m_pcResultViewer->GetAttributes()==NULL)
	    {
	    	return;
	    }
	    
	    //update attributes values
	    m_pcResultViewer->LoadCurrentAttributeValues();

	    for(l_itCategory=m_pcResultViewer->GetAttributeCategories().begin();
	    		l_itCategory!=m_pcResultViewer->GetAttributeCategories().end();l_itCategory++)
	    {
	    	  wxPanel* l_pcPage=new wxPanel(m_pcNoteBook);
			  wxSizer* l_pcSizer = new wxBoxSizer( wxVERTICAL);
			  l_pcPage->SetSizer(l_pcSizer);

			  SP_ViewerAttributeList l_aCatAttributes=m_pcResultViewer->GetAttributesByCategory((*l_itCategory));

			  for(l_itAttribute=l_aCatAttributes.begin();l_itAttribute!=l_aCatAttributes.end();l_itAttribute++)
			  {
				  (*l_itAttribute)->Create(l_pcPage,l_pcSizer);
			  }
			  m_pcNoteBook->AddPage(l_pcPage,(*l_itCategory));
	    }

		l_pcMainSizer->Add(m_pcNoteBook, 1, wxEXPAND | wxALIGN_LEFT);

		wxBoxSizer* l_pcBottonsSizer = new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("") ), wxHORIZONTAL);

		l_pcMainSizer->Add(l_pcBottonsSizer, 0, wxEXPAND | wxALIGN_CENTER);

		l_pcBottonsSizer->Add(new wxButton( this, wxID_OK, wxT("&Ok") ), 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
		l_pcBottonsSizer->Add(new wxButton( this, wxID_CANCEL, wxT("&Cancel") ), 0, wxEXPAND | wxALL|wxALIGN_CENTER, 5);

		SetSizerAndFit(l_pcMainSizer);
		Centre();
}

SP_DLG_ResultViewerProperties::~SP_DLG_ResultViewerProperties()
{

}

void SP_DLG_ResultViewerProperties::OnUpdateViewerAttributes(wxCommandEvent& event)
{
SP_ViewerAttributeList::const_iterator l_itAttribute;

		  for(l_itAttribute=m_pcResultViewer->GetAttributes()->begin();l_itAttribute!=m_pcResultViewer->GetAttributes()->end();l_itAttribute++)
		  {
				  (*l_itAttribute)->Update();
		  }

		  m_pcResultViewer->UpdateAttributes();


	      EndModal(wxID_OK);
}

