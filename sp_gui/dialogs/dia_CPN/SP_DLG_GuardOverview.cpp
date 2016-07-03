//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/9/6 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/dia_CPN/SP_DLG_GuardOverview.h"

#include <wx/valgen.h>
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"


IMPLEMENT_CLASS( SP_DLG_GuardOverview, wxDialog )

enum
{
	SP_ID_GRID_GUARD,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET
};

BEGIN_EVENT_TABLE( SP_DLG_GuardOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_GuardOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_GuardOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_GuardOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_GuardOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_GuardOverview::OnRenameSet )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_GuardOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_GuardOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_GuardOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_GuardOverview::SP_DLG_GuardOverview(wxWindow* p_pcParent,
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
	m_pcGuardGrid = new wxGrid( this, SP_ID_GRID_GUARD,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcGuardGrid->CreateGrid( 0, 0);

	m_pcGuardGrid->EnableEditing( true);	

	m_pcGuardGrid->SetDefaultColSize( 220, TRUE);
	m_pcGuardGrid->SetDefaultRowSize( 20, TRUE);

	m_pcGuardGrid->SetColLabelSize( 16);
	m_pcGuardGrid->SetRowLabelSize( 80);

	m_pcGuardGrid->AppendCols( 1);
	m_pcGuardGrid->SetColLabelValue(0, wxT("Main"));

	l_pcRowSizer->Add(m_pcGuardGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	CollectTransNodes();

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcGuardGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcSizer->Add(l_pcGridSizer, 1,  wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0,  wxEXPAND);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();

}

void SP_DLG_GuardOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_GuardOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_GuardOverview::OnAddSet(wxCommandEvent& p_cEvent)
{

}

void SP_DLG_GuardOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{
}

void SP_DLG_GuardOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{
}

void SP_DLG_GuardOverview::LoadData()
{	
	SP_DS_ColListAttribute* l_pcColList;	

	bool l_bWhite = false;
	wxString l_sPlaceName;
	wxString l_sLoadName = wxT("-1");
	int l_nGridRowNumber = 0;

	for(unsigned int l_nPos = 0; l_nPos < m_vTransNodes.size(); l_nPos++)			
	{
		l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(m_vTransNodes[l_nPos]->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();		

		m_pcGuardGrid->AppendRows( 1);
		m_pcGuardGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(m_vTransNodes[l_nPos]->GetAttribute(SP_DS_CPN_GUARDLIST) );

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcGuardGrid->SetCellValue(l_nGridRowNumber, i, l_pcColList->GetCell(i, 1));
			m_pcGuardGrid->SetCellBackgroundColour(l_nGridRowNumber, i,
					(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcGuardGrid->SetCellAlignment(l_nGridRowNumber, i,
					wxALIGN_RIGHT, wxALIGN_CENTER);

		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;
	}

}

void SP_DLG_GuardOverview::SaveData()
{	
	m_pcGuardGrid->SaveEditControlValue();

	SP_DS_ColListAttribute* l_pcColList;	
	int l_nGridRowNumber = 0;
	
	for(unsigned j = 0; j < m_vTransNodes.size(); j++)
	{
		SP_DS_Node* l_pcNode = m_vTransNodes[j];
		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST) );

		l_pcColList->Clear();

		for (int i = 0; i < m_pcGuardGrid->GetNumberCols(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			wxString l_sLabel = m_pcGuardGrid->GetColLabelValue(i);
			wxString l_sValue = m_pcGuardGrid->GetCellValue(l_nGridRowNumber, i);
			l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);
			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);			
		}
		l_nGridRowNumber++;
	}

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}



void SP_DLG_GuardOverview::OnCellValueChanged(wxGridEvent& ev)
{	
}

void SP_DLG_GuardOverview::OnGridCellSelected(wxGridEvent& ev)
{
	if ( (m_pcGuardGrid->GetNumberRows() > 0 )
			&& (m_pcGuardGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcGuardGrid->GetCellValue(ev.GetRow(),
				ev.GetCol() );
	}

	ev.Skip();
}

void SP_DLG_GuardOverview::LoadSetNames()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(l_pcGraph->GetNodeclass( m_sNodeClass )->GetElements()->size() > 0)
	{
		SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass( m_sNodeClass )->GetElements()->front();
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
		{
			wxString l_sSetName = l_pcAttr->GetCell(i,0);
			m_pcGuardGrid->AppendCols( 1);
			m_pcGuardGrid->SetColLabelValue(i, l_sSetName);
		}
	}
}

bool SP_DLG_GuardOverview::ExistMarkingSetName(const wxString& p_sName)
{	
	return true;
}

void SP_DLG_GuardOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}


void SP_DLG_GuardOverview::CollectTransNodes()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(! l_pcGraph )
		return;	

	SP_DS_Nodeclass* l_pcNodeclass;		
	SP_ListNode::const_iterator l_itElem;	

	l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Transition"));
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			m_vTransNodes.push_back(*l_itElem);
		}
	}

	l_pcNodeclass= l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			m_vTransNodes.push_back(*l_itElem);
		}
	}

	l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			m_vTransNodes.push_back(*l_itElem);
		}
	}

	l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			m_vTransNodes.push_back(*l_itElem);
		}
	}

	l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			m_vTransNodes.push_back(*l_itElem);
		}
	}

}
