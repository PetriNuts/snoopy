//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_CPN/SP_DLG_RandomMarking.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/tipwin.h>

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

IMPLEMENT_CLASS( SP_DLG_RandomMarking, wxDialog )

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING,
	SP_ID_GRID_NUMBER,
	SP_ID_COMBOX_CS,
	SP_ID_TEXTCTRL_CS
};

BEGIN_EVENT_TABLE( SP_DLG_RandomMarking, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_RandomMarking::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_RandomMarking::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_RandomMarking::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_RandomMarking::OnDeleteColorSet )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_RandomMarking::OnCheckFunction )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_RandomMarking::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_RandomMarking::OnGridCellValueChanged ) 
#endif

EVT_COMBOBOX(SP_ID_COMBOX_CS, SP_DLG_RandomMarking::OnComBoxChange) 

//EVT_GRID_LABEL_LEFT_CLICK(SP_DLG_RandomMarking::onNumberGridClick)
EVT_GRID_CMD_LABEL_LEFT_CLICK(SP_ID_GRID_NUMBER, SP_DLG_RandomMarking::onNumberGridClick)

END_EVENT_TABLE()



SP_DLG_RandomMarking::SP_DLG_RandomMarking(int p_nSelColumn, wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 500),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
	m_nSelColumn = p_nSelColumn;
	m_pcParent = p_pcParent;

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcSecondSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSizer* l_pcRowSizer = new wxBoxSizer( wxVERTICAL );

	wxSizer* l_pcComboSizer = new wxBoxSizer( wxVERTICAL );
	l_pcComboSizer->Add(new wxStaticText( this, -1, wxT("Choose a color set name:") ), 0, wxEXPAND | wxALL, 5);
	m_pcCSComboBox = new wxComboBox( this, SP_ID_COMBOX_CS, wxT(""), wxDefaultPosition, wxSize( 200, 50 ), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY );
	l_pcComboSizer->Add(m_pcCSComboBox, 1, wxEXPAND | wxALL, 5);	
	l_pcRowSizer->Add(l_pcComboSizer, 0, wxALL | wxEXPAND, 1);


	//l_pcRowSizer = new wxBoxSizer( wxVERTICAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set the overall tokens:") ), 0, wxEXPAND | wxALL, 5);
	m_pcColorGrid = new wxGrid(this, SP_ID_GRID_MARKING, wxDefaultPosition, wxSize( 300, 150 ), wxSUNKEN_BORDER );
	l_pcRowSizer->Add(m_pcColorGrid, 1, wxEXPAND | wxALL, 5);
		
	m_pcColorGrid->CreateGrid( 0, 0);
	m_pcColorGrid->EnableEditing( true);
	m_pcColorGrid->SetDefaultColSize( 220, TRUE);
	m_pcColorGrid->SetDefaultRowSize( 20, TRUE);
	m_pcColorGrid->SetColLabelSize( 16);
	m_pcColorGrid->SetRowLabelSize( 0); 
	m_pcColorGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	
	m_pcColorGrid->AppendCols(2);
	m_pcColorGrid->SetColLabelValue(0, wxT("Colors") );
	m_pcColorGrid->SetColSize(0, 195);
	m_pcColorGrid->SetColLabelValue(1, wxT("Number") );
	m_pcColorGrid->SetColSize(1, 100);	

	m_pcColorGrid->AppendRows(1);
	m_pcColorGrid->SetCellValue(0, 0, wxT("all()"));
	m_pcColorGrid->SetCellValue(0, 1, wxT("1"));

    //SP_AutoSizeRowLabelSize(m_pcColorGrid);

	wxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);	
	l_pcButtonSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcRowSizer->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 1);

	l_pcSecondSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	l_pcRowSizer = new wxBoxSizer( wxVERTICAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set the percentage for each place: \n (click a grid label for rules)") ), 0, wxEXPAND | wxALL, 5);
	m_pcPlaceGrid = new wxGrid(this, SP_ID_GRID_NUMBER, wxDefaultPosition, wxSize( 250, 200 ), wxSUNKEN_BORDER );
	l_pcRowSizer->Add(m_pcPlaceGrid, 1, wxEXPAND | wxALL, 5);	
	
	m_pcPlaceGrid->CreateGrid( 0, 0);
	m_pcPlaceGrid->EnableEditing( true);
	m_pcPlaceGrid->SetDefaultColSize( 220, TRUE);
	m_pcPlaceGrid->SetDefaultRowSize( 20, TRUE);
	m_pcPlaceGrid->SetColLabelSize( 16);
	m_pcPlaceGrid->SetRowLabelSize( 100); 
	m_pcPlaceGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	
	m_pcPlaceGrid->AppendCols(1);
	m_pcPlaceGrid->SetColLabelValue(0, wxT("Percent %") );
	m_pcPlaceGrid->SetColSize(0, 100);

	//SP_AutoSizeRowLabelSize(m_pcPlaceGrid);
	
	l_pcSecondSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	topSizer->Add(l_pcSecondSizer, 0, wxALL | wxEXPAND, 1);

	wxBoxSizer* l_pcOKButtonSizer = new wxBoxSizer(wxHORIZONTAL );	
	l_pcOKButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL | wxALIGN_RIGHT, 5);
	topSizer->Add(l_pcOKButtonSizer, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 1);	

	SetAutoLayout( TRUE);
	SetSizer(topSizer);

	if( ! Initialize() )
		return;

	LoadData();

	topSizer->Fit( this);
	topSizer->SetSizeHints( this);

	Layout();

}

