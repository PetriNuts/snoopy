/*
* SP_DLG_FpnConstantDefinition.cpp
*
*  Created on: 07.02.2019
*      Author: G. Assaf
*/

#include <wx/valgen.h>
#include <wx/tipwin.h>
#include "TriangularFN.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"
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
#include "sp_gui/dialogs/dia_CPN/SP_DLG_SortDialoug.h"

IMPLEMENT_CLASS(SP_DLG_FpnConstantDefinition, wxDialog)

enum
{
	SHOW = 0,
	NAME,
	GROUP,
	TYPE,
	COMMENT,
	
	VALUES //main

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
	SP_ID_BUTTON_RENAMESET,
	SP_ID_SORTSETS,
	SP_ID_MULTICOL_SORT,
	SP_ID_DELETEGROUP
};
BEGIN_EVENT_TABLE(SP_DLG_FpnConstantDefinition, wxDialog)
EVT_CLOSE(SP_DLG_FpnConstantDefinition::OnChildDestroy)
EVT_BUTTON(wxID_OK, SP_DLG_FpnConstantDefinition::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_FpnConstantDefinition::OnDlgCancel)
EVT_BUTTON(SP_ID_BUTTON_ADD, SP_DLG_FpnConstantDefinition::OnAddSet)
EVT_BUTTON(SP_ID_VALUESET_ADD, SP_DLG_FpnConstantDefinition::OnAddValueSet)
EVT_BUTTON(SP_ID_VALUESET_DELETE, SP_DLG_FpnConstantDefinition::OnDeleteValueSet)
EVT_BUTTON(SP_ID_BUTTON_CHECK, SP_DLG_FpnConstantDefinition::OnCheckFunction)
EVT_BUTTON(wxID_APPLY, SP_DLG_FpnConstantDefinition::OnDlgApply)
EVT_BUTTON(SP_ID_BUTTON_DELETE, SP_DLG_FpnConstantDefinition::OnDeleteSet)
EVT_BUTTON(SP_ID_NEWGROUP, SP_DLG_FpnConstantDefinition::AddNewChoice)
EVT_BUTTON(SP_ID_BUTTON_RENAMESET, SP_DLG_FpnConstantDefinition::OnRenameSet)
 
EVT_GRID_CMD_CELL_RIGHT_CLICK(SP_ID_GRID_MARKING, SP_DLG_FpnConstantDefinition::OnRowRightClick)
EVT_BUTTON(SP_ID_SORTSETS, SP_DLG_FpnConstantDefinition::OnSortVsets)
#if wxABI_VERSION > 30000
EVT_GRID_CELL_CHANGED(SP_DLG_FpnConstantDefinition::OnGridCellValueChanged)
EVT_BUTTON(SP_ID_DELETEGROUP, SP_DLG_FpnConstantDefinition::DeleteGroup)
EVT_BUTTON(SP_ID_MULTICOL_SORT, SP_DLG_FpnConstantDefinition::OnMultiColSorting)
#else
EVT_GRID_CELL_CHANGE(SP_DLG_FpnConstantDefinition::OnGridCellValueChanged)
#endif
EVT_GRID_CELL_LEFT_DCLICK(SP_DLG_FpnConstantDefinition::OnGridCellDClicked)
EVT_GRID_SELECT_CELL(SP_DLG_FpnConstantDefinition::OnGridCellSelected)
EVT_GRID_EDITOR_SHOWN(SP_DLG_FpnConstantDefinition::OnEditorShown)

END_EVENT_TABLE()

SP_DLG_FpnConstantDefinition::SP_DLG_FpnConstantDefinition(wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :
	wxDialog(p_pcParent, -1, p_sTitle, wxPoint(120, 120), wxSize(1500, 400), p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
	m_nMainSet(VALUES)

{ 
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	m_pcParent = p_pcParent;
	
	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcGridSizer = new wxBoxSizer(wxVERTICAL);

	// new row
	m_pcConstantSetGrid = new wxGrid(this, SP_ID_GRID_MARKING, wxDefaultPosition, wxSize(300, 300), wxSUNKEN_BORDER);// change size from wxSize(700,300) to wxsize(700,420)
	m_pcConstantSetGrid->CreateGrid(0, 0);
	m_pcConstantSetGrid->EnableEditing(true);
	
	m_pcConstantSetGrid->SetDefaultColSize(70, TRUE);
	m_pcConstantSetGrid->SetDefaultRowSize(20, TRUE);

	m_pcConstantSetGrid->SetColLabelSize(16);
	m_pcConstantSetGrid->SetRowLabelSize(0); //do not let it show.

	l_pcRowSizer->Add(m_pcConstantSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	

	l_bWhite = false;
	LoadSetNames();
	LoadData();
	LoadPlaces();
	
	SP_AutoSizeRowLabelSize(m_pcConstantSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* l_pcButtonSizer2 = new wxBoxSizer(wxHORIZONTAL);

	wxSizer *l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcSortingCol = new wxButton(this, SP_ID_MULTICOL_SORT, wxT("Multi column sorting"));
	m_pcSortingButton = new wxButton(this, SP_ID_SORTSETS, wxT("Sort Value Sets (Asc)"));
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add constant")), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete constant")), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check constant")), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_NEWGROUP, wxT("New Group")), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_DELETEGROUP, wxT("Delete Group")), 1, wxALL, 5);
	l_pcSizer->Add(m_pcSortingButton, 1, wxALL, 5);
	wxSizer *l_pcSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer2->Add(m_pcSortingCol, 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_ADD, wxT("New Value Set")), 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_DELETE, wxT("Delete Value Set")), 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_BUTTON_RENAMESET, wxT("Rename Value Set")), 1, wxALL, 5);

	wxSizer* l_pcBottomButtonSizer = CreateButtonSizer(wxOK | wxCANCEL | wxAPPLY);

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND);
	l_pcButtonSizer2->Add(l_pcSizer2, 0, wxEXPAND);
	l_pcButtonSizer2->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer2, 0, wxEXPAND);

	SetAutoLayout(TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit(this);
	m_pcSizer->SetSizeHints(this);
	wxString l_sNetClass=m_pcGraph->GetNetclass()->GetName();
	if (l_sNetClass.Contains(wxT("Colored")))//do not display SHOW Column
	{
		m_pcConstantSetGrid->HideCol(0);
	}
	
	m_bIsAsc = true;
	Layout();

}

void SP_DLG_FpnConstantDefinition::OnDlgApply(wxCommandEvent& p_cEvent)
{
	//if there are some errors, we do not apply changes

	if (!CheckInput())
	{
		return;
	}
	SaveData();
}

