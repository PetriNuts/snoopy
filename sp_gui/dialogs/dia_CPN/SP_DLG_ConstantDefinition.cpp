//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// @Modified: George Assaf
// @Date:15/01/2020
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h> 
#include "sp_gui/widgets/dialogs/SP_WDG_GridCellChoiceEditor.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_SortDialoug.h"
IMPLEMENT_CLASS( SP_DLG_ConstantDefinition, wxDialog )

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING,
	SP_ID_VALUESET_ADD,//added by G.A
	SP_ID_VALUESET_DELETE,//added by G.A
	SP_ID_BUTTON_RENAMESET,//added by G.A
	SP_ID_NEWGROUP,
	SP_ID_DELGROUP,
	SP_ID_SORTSETS,
	SP_ID_MULTICOL_SORT

};
 
enum {
	NAME=0,
	GROUP,
	TYPE,
	COMMENT,
	MAIN
};

BEGIN_EVENT_TABLE( SP_DLG_ConstantDefinition, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ConstantDefinition::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ConstantDefinition::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_ConstantDefinition::OnAddConstant)
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_ConstantDefinition::OnCheckFunction )
EVT_BUTTON( wxID_APPLY, SP_DLG_ConstantDefinition::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_ConstantDefinition::OnDeleteColorSet )
EVT_BUTTON(SP_ID_NEWGROUP, SP_DLG_ConstantDefinition::AddNewChoice)
EVT_BUTTON(SP_ID_VALUESET_ADD, SP_DLG_ConstantDefinition::OnAddValueSet)
EVT_BUTTON(SP_ID_VALUESET_DELETE, SP_DLG_ConstantDefinition::OnDeleteValueSet)
EVT_BUTTON(SP_ID_BUTTON_RENAMESET, SP_DLG_ConstantDefinition::OnRenameSet)
EVT_GRID_CMD_CELL_RIGHT_CLICK(SP_ID_GRID_MARKING, SP_DLG_ConstantDefinition::OnRowRightClick)
EVT_BUTTON(SP_ID_SORTSETS, SP_DLG_ConstantDefinition::OnSortVsets)
EVT_BUTTON(SP_ID_DELGROUP, SP_DLG_ConstantDefinition::DeleteGroup)

EVT_BUTTON(SP_ID_MULTICOL_SORT, SP_DLG_ConstantDefinition::OnMultiColSorting)
#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_ConstantDefinition::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_ConstantDefinition::OnGridCellValueChanged ) 
#endif

	EVT_GRID_SELECT_CELL(SP_DLG_ConstantDefinition::OnGridCellSelected)
	EVT_GRID_EDITOR_SHOWN(SP_DLG_ConstantDefinition::OnEditorShown)

END_EVENT_TABLE()



SP_DLG_ConstantDefinition::SP_DLG_ConstantDefinition( wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row
	m_pcColorSetGrid = new wxGrid( this, SP_ID_GRID_MARKING,
	wxDefaultPosition, wxSize( 300, 300 ), wxSUNKEN_BORDER );

	m_pcColorSetGrid->CreateGrid( 0, 0);
	m_pcColorSetGrid->EnableEditing( true);	

	m_pcColorSetGrid->SetDefaultColSize( 70, TRUE);
	m_pcColorSetGrid->SetDefaultRowSize( 20, TRUE);

	m_pcColorSetGrid->SetColLabelSize( 16);
	m_pcColorSetGrid->SetRowLabelSize( 0);

	l_pcRowSizer->Add(m_pcColorSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcColorSetGrid->EnableEditing(true);
	m_pcColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	


	l_bWhite = false;
	m_lMain = 4;

	m_bIsAsc = true;
	l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_pcConstants = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);

	LoadPlaces();
	LoadHarmonizedData();
 
    SP_AutoSizeRowLabelSize(m_pcColorSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL );
	wxSizer *l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pcSortingButton = new wxButton(this, SP_ID_SORTSETS, wxT("Sort Value Sets (Asc)"));
	m_pcSortingCol = new wxButton(this, SP_ID_MULTICOL_SORT, wxT("Multi column sorting"));

	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add constant") ), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete constant") ), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check constants") ), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_NEWGROUP, wxT("New Group")), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_DELGROUP, wxT("Delete Group")), 1, wxALL, 5);


	l_pcSizer->Add(m_pcSortingButton, 1, wxALL, 5);
	//l_pcSizer->Add(m_pcMovingRowsButton, 1, wxALL, 5);

	wxSizer *l_pcSizer2 = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer2->Add(m_pcSortingCol, 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_ADD, wxT("New Value Set")), 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_VALUESET_DELETE, wxT("Delete Value Set")), 1, wxALL, 5);
	l_pcSizer2->Add(new wxButton(this, SP_ID_BUTTON_RENAMESET, wxT("Rename Value Set")), 1, wxALL, 5);


	wxBoxSizer* l_pcButtonSizer2 = new wxBoxSizer(wxHORIZONTAL);//G.A
	l_pcButtonSizer2->Add(l_pcSizer2, 0, wxEXPAND);

	wxSizer* l_pcBottomButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);


	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND );
	l_pcButtonSizer2->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND );
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer2, 0, wxEXPAND);


	

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();

}


void SP_DLG_ConstantDefinition::OnDlgApply( wxCommandEvent& p_cEvent )
{	
	 SaveData();
 
}


