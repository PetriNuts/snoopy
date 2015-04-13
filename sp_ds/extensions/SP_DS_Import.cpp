/*
 * SP_DS_Import.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: avinash
 */
#include<wx/wx.h>
#include<wx/filepicker.h>
#include<wx/filefn.h>
#include "SP_DS_Import.h"

BEGIN_EVENT_TABLE(Import,wxFrame)
	EVT_BUTTON(wxID_CANCEL,Import::OnExit)
	EVT_BUTTON(wxID_OK,Import::OnOK)
END_EVENT_TABLE()

Import::Import(const wxString &title,class SP_DS_PedAnimation *temp) :
	wxFrame((wxFrame *)NULL,-1,title)
{
	m_pc_Animator = temp; //Get the current animator object

	m_pc_Panel = new wxPanel(this,wxID_ANY);
	m_pc_HBox1 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_HBox2 = new wxBoxSizer(wxHORIZONTAL);
	m_pc_VBox = new wxBoxSizer(wxVERTICAL);

	//File name
	m_pc_ChooseFile = new wxStaticText(m_pc_Panel,wxID_ANY,wxT("Choose file"));
	m_pc_HBox1->Add(m_pc_ChooseFile,0,wxRIGHT,30);

	wxString file_path(wxT("import.tseq"));
	m_pc_Browse = new wxFilePickerCtrl(m_pc_Panel, wxID_ANY, file_path, wxT(""),wxT("*.tseq"),
			wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL | wxFLP_OPEN | wxFLP_CHANGE_DIR);
	m_pc_HBox1->Add(m_pc_Browse,0,wxRIGHT,30);

	m_pc_VBox->Add(m_pc_HBox1,0,wxALL | wxEXPAND,5);

	//Horizontal line
	m_pc_StLine1 = new wxStaticLine(m_pc_Panel,wxID_ANY);
	m_pc_VBox->Add(m_pc_StLine1,0,wxALL | wxEXPAND,5);

	//Okay button
	m_pc_OKBtn = new wxButton(m_pc_Panel,wxID_OK,wxT("OK"));
	m_pc_HBox2->Add(m_pc_OKBtn,0,wxLEFT | wxEXPAND,30);

	//Cancel button
	m_pc_CancelBtn = new wxButton(m_pc_Panel,wxID_CANCEL,wxT("Cancel"));
	m_pc_HBox2->Add(m_pc_CancelBtn,0,wxLEFT | wxRIGHT,30);

	m_pc_OKBtn->SetDefault();

	m_pc_VBox->Add(m_pc_HBox2,0,wxALL | wxEXPAND,5);

	SetMinSize(m_pc_VBox->GetMinSize());
	m_pc_Panel->SetSizerAndFit(m_pc_VBox);
	Layout();
}

void Import::OnExit(wxCommandEvent &event)
{
	Close();
}

void Import::OnOK(wxCommandEvent &event)
{
	m_pc_Animator->ImportDetails(this);
	Close();
}

