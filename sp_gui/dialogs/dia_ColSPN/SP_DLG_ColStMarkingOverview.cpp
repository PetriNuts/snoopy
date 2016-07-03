//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include <algorithm>

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_RandomMarking.h"

IMPLEMENT_CLASS( SP_DLG_ColStMarkingOverview, wxDialog )

enum
{
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET,
	SP_ID_BUTTON_RANDOMMARKING

};

BEGIN_EVENT_TABLE( SP_DLG_ColStMarkingOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColStMarkingOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColStMarkingOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_ColStMarkingOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_ColStMarkingOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_ColStMarkingOverview::OnRenameSet )
EVT_BUTTON( SP_ID_BUTTON_RANDOMMARKING, SP_DLG_ColStMarkingOverview::OnRandomMarking )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_ColStMarkingOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_ColStMarkingOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_ColStMarkingOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_ColStMarkingOverview::SP_DLG_ColStMarkingOverview(wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row

	m_pcMarkingGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcMarkingGrid->CreateGrid( 0, 0);

	m_pcMarkingGrid->EnableEditing( true);	

	m_pcMarkingGrid->SetDefaultColSize( 120, TRUE);
	m_pcMarkingGrid->SetDefaultRowSize( 20, TRUE);

	m_pcMarkingGrid->SetColLabelSize( 16);
	m_pcMarkingGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcMarkingGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcMarkingGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_ADDSET, wxT("Add marking set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_DELETESET, wxT("Delete marking set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_RENAMESET, wxT("Rename marking set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton(this, SP_ID_BUTTON_RANDOMMARKING, wxT("Random Marking") ), 0, wxEXPAND | wxALL, 5);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,
			wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND | wxALIGN_TOP
			);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM
			);

	m_pcSizer->Add(l_pcGridSizer, 1, wxALIGN_CENTER | wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT | wxEXPAND);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_ColStMarkingOverview::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{
		if (IsModal() )
		{
			SaveData();
			RefreshFrame();
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}

}

void SP_DLG_ColStMarkingOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColStMarkingOverview::OnAddSet(wxCommandEvent& p_cEvent)
{
	//New codes
	//Get the column, based on which to fill in the data
	wxArrayInt l_cSelCols = m_pcMarkingGrid->GetSelectedCols();	
	int  l_nSelected;
	if ( ! (l_cSelCols.Count() > 0 ))
	{	
		SP_MESSAGEBOX(wxT("Please select a base marking."), wxT("Warning"),wxOK | wxICON_WARNING);
		return;
	}
	else
	{		
		l_nSelected = l_cSelCols.Item( 0);		
	}	

    /////////////////////////////////////////////////////////////////////////////////////////////////	
	bool l_bWhite = false;
	wxTextEntryDialog l_pcDialog( this, _T("Name of the new marking set:"), wxT("Add marking set"));
	wxString l_sName= wxT("M-Set ");
	l_sName.Append(wxString::Format(wxT("%d"), m_pcMarkingGrid->GetNumberCols()/2 + 1 ) );
	l_pcDialog.SetValue(l_sName);
	int l_nModalResult = l_pcDialog.ShowModal();
	if (wxID_OK == l_nModalResult)
	{
		//get the name
		if (ExistMarkingSetName(l_pcDialog.GetValue() ) )
		{
			SP_MESSAGEBOX(wxT("The inputed name is already existing."),	wxT("Invalid marking set name"), wxOK | wxICON_ERROR);
			return;
		}
		l_sName = l_pcDialog.GetValue();

		//append
		int l_nColorCol,l_nValueColumn;
		if( l_nSelected%2==0 )
		{
			l_nColorCol = l_nSelected;
			l_nValueColumn = l_nSelected+1;
		}
		else
		{
			l_nColorCol = l_nSelected-1;
			l_nValueColumn = l_nSelected;
		}

		m_pcMarkingGrid->AppendCols( 1);
		m_pcMarkingGrid->SetColLabelValue(m_pcMarkingGrid->GetNumberCols()-1,	l_sName + wxT(": Color(s)"));
		m_pcMarkingGrid->AppendCols( 1);
		m_pcMarkingGrid->SetColLabelValue(m_pcMarkingGrid->GetNumberCols()-1,	l_sName + wxT(": Marking"));

		int l_nGridRow = 0;
		for (l_nGridRow = 0; l_nGridRow < m_pcMarkingGrid->GetNumberRows(); l_nGridRow++)
		{
			m_pcMarkingGrid->SetCellValue(l_nGridRow, m_pcMarkingGrid->GetNumberCols()-2, m_pcMarkingGrid->GetCellValue(l_nGridRow, l_nColorCol) );
			m_pcMarkingGrid->SetCellAlignment(l_nGridRow, m_pcMarkingGrid->GetNumberCols()-2,wxALIGN_RIGHT, wxALIGN_CENTER);
			m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRow,m_pcMarkingGrid->GetNumberCols()-2, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcMarkingGrid->SetCellValue(l_nGridRow, m_pcMarkingGrid->GetNumberCols()-1, m_pcMarkingGrid->GetCellValue(l_nGridRow, l_nValueColumn) );
			m_pcMarkingGrid->SetCellAlignment(l_nGridRow, m_pcMarkingGrid->GetNumberCols()-1, wxALIGN_RIGHT, wxALIGN_CENTER);
			m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRow,m_pcMarkingGrid->GetNumberCols()-1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			(l_bWhite ? l_bWhite = false : l_bWhite = true );
		}		
	}

}

void SP_DLG_ColStMarkingOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcMarkingGrid->GetSelectedCols();
	if ( ! (l_cSelCols.Count() > 0 ))
	{
		SP_MESSAGEBOX(wxT("Select column(s) to delete."), wxT("Invalid marking choice"), wxOK | wxICON_ERROR);
		return;
	}

	vector<int> l_vnSelCols;
	for(unsigned i = 0; i < l_cSelCols.Count(); i++)
	{
		int l_nSelected = l_cSelCols.Item( i);
		if(l_nSelected%2 == 0)
		{
			l_vnSelCols.push_back( l_nSelected );
			l_vnSelCols.push_back( l_nSelected+1 );
		}
		else
		{
			l_vnSelCols.push_back( l_nSelected-1 );
			l_vnSelCols.push_back( l_nSelected );
		}
	}

	vector<int>::iterator it;
	it = std::unique(l_vnSelCols.begin(),l_vnSelCols.end()); 
	l_vnSelCols.resize( it - l_vnSelCols.begin() ); 
	std::sort(l_vnSelCols.begin(),l_vnSelCols.end());
	std::reverse(l_vnSelCols.begin(),l_vnSelCols.end()); 

	for(unsigned i = 0; i < l_vnSelCols.size() ; i++ )
	{
		if( l_vnSelCols[i] == 0 )
		{
			SP_MESSAGEBOX(wxT("Main marking set cannot be removed."), wxT("Error"),wxOK | wxICON_ERROR);
		}
		else if( l_vnSelCols[i] == 1)
		{
			SP_MESSAGEBOX(wxT("Main marking set cannot be removed."), wxT("Error"),wxOK | wxICON_ERROR);
		}
		else
		{
			m_pcMarkingGrid->DeleteCols( l_vnSelCols[i] );
		}
	}
}

