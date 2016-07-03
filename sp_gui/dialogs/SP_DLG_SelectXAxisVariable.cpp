/*
 * SP_DLG_SelectXAxisVariable.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 24.07.2011
 * Short Description:
 */
//=================================================
#include "sp_gui/dialogs/SP_DLG_SelectXAxisVariable.h"
enum
{
	SP_DLG_SELECT_X_AXIS_CHANGE_TYPE, SP_DLG_SELECT_X_AXIS_CHANGE_NAME
};

SP_DLG_SelectXAxisVariable::SP_DLG_SelectXAxisVariable(wxWindow* p_pcParent, const SP_MapString2VectorString& p_mChoices, const wxString& p_sCurrentSelectedCategory,
		const wxString& p_sCurrentSelectedSubCategory) :
		wxDialog(p_pcParent, wxID_ANY, wxT("Select X axis variable"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
		m_sSelectedCategory(p_sCurrentSelectedCategory),
		m_sSelectedSubCategory(p_sCurrentSelectedSubCategory),
		m_mChoices(p_mChoices)
{
	wxSizer* l_pcMainSizer = new wxBoxSizer(wxVERTICAL);

	//add variable type combobox
	wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcTopSizer = new wxBoxSizer(wxVERTICAL);
	l_pcMainSizer->Add(l_pcTopSizer, 0, wxALL | wxEXPAND , 5);

	l_pcRowSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Variable type:")), 1, wxEXPAND | wxALL, 5);
	m_pcVariableTypeBox = new wxChoice(this, SP_DLG_SELECT_X_AXIS_CHANGE_TYPE, wxDefaultPosition, wxSize(150, -1));
	l_pcRowSizer->Add(m_pcVariableTypeBox, 1, wxALL | wxEXPAND , 5);
	l_pcTopSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND , 5);
	l_pcTopSizer->AddSpacer(10);

	//select variable name
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Name:")), 1, wxEXPAND | wxALL, 5);
	m_pcVariableNameBox = new wxChoice(this, SP_DLG_SELECT_X_AXIS_CHANGE_NAME, wxDefaultPosition, wxSize(150, -1));

	l_pcRowSizer->Add(m_pcVariableNameBox, 1, wxALL | wxEXPAND , 5);
	l_pcTopSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND , 5);

	SP_MapString2VectorString::const_iterator l_itChoice;
	for (l_itChoice = m_mChoices.begin(); l_itChoice != m_mChoices.end(); l_itChoice++)
	{
		//Add the keys to the variable type
		m_pcVariableTypeBox->Append(l_itChoice->first);
	}
	m_pcVariableTypeBox->SetStringSelection(m_sSelectedCategory);
	AddItemNames();
	m_pcVariableNameBox->SetStringSelection(m_sSelectedSubCategory);

	l_pcMainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxEXPAND , 5);

	SetSizerAndFit(l_pcMainSizer);
	Center();

	//connect event
	Connect(SP_DLG_SELECT_X_AXIS_CHANGE_TYPE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(SP_DLG_SelectXAxisVariable::OnVariableTypeChanged));

	Connect(SP_DLG_SELECT_X_AXIS_CHANGE_NAME, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(SP_DLG_SelectXAxisVariable::OnVariableNameChanged));
}

SP_DLG_SelectXAxisVariable::~SP_DLG_SelectXAxisVariable()
{
}

void SP_DLG_SelectXAxisVariable::AddItemNames()
{
	wxString l_sCurrentCategory = m_pcVariableTypeBox->GetStringSelection();
	SP_VectorString* l_asSubCategory = m_mChoices[l_sCurrentCategory];

	m_pcVariableNameBox->Clear();

	if (l_asSubCategory == NULL)
	{
		return;
	}
	for (unsigned int l_nSubCat = 0; l_nSubCat < l_asSubCategory->size(); l_nSubCat++)
	{
		m_pcVariableNameBox->Append((*l_asSubCategory)[l_nSubCat]);
	}

	if (m_pcVariableNameBox->GetCount() == 0)
	{
		m_pcVariableNameBox->Enable(false);
	}
	else
	{
		m_pcVariableNameBox->Enable(true);
	}
}

void SP_DLG_SelectXAxisVariable::OnVariableTypeChanged(wxCommandEvent& WXUNUSED(event))
{
	m_sSelectedCategory = m_pcVariableTypeBox->GetStringSelection();

	AddItemNames();

	m_pcVariableNameBox->SetSelection(0);

	m_sSelectedSubCategory = m_pcVariableNameBox->GetStringSelection();
}

void SP_DLG_SelectXAxisVariable::OnVariableNameChanged(wxCommandEvent& WXUNUSED(event))
{
	m_sSelectedSubCategory = m_pcVariableNameBox->GetStringSelection();
}