bool SP_DLG_FpnConstantDefinition::CheckInput()
{
	//check user input values for parameters
	for (int l_nRow = 0; l_nRow < m_pcConstantSetGrid->GetNumberRows(); l_nRow++)
	{
		for (int l_nValPos = m_nMainSet; l_nValPos < m_pcConstantSetGrid->GetNumberCols(); l_nValPos++)
		{
			wxString l_sValue = m_pcConstantSetGrid->GetCellValue(l_nRow, l_nValPos);
			if (l_sValue.IsEmpty())
			{
				wxString type1 = m_pcConstantSetGrid->GetCellValue(l_nRow, TYPE);
				if (l_nValPos == m_nMainSet && (type1==wxString("int")  ||type1== wxString("double")  || type1 == wxString("TFN")))
				{
					wxString l_sMsg = wxT("Value set 'Main'");

					l_sMsg << wxT(" for '") << m_pcConstantSetGrid->GetCellValue(l_nRow, 0);

					l_sMsg << wxT("' must not be empty!");

					SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

					return false;
				}
				else {
					continue;
				}
				
			}

			//			SP_DS_ExpEvaluatorDouble e;
			//			if (!e.Check(l_sValue, NULL))
			wxString type = m_pcConstantSetGrid->GetCellValue(l_nRow, TYPE);
		
			if (type == wxString("int") || type == wxString("double")) 
			{
				if (!m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_sValue))
				{
					wxString l_sMsg = wxT("Invalid value for '");

					l_sMsg << m_pcConstantSetGrid->GetCellValue(l_nRow, 0);

				l_sMsg << wxT("' in value set ");

				l_sMsg<< m_pcConstantSetGrid->GetColLabelValue(l_nValPos);

					SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

					return false; // a problem is found
				}
			}

			
			else {
				SP_VectorDouble l_dconstants = GetFNConstants(l_sValue);
				wxString a; a << l_dconstants[0];
				wxString b; b << l_dconstants[1];
				wxString c; c << l_dconstants[2];
				wxString d; 
				if(l_dconstants.size()==4)
				d << l_dconstants[3];
				return CheckFN(type, a, b, c, d);
			}
		}
	}

	return true; //no problems found
}

void SP_DLG_FpnConstantDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
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
				* todo update for other netclasses to display in declaration tree
				*/
				wxString l_sName = m_pcGraph->GetNetclass()->GetName();
				if (l_sName == SP_DS_FUZZY_ColCPN_CLASS|| l_sName == SP_DS_FUZZY_ColSPN_CLASS|| l_sName == SP_DS_FUZZY_ColHPN_CLASS)//by george
				{
					m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

					SP_CPN_SyntaxChecking l_cSyntaxChecking;
					if (l_cSyntaxChecking.Initialize())
					{
						l_cSyntaxChecking.UpdateNetMarking();
					}
					SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					EndModal(wxID_OK);
				}
				else {


					m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

					SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					EndModal(wxID_OK);
				}
			 
			}
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_FpnConstantDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_FpnConstantDefinition::OnEditSet(wxCommandEvent& p_cEvent)
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

void SP_DLG_FpnConstantDefinition::OnDeleteSet(wxCommandEvent& p_cEvent)
{
	if (m_pcConstantSetGrid->GetNumberRows() == 0)
	{
		return;
	}
	if (m_pcConstantSetGrid->IsSelection())
	{
		wxGridUpdateLocker locker(m_pcConstantSetGrid);
		for (int n = 0; n < m_pcConstantSetGrid->GetNumberRows(); )
		{
			if (m_pcConstantSetGrid->IsInSelection(n, 0))
			{
				wxString l_sName = m_pcConstantSetGrid->GetCellValue(n, NAME);
				for (SP_DS_Metadata* l_pcMeta : *(m_pcConstants->GetElements()))
				{
					if (l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString()
						== l_sName)
					{
						m_deleted.push_back(l_pcMeta);
					}
				}
				m_pcConstantSetGrid->DeleteRows(n, 1);
			}
			else
			{
				++n;
			}
		}
	}
}

void SP_DLG_FpnConstantDefinition::OnAddSet(wxCommandEvent& p_cEvent)// on add a new constant 
{
	m_pcConstantSetGrid->AppendRows(1);

	//InitializeChoice();

	int l_nEditRowPos = m_pcConstantSetGrid->GetNumberRows() - 1;

	if (!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))//do not add SHOW Column
	{
		auto l_pcBoolEditor = new wxGridCellBoolEditor();
		l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
		m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, SHOW, l_pcBoolEditor);
		m_pcConstantSetGrid->SetCellRenderer(l_nEditRowPos, SHOW, new wxGridCellBoolRenderer());
		m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, SHOW, 1, 1);
		m_pcConstantSetGrid->SetCellAlignment(l_nEditRowPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, SHOW, wxT("0"));
		m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, SHOW, false);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, SHOW, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	}
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, NAME, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	//m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, GROUP,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
	m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
	m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, GROUP, 1, 1);
	m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, GROUP, wxT("all"));
	m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, GROUP, false);
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, GROUP, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcConstantSetGrid->SetCellEditor(l_nEditRowPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
	m_pcConstantSetGrid->SetCellSize(l_nEditRowPos, TYPE, 1, 1);
	m_pcConstantSetGrid->SetCellValue(l_nEditRowPos, TYPE, wxT("int"));
	m_pcConstantSetGrid->SetCellOverflow(l_nEditRowPos, TYPE, false);
	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, TYPE, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, COMMENT, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	//m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, CONSTA,   *wxWHITE  );
	//m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, CONSTB, *wxWHITE);
	//m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, CONSTC, *wxWHITE);
	//m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, CONSTD, *wxWHITE);
	//m_pcConstantSetGrid->SetReadOnly(l_nEditRowPos, CONSTA);
	//m_pcConstantSetGrid->SetReadOnly(l_nEditRowPos, CONSTB);
	//m_pcConstantSetGrid->SetReadOnly(l_nEditRowPos, CONSTC);
	//m_pcConstantSetGrid->SetReadOnly(l_nEditRowPos, CONSTD);
	for (int j = m_nMainSet; j < m_pcConstantSetGrid->GetNumberCols(); j++)
	{
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nEditRowPos, j, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	}

	(l_bWhite ? l_bWhite = false : l_bWhite = true);

}

void SP_DLG_FpnConstantDefinition::OnAddValueSet(wxCommandEvent& p_cEvent)
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
			m_pcConstantSetGrid->SetCellAlignment(l_nGridRow, l_nGridCol, wxALIGN_CENTER, wxALIGN_CENTER);
			m_pcConstantSetGrid->SetCellBackgroundColour(l_nGridRow, l_nGridCol, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
}

void SP_DLG_FpnConstantDefinition::OnDeleteValueSet(wxCommandEvent& p_cEvent)
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

