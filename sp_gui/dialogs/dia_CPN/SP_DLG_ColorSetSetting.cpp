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

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSetting.h"

//#include "sp_gui/dialogs/dia_CPN/SP_DLG_CPNDeclaration.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_core/SP_Vector.h"


#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSelection.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h> 

#include <wx/colordlg.h>

IMPLEMENT_CLASS( SP_DLG_ColorSetSetting, wxDialog )

enum SP_CPN_ID
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_ADD2,
	SP_ID_BUTTON_DELETE2,
	SP_ID_BUTTON_ADD3,
	SP_ID_BUTTON_CHECK,
	SP_ID_BUTTON_DELETE3,
	SP_ID_BUTTON_ADD4,
	SP_ID_BUTTON_DELETE4,
	SP_ID_GRID_BASICCOLORSET,
	SP_ID_GRID_STRUCTUREDCOLORSET,
	SP_ID_GRID_SUBSETCOLORSET,
	SP_ID_GRID_ALIASCOLORSET,
	SP_ID_BUTTON_EXPORTDECLARATION
};

BEGIN_EVENT_TABLE( SP_DLG_ColorSetSetting, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColorSetSetting::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColorSetSetting::OnDlgCancel )
EVT_BUTTON( wxID_APPLY, SP_DLG_ColorSetSetting::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_ColorSetSetting::OnCheckFunction )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_ColorSetSetting::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_ColorSetSetting::OnDeleteColorSet )
EVT_BUTTON( SP_ID_BUTTON_ADD2, SP_DLG_ColorSetSetting::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE2, SP_DLG_ColorSetSetting::OnDeleteColorSet )
EVT_BUTTON( SP_ID_BUTTON_ADD3, SP_DLG_ColorSetSetting::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE3, SP_DLG_ColorSetSetting::OnDeleteColorSet )
EVT_BUTTON( SP_ID_BUTTON_ADD4, SP_DLG_ColorSetSetting::OnAddColorSet )
EVT_BUTTON( SP_ID_BUTTON_DELETE4, SP_DLG_ColorSetSetting::OnDeleteColorSet )

//EVT_BUTTON( SP_ID_BUTTON_EXPORTDECLARATION, SP_DLG_ColorSetSetting::OnExportDeclaration )

EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, SP_DLG_ColorSetSetting::OnPageChanged)
EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, SP_DLG_ColorSetSetting::OnPageChanging)

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_ColorSetSetting::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_ColorSetSetting::OnCellValueChanged )
#endif

EVT_GRID_CELL_LEFT_CLICK( SP_DLG_ColorSetSetting::OnEditorShown )

END_EVENT_TABLE()



SP_DLG_ColorSetSetting::SP_DLG_ColorSetSetting( wxWindow* p_pcParent,SP_CPN_GridNumber p_GridNumber,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120),  wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;


	m_pcBasicColorSetGrid = NULL;
	m_pcStructuredColorSetGrid = NULL;
	m_pcAliasColorSetGrid = NULL;

	// high level seperatore

