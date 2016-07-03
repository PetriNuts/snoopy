//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStFunctionList.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"

#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"


#include "sp_ds/SP_DS_Node.h"

#include <wx/event.h>
#include <wx/tipwin.h>

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECKPREDICATE,

	SP_ID_BUTTON_EDIT, 
	SP_ID_BUTTON_OVERVIEW,
	SP_ID_BUTTON_CHECK, 
	SP_ID_BUTTON_ASSISTENT,

	SP_ID_GRID_FUNCTION

};
BEGIN_EVENT_TABLE( SP_WDG_ColStFunctionList, SP_WDG_DialogBase )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_WDG_ColStFunctionList::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_WDG_ColStFunctionList::OnGridCellValueChanged ) 
#endif

EVT_GRID_SELECT_CELL( SP_WDG_ColStFunctionList::OnGridCellSelected )

END_EVENT_TABLE()

SP_WDG_ColStFunctionList::SP_WDG_ColStFunctionList(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_ColStFunctionList::~SP_WDG_ColStFunctionList()
{
}

SP_WDG_DialogBase*
SP_WDG_ColStFunctionList::Clone()
{

	return new SP_WDG_ColStFunctionList(GetName(), GetDialogOrdering());

}

bool SP_WDG_ColStFunctionList::AddToDialog(
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
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());

	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

    wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 1, wxALL , 5);
	AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcFunctionGrid = new wxGrid(l_pcPage, SP_ID_GRID_FUNCTION + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER );

	m_pcFunctionGrid->CreateGrid(0, 0);

	m_pcFunctionGrid->EnableEditing(true);
	m_pcFunctionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcFunctionGrid->SetDefaultColSize(120, TRUE );
	m_pcFunctionGrid->SetDefaultRowSize(20, TRUE );

	m_pcFunctionGrid->SetColLabelSize(16);
	m_pcFunctionGrid->SetRowLabelSize(0);

	m_pcFunctionGrid->AppendCols(2);

	m_pcFunctionGrid->SetColLabelValue(0, wxT("Predicate") );
	m_pcFunctionGrid->SetColSize(0, 100);

	m_pcFunctionGrid->SetColLabelValue(1, GetName() ); //wxT("Function")
	m_pcFunctionGrid->SetColSize(1, 200);

	LoadData();

	l_pcSizer->Add(m_pcFunctionGrid, 1, wxALL | wxEXPAND, 5);

	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	
	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ADD + m_nDialogID
			+ wxID_HIGHEST, wxT("Add ") + GetName() ), 1, wxLEFT | wxRIGHT | wxTOP, 5);  //wxT("Add function")
	
	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_DELETE + m_nDialogID
			+ wxID_HIGHEST, wxT("Delete ") + GetName() ), 1, wxLEFT | wxRIGHT | wxTOP, 5); //wxT("Delete function")

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID
			+ wxID_HIGHEST, wxT("Check predicate") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Check ") + GetName() ), 1, wxALL, 5);  //wxT("Check function")

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ASSISTENT + m_nDialogID
			+ wxID_HIGHEST, GetName() + wxT(" assistent") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, GetName() + wxT(" overview") ), 1, wxALL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_ColStFunctionList::OnDlgOk()
{
	//if(!m_bMultiple)
	{
		SaveData();
	}
	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_ColStFunctionList::LoadData()
{
	
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL( SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

	SP_DS_Node* l_pcNode;	
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
	if(!l_pcNode)
		return false;
	wxString l_sNodeClassName = l_pcNode->GetNodeclass()->GetName();


	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	if (m_pcFunctionGrid->GetNumberRows() > 0)
	{
		m_pcFunctionGrid->DeleteRows(0, m_pcFunctionGrid->GetNumberRows());
	}
	
	if (m_pcFunctionGrid->GetNumberCols() > 0)
	{
		m_pcFunctionGrid->DeleteCols(0, m_pcFunctionGrid->GetNumberCols());
	}
	
	m_pcFunctionGrid->AppendCols(l_pcColList->GetColCount());

	unsigned int l_nMaxRow = 0;
	if(m_bMultiple)
	{
		SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		SP_DS_Nodeclass* l_pcNodeclass;
		l_pcNodeclass= l_pcGraph->GetNodeclass( l_sNodeClassName );
		SP_ListNode::const_iterator l_itElem;		
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			SP_DS_ColListAttribute* l_pcColListAll = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")) );
			if( l_nMaxRow < l_pcColListAll->GetRowCount() )
				l_nMaxRow = l_pcColListAll->GetRowCount();
		}
	}
	else
	{
		l_nMaxRow = l_pcColList->GetRowCount();
	}		

	//append rows
	if(l_nMaxRow > 0)
	{		
		m_pcFunctionGrid->AppendRows( l_nMaxRow );
	}

	for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
	{	
		bool l_bWhite = true;
		m_pcFunctionGrid->SetColLabelValue(i,l_pcColList->GetColLabel(i));
		for(unsigned int j = 0; j < l_pcColList->GetRowCount(); j++)
		{
			wxString l_sValue = SP_WILDCARD;
			if(!m_bMultiple)
			{
				l_sValue = l_pcColList->GetCell(j,i);
			}

			m_pcFunctionGrid->SetCellValue(j, i, l_sValue );
			m_pcFunctionGrid->SetCellAlignment(j, i, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcFunctionGrid->SetCellBackgroundColour(j,i, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true );
		}		
	}	


/*
	if( !m_bMultiple )
		{
			if(l_pcColList->GetRowCount()>0)
			{		
				m_pcFunctionGrid->AppendRows(l_pcColList->GetRowCount());
			
			}

			for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
			{	
				m_pcFunctionGrid->SetColLabelValue(i,l_pcColList->GetColLabel(i));
				for(unsigned int j = 0; j < l_pcColList->GetRowCount(); j++)
				{
					m_pcFunctionGrid->SetCellValue(j, i, l_pcColList->GetCell(j,i).c_str());
					m_pcFunctionGrid->SetCellAlignment(j, i, wxALIGN_LEFT);
				}		
			}	
		}
	else
		{
			m_pcFunctionGrid->AppendRows(1);
			for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
			{	
				m_pcFunctionGrid->SetColLabelValue(i,l_pcColList->GetColLabel(i));
				if( i == 0 )				
					m_pcFunctionGrid->SetCellValue(0, i, wxT("true"));
				else
					m_pcFunctionGrid->SetCellValue(0, i, SP_WILDCARD);
				m_pcFunctionGrid->SetCellAlignment(0, i, wxALIGN_LEFT);
			}
		}

	*/

	return true;
}

bool SP_WDG_ColStFunctionList::SaveData()
{
	m_pcFunctionGrid->SaveEditControlValue();

	if( m_bMultiple )
	{
		for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				for (int l_nRow = 0; l_nRow < m_pcFunctionGrid->GetNumberRows(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcFunctionGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcFunctionGrid->GetCellValue(l_nRow, l_nCol);
						if((m_bMultiple && l_sValue != SP_WILDCARD) )
						{
							l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);
						}
					}
				}
			}
		return true;
	}


	SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();
	SP_DS_Attribute* l_pcFunctionAttr = *(l_Iter);

	CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcFunctionAttr);

	l_pcColList->Clear();

	for(int i = 0; i < m_pcFunctionGrid->GetNumberCols(); i++)
	{

		l_pcColList->SetColLabel(i,m_pcFunctionGrid->GetColLabelValue(i));

		for(int j = 0; j < m_pcFunctionGrid->GetNumberRows(); j++)
		{
			if(i == 0)
				l_pcColList->AppendEmptyRow();

			l_pcColList->SetCell(j, i, m_pcFunctionGrid->GetCellValue(j, i));			
		}
	}	

	return true;
}