bool SP_DLG_FpnConstantDefinition::LoadData()
{
	InitializeChoice();
	InitializeDataTypes();
	//////////////
	//FuzzyBasics fn1;

  //tn.getMembershipValue(4);
	/*
	fn1.getAlphaCutSet(0.2, 5);
	*/
	/*
	fn1->setType(Triangular);
	SP_VectorDouble set = fn1->getAlphaCutSet(0.2, 5);
	int si = sizeof(set);
	*/
	///////////
	
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
		wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();
		if (!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))//do not add SHOW Column
		{//show the constant
			auto l_pcBoolEditor = new wxGridCellBoolEditor();
			l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
			m_pcConstantSetGrid->SetCellEditor(l_nPos, SHOW, l_pcBoolEditor);
			m_pcConstantSetGrid->SetCellRenderer(l_nPos, SHOW, new wxGridCellBoolRenderer());
			m_pcConstantSetGrid->SetCellValue(l_nPos, SHOW, l_sMetadataShow);
			m_pcConstantSetGrid->SetCellAlignment(l_nPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
			m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, SHOW, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		}
		//name of the constant
		m_pcConstantSetGrid->SetCellValue(l_nPos, NAME, l_sMetadataName);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, NAME, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, NAME, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//group of the constant
		m_pcConstantSetGrid->SetCellEditor(l_nPos, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
		m_pcConstantSetGrid->SetCellValue(l_nPos, GROUP, l_sMetadataGroup);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, GROUP, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, GROUP, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//type of the constant
		m_pcConstantSetGrid->SetCellEditor(l_nPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
		m_pcConstantSetGrid->SetCellValue(l_nPos, TYPE, l_sMetadataType);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, TYPE, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, TYPE, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//comment of the constant
		m_pcConstantSetGrid->SetCellValue(l_nPos, COMMENT, l_sMetadataComment);
		m_pcConstantSetGrid->SetCellAlignment(l_nPos, COMMENT, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, COMMENT, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//value of the constant
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
	
				m_pcConstantSetGrid->SetCellValue(l_nPos, i + m_nMainSet, l_pcColList->GetCell(i, 1));
				m_pcConstantSetGrid->SetCellAlignment(l_nPos, i + m_nMainSet, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcConstantSetGrid->SetCellBackgroundColour(l_nPos, i + m_nMainSet, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			 
		}

		l_nPos++;
		(l_bWhite ? l_bWhite = false : l_bWhite = true);

	}

	return true;

}

bool SP_DLG_FpnConstantDefinition::SaveData()
{
	m_pcConstantSetGrid->SaveEditControlValue();

	SP_DS_Metadata* l_pcConstant;

	// delete constants
	for (auto l_pcConst : m_deleted)
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

		//fuzzy number constants----------------------------------------Added by G.A
	//	wxString l_sConstA = m_pcConstantSetGrid->GetCellValue(l_nRow, CONSTA);
	//	wxString l_sConstB = m_pcConstantSetGrid->GetCellValue(l_nRow, CONSTB);
	//	wxString l_sConstC = m_pcConstantSetGrid->GetCellValue(l_nRow, CONSTC);
	//	wxString l_sConstD = m_pcConstantSetGrid->GetCellValue(l_nRow, CONSTD);
		//-----------------------------------------------------------------


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
			if (l_sType.Cmp(wxT("TFN")) != 0 || l_sType.Cmp(wxT("BFN")) != 0 || l_sType.Cmp(wxT("TZN")) != 0)
			{
				l_pcColList->SetCell(l_nRowCol, 1, m_pcConstantSetGrid->GetCellValue(l_nRow, j));

			}
			else
			{
				SP_VectorDouble l_constants = GetFNConstants(m_pcConstantSetGrid->GetCellValue(l_nRow, j));
				if (l_constants.size() == 3)
				{
					wxString ca; ca << l_constants[0];
					wxString cb; cb << l_constants[1];
					wxString cc; cc << l_constants[2];
					l_pcColList->SetCell(l_nRowCol, 2,ca);// Added by G.A
					l_pcColList->SetCell(l_nRowCol, 3,cb);// Added by G.A
					l_pcColList->SetCell(l_nRowCol, 4,cc);// Added by G.A
				}
				else if (l_constants.size() == 4)
				{
					wxString ca; ca << l_constants[0];
					wxString cb; cb << l_constants[1];
					wxString cc; cc << l_constants[2];
					wxString cd; cd << l_constants[2];
					l_pcColList->SetCell(l_nRowCol, 2, ca);// Added by G.A
					l_pcColList->SetCell(l_nRowCol, 3, cb);// Added by G.A
					l_pcColList->SetCell(l_nRowCol, 4, cc);// Added by G.A
					l_pcColList->SetCell(l_nRowCol, 5, cd);// Added by G.A
				}
			}

 

		}
		if (l_sType.Cmp(wxT("TFN"))==0 ||l_sType.Cmp(wxT("BFN")) == 0)
		{

		}
		else if (l_sType.Cmp(wxT("TZN")) == 0 )
		{ 
		}
		else {
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, m_pcConstantSetGrid->GetCellValue(l_nRow, m_nMainSet));
		}
		 l_pcColList->UpdateActiveListColumnPtr(); 

		bool l_bShow = m_pcConstantSetGrid->GetCellValue(l_nRow, SHOW) == wxT("1") ? true : false;
		l_pcConstant->SetShow(l_bShow);
		for (SP_DS_Attribute* l_pcAttr : *l_pcConstant->GetAttributes())
		{
			l_pcAttr->SetShow(l_bShow);
		}
		l_pcConstant->Update();

		++l_itElem;

	}

	m_pcGraph->Update();

	return true;

}

