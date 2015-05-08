/*
 * SP_DLG_NewConstantDefinition.cpp
 *
 *  Created on: 07.09.2012
 *      Author: Steffen Laarz
 */

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_GridCellChoiceEditor.h"

#include "sp_core/tools/CyclicGraph.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Attribute.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h>
#include <wx/textdlg.h>

IMPLEMENT_CLASS( SP_DLG_NewConstantDefinition, wxDialog)

enum
{
	SHOW = 0,
	NAME,
	GROUP,
	TYPE,
	COMMENT,
	VALUES
};

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING,
	SP_ID_VALUESET_ADD,
	SP_ID_VALUESET_DELETE,
	SP_ID_NEWGROUP,
	SP_ID_BUTTON_RENAMESET
};
BEGIN_EVENT_TABLE( SP_DLG_NewConstantDefinition, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_NewConstantDefinition::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_NewConstantDefinition::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_NewConstantDefinition::OnAddSet )
EVT_BUTTON( SP_ID_VALUESET_ADD, SP_DLG_NewConstantDefinition::OnAddValueSet )
EVT_BUTTON( SP_ID_VALUESET_DELETE, SP_DLG_NewConstantDefinition::OnDeleteValueSet )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_NewConstantDefinition::OnCheckFunction )
EVT_BUTTON( wxID_APPLY, SP_DLG_NewConstantDefinition::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_NewConstantDefinition::OnDeleteSet )
EVT_BUTTON( SP_ID_NEWGROUP, SP_DLG_NewConstantDefinition::AddNewChoice )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_NewConstantDefinition::OnRenameSet )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_NewConstantDefinition::OnGridCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_NewConstantDefinition::OnGridCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_NewConstantDefinition::OnGridCellSelected )
EVT_GRID_EDITOR_SHOWN( SP_DLG_NewConstantDefinition::OnEditorShown )

END_EVENT_TABLE()

SP_DLG_NewConstantDefinition::SP_DLG_NewConstantDefinition(wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :
		wxDialog(p_pcParent, -1, p_sTitle, wxPoint(120, 120), wxSize(1500, 400), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
		m_nMainSet(VALUES)

{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcGridSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	// new row
	m_pcConstantSetGrid = new wxGrid(this, SP_ID_GRID_MARKING, wxDefaultPosition, wxSize(700, 300), wxSUNKEN_BORDER);
	m_pcConstantSetGrid->CreateGrid(0, 0);
	m_pcConstantSetGrid->EnableEditing(true);

	m_pcConstantSetGrid->SetDefaultColSize(70, TRUE);
	m_pcConstantSetGrid->SetDefaultRowSize(20, TRUE);

	m_pcConstantSetGrid->SetColLabelSize(16);
	m_pcConstantSetGrid->SetRowLabelSize(0); //do not let it show.

	l_pcRowSizer->Add(m_pcConstantSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcConstantSetGrid->EnableEditing(true);
	m_pcConstantSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	l_bWhite = false;
	LoadSetNames();
	LoadData();

	SP_AutoSizeRowLabelSize(m_pcConstantSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* l_pcButtonSizer2 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBox *l_pcBottomButtonBox1 = new wxStaticBox(this, -1, wxT(""));
	wxSizer *l_pcSizer = new wxStaticBoxSizer(l_pcBottomButtonBox1, wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add constant")), 1, wxLEFT | wxRIGHT , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete constant")), 1, wxLEFT | wxRIGHT , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check constant")), 1, wxLEFT | wxRIGHT , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_NEWGROUP, wxT("New Group")), 1, wxLEFT | wxRIGHT , 5);

	//bysl
	wxStaticBox *l_pcBottomButtonBox2 = new wxStaticBox(this, -1, wxT(""));
	wxSizer *l_pcSizer2 = new wxStaticBoxSizer(l_pcBottomButtonBox2, wxHORIZONTAL);

	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_ADD, wxT("New Value Set")), 1, wxLEFT | wxRIGHT , 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_DELETE, wxT("Delete Value Set")), 1, wxLEFT | wxRIGHT , 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_BUTTON_RENAMESET, wxT("Rename Value Set")), 1, wxLEFT | wxRIGHT , 5);

