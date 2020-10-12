//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#include "sp_gui/dialogs/dia_CPN/SP_WDG_Guard.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_GuardOverview.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include <wx/event.h>

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ExpressionAssistent.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include <wx/tipwin.h>

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

enum
{
	SP_ID_BUTTON_EDIT, 
	SP_ID_BUTTON_CHECK,
	SP_ID_BUTTON_OVERVIEW,
	SP_ID_BUTTON_ASSISTENT,
	SP_ID_GRID_GUARD,
	SP_ID_BUTTON_BINDING

};
BEGIN_EVENT_TABLE( SP_WDG_Guard, SP_WDG_DialogBase )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_WDG_Guard::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_WDG_Guard::OnGridCellValueChanged ) 
#endif

EVT_GRID_SELECT_CELL( SP_WDG_Guard::OnGridCellSelected )

END_EVENT_TABLE()

SP_WDG_Guard::SP_WDG_Guard(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_Guard::~SP_WDG_Guard()
{
}

SP_WDG_DialogBase* SP_WDG_Guard::Clone()
{
	return new SP_WDG_Guard(GetName(), GetDialogOrdering());
}

bool SP_WDG_Guard::AddToDialog(
		const SP_ListAttribute* p_ptlAttributes,
		SP_DLG_ShapeProperties* p_pcDlg,
		bool p_bOverview)
{

	CHECK_POINTER( p_ptlAttributes, return FALSE );
	CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
	CHECK_POINTER( p_pcDlg, return FALSE );

	m_pcDlg = p_pcDlg;

	// remember the list of attributes, we display
	m_tlAttributes = *p_ptlAttributes;

	SP_DS_Attribute* l_pcAttr = (*p_ptlAttributes->begin());
	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	m_bMultiple = (p_ptlAttributes->size() > 1);

    wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER( l_pcPage, return FALSE );

	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 1, wxALL , 5);
	AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcGuardGrid = new wxGrid(l_pcPage, SP_ID_GRID_GUARD + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER );

	m_pcGuardGrid->CreateGrid(0, 0);

	m_pcGuardGrid->EnableEditing(true);
	m_pcGuardGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcGuardGrid->SetDefaultColSize(120, TRUE );
	m_pcGuardGrid->SetDefaultRowSize(20, TRUE );

	m_pcGuardGrid->SetColLabelSize(16);
	m_pcGuardGrid->SetRowLabelSize(0);

	m_pcGuardGrid->AppendCols(2);

	m_pcGuardGrid->SetColLabelValue(0, wxT("Guard set") );
	m_pcGuardGrid->SetColSize(0, 100);

	m_pcGuardGrid->SetColLabelValue(1, wxT("Guard") );
	m_pcGuardGrid->SetColSize(1, 100);

	LoadData();

	l_pcSizer->Add(m_pcGuardGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_EDIT + m_nDialogID
			+ wxID_HIGHEST, wxT("Edit guard") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Apply and check guard") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL); 

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ASSISTENT + m_nDialogID
			+ wxID_HIGHEST, wxT("Guard assistent") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, wxT("Guard overview") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL); 

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_BINDING + m_nDialogID
			+ wxID_HIGHEST, wxT("Show bindings") ), 1, wxALL, 5);
	
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);


	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_Guard::OnDlgOk()
{	
	SaveData();
	return SP_WDG_DialogBase::OnDlgOk();
}

bool SP_WDG_Guard::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL( SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	bool l_bWhite = true;

	if (m_pcGuardGrid->GetNumberRows() > 0)
	{
		m_pcGuardGrid->DeleteRows(0, m_pcGuardGrid->GetNumberRows());
	}

	m_pcGuardGrid->AppendRows(l_pcColList->GetRowCount());

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		m_pcGuardGrid->SetCellValue(i, 0, l_pcColList->GetCell(i,0).c_str());
		m_pcGuardGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcGuardGrid->SetReadOnly(i, 0, true);

		m_pcGuardGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		/////////////////
		wxString l_sValue = SP_WILDCARD;
		if( !m_bMultiple ) 
			l_sValue = l_pcColList->GetCell(i,1);
		////////////////////

		m_pcGuardGrid->SetCellValue(i, 1, l_sValue );		
		m_pcGuardGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);

		m_pcGuardGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	return true;

}