void SP_DLG_FpnConstantDefinition::OnCheckFunction(wxCommandEvent& p_cEvent)
{
	//SaveData();

	int col = m_pcConstantSetGrid->GetGridCursorCol();
	int row = m_pcConstantSetGrid->GetGridCursorRow();
	wxString l_sType1 = m_pcConstantSetGrid->GetCellValue(row, TYPE);//Added by G.A

	if (col >= m_nMainSet && l_sType1.compare("TFN") != 0 && l_sType1.compare("BFN") != 0 && l_sType1.compare("TZN") != 0)
	{
		wxString l_sCellValue = m_pcConstantSetGrid->GetCellValue(row, col);
		wxString l_sName = m_pcConstantSetGrid->GetCellValue(row, NAME);
		wxString l_sType = m_pcConstantSetGrid->GetCellValue(row, TYPE);

		DoCheckFunction1(l_sName, l_sType, l_sCellValue);
	}
	else if (l_sType1.compare("TFN") == 0 || l_sType1.compare("BFN") == 0 || l_sType1.compare("TZN") == 0)
	{
	}
	else
	{
		SP_MESSAGEBOX(wxT("Please select the value of a constant!"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
	}
}


bool SP_DLG_FpnConstantDefinition::SubstituteConstant(const wxString& p_sbefore, wxString& p_sValue)
{

	bool l_bSubstituted = false;
	wxString l_sRes;
	l_sRes = p_sbefore;
	wxStringTokenizer tokenizer(p_sbefore, " +*-/()");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		auto itFound = m_mAcceptedConst2Val.find(token);
		if (itFound != m_mAcceptedConst2Val.end())
		{
			l_sRes.Replace(token, itFound->second);
			l_bSubstituted = true;
		}
		else
		{
			double l_dval;

			if (!token.ToDouble(&l_dval))
			{
				return false;
			}

		}

	}
	p_sValue = l_sRes;
	return l_bSubstituted;

}

bool SP_DLG_FpnConstantDefinition::DoCheckFunction1(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue)
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
	if (m_datatypes.Index(p_sType) == wxNOT_FOUND)
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" has unknown type ") + p_sType, wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	if(p_sType==wxT("TFN"))
	{

		if (ParseFN(p_sType, p_sValue))
		{
			wxString x = p_sValue;
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + x + wxT(" is correct"), 1000);
			return true;
		}
		else return false;
	}
	bool l_bIsDefinedVar = false;
	bool l_bOk = false;
	wxString l_sValue;
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sValue));
	if (l_pcFunction)
	{
		SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

		std::set<std::string> l_Vars;
		l_pcExpanded->getVariables(l_Vars);

		for (auto itSet = l_Vars.begin(); itSet != l_Vars.end(); ++itSet)
		{
			std::string  l_sVarStr = *itSet;
			auto Isfound = m_mAcceptedConst2Val.find(l_sVarStr);
			if (Isfound != m_mAcceptedConst2Val.end())
			{
				l_bIsDefinedVar = true;

				break;
			}

		}
		if (l_Vars.empty() || l_bIsDefinedVar)
		{
			l_bOk = true;
		}
		else
		{
			wxString l_sMsg = wxT("\nThe following identifiers are not valid:");
			for (auto const & s : l_Vars)
			{
				l_sMsg << " " << s;
			}
			SP_LOGERROR(l_sMsg);
		}
	}
	if (p_sType == wxT("int") && p_sValue.Contains(wxT(".")))
	{
		l_bOk = false;
	}
	if (!l_bOk)
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is not correct"), wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	if (p_sType == wxT("int"))
	{
		if (l_bIsDefinedVar)
		{
			wxString l_sSubstituted;
			if (SubstituteConstant(p_sValue, l_sSubstituted))
			{
				SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sSubstituted));
				long l_Val1 = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_pcFunction, std::numeric_limits<long>::min() }();
				l_sValue << l_Val1;

				new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is correct"), 1000);
				m_mAcceptedConst2Val[p_sName] = l_sValue;
				return true;
			}
			else
			{
				SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
				return false;
			}

		}
		long l_Val = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_pcFunction, std::numeric_limits<long>::min() }();
		if (l_Val >= 0)
		{
			l_sValue << l_Val;
			//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
			//m_vAlreadyAcceptedConstants.insert(p_sName);
			m_mAcceptedConst2Val[p_sName] = l_sValue;
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
		}
		return true;
	}
	else if (p_sType == wxT("double"))
	{
		if (l_bIsDefinedVar)
		{
			wxString l_sSubstituted;
			if (SubstituteConstant(p_sValue, l_sSubstituted))
			{
				SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sSubstituted));
				double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }();
				if (l_Val >= 0.0)
				{
					l_sValue << l_Val;
					//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
					new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is correct"), 1000);
					m_mAcceptedConst2Val[p_sName] = l_sValue;
					return true;
				}
				else
				{
					SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
					return false;
				}
			}
			else
			{


				SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
				return false;

			}

		}
		double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }();
		if (l_Val >= 0.0)
		{
			l_sValue << l_Val;
			//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
			m_mAcceptedConst2Val[p_sName] = l_sValue;
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
		}
		return true;
	}
	return false;
}

 
bool   SP_DLG_FpnConstantDefinition::ParseFN(const wxString& type, const wxString &val)
{
	std::string s = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		numConst++;
		token = s.substr(0, pos);
		//std::cout << token << std::endl;
		double value;
		if (!token.ToDouble(&value)) { return false; }
		s.erase(0, pos + delimiter.length());
	
	}
	wxString ss = s;
	double d;
	
	if (!ss.ToDouble(&d)) { return false; }
	else{ numConst++; }
	if (type == wxT("TFN") && numConst != 3) { isOk = false;  SP_MESSAGEBOX(wxT("TFN Number consists of 3 values")); }
	if (type == wxT("BFN") && numConst != 3) { isOk = false; SP_MESSAGEBOX(wxT("BFN Number consists of 3 values"));}

	if (type == wxT("TZN") && numConst != 4) { isOk = false; SP_MESSAGEBOX(wxT("TZN Number consists of 4 values"));
	}
	return isOk;
}
SP_VectorDouble   SP_DLG_FpnConstantDefinition::GetFNConstants( const wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string s = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		numConst++;
		token = s.substr(0, pos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		s.erase(0, pos + delimiter.length());

	}
	wxString ss = s;
	double d;

	if (ss.ToDouble(&d)) { v_dConstants.push_back(d); }
	
	
	return v_dConstants;
}
bool SP_DLG_FpnConstantDefinition::CheckFN(const wxString& type, wxString &consta, wxString& constb, wxString& constc, wxString& constd)
{
	if (type == wxT("TFN"))
	{
		double a;
		bool aa = consta.ToDouble(&a);
		double b;
		bool bb = constb.ToDouble(&b);
		double c;
		bool cc = constc.ToDouble(&c);
		if (aa && bb && cc)
		{
			if (a < b && b < c)
			{
				return true;
			}
			else 
				SP_MESSAGEBOX(wxT("the constants of the Triangular FN should respect: a<b<c"), wxT("Check Constant"), wxOK | wxICON_WARNING);
			return false;
		}
		else {
			SP_MESSAGEBOX(wxT("you must enter all valuse of CONSTA,CONSTB and CONSTC"), wxT("Check Constant"), wxOK | wxICON_WARNING);
			return false;
		}
	}
	else if (type == wxT("TZN"))
	{
		double a;
		bool aa = consta.ToDouble(&a);
		double b;
		bool bb = constb.ToDouble(&b);
		double c;
		bool cc = constc.ToDouble(&c);
		double d;
		bool dd = constd.ToDouble(&d);
		if (aa&& bb&& cc && dd)
		{
			if (a < b && b <= c && c < d)
				return true;
			else {
				SP_MESSAGEBOX(wxT("the constants of the Trapezoidal FN should respect: a<b<=c<d"), wxT("Check Constant"), wxOK | wxICON_WARNING);

				return false;
			}

		}
		else {
			SP_MESSAGEBOX(wxT("you must enter all values of CONSTA,CONSTB,CONSTC and CONSTD"), wxT("Check Constant"), wxOK | wxICON_WARNING);
			return false;
		}

	}
	else
	{// BFN
		double a;
		bool aa = consta.ToDouble(&a);
		double b;
		bool bb = constb.ToDouble(&b);
		double c;
		bool cc = constc.ToDouble(&c);
		if (aa&&bb&&cc)
		{
			if (b > 0)
			{
				double x = c + a;
				double y = c - a;
				if (x - y == 2 * a) return true;
				else
				{
					SP_MESSAGEBOX(wxT("re-enter vlaues of const C and  Const A"), wxT("Check Constant"), wxOK | wxICON_WARNING);

					return false;
				}

			}
			else {
				SP_MESSAGEBOX(wxT("Const B must be > 0"), wxT("Check Constant"), wxOK | wxICON_WARNING);
				return false;
			}

		}
	}


}
void SP_DLG_FpnConstantDefinition::OnGridCellDClicked(wxGridEvent& ev)
{
	m_nRow = ev.GetRow();
	m_nCol = ev.GetCol();

	wxString colType = m_pcConstantSetGrid->GetCellValue(m_nRow,3);
	if (colType == wxT("TFN")&& m_nCol>4)
	{
		wxString l_sT = "TFN";
		wxString l_sCurrentVal= m_pcConstantSetGrid->GetCellValue(m_nRow, m_nCol);
		m_pcDraw =new SP_DLG_FuzzyNumber_Drawing(this, l_sT, l_sCurrentVal, 20, 30, 100, 200);
		//m_pcDraw->Show();
		if (m_pcDraw->ShowModal() == wxID_OK)
		{
			wxString l_sValA = wxString::Format(wxT("%f"), m_pcDraw->GetAVal());
			wxString l_sValB = wxString::Format(wxT("%f"), m_pcDraw->GetBVal());
			wxString l_sValC = wxString::Format(wxT("%f"), m_pcDraw->GetCVal());

			if (!(m_pcDraw->GetAVal() < m_pcDraw->GetBVal()&& m_pcDraw->GetBVal() <= m_pcDraw->GetCVal()))
			{
				wxString l_sCosnt = m_pcConstantSetGrid->GetCellValue(m_nRow, NAME);
				wxString l_sMsg = wxT("The values of the TFN ") + l_sCosnt + wxT(" are not valid");
				SP_LOGMESSAGE(l_sMsg);
				return;
			}
			l_sTFNFromDrawingPanel << l_sValA << wxT(",") << l_sValB << wxT(",") << l_sValC;
			m_pcConstantSetGrid->SetCellValue(m_nRow, m_nCol, l_sTFNFromDrawingPanel);
			l_sTFNFromDrawingPanel.Empty();

		}
	}
	ev.Skip();
}
void SP_DLG_FpnConstantDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
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

		if (m_Places.find(l_sCellValue) != m_Places.end())
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
		if (l_sCellValue == wxT("TFN")  || l_sCellValue == wxT("BFN"))
		{
			//SP_MESSAGEBOX("Bell", wxT("Error"), wxOK | wxICON_ERROR);
			//m_pcConstantSetGrid->SetReadOnly(row,8,true);
		//	m_pcConstantSetGrid->SetReadOnly(row, 9, true);
		//	m_pcConstantSetGrid->SetCellBackgroundColour(row, 8, *wxLIGHT_GREY);

		//	m_pcConstantSetGrid->SetCellBackgroundColour(row, 9, *wxLIGHT_GREY);
			///////////////////
		//	m_pcConstantSetGrid->SetReadOnly(row, 5, false);
		//	m_pcConstantSetGrid->SetReadOnly(row, 6, false);
		//	m_pcConstantSetGrid->SetReadOnly(row, 7, false);
		//	m_pcConstantSetGrid->SetReadOnly(row, 8, true);
			//m_pcConstantSetGrid->SetCellBackgroundColour(row, 5, *wxWHITE);

		//	m_pcConstantSetGrid->SetCellBackgroundColour(row, 6, *wxWHITE);

		//	m_pcConstantSetGrid->SetCellBackgroundColour(row, 7, *wxWHITE);
		//	m_pcConstantSetGrid->SetCellBackgroundColour(row, 8, *wxLIGHT_GREY);

			///////////
			wxString l_sT = "TFN";
		  //  m_pcDraw =new SP_DLG_FuzzyNumber_Drawing(this, l_sT, 20, 30, 100, 200);
			//m_nRow = row;
			//m_nCol += 2;

			//m_pcDraw->Show();
			return;
		}

		else if (l_sCellValue == wxT("int") || l_sCellValue == wxT("double"))
		{
			//SP_MESSAGEBOX("Bell", wxT("Error"), wxOK | wxICON_ERROR);
			/*
			m_pcConstantSetGrid->SetReadOnly(row, 9, false);
			m_pcConstantSetGrid->SetCellBackgroundColour(row, 9, *wxWHITE);
			m_pcConstantSetGrid->SetReadOnly(row, 5, true);
			m_pcConstantSetGrid->SetReadOnly(row, 6, true);
			m_pcConstantSetGrid->SetReadOnly(row, 7, true); 
			m_pcConstantSetGrid->SetReadOnly(row, 8, true);
			m_pcConstantSetGrid->SetCellBackgroundColour(row, 5, *wxLIGHT_GREY);

			m_pcConstantSetGrid->SetCellBackgroundColour(row, 6, *wxLIGHT_GREY);

			m_pcConstantSetGrid->SetCellBackgroundColour(row, 7, *wxLIGHT_GREY);
			m_pcConstantSetGrid->SetCellBackgroundColour(row, 8, *wxLIGHT_GREY);

			*/

			return;
		}
		else { // TZN type
			/*
			m_pcConstantSetGrid->SetReadOnly(row, 9, true);
			m_pcConstantSetGrid->SetCellBackgroundColour(row, 8, *wxWHITE);

			m_pcConstantSetGrid->SetCellBackgroundColour(row, 9, *wxLIGHT_GREY);
			///////////////////
			m_pcConstantSetGrid->SetReadOnly(row, 5, false);
			m_pcConstantSetGrid->SetReadOnly(row, 6, false);
			m_pcConstantSetGrid->SetReadOnly(row, 7, false);
			m_pcConstantSetGrid->SetReadOnly(row, 8, false);
			m_pcConstantSetGrid->SetCellBackgroundColour(row, 5, *wxWHITE);

			m_pcConstantSetGrid->SetCellBackgroundColour(row, 6, *wxWHITE);

			m_pcConstantSetGrid->SetCellBackgroundColour(row, 7, *wxWHITE);
			//m_pcConstantSetGrid->SetCellBackgroundColour(row, 8, *wxLIGHT_GREY);
			*/
			return;
			}
	}
	
	 
	

	///////
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

		if (l_sCellValue.IsEmpty())
		{
			if (col == m_nMainSet)
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
		double d;
		if (!l_sCellValue.ToDouble(&d))
		{
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
		}

		wxString l_sConstType = m_pcConstantSetGrid->GetCellValue(row, TYPE);

		if (!DoCheckFunction1(l_sConstName, l_sConstType, l_sCellValue))
		{
			m_pcConstantSetGrid->SetCellValue(row, col, m_sOldCellValue);
		}
	}
}

