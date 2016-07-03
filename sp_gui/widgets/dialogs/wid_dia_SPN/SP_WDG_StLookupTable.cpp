//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/21 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTable.h"
#include "sp_ds/SP_DS_Node.h"

#include <wx/event.h>

enum
{
	SP_ID_BUTTON_EDITROW,
	SP_ID_BUTTON_NEWROW,
	SP_ID_BUTTON_DELETEROW,

	SP_ID_GRID_LOOKUPTABLE
};
BEGIN_EVENT_TABLE( SP_WDG_StLookupTable, SP_WDG_DialogBase )
#if wxABI_VERSION > 30000
	EVT_GRID_CELL_CHANGED( SP_WDG_StLookupTable::OnGridCellValueChanged )
#else
	EVT_GRID_CELL_CHANGE( SP_WDG_StLookupTable::OnGridCellValueChanged )
#endif
	EVT_GRID_SELECT_CELL( SP_WDG_StLookupTable::OnGridCellSelected )
END_EVENT_TABLE()


SP_WDG_StLookupTable::SP_WDG_StLookupTable(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering),
m_sOldCellValue(wxT("") ),
m_pcTempLookupTableList(NULL),
m_pcLookupTableGrid(NULL),
m_pcDefaultValueTextCtrl(NULL),
m_pcDlg(NULL)
{
}

SP_WDG_StLookupTable::~SP_WDG_StLookupTable()
{
}

SP_WDG_DialogBase*
SP_WDG_StLookupTable::Clone()
{
	return new SP_WDG_StLookupTable(GetName(), GetDialogOrdering());
}

bool SP_WDG_StLookupTable::AddToDialog(const SP_ListAttribute* p_ptlAttributes,
		SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview)
{

	CHECK_POINTER( p_ptlAttributes, return FALSE );
	CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
	CHECK_POINTER( p_pcDlg, return FALSE );

	m_tlAttributes = *p_ptlAttributes;

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());

	m_pcTempLookupTableList	= dynamic_cast<SP_DS_ColListAttribute*> ((dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr))->Clone());

	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

    wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("") ), 1, wxALL , 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcLookupTableGrid = new wxGrid(l_pcPage, SP_ID_GRID_LOOKUPTABLE
			+ m_nDialogID + wxID_HIGHEST, wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER );

	m_pcLookupTableGrid->CreateGrid(0, 0);

	m_pcLookupTableGrid->EnableEditing(true);
	m_pcLookupTableGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcLookupTableGrid->SetDefaultColSize(120, TRUE );
	m_pcLookupTableGrid->SetDefaultRowSize(20, TRUE );

	m_pcLookupTableGrid->SetColLabelSize(16);
	m_pcLookupTableGrid->SetRowLabelSize(0);

	m_pcLookupTableGrid->AppendCols(2);

	m_pcLookupTableGrid->SetColLabelValue(0, wxT("x") );
	m_pcLookupTableGrid->SetColSize(0, 100);

	m_pcLookupTableGrid->SetColLabelValue(1, wxT("f(x)") );
	m_pcLookupTableGrid->SetColSize(1, 100);

	l_pcSizer->Add(m_pcLookupTableGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_NEWROW + m_nDialogID
			+ wxID_HIGHEST, wxT("New row") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_EDITROW + m_nDialogID
			+ wxID_HIGHEST, wxT("Edit row") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_DELETEROW + m_nDialogID
			+ wxID_HIGHEST, wxT("Delete row") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Default value:") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	m_pcDefaultValueTextCtrl = new wxTextCtrl(l_pcPage, -1, wxT("") );

	l_pcRowSizer->Add(m_pcDefaultValueTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcRowSizer, 0, wxEXPAND);

	/*
	 l_pcSizer = new wxBoxSizer( wxHORIZONTAL );

	 l_pcSizer->Add( new wxButton( l_pcPage, SP_ID_COLLISTSETOVERVIEW + 1 * GetId() +
	 wxID_HIGHEST, wxT("Overview") ),
	 1, wxLEFT | wxRIGHT | wxTOP, 5 );

	 l_pcPage->AddControl( l_pcSizer, 0, wxEXPAND );

	 l_pcSizer = new wxBoxSizer( wxHORIZONTAL );

	 l_pcPage->AddControl( l_pcSizer, 0, wxEXPAND );
	 */
	LoadData();

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_StLookupTable::OnDlgOk()
{
	m_pcLookupTableGrid->MoveCursorLeft(false);

	SaveData();

	return SP_WDG_DialogBase::OnDlgOk();
}

bool SP_WDG_StLookupTable::LoadData()
{
	bool l_bWhite = false;

	if (m_pcLookupTableGrid->GetNumberRows() > 0)
	{
		m_pcLookupTableGrid->DeleteRows(0, m_pcLookupTableGrid->GetNumberRows());
	}

	m_pcLookupTableGrid->AppendRows(m_pcTempLookupTableList->GetRowCount());

	for (unsigned int i = 0; i < m_pcTempLookupTableList->GetRowCount(); i++)
	{
		m_pcLookupTableGrid->SetCellValue(i, 0,
				m_pcTempLookupTableList->GetCell(i, 0));
		m_pcLookupTableGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_TOP);

		m_pcLookupTableGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE
				: *wxLIGHT_GREY));

		m_pcLookupTableGrid->SetCellValue(i, 1,
				m_pcTempLookupTableList->GetCell(i, 1));
		m_pcLookupTableGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_TOP);

		m_pcLookupTableGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE
				: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	m_pcDefaultValueTextCtrl->SetValue(
			(*m_tlAttributes.begin())->GetParent()->GetAttribute(wxT("DefaultValue"))->GetValueString());

	return true;

}