void SP_DLG_ColStMarkingOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{

		wxArrayInt l_pcSelCols = m_pcMarkingGrid->GetSelectedCols();

		if (l_pcSelCols.Count() != 1)
		{

			SP_MESSAGEBOX(wxT("Select a single column to rename."),
					wxT("Invalid marking marking choice"), wxOK | wxICON_ERROR);
			return;

		}

		if (l_pcSelCols.Item( 0)%2 == 0)
		{

			SP_MESSAGEBOX(wxT("The colum of color/predicate/function cannot be renamed."),
					wxT("Invalid marking set choice"), wxOK | wxICON_ERROR);
			return;

		}

		if (l_pcSelCols.Item( 0) == 1)
		{

			SP_MESSAGEBOX(wxT("Main marking set cannot be renamed."),
					wxT("Invalid marking set choice"), wxOK | wxICON_ERROR);
			return;

		}

		wxTextEntryDialog l_pcDialog( this, wxT("New name of the marking set: ") +
		m_pcMarkingGrid->GetColLabelValue( l_pcSelCols.Item( 0 ) ), wxT("Rename marking set"));

		l_pcDialog.SetValue(m_pcMarkingGrid->GetColLabelValue(l_pcSelCols.Item( 0) ) );

		int l_nModalResult = l_pcDialog.ShowModal();

		if (wxID_OK == l_nModalResult)
		{

			if (l_pcDialog.GetValue().Find(wxT("@")) != -1)
			{

				SP_MESSAGEBOX(
						wxT("Do not use the symbol @ in the marking set name."),
						wxT("Invalid marking set name"), wxOK | wxICON_ERROR);
				return;

			}

			if (ExistMarkingSetName(l_pcDialog.GetValue() ) )
			{

				SP_MESSAGEBOX(
						wxT("Do not use an already existing marking set name."),
						wxT("Invalid marking set name"),
						wxOK | wxICON_ERROR);

				return;

			}

			m_pcMarkingGrid->SetColLabelValue(l_pcSelCols.Item( 0),
					l_pcDialog.GetValue() );

		}

	}

}

