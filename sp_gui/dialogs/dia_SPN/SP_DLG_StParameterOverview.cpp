//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

IMPLEMENT_CLASS( SP_DLG_StParameterOverview, wxDialog )

enum
{

	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET

};

BEGIN_EVENT_TABLE( SP_DLG_StParameterOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_StParameterOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_StParameterOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_StParameterOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_StParameterOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_StParameterOverview::OnRenameSet )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_StParameterOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_StParameterOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_StParameterOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_StParameterOverview::SP_DLG_StParameterOverview(wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row


	m_pcParameterGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcParameterGrid->CreateGrid( 0, 0);

	m_pcParameterGrid->EnableEditing( true);
	//  m_pcParameterGrid->SetSelectionMode( wxGrid::wxGridSelectRows );

	m_pcParameterGrid->SetDefaultColSize( 120, TRUE);
	m_pcParameterGrid->SetDefaultRowSize( 20, TRUE);

	m_pcParameterGrid->SetColLabelSize( 16);
	m_pcParameterGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcParameterGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcParameterGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_ADDSET, wxT("Add parameter set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_DELETESET, wxT("Delete parameter set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_RENAMESET, wxT("Rename parameter set") ), 0, wxEXPAND | wxALL, 5);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,
			wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND | wxALIGN_TOP
			| wxALIGN_CENTER);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM
			| wxALIGN_CENTER);

	m_pcSizer->Add(l_pcGridSizer, 1, wxALIGN_CENTER | wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT | wxEXPAND);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_StParameterOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_StParameterOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_StParameterOverview::OnAddSet(wxCommandEvent& p_cEvent)
{

	bool l_bWhite = false;
	wxTextEntryDialog l_pcDialog( this, _T("Name of the new parameter set:"), wxT("Add parameter set"));
	wxString l_sName= wxT("P-Set ");

	l_sName.Append(wxString::Format(wxT("%d"), m_pcParameterGrid->GetNumberCols()
			+ 1) );

	l_pcDialog.SetValue(l_sName);

	int l_nModalResult = l_pcDialog.ShowModal();

	if (wxID_OK == l_nModalResult)
	{

		if (ExistSetName(l_pcDialog.GetValue() ) )
		{

			SP_MESSAGEBOX(wxT("The inputed name is already existing."),
					wxT("Invalid parameter set name"),
					wxOK | wxICON_ERROR);

			return;

		}

		l_sName = l_pcDialog.GetValue();

		m_pcParameterGrid->AppendCols( 1);
		m_pcParameterGrid->SetColLabelValue(m_pcParameterGrid->GetNumberCols()
				- 1, l_sName);

		int l_nGridCol = m_pcParameterGrid->GetNumberCols() - 1;
		int l_nGridRow = 0;

		if ( 0 < l_nGridCol)
		{

			for (l_nGridRow = 0; l_nGridRow
					< m_pcParameterGrid->GetNumberRows(); l_nGridRow++)
			{

				m_pcParameterGrid->SetCellValue(l_nGridRow, l_nGridCol,
						m_pcParameterGrid->GetCellValue(l_nGridRow, l_nGridCol
								- 1) );

				m_pcParameterGrid->SetCellAlignment(l_nGridRow, l_nGridCol,
						wxALIGN_RIGHT, wxALIGN_CENTRE);

				m_pcParameterGrid->SetCellBackgroundColour(l_nGridRow,
						l_nGridCol, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
				(l_bWhite ? l_bWhite = false : l_bWhite = true );

			}

		}

	}

}

void SP_DLG_StParameterOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcParameterGrid->GetSelectedCols();

	if ( ! (l_cSelCols.Count() > 0 ))
	{

		SP_MESSAGEBOX(wxT("Select column(s) to delete."),
				wxT("Invalid parameter parameter choice"), wxOK | wxICON_ERROR);
		return;

	}

	if (l_cSelCols.Item( 0) == 0)
	{

		SP_MESSAGEBOX(wxT("Main parameter set cannot be removed."), wxT("Error"),
				wxOK | wxICON_ERROR);

		if (l_cSelCols.Count() == 1)
		{

			return;

		}
		else
		{

			m_pcParameterGrid->DeleteCols( 1, l_cSelCols.Count() - 1);
			return;

		}

	}

	m_pcParameterGrid->DeleteCols(l_cSelCols.Item( 0), l_cSelCols.Count() );

}

