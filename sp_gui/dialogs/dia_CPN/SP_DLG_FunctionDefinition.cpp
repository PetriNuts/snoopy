//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/11/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

#include "sp_gui/dialogs/dia_CPN/SP_DLG_FunctionDefinition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_FuncParaDecl.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_FunctionAssistent.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"

IMPLEMENT_CLASS( SP_DLG_FunctionDefinition, wxDialog )

enum
{
	SHOW = 0,
	TYPE,
	NAME,
	PARAMETER,
	BODY,
	COMMENT
};

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_ASSISTENT
};


BEGIN_EVENT_TABLE( SP_DLG_FunctionDefinition, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_FunctionDefinition::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_FunctionDefinition::OnDlgCancel )
EVT_BUTTON( wxID_APPLY, SP_DLG_FunctionDefinition::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_FunctionDefinition::OnAddFunction )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_FunctionDefinition::OnDeleteFunction )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_FunctionDefinition::OnCheckFunction )
#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_FunctionDefinition::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_FunctionDefinition::OnGridCellValueChanged ) 
#endif


EVT_GRID_CELL_LEFT_CLICK( SP_DLG_FunctionDefinition::OnEditorShown )

EVT_BUTTON( SP_ID_BUTTON_ASSISTENT, SP_DLG_FunctionDefinition::OnAssistent )


END_EVENT_TABLE()



SP_DLG_FunctionDefinition::SP_DLG_FunctionDefinition( wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
	m_pcParent = p_pcParent;

	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_bIsColored = m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored"));

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* l_pcGridSizer = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("") ), wxVERTICAL );

	// new row
	m_pcGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 700, 300 ), wxSUNKEN_BORDER );
	m_pcGrid->CreateGrid( 0, 0);
	m_pcGrid->EnableEditing( true);

	m_pcGrid->SetDefaultColSize( 220, TRUE);
	m_pcGrid->SetDefaultRowSize( 20, TRUE);

	m_pcGrid->SetColLabelSize( 16);
	m_pcGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer->Add(m_pcGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcGrid->EnableEditing(true);
	m_pcGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcGrid->AppendCols(6);

	if(m_bIsColored)
	{
		m_pcGrid->HideCol(SHOW);

		m_pcGrid->SetColLabelValue(TYPE, wxT("Return Type") );
		m_pcGrid->SetColSize(TYPE, 100);
	}
	else
	{
		m_pcGrid->SetColLabelValue(SHOW, wxT("Show") );
		m_pcGrid->SetColSize(SHOW, 50);

#if wxABI_VERSION > 30000
		m_pcGrid->HideCol(TYPE);
#else
		m_pcGrid->SetColLabelValue(TYPE, wxT(" ") );
		m_pcGrid->SetColSize(TYPE,0);
#endif
	}

	m_pcGrid->SetColLabelValue(NAME, wxT("Function Name") );
	m_pcGrid->SetColSize(NAME, 150);

	m_pcGrid->SetColLabelValue(PARAMETER, wxT("Parameter List") );
	m_pcGrid->SetColSize(PARAMETER, 150);

	m_pcGrid->SetColLabelValue(BODY, wxT("Function Body") );
	m_pcGrid->SetColSize(BODY, 300);

	m_pcGrid->SetColLabelValue(COMMENT, wxT("Comment") );
	m_pcGrid->SetColSize(COMMENT, 100);

	LoadData();

    SP_AutoSizeRowLabelSize(m_pcGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL );

	wxStaticBox *l_pcBottomButtonBox1 = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcSizer = new wxStaticBoxSizer( l_pcBottomButtonBox1, wxHORIZONTAL );
	
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add function") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);	
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete function") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check functions") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ASSISTENT, wxT("Function assistent") ), 1, wxALL, 5);
	