void SP_DLG_ConstantDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (!CheckInput())//check user's input for values errors
	{
		return;
	}
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			if (SaveNewDate())
			{
				wxBusyInfo wait(wxT("Please wait, updating marking..."),this);
				//by sl
				wxString l_sName = l_pcGraph->GetNetclass()->GetName();
				/*
				 * todo update for other netclasses to show in declarationtree
				 */
				if (l_sName == SP_DS_PN_CLASS
					|| l_sName == SP_DS_EXTPN_CLASS
					|| l_sName == SP_DS_CONTINUOUSPED_CLASS
					|| l_sName == SP_DS_HYBRIDPN_CLASS
					|| l_sName == SP_DS_SPN_CLASS)
				{
					l_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

				}
				else
				{
					l_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

					SP_CPN_SyntaxChecking l_cSyntaxChecking;
					if(l_cSyntaxChecking.Initialize())
					{
						l_cSyntaxChecking.UpdateNetMarking();
					}
				}

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

void SP_DLG_ConstantDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_ConstantDefinition::OnEditColorSet( wxCommandEvent& p_cEvent )
{
	if (m_pcColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcColorSetGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcColorSetGrid->GetCellValue(l_nEditRowPos, 1);

	m_pcColorSetGrid->SelectBlock(l_nEditRowPos, 1, l_nEditRowPos, 1);
	m_pcColorSetGrid->SetFocus();
}

void SP_DLG_ConstantDefinition::OnDeleteColorSet( wxCommandEvent& p_cEvent )
{
	if (m_pcColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}

	if ( m_pcColorSetGrid->IsSelection() )
    {
		m_pcColorSetGrid->BeginBatch();
        for ( int n = 0; n < m_pcColorSetGrid->GetNumberRows(); )
        {
          if (m_pcColorSetGrid->IsInSelection(n, 0))
		  {
			wxString l_sName = m_pcColorSetGrid->GetCellValue(n, NAME);
			for (SP_DS_Metadata* l_pcMeta : *(m_pcConstants->GetElements()))
			{
				if (l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString()
					== l_sName)
				{
					m_deleted.push_back(l_pcMeta);//keep it temporarly to be deleted when saving
				}
			}
			m_pcColorSetGrid->DeleteRows(n, 1);
		 }

        else
            n++;
        }
        m_pcColorSetGrid->EndBatch();
    }
}

 
void SP_DLG_ConstantDefinition::OnAddConstant( wxCommandEvent& p_cEvent )
{
	int l_nCountRow = m_pcColorSetGrid->GetNumberRows();
	wxColour l_colour=m_pcColorSetGrid->GetCellBackgroundColour(l_nCountRow-1,0);

	if (l_colour == *wxWHITE)
	{
		l_bWhite = false;

	}
	else {
		l_bWhite = true;

	}
	//(l_bWhite ? l_bWhite = false : l_bWhite = true);
	m_pcColorSetGrid->AppendRows(1);


	InitializeChoice();

	int l_nEditRowPos = m_pcColorSetGrid->GetNumberRows()-1;	

	m_pcColorSetGrid->SetCellBackgroundColour(l_nEditRowPos, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));//Name

	m_pcColorSetGrid->SetCellEditor(l_nEditRowPos,2,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));//Type
	m_pcColorSetGrid->SetCellSize(l_nEditRowPos, 2, 1, 1);
	m_pcColorSetGrid->SetCellValue(l_nEditRowPos, 2, wxT("int"));	
	m_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 2, false);
	m_pcColorSetGrid->SetCellBackgroundColour(l_nEditRowPos,2, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcColorSetGrid->SetCellEditor(l_nEditRowPos, 1, new SP_WDG_GridCellChoiceEditor(m_asGroups));//Group
	m_pcColorSetGrid->SetCellSize(l_nEditRowPos, 1, 1, 1);
	m_pcColorSetGrid->SetCellValue(l_nEditRowPos, 1, wxT("all"));
	m_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 1, false);
	m_pcColorSetGrid->SetCellBackgroundColour(l_nEditRowPos, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcColorSetGrid->SetCellBackgroundColour(l_nEditRowPos, 3, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));//Comment

	for (int j = 4; j < m_pcColorSetGrid->GetNumberCols(); j++)
	{
		m_pcColorSetGrid->SetCellBackgroundColour(l_nEditRowPos, j, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	}

	

	//m_pcColorSetGrid->SetCellEditor(l_nEditRowPos, 1, new SP_WDG_GridCellChoiceEditor(m_asGroups));//group
}



