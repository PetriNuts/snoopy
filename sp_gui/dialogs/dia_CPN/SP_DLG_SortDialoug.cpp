/*
*@Author: George Assaf *
*@DATE: 29/04/2021*
*@REVESION:0.1*
*@DESCRIPTION: implementation*
*/
#include "SP_DLG_SortDialoug.h"
#include<algorithm>

enum
{
	SP_ID_KEY_SORT_CHOICE = 1001,
	SP_ID_ASC_SORT_CHOICE,
	SP_ID_DSC_SORT_CHOICE
};

IMPLEMENT_CLASS(SP_DLG_SortDialoug, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_SortDialoug, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_SortDialoug::OnDlgOk)
END_EVENT_TABLE()

SP_DLG_SortDialoug::SP_DLG_SortDialoug(wxWindow *p_pcParent, SP_VectorString p_vKeys, std::map<wxString, int> p_mkey2pos,SP_Vector2DString p_vvDataTable, int p_nMainSetIndex,const wxString& p_sTitle,
	long p_nStyle)
	:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
	m_vKeys(p_vKeys), m_mKey2Pos(p_mkey2pos),
	m_nMainSetIndex(p_nMainSetIndex)
{
	m_pcParent = p_pcParent;
	m_DataTable = p_vvDataTable;
	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	// set / name components
	wxBoxSizer* l_pcRowSizer; 

	for (int i = 0; i < p_vKeys.size(); i++)
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		wxString l_sLabel;
		l_sLabel << wxT("Sort key ") << i + 1;
		l_pcRowSizer->Add(new wxStaticText(this, -1, l_sLabel), 1, wxALL | wxEXPAND, 5);
		m_pcKey.push_back(new wxChoice(this, SP_ID_KEY_SORT_CHOICE, wxDefaultPosition, wxSize(100, -1)));
		//wxChoice* m_pcKey = new wxChoice(this, SP_ID_KEY_SORT_CHOICE, wxDefaultPosition, wxDefaultSize);
		l_pcRowSizer->Add(m_pcKey[i], 1, wxALL, 5);

		m_pcKey[i]->Append(wxT("Undefined"));
		for (auto itMap = m_mKey2Pos.begin(); itMap != m_mKey2Pos.end(); ++itMap)
		{
			if(itMap->second==0)
			m_pcKey[i]->Append(itMap->first);
		}
		for (auto itMap = m_mKey2Pos.begin(); itMap != m_mKey2Pos.end(); ++itMap)
		{
			if (itMap->second == 1)
				m_pcKey[i]->Append(itMap->first);
		}
		for (auto itMap = m_mKey2Pos.begin(); itMap != m_mKey2Pos.end(); ++itMap)
		{
			if (itMap->second == 2)
				m_pcKey[i]->Append(itMap->first);
		}
		 
		for (auto itMap = m_mKey2Pos.begin(); itMap != m_mKey2Pos.end(); ++itMap)
		{
			if (itMap->second == 3)
				m_pcKey[i]->Append(itMap->first);
		}

		m_pcKey[i]->SetSelection(0);

		wxBoxSizer*  l_pcSortingOn= new wxBoxSizer(wxHORIZONTAL);
		wxArrayString choices;
		choices.Add("ASC sorting");
		choices.Add("DSC sorting");
		m_pcRadioGroups.push_back(
	new wxRadioBox(this, SP_ID_ASC_SORT_CHOICE+i,
			"Select one of the options", wxDefaultPosition,
			wxDefaultSize, choices, 3, wxRA_VERTICAL)
		);
		l_pcRowSizer->Add(m_pcRadioGroups[i], 1, wxALL, 5);
		m_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND);
    }
	m_pcSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
	Layout();
	this->SetSizerAndFit(m_pcSizer);

	Centre();
}
 
