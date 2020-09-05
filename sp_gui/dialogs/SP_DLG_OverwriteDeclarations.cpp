/*
*@author:George Assaf
*@Date:28.08.2020
*@Description:implementation file
*this dialog can be used also to let the user choose among several options
*/
#include "sp_gui/dialogs/SP_DLG_OverwriteDeclarations.h"

enum {
	SP_ID_LEFT_LEST,
	SP_ID_RIGHT_LIST,
	SP_ID_BUTTON_FORTH,
	SP_ID_BUTTON_BACK,
	SP_ID_BUTTON_ALL_FORTH,
	SP_ID_BUTTON_ALL_BACK
};

BEGIN_EVENT_TABLE(SP_DLG_OverwriteDeclarations, wxDialog)

END_EVENT_TABLE()

SP_DLG_OverwriteDeclarations::SP_DLG_OverwriteDeclarations(wxWindow *p_pcParent,const wxString& p_sTitle, const std::map<wxString, std::set<wxString>>& p_mDecMap) :
	wxDialog(p_pcParent, -1, p_sTitle, wxPoint(50, 50), wxSize(1000, 1000), wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_mType2Declaration = p_mDecMap;
	m_pcSizer   = new wxBoxSizer(wxVERTICAL);
	m_pcMainSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer = new wxBoxSizer(wxVERTICAL);
	m_pcMiddleSizer = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer = new wxBoxSizer(wxVERTICAL);
	m_pcBottomsizer= new wxBoxSizer(wxHORIZONTAL);

	m_pcLeftList = new wxListBox(this, SP_ID_LEFT_LEST, wxPoint(-1, -1), wxSize(110, 200));
	m_pcRightLsit = new wxListBox(this, SP_ID_RIGHT_LIST, wxPoint(-1, -1), wxSize(110, 200));

	initialise();

	m_pcRightLsit->SetSize(m_pcLeftList->GetSize());

	m_pcButton1 = new wxButton(this, SP_ID_BUTTON_FORTH,wxT(">"));
	m_pcButton2 = new wxButton(this,  SP_ID_BUTTON_BACK,wxT("<"));
	m_pcButtonAllFront= new wxButton(this, SP_ID_BUTTON_ALL_FORTH, wxT(">>"));
	m_pcButtonAllBack= new wxButton(this, SP_ID_BUTTON_ALL_BACK, wxT("<<"));

	m_pcLeftSizer->Add(m_pcLeftList);
	m_pcMiddleSizer->Add(m_pcButton1);
	m_pcMiddleSizer->Add(m_pcButton2);
	m_pcMiddleSizer->Add(m_pcButtonAllFront);
	m_pcMiddleSizer->Add(m_pcButtonAllBack);
	m_pcRightSizer->Add(m_pcRightLsit);

	m_pcBottomsizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);

	m_pcMainSizer->Add(m_pcLeftSizer);
	m_pcMainSizer->Add(m_pcMiddleSizer);
	m_pcMainSizer->Add(m_pcRightSizer);

	m_pcSizer->Add(m_pcMainSizer);
	m_pcSizer->Add(m_pcBottomsizer);

	Connect(SP_ID_BUTTON_FORTH, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(SP_DLG_OverwriteDeclarations::OnSingleFront));
	Connect(SP_ID_BUTTON_BACK, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(SP_DLG_OverwriteDeclarations::OnSingleBack));
	Connect(SP_ID_BUTTON_ALL_FORTH, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(SP_DLG_OverwriteDeclarations::OnSingleFrontAll));
	Connect(SP_ID_BUTTON_ALL_BACK, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(SP_DLG_OverwriteDeclarations::OnSingleBackAll));
	Layout();
	this->SetSizerAndFit(m_pcSizer);
	Center();

}
void SP_DLG_OverwriteDeclarations::initialise()
{
	for (auto itMap = m_mType2Declaration.begin(); itMap != m_mType2Declaration.end(); ++itMap)
	{
		for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
		{
			m_pcLeftList->Append(*itSet);
		}

	}

}

void SP_DLG_OverwriteDeclarations::StoreCurrentSelectedChoices()
{
	m_mSelectedType2Declaration.clear();

	wxArrayString l_aSelectedItems = m_pcRightLsit->GetStrings();
	for (auto item : l_aSelectedItems)
	{
		for (auto it = m_mType2Declaration.begin(); it != m_mType2Declaration.end(); ++it)
		{
			if (it->second.find(item) != it->second.end())
			{
				m_mSelectedType2Declaration[it->first].insert(item);
			}
		}
	}
}

void SP_DLG_OverwriteDeclarations::OnSingleFront(wxCommandEvent& p_event)
{
	wxString l_sSelectedItem=m_pcLeftList->GetStringSelection();
	int l_nPos = m_pcLeftList->GetSelection();
	if (m_pcRightLsit->FindString(l_sSelectedItem) == wxNOT_FOUND)
	{
		m_pcLeftList->Delete(l_nPos);
		m_pcRightLsit->Append(l_sSelectedItem);
	}

	StoreCurrentSelectedChoices();
}

void SP_DLG_OverwriteDeclarations::OnSingleBack(wxCommandEvent& p_event)
{
	wxString l_sSelectedItem = m_pcRightLsit->GetStringSelection();
	int l_nPos = m_pcRightLsit->GetSelection();
	if (m_pcLeftList->FindString(l_sSelectedItem) == wxNOT_FOUND)
	{
		m_pcRightLsit->Delete(l_nPos);
		m_pcLeftList->Append(l_sSelectedItem);
	}

	StoreCurrentSelectedChoices();
}
void SP_DLG_OverwriteDeclarations::OnSingleFrontAll(wxCommandEvent& p_event)
{
	wxArrayString l_aList=m_pcLeftList->GetStrings();
	wxArrayString  l_arightlist=m_pcRightLsit->GetStrings();

	if (l_aList.size() == 0)
		return;
	for (unsigned i=0;i< l_arightlist.size();i++)
	{
		m_pcRightLsit->Delete(i);
	}

	for (auto item : l_aList)
	{
		m_pcRightLsit->Append(item);
	}

	l_aList.Empty();
	m_pcLeftList->Clear();

	StoreCurrentSelectedChoices();
}
void SP_DLG_OverwriteDeclarations::OnSingleBackAll(wxCommandEvent& p_event)
{
	wxArrayString l_aList = m_pcLeftList->GetStrings();
	wxArrayString  l_arightlist = m_pcRightLsit->GetStrings();

	if (l_arightlist.size() == 0)
		return;

	for (unsigned i = 0; i< l_aList.size(); i++)
	{
		m_pcLeftList->Delete(i);
	}

	for (auto item : l_arightlist)
	{
		m_pcLeftList->Append(item);
	}

	l_arightlist.Empty();
	m_pcRightLsit->Clear();

	StoreCurrentSelectedChoices();
}
