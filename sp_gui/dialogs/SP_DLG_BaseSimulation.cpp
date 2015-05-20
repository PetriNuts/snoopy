#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <iostream>
#include "sp_gui/dialogs/SP_DLG_BaseSimulation.h"
#include "sp_ds/SP_DS_Graph.h"


IMPLEMENT_CLASS(SP_DLG_BaseSimulation, wxDialog )

BEGIN_EVENT_TABLE( SP_DLG_BaseSimulation, wxDialog )
END_EVENT_TABLE()

SP_DLG_BaseSimulation::SP_DLG_BaseSimulation(wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	wxDialog(p_pcParent, -1, p_sTitle, wxPoint(10, 10), wxSize(800, 800), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
#ifdef __WXGTK__
	, m_WindowDisabler(this)
#endif
{
	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcLeftSizer = new wxBoxSizer(wxVERTICAL);

	m_pcSimulationControlSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Simulation control")), wxVERTICAL);
	m_pcLeftSizer->Add(m_pcSimulationControlSizer, 1, wxDOWN | wxEXPAND, 5);

    m_pcMainSizer->Add(m_pcContentSizer, 1, wxEXPAND);
    m_pcContentSizer->Add(m_pcLeftSizer, 1, wxDOWN | wxEXPAND, 5);


	m_pcPropertySizer = new wxBoxSizer(wxVERTICAL);
	m_pcSimulationButtonSizer = new wxBoxSizer(wxVERTICAL);
	m_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
	m_pcDirectExportSizer = new wxBoxSizer(wxVERTICAL);
	m_pcModelViewsSizer = new wxBoxSizer(wxVERTICAL);


	SetSizerAndFit(m_pcMainSizer);

	Bind(wxEVT_INIT_DIALOG, &SP_DLG_BaseSimulation::OnInitDialog, this);

}

SP_DLG_BaseSimulation::~SP_DLG_BaseSimulation()
{
}