/*
* find cycle in the constants definition
* return: true iff cycle exists
*/
bool SP_DLG_FpnConstantDefinition::FindCycle()
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

bool SP_DLG_FpnConstantDefinition::ExistConstant(const wxString& p_sName, const int row)
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

void SP_DLG_FpnConstantDefinition::LoadSetNames()
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
	//m_pcConstantSetGrid->SetUseNativeColLabels();
	//m_pcConstantSetGrid->UseNativeColHeader();//

	m_pcConstantSetGrid->SetColLabelValue(NAME, wxT("Constant"));
	m_pcConstantSetGrid->SetColSize(NAME, 100);



	m_pcConstantSetGrid->SetColLabelValue(GROUP, wxT("Group"));
	m_pcConstantSetGrid->SetColSize(GROUP, 120);

	m_pcConstantSetGrid->SetColLabelValue(TYPE, wxT("Type"));
	m_pcConstantSetGrid->SetColSize(TYPE, 120);

	m_pcConstantSetGrid->SetColLabelValue(COMMENT, wxT("Comment"));
	m_pcConstantSetGrid->SetColSize(COMMENT, 120);
	if (!m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))//do not add SHOW Column
	{
		m_pcConstantSetGrid->SetColLabelValue(SHOW, wxT("Show"));
		m_pcConstantSetGrid->SetColSize(SHOW, 50);
	}
	else
	{
		m_pcConstantSetGrid->HideCol(SHOW);
	}

 
	m_pcConstantSetGrid->SetColLabelValue(VALUES, wxT("Main"));
	m_pcConstantSetGrid->SetColSize(VALUES, 70);


	for (unsigned int i = 1; i < l_nSize; i++)
	{
		m_pcConstantSetGrid->AppendCols(1);
		m_pcConstantSetGrid->SetColLabelValue(i + m_nMainSet, l_pcColListTemp->GetCell(i, 0));
		m_pcConstantSetGrid->SetColSize(i + 3, 70);
	}
}