void SP_DLG_SortDialoug::OnDlgOk(wxCommandEvent& p_cEvent)
{

	for (int i = 0; i < m_pcKey.size(); i++)
	{
		if (m_pcKey[i]&& m_pcRadioGroups[i])
		{
			int l_nSel=m_pcKey[i]->GetSelection();
			if (l_nSel == -1 || l_nSel==0) continue;
		 
			int l_nGroup=m_pcRadioGroups[i]->GetSelection();
			bool l_bSortingType;
			if (l_nGroup == 0)
			{
				l_bSortingType = false;
			}
			else
			{
				l_bSortingType = true;
			}
			m_mKey2SortType[m_vKeys[l_nSel-1]] = l_bSortingType;
		}
	}

	SP_Vector2DString l_vvSortedTable;
	if (m_mKey2SortType.size()==1)
	{
			wxString l_sKey = m_mKey2SortType.begin()->first;
			bool l_bIsASC = m_mKey2SortType.begin()->second;
			SortBasedOneColumn(m_mKey2Pos[l_sKey], l_bIsASC, l_vvSortedTable);
			m_DataTable = l_vvSortedTable;
			//for (int i=0;i<m_vKeys.size();i++)
			//{
			//	if (m_vKeys[i] == l_sKey)
				//{
			//		SortBasedOneColumn(m_mKey2Pos[l_sKey], l_bIsASC, l_vvSortedTable);
				//	m_DataTable = l_vvSortedTable;
			//	}
			//}
		 
		
	}

	if (m_mKey2SortType.size() == 2)
	{
		SP_VectorString l_vKeys;
		SP_VectorInt  l_vKeyIds;
		SP_VectorBool l_vTypes;
		unsigned int key1 = 0;
		unsigned int key2 = 0;
		bool l_bsort1 = false;
		bool l_bsort2 = false;
		int count = 0;



		for (int i = 0; i < m_pcKey.size(); i++)
		{
			if (m_pcKey[i] && m_pcRadioGroups[i])
			{
				int l_nSel = m_pcKey[i]->GetSelection();
				if (l_nSel == -1 || l_nSel == 0) continue;

				int l_nGroup = m_pcRadioGroups[i]->GetSelection();
				bool l_bSortingType;
				if (l_nGroup == 0)
				{
					l_bSortingType = false;
				}
				else
				{
					l_bSortingType = true;
				}

				if (count == 0)
				{
					key1 = m_mKey2Pos[m_vKeys[l_nSel - 1]];
					l_bsort1 = l_bSortingType;
				}
				else
				{
					key2 = m_mKey2Pos[m_vKeys[l_nSel - 1]];
					l_bsort2 = l_bSortingType;
				}
				count++;
				m_mKey2SortType[m_vKeys[l_nSel - 1]] = l_bSortingType;
			}
		}

		SortBasedTwoColumns(key1,key2, l_bsort1, l_bsort2, l_vvSortedTable);
		m_DataTable = l_vvSortedTable;
	}

	if (m_mKey2SortType.size() == 3)
	{
		unsigned int key1 = 0;
		unsigned int key2 = 0;
		unsigned int key3 = 0;
		bool l_bsort1 = false;
		bool l_bsort2 = false;
		bool l_bsort3 = false;
		int count = 0;

		for (int i = 0; i < m_pcKey.size(); i++)
		{
			if (m_pcKey[i] && m_pcRadioGroups[i])
			{
				int l_nSel = m_pcKey[i]->GetSelection();
				if (l_nSel == -1 || l_nSel == 0) continue;

				int l_nGroup = m_pcRadioGroups[i]->GetSelection();
				bool l_bSortingType;
				if (l_nGroup == 0)
				{
					l_bSortingType = false;
				}
				else
				{
					l_bSortingType = true;
				}

				if (count == 0)
				{
					key1 = m_mKey2Pos[m_vKeys[l_nSel - 1]];
					l_bsort1 = l_bSortingType;
				}
				else if(count==1)
				{
					key2 = m_mKey2Pos[m_vKeys[l_nSel - 1]];
					l_bsort2 = l_bSortingType;
				}
				else
				{
					key3 = m_mKey2Pos[m_vKeys[l_nSel - 1]];
					l_bsort3 = l_bSortingType;
				}
				count++;
				m_mKey2SortType[m_vKeys[l_nSel - 1]] = l_bSortingType;
			}
		}
		SortBasedThreColumns(key1,key2,key3, l_bsort1, l_bsort2, l_bsort3, l_vvSortedTable);
		m_DataTable = l_vvSortedTable;
	}

	EndModal(wxID_OK);
}