void SP_DLG_StParameterOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{

		wxArrayInt l_pcSelCols = m_pcParameterGrid->GetSelectedCols();

		if (l_pcSelCols.Count() != 1)
		{

			SP_MESSAGEBOX(wxT("Select a single column to rename."),
					wxT("Invalid parameter parameter choice"), wxOK | wxICON_ERROR);
			return;

		}

		if (l_pcSelCols.Item( 0) == 0)
		{

			SP_MESSAGEBOX(wxT("Main parameter set cannot be renamed."),
					wxT("Invalid parameter set choice"), wxOK | wxICON_ERROR);
			return;

		}

		wxTextEntryDialog l_pcDialog( this, wxT("New name of the parameter set: ") +
		m_pcParameterGrid->GetColLabelValue( l_pcSelCols.Item( 0 ) ), wxT("Rename parameter set"));

		l_pcDialog.SetValue(m_pcParameterGrid->GetColLabelValue(l_pcSelCols.Item( 0) ) );

		int l_nModalResult = l_pcDialog.ShowModal();

		if (wxID_OK == l_nModalResult)
		{

			if (l_pcDialog.GetValue().Find(wxT("@")) != -1)
			{

				SP_MESSAGEBOX(
						wxT("Do not use the symbol @ in the parameter set name."),
						wxT("Invalid parameter set name"), wxOK | wxICON_ERROR);
				return;

			}

			if (ExistSetName(l_pcDialog.GetValue() ) )
			{

				SP_MESSAGEBOX(
						wxT("Do not use an already existing parameter set name."),
						wxT("Invalid parameter set name"),
						wxOK | wxICON_ERROR);

				return;

			}

			m_pcParameterGrid->SetColLabelValue(l_pcSelCols.Item( 0),
					l_pcDialog.GetValue() );

		}

	}

}

void SP_DLG_StParameterOverview::LoadData()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= l_pcGraph->GetNodeclass(SP_DS_PARAM);

	SP_ListNode::const_iterator l_itElem;

	bool l_bWhite = false;
	wxString l_sParameterName;
	wxString l_sLoadName = wxT("-1");

	int l_nGridRowNumber = 0;

	LoadSetNames();

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_pcParameterGrid->AppendRows( 1);
		m_pcParameterGrid->SetRowLabelValue(l_nGridRowNumber, l_sParameterName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("ParameterList")) );

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcParameterGrid->SetCellValue(l_nGridRowNumber, i, l_pcColList->GetCell(i, 1));
			m_pcParameterGrid->SetCellBackgroundColour(l_nGridRowNumber, i,
					(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcParameterGrid->SetCellAlignment(l_nGridRowNumber, i,
					wxALIGN_RIGHT, wxALIGN_CENTRE);

		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;

	}

	if ( (m_pcParameterGrid->GetNumberRows() > 0 )
			&& (m_pcParameterGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcParameterGrid->GetCellValue( 0, 0);

	}

}

void SP_DLG_StParameterOverview::SaveData()
{
	m_pcParameterGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= l_pcGraph->GetNodeclass(SP_DS_PARAM);

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("ParameterList")) );
		l_pcColList->Clear();

		for (int i = 0; i < m_pcParameterGrid->GetNumberCols(); i++)
		{
			wxString l_sLabel = m_pcParameterGrid->GetColLabelValue(i);
			wxString l_sValue = m_pcParameterGrid->GetCellValue(l_nGridRowNumber, i);
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);
			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);
		}

		l_nGridRowNumber++;
		(*l_itElem)->Update();
	}
	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_StParameterOverview::OnCellValueChanged(wxGridEvent& ev)
{

	double l_nDouble = -1;

	wxString l_sValue = m_pcParameterGrid->GetCellValue(ev.GetRow(),
			ev.GetCol() );

	if ( ! (l_sValue.ToDouble( &l_nDouble) ))
	{

		SP_MESSAGEBOX(
				wxT("The parameter value must be a real value. The inputed value is invalid."),
				wxT("Error"), wxOK | wxICON_ERROR);

		m_pcParameterGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);

	}

	ev.Skip();

}

void SP_DLG_StParameterOverview::OnGridCellSelected(wxGridEvent& ev)
{

	if ( (m_pcParameterGrid->GetNumberRows() > 0 )
			&& (m_pcParameterGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcParameterGrid->GetCellValue(ev.GetRow(),
				ev.GetCol() );

	}

	ev.Skip();

}

void SP_DLG_StParameterOverview::LoadSetNames()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(l_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements()->size() > 0)
	{
		SP_DS_Node*	l_pcNode = l_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements()->front();
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("ParameterList")));
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
		{
			wxString l_sSetName = l_pcAttr->GetCell(i,0);

			m_pcParameterGrid->AppendCols( 1);
			m_pcParameterGrid->SetColLabelValue(i, l_sSetName);
		}
	}
}

bool SP_DLG_StParameterOverview::ExistSetName(const wxString& p_sName)
{
	for (int i = 0; i < m_pcParameterGrid->GetNumberCols(); i++)
	{
		if (p_sName== m_pcParameterGrid->GetColLabelValue(i) )
		{
			return true;
		}
	}
	return false;
}

void SP_DLG_StParameterOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}