#if wxABI_VERSION < 30000
	wxStaticBox *l_pcBottomButtonBox = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcBottomButtonSizer = new wxStaticBoxSizer( l_pcBottomButtonBox, wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	l_pcBottomButtonSizer->Add(new wxButton(this,wxID_APPLY,wxT("Apply")), 0, wxALL, 5);
#else
    wxStdDialogButtonSizer* l_pcBottomButtonSizer = CreateStdDialogButtonSizer(wxOK|wxCANCEL|wxAPPLY);
#endif

	wxBoxSizer* l_pcSpaceSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSpaceSizer->Add(30, 0, 0);

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer2->Add(l_pcSizer2, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer2->Add(l_pcSpaceSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer2->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_RIGHT);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer2, 0, wxEXPAND | wxALIGN_RIGHT);

	SetAutoLayout(TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit(this);
	m_pcSizer->SetSizeHints(this);

	Layout();

}

void SP_DLG_NewConstantDefinition::OnDlgApply(wxCommandEvent& p_cEvent)
{
	//if there are some errors, we do not apply changes
	if (!CheckInput())
	{
		return;
	}
	SaveData();
}

bool SP_DLG_NewConstantDefinition::CheckInput()
{
	//check user input values for parameters
	for (int l_nRow = 0; l_nRow < m_pcConstantSetGrid->GetNumberRows(); l_nRow++)
	{
		for (int l_nValPos = m_nMainSet; l_nValPos < m_pcConstantSetGrid->GetNumberCols(); l_nValPos++)
		{
			wxString l_sValue = m_pcConstantSetGrid->GetCellValue(l_nRow, l_nValPos);
			if (l_sValue.IsEmpty())
			{
				if (l_nValPos == m_nMainSet)
				{
					wxString l_sMsg = wxT("Value set 'Main'");

					l_sMsg << wxT(" for '") << m_pcConstantSetGrid->GetCellValue(l_nRow, 0);

					l_sMsg << wxT("' must not be empty!");

					SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

					return false;
				}
				else
				{
					continue;
				}
			}

//			SP_DS_ExpEvaluatorDouble e;
//			if (!e.Check(l_sValue, NULL))
			if(!m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_sValue))
			{
				wxString l_sMsg = wxT("Invalid value for '");

				l_sMsg << m_pcConstantSetGrid->GetCellValue(l_nRow, 0);

				l_sMsg << wxT("' in value set ");

				l_sMsg << m_pcConstantSetGrid->GetColLabelValue(l_nValPos);

				SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

				return false; // a problem is found
			}
		}
	}

	return true; //no problems found
}