void SP_WDG_ColStFunctionList::OnCheck(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	if( m_pcFunctionGrid->GetGridCursorCol() == 0)
	{
		SP_MESSAGEBOX(wxT("Please select function to be checked"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	SP_DS_StParser l_cParser;

	wxArrayString* l_plMissingVariableList = 0;

	wxArrayString* l_psUnusedModifierPlaceList = 0;
	wxArrayString* l_psUnusedPrePlaceList = 0;


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

	bool l_bCheckResult;
	if( GetName() == wxT("Periodic") )
	{
		int l_nSelCol =  m_pcFunctionGrid->GetGridCursorCol();
		l_nSelCol = (l_nSelCol-1)/3;
		wxString l_sBegin = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), (3*l_nSelCol+1) );
		wxString l_sRepetition = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), (3*l_nSelCol+2) );
		wxString l_sEnd = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), (3*l_nSelCol+3) );
		l_sFormula = wxT("FixedTimedFiring_Periodic(")+ l_sBegin +wxT(",")+ l_sRepetition+wxT(",")+l_sEnd+wxT(")");
		l_sOriginalFormula = l_sFormula;
		l_bCheckResult = l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode,true);
	}
	else if( GetName() == wxT("Delay") )
	{
		l_sFormula = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());
		l_sFormula = wxT("TimedFiring(")+ l_sFormula +wxT(")");
		l_sOriginalFormula = l_sFormula;
		l_bCheckResult = l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode,true);
	}	
	else if( GetName() == wxT("Weight") )
	{
		l_sFormula = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());
		l_sFormula = wxT("ImmediateFiring(") + l_sFormula + wxT(")");
		l_sOriginalFormula = l_sFormula;
		l_bCheckResult = l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode,true);
	}	
	else 
	{
		//Stochastic transtions
		l_sFormula = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());
		l_sOriginalFormula = l_sFormula;
		
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.Initialize())
			return;

		l_bCheckResult = l_cSyntaxChecking.CheckFormula(l_pcNode,l_sFormula,&l_cParser,true);
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

		SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR );

	}
	else
	{
		//Consistence checking results		
		l_psUnusedModifierPlaceList = l_cParser.GetUnusedModifierPlaceList();
		l_psUnusedPrePlaceList = l_cParser.GetUnusedPrePlaceList();

		if(l_psUnusedModifierPlaceList->size() > 0 || l_psUnusedPrePlaceList->size() > 0)
		{
			wxString l_sWarningVariables = wxT("");

			if(l_psUnusedModifierPlaceList->size() > 0 )
			{
				l_sWarningVariables = wxT("The formula has unused modifier places: \n ");

				for (unsigned int i = 0; i < l_psUnusedModifierPlaceList->Count(); i++)
				{

					l_sWarningVariables += (*l_psUnusedModifierPlaceList)[i];
					l_sWarningVariables += wxT("\n");
				}
			}

			
			if( l_psUnusedPrePlaceList->size() > 0) 
			{
				l_sWarningVariables += wxT("The formula has unused preplaces: \n ");

				for (unsigned int i = 0; i < l_psUnusedPrePlaceList->Count(); i++)
				{

					l_sWarningVariables += (*l_psUnusedPrePlaceList)[i];
					l_sWarningVariables += wxT("\n");
				}		
			}

			SP_MESSAGEBOX(l_sWarningVariables, wxT("Consistence checking"), wxOK | wxICON_WARNING );
		}
		/////////////////////////////

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sOriginalFormula;
		l_sVariables += wxT("\" is correct.");

		new wxTipWindow(m_pcDlg, l_sVariables, 1000);
	}

}