//	wxBoxSizer* l_pcHighLevelSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	
    /* notebook to show multiple pages on */
    //m_pcNotebook = new SP_WDG_Notebook(this);
	m_pcNotebook = new SP_WDG_Notebook(this, wxT("ColorSetSetting"), wxDefaultPosition, wxSize(700,360));

   m_pcSizer->Add(m_pcNotebook, 1, wxEXPAND | wxALL, 5);

	////begin of page 1

    SP_WDG_NotebookPage* l_pcNotebookPage = AddPage(wxT("Simple Color sets"));
	
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	
	// m_pcBasicColorSetGrid
	m_pcBasicColorSetGrid = new wxGrid( l_pcNotebookPage, SP_ID_GRID_BASICCOLORSET,
			wxDefaultPosition, wxSize( 350, 250 ), wxSUNKEN_BORDER );
	m_pcBasicColorSetGrid->CreateGrid( 0, 0);
	m_pcBasicColorSetGrid->EnableEditing( true);	

	m_pcBasicColorSetGrid->SetDefaultColSize( 220, TRUE);
	m_pcBasicColorSetGrid->SetDefaultRowSize( 20, TRUE);

	m_pcBasicColorSetGrid->SetColLabelSize( 30);
	m_pcBasicColorSetGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer->Add(m_pcBasicColorSetGrid, 1, wxALL | wxEXPAND, 2);	
	l_pcNotebookPage->AddControl(l_pcRowSizer, 0, wxALL | wxEXPAND, 5);

	m_pcBasicColorSetGrid->EnableEditing(true);
	m_pcBasicColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	

	m_pcBasicColorSetGrid->AppendCols(5);

	m_pcBasicColorSetGrid->SetColLabelValue(0, wxT("Name") );
	m_pcBasicColorSetGrid->SetColSize(0, 100);

	m_pcBasicColorSetGrid->SetColLabelValue(1, wxT("Type") );
	m_pcBasicColorSetGrid->SetColSize(1, 100);

	m_pcBasicColorSetGrid->SetColLabelValue(2, wxT("Colors") );
	m_pcBasicColorSetGrid->SetColSize(2, 200);

	m_pcBasicColorSetGrid->SetColLabelValue(3, wxT("Place color") );
	m_pcBasicColorSetGrid->SetColSize(3, 70);	

	m_pcBasicColorSetGrid->SetColLabelValue(4, wxT("Show") );
	m_pcBasicColorSetGrid->SetColSize(4, 70);

	LoadData();

 	if(m_pcBasicColorSetGrid->GetNumberRows()==0)
	{	
		m_pcBasicColorSetGrid->AppendRows(1);

		m_pcBasicColorSetGrid->SetCellValue(0,0,wxT("Dot"));
		m_pcBasicColorSetGrid->SetCellEditor(0,1,new wxGridCellChoiceEditor(WXSIZEOF(m_Datatypes), m_Datatypes));
		m_pcBasicColorSetGrid->SetCellSize(0, 1, 1, 1);
		m_pcBasicColorSetGrid->SetCellValue(0, 1, wxT("dot"));
		m_pcBasicColorSetGrid->SetCellValue(0, 2, wxT("dot"));
		m_pcBasicColorSetGrid->SetCellOverflow(0, 1, false);

		m_pcBasicColorSetGrid->SetReadOnly(0,3);

		m_pcBasicColorSetGrid->SetCellEditor(0,4,new wxGridCellBoolEditor());

	}

    SP_AutoSizeRowLabelSize(m_pcBasicColorSetGrid);

	
	// command control
	wxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcButtonSizer->Add(new wxButton(l_pcNotebookPage, SP_ID_BUTTON_ADD, wxT("Add colorset") ), 1, wxALL, 5);
	l_pcButtonSizer->Add(new wxButton(l_pcNotebookPage, SP_ID_BUTTON_DELETE, wxT("Delete colorset") ), 1, wxALL, 5);
	l_pcButtonSizer->Add(new wxButton(l_pcNotebookPage, SP_ID_BUTTON_CHECK, wxT("Check colorsets") ), 1, wxALL, 5);

	//l_pcButtonSizer->Add(new wxButton(l_pcNotebookPage, SP_ID_BUTTON_EXPORTDECLARATION, wxT("Export declaration") ), 1, wxALL, 5);

    l_pcNotebookPage->AddControl(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

	//l_pcRowSizer->Add(l_pcButtonSizer, 0);

	//l_pcRowSizer->Layout();

 
	//The end of Page 1


    // begin of Page 2: define structured color set

	SP_WDG_NotebookPage* l_pcNotebookPage2 = AddPage(wxT("Compound Color Sets"));

	wxBoxSizer* l_pcRowSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	// m_pcStructuredColorSetGrid
	m_pcStructuredColorSetGrid = new wxGrid( l_pcNotebookPage2, SP_ID_GRID_STRUCTUREDCOLORSET,
			wxDefaultPosition, wxSize( 350, 250 ), wxSUNKEN_BORDER );
	m_pcStructuredColorSetGrid->CreateGrid( 0, 0);
	m_pcStructuredColorSetGrid->EnableEditing( true);	

	m_pcStructuredColorSetGrid->SetDefaultColSize( 220, TRUE);
	m_pcStructuredColorSetGrid->SetDefaultRowSize( 20, TRUE);

	m_pcStructuredColorSetGrid->SetColLabelSize( 30);
	m_pcStructuredColorSetGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer2->Add(m_pcStructuredColorSetGrid, 1, wxALL | wxEXPAND, 2);	
	l_pcNotebookPage2->AddControl(l_pcRowSizer2, 0, wxALL | wxEXPAND, 5);

	m_pcStructuredColorSetGrid->EnableEditing(true);
	m_pcStructuredColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	

	m_pcStructuredColorSetGrid->AppendCols(6);

	m_pcStructuredColorSetGrid->SetColLabelValue(0, wxT("Name") );
	m_pcStructuredColorSetGrid->SetColSize(0, 100);

	m_pcStructuredColorSetGrid->SetColLabelValue(1, wxT("Type") );
	m_pcStructuredColorSetGrid->SetColSize(1, 100);

	m_pcStructuredColorSetGrid->SetColLabelValue(2, wxT("Component color sets") );
	m_pcStructuredColorSetGrid->SetColSize(2, 200);

	m_pcStructuredColorSetGrid->SetColLabelValue(3, wxT("Colors") );
	m_pcStructuredColorSetGrid->SetColSize(3, 100);

	m_pcStructuredColorSetGrid->SetColLabelValue(4, wxT("Place color") );
	m_pcStructuredColorSetGrid->SetColSize(4, 70);

	m_pcStructuredColorSetGrid->SetColLabelValue(5, wxT("Show") );
	m_pcStructuredColorSetGrid->SetColSize(5, 70);

	LoadData2();

    SP_AutoSizeRowLabelSize(m_pcStructuredColorSetGrid);

	
	// command control
	wxSizer* l_pcButtonSizer2 = new wxBoxSizer( wxHORIZONTAL );

	l_pcButtonSizer2->Add(new wxButton(l_pcNotebookPage2, SP_ID_BUTTON_ADD2, wxT("Add colorset") ), 1, wxALL, 5);
	l_pcButtonSizer2->Add(new wxButton(l_pcNotebookPage2, SP_ID_BUTTON_DELETE2, wxT("Delete colorset") ), 1, wxALL, 5);
	l_pcButtonSizer2->Add(new wxButton(l_pcNotebookPage2, SP_ID_BUTTON_CHECK, wxT("Check colorset") ), 1, wxALL, 5);
	
    l_pcNotebookPage2->AddControl(l_pcButtonSizer2, 0, wxALL | wxEXPAND, 5);

	//The end of Page 2

  // begin of Page 4

	SP_WDG_NotebookPage* l_pcNotebookPage4 = AddPage(wxT("Alias Color Sets"));

	wxBoxSizer* l_pcRowSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	// m_pcStructuredColorSetGrid
	m_pcAliasColorSetGrid = new wxGrid( l_pcNotebookPage4, SP_ID_GRID_ALIASCOLORSET,
			wxDefaultPosition, wxSize( 350, 250 ), wxSUNKEN_BORDER );
	m_pcAliasColorSetGrid->CreateGrid( 0, 0);
	m_pcAliasColorSetGrid->EnableEditing( true);	

	m_pcAliasColorSetGrid->SetDefaultColSize( 220, TRUE);
	m_pcAliasColorSetGrid->SetDefaultRowSize( 20, TRUE);

	m_pcAliasColorSetGrid->SetColLabelSize( 30);
	m_pcAliasColorSetGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer4->Add(m_pcAliasColorSetGrid, 1, wxALL | wxEXPAND, 2);	
	l_pcNotebookPage4->AddControl(l_pcRowSizer4, 0, wxALL | wxEXPAND, 5);

	m_pcAliasColorSetGrid->EnableEditing(true);
	m_pcAliasColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	

	m_pcAliasColorSetGrid->AppendCols(2);

	m_pcAliasColorSetGrid->SetColLabelValue(0, wxT("Alias name") );
	m_pcAliasColorSetGrid->SetColSize(0, 200);

	m_pcAliasColorSetGrid->SetColLabelValue(1, wxT("Original color set") );
	m_pcAliasColorSetGrid->SetColSize(1, 200);


	LoadData4();
	
    SP_AutoSizeRowLabelSize(m_pcAliasColorSetGrid);

	
	// command control
	wxSizer* l_pcButtonSizer4 = new wxBoxSizer( wxHORIZONTAL );

	l_pcButtonSizer4->Add(new wxButton(l_pcNotebookPage4, SP_ID_BUTTON_ADD4, wxT("Add colorset") ), 1, wxALL, 5);
	l_pcButtonSizer4->Add(new wxButton(l_pcNotebookPage4, SP_ID_BUTTON_DELETE4, wxT("Delete colorset") ), 1, wxALL, 5);
	l_pcButtonSizer4->Add(new wxButton(l_pcNotebookPage4, SP_ID_BUTTON_CHECK, wxT("Check colorset") ), 1, wxALL, 5);
	
    l_pcNotebookPage4->AddControl(l_pcButtonSizer4, 0, wxALL | wxEXPAND, 5);

	//The end of Page 4

	// OK and Cancel command for the whole dialogue
	wxSizer* l_pcBottomButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);
	
	
	m_pcSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcNotebook->ChangeSelection(p_GridNumber);
  
	   
	SetSizerAndFit(m_pcSizer);
	m_pcSizer->SetSizeHints( this);
    Centre(wxBOTH);
}