void SP_DLG_NewConstantDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
{

	//if there are some errors, we do not close the dialog
	if (!CheckInput())
	{
		return;
	}

	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			if (SaveData())
			{
				wxBusyInfo wait(wxT("Please wait, updating marking..."), this);
				//by sl

				/*
				 * todo update for other netclasses to show in declaration tree
				 */

				m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

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

void SP_DLG_NewConstantDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	m_deleted.clear();

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

void SP_DLG_NewConstantDefinition::OnEditSet(wxCommandEvent& p_cEvent)
{
	if (m_pcConstantSetGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcConstantSetGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcConstantSetGrid->GetCellValue(l_nEditRowPos, GROUP);

	m_pcConstantSetGrid->SelectBlock(l_nEditRowPos, GROUP, l_nEditRowPos, GROUP);
	m_pcConstantSetGrid->SetFocus();
}

void SP_DLG_NewConstantDefinition::OnDeleteSet(wxCommandEvent& p_cEvent)
{
	if (m_pcConstantSetGrid->GetNumberRows() == 0)
	{
		return;
	}

	//by default delete selected rows
	wxArrayInt l_anSelectedRows = m_pcConstantSetGrid->GetSelectedRows();

	//if no row is selected, then delete the current editing row
	if (l_anSelectedRows.Count() == 0)
	{
		int l_nEditRowPos = m_pcConstantSetGrid->GetGridCursorRow();

		l_anSelectedRows.Add(l_nEditRowPos);
	}

	for(auto idx : l_anSelectedRows)
	{
		wxString l_sName = m_pcConstantSetGrid->GetCellValue(idx, NAME);
		for (SP_DS_Metadata* l_pcMeta : *(m_pcConstants->GetElements()))
		{
			if(l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString()
				== l_sName)
			{
				m_deleted.push_back(l_pcMeta);
			}
		}
	}
	//delete the selected rows
	m_pcConstantSetGrid->DeleteRows(l_anSelectedRows[0], l_anSelectedRows.Count());

}

void SP_DLG_NewConstantDefinition::OnAddSet(wxCommandEvent& p_cEvent)
{
	m_pcConstantSetGrid->AppendRows(1);

	//InitializeChoice();

	int l_nEditRowPos = m_pcConstantSetGrid->GetNumberRows() - 1;

	auto l_pcBoolEditor = new wxGridCellBoolEditor();
	l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
	m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, SHOW, l_pcBoolEditor);
	m_pcConstantSetGrid->SetCellRenderer(l_nEditRowPos, SHOW, new wxGridCellBoolRenderer());
	m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, SHOW, 1, 1);
	m_pcConstantSetGrid->SetCellAlignment(l_nEditRowPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
	m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, SHOW, wxT("0"));
	m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, SHOW, false);
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, SHOW, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, NAME, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	//m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, GROUP,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
	m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
	m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, GROUP, 1, 1);
	m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, GROUP, wxT("all"));
	m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, GROUP, false);
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, GROUP, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
	m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, TYPE, 1, 1);
	m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, TYPE, wxT("int"));
	m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, TYPE, false);
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, TYPE, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, COMMENT, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	for (int j = m_nMainSet; j < m_pcConstantSetGrid->GetNumberCols(); j++)
	{
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, j, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
	}

	(l_bWhite ? l_bWhite = false : l_bWhite = true);

}

