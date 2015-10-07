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
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetOverview.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"


IMPLEMENT_CLASS( SP_DLG_ColorSetOverview, wxDialog )

enum
{
	SP_ID_BUTTON_IMPORTCOLORS,
	SP_ID_BUTTON_IMPORTCOLORSET,
	SP_ID_GRID_MARKING
};

BEGIN_EVENT_TABLE( SP_DLG_ColorSetOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColorSetOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColorSetOverview::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_IMPORTCOLORS, SP_DLG_ColorSetOverview::OnImportColors )
EVT_BUTTON( SP_ID_BUTTON_IMPORTCOLORSET, SP_DLG_ColorSetOverview::OnImportColorSet )

END_EVENT_TABLE()



SP_DLG_ColorSetOverview::SP_DLG_ColorSetOverview(wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{	
	m_pcParent = p_pcParent;

	m_sColorSetName = wxT("");

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

	m_pcColorSetGrid->SetColLabelSize( 30);
	m_pcColorSetGrid->SetRowLabelSize( 0); //do not let it show.

	l_pcRowSizer->Add(m_pcColorSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcColorSetGrid->EnableEditing(true);
	m_pcColorSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);	

	m_pcColorSetGrid->AppendCols(4);

	m_pcColorSetGrid->SetColLabelValue(0, wxT("Group") );
	m_pcColorSetGrid->SetColSize(0, 100);

	m_pcColorSetGrid->SetColLabelValue(1, wxT("Name") );
	m_pcColorSetGrid->SetColSize(1, 100);

	m_pcColorSetGrid->SetColLabelValue(2, wxT("Type") );
	m_pcColorSetGrid->SetColSize(2, 100);

	m_pcColorSetGrid->SetColLabelValue(3, wxT("Colors") );
	m_pcColorSetGrid->SetColSize(3, 300);


	LoadData();


    SP_AutoSizeRowLabelSize(m_pcColorSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL );

	wxSizer *l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
	

	//l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_IMPORTCOLORS, wxT("Import colors") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	//l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_IMPORTCOLORSET, wxT("Import colorset") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	
	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	
	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_CENTER);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM | wxALIGN_CENTER);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();

}

void SP_DLG_ColorSetOverview::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
			
		{
			ImportData();
			EndModal(wxID_OK);	

		}
		else
		{

			SetReturnCode(wxID_OK);
			this->Show(FALSE);

		}

	}

}

void SP_DLG_ColorSetOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_ColorSetOverview::OnEditColorSet( wxCommandEvent& p_cEvent )
{

}

void SP_DLG_ColorSetOverview::OnImportColorSet( wxCommandEvent& p_cEvent )
{
	/*
	wxGrid* l_pcColorMarkingGrid=m_pcColorMarkingDlg->GetColorGrid();
	
	if (m_pcColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}
	int l_nEditRowPos = m_pcColorSetGrid->GetGridCursorRow();

	if(m_pcColorSetGrid->GetGridCursorCol() != 1 )
	{
		SP_MESSAGEBOX(wxT("Please select proper colorset"), wxT("Notication"), wxOK | wxICON_INFORMATION);
		return;
	}
    //Here inser the code to import the colorset for a place

	wxString l_sColorSetName = m_pcColorSetGrid->GetCellValue(l_nEditRowPos,1);	

	m_pcColorMarkingDlg->GetNameAttibute()->SetName(l_sColorSetName);

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{			
			EndModal(wxID_OK);	

		}
		else
		{

			SetReturnCode(wxID_OK);
			this->Show(FALSE);

		}

	}
*/
}