void SP_DLG_SortDialoug::SortBasedOneColumn(int key, bool p_bIsAscending, SP_Vector2DString& p_vvTable)
{
	if (key >= m_DataTable.size()) return;

	std::vector<std::string> l_vColumn;

	for (int i = 0; i < m_DataTable.size(); i++)
	{
		for (int j = 0; j < m_DataTable[i].size(); j++)
		{
			if (key == j)
			{
				l_vColumn.push_back(m_DataTable[i][j]);
			}
		}
		
	}

	if (p_bIsAscending==false)
	{
		std::sort(l_vColumn.begin(),l_vColumn.end());
	}
	else
	{
		std::sort(l_vColumn.begin(), l_vColumn.end());
		std::reverse(l_vColumn.begin(), l_vColumn.end());
	}


	SP_Vector2DString l_vvData = m_DataTable;

	for (int s=0;s< l_vColumn.size();s++)
	{
		for (int i = 0; i < m_DataTable.size(); i++)
		{

			if (l_vColumn[s] == m_DataTable[i][key])
			{
				for (int j = 0; j < m_DataTable[i].size(); j++)
				{
					l_vvData[s][j] = m_DataTable[i][j];

				}
				m_DataTable[i][key] = wxT("");
				break;
			}
		}
	}
	p_vvTable = l_vvData;
}

void SP_DLG_SortDialoug::SortBasedTwoColumns(int key1, int key2, bool p_bKey1SortType, bool p_bKey2SortType, SP_Vector2DString& p_vvTable)
{
	if (key1 >= m_DataTable.size() || key2 >= m_DataTable.size()) return;

	std::vector<std::string> l_vColumn1;
	std::vector<std::string> l_vColumn2;

	for (int i = 0; i < m_DataTable.size(); i++)
	{
		for (int j = 0; j < m_DataTable[i].size(); j++)
		{
			if (key1 == j)
			{
				l_vColumn1.push_back(m_DataTable[i][j]);
			}
			else if (key2 == j)
			{
				l_vColumn2.push_back(m_DataTable[i][j]);
			}
			else
			{
				//do nothings
			}

		}

	}
	//============================

	if (p_bKey1SortType == false)
	{
		std::sort(l_vColumn1.begin(), l_vColumn1.end());
	}
	else
	{
		std::sort(l_vColumn1.begin(), l_vColumn1.end());
		std::reverse(l_vColumn1.begin(), l_vColumn1.end());
	}

	if (p_bKey2SortType == false)
	{
		std::sort(l_vColumn2.begin(), l_vColumn2.end());
	}
	else
	{
		std::sort(l_vColumn2.begin(), l_vColumn2.end());
		std::reverse(l_vColumn2.begin(), l_vColumn2.end());
	}
	//=======================
	SP_Vector2DString l_vvData = m_DataTable;
 
	int l_nPos = 0;
	for (auto col1 : l_vColumn1)
	{
		for (auto col2 : l_vColumn2)
		{
			for (int i = 0; i < m_DataTable.size(); i++)
			{
				wxString l_sMetadatakey1 = m_DataTable[i][key1];
				wxString l_sMetadatakey2= m_DataTable[i][key2];
				if (l_sMetadatakey1 == col1 && l_sMetadatakey2 == col2)
				{
					l_vvData[l_nPos][0] = m_DataTable[i][0];
					l_vvData[l_nPos][1] = m_DataTable[i][1];
					l_vvData[l_nPos][2] = m_DataTable[i][2];
					l_vvData[l_nPos][3] = m_DataTable[i][3];

					for (unsigned int col = m_nMainSetIndex; col < m_DataTable[0].size(); col++)
					{
						l_vvData[l_nPos][col] = m_DataTable[i][col];
						
					}
					m_DataTable[i][key2]="";
					l_nPos++;
				}
			 
			}
		}
	}

	 
	p_vvTable = l_vvData;
}