void SP_DLG_NewConstantDefinition::OnAddValueSet(wxCommandEvent& p_cEvent)
{
	wxString l_sName = wxT("V-Set ");
	l_sName.Append(wxString::Format(wxT("%d"), m_pcConstantSetGrid->GetNumberCols() - m_nMainSet - 1));

	m_pcConstantSetGrid->AppendCols(1);
	m_pcConstantSetGrid->SetColSize(1, 70);
	m_pcConstantSetGrid->SetColLabelValue(m_pcConstantSetGrid->GetNumberCols() - 1, l_sName);

	int l_nGridCol = m_pcConstantSetGrid->GetNumberCols() - 1;
	int l_nGridRow = 0;

	//bool l_bWhite = false;
	l_bWhite = false;
	if (0 < l_nGridCol)
	{

		for (l_nGridRow = 0; l_nGridRow < m_pcConstantSetGrid->GetNumberRows(); l_nGridRow++)
		{
			m_pcConstantSetGrid->SetCellValue(l_nGridRow, l_nGridCol, wxT(""));
			m_pcConstantSetGrid->SetCellAlignment(l_nGridRow, l_nGridCol, wxALIGN_CENTRE, wxALIGN_CENTRE);
			m_pcConstantSetGrid->SetCellBackgroundColour(l_nGridRow, l_nGridCol, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
}

void SP_DLG_NewConstantDefinition::OnDeleteValueSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcConstantSetGrid->GetSelectedCols();

	if (!(l_cSelCols.Count() > 0))
	{
		SP_MESSAGEBOX(wxT("Select column(s) to delete."), wxT("Invalid marking choice"), wxOK | wxICON_ERROR);
		return;
	}
	if (l_cSelCols.Count() > 1)
	{
		if (l_cSelCols.Item(0) == 0 && l_cSelCols.Item(1) == 1)
		{
			SP_MESSAGEBOX(wxT("Main marking set cannot be removed."), wxT("Error"), wxOK | wxICON_ERROR);
			if (l_cSelCols.Count() == 2)
			{
				return;
			}
			else
			{
				m_pcConstantSetGrid->DeleteCols(2, l_cSelCols.Count() - 1, false);
				return;
			}
		}
	}

	if (l_cSelCols.Item(0) == 0 || l_cSelCols.Item(0) == 1)
	{
		SP_MESSAGEBOX(wxT("Main marking set cannot be removed."), wxT("Error"), wxOK | wxICON_ERROR);
		if (l_cSelCols.Count() == 1)
		{
			return;
		}
		else
		{
			m_pcConstantSetGrid->DeleteCols(1, l_cSelCols.Count() - 1, false);
			return;
		}
	}
	m_pcConstantSetGrid->DeleteCols(l_cSelCols.Item(0), l_cSelCols.Count(), false);
}

bool SP_DLG_NewConstantDefinition::LoadData()
{
	InitializeChoice();
	InitializeDataTypes();

	unsigned int l_nPos = 0;
	//bool l_bWhite = false;
	l_bWhite = false;
	for (SP_DS_Metadata* l_pcMetadata : *(m_pcConstants->GetElements()))
	{
		wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
		wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
		wxString l_sMetadataComment = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Comment")))->GetValue();
		wxString l_sMetadataShow = l_pcMetadata->GetShow() ? wxT("1") : wxT("0");
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));

		m_pcConstantSetGrid->AppendRows(1);
		//show the constant
		auto l_pcBoolEditor = new wxGridCellBoolEditor();
		l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
		m_pcConstantSetGrid->SetCellEditor(l_nPos, SHOW, l_pcBoolEditor);
		m_pcConstantSetGrid->SetCellRenderer(l_nPos, SHOW, new wxGridCellBoolRenderer());
		m_pcConstantSetGrid->SetCellValue(l_nPos, SHOW, l_sMetadataShow);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, SHOW, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//name of the constant
		m_pcConstantSetGrid->SetCellValue(l_nPos, NAME, l_sMetadataName);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, NAME, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, NAME, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//group of the constant
		m_pcConstantSetGrid->SetCellEditor(l_nPos, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
		m_pcConstantSetGrid->SetCellValue(l_nPos, GROUP, l_sMetadataGroup);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, GROUP, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, GROUP, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//type of the constant
		m_pcConstantSetGrid->SetCellEditor(l_nPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
		m_pcConstantSetGrid->SetCellValue(l_nPos, TYPE, l_sMetadataType);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, TYPE, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, TYPE, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//comment of the constant
		m_pcConstantSetGrid->SetCellValue(l_nPos, COMMENT, l_sMetadataComment);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, COMMENT, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, COMMENT, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//value of the constant
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcConstantSetGrid->SetCellValue(l_nPos, i + m_nMainSet, l_pcColList->GetCell(i, 1));
			m_pcConstantSetGrid->SetCellAlignment(l_nPos, i + m_nMainSet, wxALIGN_CENTER, wxALIGN_TOP);
			m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, i + m_nMainSet, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		}

		l_nPos++;
		(l_bWhite ? l_bWhite = false : l_bWhite = true);

	}

	return true;

}

bool SP_DLG_NewConstantDefinition::SaveData()
{
	m_pcConstantSetGrid->SaveEditControlValue();

	SP_DS_Metadata* l_pcConstant;

	// delete constants
	for(auto l_pcConst : m_deleted)
	{
		m_pcConstants->RemoveElement(l_pcConst);
		wxDELETE(l_pcConst);
	}

	while (m_pcConstants->GetElements()->size() < m_pcConstantSetGrid->GetNumberRows())
	{
		m_pcConstants->NewElement(1);
	}

	SP_ListMetadata::const_iterator l_itElem;
	l_itElem = m_pcConstants->GetElements()->begin();

	for (int l_nRow = 0; l_nRow < m_pcConstantSetGrid->GetNumberRows(); l_nRow++)
	{
		l_pcConstant = *l_itElem;

		wxString l_sName = m_pcConstantSetGrid->GetCellValue(l_nRow, NAME);
		wxString l_sGroup = m_pcConstantSetGrid->GetCellValue(l_nRow, GROUP);
		wxString l_sType = m_pcConstantSetGrid->GetCellValue(l_nRow, TYPE);
		wxString l_sComment = m_pcConstantSetGrid->GetCellValue(l_nRow, COMMENT);

		l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
		l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
		l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

		bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);
		if (!l_bValue)
		{
			SP_MESSAGEBOX(wxT("datatype '") + l_sType + wxT("' for constant '") + l_sName + wxT("' is not allowed (use one of '") + m_sAvailableDatatypes + wxT("')"), wxT("Error"),
					wxOK | wxICON_ERROR);
			return false;
		}

		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));

		l_pcColList->Clear();
		for (int j = m_nMainSet; j < m_pcConstantSetGrid->GetNumberCols(); j++)
		{
			int l_nRowCol = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nRowCol, 0, m_pcConstantSetGrid->GetColLabelValue(j));
			l_pcColList->SetCell(l_nRowCol, 1, m_pcConstantSetGrid->GetCellValue(l_nRow, j));
		}

		SP_FunctionPtr l_pcName = m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_sName);
		SP_FunctionPtr l_pcValue = m_pcGraph->GetFunctionRegistry()->parseFunctionString(m_pcConstantSetGrid->GetCellValue(l_nRow, m_nMainSet));