void SP_DLG_ColorSetOverview::OnImportColors( wxCommandEvent& p_cEvent )
{
/*
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	SP_CPN_ColorSet* l_pcColorSet;
	l_cValueAssign.InitializeColorset(l_cColorSetClass);
	
	wxGrid* l_pcColorMarkingGrid=m_pcColorMarkingDlg->GetColorGrid();

	if (l_pcColorMarkingGrid->GetNumberRows() > 0)
	{
		l_pcColorMarkingGrid->DeleteRows(0, l_pcColorMarkingGrid->GetNumberRows());
	}
	
	if (m_pcColorSetGrid->GetNumberRows() == 0)
	{
		return;
	}
	int l_nEditRowPos = m_pcColorSetGrid->GetGridCursorRow();
    //Here inser the code to import the colors for a place

	wxString l_sColorSetName;
	vector<wxString> l_vStringColors;
	if(m_pcColorSetGrid->GetCellValue(l_nEditRowPos,0) == wxT("Basic")||
		m_pcColorSetGrid->GetCellValue(l_nEditRowPos,0) == wxT("Structural"))
	{
		l_sColorSetName = m_pcColorSetGrid->GetCellValue(l_nEditRowPos,1);
		l_pcColorSet = l_cColorSetClass.LookupColorSet(l_sColorSetName);		
		if(!l_pcColorSet)
			return;
		
		if(l_pcColorSet->GetDataType() == CPN_INTEGER)
		{
			vector<int> l_vIntColors;
			l_vIntColors = l_pcColorSet->GetIntegerValue();
			for(unsigned i = 0; i < l_vIntColors.size(); i++)
				l_vStringColors.push_back(wxString::Format(wxT("%d"),l_vIntColors[i]));
		}
		else if(l_pcColorSet->GetDataType() == CPN_BOOLEAN)
		{
			l_vStringColors.push_back(wxT("false"));
			l_vStringColors.push_back(wxT("true"));
		}
		else
		
		{
			l_vStringColors = l_pcColorSet->GetStringValue();
		}

		if (l_pcColorMarkingGrid->GetNumberRows() > 0)
		{
			l_pcColorMarkingGrid->DeleteRows(0, l_pcColorMarkingGrid->GetNumberRows());
		}	
		

		if(l_vStringColors.size()>0)
		{		
			l_pcColorMarkingGrid->AppendRows(l_vStringColors.size());
		}

		for(unsigned i = 0; i < l_vStringColors.size(); i++)
		{
			l_pcColorMarkingGrid->SetCellValue(i, 0, l_vStringColors[i]);
			l_pcColorMarkingGrid->SetCellValue(i, 1, wxT("1"));
		}
	}

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{			
			EndModal(wxID_OK);	

		}
		else
		{

			SetReturnCode(wxID_OK);
			this->Show(FALSE);

		}

	}
	*/
}


bool SP_DLG_ColorSetOverview::LoadData()
{
	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!l_pcGraph)
		return false;

	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));	
	if(!l_pcColList)
		return false;

	bool l_bWhite = true;
	int l_RowRecord = 0;

	if (m_pcColorSetGrid->GetNumberRows() > 0)
	{
		m_pcColorSetGrid->DeleteRows(0, m_pcColorSetGrid->GetNumberRows());
	}

	m_pcColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcColorSetGrid->SetCellValue(i, 0, wxT("Basic"));
		m_pcColorSetGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
       // (l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i, 1, l_pcColList->GetCell(i,0).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);
        
		m_pcColorSetGrid->SetCellValue(i, 2, l_pcColList->GetCell(i,1).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));	
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);		

		m_pcColorSetGrid->SetCellValue(i, 3, l_pcColList->GetCell(i,2).c_str());
		m_pcColorSetGrid->SetCellAlignment(i, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i, 3, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		
		(l_bWhite ? l_bWhite = false : l_bWhite = true);	
		
	}

	l_RowRecord = l_pcColList->GetRowCount(); //get the overall row number


    /////////////////
	//l_bWhite = true;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;

	m_pcColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 0, wxT("Structural"));
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 1, l_pcColList->GetCell(i,0).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
       // (l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 2, l_pcColList->GetCell(i,1).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);		

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 3, l_pcColList->GetCell(i,2).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 3, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		
		(l_bWhite ? l_bWhite = false : l_bWhite = true);	

	}

	l_RowRecord = l_RowRecord + l_pcColList->GetRowCount(); //get the overall row number


	

	////////////////////
	//l_bWhite = true;
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	if(!l_pcColList)
		return false;

	m_pcColorSetGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 0, wxT("Alias"));
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 1, l_pcColList->GetCell(i,0).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 1, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 1, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
       // (l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 2, l_pcColList->GetCell(i,1).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 2, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 2, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		//(l_bWhite ? l_bWhite = false : l_bWhite = true);

		m_pcColorSetGrid->SetCellValue(i+l_RowRecord, 3, l_pcColList->GetCell(i,2).c_str());
		m_pcColorSetGrid->SetCellAlignment(i+l_RowRecord, 3, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcColorSetGrid->SetCellBackgroundColour(i+l_RowRecord, 3, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);	

	}

	////////////////////

	return true;

}

bool SP_DLG_ColorSetOverview::SaveData()
{
	return true;
}


bool SP_DLG_ColorSetOverview::ImportData()
{

	int l_nEditRowPos = 0;
	
	if(m_pcColorSetGrid->GetSelectedRows().size()>0)
		l_nEditRowPos = m_pcColorSetGrid->GetSelectedRows()[0];

	if(l_nEditRowPos < m_pcColorSetGrid->GetNumberRows())
		m_sColorSetName = m_pcColorSetGrid->GetCellValue(l_nEditRowPos,1);

	/*
	wxGrid* l_pcColorMarkingGrid=m_pcColorMarkingDlg->GetColorGrid();

	
	if (m_pcColorSetGrid->GetNumberRows() == 0)
	{
		return false;
	}
	int l_nEditRowPos = m_pcColorSetGrid->GetGridCursorRow();

    //Here inser the code to import the colors for a place.

	l_pcColorMarkingGrid->AppendRows(1); // test
	l_pcColorMarkingGrid->SetCellValue(0, 1, wxT("string"));	//test
*/
	return true;
}
