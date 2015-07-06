//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include <wx/event.h>
#include <wx/tipwin.h>

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECKPREDICATE,
	SP_ID_BUTTON_EDIT, 	
	SP_ID_BUTTON_CHECK, 

	SP_ID_BUTTON_OVERVIEW,
	SP_ID_GRID_PARAMETER
};

BEGIN_EVENT_TABLE( SP_WDG_DialogColList, SP_WDG_DialogBase )
#if wxABI_VERSION > 30000
	EVT_GRID_CELL_CHANGED( SP_WDG_DialogColList::OnGridCellValueChanged )
#else
	EVT_GRID_CELL_CHANGE( SP_WDG_DialogColList::OnGridCellValueChanged )
#endif
	EVT_GRID_SELECT_CELL( SP_WDG_DialogColList::OnGridCellSelected )
END_EVENT_TABLE()

SP_WDG_DialogColList::SP_WDG_DialogColList(const wxString& p_sPage, unsigned int p_nOrdering) :
SP_WDG_DialogBase(p_sPage, p_nOrdering),
m_eColListType(SP_COLLIST_STRING),
m_pcColList(NULL),
m_pcValueGrid(NULL),
m_pcDlg(NULL)
{
}

SP_WDG_DialogColList::~SP_WDG_DialogColList()
{
}

SP_WDG_DialogBase* SP_WDG_DialogColList::Clone()
{

	return new SP_WDG_DialogColList(GetName(), GetDialogOrdering());

}

bool SP_WDG_DialogColList::AddToDialog(
		const SP_ListAttribute* p_ptlAttributes,
		SP_DLG_ShapeProperties* p_pcDlg,
		bool p_bOverview)
{
	CHECK_POINTER( p_ptlAttributes, return FALSE );
	CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
	CHECK_POINTER( p_pcDlg, return FALSE );

	m_pcDlg = p_pcDlg;

	// remember the list of attributes, we display
	m_tlAttributes = *p_ptlAttributes;
	m_bMultiple = (p_ptlAttributes->size() > 1 );

	for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin();
			it != m_tlAttributes.end(); it++)
	{
		SP_DS_Attribute* l_pcAttr = *it;
		m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
		if(m_pcColList)
		{
			m_eColListType = m_pcColList->GetColListType();
			break;
		}
	}

    wxString l_sPage = GetName() + wxT(":") + m_pcColList->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	//
	SP_DS_Node* l_pcNode= (SP_DS_Node*)(m_pcColList->GetParent());
	m_sNetClassName = l_pcNode->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName();
	
	//

	wxBoxSizer* l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, m_pcColList->GetName()), 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	AddShowFlag(l_pcPage, l_pcSizer, m_pcColList);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pcValueGrid = new wxGrid( l_pcPage, SP_ID_GRID_PARAMETER + m_nDialogID + wxID_HIGHEST,
			wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );

	m_pcValueGrid->CreateGrid( 0, 0);

	m_pcValueGrid->EnableEditing( true);
	m_pcValueGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcValueGrid->SetDefaultColSize( 120, TRUE);
	m_pcValueGrid->SetDefaultRowSize( 20, TRUE);

	m_pcValueGrid->SetColLabelSize( 16);
	m_pcValueGrid->SetRowLabelSize( 0);

	LoadData();

	l_pcSizer->Add(m_pcValueGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	
	//////////////////////////
	if( m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

		l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ADD + m_nDialogID
				+ wxID_HIGHEST, wxT("Add ") + GetName() ), 1, wxLEFT | wxRIGHT | wxTOP, 5);  
	
		l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_DELETE + m_nDialogID
				+ wxID_HIGHEST, wxT("Delete ") + GetName() ), 1, wxLEFT | wxRIGHT | wxTOP, 5); 

		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

		l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID
				+ wxID_HIGHEST, wxT("Check predicate") ), 1, wxALL, 5);

		l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
				+ wxID_HIGHEST, wxT("Check ") + GetName() ), 1, wxALL, 5); 

		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	}

	//////////////////////////


	l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer->Add(new wxButton( l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID +
			wxID_HIGHEST, wxT("Lists Overview") ), 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);


	ConnectEvents();
	p_pcDlg->PushEventHandler( this);

	return TRUE;

}