void SP_DLG_ColorSetSetting::OnDlgApply( wxCommandEvent& p_cEvent )
{	
	if(m_pcBasicColorSetGrid)
		SaveData();
	if(m_pcStructuredColorSetGrid)
		SaveData2();
	if(m_pcAliasColorSetGrid)	
		SaveData4();

	if(m_pcBasicColorSetGrid)
		LoadData();
	if(m_pcStructuredColorSetGrid)
		LoadData2();
	if(m_pcAliasColorSetGrid)	
		LoadData4();	
}

void SP_DLG_ColorSetSetting::OnDlgOk(wxCommandEvent& p_cEvent)
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	
	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{

			if (SaveData()&&SaveData2()&&SaveData4())
			{
				wxBusyInfo wait(wxT("Please wait, updating marking..."),this);
				l_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

				SP_CPN_SyntaxChecking l_cSyntaxChecking;
				if( l_cSyntaxChecking.Initialize())
				{					
					l_cSyntaxChecking.UpdateNetMarking();
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

void SP_DLG_ColorSetSetting::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_ColorSetSetting::OnEditColorSet( wxCommandEvent& p_cEvent )
{
	
	// not used now

	if (m_pcBasicColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcBasicColorSetGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcBasicColorSetGrid->GetCellValue(l_nEditRowPos, 1);

	m_pcBasicColorSetGrid->SelectBlock(l_nEditRowPos, 1, l_nEditRowPos, 1);
	m_pcBasicColorSetGrid->SetFocus();
}

void SP_DLG_ColorSetSetting::OnDeleteColorSet( wxCommandEvent& p_cEvent )
{
	int l_nSel=m_pcNotebook->GetSelection();
	wxGrid* l_pcColorSetGrid;

	switch(l_nSel) {

		case 0:
			l_pcColorSetGrid=m_pcBasicColorSetGrid;
			break;
		case 1:
			l_pcColorSetGrid=m_pcStructuredColorSetGrid;
			break;
		case 2:
			l_pcColorSetGrid=m_pcAliasColorSetGrid;
			break;
		default:
			l_pcColorSetGrid=m_pcBasicColorSetGrid;
			break;
	}

	if (l_pcColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}


	if ( l_pcColorSetGrid->IsSelection() )
    {
        l_pcColorSetGrid->BeginBatch();
        for ( int n = 0; n < l_pcColorSetGrid->GetNumberRows(); )
        {
            if ( l_pcColorSetGrid->IsInSelection( n , 0 ) )
                l_pcColorSetGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        l_pcColorSetGrid->EndBatch();
    }

}

void SP_DLG_ColorSetSetting::OnAddColorSet( wxCommandEvent& p_cEvent )
{
    
	int l_nSel=m_pcNotebook->GetSelection();
	wxGrid* l_pcColorSetGrid;
	int l_nEditRowPos;

	switch(l_nSel) {

		case 0:
			l_pcColorSetGrid=m_pcBasicColorSetGrid;

			InitializeBasicChoice();

			l_pcColorSetGrid->AppendRows(1);
			l_nEditRowPos = l_pcColorSetGrid->GetNumberRows()-1;				
			l_pcColorSetGrid->SetCellEditor(l_nEditRowPos,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
			l_pcColorSetGrid->SetCellSize(l_nEditRowPos, 1, 1, 1);
			l_pcColorSetGrid->SetCellValue(l_nEditRowPos, 1, wxT("int"));	
			l_pcColorSetGrid->SetCellValue(l_nEditRowPos, 2, wxT("0-100000"));	
			l_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 1, false);

			l_pcColorSetGrid->SetCellEditor(l_nEditRowPos,4,new wxGridCellBoolEditor());

			delete [] choices;

			break;
		case 1:
			l_pcColorSetGrid=m_pcStructuredColorSetGrid;

			InitializeStructuredChoice();
			
			l_pcColorSetGrid->AppendRows(1);
			l_nEditRowPos = l_pcColorSetGrid->GetNumberRows()-1;				
			l_pcColorSetGrid->SetCellEditor(l_nEditRowPos,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
			l_pcColorSetGrid->SetCellSize(l_nEditRowPos, 1, 1, 1);
			l_pcColorSetGrid->SetCellValue(l_nEditRowPos, 1, wxT("product"));
			l_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 1, false);
			l_pcColorSetGrid->SetReadOnly(l_nEditRowPos,3);

			l_pcColorSetGrid->SetCellEditor(l_nEditRowPos,5,new wxGridCellBoolEditor());

			delete [] choices;
			break;
		case 2:
			l_pcColorSetGrid=m_pcAliasColorSetGrid;

			InitializeChoice();	

			l_pcColorSetGrid->AppendRows(1);
			l_nEditRowPos = l_pcColorSetGrid->GetNumberRows()-1;	
			l_pcColorSetGrid->SetCellEditor(l_nEditRowPos,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
			l_pcColorSetGrid->SetCellSize(l_nEditRowPos, 1, 1, 1);
			l_pcColorSetGrid->SetCellValue(l_nEditRowPos, 1, wxT(""));	
			l_pcColorSetGrid->SetCellOverflow(l_nEditRowPos, 1, false);

			delete [] choices;

			break;
		default:
			break;
	}
	

}


bool SP_DLG_ColorSetSetting::LoadData()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);

	if(!l_pcMetadataclass)
		return false;

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));		

	bool l_bWhite = false;

	if (m_pcBasicColorSetGrid->GetNumberRows() > 0)
	{
		m_pcBasicColorSetGrid->DeleteRows(0, m_pcBasicColorSetGrid->GetNumberRows());
	}

	m_pcBasicColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	InitializeBasicChoice();

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcBasicColorSetGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcBasicColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcBasicColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		
		//m_pcBasicColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(WXSIZEOF(m_Datatypes), m_Datatypes));
		m_pcBasicColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcBasicColorSetGrid->SetCellSize(i, 1, 1, 1);
		m_pcBasicColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1).c_str());
		m_pcBasicColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcBasicColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		

		m_pcBasicColorSetGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,2).c_str());
		m_pcBasicColorSetGrid->SetCellAlignment(i, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcBasicColorSetGrid->SetCellBackgroundColour(i, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));


		//place color
		wxString l_sPlaceColor = l_pcColList->GetCell(i,3);
		wxColour l_PlaceColor = wxColour( l_sPlaceColor);
		m_pcBasicColorSetGrid->SetCellBackgroundColour(i, 3, l_PlaceColor);
		m_pcBasicColorSetGrid->SetReadOnly(i, 3);

		m_pcBasicColorSetGrid->SetCellValue(i, 4, l_pcColList->GetCell(i,4).c_str());
		m_pcBasicColorSetGrid->SetCellEditor(i,4,new wxGridCellBoolEditor());


		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	delete [] choices;

	return true;

}