void SP_DLG_RandomMarking::OnDlgOk(wxCommandEvent& p_cEvent)
{	
	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			if (Process())
			{				
				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				EndModal(wxID_OK);
			}
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_RandomMarking::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(FALSE);
	}
}

void SP_DLG_RandomMarking::OnDeleteColorSet( wxCommandEvent& p_cEvent )
{
	if (m_pcColorGrid->GetNumberRows() == 0)
		return;

	if ( m_pcColorGrid->IsSelection() )
    {
		m_pcColorGrid->BeginBatch();
        for ( int n = 0; n < m_pcColorGrid->GetNumberRows(); )
        {
            if ( m_pcColorGrid->IsInSelection( n , 0 ) )
            	m_pcColorGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        m_pcColorGrid->EndBatch();
    }
}

void SP_DLG_RandomMarking::OnAddColorSet( wxCommandEvent& p_cEvent )
{
	m_pcColorGrid->AppendRows(1);
}

void SP_DLG_RandomMarking::OnComBoxChange(wxCommandEvent& p_cEvent)
{
	LoadData();
}

bool SP_DLG_RandomMarking::Initialize()
{
	SP_CPN_ValueAssign l_cValueAssign;	
	if( ! l_cValueAssign.InitializeColorset(m_cColorSetClass) )
		return false;
	
	for( unsigned i = 0; i < m_cColorSetClass.GetColorSetVector()->size(); i++ )
	{
		wxString l_sColorSetName = (*(m_cColorSetClass.GetColorSetVector()))[i]->GetName();
		m_pcCSComboBox->Append( l_sColorSetName );
	}
	//TODO
	m_pcCSComboBox->SetSelection(m_pcCSComboBox->GetCount()-1);

	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!m_pcGraph)
		return false;

	return true;
}