void SP_DLG_SortDialoug::SortBasedThreColumns(int key1, int key2, int key3, bool bsort1, bool bsort2, bool bsort3, SP_Vector2DString& p_vvTable)
{
	if (key1 >= m_DataTable.size() || key2 >= m_DataTable.size() || key3 >= m_DataTable.size()) return;

	std::vector<std::string> l_vColumn1;
	std::vector<std::string> l_vColumn2;
	std::vector<std::string> l_vColumn3;
	for (int i = 0; i < m_DataTable.size(); i++)
	{
		for (int j = 0; j < m_DataTable[i].size(); j++)
		{
			if (key1 == j)
			{
				l_vColumn1.push_back(m_DataTable[i][j]);
			}
			else if (key2 == j)
			{
				l_vColumn2.push_back(m_DataTable[i][j]);
			}
			else if(key3==j)
			{
				l_vColumn3.push_back(m_DataTable[i][j]);
			}
			else
			{
				//do nothing
			}

		}

	}
	//=============================

	if (bsort1 == false)
	{
		std::sort(l_vColumn1.begin(), l_vColumn1.end());
	}
	else
	{
		std::sort(l_vColumn1.begin(), l_vColumn1.end());
		std::reverse(l_vColumn1.begin(), l_vColumn1.end());
	}

	if (bsort1 == false)
	{
		std::sort(l_vColumn2.begin(), l_vColumn2.end());
	}
	else
	{
		std::sort(l_vColumn2.begin(), l_vColumn2.end());
		std::reverse(l_vColumn2.begin(), l_vColumn2.end());
	}

	if (bsort3 == false)
	{
		std::sort(l_vColumn3.begin(), l_vColumn3.end());
	}
	else
	{
		std::sort(l_vColumn3.begin(), l_vColumn3.end());
		std::reverse(l_vColumn3.begin(), l_vColumn3.end());
	}
	//=======================
	SP_Vector2DString l_vvData = m_DataTable;

	int l_nPos = 0;
	for (auto col1 : l_vColumn1)
	{
		for (auto col2 : l_vColumn2)
		{
			for (auto col3 : l_vColumn3)
			{
				for (int i = 0; i < m_DataTable.size(); i++)
				{
					wxString l_sMetadatakey1 = m_DataTable[i][key1];
					wxString l_sMetadatakey2 = m_DataTable[i][key2];
					wxString l_sMetadatakey3 = m_DataTable[i][key3];
					if (l_sMetadatakey1 == col1 && l_sMetadatakey2 == col2 &&  l_sMetadatakey3 == col3)
					{
						l_vvData[l_nPos][0] = m_DataTable[i][0];
						l_vvData[l_nPos][1] = m_DataTable[i][1];
						l_vvData[l_nPos][2] = m_DataTable[i][2];
						l_vvData[l_nPos][3] = m_DataTable[i][3];

						for (unsigned int col = m_nMainSetIndex; col < m_DataTable[0].size(); col++)
						{
							l_vvData[l_nPos][col] = m_DataTable[i][col];

						}
						m_DataTable[i][key2] = "";
						l_nPos++;
					}

				}
			}
		}
	}


	p_vvTable = l_vvData;

}
void SP_DLG_SortDialoug::MakeUniqueVector(std::vector<string> p_v1, std::vector<string>& p_vres)
{
	std::set<string> l_vset1;
	for (auto element : p_v1)
	{
		l_vset1.insert(element);
	}

	for (auto elem : l_vset1)
	{
		p_vres.push_back(elem);
	}
}
