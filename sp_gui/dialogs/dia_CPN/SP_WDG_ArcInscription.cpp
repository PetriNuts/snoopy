//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_InscriptionOverview.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ExpressionAssistent.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include <wx/event.h>

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include <wx/tipwin.h>


enum
{
	SP_ID_BUTTON_EDIT, 
	SP_ID_BUTTON_OVERVIEW,
	SP_ID_BUTTON_ASSISTENT,
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_CHECK

};
BEGIN_EVENT_TABLE( SP_WDG_ArcInscription, SP_WDG_DialogBase )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_WDG_ArcInscription::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_WDG_ArcInscription::OnGridCellValueChanged ) 
#endif

EVT_GRID_SELECT_CELL( SP_WDG_ArcInscription::OnGridCellSelected )

END_EVENT_TABLE()

SP_WDG_ArcInscription::SP_WDG_ArcInscription(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering),
	m_pcInscriptionGrid(NULL),
	m_pcDlg(NULL)
{
}

SP_WDG_ArcInscription::~SP_WDG_ArcInscription()
{
}

SP_WDG_DialogBase*
SP_WDG_ArcInscription::Clone()
{
	return new SP_WDG_ArcInscription(GetName(), GetDialogOrdering());
}

bool SP_WDG_ArcInscription::AddToDialog(
		const SP_ListAttribute* p_ptlAttributes,
		SP_DLG_ShapeProperties* p_pcDlg,
		bool p_bOverview)
{

	CHECK_POINTER( p_ptlAttributes, return FALSE );
	CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
	CHECK_POINTER( p_pcDlg, return FALSE );

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sNetClass = l_pcGraph->GetNetclass()->GetName();

	if(l_sNetClass == SP_DS_COLCPN_CLASS)
	{
		m_sNodeClass = SP_DS_CONTINUOUS_PLACE;
	}
	else
		//(l_sNetClass == SP_DS_COLSPN_CLASS)
	{
		m_sNodeClass = SP_DS_DISCRETE_PLACE;
	}

	m_pcDlg = p_pcDlg;

	// remember the list of attributes, we display
	m_tlAttributes = *p_ptlAttributes;

	SP_DS_Attribute* l_pcAttr = (*p_ptlAttributes->begin());

	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

    wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("") ), 1, wxALL , 5);

	AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcInscriptionGrid = new wxGrid(l_pcPage, SP_ID_GRID_MARKING + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxSize(350, 300),
			wxSUNKEN_BORDER );

	m_pcInscriptionGrid->CreateGrid(0, 0);

	m_pcInscriptionGrid->EnableEditing(true);
	m_pcInscriptionGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcInscriptionGrid->SetDefaultColSize(120, TRUE );
	m_pcInscriptionGrid->SetDefaultRowSize(20, TRUE );

	m_pcInscriptionGrid->SetColLabelSize(16);
	m_pcInscriptionGrid->SetRowLabelSize(0);

	m_pcInscriptionGrid->AppendCols(2);

	m_pcInscriptionGrid->SetColLabelValue(0, wxT("Expression set") );
	m_pcInscriptionGrid->SetColSize(0, 100);

	m_pcInscriptionGrid->SetColLabelValue(1, wxT("Expression") );
	m_pcInscriptionGrid->SetColSize(1, 200);

	LoadData();

	l_pcSizer->Add(m_pcInscriptionGrid, 1, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_EDIT + m_nDialogID
			+ wxID_HIGHEST, wxT("Edit expression") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);	

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Check expression ") ), 1, wxALL, 5);


	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	
	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ASSISTENT + m_nDialogID
			+ wxID_HIGHEST, wxT("Expression assistent") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, wxT("Expression overview") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_ArcInscription::OnDlgOk()
{

	SaveData();
	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_ArcInscription::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL( SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);	

	bool l_bWhite = false;

	if (m_pcInscriptionGrid->GetNumberRows() > 0)
	{
		m_pcInscriptionGrid->DeleteRows(0, m_pcInscriptionGrid->GetNumberRows());
	}

	m_pcInscriptionGrid->AppendRows(1);

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		if( i > 0 )
		{
			break;
		}

		m_pcInscriptionGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcInscriptionGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcInscriptionGrid->SetReadOnly(i, 0, true);

		m_pcInscriptionGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		/////////////////
		wxString l_sValue = SP_WILDCARD;
		if( !m_bMultiple ) 
			l_sValue = l_pcColList->GetCell(i,1);
		////////////////////

		m_pcInscriptionGrid->SetCellValue(i, 1, l_sValue );
		m_pcInscriptionGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);

		m_pcInscriptionGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	return true;

}