void SP_DLG_ConstantDefinition::LoadSetNames()
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

	m_pcColorSetGrid->AppendCols(5);
	

	m_pcColorSetGrid->SetColLabelValue(0, wxT("Name"));
	m_pcColorSetGrid->SetColSize(0, 100);


	m_pcColorSetGrid->SetColLabelValue(1, wxT("Group"));
	m_pcColorSetGrid->SetColSize(1, 120);

	m_pcColorSetGrid->SetColLabelValue(2, wxT("Type"));
	m_pcColorSetGrid->SetColSize(2, 120);

	m_pcColorSetGrid->SetColLabelValue(3, wxT("Comment"));
	m_pcColorSetGrid->SetColSize(3, 120);


	m_pcColorSetGrid->SetColLabelValue(4, wxT("Main"));
	m_pcColorSetGrid->SetColSize(4, 70);

	for (unsigned int i = 1; i < l_nSize; i++)
	{
		if( l_pcColListTemp->GetCell(i, 0).IsEmpty())
		{
			continue;
		}
		m_pcColorSetGrid->AppendCols(1);
		m_pcColorSetGrid->SetColLabelValue(i + 4, l_pcColListTemp->GetCell(i, 0));
		m_pcColorSetGrid->SetColSize(i + 3, 70);
	}
}
void SP_DLG_ConstantDefinition::LoadHarmonizedData()
{

	LoadSetNames();


	bool l_bWhite = false;

	if (m_pcColorSetGrid->GetNumberRows() > 0)
	{
		m_pcColorSetGrid->DeleteRows(0, m_pcColorSetGrid->GetNumberRows());
	}
	
	

	InitializeChoice();

	unsigned int l_nPos = 0;
	l_bWhite = false;
	for (SP_DS_Metadata* l_pcMetadata : *(m_pcConstants->GetElements()))
	{
		wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
		wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
		wxString l_sMetadataComment = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Comment")))->GetValue();

		//wxString l_sMetadataShow = l_pcMetadata->GetShow() ? wxT("1") : wxT("0");
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));

		m_pcColorSetGrid->AppendRows(1);
		 
		//name of the constant
		m_pcColorSetGrid->SetCellValue(l_nPos, 0, l_sMetadataName);
		m_pcColorSetGrid->SetCellAlignment(l_nPos, 0, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcColorSetGrid->SetCellBackgroundColour(l_nPos, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		//group of the constant
		 
		m_pcColorSetGrid->SetCellEditor(l_nPos, 1, new SP_WDG_GridCellChoiceEditor(m_asGroups));//new SP_WDG_GridCellChoiceEditor(m_asGroups.Count(),groups));
		m_pcColorSetGrid->SetCellValue(l_nPos, 1, l_sMetadataGroup);
		m_pcColorSetGrid->SetCellAlignment(l_nPos, 1, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcColorSetGrid->SetCellBackgroundColour(l_nPos, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//type of the constant
		m_pcColorSetGrid->SetCellEditor(l_nPos, 2, new wxGridCellChoiceEditor(m_choices.Count(), choices));//new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcColorSetGrid->SetCellValue(l_nPos, 2, l_sMetadataType);
		m_pcColorSetGrid->SetCellAlignment(l_nPos, 2, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcColorSetGrid->SetCellBackgroundColour(l_nPos, 2, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//comment of the constant
		m_pcColorSetGrid->SetCellValue(l_nPos, 3, l_sMetadataComment);
		m_pcColorSetGrid->SetCellAlignment(l_nPos, 3, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcColorSetGrid->SetCellBackgroundColour(l_nPos, 3, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//value of the constant
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcColorSetGrid->SetCellValue(l_nPos, i + 4, l_pcColList->GetCell(i, 1));
			m_pcColorSetGrid->SetCellAlignment(l_nPos, i + 4, wxALIGN_CENTER, wxALIGN_TOP);
			m_pcColorSetGrid->SetCellBackgroundColour(l_nPos, i + 4, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		}

		l_nPos++;
		(l_bWhite ? l_bWhite = false : l_bWhite = true);

	}

}

bool SP_DLG_ConstantDefinition::LoadData()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());//l_pcMetadataclass->NewElement(1);
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));		

	bool l_bWhite = false;

	if (m_pcColorSetGrid->GetNumberRows() > 0)
	{
		m_pcColorSetGrid->DeleteRows(0, m_pcColorSetGrid->GetNumberRows());
	}

	m_pcColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	InitializeChoice();
	 
	

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		//name
		m_pcColorSetGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		
		//group
		//m_pcColorSetGrid->SetCellEditor(i, 1, new wxGridCellChoiceEditor(m_asGroups.GetCount(), groups));//group
	     m_pcColorSetGrid->SetCellEditor(i, 1, new SP_WDG_GridCellChoiceEditor(m_asGroups));//group
			wxString l_sGroup;
		l_sGroup<< l_pcColList->GetCell(i, 4).c_str();
		if (l_sGroup.IsEmpty())
		{
			m_pcColorSetGrid->SetCellSize(i, 1, 1, 1);
			m_pcColorSetGrid->SetCellValue(i, 1, wxT("all"));//cell val
			m_pcColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		}
		else {
			m_pcColorSetGrid->SetCellSize(i, 1, 1, 1);
			m_pcColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i, 4).c_str());//cell val
			m_pcColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		}
		

		
		//type
		m_pcColorSetGrid->SetCellEditor(i,2,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcColorSetGrid->SetCellSize(i, 2, 1, 1);
		m_pcColorSetGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,1).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		
		//value 
		wxString l_sVal;
		l_sVal = l_pcColList->GetCell(i, 2).c_str();
		m_pcColorSetGrid->SetCellValue(i, 4, l_pcColList->GetCell(i,2).c_str());
		m_pcColorSetGrid->SetCellAlignment(i,4, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 4, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));

		//comment
		wxString l_sComm;
		l_sComm << l_pcColList->GetCell(i, 3).c_str();
		m_pcColorSetGrid->SetCellValue(i,3, l_pcColList->GetCell(i,3).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 3, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));


		

		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	 
	}

	vector<wxString> l_sVsets;
	map<wxString, int> l_mLabel2Col;
	//l_bWhite = false;
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		for (unsigned int j = 6; j <= l_pcColList->GetColCount(); j++)
		{
			wxString l_sLabel= l_pcColList->GetCell(i, j).c_str();
			bool l_bIsChanged = false;
			if (l_sLabel.Contains("V-"))
			{
				unsigned int cn = m_pcColorSetGrid->GetNumberCols();
				auto it = std::find(l_sVsets.begin(), l_sVsets.end(), l_sLabel);
				auto itm = l_mLabel2Col.find(l_sLabel);
				
				if (itm == l_mLabel2Col.end())
				{
					l_bIsChanged = true;
					m_pcColorSetGrid->AppendCols(1);
					l_sVsets.push_back(l_sLabel);
					l_mLabel2Col[l_sLabel] = cn;
					m_pcColorSetGrid->SetColLabelValue(cn, l_sLabel);
					m_pcColorSetGrid->SetColSize(i + 3, 70);

					m_pcColorSetGrid->SetCellValue(i, cn, l_pcColList->GetCell(i, j + 1).c_str());
					m_pcColorSetGrid->SetCellAlignment(i, cn, wxALIGN_LEFT, wxALIGN_CENTER);
					wxColour xc = m_pcColorSetGrid->GetCellBackgroundColour(i, cn - 1);
					m_pcColorSetGrid->SetCellBackgroundColour(i, cn, xc);
					 
				}
				else {
					l_bIsChanged = true;
					int cn1 = l_mLabel2Col[l_sLabel];
					m_pcColorSetGrid->SetCellValue(i, cn1, l_pcColList->GetCell(i, j + 1).c_str());
					m_pcColorSetGrid->SetCellAlignment(i, cn1, wxALIGN_LEFT, wxALIGN_CENTER);
					wxColour xc = m_pcColorSetGrid->GetCellBackgroundColour(i, cn1 - 1);
					m_pcColorSetGrid->SetCellBackgroundColour(i, cn1, xc);
					 
				}
				//if(l_bIsChanged = true)
				(l_bWhite ? l_bWhite = false : l_bWhite = true);
				}

		 
			 
		}

	}
	/*******************************/
  
	delete [] choices;
	return true;

}
bool SP_DLG_ConstantDefinition::SaveNewDate()
{
	m_pcColorSetGrid->SaveEditControlValue();

	SP_DS_Metadata* l_pcConstant;

	// delete constants
	for (auto l_pcConst : m_deleted)
	{
		m_pcConstants->RemoveElement(l_pcConst);
		wxDELETE(l_pcConst);
	}
	SP_DS_Graph* l_pcGraph;

	if (m_pcColorSetGrid->GetNumberRows() == 0) return true;
 
	while (m_pcConstants->GetElements()->size() < m_pcColorSetGrid->GetNumberRows())
	{
		m_pcConstants->NewElement(1);

	}

	SP_ListMetadata::const_iterator l_itElem;
	l_itElem = m_pcConstants->GetElements()->begin();

	for (int l_nRow = 0; l_nRow < m_pcColorSetGrid->GetNumberRows(); l_nRow++)
	{
		l_pcConstant = *l_itElem;

		wxString l_sName = m_pcColorSetGrid->GetCellValue(l_nRow, 0);
		wxString l_sGroup = m_pcColorSetGrid->GetCellValue(l_nRow, 1);
		wxString l_sType = m_pcColorSetGrid->GetCellValue(l_nRow, 2);
		wxString l_sComment = m_pcColorSetGrid->GetCellValue(l_nRow, 3);
 
		l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
		l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
		l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

		bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);
		if (!l_bValue)
		{
			wxString l_sAvailableDataTypes;
			for (int i = 0; i < m_choices.GetCount(); i++)
			{ l_sAvailableDataTypes += m_choices[i] + "\n "; };
		
			
			SP_MESSAGEBOX(wxT("datatype '") + l_sType + wxT("' for constant '") + l_sName + wxT("' is not allowed (use one of '") +  l_sAvailableDataTypes + wxT("')"), wxT("Error"),
				wxOK | wxICON_ERROR);
			return false;
		}

		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));

		l_pcColList->Clear();
		for (int j = 4; j < m_pcColorSetGrid->GetNumberCols(); j++)
		{
			int l_nRowCol = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nRowCol, 0, m_pcColorSetGrid->GetColLabelValue(j));
			l_pcColList->SetCell(l_nRowCol, 1, m_pcColorSetGrid->GetCellValue(l_nRow, j));
		}
		wxString l_sMainVal = m_pcColorSetGrid->GetCellValue(l_nRow, 4);
	     l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		 SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
		 SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sMainVal));
		 if (l_sType == wxT("double"))
		 {
			 double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }();
			 wxString l_sDouble;
			 l_sDouble << l_Val;
			 l_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sDouble);
			 l_pcColList->UpdateActiveListColumnPtr();

			 l_pcConstant->SetShow(true);
			 l_pcConstant->Update();

			 ++l_itElem;
			 continue;
		 }

		l_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, m_pcColorSetGrid->GetCellValue(l_nRow, 4));

		l_pcColList->UpdateActiveListColumnPtr();

		l_pcConstant->SetShow(true);
		l_pcConstant->Update();

		++l_itElem;

	}

	l_pcGraph->Update();

	return true;

}
bool SP_DLG_ConstantDefinition::SaveData()
{
	m_pcColorSetGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	l_pcColList->Clear();

	SP_DS_Metadata* l_pcConstant;
	SP_DS_Metadataclass* m_pcConstants = l_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT); //

	SP_ListMetadata::const_iterator l_itElem;
	l_itElem = m_pcConstants->GetElements()->begin();

	
	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		 
		int l_nRow=l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nRow, 0, m_pcColorSetGrid->GetCellValue(i, 0));//name		
		l_pcColList->SetCell(l_nRow, 1, m_pcColorSetGrid->GetCellValue(i, 2));//type
		l_pcColList->SetCell(l_nRow, 2, m_pcColorSetGrid->GetCellValue(i, 4));//value
		l_pcColList->SetCell(l_nRow, 3, m_pcColorSetGrid->GetCellValue(i, 3));//comment
		l_pcColList->SetCell(l_nRow, 4, m_pcColorSetGrid->GetCellValue(i, 1));//group
		
	
	 
		bool b_isVal = false;
		for (unsigned int j = m_lMain; j < m_pcColorSetGrid->GetNumberCols(); j++)
		{
			b_isVal = true;
			//int l_nRowCol = l_pcColList->AppendEmptyRow();
			int l_nC=l_pcColList->AppendEmptyColum();
			l_pcColList->SetCell(l_nRow, l_nC, m_pcColorSetGrid->GetColLabelValue(j));
			int l_nC1 = l_pcColList->AppendEmptyColum();
			l_pcColList->SetCell(l_nRow , l_nC1, m_pcColorSetGrid->GetCellValue(l_nRow, j));
			 
		}
	 
	}

	SaveNewDate();

	return true;

}