bool SP_DLG_ColorSetSetting::SaveData()
{
	m_pcBasicColorSetGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	l_pcColList->Clear();

	for (int i = 0; i < m_pcBasicColorSetGrid->GetNumberRows(); i++)
	{
		int l_nRow=l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nRow, 0, m_pcBasicColorSetGrid->GetCellValue(i, 0));		
		l_pcColList->SetCell(l_nRow, 1, m_pcBasicColorSetGrid->GetCellValue(i, 1));
		l_pcColList->SetCell(l_nRow, 2, m_pcBasicColorSetGrid->GetCellValue(i, 2));

		wxColour l_PlaceColor = m_pcBasicColorSetGrid->GetCellBackgroundColour(i, 3);
		wxString l_sPlaceColor = l_PlaceColor.GetAsString();			
		wxTheColourDatabase->AddColour(l_sPlaceColor, l_PlaceColor);
		l_pcColList->SetCell(l_nRow, 3, l_sPlaceColor);

		l_pcColList->SetCell(l_nRow, 4, m_pcBasicColorSetGrid->GetCellValue(i, 4));
	}

	return true;

}

SP_WDG_NotebookPage*
SP_DLG_ColorSetSetting::AddPage(const wxString& p_sLabel)
{
    if (m_pcNotebook)
        return m_pcNotebook->AddOrGetPage(p_sLabel);

    return NULL;
}

