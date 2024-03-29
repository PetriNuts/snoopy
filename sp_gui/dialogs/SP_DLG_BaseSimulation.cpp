#include "sp_gui/dialogs/SP_DLG_BaseSimulation.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include <wx/statline.h>


IMPLEMENT_CLASS(SP_DLG_BaseSimulation, wxDialog )

BEGIN_EVENT_TABLE( SP_DLG_BaseSimulation, wxDialog )
END_EVENT_TABLE()

SP_DLG_BaseSimulation::SP_DLG_BaseSimulation(wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle, long p_nStyle) :
	wxDialog(p_pcParent, -1, p_sTitle, wxPoint(10, 10), wxDefaultSize, p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcContentSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcScrolledWindow = new wxScrolledWindow(this);
	//m_pcSimulationControlSizer = new wxStaticBoxSizer(new wxStaticBox(m_pcScrolledWindow, -1, wxT("")), wxVERTICAL);
	m_pcSimulationControlSizer = new wxBoxSizer(wxVERTICAL);


	m_pcScrolledWindow->SetSizer(m_pcSimulationControlSizer);
	m_pcScrolledWindow->SetScrollRate(5,5);
	m_pcScrolledWindow->SetMinClientSize(wxSize(400,500));
	m_pcContentSizer->Add(m_pcScrolledWindow, wxSizerFlags(1).Expand().Border(wxALL, 5));

    m_pcMainSizer->Add(m_pcContentSizer, wxSizerFlags(1).Expand().Border(wxALL, 5));

    //model configuration
	m_pcModelConfigSizer=new wxBoxSizer(wxVERTICAL);
	m_pcSimulationControlSizer->Add(m_pcModelConfigSizer);
	m_pcSimulationControlSizer->Add(new wxStaticLine(m_pcScrolledWindow), wxSizerFlags(0).Expand().Border(wxALL, 5));

	//model simulation
	m_pcModelSimulationSizer=new wxBoxSizer(wxVERTICAL);
	m_pcSimulationControlSizer->Add(m_pcModelSimulationSizer);
	m_pcSimulationControlSizer->Add(new wxStaticLine(m_pcScrolledWindow), wxSizerFlags(0).Expand().Border(wxALL, 5));

	//export sizer
	m_pcExportSizer=new wxBoxSizer(wxVERTICAL);
	m_pcSimulationControlSizer->Add(m_pcExportSizer);
	m_pcSimulationControlSizer->Add(new wxStaticLine(m_pcScrolledWindow), wxSizerFlags(0).Expand().Border(wxALL, 5));

	//viewer sizer
	m_pcViewerSizer=new wxBoxSizer(wxVERTICAL);
	m_pcSimulationControlSizer->Add(m_pcViewerSizer);


   	m_pcPropertySizer = new wxBoxSizer(wxVERTICAL);
	m_pcSimulationButtonSizer = new wxBoxSizer(wxVERTICAL);
	m_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
	m_pcDirectExportSizer = new wxBoxSizer(wxVERTICAL);
	m_pcModelViewsSizer = new wxBoxSizer(wxVERTICAL);

	SetSizerAndFit(m_pcMainSizer);

	Bind(wxEVT_INIT_DIALOG, &SP_DLG_BaseSimulation::OnInitDialog, this);

	SP_Core::Instance()->SetSimulationMode(true);
}

SP_DLG_BaseSimulation::~SP_DLG_BaseSimulation()
{
    SP_Core::Instance()->SetSimulationMode(false);
}