void SP_DLG_ConstantDefinition::InitializeChoice()
{
	unsigned l_nArraysize;
	l_nArraysize = 0;

	m_choices.Alloc(l_nArraysize + 8);
	m_choices.Empty();

	m_choices.Add(wxT("----Basic types----"));
	m_choices.Add(wxT("string"));
	m_choices.Add(wxT("int"));
	m_choices.Add(wxT("double"));//added by G.A for harmonize constants
	m_choices.Add(wxT("bool"));
	m_choices.Add(wxT("enum"));
	m_choices.Add(wxT("index"));
	m_choices.Add(wxT("----Structured types----"));
	m_choices.Add(wxT("product"));

	size_t count = m_choices.GetCount();
	choices = new wxString[count];
	for ( size_t n = 0; n < count; n++ )
	{
		
		choices[n] = m_choices[n];
		wxString s = choices[n];
	}

 /**********/
	set<wxString> l_lsChoice;

	l_lsChoice.clear();
	l_lsChoice.insert(wxT("all"));
	l_lsChoice.insert(wxT("marking"));
	l_lsChoice.insert(wxT("parameter"));
	l_lsChoice.insert(wxT("coloring"));

	for (auto it = m_pcConstants->GetElements()->begin(); it != m_pcConstants->GetElements()->end(); ++it)
	{

		SP_DS_Metadata* l_pcMetadata = *it;

		wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();

		//set's elements are unique, therefore we do not need to make a check
		l_lsChoice.insert(l_sGroup);
	}

	size_t count1 = l_lsChoice.size();
	groups = new wxString[count1];
	size_t n = 0;
	for (auto it = l_lsChoice.begin(); it != l_lsChoice.end(); ++it)
	{
		groups[n] = *it;
		n++;
	}


	m_asGroups.Alloc(l_lsChoice.size());

	for (auto iter = l_lsChoice.begin(); iter != l_lsChoice.end(); ++iter)
	{
		
		auto it = std::find(m_asGroups.begin(), m_asGroups.end(), *iter);
		if (it == m_asGroups.end())
		{
			m_asGroups.Add(*iter);
		}
		
	}
}


void SP_DLG_ConstantDefinition::OnCheckFunction( wxCommandEvent& p_cEvent )
{
	
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	bool l_bCheckConForCs = l_cValueAssign.InitializeColorset(l_cColorSetClass);
	if( !l_bCheckConForCs)
		return;
	

	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The constant definition");	
	l_sVariables += wxT(" is correct.");
    // new wxTipWindow(this, l_sVariables, 1000);
	/*****by george *****/
	int col = m_pcColorSetGrid->GetGridCursorCol();
	int row = m_pcColorSetGrid->GetGridCursorRow();

	if (col >= 4)
	{
		wxString l_sCellValue = m_pcColorSetGrid->GetCellValue(row, col);
		wxString l_sName = m_pcColorSetGrid->GetCellValue(row,0);
		wxString l_sType = m_pcColorSetGrid->GetCellValue(row, 2);

		bool l_bCheck=DoCheckFunction(l_sName, l_sType, l_sCellValue);
		return;
	}
	else {
		wxString l_sCellContent = m_pcColorSetGrid->GetCellValue(row,col);
		if (l_sCellContent.IsEmpty())
		{
			wxString l_sName = m_pcColorSetGrid->GetCellValue(row, 0);
			wxString l_sType = m_pcColorSetGrid->GetCellValue(row, 2);
			wxString l_sMain= m_pcColorSetGrid->GetCellValue(row, 4);
			bool l_bCheck = DoCheckFunction(l_sName, l_sType, l_sMain);
			SP_MESSAGEBOX(wxT("Please select a value of a constant!"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			return;
		}

		
	}

	new wxTipWindow(this, l_sVariables, 1000);
	return;
}


bool SP_DLG_ConstantDefinition::GetIntConstantValue(wxString p_sConstExpr, double& p_nValue)
{
	SP_CPN_Parse_Context_ptr m_pParseContext;
	SP_CPN_Driver_ptr  m_pDriver;

	SP_CPN_ColorSetClass* m_pcColorSetClass;
	wxString l_sConstExpr = p_sConstExpr;
	double l_nValue;
	if (l_sConstExpr.ToDouble(&l_nValue))
	{
		p_nValue = l_nValue;
	}
	else
	{
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);

		m_pParseContext->SetColorSetClass(m_pcColorSetClass);
		m_pParseContext->SetColorSetName(wxT(""));
		m_pParseContext->SetFunctionFlag(false);
		//m_pParseContext->SetNetClassName( m_sNetClassName );	
		m_pParseContext->SetPlaceType(wxT("Place"));
		std::string m_ParseString;
		m_ParseString = string(l_sConstExpr.mb_str());

		bool l_bEval = m_pDriver->parse_string(m_ParseString, "input");
		if (!l_bEval)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.");
			SP_LOGERROR(l_sError);
			return false;
		}

		bool l_bCheck = m_pParseContext->CheckExpression();
		if (!l_bCheck)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.");
			SP_LOGERROR(l_sError);
			return false;
		}

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;
		if (!m_pParseContext->EvaluateExpression(l_EvaluateResultArray) || l_EvaluateResultArray.size() == 0)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.");
			SP_LOGERROR(l_sError);
			return false;
		}

		wxString l_sValue = l_EvaluateResultArray.begin()->m_ColorValue;

		if (l_sValue.ToDouble(&l_nValue))
		{
			p_nValue = l_nValue;
		}
		else
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.");
			SP_LOGERROR(l_sError);
			return false;
		}

	}

	return true;
}