bool SP_DLG_RandomMarking::LoadData()
{
	wxString l_sSelectedCS = m_pcCSComboBox->GetValue();

	if (m_pcPlaceGrid->GetNumberRows() > 0)
	{
		m_pcPlaceGrid->DeleteRows(0, m_pcPlaceGrid->GetNumberRows());
	}
	int l_nPos = 0;
	
	wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();	

	if( l_sNetClass == SP_DS_COLCPN_CLASS )
	{
		m_sPlaceNodeClass = SP_DS_CONTINUOUS_PLACE;		
		SP_MESSAGEBOX(wxT("This net class can not be supported."), wxT("Random marking"), wxOK | wxICON_ERROR);	
		return false;
	}
	else if ( l_sNetClass == SP_DS_COLSPN_CLASS )
	{
		m_sPlaceNodeClass = SP_DS_DISCRETE_PLACE;			
	}
	else
	{
		SP_MESSAGEBOX(wxT("This net class can not be supported."), wxT("Random marking"), wxOK | wxICON_ERROR);	
		return false;
	}	
	
	SP_DS_Nodeclass* l_pcNodeclass= m_pcGraph->GetNodeclass( m_sPlaceNodeClass );
	SP_ListNode::const_iterator l_itElem;	
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;	
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sColorSetName = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME) )->GetValue();

		if( l_sSelectedCS == l_sColorSetName )
		{
			m_pcPlaceGrid->AppendRows();
			m_pcPlaceGrid->SetRowLabelValue(l_nPos,l_sPlaceName);
			m_pcPlaceGrid->SetCellValue(l_nPos,0,wxT(""));
			l_nPos++;
		}
	}
	
	return true;
}

bool SP_DLG_RandomMarking::SaveData()
{	
	wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();	
	if( l_sNetClass == SP_DS_COLCPN_CLASS )
	{
		m_sPlaceNodeClass = SP_DS_CONTINUOUS_PLACE;		
		SP_MESSAGEBOX(wxT("This net class can not be supported."), wxT("Random marking"), wxOK | wxICON_ERROR);	
		return false;
	}
	else if ( l_sNetClass == SP_DS_COLSPN_CLASS )
	{
		m_sPlaceNodeClass = SP_DS_DISCRETE_PLACE;			
	}
	else
	{
		SP_MESSAGEBOX(wxT("This net class can not be supported."), wxT("Random marking"), wxOK | wxICON_ERROR);	
		return false;
	}	
	
	SP_DS_Nodeclass* l_pcNodeclass= m_pcGraph->GetNodeclass( m_sPlaceNodeClass );
	SP_ListNode::const_iterator l_itElem;	
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;	
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		if(m_mPlace2Color2Num.find(l_sPlaceName)!= m_mPlace2Color2Num.end())
		{
			////////////////////////////////////
			//to group colors with identical numbers
			map<wxString,int> l_mColor2Num = m_mPlace2Color2Num.find(l_sPlaceName)->second;
			map<wxString,int>::iterator itMap;
			wxString l_sColors_Num1;
			map<int,wxString> l_mMergedNum2Color;
			for(itMap = l_mColor2Num.begin(); itMap != l_mColor2Num.end(); itMap++)
			{
				wxString l_sColor = itMap->first;
				int l_nNum = itMap->second;
				if( l_mMergedNum2Color.find(l_nNum) != l_mMergedNum2Color.end() )
				{
					l_mMergedNum2Color[l_nNum] = l_mMergedNum2Color[l_nNum] + wxT(",") + l_sColor;
				}
				else
				{
					l_mMergedNum2Color[l_nNum] = l_sColor;
				}
			}
			////////////////////////////////////
			//write to collist
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));	
			if(!l_pcColList)
				return false;
			
			int l_nSelCol_Colors = 2*(m_nSelColumn/2);
			int l_nSelCol_Number = l_nSelCol_Colors +1;
			if( l_nSelCol_Number >= (int)(l_pcColList->GetColCount()) )
			{
				SP_MESSAGEBOX(wxT("The selected column does not exist."), wxT("Random marking"), wxOK | wxICON_ERROR);	
				return false;
			}
			for(unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				l_pcColList->SetCell(i, l_nSelCol_Colors, wxT(""));	
				l_pcColList->SetCell(i, l_nSelCol_Number, wxT(""));	
			}
			if( l_mMergedNum2Color.size() > l_pcColList->GetRowCount() )
			{
				for(unsigned i=0; i< l_mMergedNum2Color.size() - l_pcColList->GetRowCount(); i++)
				{
					l_pcColList->AppendEmptyRow();
				}
			}
			int l_nCurrentRow =0;
			map<int,wxString>::iterator itMegMap;
			for(itMegMap = l_mMergedNum2Color.begin(); itMegMap != l_mMergedNum2Color.end(); itMegMap++)
			{
				wxString l_sColor = itMegMap->second;
				int l_nNum = itMegMap->first;
				l_pcColList->SetCell(l_nCurrentRow, l_nSelCol_Colors, l_sColor);	
				l_pcColList->SetCell(l_nCurrentRow, l_nSelCol_Number, wxString::Format(wxT("%d"),l_nNum));	
				l_nCurrentRow++;
			}
		}		
	}	

	return true;
}