void SP_DLG_FpnConstantDefinition::InitializeChoice()
{
	SP_ListMetadata::const_iterator it;
	set<wxString>::iterator iter;

	set<wxString> l_lsChoice;

	l_lsChoice.clear();
	l_lsChoice.insert(wxT("all"));
	l_lsChoice.insert(wxT("marking"));
	l_lsChoice.insert(wxT("parameter"));

	if(m_pcGraph->GetNetclass()->GetName().Contains(_T("Colored")))
		l_lsChoice.insert(wxT("coloring"));

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

void SP_DLG_FpnConstantDefinition::AddNewChoice(wxCommandEvent& p_cEvent)
{
	wxString l_sNewValue = wxGetTextFromUser(wxT("Enter a new Group"), wxT("New Group"), wxT("default"));

	//no group with this name has been found
	if (!FindString(m_asGroups, l_sNewValue))
	{
		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sNewValue));

		if (!l_pcFunction->isVariable())
		{
			SP_MESSAGEBOX(wxT("group name '") + l_sNewValue + wxT("' is not valid"), wxT("Error"), wxOK | wxICON_ERROR);
			return;
		}

		//add this group name to the list
		m_asGroups.push_back(l_sNewValue);

		l_bWhite = false;
		for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
		{
			SP_WDG_GridCellChoiceEditor* l_pcCellEditor = dynamic_cast<SP_WDG_GridCellChoiceEditor*>(m_pcConstantSetGrid->GetCellEditor(i, GROUP));

			l_pcCellEditor->AddItem(l_sNewValue);

			m_pcConstantSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
	else
	{
		SP_MESSAGEBOX(wxT("group name '") + l_sNewValue + wxT("' already exists"), wxT("Error"), wxOK | wxICON_ERROR);
		//SP_LOG(wxLOG_Error, wxT("group name already exists"));
	}
}

bool SP_DLG_FpnConstantDefinition::FindString(wxArrayString& p_asStrArray, wxString& p_sValue)
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

void SP_DLG_FpnConstantDefinition::OnRenameSet(wxCommandEvent& p_cEvent)
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

bool SP_DLG_FpnConstantDefinition::ExistSetName(const wxString& p_sName)
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

void SP_DLG_FpnConstantDefinition::InitializeDataTypes()
{
	SP_DS_Metadata* l_pcMetadata;

	l_pcMetadata = m_pcConstants->GetPrototype();

	//dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->AddPossibleValue(wxString("--Fuzzy Numbers--"));// Added by G.A
	//dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->AddPossibleValue(wxString("TFN"));// Added by G.A
	//dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->AddPossibleValue(wxString("TZN"));// Added by G.A
	//dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->AddPossibleValue(wxString("BFN"));// Added by G.A
	//dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->AddPossibleValue(wxString("--Primitive Types--"));
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

void SP_DLG_FpnConstantDefinition::OnGridCellSelected(wxGridEvent& ev)
{
	ev.Skip();
}

void SP_DLG_FpnConstantDefinition::OnEditorShown(wxGridEvent& ev)
{
	m_sOldCellValue = m_pcConstantSetGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();
}

void SP_DLG_FpnConstantDefinition::LoadPlaces()
{
	//load discrete places
	LoadPlaceOfType(SP_DS_DISCRETE_PLACE);

	//load continuous places
	LoadPlaceOfType(SP_DS_CONTINUOUS_PLACE);
}

void SP_DLG_FpnConstantDefinition::LoadPlaceOfType(const wxString& p_sPlaceType)
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(p_sPlaceType);

	if (l_pcNodeclass == NULL)
	{
		return;
	}

	SP_ListNode::const_iterator l_itElem;
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Places.insert(l_sPlaceName);
	}
}


void SP_DLG_FpnConstantDefinition::OnChildDestroy(wxCloseEvent& event)
{

	int id = event.GetId();

	if (event.GetId() == this->GetId())
	{
		this->Destroy();
		return;
	}
	if (m_pcDraw)
	{/**
		wxString l_sValA = wxString::Format(wxT("%f"), m_pcDraw->GetAVal());
		wxString l_sValB = wxString::Format(wxT("%f"), m_pcDraw->GetBVal());
		wxString l_sValC = wxString::Format(wxT("%f"), m_pcDraw->GetCVal());
		l_sTFNFromDrawingPanel << l_sValA << wxT(",") << l_sValB << wxT(",")<< l_sValC;
		m_pcConstantSetGrid->SetCellValue(m_nRow, m_nCol, l_sTFNFromDrawingPanel);
		l_sTFNFromDrawingPanel.Empty();
		m_pcDraw->Destroy();
	*/
	}
}

