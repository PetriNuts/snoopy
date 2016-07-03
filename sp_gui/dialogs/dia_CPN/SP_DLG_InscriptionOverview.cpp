//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/26 16:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/dia_CPN/SP_DLG_InscriptionOverview.h"

#include <wx/valgen.h>
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"


IMPLEMENT_CLASS( SP_DLG_InscriptionOverview, wxDialog )

enum
{
	SP_ID_GRID_INSCRIPTION
};

BEGIN_EVENT_TABLE( SP_DLG_InscriptionOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_InscriptionOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_InscriptionOverview::OnDlgCancel )


#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_InscriptionOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_InscriptionOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_InscriptionOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_InscriptionOverview::SP_DLG_InscriptionOverview(wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{
/*	
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sNetClass = l_pcGraph->GetNetclass()->GetName();

	if(l_sNetClass == SP_DS_COLCPN_CLASS)
	{
		m_sEdgeClass = SP_DS_CONTINUOUSPED_EDGE;
	}
	else		
	{
		m_sEdgeClass = wxT("Edge");
	}
*/
	m_pcParent = p_pcParent;
	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row

	m_pcInscriptionGrid = new wxGrid( this, SP_ID_GRID_INSCRIPTION,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcInscriptionGrid->CreateGrid( 0, 0);

	m_pcInscriptionGrid->EnableEditing( true);

	m_pcInscriptionGrid->SetDefaultColSize( 220, TRUE);
	m_pcInscriptionGrid->SetDefaultRowSize( 20, TRUE);

	m_pcInscriptionGrid->SetColLabelSize( 16);
	m_pcInscriptionGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcInscriptionGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcInscriptionGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );


	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,
			wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND | wxALIGN_TOP
			);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM
			);

	m_pcSizer->Add(l_pcGridSizer, 1,  wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT | wxEXPAND);

	SetAutoLayout( TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit( this);
	m_pcSizer->SetSizeHints( this);

	Layout();

}

void SP_DLG_InscriptionOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_InscriptionOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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