bool SP_WDG_DialogColList::OnDlgOk()
{
	m_pcValueGrid->MoveCursorLeft(false);

	SaveData();

	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_DialogColList::LoadData()
{

	if( m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
		CHECK_BOOL(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

		if (m_pcValueGrid->GetNumberRows() > 0)
		{
			m_pcValueGrid->DeleteRows(0, m_pcValueGrid->GetNumberRows());
		}
	
		if (m_pcValueGrid->GetNumberCols() > 0)
		{
			m_pcValueGrid->DeleteCols(0, m_pcValueGrid->GetNumberCols());
		}
	
		m_pcValueGrid->AppendCols(l_pcColList->GetColCount());


		if( !m_bMultiple )
		{
			if(l_pcColList->GetRowCount()>0)
			{		
				m_pcValueGrid->AppendRows(l_pcColList->GetRowCount());
			
			}

			for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
			{	
				m_pcValueGrid->SetColLabelValue(i,l_pcColList->GetColLabel(i));
				for(unsigned int j = 0; j < l_pcColList->GetRowCount(); j++)
				{
					m_pcValueGrid->SetCellValue(j, i, l_pcColList->GetCell(j,i).c_str());
					m_pcValueGrid->SetCellAlignment(j, i, wxALIGN_LEFT, wxALIGN_TOP);
				}		
			}	
		}
		else
		{
			m_pcValueGrid->AppendRows(1);
			for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
			{	
				m_pcValueGrid->SetColLabelValue(i,l_pcColList->GetColLabel(i));
				if( i == 0 )				
					m_pcValueGrid->SetCellValue(0, i, wxT("true"));
				else
					m_pcValueGrid->SetCellValue(0, i, SP_WILDCARD);
				m_pcValueGrid->SetCellAlignment(0, i, wxALIGN_LEFT, wxALIGN_TOP);
			}
		}
	}

	else
	{
		CHECK_POINTER(m_pcColList, return false);
		bool l_bWhite = true;

		if(m_pcValueGrid->GetNumberCols() > 0)
		{
			m_pcValueGrid->DeleteCols(0, m_pcValueGrid->GetNumberCols());
		}
		m_pcValueGrid->AppendCols(m_pcColList->GetColCount());

		for (unsigned int i=0; i<m_pcColList->GetColCount(); i++)
		{
			m_pcValueGrid->SetColLabelValue( i, m_pcColList->GetColLabel(i));
			m_pcValueGrid->SetColSize(i, 100);
		}
		if (m_pcValueGrid->GetNumberRows() > 0)
		{
			m_pcValueGrid->DeleteRows( 0, m_pcValueGrid->GetNumberRows() );
		}

		m_pcValueGrid->AppendRows(m_pcColList->GetRowCount() );

		for (unsigned int l_nRow = 0; l_nRow < m_pcColList->GetRowCount(); l_nRow++)
		{
			m_pcValueGrid->SetReadOnly(l_nRow, 0, true);
			for(unsigned int l_nCol = 0; l_nCol < m_pcColList->GetColCount(); l_nCol++)
			{
				wxString l_sValue = SP_WILDCARD;
				if(!m_bMultiple || l_nCol == 0)
				{
					l_sValue = m_pcColList->GetCell(l_nRow, l_nCol);
				}
				m_pcValueGrid->SetCellValue(l_nRow, l_nCol, l_sValue);
				m_pcValueGrid->SetCellBackgroundColour(l_nRow, l_nCol,
													(l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			}
			(l_bWhite ? l_bWhite = false : l_bWhite = true );
		}
	}

	return true;

}

bool SP_WDG_DialogColList::SaveData()
{
	m_pcValueGrid->SaveEditControlValue();

	if( m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		if( !m_bMultiple )
		{
			SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();
			SP_DS_Attribute* l_pcFunctionAttr = *(l_Iter);
			CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcFunctionAttr);
			l_pcColList->Clear();

			for(int i = 0; i < m_pcValueGrid->GetNumberCols(); i++)
			{
				l_pcColList->SetColLabel(i,m_pcValueGrid->GetColLabelValue(i));

				for(int j = 0; j < m_pcValueGrid->GetNumberRows(); j++)
				{
					wxString l_sValue = m_pcValueGrid->GetCellValue(j, i);
					if(i == 0)
						l_pcColList->AppendEmptyRow();

					//if((m_bMultiple && l_sValue != SP_WILDCARD) || !m_bMultiple)
					l_pcColList->SetCell(j, i, l_sValue);			
				}
			}	
		}
		else
		{
			//to deal with a group of nodes
			for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				for (int l_nRow = 0; l_nRow < m_pcValueGrid->GetNumberRows(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcValueGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcValueGrid->GetCellValue(l_nRow, l_nCol);
						if((m_bMultiple && l_sValue != SP_WILDCARD) )
						{
							l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);
						}
					}
				}
			}

		}

	}
	else
	{
		for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
		{
			SP_DS_Attribute* l_pcAttr = *it;
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
			for (int l_nRow = 0; l_nRow < m_pcValueGrid->GetNumberRows(); l_nRow++)
			{
				for(int l_nCol = 0; l_nCol < m_pcValueGrid->GetNumberCols(); l_nCol++)
				{
					wxString l_sValue = m_pcValueGrid->GetCellValue(l_nRow, l_nCol);
					if((m_bMultiple && l_sValue != SP_WILDCARD) || !m_bMultiple)
					{
						l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);
					}
				}
			}
		}
	}

	return true;
}