void SP_DLG_ColorSetSetting::OnPageChanged( wxNotebookEvent& event)
{	
	if(m_pcBasicColorSetGrid)
		LoadData();
	if(m_pcStructuredColorSetGrid)
		LoadData2();
	if(m_pcAliasColorSetGrid)	
		LoadData4();	
}

void SP_DLG_ColorSetSetting::OnPageChanging( wxNotebookEvent& event)
{	
	if(m_pcBasicColorSetGrid)
		SaveData();
	if(m_pcStructuredColorSetGrid)
		SaveData2();
	if(m_pcAliasColorSetGrid)	
		SaveData4();	
}

bool SP_DLG_ColorSetSetting::LoadData2()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));		

	bool l_bWhite = false;

	if (m_pcStructuredColorSetGrid->GetNumberRows() > 0)
	{
		m_pcStructuredColorSetGrid->DeleteRows(0, m_pcStructuredColorSetGrid->GetNumberRows());
	}

	m_pcStructuredColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	InitializeStructuredChoice();

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcStructuredColorSetGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcStructuredColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcStructuredColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		
		//m_pcStructuredColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(WXSIZEOF(m_Structuredtypes), m_Structuredtypes));
		m_pcStructuredColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcStructuredColorSetGrid->SetCellSize(i, 1, 1, 1);
		m_pcStructuredColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1).c_str());
		m_pcStructuredColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcStructuredColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		

		m_pcStructuredColorSetGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,2).c_str());
		m_pcStructuredColorSetGrid->SetCellAlignment(i, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcStructuredColorSetGrid->SetCellBackgroundColour(i, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		m_pcStructuredColorSetGrid->SetReadOnly(i,3);

		m_pcStructuredColorSetGrid->SetCellValue(i, 3, l_pcColList->GetCell(i,3).c_str());
		m_pcStructuredColorSetGrid->SetCellAlignment(i, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcStructuredColorSetGrid->SetCellBackgroundColour(i, 3, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));

		//place color
		m_pcStructuredColorSetGrid->SetReadOnly(i, 4);
		wxString l_sPlaceColor = l_pcColList->GetCell(i,4);
		wxColour l_PlaceColor = wxColour( l_sPlaceColor);
		m_pcStructuredColorSetGrid->SetCellBackgroundColour(i, 4, l_PlaceColor);
		

		m_pcStructuredColorSetGrid->SetCellValue(i, 5, l_pcColList->GetCell(i,5).c_str());
		m_pcStructuredColorSetGrid->SetCellEditor(i,5,new wxGridCellBoolEditor());


		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	delete [] choices;

	return true;

}

bool SP_DLG_ColorSetSetting::LoadData4()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));		

	bool l_bWhite = false;

	if (m_pcAliasColorSetGrid->GetNumberRows() > 0)
	{
		m_pcAliasColorSetGrid->DeleteRows(0, m_pcAliasColorSetGrid->GetNumberRows());
	}

	m_pcAliasColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	InitializeChoice();	

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcAliasColorSetGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcAliasColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcAliasColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		
		m_pcAliasColorSetGrid->SetCellEditor(i,1,new wxGridCellChoiceEditor(m_choices.GetCount(), choices));
		m_pcAliasColorSetGrid->SetCellSize(i, 1, 1, 1);
		m_pcAliasColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,1).c_str());
		m_pcAliasColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcAliasColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	delete [] choices;
	return true;

}