bool SP_WDG_Guard::SaveData()
{
	m_pcGuardGrid->SaveEditControlValue();

	if( m_bMultiple )
	{
			//to deal with a group of nodes
			for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				for (int l_nRow = 0; l_nRow < m_pcGuardGrid->GetNumberRows(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcGuardGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcGuardGrid->GetCellValue(l_nRow, l_nCol);
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

	wxString l_sMainMarking;

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMarkingAttr);

	for (int i = 0; i < m_pcGuardGrid->GetNumberRows(); i++)
	{
		l_pcColList->SetCell(i, 0, m_pcGuardGrid->GetCellValue(i, 0));
		l_pcColList->SetCell(i, 1, m_pcGuardGrid->GetCellValue(i, 1));		
	}	

	return true;

}

void SP_WDG_Guard::OnEdit(wxCommandEvent& p_cEvent)
{

	if (m_pcGuardGrid->GetNumberRows() == 0)
	{
		return;
	}

	int l_nEditRowPos = m_pcGuardGrid->GetGridCursorRow();
	m_sOldCellValue = m_pcGuardGrid->GetCellValue(l_nEditRowPos, 1);
	m_pcGuardGrid->SelectBlock(l_nEditRowPos, 1, l_nEditRowPos, 1);
	m_pcGuardGrid->SetFocus();

}

void SP_WDG_Guard::OnOverview(wxCommandEvent& p_cEvent)
{

	SP_DLG_GuardOverview l_cDlg(m_pcDlg);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadData();
	}

	l_cDlg.Destroy();

}

void SP_WDG_Guard::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnEdit);
	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnCheck);
	Connect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnAssistent);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnOverview);
	
	Connect(
			SP_ID_BUTTON_BINDING + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnShowbindings);

}

void SP_WDG_Guard::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_EDIT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnEdit);

	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnCheck);

	Disconnect(
			SP_ID_BUTTON_ASSISTENT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnAssistent);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnOverview);
	
	Disconnect(
			SP_ID_BUTTON_BINDING + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_Guard::OnShowbindings);

}

void SP_WDG_Guard::OnGridCellSelected(wxGridEvent& ev)
{	
	wxGrid* l_pcGrid = dynamic_cast<wxGrid*>(ev.GetEventObject());		
	wxWindowID l_WindowID = l_pcGrid->GetId();

	if(l_WindowID == SP_ID_GRID_GUARD)
		m_sOldCellValue = m_pcGuardGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();
}

void SP_WDG_Guard::OnGridCellValueChanged(wxGridEvent& ev)
{
	ev.Skip();
}


void SP_WDG_Guard::OnAssistent(wxCommandEvent& p_cEvent)
{
	m_pcGuardGrid->SaveEditControlValue();

	if (m_pcGuardGrid->GetNumberRows() == 0)
	{
		return;
	}
	wxString l_pcReturnText = m_pcGuardGrid->GetCellValue(0, 1);

	SP_CPN_ValueAssign l_cValueAssign;		
	l_cValueAssign.InitializeColorset(m_cColorSetClass);	

	SP_DLG_ExpressionAssistent* l_pcDlg = new SP_DLG_ExpressionAssistent(EXPRESSION_ARCGUARD, wxT(""), wxT("Place"),m_cColorSetClass,l_pcReturnText, m_pcDlg);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		m_pcGuardGrid->SetCellValue(0, 1, l_pcDlg->GetReturnText());
	}

	l_pcDlg->Destroy();

}

void SP_WDG_Guard::OnCheck( wxCommandEvent& p_cEvent )
{
	SaveData();//do apply before check, by george
	
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return;

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
	if(!l_pcNode)
		return;

	/////////////////
	SP_CPN_SyntaxChecking l_cSyntaxChecking;	
	if( !l_cSyntaxChecking.Initialize())
			return;

	if( ! l_cSyntaxChecking.CheckGuard(l_pcNode) )
	 {//showing the tip, by george
		wxString l_sTip = wxT("");
		l_sTip = wxT("The guard ");
		l_sTip += wxT(" is not correct.");
		new wxTipWindow(m_pcDlg, l_sTip, 1000);
		return;
	 }

	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The guard ");	
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(m_pcDlg, l_sVariables, 1000);

}


void SP_WDG_Guard::OnShowbindings( wxCommandEvent& p_cEvent )
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return;

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
	if(!l_pcNode)
		return;

	SP_LOGMESSAGE( wxT("The binding information for transition ") + dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue() + wxT(" is as follows: \n") );	

	SP_DS_ColPN_Unfolding l_cComputeBingdings;
	l_cComputeBingdings.ComputeBindings(l_pcNode);

}
