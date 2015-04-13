//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StFunctionList.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"

#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"

#include "sp_ds/SP_DS_Node.h"

#include <wx/event.h>
#include <wx/tipwin.h>

enum
{

	SP_ID_BUTTON_EDIT, 
	SP_ID_BUTTON_OVERVIEW,

	SP_ID_BUTTON_CHECK, 
	SP_ID_BUTTON_ASSISTENT,

	SP_ID_GRID_FUNCTION

};
BEGIN_EVENT_TABLE( SP_WDG_StFunctionList, SP_WDG_DialogBase )
#if wxABI_VERSION > 30000
	EVT_GRID_CELL_CHANGED( SP_WDG_StFunctionList::OnGridCellValueChanged )
#else
	EVT_GRID_CELL_CHANGE( SP_WDG_StFunctionList::OnGridCellValueChanged )
#endif
	EVT_GRID_SELECT_CELL( SP_WDG_StFunctionList::OnGridCellSelected )
END_EVENT_TABLE()

SP_WDG_StFunctionList::SP_WDG_StFunctionList(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering),
	 m_pcDlg(NULL)
{
}

SP_WDG_StFunctionList::~SP_WDG_StFunctionList()
{
}

SP_WDG_DialogBase*
SP_WDG_StFunctionList::Clone()
{

	return new SP_WDG_StFunctionList(GetName(), GetDialogOrdering());

}

bool SP_WDG_StFunctionList::AddToDialog(
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

    wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

	AddShowFlag(l_pcPage, l_pcSizer, dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr));

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

	m_pcFunctionGrid->SetColLabelValue(0, GetName() + wxT(" set") );
	m_pcFunctionGrid->SetColSize(0, 100);

	m_pcFunctionGrid->SetColLabelValue(1, GetName() );
	m_pcFunctionGrid->SetColSize(1, 200);

	LoadData();

	l_pcSizer->Add(m_pcFunctionGrid, 1, wxALL | wxEXPAND, 5);

	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Check ") + GetName() ), 1, wxALL | wxCENTER, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ASSISTENT + m_nDialogID
			+ wxID_HIGHEST, GetName() + wxT(" assistant") ), 1, wxALL | wxCENTER, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, GetName() + wxT(" overview") ), 1, wxALL | wxCENTER, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_StFunctionList::OnDlgOk()
{
	m_pcFunctionGrid->MoveCursorLeft(false);

	SaveData();

	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_StFunctionList::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL((SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType()), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	bool l_bWhite = false;

	if (m_pcFunctionGrid->GetNumberRows() > 0)
	{
		m_pcFunctionGrid->DeleteRows(0, m_pcFunctionGrid->GetNumberRows());
	}

	m_pcFunctionGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcFunctionGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0));
		m_pcFunctionGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_TOP);
		m_pcFunctionGrid->SetReadOnly(i, 0, true);

		m_pcFunctionGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		/////////////////
		wxString l_sValue = wxT("*");
		if( !m_bMultiple ) 
			l_sValue = l_pcColList->GetCell(i,1);
		////////////////////

		m_pcFunctionGrid->SetCellValue(i, 1, l_sValue );

		//m_pcFunctionGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1));
		m_pcFunctionGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_TOP);

		m_pcFunctionGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	return true;
}

bool SP_WDG_StFunctionList::SaveData()
{
	m_pcFunctionGrid->SaveEditControlValue();

	if( m_bMultiple )
	{
			//to deal with a group of nodes
			for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				for (int l_nRow = 0; l_nRow < m_pcFunctionGrid->GetNumberRows(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcFunctionGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcFunctionGrid->GetCellValue(l_nRow, l_nCol);
						if((m_bMultiple && l_sValue != wxT("*")) )
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

	for (int i = 0; i < m_pcFunctionGrid->GetNumberRows(); i++)
	{
		l_pcColList->SetCell(i, 0, m_pcFunctionGrid->GetCellValue(i, 0));
		l_pcColList->SetCell(i, 1, m_pcFunctionGrid->GetCellValue(i, 1));
	}

	return true;
}

void SP_WDG_StFunctionList::OnCheck(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	SP_DS_StParser l_cParser;

	wxArrayString* l_plMissingVariableList = 0;

	wxArrayString* l_psUnusedModifierPlaceList = 0;
	wxArrayString* l_psUnusedPrePlaceList = 0;

	wxString l_sVariables = wxT("");
	wxString l_sFormula = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), 1);

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	SP_Data* l_pcDataNode = l_pcAttr->GetParent();

	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{
		return;
	}

	SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcDataNode);
	CHECK_POINTER( l_pcNode, return );

	if (!(l_cParser.CheckFormulaFunction(l_sFormula, l_pcNode)))
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
		l_sVariables += l_sFormula;
		l_sVariables += wxT("\" is correct.");

		new wxTipWindow(m_pcDlg, l_sVariables, 1000);
	}

}

void SP_WDG_StFunctionList::OnAssistent(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	wxString l_pcReturnText = m_pcFunctionGrid->GetCellValue(
			m_pcFunctionGrid->GetGridCursorRow(), 1);

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

		m_pcFunctionGrid->SetCellValue(m_pcFunctionGrid->GetGridCursorRow(), 1,
				l_pcDlg->GetReturnText());

	}

	l_pcDlg->Destroy();

}

void SP_WDG_StFunctionList::OnOverview(wxCommandEvent& p_cEvent)
{
	wxString l_sNodeclass = m_tlAttributes.front()->GetParent()->GetClassName();
	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(l_sNodeclass, m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		LoadData();

	}

	l_pcDlg->Destroy();

}

void SP_WDG_StFunctionList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnCheck);

	Connect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnAssistent);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnOverview);

}

void SP_WDG_StFunctionList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnCheck);

	Disconnect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnAssistent);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StFunctionList::OnOverview);

}

void SP_WDG_StFunctionList::OnGridCellSelected(wxGridEvent& ev)
{
	if(m_pcFunctionGrid == ev.GetEventObject())
	{
		m_sOldCellValue = m_pcFunctionGrid->GetCellValue(ev.GetRow(), ev.GetCol());
	}

	ev.Skip();
}

void SP_WDG_StFunctionList::OnGridCellValueChanged(wxGridEvent& ev)
{

	ev.Skip();

}
