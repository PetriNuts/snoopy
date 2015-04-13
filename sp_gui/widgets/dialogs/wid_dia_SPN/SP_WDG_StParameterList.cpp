//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StParameterList.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"

#include "sp_ds/SP_DS_Node.h"

#include <wx/event.h>

enum
{
	SP_ID_BUTTON_EDIT, SP_ID_BUTTON_OVERVIEW,

	SP_ID_GRID_PARAMETER

};
BEGIN_EVENT_TABLE( SP_WDG_StParameterList, SP_WDG_DialogBase )
#if wxABI_VERSION > 30000
	EVT_GRID_CELL_CHANGED( SP_WDG_StParameterList::OnGridCellValueChanged )
#else
	EVT_GRID_CELL_CHANGE( SP_WDG_StParameterList::OnGridCellValueChanged )
#endif
	EVT_GRID_SELECT_CELL( SP_WDG_StParameterList::OnGridCellSelected )
END_EVENT_TABLE()

SP_WDG_StParameterList::SP_WDG_StParameterList(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering),
	m_pcParameterGrid(NULL),
	m_pcDlg(NULL)
{
}

SP_WDG_StParameterList::~SP_WDG_StParameterList()
{
}

SP_WDG_DialogBase*
SP_WDG_StParameterList::Clone()
{

	return new SP_WDG_StParameterList(GetName(), GetDialogOrdering());

}

bool SP_WDG_StParameterList::AddToDialog(
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

	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("") ), 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcParameterGrid = new wxGrid(l_pcPage, SP_ID_GRID_PARAMETER + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER );

	m_pcParameterGrid->CreateGrid(0, 0);

	m_pcParameterGrid->EnableEditing(true);
	m_pcParameterGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcParameterGrid->SetDefaultColSize(120, TRUE );
	m_pcParameterGrid->SetDefaultRowSize(20, TRUE );

	m_pcParameterGrid->SetColLabelSize(16);
	m_pcParameterGrid->SetRowLabelSize(0);

	m_pcParameterGrid->AppendCols(2);

	m_pcParameterGrid->SetColLabelValue(0, wxT("Parameter set") );
	m_pcParameterGrid->SetColSize(0, 100);

	m_pcParameterGrid->SetColLabelValue(1, wxT("Parameter") );
	m_pcParameterGrid->SetColSize(1, 100);

	LoadData();

	l_pcSizer->Add(m_pcParameterGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_EDIT + m_nDialogID
			+ wxID_HIGHEST, wxT("Edit parameter") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, wxT("Parameter overview") ), 1, wxALL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_StParameterList::OnDlgOk()
{
	m_pcParameterGrid->MoveCursorLeft(false);

	SaveData();

	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_StParameterList::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	bool l_bWhite = false;

	if (m_pcParameterGrid->GetNumberRows() > 0)
	{
		m_pcParameterGrid->DeleteRows(0, m_pcParameterGrid->GetNumberRows());
	}

	m_pcParameterGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcParameterGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0));
		m_pcParameterGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_TOP);
		m_pcParameterGrid->SetReadOnly(i, 0, true);

		m_pcParameterGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		wxString l_sValue = wxT("*");
		if(!m_bMultiple)
		{
			l_sValue = l_pcColList->GetCell(i,1);
		}
		m_pcParameterGrid->SetCellValue(i, 1, l_sValue);
		m_pcParameterGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_TOP);

		m_pcParameterGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);

	}

	return true;

}

bool SP_WDG_StParameterList::SaveData()
{
	CHECK_BOOL( !m_tlAttributes.empty(), return FALSE );

	m_pcParameterGrid->SaveEditControlValue();

	wxString l_sNodeClassName=dynamic_cast<SP_DS_Node*>((*m_tlAttributes.begin())->GetParent())->GetClassName();
	for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin();
			it != m_tlAttributes.end(); it++)
	{
		SP_DS_Attribute* l_pcAttr = *it;
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);

		for (int i = 0; i < m_pcParameterGrid->GetNumberRows(); i++)
		{
			l_pcColList->SetCell(i, 0, m_pcParameterGrid->GetCellValue(i, 0));
			wxString l_sValue = m_pcParameterGrid->GetCellValue(i, 1);
			if((m_bMultiple && l_sValue != wxT("*")) || !m_bMultiple)
			{
				l_pcColList->SetCell(i, 1, l_sValue);
			}
		}
	}

	return true;
}

void SP_WDG_StParameterList::OnEdit(wxCommandEvent& p_cEvent)
{
	if (m_pcParameterGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcParameterGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcParameterGrid->GetCellValue(l_nEditRowPos, 1);

	m_pcParameterGrid->SelectBlock(l_nEditRowPos, 1, l_nEditRowPos, 1);
	m_pcParameterGrid->SetFocus();

}

void SP_WDG_StParameterList::OnOverview(wxCommandEvent& p_cEvent)
{
	wxString l_sNodeClassName=dynamic_cast<SP_DS_Node*>((*m_tlAttributes.begin())->GetParent())->GetClassName();
	SP_DLG_StParameterOverview l_cDlg(m_pcDlg,l_sNodeClassName);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadData();
	}

	l_cDlg.Destroy();
}

void SP_WDG_StParameterList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StParameterList::OnEdit);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StParameterList::OnOverview);

}

void SP_WDG_StParameterList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StParameterList::OnEdit);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_StParameterList::OnOverview);

}

void SP_WDG_StParameterList::OnGridCellSelected(wxGridEvent& ev)
{
	if(m_pcParameterGrid == ev.GetEventObject())
	{
		m_sOldCellValue = m_pcParameterGrid->GetCellValue(ev.GetRow(), ev.GetCol());
	}

	ev.Skip();
}

void SP_WDG_StParameterList::OnGridCellValueChanged(wxGridEvent& ev)
{
	if(m_pcParameterGrid == ev.GetEventObject())
	{
		double l_nDouble = -1;

		wxString l_sValue = m_pcParameterGrid->GetCellValue(ev.GetRow(), ev.GetCol());

		if (!l_sValue.ToDouble(&l_nDouble))
		{
			SP_MESSAGEBOX(
					wxT("The parameter must be a real value. The inputed value is invalid."),
					wxT("Error"), wxOK| wxICON_ERROR );

			m_pcParameterGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
	ev.Skip();

}