void SP_DLG_ColStMarkingOverview::LoadData()
{
	if(! m_pcGraph )
		return;

	SP_DS_ColListAttribute* l_pcColList;

	SP_ListNode::const_iterator l_itElem;
	vector<SP_DS_Node*> l_vPlaceNodes; 
	SP_DS_Nodeclass* l_pcNodeclass;	
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back( dynamic_cast<SP_DS_Node*>(*l_itElem) );
		}
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back( dynamic_cast<SP_DS_Node*>(*l_itElem) );
		}
	}

	
	bool l_bWhite = false;
	wxString l_sPlaceName;
	wxString l_sLoadName = wxT("-1");

	int l_nGridRowNumber = 0;

	if (m_pcMarkingGrid->GetNumberRows() > 0)
	{
		m_pcMarkingGrid->DeleteRows(0, m_pcMarkingGrid->GetNumberRows());
	}
	if (m_pcMarkingGrid->GetNumberCols() > 0)
	{
		m_pcMarkingGrid->DeleteCols(0, m_pcMarkingGrid->GetNumberCols());
	}

	LoadSetNames();

	//for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	for(unsigned int l_nPos = 0; l_nPos < l_vPlaceNodes.size(); l_nPos++)
	{

		l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_vPlaceNodes[l_nPos]->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//( *l_itElem )->Update( true);

		m_pcMarkingGrid->AppendRows( 1);
		m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST) );

		if(l_pcColList->GetRowCount() == 0)
		{
			l_nGridRowNumber++;
		}

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if(i >= 1)
			{
				m_pcMarkingGrid->AppendRows( 1);
				m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, wxT(""));
			}

			for (unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
			{				
				m_pcMarkingGrid->SetCellValue(l_nGridRowNumber, j, l_pcColList->GetCell(i, j));	
				m_pcMarkingGrid->SetCellAlignment(l_nGridRowNumber, j, wxALIGN_RIGHT, wxALIGN_CENTER);
				m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRowNumber,j, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

				if(j%2==1)
					(l_bWhite ? l_bWhite = false : l_bWhite = true );
			}
			
			l_nGridRowNumber++;
		}			

	}

/*
	if ( (m_pcMarkingGrid->GetNumberRows() > 0 )
			&& (m_pcMarkingGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcMarkingGrid->GetCellValue( 0, 0);
	}
*/

}