bool SP_DLG_ConstantDefinition::DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sValue)
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
	if (m_choices.Index(p_sType) == wxNOT_FOUND && p_sType!=wxT("product"))
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" has unknown type ") + p_sType, wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	if (SP_Find(SP_KEYWORDLIST, p_sName) != SP_KEYWORDLIST.end())
	{
		SP_MESSAGEBOX(wxT(" A Keyword can not be a constant name! "), wxT("Check Constant"), wxOK | wxICON_ERROR);
		//m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
		return false;
	}
	 
	bool l_bOk = false;
	wxString l_sValue = p_sValue;
	double l_dVal;

	 
	  
	m_pcConstants = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sValue));
	if (l_pcFunction)
	{
		SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

		std::set<std::string> l_Vars;
		l_pcExpanded->getVariables(l_Vars);

		if (l_Vars.empty())
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
	 
	if (!l_bOk)
	{
		SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is not correct"), wxT("Check Constant"), wxOK | wxICON_ERROR);
		return false;
	}

	if (p_sType == wxT("int"))
	{
		long l_Val = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_pcFunction, std::numeric_limits<long>::min() }();
		if (l_bOk)
		{
			// l_sValue << l_Val;
			//SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), wxT("Check Constant"), wxOK | wxICON_INFORMATION);
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
		}
		return true;
	}
	else if (p_sType == wxT("double"))
	{
		double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }();
		if (l_Val >= 0.0)
		{
			l_sValue << l_Val;
			
			new wxTipWindow(this, wxT("the constant ") + p_sName + wxT(" with value ") + l_sValue + wxT(" is correct"), 1000);
		}
		else
		{
			SP_MESSAGEBOX(wxT("the constant ") + p_sName + wxT(" with value ") + p_sValue + wxT(" is possibly not correct"), wxT("Check Constant"), wxOK | wxICON_WARNING);
		}
		return true;
	}
	return false;
}


void SP_DLG_ConstantDefinition::LoadPlaces()
{
	//load discrete places
	LoadPlaceOfType(SP_DS_DISCRETE_PLACE);

	//load continuous places
	LoadPlaceOfType(SP_DS_CONTINUOUS_PLACE);
}