bool SP_WDG_StLookupTable::SaveData()
{

	CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

	m_pcLookupTableGrid->SaveEditControlValue();

	SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();
	SP_DS_Attribute* l_pcAttr = *l_Iter;
	wxString l_sMainFunction;

	SP_DS_ColListAttribute* l_pcColList =
			dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	for (int i = 0; i < m_pcLookupTableGrid->GetNumberRows(); i++)
	{
		if (i >= l_pcColList->GetRowCount())
		{
			l_pcColList->AppendEmptyRow();
		}
		l_pcColList->SetCell(i, 0, m_pcLookupTableGrid->GetCellValue(i, 0));
		l_pcColList->SetCell(i, 1, m_pcLookupTableGrid->GetCellValue(i, 1));
	}

	l_pcAttr->GetParent()->GetAttribute(wxT("DefaultValue"))->SetValueString(
			m_pcDefaultValueTextCtrl->GetValue());

	return true;

}

void SP_WDG_StLookupTable::OnEditRow(wxCommandEvent& p_cEvent)
{

	if (m_pcLookupTableGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcLookupTableGrid->GetGridCursorRow();
	int l_nEditColPos = m_pcLookupTableGrid->GetGridCursorCol();

	m_sOldCellValue = m_pcLookupTableGrid->GetCellValue(l_nEditRowPos,
			l_nEditColPos);

	m_pcLookupTableGrid->SelectBlock(l_nEditRowPos, l_nEditColPos,
			l_nEditRowPos, l_nEditColPos);
	m_pcLookupTableGrid->SetFocus();

}

void SP_WDG_StLookupTable::OnNewRow(wxCommandEvent& p_cEvent)
{

	wxString l_s0 = (wxString::Format(wxT("%f"), GetGreatestFunctionArgument() + 1)).c_str();
	wxString l_s1 = wxT("0");

	unsigned int l_nNewRow = m_pcTempLookupTableList->AppendEmptyRow();
	m_pcTempLookupTableList->SetCell(l_nNewRow, 0, l_s0);
	m_pcTempLookupTableList->SetCell(l_nNewRow, 1, l_s1);

	m_pcLookupTableGrid->AppendRows(1);

	m_pcLookupTableGrid->SetCellValue(m_pcLookupTableGrid->GetNumberRows() - 1,
			0, l_s0);

	m_pcLookupTableGrid->SetCellAlignment(m_pcLookupTableGrid->GetNumberRows()
			- 1, 0, wxALIGN_LEFT, wxALIGN_TOP);

	m_pcLookupTableGrid->SetCellBackgroundColour(
			m_pcLookupTableGrid->GetNumberRows() - 1, 0,
			((m_pcLookupTableGrid->GetNumberRows() % 2) ? *wxWHITE
					: *wxLIGHT_GREY));

	m_pcLookupTableGrid->SetCellValue(m_pcLookupTableGrid->GetNumberRows() - 1,
			1, l_s1);

	m_pcLookupTableGrid->SetCellAlignment(m_pcLookupTableGrid->GetNumberRows()
			- 1, 1, wxALIGN_LEFT, wxALIGN_TOP);

	m_pcLookupTableGrid->SetCellBackgroundColour(
			m_pcLookupTableGrid->GetNumberRows() - 1, 1,
			((m_pcLookupTableGrid->GetNumberRows() % 2) ? *wxWHITE
					: *wxLIGHT_GREY));

}

void SP_WDG_StLookupTable::OnDeleteRow(wxCommandEvent& p_cEvent)
{

	if (m_pcLookupTableGrid->GetNumberRows() == 0)
	{

		return;

	}

	int l_nDeleteRowPos = m_pcLookupTableGrid->GetGridCursorRow();

	m_pcTempLookupTableList->RemoveRow(l_nDeleteRowPos);
	LoadData();

}

void SP_WDG_StLookupTable::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_EDITROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnEditRow);

	Connect(
			SP_ID_BUTTON_NEWROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnNewRow);

	Connect(
			SP_ID_BUTTON_DELETEROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnDeleteRow);

}