/*		wxString l_sSig(l_pcSig->toString().c_str(), wxConvUTF8);
		wxString l_sDef(l_pcDef->toString().c_str(), wxConvUTF8);
		SP_LOGMESSAGE( wxT("Signature: ") + l_sSig);
		SP_LOGMESSAGE( wxT("Definition: ") + l_sDef);
*/		m_pcGraph->GetFunctionRegistry()->registerFunction(l_pcName, l_pcValue);

		l_pcColList->UpdateActiveListColumnPtr();

		bool l_bShow = m_pcConstantSetGrid->GetCellValue(l_nRow, SHOW) == wxT("1") ? true : false;
		l_pcConstant->SetShow(l_bShow);
		for(SP_DS_Attribute* l_pcAttr : *l_pcConstant->GetAttributes())
		{
			l_pcAttr->SetShow(l_bShow);
		}
		l_pcConstant->Update();

		++l_itElem;

	}

	m_pcGraph->Update();

	return true;

}

void SP_DLG_NewConstantDefinition::OnCheckFunction(wxCommandEvent& p_cEvent)
{
	//SaveData();

	int col = m_pcConstantSetGrid->GetGridCursorCol();
	int row = m_pcConstantSetGrid->GetGridCursorRow();

	if (col >= m_nMainSet)
	{
		wxString l_sCellValue = m_pcConstantSetGrid->GetCellValue(row, col);
		wxString l_sName = m_pcConstantSetGrid->GetCellValue(row, NAME);
		wxString l_sType = m_pcConstantSetGrid->GetCellValue(row, TYPE);

		DoCheckFunction(l_sName, l_sType, l_sCellValue);
	}
	else
	{
		SP_MESSAGEBOX(wxT("Please select a value of a constant!"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
	}
}

bool SP_DLG_NewConstantDefinition::DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue)
{
	if (p_sName.IsEmpty())
	{
		SP_MESSAGEBOX(wxT("the constant'name is empty"), wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}
	if (p_sValue.IsEmpty())
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" is empty "), wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}
	if (p_sType != wxT("int") && p_sType != wxT("double"))
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" has unknown type ") + p_sType, wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	wxString l_sValue;
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sValue));
	if(!l_pcFunction)
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is not correct"), wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	if(p_sType == wxT("int"))
	{
		long l_Val = SP_DS_FunctionEvaluatorLong{l_pcFR, l_pcFunction, std::numeric_limits<long>::min()}();
		if(l_Val > std::numeric_limits<long>::min())
		{
			l_sValue << l_Val;
			//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
			return true;
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is not correct"), wxT("Check Constant"), wxOK | wxICON_ERROR);
			return false;
		}
	}
	else if(p_sType == wxT("double"))
	{
		double l_Val = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_pcFunction, std::numeric_limits<double>::min()}();
		if(l_Val > std::numeric_limits<double>::min())
		{
			l_sValue << l_Val;
			//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
			return true;
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is not correct"), wxT("Check Constant"), wxOK | wxICON_ERROR);
			return false;
		}
	}
	return false;
}