////////////////////save data


bool SP_DLG_ColorSetSetting::SaveData2()
{
	m_pcStructuredColorSetGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	l_pcColList->Clear();

	for (int i = 0; i < m_pcStructuredColorSetGrid->GetNumberRows(); i++)
	{
		int l_nRow=l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nRow, 0, m_pcStructuredColorSetGrid->GetCellValue(i, 0));		
		l_pcColList->SetCell(l_nRow, 1, m_pcStructuredColorSetGrid->GetCellValue(i, 1));
		l_pcColList->SetCell(l_nRow, 2, m_pcStructuredColorSetGrid->GetCellValue(i, 2));
		l_pcColList->SetCell(l_nRow, 3, m_pcStructuredColorSetGrid->GetCellValue(i, 3));

		wxColour l_PlaceColor = m_pcStructuredColorSetGrid->GetCellBackgroundColour(i, 4);
		wxString l_sPlaceColor = l_PlaceColor.GetAsString();			
		wxTheColourDatabase->AddColour(l_sPlaceColor, l_PlaceColor);
		l_pcColList->SetCell(l_nRow, 4, l_sPlaceColor);

		l_pcColList->SetCell(l_nRow, 5, m_pcStructuredColorSetGrid->GetCellValue(i, 5));
	}

	return true;

}

bool SP_DLG_ColorSetSetting::SaveData4()
{
	m_pcAliasColorSetGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	l_pcColList->Clear();

	for (int i = 0; i < m_pcAliasColorSetGrid->GetNumberRows(); i++)
	{
		int l_nRow=l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nRow, 0, m_pcAliasColorSetGrid->GetCellValue(i, 0));		
		l_pcColList->SetCell(l_nRow, 1, m_pcAliasColorSetGrid->GetCellValue(i, 1));
	}

	return true;

}


void SP_DLG_ColorSetSetting::InitializeChoice()
{
	unsigned l_nArraysize;
	l_nArraysize = 0;

	l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();
	
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));		
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();


	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));		
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();

	
	m_choices.Alloc(l_nArraysize);
	m_choices.Empty();

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
	}

}