void SP_DLG_ConstantDefinition::LoadPlaceOfType(const wxString& p_sPlaceType)
{
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(p_sPlaceType);

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



void SP_DLG_ConstantDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{

	int col = p_gEvent.GetCol();
	int row = p_gEvent.GetRow();

	wxString colLabel = m_pcColorSetGrid->GetColLabelValue(col);

	wxString l_sCellValue = m_pcColorSetGrid->GetCellValue(row, col);

	if (colLabel == wxT("Name"))
	{
		if (SP_Find(SP_KEYWORDLIST, l_sCellValue) != SP_KEYWORDLIST.end())
		{
			SP_MESSAGEBOX(wxT(" A Keyword can not be a constant name! "), wxT("Check Constant"), wxOK | wxICON_ERROR);
			m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		if (ExistConstant(l_sCellValue, row))
		{
			SP_MESSAGEBOX(wxT("the constant '") + l_sCellValue + wxT("' already exists !"), wxT("Error"), wxOK | wxICON_ERROR);


			m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		if (m_Places.find(l_sCellValue) != m_Places.end())
		{
			SP_MESSAGEBOX(wxT("the constant '") + l_sCellValue + wxT("' already exists as place!"), wxT("Error"), wxOK | wxICON_ERROR);


			m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}
		return;
	}

	if (colLabel == wxT("Type"))
	{
		if (l_sCellValue == wxT("Fuzzy Number"))
			SP_MESSAGEBOX("Bell", wxT("Error"), wxOK | wxICON_ERROR);

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

	if (col >= 4)
	{

		 
		if (l_sCellValue.IsEmpty())
		{
			if (col == 4)
			{
				wxString l_sMsg = wxT("Value set 'Main'");

				l_sMsg << wxT(" for '") << m_pcColorSetGrid->GetCellValue(p_gEvent.GetRow(), 0);

				l_sMsg << wxT("' must not be empty!");

				SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

				m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
			}

			return;
		}

		wxString l_sConstName = m_pcColorSetGrid->GetCellValue(row, NAME);
		double d;
		if (!l_sCellValue.ToDouble(&d))
		{

		}

		wxString l_sConstType = m_pcColorSetGrid->GetCellValue(row, TYPE);

		if (!DoCheckUserInput(row))
		{
			//SP_MESSAGEBOX(wxT("the constant '") + l_sConstName + wxT("' is not allowed in the expression: ") + l_sCellValue, wxT("Error"), wxOK | wxICON_ERROR);
			m_pcColorSetGrid->SetCellValue(row, col, m_sOldCellValue);
		}
		else {
			new wxTipWindow(this, wxT("the constant ") + l_sConstName + wxT(" with value ") + l_sCellValue + wxT(" is correct"), 1000);
		}
	}

	return;
}

void SP_DLG_ConstantDefinition::OnAddValueSet(wxCommandEvent& p_cEvent)
{
	//m_pcColorSetGrid
	wxString l_sName = wxT("V-Set ");
	l_sName.Append(wxString::Format(wxT("%d"),(unsigned) m_pcColorSetGrid->GetNumberCols() - m_lMain - 1));

	m_pcColorSetGrid->AppendCols(1);
	m_pcColorSetGrid->SetColSize(1, 70);
	m_pcColorSetGrid->SetColLabelValue(m_pcColorSetGrid->GetNumberCols() - 1, l_sName);

	int l_nGridCol = m_pcColorSetGrid->GetNumberCols() - 1;
	int l_nGridRow = 0;

	//bool l_bWhite = false;
	l_bWhite = false;
	if (0 < l_nGridCol)
	{
		//m_lMain++;
		for (l_nGridRow = 0; l_nGridRow < m_pcColorSetGrid->GetNumberRows(); l_nGridRow++)
		{
			m_pcColorSetGrid->SetCellValue(l_nGridRow, l_nGridCol, wxT(""));
			m_pcColorSetGrid->SetCellAlignment(l_nGridRow, l_nGridCol, wxALIGN_CENTER, wxALIGN_CENTER);
			m_pcColorSetGrid->SetCellBackgroundColour(l_nGridRow, l_nGridCol, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}

}

void SP_DLG_ConstantDefinition::AddNewChoice(wxCommandEvent& p_cEvent)
{
	wxString l_sNewValue = wxGetTextFromUser(wxT("Enter a new Group"), wxT("New Group"), wxT("default"));

	//no group with this name has been found
	if (!FindString(m_asGroups, l_sNewValue))
	{
		SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();

		SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sNewValue));

		if (!l_pcFunction->isVariable())
		{
			SP_MESSAGEBOX(wxT("group name '") + l_sNewValue + wxT("' is not valid"), wxT("Error"), wxOK | wxICON_ERROR);
			return;
		}
		//add this group name to the list
		m_asGroups.push_back(l_sNewValue);

		l_bWhite = false;
		for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
		{
			SP_WDG_GridCellChoiceEditor* l_pcCellEditor = dynamic_cast<SP_WDG_GridCellChoiceEditor*>(m_pcColorSetGrid->GetCellEditor(i, 1));
			
			l_pcCellEditor->AddItem(l_sNewValue);

			m_pcColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
	else
	{
		SP_MESSAGEBOX(wxT("group name '") + l_sNewValue + wxT("' already exists"), wxT("Error"), wxOK | wxICON_ERROR);

	}
}

bool SP_DLG_ConstantDefinition::FindString(wxArrayString& p_asStrArray, wxString& p_sValue)
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

bool SP_DLG_ConstantDefinition::CheckInput()
{
	map<std::string, std::string> l_mID2Val;
	//check user input values for parameters
	for (int l_nRow = 0; l_nRow < m_pcColorSetGrid->GetNumberRows(); l_nRow++)
	{
		for (int l_nValPos = 4; l_nValPos < m_pcColorSetGrid->GetNumberCols(); l_nValPos++)
		{
			bool l_bOk = false;
			wxString l_sValue = m_pcColorSetGrid->GetCellValue(l_nRow, l_nValPos);
			wxString l_sName = m_pcColorSetGrid->GetCellValue(l_nRow, 0);
			if (l_sValue.IsEmpty())
			{
				if (l_nValPos == 4)
				{
					wxString l_sMsg = wxT("Value set 'Main'");

					l_sMsg << wxT(" for '") << m_pcColorSetGrid->GetCellValue(l_nRow, 0);

					l_sMsg << wxT("' must not be empty!");

					SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

					return false;
				}
				else
				{
					continue;
				}
			}

			if (SP_Find(SP_KEYWORDLIST, l_sName) != SP_KEYWORDLIST.end())
			{
				SP_MESSAGEBOX(wxT(" A Keyword can not be a constant name! "), wxT("Check Constant"), wxOK | wxICON_ERROR);
				m_pcColorSetGrid->SetCellValue(l_nRow, 0, m_sOldCellValue);
				return false;
			}

			 
			SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
			 
			SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
			SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sValue));
			if (l_pcFunction)
			{
				SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

				std::set<std::string> l_Vars;
				l_pcExpanded->getVariables(l_Vars);

				if (l_Vars.empty())
				{
					l_mID2Val[l_sName] = l_sValue;
					l_bOk = true;

				}
				else {
					std::set<std::string>::iterator it = l_Vars.begin();

					// Iterate till the end of set
					int i = 0;
					while (i<l_Vars.size())
					{
						l_bOk = true;
						string l_s = *it;
						auto it = l_mID2Val.find(l_s);
						if (it == l_mID2Val.end())
						{
							l_bOk = false;
							break;
						}
					 
						i++;
					}
					if (l_bOk)
					{
						l_mID2Val[l_sName] = l_sValue;
					}
                   
				}
			}
		 
			
			if ( !l_bOk)
			{
				wxString l_sMsg = wxT("Invalid value for '");

				l_sMsg << m_pcColorSetGrid->GetCellValue(l_nRow, 0);

				l_sMsg << wxT("' in value set ");

				l_sMsg << m_pcColorSetGrid->GetColLabelValue(l_nValPos);

				SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

				return false; // a problem is found
			}
		}
	}

	return true; //no problems found
}


bool SP_DLG_ConstantDefinition::ExistConstant(const wxString& p_sName, const int row)
{

	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		if (i != row)
		{
			if (p_sName.IsSameAs(m_pcColorSetGrid->GetCellValue(i, NAME)))
			{
				return true;
			}
		}
	}

	return false;
}
void SP_DLG_ConstantDefinition::OnGridCellSelected(wxGridEvent& ev)
{
	ev.Skip();
}


void SP_DLG_ConstantDefinition::OnEditorShown(wxGridEvent& ev)
{
	m_sOldCellValue = m_pcColorSetGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();
}


bool SP_DLG_ConstantDefinition::ExistSetName(const wxString& p_sName)
{
	for (int i = 0; i < m_pcColorSetGrid->GetNumberCols(); i++)
	{
		if (p_sName == m_pcColorSetGrid->GetColLabelValue(i))
		{
			return true;
		}
	}
	return false;
}

void SP_DLG_ConstantDefinition::OnRenameSet(wxCommandEvent& p_cEvent)
{
	wxArrayInt l_pcSelCols = m_pcColorSetGrid->GetSelectedCols();

	if (l_pcSelCols.Count() != 1)
	{

		SP_MESSAGEBOX(wxT("Select a single column to rename."), wxT("Invalid column choice"), wxOK | wxICON_ERROR);
		return;

	}

	if (l_pcSelCols.Item(0) <= 4)
	{

		SP_MESSAGEBOX(wxT("This column cannot be renamed."), wxT("Invalid column choice"), wxOK | wxICON_ERROR);
		return;

	}

	wxTextEntryDialog l_pcDialog(this, wxT("New name of the value set: ") + m_pcColorSetGrid->GetColLabelValue(l_pcSelCols.Item(0)), wxT("Rename value set"));

	l_pcDialog.SetValue(m_pcColorSetGrid->GetColLabelValue(l_pcSelCols.Item(0)));

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

		m_pcColorSetGrid->SetColLabelValue(l_pcSelCols.Item(0), l_pcDialog.GetValue());

	}
}

void SP_DLG_ConstantDefinition::OnDeleteValueSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcColorSetGrid->GetSelectedCols();

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
				m_pcColorSetGrid->DeleteCols(2, l_cSelCols.Count() - 1, false);
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
			m_pcColorSetGrid->DeleteCols(1, l_cSelCols.Count() - 1, false);
			return;
		}
	}
	m_pcColorSetGrid->DeleteCols(l_cSelCols.Item(0), l_cSelCols.Count(), false);
}
bool SP_DLG_ConstantDefinition::DoCheckUserInput(const int& p_nRow)
{

	for (int l_nRow = 0; l_nRow < m_pcColorSetGrid->GetNumberRows(); l_nRow++)
	{
		if (l_nRow != p_nRow) continue;

		for (int l_nValPos = 4; l_nValPos < m_pcColorSetGrid->GetNumberCols(); l_nValPos++)
		{
			bool l_bOk = false;
			wxString l_sValue = m_pcColorSetGrid->GetCellValue(l_nRow, l_nValPos);
			wxString l_sName = m_pcColorSetGrid->GetCellValue(l_nRow, 0);
			wxString l_SType= m_pcColorSetGrid->GetCellValue(l_nRow, 2);
			if (l_sValue.IsEmpty())
			{
				if (l_nValPos == 4)
				{
					wxString l_sMsg = wxT("Value set 'Main'");

					l_sMsg << wxT(" for '") << m_pcColorSetGrid->GetCellValue(l_nRow, 0);

					l_sMsg << wxT("' must not be empty!");

					SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

					return false;
				}
				else
				{
					continue;
				}
			}


			SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

			SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
			SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sValue));
			if (l_pcFunction)
			{
				SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

				std::set<std::string> l_Vars;
				l_pcExpanded->getVariables(l_Vars);

				if (l_Vars.empty())
				{

					m_mID2Val[l_sName] = l_sValue;
					l_bOk = true;

				}
				else {
					std::set<std::string>::iterator it = l_Vars.begin();

					// Iterate till the end of set
					int i = 0;
					while (i<l_Vars.size())
					{
						l_bOk = true;
						string l_s = *it;
						wxString l_sValString(l_s.c_str(), wxConvUTF8);
						auto it = m_mID2Val.find(l_sValString);
						if (it == m_mID2Val.end())
						{
							l_bOk = false;
							break;
						}

						i++;
					}
					if (l_bOk)
					{
						m_mID2Val[l_sName] = l_sValue;
					}

				}
			}

			if(l_SType==wxT("int")&& l_sValue.Contains(wxT(".")))
			{
				l_bOk = false;
			}

			if (!l_bOk)
			{
				wxString l_sMsg = wxT("Invalid value for '");

				l_sMsg << m_pcColorSetGrid->GetCellValue(l_nRow, 0);

				l_sMsg << wxT("' in value set ");

				l_sMsg << m_pcColorSetGrid->GetColLabelValue(l_nValPos);

				SP_MESSAGEBOX(l_sMsg, wxT("Error"), wxOK | wxICON_ERROR);

				return false; // a problem is found
			}
		}
	}

	return true; //no problems found
}