void SP_WDG_ColStFunctionList::OnAssistent(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	if (m_pcFunctionGrid->GetNumberCols() == 0)
	{
		return;
	}

	if( m_pcFunctionGrid->GetGridCursorCol() == 0)
	{
		SP_MESSAGEBOX(wxT("Please select proper function position"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	wxString l_pcReturnText = m_pcFunctionGrid->GetCellValue(
		m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	SP_Data* l_pcDataNode = l_pcAttr->GetParent();

	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{
		return;
	}

	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcDataNode);
	CHECK_POINTER( l_pcNode, return );

	SP_DLG_StFunctionAssistent* l_pcDlg = new SP_DLG_StFunctionAssistent(SP_ST_PARSER_TYPE_CHECK_FUNCTION,
			l_pcNode, l_pcReturnText, m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		m_pcFunctionGrid->SetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol(),
				l_pcDlg->GetReturnText());

	}

	l_pcDlg->Destroy();

}

void SP_WDG_ColStFunctionList::OnOverview(wxCommandEvent& p_cEvent)
{

	wxString l_sNodeclass = m_tlAttributes.front()->GetParent()->GetClassName();
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(l_sNodeclass,m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		LoadData();

	}

	l_pcDlg->Destroy();

}

void SP_WDG_ColStFunctionList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnAdd);

	Connect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnDelete);

	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnCheck);

	Connect(
			SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnCheckPredicate);

	Connect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnAssistent);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnOverview);

}

void SP_WDG_ColStFunctionList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnAdd);

	Disconnect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnDelete);


	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnCheck);

	Disconnect(
			SP_ID_BUTTON_CHECKPREDICATE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnCheckPredicate);

	Disconnect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnAssistent);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStFunctionList::OnOverview);

}

void SP_WDG_ColStFunctionList::OnGridCellSelected(wxGridEvent& ev)
{
	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(ev.GetEventObject());		
	wxWindowID l_WindowID = l_pcGrid->GetId();

	if(l_WindowID == SP_ID_GRID_FUNCTION)
		m_sOldCellValue = m_pcFunctionGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();

}

void SP_WDG_ColStFunctionList::OnGridCellValueChanged(wxGridEvent& ev)
{

	ev.Skip();

}


void SP_WDG_ColStFunctionList::OnAdd(wxCommandEvent& p_cEvent)
{

	m_pcFunctionGrid->AppendRows(1);

}


void SP_WDG_ColStFunctionList::OnDelete( wxCommandEvent& p_cEvent )
{
	if( m_pcFunctionGrid->GetGridCursorRow() == 0)
	{
		SP_MESSAGEBOX(wxT("Main function can not be deleted"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	int l_nEditRowPos = m_pcFunctionGrid->GetGridCursorRow();
	m_pcFunctionGrid->DeleteRows(l_nEditRowPos);

}

void SP_WDG_ColStFunctionList::OnCheckPredicate(wxCommandEvent& p_cEvent)
{
	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}
	if( m_pcFunctionGrid->GetGridCursorCol() != 0)
	{
		SP_MESSAGEBOX(wxT("Please select a predicate to be checked"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	wxString l_sPredicate = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());

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


/*
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	if(	! l_cValueAssign.InitializeColorset(l_cColorSetClass) )
		return;

	/////////////////
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return;
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

	if(!l_pcNode)
		return;
	/////////////////
	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return;	

	if( ! l_cSyntaxChecking.CheckRateFunctionPredicate(l_pcNode,&l_cColorSetClass) )
		return;
*/



	wxString l_sVariables = wxT("The predicate definition \"");
	l_sVariables += l_sPredicate;
	l_sVariables += wxT("\" is correct.");

	new wxTipWindow(m_pcDlg, l_sVariables, 1000);
	
}