void SP_DLG_NewConstantDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{
	int col = p_gEvent.GetCol();
	int row = p_gEvent.GetRow();

	wxString colLabel = m_pcConstantSetGrid->GetColLabelValue(col);
	wxString l_sCellValue = m_pcConstantSetGrid->GetCellValue(row, col);

	if (colLabel == wxT("Constant"))
	{

		if (ExistConstant(l_sCellValue, row))
		{
			SP_MESSAGEBOX(wxT("the constant '") + l_sCellValue + wxT("' already exists !"), wxT("Error"), wxOK | wxICON_ERROR);
			//SP_LOG(wxLOG_Error, wxT("constant already exists"));

			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		if (LoadPlaces(l_sCellValue))
		{
			SP_MESSAGEBOX(wxT("the constant '") + l_sCellValue + wxT("' already exists as place!"), wxT("Error"), wxOK | wxICON_ERROR);
			//SP_LOG(wxLOG_Error, wxT("constant name already exists as place"));

			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}
		return;
	}

	if (colLabel == wxT("Type"))
	{
		return;
	}

	if (colLabel == wxT("Group"))
	{
		return;
	}

	if (colLabel == wxT("Comment"))
	{
		return;
	}

	if (colLabel == wxT("Show"))
	{
		return;
	}

	if (col >= m_nMainSet)
	{

		//SaveData();

		if( l_sCellValue.IsEmpty())
		{
			if(col == m_nMainSet)
			{
				wxString l_sMsg = wxT("Value set 'Main'");

				l_sMsg << wxT(" for '") << m_pcConstantSetGrid->GetCellValue(p_gEvent.GetRow(), 0);

				l_sMsg << wxT("' must not be empty!");

				SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

				m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
			}

			return;
		}

		wxString l_sConstName = m_pcConstantSetGrid->GetCellValue(row, NAME);
		if (l_sCellValue.Contains(l_sConstName))
		{
			SP_MESSAGEBOX(wxT("the constant '") + l_sConstName + wxT("' is not allowed in the expression: ") + l_sCellValue, wxT("Error"), wxOK | wxICON_ERROR);
			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		if (FindCycle())
		{
			SP_MESSAGEBOX(wxT("cycle in definition detected"), wxT("Error"), wxOK | wxICON_ERROR);
			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		wxString l_sConstType = m_pcConstantSetGrid->GetCellValue(row, TYPE);

		if(!DoCheckFunction(l_sConstName, l_sConstType, l_sCellValue))
		{
			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
		}
	}
}

/*
 * find cycle in the constants definition
 * return: true iff cycle exists
 */
bool SP_DLG_NewConstantDefinition::FindCycle()
{

	CyclicGraph g(m_pcConstantSetGrid->GetNumberRows());

	//built matrix
	for (int j = 0; j < m_pcConstantSetGrid->GetNumberRows(); j++)
	{
		for (int i = m_nMainSet; i < m_pcConstantSetGrid->GetNumberCols(); i++)
		{
			for (int k = 0; k < m_pcConstantSetGrid->GetNumberRows(); k++)
			{
				wxString l_sCellValue = m_pcConstantSetGrid->GetCellValue(j, i);
				wxString l_sConstValue = m_pcConstantSetGrid->GetCellValue(k, NAME);
				if (l_sCellValue.Contains(l_sConstValue))
				{
					g.addEdge(j, k);
				}
			}
		}
	}
	//search in matrix
	if (g.isCyclic())
		return true;
	else
		return false;
}

bool SP_DLG_NewConstantDefinition::ExistConstant(const wxString& p_sName, const int row)
{

	for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		if (i != row)
		{
			if (p_sName.IsSameAs(m_pcConstantSetGrid->GetCellValue(i, NAME)))
			{
				return true;
			}
		}
	}

	return false;
}

void SP_DLG_NewConstantDefinition::LoadSetNames()
{
	SP_ListMetadata::const_iterator it;
	SP_DS_ColListAttribute * l_pcColListTemp = NULL;

	unsigned int l_nSize = 0;
	for (it = m_pcConstants->GetElements()->begin(); it != m_pcConstants->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* meta = *it;
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(meta->GetAttribute(wxT("ValueList")));
		if (l_pcColList->GetRowCount() > l_nSize)
			l_nSize = l_pcColList->GetRowCount();
		l_pcColListTemp = l_pcColList;
	}

	m_pcConstantSetGrid->AppendCols(6);

	m_pcConstantSetGrid->SetColLabelValue(NAME, wxT("Constant"));
	m_pcConstantSetGrid->SetColSize(NAME, 100);

	m_pcConstantSetGrid->SetColLabelValue(GROUP, wxT("Group"));
	m_pcConstantSetGrid->SetColSize(GROUP, 120);

	m_pcConstantSetGrid->SetColLabelValue(TYPE, wxT("Type"));
	m_pcConstantSetGrid->SetColSize(TYPE, 120);

	m_pcConstantSetGrid->SetColLabelValue(COMMENT, wxT("Comment"));
	m_pcConstantSetGrid->SetColSize(COMMENT, 120);

	m_pcConstantSetGrid->SetColLabelValue(SHOW, wxT("Show"));
	m_pcConstantSetGrid->SetColSize(SHOW, 50);

	m_pcConstantSetGrid->SetColLabelValue(VALUES, wxT("Main"));
	m_pcConstantSetGrid->SetColSize(VALUES, 70);

	for (unsigned int i = 1; i < l_nSize; i++)
	{
		m_pcConstantSetGrid->AppendCols(1);
		m_pcConstantSetGrid->SetColLabelValue(i + m_nMainSet, l_pcColListTemp->GetCell(i, 0));
		m_pcConstantSetGrid->SetColSize(i + 3, 70);
	}
}

void SP_DLG_NewConstantDefinition::InitializeChoice()
{
	SP_ListMetadata::const_iterator it;
	set<wxString>::iterator iter;

	set<wxString> l_lsChoice;

	l_lsChoice.clear();
	l_lsChoice.insert(wxT("all"));
	l_lsChoice.insert(wxT("marking"));
	l_lsChoice.insert(wxT("parameter"));

	//bool flag = false;
	for (it = m_pcConstants->GetElements()->begin(); it != m_pcConstants->GetElements()->end(); ++it)
	{

		SP_DS_Metadata* l_pcMetadata = *it;

		wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();

		//set's elements are unique, therefore we do not need to make a check
		l_lsChoice.insert(l_sGroup);
	}

	m_asGroups.Alloc(l_lsChoice.size());

	for (iter = l_lsChoice.begin(); iter != l_lsChoice.end(); ++iter)
	{
		m_asGroups.Add(*iter);
	}

}

void SP_DLG_NewConstantDefinition::AddNewChoice(wxCommandEvent& p_cEvent)
{
	wxString l_sNewValue = wxGetTextFromUser(wxT("Enter a new Group"), wxT("New Group"), wxT("default"));

	//no group with this name has been found
	if (!FindString(m_asGroups, l_sNewValue))
	{
		//add this group name to the list
		m_asGroups.push_back(l_sNewValue);

		l_bWhite = false;
		for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
		{
			SP_WDG_GridCellChoiceEditor* l_pcCellEditor = dynamic_cast<SP_WDG_GridCellChoiceEditor*>(m_pcConstantSetGrid->GetCellEditor(i, GROUP));

			l_pcCellEditor->AddItem(l_sNewValue);

			m_pcConstantSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
	else
	{
		SP_MESSAGEBOX(wxT("group name '") + l_sNewValue + wxT("' already exists"), wxT("Error"), wxOK | wxICON_ERROR);
		//SP_LOG(wxLOG_Error, wxT("group name already exists"));
	}
}

bool SP_DLG_NewConstantDefinition::FindString(wxArrayString& p_asStrArray, wxString& p_sValue)
{
	for (unsigned long l_nPos = 0; l_nPos < p_asStrArray.GetCount(); l_nPos++)
	{
		if (p_asStrArray[l_nPos] == p_sValue)
		{
			return true; //found
		}
	}

	return false; //not found
}

void SP_DLG_NewConstantDefinition::OnRenameSet(wxCommandEvent& p_cEvent)
{
	wxArrayInt l_pcSelCols = m_pcConstantSetGrid->GetSelectedCols();

	if (l_pcSelCols.Count() != 1)
	{

		SP_MESSAGEBOX(wxT("Select a single column to rename."), wxT("Invalid column choice"), wxOK | wxICON_ERROR);
		return;

	}

	if (l_pcSelCols.Item(0) <= m_nMainSet)
	{

		SP_MESSAGEBOX(wxT("This column cannot be renamed."), wxT("Invalid column choice"), wxOK | wxICON_ERROR);
		return;

	}

	wxTextEntryDialog l_pcDialog(this, wxT("New name of the value set: ") + m_pcConstantSetGrid->GetColLabelValue(l_pcSelCols.Item(0)), wxT("Rename value set"));

	l_pcDialog.SetValue(m_pcConstantSetGrid->GetColLabelValue(l_pcSelCols.Item(0)));

	int l_nModalResult = l_pcDialog.ShowModal();

	if (wxID_OK == l_nModalResult)
	{

		if (l_pcDialog.GetValue().Find(wxT("@")) != -1)
		{

			SP_MESSAGEBOX(wxT("Do not use the symbol @ in the value set name."), wxT("Invalid value set name"), wxOK | wxICON_ERROR);
			return;

		}

		if (ExistSetName(l_pcDialog.GetValue()))
		{

			SP_MESSAGEBOX(wxT("Do not use an already existing value set name."), wxT("Invalid value set name"), wxOK | wxICON_ERROR);

			return;

		}

		m_pcConstantSetGrid->SetColLabelValue(l_pcSelCols.Item(0), l_pcDialog.GetValue());

	}
}

bool SP_DLG_NewConstantDefinition::ExistSetName(const wxString& p_sName)
{
	for (int i = 0; i < m_pcConstantSetGrid->GetNumberCols(); i++)
	{
		if (p_sName == m_pcConstantSetGrid->GetColLabelValue(i))
		{
			return true;
		}
	}
	return false;
}

void SP_DLG_NewConstantDefinition::InitializeDataTypes()
{
	SP_DS_Metadata* l_pcMetadata;

	l_pcMetadata = m_pcConstants->GetPrototype();

	set<wxString> l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetPossibleValues();

	set<wxString>::iterator it;

	m_sAvailableDatatypes = wxT(" ");

	m_datatypes.Alloc(l_sType.size());
	m_datatypes.Empty();

	for (it = l_sType.begin(); it != l_sType.end(); it++)
	{
		m_datatypes.Add(*it);
		m_sAvailableDatatypes << *it;
		m_sAvailableDatatypes << wxT(" ");
	}

	size_t count = m_datatypes.GetCount();
	datatypes = new wxString[count];
	for (size_t n = 0; n < count; n++)
	{

		datatypes[n] = m_datatypes[n];
		wxString s = datatypes[n];
	}

}

void SP_DLG_NewConstantDefinition::OnGridCellSelected(wxGridEvent& ev)
{
	ev.Skip();
}

void SP_DLG_NewConstantDefinition::OnEditorShown(wxGridEvent& ev)
{
	m_sOldCellValue = m_pcConstantSetGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();
}

bool SP_DLG_NewConstantDefinition::LoadPlaces(const wxString& p_sConstant)
{
	//bool l_bool = false;

	//load discrete places
	if (LoadPlaceOfType(SP_DS_DISCRETE_PLACE, p_sConstant))
		return true;
	//load continuous places
	if (LoadPlaceOfType(SP_DS_CONTINUOUS_PLACE, p_sConstant))
		return true;

	return false;
}

bool SP_DLG_NewConstantDefinition::LoadPlaceOfType(const wxString& p_sPlaceType, const wxString& p_sConstant)
{

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(p_sPlaceType);

	if (l_pcNodeclass == NULL)
	{
		return false;
	}

	SP_ListNode::const_iterator l_itElem;
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		if (l_sPlaceName.IsSameAs(p_sConstant))
		{
			return true;
		}
	}

	return false;
}