void SP_DLG_ConstantDefinition::MoveRow(long p_nToPos)
{
	if (p_nToPos <=0) return;

	int l_nToRow = 0;
	l_nToRow = p_nToPos;
	l_nToRow = l_nToRow - 1;

	m_RowData.clear();
	m_RowData.Alloc(m_pcColorSetGrid->GetNumberCols());
	for (unsigned i = 0; i < m_pcColorSetGrid->GetNumberCols(); i++)
	{
		m_RowData.Add(m_pcColorSetGrid->GetCellValue(m_norgRow, i));

	}

	wxColour l_OrgColour = m_pcColorSetGrid->GetCellBackgroundColour(m_norgRow, 1);
	wxColour l_ToColour = m_pcColorSetGrid->GetCellBackgroundColour(l_nToRow, 1);
	m_pcColorSetGrid->DeleteRows(m_norgRow);
	int l_nfrom;
	int l_nto;
	if (m_pcColorSetGrid->GetNumberRows() == l_nToRow)
	{
		//last row
		m_pcColorSetGrid->AppendRows(1);
		l_nfrom = m_norgRow;
		l_nto = m_pcColorSetGrid->GetNumberRows() - 1;
	}
	else if (l_nToRow == 0)
	{
		m_pcColorSetGrid->InsertRows(0, 1);


	}
	else
	{
		m_pcColorSetGrid->InsertRows(l_nToRow - 1, 1);
		for (unsigned i = 0; i <m_pcColorSetGrid->GetNumberCols(); i++)
		{
			m_pcColorSetGrid->SetCellValue(l_nToRow - 1, i, m_pcColorSetGrid->GetCellValue(l_nToRow, i));
			m_pcColorSetGrid->SetCellAlignment(l_nToRow - 1, i, wxALIGN_CENTER, wxALIGN_TOP);

		}
	}

	for (unsigned i = 0; i <m_pcColorSetGrid->GetNumberCols(); i++)
	{
		m_pcColorSetGrid->SetCellValue(l_nToRow, i, m_RowData[i]);
		m_pcColorSetGrid->SetCellAlignment(l_nToRow, i, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcColorSetGrid->SetCellBackgroundColour(l_nToRow, i, l_ToColour);// (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
	}


	bool l_bISWhite = false;
	for (unsigned i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		for (unsigned j = 0; j < m_pcColorSetGrid->GetNumberCols(); j++)
		{
			m_pcColorSetGrid->SetCellBackgroundColour(i, j, (l_bISWhite ? *wxWHITE : *wxLIGHT_GREY));

		}
		l_bISWhite = !l_bISWhite;
	}
}



void SP_DLG_ConstantDefinition::OnRowRightClick(wxGridEvent& event)
{

	unsigned row;
	row= event.GetRow();
	wxString l_sRow;
	row++;

	wxString l_sMsg = wxT("Please select the target constant position, to which you want to move the current constant.\n");

	wxArrayString l_Choices;

	 
	m_norgRow = row - 1;
	for (unsigned i = 1; i <= m_pcColorSetGrid->GetNumberRows(); i++)
	{
		if (i == row) continue;

		wxString l_sOption;
		l_sOption <<i;

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


void SP_DLG_ConstantDefinition::OnSortVsets(wxCommandEvent& p_cEvent)
{
	std::vector<double> l_vValues;
    std:vector<string> l_vExpressions;
	if (m_bIsAsc)
	{
		m_bIsAsc = false;
		m_pcSortingButton->SetLabelText(_T("Sort Value Sets (Desc.)"));

		for (unsigned int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
		{
			std::multimap<std::string, float> l_mVset2Val;
			for (unsigned j = 4; j < m_pcColorSetGrid->GetNumberCols(); j++)
			{


				if (m_pcColorSetGrid->GetCellValue(i, j).ToStdString() == "")
				{
					string l_sTemp = "";
					//continue;

				}

				double l_dval;
				bool l_bIsEvaluated = EvalConstantExpression(m_pcColorSetGrid->GetCellValue(i, j), l_dval);


				if (m_pcColorSetGrid->GetCellValue(i, j).ToDouble(&l_dval))
				{

				    std::string l_svalue = m_pcColorSetGrid->GetCellValue(i, j).ToStdString();
		            //this avoids converting e.g., 0.000045 to the scientific notation
					std::stringstream l_sStream;
					l_sStream.precision(15);
					l_sStream<<l_svalue<<std::fixed;
					double l_dval;
					l_sStream >> l_dval;
					l_vValues.push_back(l_dval);
				}
				else
				{
					l_vExpressions.push_back(m_pcColorSetGrid->GetCellValue(i, j).ToStdString());
				}

			}

			std::sort(l_vValues.begin(), l_vValues.end());


			int l_nLastPos = 0;

			//value of the constant
			for (unsigned int j = 0; j < l_vExpressions.size(); j++)
			{
				if (l_vExpressions[j] == "") {  continue; }


				m_pcColorSetGrid->SetCellValue(i, j + 4, l_vExpressions[j]);
				m_pcColorSetGrid->SetCellAlignment(i, j + 4, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcColorSetGrid->SetCellBackgroundColour(i, j + 4, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
				l_nLastPos = j + 4;

			}

			if (l_vExpressions.size() > 0)
			{
				if (l_nLastPos < 4)
				{
					l_nLastPos = 4;
				}
				else
				{
					l_nLastPos++;
				}
			}
			else
			{
				l_nLastPos = 4;
			}

			//value of the constant
			for (int j=0; j < l_vValues.size(); j++)
			{
				wxString l_sVal;
				l_sVal << l_vValues[j];
				m_pcColorSetGrid->SetCellValue(i, l_nLastPos+j, l_sVal);
				m_pcColorSetGrid->SetCellAlignment(i, l_nLastPos + j, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcColorSetGrid->SetCellBackgroundColour(i, l_nLastPos + j, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			}

			l_vValues.clear();
			l_vExpressions.clear();
		}
	}
	else
	{
		m_bIsAsc = true;
		m_pcSortingButton->SetLabelText(_T("Sort Value Sets (Asc.)"));

		for (unsigned int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
		{
			std::multimap<std::string, float> l_mVset2Val;
			for (unsigned j = 4; j < m_pcColorSetGrid->GetNumberCols(); j++)
			{


				if (m_pcColorSetGrid->GetCellValue(i, j).ToStdString() == "")
				{
					string l_sTemp = "";
					//continue;

				}

				double l_dval;

				if (m_pcColorSetGrid->GetCellValue(i, j).ToDouble(&l_dval))
				{
					std::string l_svalue = m_pcColorSetGrid->GetCellValue(i, j).ToStdString();
					std::stringstream l_sStream;
					l_sStream.precision(15);
					l_sStream << l_svalue << std::fixed;
					double l_dval;
					l_sStream >> l_dval;
					l_vValues.push_back(l_dval);

				}
				else
				{
					l_vExpressions.push_back(m_pcColorSetGrid->GetCellValue(i, j).ToStdString());
				}

			}

			std::sort(l_vValues.begin(), l_vValues.end());
			std::reverse(l_vValues.begin(), l_vValues.end());

			int l_nLastPos = 0;

			//value of the constant
			for (unsigned int j = 0; j < l_vExpressions.size(); j++)
			{
				if (l_vExpressions[j] == "") {   continue; }
				m_pcColorSetGrid->SetCellValue(i, j + 4, l_vExpressions[j]);
				m_pcColorSetGrid->SetCellAlignment(i, j + 4, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcColorSetGrid->SetCellBackgroundColour(i, j + 4, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
				l_nLastPos = j + 4;
			}

			if (l_vExpressions.size() > 0)
			{
				if (l_nLastPos < 4)
				{
					l_nLastPos = 4;
				}
				else
				{
					l_nLastPos++;
				}
			}
			else
			{
				l_nLastPos = 4;
			}

			//value of the constant
			for (int j=0; j < l_vValues.size(); j++)
			{
				wxString l_sVal;
				l_sVal << l_vValues[j];
				m_pcColorSetGrid->SetCellValue(i, l_nLastPos + j, l_sVal);
				m_pcColorSetGrid->SetCellAlignment(i, l_nLastPos + j, wxALIGN_CENTER, wxALIGN_TOP);
				m_pcColorSetGrid->SetCellBackgroundColour(i, l_nLastPos + j, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
			}

			l_vValues.clear();
			l_vExpressions.clear();
		}
	}
	
	//recover background colour of rows
	bool l_bISWhite = false;
	for (unsigned i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		for (unsigned j = 0; j < m_pcColorSetGrid->GetNumberCols(); j++)
		{
			m_pcColorSetGrid->SetCellBackgroundColour(i, j, (l_bISWhite ? *wxWHITE : *wxLIGHT_GREY));

		}
		l_bISWhite = !l_bISWhite;
	}
}




bool SP_DLG_ConstantDefinition::EvalConstantExpression(const wxString& p_sArcWeight, double& p_dVal)
{
	double dValue = 0.0;
	std::string strValue = p_sArcWeight;
	SP_DS_Graph* l_pcGraph=SP_Core ::Instance()->GetRootDocument()->GetGraph();
	  
	SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
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

void SP_DLG_ConstantDefinition::OnMultiColSorting(wxCommandEvent&  event)
{
	SP_Vector2DString l_vvGrid;
	l_vvGrid.resize(m_pcColorSetGrid->GetNumberRows(), std::vector<wxString>(m_pcColorSetGrid->GetNumberCols(), wxT("")));
	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		SP_VectorString l_vRow;
		for (int j = 0; j < m_pcColorSetGrid->GetNumberCols(); j++)
		{
			l_vvGrid[i][j] = m_pcColorSetGrid->GetCellValue(i, j);

		}
	}

	std::map<wxString, int> l_mKey2Pos;


	SP_VectorString l_vColLabels;
	for (int j = 0; j < 3; j++)
	{

		l_vColLabels.push_back(m_pcColorSetGrid->GetColLabelValue(j));
		l_mKey2Pos[m_pcColorSetGrid->GetColLabelValue(j)] = j;
	}


		SP_DLG_SortDialoug* l_pcDialog = new SP_DLG_SortDialoug(this, l_vColLabels, l_mKey2Pos ,l_vvGrid,m_lMain);
		if (l_pcDialog->ShowModal())
		{
			l_vvGrid = l_pcDialog->GetSortedTable();

			for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
			{
				SP_VectorString l_vRow;
				for (int j = 0; j < m_pcColorSetGrid->GetNumberCols(); j++)
				{
					m_pcColorSetGrid->SetCellValue(i,j, l_vvGrid[i][j])  ;

				}
			}
		}
		l_pcDialog->Destroy();

}


void SP_DLG_ConstantDefinition::DeleteGroup(wxCommandEvent& p_cEvent)
{
	wxString l_sMsg = wxT("Please select a group to be deleted.\n");

	wxArrayString l_Choices;

	int l_nSize=0;


	for (unsigned i = 0; i < m_asGroups.GetCount(); i++)
	{
			if (m_asGroups[i] == wxT("")||m_asGroups[i] == wxT("all") || m_asGroups[i] == wxT("parameter") ||
				m_asGroups[i] == wxT("marking") || m_asGroups[i] == wxT("coloring")) continue;

			l_nSize++;
	}

	if(l_nSize==0) return ;

	l_Choices.Alloc(l_nSize);

	for (unsigned i = 0; i < m_asGroups.GetCount(); i++)
	{
		if (m_asGroups[i] == wxT("")||m_asGroups[i] == wxT("all") || m_asGroups[i] == wxT("parameter") ||
			m_asGroups[i] == wxT("marking") || m_asGroups[i] == wxT("coloring")) continue;

		l_Choices.Add(m_asGroups[i]);
	}



	int l_ReturnChoice = ::wxGetSingleChoiceIndex(l_sMsg, wxT("Delete constant."), l_Choices);



	if (l_ReturnChoice == -1) return;

	wxString l_sCohesnGroup = l_Choices[l_ReturnChoice];

	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		if (m_pcColorSetGrid->GetCellValue(i, GROUP) == l_sCohesnGroup)
		{
			SP_MESSAGEBOX(wxT("The selected group cannot be deleted as it is already used"), wxT("Error"), wxOK | wxICON_ERROR);
			return;
		}
	}

	m_asGroups.Remove(l_sCohesnGroup);
	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		m_pcColorSetGrid->SetCellEditor(i, GROUP, new SP_WDG_GridCellChoiceEditor(m_asGroups));
	}
	m_pcColorSetGrid->Refresh();

}
