//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/22 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>
#include <wx/busyinfo.h>

#include "SP_DLG_LayoutProperties.h"
#include "sp_core/SP_Core.h"
#include "sp_core/tools/SP_StopWatch.h"

#include "sp_gui/layout/SP_LayoutOGDF.h"

IMPLEMENT_CLASS(SP_DLG_LayoutProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_LayoutProperties, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_LayoutProperties::OnDlgOk)
	EVT_BUTTON(wxID_CANCEL, SP_DLG_LayoutProperties::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_LayoutProperties::SP_DLG_LayoutProperties(SP_GUI_Mainframe* p_pcParent,
                                               SP_MDI_Doc* p_pcDoc,
                                               const wxString& p_sTitle,
                                               long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
          p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
m_pcDoc(p_pcDoc)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

	/* Layout */
    wxArrayString l_asLayoutChoices;
    l_asLayoutChoices.Add(wxT("FMMM"));
    l_asLayoutChoices.Add(wxT("Planarization"));
    l_asLayoutChoices.Add(wxT("Sugiyama"));
	m_pcLayoutChoice = new wxChoice(this,
									-1,
									wxDefaultPosition,
									wxDefaultSize,
									l_asLayoutChoices
									);
	m_pcLayoutChoice->SetSelection(0);
	wxBoxSizer* l_pcLayoutSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcLayoutSizer->Add(new wxStaticText(this, -1, wxT("Layout:")), 0, wxRIGHT , 5);
    l_pcLayoutSizer->Add(m_pcLayoutChoice, 1);
    m_pcSizer->Add(l_pcLayoutSizer, 0, wxEXPAND | wxALL, 5);

	/* all Views or only the current one*/
	wxArrayString l_asOneOrAllViewsChoice;
	l_asOneOrAllViewsChoice.Add(wxT("layout only current view"));
	l_asOneOrAllViewsChoice.Add(wxT("layout all views"));
	m_pcChooseViewChoice = new wxRadioBox(
        this,
        SP_TRANSFORM_RADIO_ID,
        wxT("Choose View"),
        wxDefaultPosition,
        wxDefaultSize,
        l_asOneOrAllViewsChoice,
        1,
        wxRA_SPECIFY_COLS);
    m_pcSizer->Add(m_pcChooseViewChoice, 0, wxEXPAND | wxALL, 5);

#if 0
	/* Nodeseperation */
	wxBoxSizer* l_pcNodeSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcNodeSepCtrl = new wxTextCtrl(this, -1, wxT("0.5"),
        wxDefaultPosition, wxDefaultSize, 0);

	l_pcNodeSizer->Add(new wxStaticText(this, -1, wxT("Nodeseperation:")), 0, wxRIGHT , 5);
    l_pcNodeSizer->Add(m_pcNodeSepCtrl, 1);
    m_pcSizer->Add(l_pcNodeSizer, 0, wxEXPAND | wxALL, 5);

	/* Custom */
	wxBoxSizer* l_pcCustomSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcNameCtrl = new wxTextCtrl(this, -1, wxT(""),
        wxDefaultPosition, wxDefaultSize, 0);

    m_pcValueCtrl = new wxTextCtrl(this, -1, wxT(""),
        wxDefaultPosition, wxDefaultSize, 0);

    l_pcCustomSizer->Add(m_pcNameCtrl, 1);
    l_pcCustomSizer->Add(m_pcValueCtrl, 1);
    m_pcSizer->Add(l_pcCustomSizer, 0, wxEXPAND | wxALL, 5);
#endif
	/* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0);

    SetSizerAndFit(m_pcSizer);

    Centre(wxBOTH);
}

wxString
SP_DLG_LayoutProperties::GetLayout()
{
	return( m_pcLayoutChoice->GetStringSelection() );
}

SP_LAYOUT_VIEWS
SP_DLG_LayoutProperties::GetView()
{
	switch (m_pcChooseViewChoice->GetSelection())
	{
		case 0:
			return (SP_LAYOUT_ACTUAL_VIEW);
		case 1:
			 return(SP_LAYOUT_ALL_VIEWS);

		default:
			return (SP_LAYOUT_ACTUAL_VIEW);
	}

}


SP_MapString2String
SP_DLG_LayoutProperties::GetAttributes()
{

	//m_lAttributes[m_pcNameCtrl->GetValue()] = m_pcValueCtrl->GetValue();

	return m_lAttributes;
}

//------------------------------------------------------------------------
void
SP_DLG_LayoutProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	SP_StopWatch sw(wxT("Doing Layout"),
					wxT("Finished layout in"));

    wxBusyInfo wait("Please wait, layout in progress...");

    SP_LayoutOGDF l_Layout;

	l_Layout.Visualization(this, m_pcDoc);
/**
    map<wxCheckBox*, SP_DS_Attribute*>::iterator l_Iter;
    for (l_Iter = m_mAttributes.begin(); l_Iter != m_mAttributes.end(); ++l_Iter)
    {
        if ((*l_Iter).first && (*l_Iter).second)
            (*l_Iter).second->SetShow((*l_Iter).first->GetValue());
    }
*/
    p_cEvent.Skip();
}

void
SP_DLG_LayoutProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}

