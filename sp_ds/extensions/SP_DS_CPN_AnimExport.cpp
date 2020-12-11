/*
* SP_DS_CPN_AnimExport.h
*
*  Created on: Oct 8, 2020
*   Author: George Assaf
*/

#include<wx/wx.h>
#include<wx/filepicker.h>
#include<wx/filefn.h>
#include "SP_DS_CPN_AnimExport.h"

enum
{
	//export & import ID
	wxID_BROWSE_FILE,
	wxID_START_TEXT,
	wxID_EVERY_TEXT,
	wxID_STOP_TEXT,
	wxID_RADIO_CHOICE
};


BEGIN_EVENT_TABLE(ExportCPN, wxFrame)
EVT_BUTTON(wxID_OK, ExportCPN::OnOK)
EVT_BUTTON(wxID_CANCEL, ExportCPN::OnExit)
EVT_RADIOBOX(wxID_RADIO_CHOICE, ExportCPN::OnRadioChoice)
END_EVENT_TABLE()

ExportCPN::ExportCPN(const wxString &title, SP_DS_CPN_Animation *temp/*, SP_DS_CPN_Animation * cpntemp*/) :
	wxFrame((wxFrame *)NULL, -1, title)
{
	m_pc_Animator = temp; //Gets the current animation window object
						  //m_pcCpnAnimator = cpntemp;
						  //Adds the filename
	m_pc_Panel = new wxPanel(this, -1);
	m_pc_HBox1 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_HBox2 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_HBox3 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_HBox4 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_HBox5 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_VBox = new wxBoxSizer(wxVERTICAL);

	m_choices[0] = wxT("Markings");
	m_choices[1] = wxT("Step sequences");


	m_pc_Filename = new wxStaticText(m_pc_Panel, wxID_ANY, wxT("File Name"));
	m_pc_HBox1->Add(m_pc_Filename, 0, wxRIGHT, 30);

	wxString l_sExportFilename;
	if (SP_Core::Instance()->GetRootDocument()->GetGraph())
	{
		l_sExportFilename = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->GetFilename().BeforeLast('.');

		if (l_sExportFilename.IsEmpty())
		{
			l_sExportFilename = wxT("unnamed");
		}

		wxString l_sTimeDate(wxDateTime::Now().Format(wxT("%Y-%m-%dT%H-%M-%S%Z")));

		l_sExportFilename << wxT("-")
			<< l_sTimeDate;

		l_sExportFilename.Replace(wxT("."), wxT("-"));

		l_sExportFilename.Replace(wxT(" "), wxT(""));
	}
	wxString l_file_path;
	if (l_sExportFilename != wxT(""))
		l_file_path = (wxGetCwd() + wxT("/") + l_sExportFilename);
	else
		l_file_path = (wxGetCwd() + wxT("/export"));

	m_pc_Browse = new wxFilePickerCtrl(m_pc_Panel, wxID_BROWSE_FILE, l_sExportFilename, wxT(""), wxT("*.*"),
		wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL | wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT);
	m_pc_HBox1->Add(m_pc_Browse, 0, wxRIGHT, 30);

	m_pc_VBox->Add(m_pc_HBox1, 0, wxALL | wxEXPAND, 5);

	//Horizontal line
	m_pc_StLine1 = new wxStaticLine(m_pc_Panel, wxID_ANY);
	m_pc_VBox->Add(m_pc_StLine1, 0, wxALL | wxEXPAND, 5);

	//Step Details
	m_pc_StepDetails = new wxStaticText(m_pc_Panel, wxID_ANY, wxT("Step Details"));
	m_pc_VBox->Add(m_pc_StepDetails, 0, 5);

	//Horizontal Line
	m_pc_StLine2 = new wxStaticLine(m_pc_Panel, wxID_ANY);
	m_pc_VBox->Add(m_pc_StLine2, 0, wxALL | wxEXPAND, 5);

	//Start
	m_pc_Start = new wxStaticText(m_pc_Panel, wxID_ANY, wxT("Start"));
	m_pc_HBox2->Add(m_pc_Start, 0, wxALL, 5);

	//Start text
	m_pc_StartText = new wxTextCtrl(m_pc_Panel, wxID_START_TEXT, wxT("1"));
	m_pc_HBox2->Add(m_pc_StartText, 1, wxALL | wxEXPAND, 5);

	m_pc_VBox->Add(m_pc_HBox2, 0, wxALL | wxEXPAND, 5);

	//Every
	m_pc_Every = new wxStaticText(m_pc_Panel, wxID_ANY, wxT("Every"));
	m_pc_HBox3->Add(m_pc_Every, 0, wxALL, 5);

	//Every text
	m_pc_EveryText = new wxTextCtrl(m_pc_Panel, wxID_EVERY_TEXT, wxT("1"));
	m_pc_HBox3->Add(m_pc_EveryText, 1, wxALL | wxEXPAND, 5);

	m_pc_VBox->Add(m_pc_HBox3, 0, wxALL | wxEXPAND, 5);

	//Stop
	m_pc_Stop = new wxStaticText(m_pc_Panel, wxID_ANY, wxT("Stop"));
	m_pc_HBox4->Add(m_pc_Stop, 0, wxALL, 5);

	//Stop every
	m_pc_StopText = new wxTextCtrl(m_pc_Panel, wxID_STOP_TEXT, wxT("100"));
	m_pc_HBox4->Add(m_pc_StopText, 1, wxALL | wxEXPAND, 5);
	m_pc_VBox->Add(m_pc_HBox4, 0, wxALL | wxEXPAND, 5);

	//Export Options
	m_pc_RadioChoice = new wxRadioBox(m_pc_Panel, wxID_RADIO_CHOICE, wxT("Export Options"), wxDefaultPosition, wxDefaultSize, 2, m_choices, 1, wxRA_SPECIFY_ROWS);
	m_pc_RadioChoice->SetSelection(0);
	m_pc_VBox->Add(m_pc_RadioChoice, 0, wxALL | wxEXPAND, 5);

	//Okay Button
	m_pc_OKBtn = new wxButton(m_pc_Panel, wxID_OK, wxT("OK"));
	m_pc_HBox5->Add(m_pc_OKBtn, 0, wxLEFT | wxRIGHT | wxEXPAND, 75);

	//Cancel Button
	m_pc_CancelBtn = new wxButton(m_pc_Panel, wxID_CANCEL, wxT("Cancel"));
	m_pc_HBox5->Add(m_pc_CancelBtn, 0, wxLEFT | wxRIGHT | wxEXPAND, 75);

	m_pc_OKBtn->SetDefault();

	m_pc_VBox->Add(m_pc_HBox5, 0, wxALL | wxEXPAND, 5);

	m_pc_Panel->SetSizerAndFit(m_pc_VBox);

	SetMinSize(m_pc_VBox->GetMinSize());
	Layout();
}

void ExportCPN::OnRadioChoice(wxCommandEvent &event)
{
	if (m_pc_RadioChoice->GetSelection() == 0)
	{
		m_pc_StartText->Clear();
		m_pc_StartText->AppendText(wxT("1"));
		m_pc_StartText->SetEditable(true);
		m_pc_EveryText->Clear();
		m_pc_EveryText->AppendText(wxT("1"));
		m_pc_EveryText->SetEditable(true);
	}

	else if (m_pc_RadioChoice->GetSelection() == 1)
	{
		m_pc_StartText->Clear();
		m_pc_StartText->AppendText(wxT("1"));
		m_pc_StartText->SetEditable(false);
		m_pc_EveryText->Clear();
		m_pc_EveryText->AppendText(wxT("1"));
		m_pc_EveryText->SetEditable(false);
	}

	m_pc_StopText->Clear();
	m_pc_StopText->AppendText(wxT("100"));
}

void ExportCPN::OnOK(wxCommandEvent &event)
{
	m_pc_Animator->ExportDetailsCPN(this);
	Close();
}

void ExportCPN::OnExit(wxCommandEvent &event)
{
	Close();
}