void SP_DLG_InscriptionOverview::LoadData()
{
	CollectEdges();

	SP_DS_ColListAttribute* l_pcColList;
	
	bool l_bWhite = false;
	wxString l_sEdgeName;
	wxString l_sLoadName = wxT("-1");

	int l_nGridRowNumber = 0;
	m_pcInscriptionGrid->AppendCols( 1);
	m_pcInscriptionGrid->SetColLabelValue(0, wxT("Main"));

	for(unsigned j = 0; j < m_pcEdgeVector.size(); j++ )	
	{
		SP_DS_Edge* l_pcEdge = m_pcEdgeVector[j];
		
		wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcEdge->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcEdge->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		l_sEdgeName = l_sSourceNodeName + wxT(" --> ") + l_sTargetNodeName;

		m_pcInscriptionGrid->AppendRows( 1);
		m_pcInscriptionGrid->SetRowLabelValue(l_nGridRowNumber, l_sEdgeName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if(!l_pcColList)
			return;

		if(l_pcColList->GetRowCount() > 0)
		{
			wxString l_sExprssion = l_pcColList->GetCell(0, 1);			
			m_pcInscriptionGrid->SetCellValue(l_nGridRowNumber, 0, l_sExprssion);
			m_pcInscriptionGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
			m_pcInscriptionGrid->SetCellAlignment(l_nGridRowNumber, 0 ,wxALIGN_RIGHT, wxALIGN_CENTER);
		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;
	}
}



void SP_DLG_InscriptionOverview::SaveData()
{
	m_pcInscriptionGrid->SaveEditControlValue();

	SP_DS_ColListAttribute* l_pcColList;

	int l_nGridRowNumber = 0;

	for(unsigned j = 0; j < m_pcEdgeVector.size(); j++ )	
	{
		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(m_pcEdgeVector[j]->GetAttribute(SP_DS_CPN_INSCRIPTION) );

		l_pcColList->Clear();

		for (int i = 0; i < m_pcInscriptionGrid->GetNumberCols(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			wxString l_sLabel = m_pcInscriptionGrid->GetColLabelValue(i);
			wxString l_sValue = m_pcInscriptionGrid->GetCellValue(l_nGridRowNumber, i);
			l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);
			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);			
		}

		m_pcEdgeVector[j]->Update( true);

		l_nGridRowNumber++;
	}

	SP_Core::Instance()->GetRootDocument()->Modify(true);

}

void SP_DLG_InscriptionOverview::OnCellValueChanged(wxGridEvent& ev)
{

}

void SP_DLG_InscriptionOverview::OnGridCellSelected(wxGridEvent& ev)
{

	if ( (m_pcInscriptionGrid->GetNumberRows() > 0 )
			&& (m_pcInscriptionGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcInscriptionGrid->GetCellValue(ev.GetRow(),
				ev.GetCol() );
	}

	ev.Skip();
}



void SP_DLG_InscriptionOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}


/*
void SP_DLG_InscriptionOverview::LoadDataForColoredCPN()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Edgeclass* l_pcEdgeclass;
	SP_ListEdge::const_iterator l_itElem;

	bool l_bWhite = false;
	wxString l_sEdgeName;
	wxString l_sLoadName = wxT("-1");

	int l_nGridRowNumber = 0;
	m_pcInscriptionGrid->AppendCols( 1);
	m_pcInscriptionGrid->SetColLabelValue(0, wxT("Main"));

	l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_EDGE );
	for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		( *l_itElem )->Update( true);

		l_sEdgeName = l_sSourceNodeName + wxT("-->") + l_sTargetNodeName;

		m_pcInscriptionGrid->AppendRows( 1);
		m_pcInscriptionGrid->SetRowLabelValue(l_nGridRowNumber, l_sEdgeName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if(!l_pcColList)
			return;

		if(l_pcColList->GetRowCount() > 0)
		{
			wxString l_sExprssion = l_pcColList->GetCell(0, 1);			
			m_pcInscriptionGrid->SetCellValue(l_nGridRowNumber, 0, l_sExprssion);
			m_pcInscriptionGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
			m_pcInscriptionGrid->SetCellAlignment(l_nGridRowNumber, 0 ,wxALIGN_RIGHT, wxALIGN_CENTER);
		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;

	}


	l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_INHIBITOR_EDGE );
	for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		( *l_itElem )->Update( true);

		l_sEdgeName = l_sSourceNodeName + wxT("-->") + l_sTargetNodeName;

		m_pcInscriptionGrid->AppendRows( 1);
		m_pcInscriptionGrid->SetRowLabelValue(l_nGridRowNumber, l_sEdgeName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if(!l_pcColList)
			return;

		if(l_pcColList->GetRowCount() > 0)
		{
			wxString l_sExprssion = l_pcColList->GetCell(0, 1);			
			m_pcInscriptionGrid->SetCellValue(l_nGridRowNumber, 0, l_sExprssion);
			m_pcInscriptionGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
			m_pcInscriptionGrid->SetCellAlignment(l_nGridRowNumber, 0 ,wxALIGN_RIGHT, wxALIGN_CENTER);
		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;
	}


	l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_READ_EDGE );
	for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		( *l_itElem )->Update( true);

		l_sEdgeName = l_sSourceNodeName + wxT("-->") + l_sTargetNodeName;

		m_pcInscriptionGrid->AppendRows( 1);
		m_pcInscriptionGrid->SetRowLabelValue(l_nGridRowNumber, l_sEdgeName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if(!l_pcColList)
			return;

		if(l_pcColList->GetRowCount() > 0)
		{
			wxString l_sExprssion = l_pcColList->GetCell(0, 1);			
			m_pcInscriptionGrid->SetCellValue(l_nGridRowNumber, 0, l_sExprssion);
			m_pcInscriptionGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
			m_pcInscriptionGrid->SetCellAlignment(l_nGridRowNumber, 0 ,wxALIGN_RIGHT, wxALIGN_CENTER);
		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;
	}

	

	if ( (m_pcInscriptionGrid->GetNumberRows() > 0 )
			&& (m_pcInscriptionGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcInscriptionGrid->GetCellValue( 0, 0);

	}

}

void SP_DLG_InscriptionOverview::SaveDataForColoredCPN()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Edgeclass* l_pcEdgeclass;
	SP_ListEdge::const_iterator l_itElem;	

	int l_nGridRowNumber = 0;
	for(unsigned j = 0; j < m_pcEdgeVector.size(); j++ )	
	{
		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(m_pcEdgeVector[j]->GetAttribute(SP_DS_CPN_INSCRIPTION) );

		l_pcColList->Clear();

		for (int i = 0; i < m_pcInscriptionGrid->GetNumberCols(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			wxString l_sLabel = m_pcInscriptionGrid->GetColLabelValue(i);
			wxString l_sValue = m_pcInscriptionGrid->GetCellValue(l_nGridRowNumber, i);
			l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);
			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);			
		}

		m_pcEdgeVector[j]->Update( true);

		l_nGridRowNumber++;
	}

	SP_Core::Instance()->GetRootDocument()->Modify(true);

}
*/


void SP_DLG_InscriptionOverview::CollectEdges()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!l_pcGraph)
		return;	

	SP_DS_Edgeclass* l_pcEdgeclass;
	SP_ListEdge::const_iterator l_itElem;

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Inhibitor Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Read Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Equal Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Reset Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Modifier Edge"));
	if(l_pcEdgeclass)
	{
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}




/*
	wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName() ;	

	//Standard arcs
	if( l_sNetClassName ==  SP_DS_COLPN_CLASS || l_sNetClassName ==  SP_DS_COLEPN_CLASS || l_sNetClassName ==  SP_DS_COLSPN_CLASS)
	{
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	if( l_sNetClassName ==  SP_DS_COLEPN_CLASS || l_sNetClassName ==  SP_DS_COLSPN_CLASS)
	{
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Inhibitor Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Equal Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Reset Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Read Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}
	if( l_sNetClassName ==  SP_DS_COLSPN_CLASS )
	{
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(wxT("Modifier Edge"));
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}

	if( l_sNetClassName ==  SP_DS_COLCPN_CLASS )
	{
		l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_EDGE );
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}		
		l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_INHIBITOR_EDGE );
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}

		l_pcEdgeclass= l_pcGraph->GetEdgeclass( SP_DS_READ_EDGE );
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
		l_pcEdgeclass= l_pcGraph->GetEdgeclass(SP_DS_MODIFIER_EDGE);
		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			m_pcEdgeVector.push_back(*l_itElem);
		}
	}
*/
}