void SP_WDG_StLookupTable::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_EDITROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnEditRow);

	Disconnect(
			SP_ID_BUTTON_NEWROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnNewRow);

	Disconnect(
			SP_ID_BUTTON_DELETEROW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StLookupTable::OnDeleteRow);

}

void SP_WDG_StLookupTable::OnGridCellSelected(wxGridEvent& ev)
{

	m_sOldCellValue = m_pcLookupTableGrid->GetCellValue(ev.GetRow(),
			ev.GetCol());
	ev.Skip();

}

void SP_WDG_StLookupTable::OnGridCellValueChanged(wxGridEvent& ev)
{

	double l_nDouble = -1;

	wxString l_sValue = m_pcLookupTableGrid->GetCellValue(ev.GetRow(),
			ev.GetCol());

	if (l_sValue.ToDouble(&l_nDouble))
	{

		if (ev.GetCol() == 0)
		{

			if (CheckFunctionProperty(ev.GetRow(), l_nDouble) != true)
			{

				m_pcLookupTableGrid->SetCellValue(ev.GetRow(), ev.GetCol(),
						m_sOldCellValue);
				//        m_pcLookupTableGrid->SetGridCursor( ev.GetRow(), ev.GetCol() );

				SP_MESSAGEBOX(
						wxT("The value already exist as function argument. The inputed value is invalid."),
						wxT("Error"), wxOK| wxICON_ERROR );

			}

		}

	}
	else
	{

		SP_MESSAGEBOX(
				wxT("The value must be a real value. The inputed value is invalid."),
				wxT("Error"), wxOK| wxICON_ERROR );

		m_pcLookupTableGrid->SetCellValue(ev.GetRow(), ev.GetCol(),
				m_sOldCellValue);

	}

	ev.Skip();

}

bool SP_WDG_StLookupTable::CheckFunctionProperty(int p_nRowNumber,
		double p_nToCheck)
{

	if (m_pcLookupTableGrid->GetNumberRows() == 0)
	{

		return 1;

	}

	double l_nDouble = -1;

	for (int i = 0; i < m_pcLookupTableGrid->GetNumberRows(); i++)
	{

		m_pcLookupTableGrid->GetCellValue(i, 0).ToDouble(&l_nDouble);

		if ((p_nRowNumber != i) && (p_nToCheck == l_nDouble))
		{

			return false;

		}

	}

	return true;

}

double SP_WDG_StLookupTable::GetGreatestFunctionArgument()
{

	if (m_pcLookupTableGrid->GetNumberRows() == 0)
	{

		return 0;

	}

	double l_nDouble = 0;
	double l_nGreatest = 0;

	if (m_pcLookupTableGrid->GetNumberRows() == 1)
	{

		return 1;

	}

	m_pcLookupTableGrid->GetCellValue(0, 0).ToDouble(&l_nGreatest);

	for (int i = 1; i < m_pcLookupTableGrid->GetNumberRows(); i++)
	{

		m_pcLookupTableGrid->GetCellValue(i, 0).ToDouble(&l_nDouble);

		if (l_nGreatest < l_nDouble)
		{

			l_nGreatest = l_nDouble;

		}

	}

	return l_nGreatest;

}