void SP_DLG_ColorSetSetting::OnCellValueChanged(wxGridEvent& p_gEvent)
{
    int l_nRow = p_gEvent.GetRow();
    int l_nCol = p_gEvent.GetCol();

	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(p_gEvent.GetEventObject());
	wxWindowID l_WindowID = l_pcGrid->GetId();
	wxString l_sGridValue = l_pcGrid->GetCellValue(l_nRow,l_nCol);

	wxString l_sColorsetName = l_pcGrid->GetCellValue(l_nRow,0);

	if(l_WindowID == SP_ID_GRID_BASICCOLORSET)
	{
		if( l_nCol == 0 )
		{
			wxString l_sError;
			m_cColorProcessing.ProcessingIdentifier(l_sGridValue,l_sError);
		}
		if( l_nCol == 1 && l_sGridValue == wxT("dot") )
		{
			if(l_pcGrid->GetCellValue(l_nRow,0) != wxT("Dot"))
				SP_MESSAGEBOX(wxT("The colorset should be \"Dot\" "), wxT("Colorset check"), wxOK | wxICON_INFORMATION );
		}

		if( l_nCol == 1 && l_sGridValue == wxT("int") )
		{
			l_pcGrid->SetCellValue(l_nRow,2,wxT("0-100000"));
			l_pcGrid->TransferDataToWindow();
			
		}

		if( l_nCol == 1 && l_sGridValue == wxT("bool") )
		{
			l_pcGrid->SetCellValue(l_nRow,2,wxT("true,false"));
			l_pcGrid->TransferDataToWindow();
			
		}

		if( l_nCol == 2 )
		{
			wxString l_sDataType = l_pcGrid->GetCellValue(l_nRow,1);
			if( l_sDataType == wxT("dot"))
			{
				if(l_sGridValue != wxT("dot"))
					SP_MESSAGEBOX(wxT("The color should be \"dot\" "), wxT("Colorset check"), wxOK | wxICON_INFORMATION );
			}
			if( l_sDataType == wxT("int"))
			{
				bool l_bUnUsed;
				vector<unsigned> l_vResult;
				m_cColorProcessing.ProcessingInt(l_sColorsetName,l_sGridValue,l_vResult,l_bUnUsed);
			}
			if( l_sDataType == wxT("string"))
			{
				vector<wxString> l_vResult;
				m_cColorProcessing.ProcessingString(l_sColorsetName,l_sGridValue,l_vResult);
			}
			if( l_sDataType == wxT("enum"))
			{
				vector<wxString> l_vResult;
				//m_cColorProcessing.ProcessingEnum(l_sColorsetName,l_sGridValue,l_vResult);
				m_cColorProcessing.ProcessingString(l_sColorsetName,l_sGridValue,l_vResult);
			}
			if( l_sDataType == wxT("index"))
			{
				wxString l_sIndexName;
				vector<unsigned> l_vResult;
				m_cColorProcessing.ProcessingIndex(l_sColorsetName,l_sGridValue,l_sIndexName,l_vResult);
			}
			if(l_sDataType == wxT("----Basic types----") || l_sDataType == wxT("----User-defined colorsets----"))
			{
				SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
				return;
			}
		}
	}

	if(l_WindowID == SP_ID_GRID_STRUCTUREDCOLORSET)
	{
		if( l_nCol == 0 )
		{
			wxString l_sError;
			m_cColorProcessing.ProcessingIdentifier(l_sGridValue,l_sError);
		}
		if( l_nCol == 1 )
		{
			wxString l_sDataType = l_pcGrid->GetCellValue(l_nRow,1);
			if(l_sDataType == wxT("----Structured types----") || l_sDataType == wxT("----User-defined colorsets----"))
			{
				SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
				return;
			}
		}
		if( l_nCol == 2 )
		{
			vector<wxString> l_vResult;
			m_cColorProcessing.ProcessingComponent(l_sColorsetName,l_sGridValue,l_vResult);
		}
	}

	

    p_gEvent.Skip();
}