void SP_DLG_FpnConstantDefinition::DeleteGroup(wxCommandEvent& p_cEvent)
{
	wxString l_sMsg = wxT("Please select a group to be deleted.\n");

	wxArrayString l_Choices;

	for (unsigned i = 0; i < m_asGroups.GetCount(); i++)
	{
		if (m_asGroups[i] == wxT("all") || m_asGroups[i] == wxT("parameter") ||
			m_asGroups[i] == wxT("marking") || m_asGroups[i] == wxT("coloring")) continue;

		l_Choices.Add(m_asGroups[i]);
	}
	int l_ReturnChoice = ::wxGetSingleChoiceIndex(l_sMsg, wxT("Delete constant."), l_Choices);



	if (l_ReturnChoice == -1 || l_Choices.GetCount()==0) return;

	wxString l_sCohesnGroup = l_Choices[l_ReturnChoice];

	for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		if (m_pcConstantSetGrid->GetCellValue(i, GROUP) == l_sCohesnGroup)
		{
			SP_MESSAGEBOX(wxT("The selected group cannot be deleted as it is already used"), wxT("Error"), wxOK | wxICON_ERROR);
			return;
		}
	}

	m_asGroups.Remove(l_sCohesnGroup);
	for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		m_pcConstantSetGrid->SetCellEditor(i, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
	}
	m_pcConstantSetGrid->Refresh();

}


void SP_DLG_FpnConstantDefinition::OnMultiColSorting(wxCommandEvent&  event)
{
	SP_Vector2DString l_vvGrid;
	l_vvGrid.resize(m_pcConstantSetGrid->GetNumberRows(), std::vector<wxString>(m_pcConstantSetGrid->GetNumberCols(), wxT("")));
	for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		SP_VectorString l_vRow;
		for (int j = 0; j < m_pcConstantSetGrid->GetNumberCols(); j++)
		{
			l_vvGrid[i][j] = m_pcConstantSetGrid->GetCellValue(i, j);

		}
	}

	std::map<wxString, int> l_mKey2Pos;


	SP_VectorString l_vColLabels;
	for (int j = 1; j < 4; j++)
	{

		l_vColLabels.push_back(m_pcConstantSetGrid->GetColLabelValue(j));
		l_mKey2Pos[m_pcConstantSetGrid->GetColLabelValue(j)] = j;
	}


	SP_DLG_SortDialoug* l_pcDialog = new SP_DLG_SortDialoug(this, l_vColLabels, l_mKey2Pos, l_vvGrid, m_nMainSet);
	if (l_pcDialog->ShowModal())
	{
		l_vvGrid = l_pcDialog->GetSortedTable();

		if (l_vvGrid.size() == 0)
		{
			l_pcDialog->Destroy();
			return;
		}

		for (int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
		{
			SP_VectorString l_vRow;
			for (int j = 0; j < m_pcConstantSetGrid->GetNumberCols(); j++)
			{
				m_pcConstantSetGrid->SetCellValue(i, j, l_vvGrid[i][j]);

			}
		}
	}

	l_pcDialog->Destroy();

	//mark the old constants to be deleted
	SP_VectorString v_deleteTmp;

	for (int n = 0; n < m_pcConstantSetGrid->GetNumberRows(); n++)
	{
		for (SP_DS_Metadata* l_pcMeta : *(m_pcConstants->GetElements()))
		{
			wxString l_sName = m_pcConstantSetGrid->GetCellValue(n, NAME);
			for (SP_DS_Metadata* l_pcMeta : *(m_pcConstants->GetElements()))
			{
				if (l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString()
					== l_sName)
				{
					auto it = std::find(v_deleteTmp.begin(), v_deleteTmp.end(), l_sName);
					if (it == v_deleteTmp.end())
					{
						m_deleted.push_back(l_pcMeta);
						v_deleteTmp.push_back(l_sName);
					}
				}
			}
		}
	}

}



void SP_DLG_FpnConstantDefinition::SortVlaueSets(std::multimap<std::string, float>&p_mVset2Val, std::vector<std::string>&p_vRes, bool p_bIsAscending)
{

	typedef std::function<bool(std::pair<std::string, float>, std::pair<std::string, float>)> Comparator;
	Comparator compFunctor =
		[](std::pair<std::string, float> elem1, std::pair<std::string, float> elem2)
	{

		return elem1.second < elem2.second;

	};

	// Declaring a set that will store the pairs using above comparision logic
	std::set<std::pair<std::string, float>, Comparator> setOfValues(
		p_mVset2Val.begin(), p_mVset2Val.end(), compFunctor);
	wxString ll;
	for (std::pair<std::string, float> element : p_mVset2Val)
	{
		ll << element.first << "::" << element.second << "\n";
		p_vRes.push_back(element.first);
	}


	if (!p_bIsAscending)
	{
		std::reverse(begin(p_vRes), end(p_vRes));
	}


}




bool SP_DLG_FpnConstantDefinition::EvalConstantExpression(const wxString& p_sArcWeight, double& p_dVal)
{
	double dValue = 0.0;
	std::string strValue = p_sArcWeight;

	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	wxString l_sArcWeight = p_sArcWeight;
	SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_sArcWeight);
	wxString l_sExpanded;
	if (l_pcFunction)
	{
		SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
		l_sExpanded = l_pcExpanded->toString();

		if (l_sExpanded.ToDouble(&dValue))
		{
			p_dVal = dValue;
			return true; //constant
		}
		else
		{
			return false;
		}
	}
	else
	{

		l_sExpanded = l_sArcWeight;
		return false;
	}
	return true;

}


void SP_DLG_FpnConstantDefinition::OnRowRightClick(wxGridEvent& event)
{
	unsigned row;
	row = event.GetRow();
	wxString l_sRow;
	row++;
	wxString l_sMsg = wxT("Please select the target constant position, to which you want to move the current constant.\n");

	wxArrayString l_Choices;


	m_norgRow = row - 1;
	for (unsigned i = 1; i <= m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		if (i == row) continue;

		wxString l_sOption;
		l_sOption << i;

		l_Choices.Add(l_sOption);
	}
	int l_ReturnChoice = ::wxGetSingleChoiceIndex(l_sMsg, wxT("Move constant."), l_Choices);

	long  l_nIndex;

	if (l_ReturnChoice == -1) return;

	if (l_Choices[l_ReturnChoice].ToLong(&l_nIndex))
	{

		MoveRow(l_nIndex);
	}
	else
	{
		return;
	}
}