#if wxABI_VERSION < 30000
	wxStaticBox *l_pcBottomButtonBox = new wxStaticBox( this, -1, wxT("") );
	wxSizer *l_pcBottomButtonSizer = new wxStaticBoxSizer( l_pcBottomButtonBox, wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	l_pcBottomButtonSizer->Add(new wxButton(this,wxID_APPLY,wxT("Apply")), 0, wxALL, 5);
#else
    wxStdDialogButtonSizer* l_pcBottomButtonSizer = CreateStdDialogButtonSizer(wxOK|wxCANCEL|wxAPPLY);
#endif
	
	wxBoxSizer* l_pcSpaceSizer = new wxBoxSizer(wxHORIZONTAL );
	l_pcSpaceSizer->Add(30,0,0);	
	

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_CENTER);
	l_pcButtonSizer->Add(l_pcSpaceSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_LEFT);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_CENTER);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();
}

void SP_DLG_FunctionDefinition::OnDlgApply( wxCommandEvent& p_cEvent )
{	
	if(DoCheckFunction())
	{
		SaveData();
	}
}


void SP_DLG_FunctionDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow())
	{
		if(DoCheckFunction())
		{
			if (SaveData())
			{
				if(m_bIsColored)
				{
					m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
				}
				else
				{
					m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();
				}
				if (IsModal())
				{
					SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					EndModal(wxID_OK);
				}
				else
				{
					SetReturnCode(wxID_OK);
					this->Show(FALSE);
				}
			}
		}
	}
}