bool SP_DLG_RandomMarking::GetInitialMarking()
{
	wxString l_sColorSetName = m_pcCSComboBox->GetValue();

	vector< vector<wxString> > l_vvOriginalMarkings;
	for (int i = 0; i < m_pcColorGrid->GetNumberRows(); i++)
	{
		vector<wxString> l_vRow;
		for(int j = 0; j < m_pcColorGrid->GetNumberCols(); j++)
		{
			wxString l_sToken = m_pcColorGrid->GetCellValue(i, j);
			l_vRow.push_back( l_sToken );
		}
		l_vvOriginalMarkings.push_back(l_vRow);
	}

	//this is the pool to store the overall tokens
	m_mColorToMarkingMap.clear();

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return false;
	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	if( ! l_cSyntaxChecking.ComputeRandomMarking(l_sColorSetName,l_vvOriginalMarkings,l_mColorToMarkingMap) )
		return false;

	map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;
	for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
	{
		wxString l_sColor = itMap->first;
		vector<SP_CPN_TokenNum> l_vTokenNums = itMap->second;
		if(l_vTokenNums.size()==0)
			return false;
		int l_nTokenNum = l_vTokenNums[0].m_intMultiplicity; //to get the first column

		m_mColorToMarkingMap[l_sColor] = l_nTokenNum;
	}

    return true;
}