bool SP_WDG_ArcInscription::SaveData()
{	
	m_pcInscriptionGrid->SaveEditControlValue();

	if( m_bMultiple )
	{
			//to deal with a group of nodes
			for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				for (int l_nRow = 0; l_nRow < m_pcInscriptionGrid->GetNumberRows(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcInscriptionGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcInscriptionGrid->GetCellValue(l_nRow, l_nCol);
						if((m_bMultiple && l_sValue != SP_WILDCARD) )
						{
							l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);
						}
					}
				}
			}
			
		return true;
	}
	
	SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();;
	SP_DS_Attribute* l_pcMarkingAttr = *l_Iter;

	CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );	

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMarkingAttr);
	for (int i = 0; i < m_pcInscriptionGrid->GetNumberRows(); i++)
	{
		l_pcColList->SetCell(i, 0, m_pcInscriptionGrid->GetCellValue(i, 0));
		l_pcColList->SetCell(i, 1, m_pcInscriptionGrid->GetCellValue(i, 1));		
	}
	
	return true;

}

void SP_WDG_ArcInscription::OnEdit(wxCommandEvent& p_cEvent)
{

	if (m_pcInscriptionGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcInscriptionGrid->GetGridCursorRow();

	m_sOldCellValue = m_pcInscriptionGrid->GetCellValue(l_nEditRowPos, 1);

	m_pcInscriptionGrid->SelectBlock(l_nEditRowPos, 1, l_nEditRowPos, 1);
	m_pcInscriptionGrid->SetFocus();

}

void SP_WDG_ArcInscription::OnOverview(wxCommandEvent& p_cEvent)
{

	SP_DLG_InscriptionOverview l_cDlg(m_pcDlg);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadData();
	}
	l_cDlg.Destroy();

}

void SP_WDG_ArcInscription::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnEdit);
	Connect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnAssistent);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnOverview);
	
	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnCheck);
}

void SP_WDG_ArcInscription::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnEdit);

	Disconnect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnAssistent);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnOverview);
	
	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ArcInscription::OnCheck);
}

void SP_WDG_ArcInscription::OnGridCellSelected(wxGridEvent& ev)
{

	m_sOldCellValue = m_pcInscriptionGrid->GetCellValue(ev.GetRow(), ev.GetCol());
	ev.Skip();

}

void SP_WDG_ArcInscription::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{
}

void SP_WDG_ArcInscription::OnAssistent(wxCommandEvent& p_cEvent)
{
	if (m_pcInscriptionGrid->GetNumberRows() == 0)
	{
		return;
	}
	wxString l_pcReturnText = m_pcInscriptionGrid->GetCellValue(0, 1);

	SP_DS_Attribute* l_pcAttr = m_tlAttributes.front();
	SP_DS_TextAttribute* m_pcNameAttibute;
	wxString l_sColorSet;
	SP_DS_Edge* l_pcEdge = NULL;
	SP_DS_Node* l_pcNode = NULL;
	wxString l_sPlaceType;

	if(l_pcAttr)
		l_pcEdge = dynamic_cast<SP_DS_Edge*> (l_pcAttr->GetParent());
	if(l_pcEdge)
		l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetSource());
	if(l_pcNode)
		if(l_pcNode->GetNodeclass()->GetName() == m_sNodeClass)
		{
			m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		    if(m_pcNameAttibute)
				l_sColorSet = m_pcNameAttibute->GetValue();			
		}

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetTarget());
	if(l_pcNode)
		if(l_pcNode->GetNodeclass()->GetName() == m_sNodeClass )
		{
			m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
			if(m_pcNameAttibute)
				l_sColorSet = m_pcNameAttibute->GetValue();
		}


	SP_CPN_ValueAssign l_cValueAssign;		
	l_cValueAssign.InitializeColorset(m_cColorSetClass);	

	SP_DLG_ExpressionAssistent* l_pcDlg = new SP_DLG_ExpressionAssistent(EXPRESSION_ARCGUARD, l_sColorSet, m_sNodeClass, m_cColorSetClass,l_pcReturnText, m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		m_pcInscriptionGrid->SetCellValue(0, 1, l_pcDlg->GetReturnText());
	}

	l_pcDlg->Destroy();

}


void SP_WDG_ArcInscription::OnCheck(wxCommandEvent& p_cEvent)
{

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSet;
	SP_DS_Edge* l_pcEdge;
	SP_DS_Node* l_pcNode;
	SP_DS_Node* l_pcPlaceNode;
	SP_DS_Node* l_pcTransitionNode;

	if(!l_pcAttr)
		return;

	l_pcEdge = dynamic_cast<SP_DS_Edge*> (l_pcAttr->GetParent());

	if(!l_pcEdge)
		return;
	
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetSource());

	if(!l_pcNode)
		return;

	if(l_pcNode->GetNodeclass()->GetName() == m_sNodeClass )
		l_pcPlaceNode = l_pcNode;
	else
		l_pcTransitionNode = l_pcNode;

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetTarget());
	
	if(!l_pcNode)
		return;

	if(l_pcNode->GetNodeclass()->GetName() == m_sNodeClass )
		l_pcPlaceNode = l_pcNode;
	else
		l_pcTransitionNode = l_pcNode;



	SP_CPN_SyntaxChecking l_cSyntaxChecking;	
	if( !l_cSyntaxChecking.Initialize())
			return;

	if( !l_cSyntaxChecking.CheckArcExpression(l_pcPlaceNode,l_pcEdge,l_pcTransitionNode) )
		return;



	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The arc expression definition");
	//l_sVariables += l_sPredicate;
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(m_pcDlg, l_sVariables, 1000);
}