void SP_DLG_FunctionDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_FunctionDefinition::OnEditFunction( wxCommandEvent& p_cEvent )
{
	if (m_pcGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcGrid->GetCellValue(l_nEditRowPos, NAME);

	m_pcGrid->SelectBlock(l_nEditRowPos, NAME, l_nEditRowPos, NAME);
	m_pcGrid->SetFocus();
}

void SP_DLG_FunctionDefinition::OnDeleteFunction( wxCommandEvent& p_cEvent )
{
	if (m_pcGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcGrid->GetGridCursorRow();
	m_pcGrid->DeleteRows(l_nEditRowPos);
}

void SP_DLG_FunctionDefinition::OnAddFunction( wxCommandEvent& p_cEvent )
{
	m_pcGrid->AppendRows(1);
	int l_nEditRowPos = m_pcGrid->GetNumberRows()-1;

	//show the function
	auto l_pcBoolEditor = new wxGridCellBoolEditor();
	l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
	m_pcGrid->SetCellEditor(l_nEditRowPos, SHOW, l_pcBoolEditor);
	m_pcGrid->SetCellRenderer(l_nEditRowPos, SHOW, new wxGridCellBoolRenderer());
	m_pcGrid->SetCellValue(l_nEditRowPos, SHOW, wxT("0"));
	m_pcGrid->SetCellAlignment(l_nEditRowPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);

	InitializeChoice();
	m_pcGrid->SetCellEditor(l_nEditRowPos,TYPE,new wxGridCellChoiceEditor(m_choices));
	m_pcGrid->SetCellSize(l_nEditRowPos, TYPE, 1, 1);
	m_pcGrid->SetCellOverflow(l_nEditRowPos, TYPE, false);

	if(m_pcGrid->GetNumberRows() == 0 )
	{
		m_pcGrid->SetCellValue(l_nEditRowPos, TYPE, wxT("int"));
		return;
	}

	//copy the last row
	if(l_nEditRowPos > 0)
	{
		int l_nSelRowPos = m_pcGrid->GetGridCursorRow();
		if(l_nSelRowPos < 0 || l_nSelRowPos >= m_pcGrid->GetNumberRows() )
		{
			l_nSelRowPos = m_pcGrid->GetNumberRows()-1;
		}
	
		m_pcGrid->SetCellValue(l_nEditRowPos,TYPE,m_pcGrid->GetCellValue(l_nSelRowPos, TYPE));
		m_pcGrid->SetCellValue(l_nEditRowPos,NAME,m_pcGrid->GetCellValue(l_nSelRowPos, NAME)+wxT("_copy"));
		m_pcGrid->SetCellValue(l_nEditRowPos,PARAMETER,m_pcGrid->GetCellValue(l_nSelRowPos, PARAMETER));
		m_pcGrid->SetCellValue(l_nEditRowPos,BODY,m_pcGrid->GetCellValue(l_nSelRowPos, BODY));
	}
}


bool SP_DLG_FunctionDefinition::LoadData()
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);

	if(m_bIsColored)
	{
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));

		bool l_bWhite = false;

		if (m_pcGrid->GetNumberRows() > 0)
		{
			m_pcGrid->DeleteRows(0, m_pcGrid->GetNumberRows());
		}

		m_pcGrid->AppendRows(l_pcColList->GetRowCount());

		InitializeChoice();

		m_BackupData.clear();
		m_BackupData.assign(l_pcColList->GetRowCount(), SP_VectorString(l_pcColList->GetColCount(), wxEmptyString));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sType = l_pcColList->GetCell(i,0);
			m_BackupData[i][0] = l_sType;
			m_pcGrid->SetCellEditor(i,TYPE,new wxGridCellChoiceEditor(m_choices));
			m_pcGrid->SetCellSize(i, TYPE, 1, 1);
			m_pcGrid->SetCellValue(i, TYPE, l_sType);
			m_pcGrid->SetCellAlignment(i, TYPE, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, TYPE, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));

			wxString l_sName = l_pcColList->GetCell(i,1);
			m_BackupData[i][1] = l_sName;
			m_pcGrid->SetCellValue(i, NAME, l_sName);
			m_pcGrid->SetCellAlignment(i, NAME, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, NAME, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			wxString l_sParameter = l_pcColList->GetCell(i,2);
			m_BackupData[i][2] = l_sParameter;
			m_pcGrid->SetCellValue(i, PARAMETER, l_sParameter);
			m_pcGrid->SetCellAlignment(i, PARAMETER, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, PARAMETER, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			wxString l_sBody = l_pcColList->GetCell(i,3);
			m_BackupData[i][3] = l_sBody;
			m_pcGrid->SetCellValue(i, BODY, l_sBody);
			m_pcGrid->SetCellAlignment(i, BODY, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, BODY, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			wxString l_sComment = l_pcColList->GetCell(i,4);
			m_BackupData[i][5] = l_sComment;
			m_pcGrid->SetCellValue(i, COMMENT, l_sComment);
			m_pcGrid->SetCellAlignment(i, COMMENT, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, COMMENT, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			(l_bWhite ? l_bWhite = false : l_bWhite = true);
		}
	}
	else
	{
		bool l_bWhite = false;

		if (m_pcGrid->GetNumberRows() > 0)
		{
			m_pcGrid->DeleteRows(0, m_pcGrid->GetNumberRows());
		}
		m_pcGrid->AppendRows(l_pcMetadataclass->GetElements()->size());

		unsigned i = 0 ;
		for(SP_DS_Metadata* l_pcMetadata : *l_pcMetadataclass->GetElements())
		{
			//show the function
			wxString l_sMetadataShow = l_pcMetadata->GetShow() ? wxT("1") : wxT("0");
			auto l_pcBoolEditor = new wxGridCellBoolEditor();
			l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
			m_pcGrid->SetCellEditor(i, SHOW, l_pcBoolEditor);
			m_pcGrid->SetCellRenderer(i, SHOW, new wxGridCellBoolRenderer());
			m_pcGrid->SetCellValue(i, SHOW, l_sMetadataShow);
			m_pcGrid->SetCellAlignment(i, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
			m_pcGrid->SetCellBackgroundColour(i, SHOW, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcGrid->SetCellEditor(i,TYPE,new wxGridCellChoiceEditor(m_choices));
			//m_pcGrid->SetCellSize(i, TYPE, 1, 1);
			m_pcGrid->SetCellValue(i, TYPE, l_pcMetadata->GetAttribute(wxT("Return"))->GetValueString());
			m_pcGrid->SetCellAlignment(i, TYPE, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, TYPE, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcGrid->SetCellValue(i, NAME, l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString());
			m_pcGrid->SetCellAlignment(i, NAME, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, NAME, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcGrid->SetCellValue(i, PARAMETER, l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString());
			m_pcGrid->SetCellAlignment(i, PARAMETER, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, PARAMETER, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcGrid->SetCellValue(i, BODY, l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString());
			m_pcGrid->SetCellAlignment(i, BODY, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, BODY, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			m_pcGrid->SetCellValue(i, COMMENT, l_pcMetadata->GetAttribute(wxT("Comment"))->GetValueString());
			m_pcGrid->SetCellAlignment(i, COMMENT, wxALIGN_LEFT, wxALIGN_CENTER);
			m_pcGrid->SetCellBackgroundColour(i, COMMENT, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

			(l_bWhite ? l_bWhite = false : l_bWhite = true);
			++i;
		}
	}
	return true;

}

bool SP_DLG_FunctionDefinition::SaveData()
{
	m_pcGrid->SaveEditControlValue();

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);

	if(m_bIsColored)
	{
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		l_pcColList->Clear();

		for (int i = 0; i < m_pcGrid->GetNumberRows(); i++)
		{
			int l_nRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nRow, 0, m_pcGrid->GetCellValue(i, TYPE));
			l_pcColList->SetCell(l_nRow, 1, m_pcGrid->GetCellValue(i, NAME));
			l_pcColList->SetCell(l_nRow, 2, m_pcGrid->GetCellValue(i, PARAMETER));
			l_pcColList->SetCell(l_nRow, 3, m_pcGrid->GetCellValue(i, BODY));
			l_pcColList->SetCell(l_nRow, 4, m_pcGrid->GetCellValue(i, COMMENT));
		}
	}
	else
	{
		SP_ListMetadata::const_iterator it = l_pcMetadataclass->GetElements()->begin();
		SP_DS_Metadata* l_pcMetadata;
		for (int i = 0; i < m_pcGrid->GetNumberRows(); ++i)
		{
			if(i < l_pcMetadataclass->GetElements()->size() )
			{
				l_pcMetadata = *it;
				++it;
			}
			else
			{
				l_pcMetadata = l_pcMetadataclass->NewElement(1);
			}
			//l_pcMetadata->GetAttribute(wxT("Return"))->SetValueString(m_pcGrid->GetCellValue(i, TYPE));
			l_pcMetadata->GetAttribute(wxT("Name"))->SetValueString(m_pcGrid->GetCellValue(i, NAME));
			l_pcMetadata->GetAttribute(wxT("Parameter"))->SetValueString(m_pcGrid->GetCellValue(i, PARAMETER));
			l_pcMetadata->GetAttribute(wxT("Body"))->SetValueString(m_pcGrid->GetCellValue(i, BODY));
			l_pcMetadata->GetAttribute(wxT("Comment"))->SetValueString(m_pcGrid->GetCellValue(i, COMMENT));

			SP_FunctionPtr l_pcSig = m_pcGraph->GetFunctionRegistry()->parseFunctionString(
					m_pcGrid->GetCellValue(i, NAME) + wxT("(") + m_pcGrid->GetCellValue(i, PARAMETER) + wxT(")"));
			SP_FunctionPtr l_pcDef = m_pcGraph->GetFunctionRegistry()->parseFunctionString(m_pcGrid->GetCellValue(i, BODY));
			//wxString l_sSig(l_pcSig->toString().c_str(), wxConvUTF8);
			//wxString l_sDef(l_pcDef->toString().c_str(), wxConvUTF8);
			//SP_LOGMESSAGE( wxT("Signature: ") + l_sSig);
			//SP_LOGMESSAGE( wxT("Definition: ") + l_sDef);
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_pcSig, l_pcDef);

			bool l_bShow = m_pcGrid->GetCellValue(i, SHOW) == wxT("1") ? true : false;
			l_pcMetadata->SetShow(l_bShow);
			for(SP_DS_Attribute* l_pcAttr : *l_pcMetadata->GetAttributes())
			{
				l_pcAttr->SetShow(l_bShow);
			}
			l_pcMetadata->Update();
		}
		m_pcGraph->Update();
	}
	return true;

}

void SP_DLG_FunctionDefinition::RestoreBackup()
{
	if(m_bIsColored)
	{
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		l_pcColList->Clear();

		for (int i = 0; i < m_BackupData.size(); i++)
		{
			int l_nRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nRow, 0, m_BackupData[i][0]);
			l_pcColList->SetCell(l_nRow, 1, m_BackupData[i][1]);
			l_pcColList->SetCell(l_nRow, 2, m_BackupData[i][2]);
			l_pcColList->SetCell(l_nRow, 3, m_BackupData[i][3]);
			l_pcColList->SetCell(l_nRow, 4, m_BackupData[i][4]);
		}
	}
}

void SP_DLG_FunctionDefinition::InitializeChoice()
{
	if(m_bIsColored)
	{
		unsigned l_nArraysize;
		l_nArraysize = 0;

		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute *l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
		l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();

		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
		l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();


		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
		l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();
	

		m_choices.Alloc(l_nArraysize + 10);
		m_choices.Empty();

		m_choices.Add(wxT("----Basic types----"));
		m_choices.Add(wxT("string"));
		m_choices.Add(wxT("int"));
		m_choices.Add(wxT("bool"));
		m_choices.Add(wxT("enum"));
		m_choices.Add(wxT("index"));
		m_choices.Add(wxT("----Structured types----"));
		m_choices.Add(wxT("product"));
		m_choices.Add(wxT("union"));

		m_choices.Add(wxT("----User-defined color sets----"));
	
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{

			wxString s = l_pcColList->GetCell(i,0).c_str();
			m_choices.Add(l_pcColList->GetCell(i,0).c_str());
		}
	
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{

			m_choices.Add(l_pcColList->GetCell(i,0).c_str());
		}
	
		
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_choices.Add(l_pcColList->GetCell(i,0).c_str());
		}
	}
	else
	{
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
		SP_DS_Metadata* l_pcMetadata = mc->GetPrototype();
		set<wxString> l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Return")))->GetPossibleValues();

		m_choices.Empty();
		set<wxString>::iterator it;
		for (it = l_sType.begin(); it != l_sType.end(); it++)
		{
			m_choices.Add(*it);
		}
	}
}

void SP_DLG_FunctionDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{
	
	int l_nRow = p_gEvent.GetRow();

	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(p_gEvent.GetEventObject());	

	wxString l_sDataType = l_pcGrid->GetCellValue(l_nRow, TYPE);
	
	if(l_sDataType == wxT("----Basic----") || l_sDataType == wxT("----Structured----"))
	{
		SP_MESSAGEBOX(wxT("Please select data type"), wxT("Function checking"), wxOK | wxICON_ERROR);
		return;
	}
}

void SP_DLG_FunctionDefinition::OnEditorShown( wxGridEvent& ev )
{
	if(m_bIsColored)
	{
		int l_nRow = ev.GetRow();
		int l_nCol = ev.GetCol();

		wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(ev.GetEventObject());
	
		SP_VectorString l_vParameter;

		if(l_nCol == PARAMETER)
		{
			wxString l_sParameter;
			wxString l_sParaValue;
			wxString l_sFunctionName = l_pcGrid->GetCellValue(l_nRow, NAME);
			l_sParaValue = l_pcGrid->GetCellValue(l_nRow,l_nCol);
			SP_DLG_FuncParaDecl l_cParaDlg(l_sFunctionName,l_sParaValue,NULL);
			if ( l_cParaDlg.ShowModal() == wxID_OK )
			{
				l_vParameter = l_cParaDlg.GetParameters();

				m_FuncParaMap[l_nRow] = l_vParameter;

				for(unsigned i = 0; i < l_vParameter.size(); i++)
				{
					if(i == 0)
						l_sParameter = l_vParameter[i+1] + wxT(" ") + l_vParameter[i];
					else
						l_sParameter = l_sParameter + wxT(",") + l_vParameter[i+1] + wxT(" ") + l_vParameter[i];
					i++;
				}
				l_pcGrid->SetCellValue(l_nRow, l_nCol,l_sParameter);
			}


			l_cParaDlg.Destroy();
		}
	}

    ev.Skip();
}



void SP_DLG_FunctionDefinition::OnCheckFunction( wxCommandEvent& p_cEvent )
{
	DoCheckFunction();
}

bool SP_DLG_FunctionDefinition::DoCheckFunction()
{
	if(m_bIsColored)
	{
		//we must save date data, because the check is done on the internal data structures
		SaveData();

		SP_CPN_ValueAssign l_cValueAssign;
		SP_CPN_ColorSetClass l_cColorSetClass;
		if( ! l_cValueAssign.InitializeColorset(l_cColorSetClass) )
		{
			RestoreBackup();
			return false;
		}
		else
		{
			LoadData();
		}
	}
	else
	{
		for (int i = 0; i < m_pcGrid->GetNumberRows(); ++i)
		{
			//TODO
			wxString l_sName = m_pcGrid->GetCellValue(i, NAME);
			wxString l_sParameter = m_pcGrid->GetCellValue(i, PARAMETER);
			wxString l_sBody = m_pcGrid->GetCellValue(i, BODY);
			if(l_sName.IsEmpty())
			{
				SP_LOGERROR( wxT("empty function name!"));
				return false;
			}
/*
			else
			{
				wxRegEx l_reName(wxT("[[:alnum:]_]+"));
				if(!l_reName.Matches(l_sName))
				{
					SP_LOGERROR( wxT("in function name: only alphanumeric (letter or digit) and underscore (_) are allowed."));
					return false;
				}
			}
			if(!l_sParameter.IsEmpty())
			{
				wxRegEx l_reParam(wxT("([[:alnum:]_]+)(,([[:alnum:]_]+))*"));
				if(!l_reParam.Matches(l_sParameter))
				{
					SP_LOGERROR( wxT("in function parameter: only names separated by comma are allowed."));
					return false;
				}
			}
*/
			if(l_sBody.IsEmpty())
			{
				SP_LOGERROR( wxT("empty function body!"));
				return false;
			}
			if(!m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_sName + wxT("(") + l_sParameter + wxT(")"))
				||
				!m_pcGraph->GetFunctionRegistry()->parseFunctionString(l_sBody))
			{
				return false;
			}
		}
	}
	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The function definition");	
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(this, l_sVariables, 1000);

	return true;
}

void SP_DLG_FunctionDefinition::OnAssistent(wxCommandEvent& p_cEvent)
{
	if (m_pcGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcGrid->GetGridCursorRow();

	if(m_bIsColored)
	{
		wxString l_sColorSet = m_pcGrid->GetCellValue(l_nEditRowPos, TYPE);
		wxString l_pcReturnText = m_pcGrid->GetCellValue(l_nEditRowPos, BODY);

		wxString l_sParaValue = m_pcGrid->GetCellValue(l_nEditRowPos,PARAMETER);
		vector<wxString> l_vParameter;
		while(l_sParaValue != wxT(""))
		{
			l_vParameter.push_back(l_sParaValue.BeforeFirst(wxT(' ')));
			l_sParaValue = l_sParaValue.AfterFirst(wxT(' '));
			l_vParameter.push_back(l_sParaValue.BeforeFirst(wxT(',')));
			l_sParaValue = l_sParaValue.AfterFirst(wxT(','));
		}

		SP_CPN_ColorSetClass l_cColorSetClass;
		SP_CPN_ValueAssign l_cValueAssign;
		l_cValueAssign.InitializeColorset(l_cColorSetClass);

		SP_DLG_FunctionAssistent* l_pcDlg = new SP_DLG_FunctionAssistent( l_vParameter, l_sColorSet, l_cColorSetClass,l_pcReturnText, this);

		if (l_pcDlg->ShowModal() == wxID_OK)
		{
			m_pcGrid->SetCellValue(l_nEditRowPos, BODY, l_pcDlg->GetReturnText());
		}

		l_pcDlg->Destroy();
	}
	else
	{
		//TODO
	}
}