bool SP_DLG_RandomMarking::RandomAssign()
{
	//put the tokens to be randomly assigned in a vector
	vector<wxString> l_vAssignedColors;
	map<wxString,int>::iterator itMap;
	for(itMap = m_mColorToMarkingMap.begin(); itMap != m_mColorToMarkingMap.end(); itMap++)
	{
		wxString l_sColor = itMap->first;
		int l_nNum = itMap->second;		
		for(int i=0; i < l_nNum; i++)
		{
			l_vAssignedColors.push_back(l_sColor);
		}
	}

	int l_nTotalNum = l_vAssignedColors.size();

	//get the number to be assigned to each place
	map<wxString, int> l_mPlace2Number;
	for (int i = 0; i < m_pcPlaceGrid->GetNumberRows(); i++)
	{
		wxString l_sPlacename = m_pcPlaceGrid->GetRowLabelValue(i);
		wxString l_sPercent = m_pcPlaceGrid->GetCellValue(i,0);

		if(l_sPercent.Find(wxT("%")) != wxNOT_FOUND)
		{
			l_sPercent = l_sPercent.BeforeLast(wxT('%'));
		}

		long l_nPercent = 0;
		if(!l_sPercent.IsEmpty() && !l_sPercent.ToLong(&l_nPercent))
		{
			wxString l_sMessage = wxT("There is a place not randomly assigned tokens because the percentage is not specified or wrongly specified.");
			SP_LOGWARNING( l_sMessage);		
		}
		else if (l_nPercent < 0 || l_nPercent > 100)
		{
			wxString l_sMessage = wxT("The percentage must be between 0% and 100%.");
			SP_LOGWARNING( l_sMessage);
		}
		else
		{
			int l_nNumber = l_nPercent*l_nTotalNum/100;
			l_mPlace2Number[l_sPlacename] = l_nNumber;
		}
	}

	//assign tokens to places specified by a number
	vector<wxString> l_vOtherPlaces;
	map<wxString, int>::iterator itSIMap;
	for(itSIMap = l_mPlace2Number.begin(); itSIMap != l_mPlace2Number.end(); itSIMap++)
	{	
		wxString l_sPlaceName = itSIMap->first;
		map<wxString,int> l_mColor2Number;
		// implemented by fei
/*
		for(int i = 0; i < itSIMap->second; i++)
		{
			if(l_vAssignedColors.size()>0)
			{
				int l_nRandNum = SP_RandomLong(l_vAssignedColors.size());

				wxString l_sColor = l_vAssignedColors[l_nRandNum];				
				if( l_mColor2Number.find(l_sColor) != l_mColor2Number.end() )
				{
					l_mColor2Number[l_sColor] += 1;
				}
				else
				{
					l_mColor2Number[l_sColor] = 1;
				}
								
				l_vAssignedColors.erase(l_vAssignedColors.begin()+l_nRandNum);
			}
		}
*/
		// changed by cr
		std::random_shuffle(l_vAssignedColors.begin(), l_vAssignedColors.end(), SP_RandomLong);
		for(int i = 0; i < itSIMap->second; i++)
		{
			wxString l_sColor = l_vAssignedColors[i];
			if( l_mColor2Number.find(l_sColor) != l_mColor2Number.end() )
			{
				l_mColor2Number[l_sColor] += 1;
			}
			else
			{
				l_mColor2Number[l_sColor] = 1;
			}
		}

		m_mPlace2Color2Num[l_sPlaceName] = l_mColor2Number;
	}	

	return true;
}

void SP_DLG_RandomMarking::AddMultiSet(wxString p_sPlaceName, wxString p_Color, int p_Appearance)
{

/*	map<wxString,int> l_Color2Num;
	if(m_mPlace2Color2Num.find(p_sPlaceName)!= m_mPlace2Color2Num.end())
	{
		l_Color2Num = m_mPlace2Color2Num.find(p_sPlaceName)->second;
	}
	else
	{
		m_mPlace2Color2Num[p_sPlaceName] = l_Color2Num;
	}

	if( l_Color2Num.find(p_Color) != l_Color2Num.end() )
	{
		l_Color2Num[p_Color] = l_Color2Num.find(p_Color)->second + p_Appearance;
	}
	else
	{
		if(p_Appearance > 0)
			l_Color2Num[p_Color] = p_Appearance;
	}

	m_mPlace2Color2Num[p_sPlaceName] = l_Color2Num;
*/
}

bool SP_DLG_RandomMarking::Process()
{
	m_pcColorGrid->SaveEditControlValue();
	m_pcPlaceGrid->SaveEditControlValue();

	if(!GetInitialMarking())
		return false;

	if( !RandomAssign())
		return false;

	if( !SaveData() )
		return false;

	return true;
}

void SP_DLG_RandomMarking::onNumberGridClick(wxGridEvent& p_cEvent)
{
	wxString l_sHint = wxT("Rules: \n - integer: set a percent for a place; \n - 0         : set the marking of a place to 0; \n - empty : keep the current marking of a place. ");

	new wxTipWindow(this, l_sHint, 1000);
}

void SP_DLG_RandomMarking::InitializeChoice()
{	
}


void SP_DLG_RandomMarking::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{	
}

void SP_DLG_RandomMarking::OnCheckFunction( wxCommandEvent& p_cEvent )
{
/*	
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	if( ! l_cValueAssign.InitializeColorset(l_cColorSetClass) )
		return;

	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The variable definition");	
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(this, l_sVariables, 1000);
*/
}
