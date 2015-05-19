#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <iostream>
#include "sp_gui/dialogs/SP_DLG_BaseSimulation.h"
#include "sp_ds/SP_DS_Graph.h"


IMPLEMENT_CLASS(SP_DLG_BaseSimulation, wxDialog )

enum
{
	SP_ID_COLLAPSEPANEL_PROPERTY_SIZER
};

BEGIN_EVENT_TABLE( SP_DLG_BaseSimulation, wxDialog )
END_EVENT_TABLE()

SP_DLG_BaseSimulation::SP_DLG_BaseSimulation(wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	wxDialog(p_pcParent, -1, p_sTitle, wxPoint(10, 10), wxSize(800, 800), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
#ifdef __WXGTK__
	, m_WindowDisabler(this)
#endif
{
	//SP_LOGERROR(wxT("name"));
	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcLeftSizer = new wxBoxSizer(wxVERTICAL);

	m_pcSimulationControlSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Simulation control")), wxVERTICAL);

    m_pcMainSizer->Add(m_pcContentSizer, 1, wxEXPAND);
    m_pcContentSizer->Add(m_pcLeftSizer, 1, wxDOWN | wxEXPAND, 5);

	m_pcLeftSizer->Add(m_pcSimulationControlSizer, 1, wxDOWN | wxEXPAND, 5);

	m_pcPropertySizer = new wxBoxSizer(wxVERTICAL);
	m_pcSimulationButtonSizer = new wxBoxSizer(wxVERTICAL);
	m_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
	m_pcDirectExportSizer = new wxBoxSizer(wxVERTICAL);
	m_pcModelViewsSizer = new wxBoxSizer(wxVERTICAL);

	/**********************Simulation Configuration starts*********************/
	m_pcCollpanePropertySizer = new wxCollapsiblePane(this, SP_ID_COLLAPSEPANEL_PROPERTY_SIZER, "Simulator Configuration");
	m_pcPropertyWindowPropertySizer = m_pcCollpanePropertySizer->GetPane();


	m_pcPropertyWindowPropertySizer->SetSizerAndFit(m_pcPropertySizer);
	m_pcPropertySizer->SetSizeHints(m_pcPropertyWindowPropertySizer);
	m_pcSimulationControlSizer->Add(m_pcCollpanePropertySizer, 0, wxALL | wxEXPAND, 5);
	/**********************Simulation Configuration ends*********************/


	SetSizerAndFit(m_pcMainSizer);

	Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &SP_DLG_BaseSimulation::OnCollapsePropertySizer, this, SP_ID_COLLAPSEPANEL_PROPERTY_SIZER);
	Bind(wxEVT_INIT_DIALOG, &SP_DLG_BaseSimulation::OnInitDialog, this);

}

SP_DLG_BaseSimulation::~SP_DLG_BaseSimulation()
{
}

void SP_DLG_BaseSimulation::OnCollapsePropertySizer(wxCollapsiblePaneEvent& event) {
	wxSize temp = m_pcMainSizer->GetSize();
	temp.SetHeight( wxSystemSettings::GetMetric(wxSYS_SCREEN_X));
	DoLayoutAdaptation();
	this->SetSize(temp);
}