void SP_WDG_DialogColList::OnOverview(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(m_pcColList->GetName(),m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		LoadData();

	}

	l_pcDlg->Destroy();

}

void SP_WDG_DialogColList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			( wxObjectEventFunction ) ( wxEventFunction ) (wxCommandEventFunction )
					&SP_WDG_DialogColList::OnOverview);

	Connect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnAdd);

	Connect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnDelete);

	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnCheck);

	Connect(
			SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnCheckPredicate);


}

void SP_WDG_DialogColList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnAdd);

	Disconnect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnDelete);


	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnCheck);

	Disconnect(
			SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogColList::OnCheckPredicate);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			( wxObjectEventFunction ) ( wxEventFunction ) (wxCommandEventFunction )
					&SP_WDG_DialogColList::OnOverview);

}

void SP_WDG_DialogColList::OnGridCellSelected(wxGridEvent& ev)
{
	if(m_pcValueGrid == ev.GetEventObject())
	{
		if ((m_pcValueGrid->GetNumberRows() > 0 ) &&
			(m_pcValueGrid->GetNumberCols() > 0 ))
		{
			m_sOldCellValue = m_pcValueGrid->GetCellValue(ev.GetRow(), ev.GetCol() );
		}
	}
	ev.Skip();
}

void SP_WDG_DialogColList::OnGridCellValueChanged(wxGridEvent& ev)
{
//TODO: temporary disable check, need to be tested for correctness
#if 0
	if(m_pcValueGrid == ev.GetEventObject() && ev.GetCol() > 0)
	{
		wxString l_sValue = m_pcValueGrid->GetCellValue(ev.GetRow(), ev.GetCol() );

		if(m_eColListType == SP_COLLIST_BOOL)
		{
			if(l_sValue != wxT("0") && l_sValue != wxT("1"))
			{
				SP_MESSAGEBOX(
						wxT("The value must be \"0\" or \"1\". The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

				m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
			}
		}
		else if(m_eColListType == SP_COLLIST_CHAR)
		{
			if(l_sValue.length() > 1)
			{
				SP_MESSAGEBOX(
						wxT("The value must be a single char. The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

				m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
			}
		}
		else if(m_eColListType == SP_COLLIST_DOUBLE)
		{
			double l_nDouble = -1;
			if ( ! (l_sValue.ToDouble( &l_nDouble) ))
			{
				SP_MESSAGEBOX(
						wxT("The value must be a real value. The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

				m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
			}
		}
		else if(m_eColListType == SP_COLLIST_INTEGER)
		{
			long l_nValue = -1;
			if ( ! (l_sValue.ToLong( &l_nValue) ))
			{
				SP_MESSAGEBOX(
						wxT("The value must be an integer value. The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

				m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
			}
		}
		else if(m_eColListType == SP_COLLIST_UNSIGNED_INTEGER)
		{
			unsigned long l_nValue = -1;
			if ( ! (l_sValue.ToULong( &l_nValue) ))
			{
				SP_MESSAGEBOX(
						wxT("The value must be an unsigned integer value. The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

				m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
			}
		}
	}
#endif
	ev.Skip();
}


void SP_WDG_DialogColList::OnAdd(wxCommandEvent& p_cEvent)
{

	m_pcValueGrid->AppendRows(1);

}


void SP_WDG_DialogColList::OnDelete( wxCommandEvent& p_cEvent )
{
	if( m_pcValueGrid->GetGridCursorRow() == 0)
	{
		SP_MESSAGEBOX(wxT("Main function can not be deleted"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	if ( m_pcValueGrid->IsSelection() )
    {
		m_pcValueGrid->BeginBatch();
        for ( int n = 0; n < m_pcValueGrid->GetNumberRows(); )
        {
            if ( m_pcValueGrid->IsInSelection( n , 0 ) )
            	m_pcValueGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        m_pcValueGrid->EndBatch();
    }

}

void SP_WDG_DialogColList::OnCheckPredicate(wxCommandEvent& p_cEvent)
{
	if (m_pcValueGrid->GetNumberRows() == 0)
	{
		return;
	}
	if( m_pcValueGrid->GetGridCursorCol() != 0)
	{
		SP_MESSAGEBOX(wxT("Please select a predicate to be checked"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	wxString l_sPredicate = m_pcValueGrid->GetCellValue(m_pcValueGrid->GetGridCursorRow(), m_pcValueGrid->GetGridCursorCol());

/*
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	l_cValueAssign.InitializeColorset(l_cColorSetClass);

	/////////////////
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return;
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

	if(!l_pcNode)
		return;
*/
	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return;
	if( ! l_cSyntaxChecking.CheckPredicateInRateFunction(l_sPredicate, wxT("")))
	{
		wxString l_sVariables = wxT("The predicate definition \"");
		l_sVariables += l_sPredicate;
		l_sVariables += wxT("\" is not correct.");
		new wxTipWindow(m_pcDlg, l_sVariables, 1000);

		return;
	}


	wxString l_sVariables = wxT("The predicate definition \"");
	l_sVariables += l_sPredicate;
	l_sVariables += wxT("\" is correct.");

	new wxTipWindow(m_pcDlg, l_sVariables, 1000);

}

void SP_WDG_DialogColList::OnCheck(wxCommandEvent& p_cEvent)
{
	
	if (m_pcValueGrid->GetNumberRows() == 0)
	{
		return;
	}

	if( m_pcValueGrid->GetGridCursorCol() == 0)
	{
		SP_MESSAGEBOX(wxT("Please select function to be checked"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	SP_DS_StParser l_cParser;

	wxArrayString* l_plMissingVariableList = 0;


	wxString l_sVariables = wxT("");

	wxString l_sFormula;
	wxString l_sOriginalFormula;

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	SP_Data* l_pcDataNode = l_pcAttr->GetParent();

	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{
		return;
	}

	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcDataNode);
	CHECK_POINTER( l_pcNode, return );

	bool l_bCheckResult = true;
	if( GetName() == wxT("Periodic") )
	{
		int l_nSelCol =  m_pcValueGrid->GetGridCursorCol();
		l_nSelCol = (l_nSelCol-1)/3;
		wxString l_sBegin = m_pcValueGrid->GetCellValue(m_pcValueGrid->GetGridCursorRow(), (3*l_nSelCol+1) );
		wxString l_sRepetition = m_pcValueGrid->GetCellValue(m_pcValueGrid->GetGridCursorRow(), (3*l_nSelCol+2) );
		wxString l_sEnd = m_pcValueGrid->GetCellValue(m_pcValueGrid->GetGridCursorRow(), (3*l_nSelCol+3) );
		l_sFormula = wxT("FixedTimedFiring_Periodic(")+ l_sBegin +wxT(",")+ l_sRepetition+wxT(",")+l_sEnd+wxT(")");
		l_sOriginalFormula = l_sFormula;
		l_bCheckResult = l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode,true);
	}
	if( GetName() == wxT("Delay") )
	{
		l_sFormula = m_pcValueGrid->GetCellValue(m_pcValueGrid->GetGridCursorRow(), m_pcValueGrid->GetGridCursorCol());
		l_sFormula = wxT("TimedFiring(")+ l_sFormula +wxT(")");
		l_sOriginalFormula = l_sFormula;
		l_bCheckResult = l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode,true);
	}	

	if (!l_bCheckResult)
	{

		l_plMissingVariableList = l_cParser.GetMissingVariableList();

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sFormula;
		l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

		for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
		{

			l_sVariables += (*l_plMissingVariableList)[i];
			l_sVariables += wxT("\n");

		}

		new wxTipWindow(m_pcDlg, l_sVariables, 1000);
		return;

	}
	

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sOriginalFormula;
		l_sVariables += wxT("\" is correct.");

		new wxTipWindow(m_pcDlg, l_sVariables, 1000);
	
}
