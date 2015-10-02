//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_CPN/SP_DLG_VariableDefinition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"



IMPLEMENT_CLASS( SP_DLG_VariableDefinition, wxDialog )

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING
};

BEGIN_EVENT_TABLE( SP_DLG_VariableDefinition, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_VariableDefinition::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_VariableDefinition::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_VariableDefinition::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_VariableDefinition::OnDeleteColorSet )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_VariableDefinition::OnCheckFunction )
EVT_BUTTON( wxID_APPLY, SP_DLG_VariableDefinition::OnDlgApply )
#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_VariableDefinition::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_VariableDefinition::OnGridCellValueChanged ) 
#endif



END_EVENT_TABLE()



SP_DLG_VariableDefinition::SP_DLG_VariableDefinition( wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row
	m_pcColorSetGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 700, 300 ), wxSUNKEN_BORDER );
	m_pcColorSetGrid->CreateGrid( 0, 0);
	m_pcColorSetGrid->EnableEditing( true);	

	m_pcColorSetGrid->SetDefaultColSize( 220, TRUE);
	m_pcColorSetGrid->SetDefaultRowSize( 20, TRUE);

	m_pcColorSetGrid->SetColLabelSize( 16);
	m_pcColorSetGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer->Add(m_pcColorSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcColorSetGrid->EnableEditing(true);
	m_pcColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	

	m_pcColorSetGrid->AppendCols(3);

	m_pcColorSetGrid->SetColLabelValue(0, wxT("Name") );
	m_pcColorSetGrid->SetColSize(0, 100);

	m_pcColorSetGrid->SetColLabelValue(1, wxT("Color Set") );
	m_pcColorSetGrid->SetColSize(1, 100);

	m_pcColorSetGrid->SetColLabelValue(2, wxT("Comment") );
	m_pcColorSetGrid->SetColSize(2, 300);


	LoadData();

    SP_AutoSizeRowLabelSize(m_pcColorSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL );

	wxSizer *l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	

	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add variable") ), 1,wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete variable") ), 1, wxALL, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check variables") ), 1, wxALL, 5);

	wxStdDialogButtonSizer* l_pcBottomButtonSizer = CreateStdDialogButtonSizer(wxOK|wxCANCEL|wxAPPLY);

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_RIGHT);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();

}

void SP_DLG_VariableDefinition::OnDlgApply( wxCommandEvent& p_cEvent )
{	
	SaveData();
}


void SP_DLG_VariableDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	//SP_MDI_Doc* l_pcDoc =SP_Core::Instance()->GetRootDocument();

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{

			if (SaveData())
			{
				l_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
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

void SP_DLG_VariableDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_VariableDefinition::OnEditColorSet( wxCommandEvent& p_cEvent )
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

void SP_DLG_VariableDefinition::OnDeleteColorSet( wxCommandEvent& p_cEvent )
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
            if ( m_pcColorSetGrid->IsInSelection( n , 0 ) )
            	m_pcColorSetGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        m_pcColorSetGrid->EndBatch();
    }
}

void SP_DLG_VariableDefinition::OnAddColorSet( wxCommandEvent& p_cEvent )
{

	m_pcColorSetGrid->AppendRows(1);

	InitializeChoice();

	int l_nEditRowPos = m_pcColorSetGrid->GetNumberRows()-1;	
	m_pcColorSetGrid->SetCellEditor(l_nEditRowPos,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
	m_pcColorSetGrid->SetCellSize(l_nEditRowPos, 1, 1, 1);
	m_pcColorSetGrid->SetCellValue(l_nEditRowPos, 1, wxT(""));	
	m_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 1, false);
}


bool SP_DLG_VariableDefinition::LoadData()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());//l_pcMetadataclass->NewElement(1);
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));		

	bool l_bWhite = false;

	if (m_pcColorSetGrid->GetNumberRows() > 0)
	{
		m_pcColorSetGrid->DeleteRows(0, m_pcColorSetGrid->GetNumberRows());
	}

	m_pcColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	InitializeChoice();

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcColorSetGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		m_pcColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcColorSetGrid->SetCellSize(i, 1, 1, 1);
		m_pcColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));

		m_pcColorSetGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,2).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 2, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		(l_bWhite ? l_bWhite = false : l_bWhite = true);

	}
	
	delete [] choices;
	return true;

}

bool SP_DLG_VariableDefinition::SaveData()
{
	m_pcColorSetGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	l_pcColList->Clear();

	for (int i = 0; i < m_pcColorSetGrid->GetNumberRows(); i++)
	{
		int l_nRow=l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nRow, 0, m_pcColorSetGrid->GetCellValue(i, 0));		
		l_pcColList->SetCell(l_nRow, 1, m_pcColorSetGrid->GetCellValue(i, 1));		
		l_pcColList->SetCell(l_nRow, 2, m_pcColorSetGrid->GetCellValue(i, 2));		
	}

	return true;

}


void SP_DLG_VariableDefinition::InitializeChoice()
{
	
	unsigned l_nArraysize;
	l_nArraysize = 0;

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute *l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();
	
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));		
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();

	
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));		
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();

	m_choices.Alloc(l_nArraysize );
	//m_choices.Alloc(l_nArraysize + 9);
	m_choices.Empty();
/*
	m_choices.Add(wxT("----Basic types----"));
	m_choices.Add(wxT("string"));
	m_choices.Add(wxT("int"));
	m_choices.Add(wxT("bool"));
	m_choices.Add(wxT("enum"));
	m_choices.Add(wxT("index"));
	m_choices.Add(wxT("----Structured types----"));
	m_choices.Add(wxT("product"));
	m_choices.Add(wxT("union"));

	m_choices.Add(wxT("----User-defining colorsets----"));
*/
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		
		wxString s = l_pcColList->GetCell(i,0).c_str();
		m_choices.Add(l_pcColList->GetCell(i,0).c_str());
	}

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		
		m_choices.Add(l_pcColList->GetCell(i,0).c_str());
	}

	
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_choices.Add(l_pcColList->GetCell(i,0).c_str());
	}

	size_t count = m_choices.GetCount();
	choices = new wxString[count];
	for ( size_t n = 0; n < count; n++ )
	{
		
		choices[n] = m_choices[n];
		wxString s = choices[n];

	}

}


void SP_DLG_VariableDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{
	
	int l_nRow = p_gEvent.GetRow();

	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(p_gEvent.GetEventObject());	

	wxString l_sDataType = l_pcGrid->GetCellValue(l_nRow,1);	
	
	if(l_sDataType == wxT("----Basic types----") || l_sDataType == wxT("----Structured types----") || l_sDataType == wxT("----User-defined colorsets----"))
	{
		SP_MESSAGEBOX(wxT("Please select data type"), wxT("Variable checking"), wxOK | wxICON_ERROR);
		return;
	}
}

void SP_DLG_VariableDefinition::OnCheckFunction( wxCommandEvent& p_cEvent )
{
	
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	if( ! l_cValueAssign.InitializeColorset(l_cColorSetClass) )
		return;
	

	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The variable definition");	
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(this, l_sVariables, 1000);

}