void SP_DLG_ColorSetSetting::OnEditorShown( wxGridEvent& ev )
{
	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(ev.GetEventObject());
	wxWindowID l_WindowID = l_pcGrid->GetId();
	
	if(l_WindowID == SP_ID_GRID_STRUCTUREDCOLORSET && ev.GetCol() == 3)	
	{
		SP_MESSAGEBOX(wxT("The colors will be generated automatically."), wxT("Notification"), wxOK | wxICON_INFORMATION );
	}

	
	
	if(l_WindowID == SP_ID_GRID_STRUCTUREDCOLORSET && ev.GetCol() == 2  )	
	{
		wxString l_sDataType = l_pcGrid->GetCellValue(ev.GetRow(), 1);

		if( l_sDataType == wxT("product") || l_sDataType == wxT("union") )
		{
			wxArrayString l_ArrayString;
			wxString l_sComponents = wxT("");

			InitializeChoice();	
			SP_DLG_ColorSetSelection l_cColorSetDlg(m_choices,NULL);
			if ( l_cColorSetDlg.ShowModal() == wxID_OK )
			{
				l_ArrayString = l_cColorSetDlg.GetStrings();
				for( unsigned i = 0; i < l_ArrayString.size(); i++)
				{
					if(i == 0)
						l_sComponents = l_ArrayString[i];
					else
						l_sComponents = l_sComponents + wxT(",")+ l_ArrayString[i];
				}
				l_pcGrid->SetCellValue(ev.GetRow(),ev.GetCol(),l_sComponents);
			}
			l_cColorSetDlg.Destroy();
		}
	}

	if((l_WindowID == SP_ID_GRID_STRUCTUREDCOLORSET && ev.GetCol() == 4) ||
	   (l_WindowID == SP_ID_GRID_BASICCOLORSET && ev.GetCol() == 3) )
	{
		wxColourDialog l_dial(this);
		if(l_dial.ShowModal()==wxID_OK)
		{
			wxColour l_PlaceColor= l_dial.GetColourData().GetColour();
			l_pcGrid->SetCellBackgroundColour(ev.GetRow(),ev.GetCol(),l_PlaceColor);
		}
	}

    ev.Skip();
}


void SP_DLG_ColorSetSetting::OnCheckFunction( wxCommandEvent& p_cEvent )
{
	
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	if( ! l_cValueAssign.InitializeColorset(l_cColorSetClass) )
		return;
	

	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The colorset definition");	
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(m_pcNotebook, l_sVariables, 1000);

}


void SP_DLG_ColorSetSetting::InitializeBasicChoice()
{
	unsigned l_nArraysize;
	l_nArraysize = 0;

	l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();

	m_choices.Alloc(l_nArraysize+7);
	m_choices.Empty();

	m_choices.Add(wxT("----Basic types----"));
	m_choices.Add(wxT("dot"));
	m_choices.Add(wxT("string"));
	m_choices.Add(wxT("int"));
	m_choices.Add(wxT("bool"));
	m_choices.Add(wxT("enum"));
	m_choices.Add(wxT("index"));
	m_choices.Add(wxT("----User-defined colorsets----"));

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		
		wxString l_sColorSetName = l_pcColList->GetCell(i,0).c_str();
		m_choices.Add(l_pcColList->GetCell(i,0).c_str());
	}
	

	size_t count = m_choices.GetCount();
	choices = new wxString[count];
	for ( size_t n = 0; n < count; n++ )
	{		
		choices[n] = m_choices[n];		
	}

}



void SP_DLG_ColorSetSetting::InitializeStructuredChoice()
{
	unsigned l_nArraysize;
	l_nArraysize = 0;

	l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	////////////////////////////by george///////////////////////////////////////
		SP_DS_ColListAttribute* l_pcColListBasic;

		l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		l_pcColListBasic = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
		l_nArraysize = l_nArraysize + l_pcColListBasic->GetRowCount();

	//////////////////////////////////////////////////////////////////
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));		
	l_nArraysize = l_nArraysize + l_pcColList->GetRowCount();


	m_choices.Alloc(l_nArraysize+4);
	m_choices.Empty();

	m_choices.Add(wxT("----Basic types----"));//by george
	m_choices.Add(wxT("----Structured types----"));
	m_choices.Add(wxT("product"));
	m_choices.Add(wxT("union"));
	m_choices.Add(wxT("----User-defined colorsets----"));


	/////////////////////////////by george////////////////////////

	for (unsigned int i = 0; i < l_pcColListBasic->GetRowCount(); i++)
	{
		wxString l_sColorSetName = l_pcColListBasic->GetCell(i, 0).c_str();
		m_choices.Add(l_pcColListBasic->GetCell(i, 0).c_str());
	}

		/////////////////////////////////////////////////////////////

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{		
		wxString l_sColorSetName = l_pcColList->GetCell(i,0).c_str();
		m_choices.Add(l_pcColList->GetCell(i,0).c_str());
	}
	

	size_t count = m_choices.GetCount();
	choices = new wxString[count];
	for ( size_t n = 0; n < count; n++ )
	{		
		choices[n] = m_choices[n];		
	}

}


void SP_DLG_ColorSetSetting::OnExportDeclaration( wxCommandEvent& p_cEvent )
{
}