void SP_DLG_ColStMarkingOverview::SaveData()
{
	m_pcMarkingGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(! l_pcGraph )
		return;
	SP_DS_ColListAttribute* l_pcColList;

	SP_ListNode::const_iterator l_itElem;
	vector<SP_DS_Node*> l_vPlaceNodes; 
	SP_DS_Nodeclass* l_pcNodeclass;	
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back( dynamic_cast<SP_DS_Node*>(*l_itElem) );
		}
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back( dynamic_cast<SP_DS_Node*>(*l_itElem) );
		}
	}



	int l_nGridRowNumber = 0;


	for(unsigned int l_nPos = 0; l_nPos < l_vPlaceNodes.size(); l_nPos++)
	{

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST) );

		l_pcColList->Clear();
		l_pcColList->RemoveAllColumns();

		int l_nBegin = l_nGridRowNumber;		
		
		for(int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++ )
		{
			l_pcColList->AppendEmptyColum();
			l_pcColList->SetColLabel(i,m_pcMarkingGrid->GetColLabelValue(i));
		}		

		l_pcColList->AppendEmptyRow();

		wxString l_sMainMarking;

		for(int j = l_nBegin; j < m_pcMarkingGrid->GetNumberRows(); j++)
		{						
			l_nGridRowNumber++;

			for (int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
			{
				wxString l_sValue = m_pcMarkingGrid->GetCellValue(j, i);
				l_pcColList->SetCell(j-l_nBegin, i, l_sValue);				
			}

			if( j < m_pcMarkingGrid->GetNumberRows()-1 )
			{
				wxString l_sRowLabelValue = m_pcMarkingGrid->GetRowLabelValue(j+1);
				if(l_sRowLabelValue == wxT(""))
					l_pcColList->AppendEmptyRow();
				else
					break;
			}			

		}
		//update it the first time
		l_vPlaceNodes[l_nPos]->Update(TRUE);

		
		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.Initialize())
			return;
	

		////////////////////////////////////////////////////////
					l_sMainMarking = wxT("");
					map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
					if( ! l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos],l_mColorToMarkingMap,false) )
						return;
					map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;	
				
					wxString l_sNodeClass = l_vPlaceNodes[l_nPos]->GetNodeclass()->GetName();

					if( l_sNodeClass == SP_DS_CONTINUOUS_PLACE)
					{
						double l_dMarking = 0;
						for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
						{		
							l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;			
						}
						l_sMainMarking <<l_dMarking;
					}
					else
					{
						long l_nMarking = 0;
						for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
						{			
							l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;			
						}
						l_sMainMarking <<l_nMarking;
					}
		////////////////////////////////////////////////////////////////
		
		SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast< SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute( wxT("Marking") ) );
		if(l_pcMarkingAttr)
			l_pcMarkingAttr->SetValueString(l_sMainMarking);

		l_vPlaceNodes[l_nPos]->Update(TRUE);
	}


	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_ColStMarkingOverview::OnCellValueChanged(wxGridEvent& ev)
{

	ev.Skip();

}

void SP_DLG_ColStMarkingOverview::OnGridCellSelected(wxGridEvent& ev)
{

	if ( (m_pcMarkingGrid->GetNumberRows() > 0 )
			&& (m_pcMarkingGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcMarkingGrid->GetCellValue(ev.GetRow(),
				ev.GetCol() );

	}

	ev.Skip();

}

void SP_DLG_ColStMarkingOverview::LoadSetNames()
{
	SP_DS_Nodeclass* l_pcNodeclass;	
	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_DISCRETE_PLACE );
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
			for(unsigned int i = 0; i < l_pcAttr->GetColCount(); i++ )
			{
				wxString l_sSetName = l_pcAttr->GetColLabel(i);
				m_pcMarkingGrid->AppendCols(1);
				m_pcMarkingGrid->SetColLabelValue(i, l_sSetName);
			}
			return;
		}
	}


	l_pcNodeclass= m_pcGraph->GetNodeclass( SP_DS_CONTINUOUS_PLACE );
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0)
		{
			SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
			for(unsigned int i = 0; i < l_pcAttr->GetColCount(); i++ )
			{
				wxString l_sSetName = l_pcAttr->GetColLabel(i);
				m_pcMarkingGrid->AppendCols(1);
				m_pcMarkingGrid->SetColLabelValue(i, l_sSetName);
			}
			return;
		}
	}
	
	
}

bool SP_DLG_ColStMarkingOverview::ExistMarkingSetName(const wxString& p_sName)
{

	for (int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
	{

		if (p_sName ==  m_pcMarkingGrid->GetColLabelValue(i) )
		{

			return true;

		}

	}

	return false;

}

void SP_DLG_ColStMarkingOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}

void SP_DLG_ColStMarkingOverview::OnRandomMarking(wxCommandEvent& p_cEvent)
{
	if(m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS )
	{
		SP_MESSAGEBOX(wxT("Not support for this net class yet."), wxT("Notification"), wxOK);
		return;
	}


	SaveData();
	RefreshFrame();

	wxArrayInt l_cSelCols = m_pcMarkingGrid->GetSelectedCols();

	if ( ! (l_cSelCols.Count() > 0 ))
	{
		SP_MESSAGEBOX(wxT("Select column to randomly assign initial marking."), wxT("Invalid marking choice"), wxOK | wxICON_ERROR);
		return;
	}

	vector<int> l_vnSelCols;
	int l_nSelColumn = l_cSelCols.Item( 0);

	SP_DLG_RandomMarking l_cDlg(l_nSelColumn,this);

	if (l_cDlg.ShowModal() == wxID_OK)
	{		
		LoadData();
	}

	l_cDlg.Destroy();

}