void SP_DLG_FpnConstantDefinition::OnSortVsets(wxCommandEvent& p_cEvent)
{
	if (m_bIsAsc)
	{
		m_bIsAsc = false;
		m_pcSortingButton->SetLabelText(_T("Sort Value Sets (Desc.)"));

		for (unsigned int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
		{
			std::multimap<std::string, float> l_mVset2Val;
			for (unsigned j = m_nMainSet; j < m_pcConstantSetGrid->GetNumberCols(); j++)
			{


				if (m_pcConstantSetGrid->GetCellValue(i, j).ToStdString() == "")
				{
					string l_sTemp = "";
					continue;

				}

				double l_dval;
				bool l_bIsEvaluated = EvalConstantExpression(m_pcConstantSetGrid->GetCellValue(i, j), l_dval);

				if (!l_bIsEvaluated)
				{
					continue;
				}
				float fval = static_cast<float>(l_dval);

				l_mVset2Val.insert(std::pair<string, float>(m_pcConstantSetGrid->GetCellValue(i, j).ToStdString(), fval));


			}
			std::vector<string> l_vRes;
			SortVlaueSets(l_mVset2Val, l_vRes, true);

			//value of the constant
			for (unsigned int k = 0; k < l_vRes.size(); k++)
			{
				m_pcConstantSetGrid->SetCellValue(i, k + m_nMainSet, l_vRes[k]);
				m_pcConstantSetGrid->SetCellAlignment(i, k + m_nMainSet, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcConstantSetGrid->SetCellBackgroundColour(i, k + m_nMainSet, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			}
		}
	}
	else
	{
		m_bIsAsc = true;
		m_pcSortingButton->SetLabelText(_T("Sort Value Sets (Asc.)"));

		for (unsigned int i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
		{
			std::multimap<std::string, float> l_mVset2Val;
			for (unsigned j = m_nMainSet; j < m_pcConstantSetGrid->GetNumberCols(); j++)
			{


				if (m_pcConstantSetGrid->GetCellValue(i, j).ToStdString() == "")
				{
					string l_sTemp = "";
					continue;

				}

				double l_dval;
				bool l_bIsEvaluated = EvalConstantExpression(m_pcConstantSetGrid->GetCellValue(i, j), l_dval);

				if (!l_bIsEvaluated)
				{
					continue;
				}
				float fval = static_cast<float>(l_dval);

				l_mVset2Val.insert(std::pair<string, float>(m_pcConstantSetGrid->GetCellValue(i, j).ToStdString(), fval));


			}
			std::vector<string> l_vRes;
			SortVlaueSets(l_mVset2Val, l_vRes, false);

			//value of the constant
			for (unsigned int k = 0; k < l_vRes.size(); k++)
			{
				m_pcConstantSetGrid->SetCellValue(i, k + m_nMainSet, l_vRes[k]);
				m_pcConstantSetGrid->SetCellAlignment(i, k + m_nMainSet, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcConstantSetGrid->SetCellBackgroundColour(i, k + m_nMainSet, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			}
		}
	}

	//recover background colour of rows
	bool l_bISWhite = false;
	for (unsigned i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		for (unsigned j = 0; j < m_pcConstantSetGrid->GetNumberCols(); j++)
		{
			m_pcConstantSetGrid->SetCellBackgroundColour(i, j, (l_bISWhite ? *wxWHITE : *wxLIGHT_GREY));

		}
		l_bISWhite = !l_bISWhite;
	}
}


void SP_DLG_FpnConstantDefinition::MoveRow(long p_nToPos)
{
	if (p_nToPos <= 0) return;

	int l_nToRow = 0;
	l_nToRow = p_nToPos;
	l_nToRow = l_nToRow - 1;

	m_RowData.clear();
	m_RowData.Alloc(m_pcConstantSetGrid->GetNumberCols());
	for (unsigned i = 0; i < m_pcConstantSetGrid->GetNumberCols(); i++)
	{
		m_RowData.Add(m_pcConstantSetGrid->GetCellValue(m_norgRow, i));

	}

	wxColour l_OrgColour = m_pcConstantSetGrid->GetCellBackgroundColour(m_norgRow, 1);
	wxColour l_ToColour = m_pcConstantSetGrid->GetCellBackgroundColour(l_nToRow, 1);
	m_pcConstantSetGrid->DeleteRows(m_norgRow);
	int l_nfrom;
	int l_nto;
	if (m_pcConstantSetGrid->GetNumberRows() == l_nToRow)
	{
		//last row
		m_pcConstantSetGrid->AppendRows(1);
		l_nfrom = m_norgRow;
		l_nto = m_pcConstantSetGrid->GetNumberRows() - 1;
	}
	else if (l_nToRow == 0)
	{
		m_pcConstantSetGrid->InsertRows(0, 1);


	}
	else
	{
		m_pcConstantSetGrid->InsertRows(l_nToRow - 1, 1);
		for (unsigned i = 0; i <m_pcConstantSetGrid->GetNumberCols(); i++)
		{
			if (i == 0)//show coloumn
			{
				//show the constant
				auto l_pcBoolEditor = new wxGridCellBoolEditor();
				l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
				m_pcConstantSetGrid->SetCellEditor(l_nToRow - 1, SHOW, l_pcBoolEditor);
				m_pcConstantSetGrid->SetCellRenderer(l_nToRow - 1, SHOW, new wxGridCellBoolRenderer());
				m_pcConstantSetGrid->SetCellValue(l_nToRow - 1, SHOW, m_pcConstantSetGrid->GetCellValue(l_nToRow, i));
				continue;
			}
			m_pcConstantSetGrid->SetCellValue(l_nToRow - 1, i, m_pcConstantSetGrid->GetCellValue(l_nToRow, i));
			m_pcConstantSetGrid->SetCellAlignment(l_nToRow - 1, i, wxALIGN_CENTER, wxALIGN_TOP);

		}
	}

	for (unsigned i = 0; i <m_pcConstantSetGrid->GetNumberCols(); i++)
	{
		if (i == 0)//show coloumn
		{
			//show the constant
			auto l_pcBoolEditor = new wxGridCellBoolEditor();
			l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
			m_pcConstantSetGrid->SetCellEditor(l_nToRow - 1, SHOW, l_pcBoolEditor);
			m_pcConstantSetGrid->SetCellRenderer(l_nToRow - 1, SHOW, new wxGridCellBoolRenderer());
			m_pcConstantSetGrid->SetCellValue(l_nToRow - 1, SHOW, m_RowData[i]);
			m_pcConstantSetGrid->SetCellAlignment(l_nToRow, i, wxALIGN_CENTER, wxALIGN_TOP);
			m_pcConstantSetGrid->SetCellBackgroundColour(l_nToRow, i, l_ToColour);
			continue;
		}

		m_pcConstantSetGrid->SetCellValue(l_nToRow, i, m_RowData[i]);
		m_pcConstantSetGrid->SetCellAlignment(l_nToRow, i, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcConstantSetGrid->SetCellBackgroundColour(l_nToRow, i, l_ToColour);// (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	}


	bool l_bISWhite = false;
	for (unsigned i = 0; i < m_pcConstantSetGrid->GetNumberRows(); i++)
	{
		for (unsigned j = 0; j < m_pcConstantSetGrid->GetNumberCols(); j++)
		{
			m_pcConstantSetGrid->SetCellBackgroundColour(i, j, (l_bISWhite ? *wxWHITE : *wxLIGHT_GREY));

		}
		l_bISWhite = !l_bISWhite;
	}
}
