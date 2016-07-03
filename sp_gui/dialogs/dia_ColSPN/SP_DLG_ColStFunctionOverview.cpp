//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

IMPLEMENT_CLASS( SP_DLG_ColStFunctionOverview, wxDialog )

enum
{

	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_CHECK_FUNCTION,
	SP_ID_BUTTON_FUNCTION_ASSISTENT,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET

};

BEGIN_EVENT_TABLE( SP_DLG_ColStFunctionOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColStFunctionOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColStFunctionOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_CHECK_FUNCTION, SP_DLG_ColStFunctionOverview :: OnCheckFunction )
EVT_BUTTON( SP_ID_BUTTON_FUNCTION_ASSISTENT, SP_DLG_ColStFunctionOverview :: OnFunctionAssistent )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_ColStFunctionOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_ColStFunctionOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_ColStFunctionOverview::OnRenameSet )

END_EVENT_TABLE()

SP_DLG_ColStFunctionOverview::SP_DLG_ColStFunctionOverview(wxString p_sNodeclass,  wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_sNodeclass = p_sNodeclass;

	if (p_sNodeclass == wxT("Transition"))
	{
		m_sFunctionType = wxT("Function") ;
	}
	else if (p_sNodeclass == SP_DS_CONTINUOUS_TRANS )
	{
		m_sFunctionType = wxT("Function") ;
	}
	else if (p_sNodeclass == wxT("Immediate Transition"))
	{
		m_sFunctionType = wxT("Weight") ;
	}
	else if (p_sNodeclass == wxT("Deterministic Transition"))
	{
		m_sFunctionType = wxT("Delay") ;
	}
	else if (p_sNodeclass == wxT("Scheduled Transition"))
	{
		m_sFunctionType = wxT("Periodic") ;
	}



	SetTitle( m_sFunctionType + wxT(" Overview") );

	m_pcNodeclass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(p_sNodeclass);

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row


	m_pcFunctionGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcFunctionGrid->CreateGrid( 0, 0);

	m_pcFunctionGrid->EnableEditing( true);

	m_pcFunctionGrid->SetDefaultColSize( 220, TRUE);
	m_pcFunctionGrid->SetDefaultRowSize( 20, TRUE);

	m_pcFunctionGrid->SetColLabelSize( 16);
	m_pcFunctionGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcFunctionGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcFunctionGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_CHECK_FUNCTION, wxT("Check ")+ m_sFunctionType ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_FUNCTION_ASSISTENT, m_sFunctionType+wxT(" assistent") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_ADDSET, wxT("Add ")+m_sFunctionType+wxT(" set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_DELETESET, wxT("Delete ")+m_sFunctionType+wxT(" set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_RENAMESET, wxT("Rename ")+m_sFunctionType+wxT(" set") ), 0, wxEXPAND | wxALL, 5);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_ColStFunctionOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColStFunctionOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColStFunctionOverview::OnAddSet(wxCommandEvent& p_cEvent)
{

	bool l_bWhite = false;
	wxTextEntryDialog l_pcDialog( this, _T("Name of the new function set:"), wxT("Add function set"));
	wxString l_sName= wxT("F-Set ");

	//only for the Scheduled Transition
	int l_nSetNum;
	if (m_sNodeclass == wxT("Scheduled Transition"))
	{
		l_nSetNum = m_pcFunctionGrid->GetNumberCols();
		l_nSetNum = l_nSetNum/3;
	}
	else
	{
		l_nSetNum = m_pcFunctionGrid->GetNumberCols();
	}

	l_sName.Append(wxString::Format(wxT("%d"), l_nSetNum ) );

	l_pcDialog.SetValue(l_sName);

	int l_nModalResult = l_pcDialog.ShowModal();

	if (wxID_OK == l_nModalResult)
	{

		if (ExistSetName(l_pcDialog.GetValue() ) )
		{
			SP_MESSAGEBOX(wxT("The inputed name is already existing."),
					wxT("Invalid function set name"),
					wxOK | wxICON_ERROR);
			return;
		}

		l_sName = l_pcDialog.GetValue();

		//
		//only for the Scheduled Transition	
		if (m_sNodeclass == wxT("Scheduled Transition"))
		{
			m_pcFunctionGrid->AppendCols( 3 );
			m_pcFunctionGrid->SetColLabelValue(m_pcFunctionGrid->GetNumberCols()- 1, l_sName + wxT(": End"));
			m_pcFunctionGrid->SetColLabelValue(m_pcFunctionGrid->GetNumberCols()- 2, l_sName + wxT(": Repetition"));
			m_pcFunctionGrid->SetColLabelValue(m_pcFunctionGrid->GetNumberCols()- 3, l_sName + wxT(": Begin"));

			int l_nGridRow = 0;
			int l_nGridCol = m_pcFunctionGrid->GetNumberCols() - 3;
			if ( 3 < l_nGridCol)
			{
				for (l_nGridRow = 0; l_nGridRow < m_pcFunctionGrid->GetNumberRows(); l_nGridRow++)
				{
					m_pcFunctionGrid->SetCellValue(l_nGridRow, l_nGridCol,	m_pcFunctionGrid->GetCellValue(l_nGridRow, l_nGridCol- 3) );
					m_pcFunctionGrid->SetCellValue(l_nGridRow, l_nGridCol+1,m_pcFunctionGrid->GetCellValue(l_nGridRow, l_nGridCol- 2) );
					m_pcFunctionGrid->SetCellValue(l_nGridRow, l_nGridCol+2,m_pcFunctionGrid->GetCellValue(l_nGridRow, l_nGridCol- 1) );
				}				
			}
		}
		else
		{
			m_pcFunctionGrid->AppendCols( 1);
			m_pcFunctionGrid->SetColLabelValue(m_pcFunctionGrid->GetNumberCols()- 1, l_sName + wxT(": ") + m_sFunctionType  );

			int l_nGridCol = m_pcFunctionGrid->GetNumberCols() - 1;
			int l_nGridRow = 0;

			if ( 0 < l_nGridCol)
			{

				for (l_nGridRow = 0; l_nGridRow < m_pcFunctionGrid->GetNumberRows(); l_nGridRow++)
				{

					m_pcFunctionGrid->SetCellValue(l_nGridRow, l_nGridCol,
						m_pcFunctionGrid->GetCellValue(l_nGridRow, l_nGridCol
								- 1) );

					m_pcFunctionGrid->SetCellAlignment(l_nGridRow, l_nGridCol,
						wxALIGN_RIGHT, wxALIGN_CENTER);

					m_pcFunctionGrid->SetCellBackgroundColour(l_nGridRow,
						l_nGridCol, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
						(l_bWhite ? l_bWhite = false : l_bWhite = true );

				}

			}
		}

		//

		

	}

}

void SP_DLG_ColStFunctionOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{
	wxArrayInt l_cSelCols = m_pcFunctionGrid->GetSelectedCols();

	if ( ! (l_cSelCols.Count() > 0 ))
	{
		SP_MESSAGEBOX(wxT("Select column(s) to delete."),
				wxT("Invalid function choice"), wxOK | wxICON_ERROR);
		return;
	}

	if (m_sNodeclass == wxT("Scheduled Transition"))
	{
		if (l_cSelCols.Item( 0) == 0 || l_cSelCols.Item( 0) == 1 || l_cSelCols.Item( 0) == 2 || l_cSelCols.Item( 0) == 3)
		{
			SP_MESSAGEBOX(wxT("Predicate or Main function set cannot be removed."), wxT("Error"), wxOK | wxICON_ERROR);
			if (l_cSelCols.Count() == 4)
			{
				return;
			}
			else
			{
				int l_nLow = l_cSelCols.Item( 0) ;
				int l_nUp = l_cSelCols.Item( l_cSelCols.Count()-1 ) ;

				l_nLow = 4;
				l_nUp = (l_nUp-1)/3*3+1;
				m_pcFunctionGrid->DeleteCols( l_nLow, l_nUp);

			}
		}
		else
		{
			int l_nLow = l_cSelCols.Item( 0) ;
			int l_nUp = l_cSelCols.Item( l_cSelCols.Count()-1 ) ;

			l_nLow = (l_nLow-1)/3*3+1;
			l_nUp = (l_nUp-1)/3*3+1;
			m_pcFunctionGrid->DeleteCols( l_nLow, l_nUp);
		}
	}
	else
	{

		if (l_cSelCols.Item( 0) == 0 || l_cSelCols.Item( 0) == 1)
		{
			SP_MESSAGEBOX(wxT("Predicate or Main function set cannot be removed."), wxT("Error"), wxOK | wxICON_ERROR);

			if (l_cSelCols.Count() == 2)
			{
				return;
			}
			else
			{
				m_pcFunctionGrid->DeleteCols( 1, l_cSelCols.Count() - 1);
				return;
			}
		}

		m_pcFunctionGrid->DeleteCols(l_cSelCols.Item( 0), l_cSelCols.Count() );
	}

}

void SP_DLG_ColStFunctionOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{

		wxArrayInt l_pcSelCols = m_pcFunctionGrid->GetSelectedCols();

		if (l_pcSelCols.Count() != 2)
		{

			SP_MESSAGEBOX(wxT("Select a single column to rename."),
					wxT("Invalid function choice"), wxOK | wxICON_ERROR);
			return;

		}

		if (l_pcSelCols.Item( 0) == 0 || l_pcSelCols.Item( 0) == 1)
		{

			SP_MESSAGEBOX(wxT("Main function set cannot be renamed."),
					wxT("Invalid function set choice"), wxOK | wxICON_ERROR);
			return;

		}

		wxTextEntryDialog l_pcDialog( this, wxT("New name of the function set: ") +
		m_pcFunctionGrid->GetColLabelValue( l_pcSelCols.Item( 0 ) ), wxT("Rename function set"));

		l_pcDialog.SetValue(m_pcFunctionGrid->GetColLabelValue(l_pcSelCols.Item( 0) ) );

		int l_nModalResult = l_pcDialog.ShowModal();

		if (wxID_OK == l_nModalResult)
		{

			if (ExistSetName(l_pcDialog.GetValue() ) )
			{

				SP_MESSAGEBOX(
						wxT("Do not use an already existing function set name."),
						wxT("Invalid function set name"),
						wxOK | wxICON_ERROR);

				return;

			}

			m_pcFunctionGrid->SetColLabelValue(l_pcSelCols.Item( 0),
					l_pcDialog.GetValue() );

		}

	}

}

void SP_DLG_ColStFunctionOverview::LoadData()
{
	//SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	//SP_DS_Nodeclass* l_pcNodeclass;
	//l_pcNodeclass = l_pcGraph->GetNodeclass(wxT("Transition"));

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	m_mRow2Node.clear();

	LoadSetNames();

	for (l_itElem = m_pcNodeclass->GetElements()->begin(); l_itElem != m_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		SP_DS_Node* l_pcTransitionNode = ( *l_itElem );
		
		wxString l_sFunctionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransitionNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_pcFunctionGrid->AppendRows( 1);
		m_pcFunctionGrid->SetRowLabelValue(l_nGridRowNumber, l_sFunctionName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransitionNode->GetAttribute(wxT("FunctionList")) );

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if(i >= 1)
			{
				m_pcFunctionGrid->AppendRows( 1);
				m_pcFunctionGrid->SetRowLabelValue(l_nGridRowNumber, wxT(""));
			}

			for (unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
			{				
				m_pcFunctionGrid->SetCellValue(l_nGridRowNumber, j, l_pcColList->GetCell(i, j));				
			}
			m_mRow2Node[l_nGridRowNumber] = l_pcTransitionNode;
			
			l_nGridRowNumber++;
		}		

	}

	if ( (m_pcFunctionGrid->GetNumberRows() > 0 ) && (m_pcFunctionGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcFunctionGrid->GetCellValue( 0, 0);
	}

}

void SP_DLG_ColStFunctionOverview::SaveData()
{
	m_pcFunctionGrid->SaveEditControlValue();

	//SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	//SP_DS_Nodeclass* l_pcNodeclass;
	//l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Transition"));

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	for (l_itElem = m_pcNodeclass->GetElements()->begin(); l_itElem != m_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("FunctionList")) );
		l_pcColList->Clear();
	

		int l_nBegin = l_nGridRowNumber;		

		l_pcColList->RemoveAllColumns(); //for colored Petri Net

		for(unsigned i = 0 ; i < (unsigned int)(m_pcFunctionGrid->GetNumberCols()); i++ )
			l_pcColList->AppendEmptyColum();
		for(unsigned i = 0; i < l_pcColList->GetColCount(); i++)
			l_pcColList->SetColLabel(i,m_pcFunctionGrid->GetColLabelValue(i));


		l_pcColList->AppendEmptyRow();

		for(int j = l_nBegin; j < m_pcFunctionGrid->GetNumberRows(); j++)
		{						
			l_nGridRowNumber++;

			for (int i = 0; i < m_pcFunctionGrid->GetNumberCols(); i++)
			{
				wxString l_sValue = m_pcFunctionGrid->GetCellValue(j, i);
				l_pcColList->SetCell(j-l_nBegin, i, l_sValue);
			}
			if( j < m_pcFunctionGrid->GetNumberRows()-1 )
			{
				wxString l_sRowLabelValue = m_pcFunctionGrid->GetRowLabelValue(j+1);
				if(l_sRowLabelValue == wxT(""))
					l_pcColList->AppendEmptyRow();
				else
					break;
			}			

		}

	}
	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_ColStFunctionOverview::LoadSetNames()
{
	//SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	//if(l_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0)
	if(m_pcNodeclass->GetElements()->size() > 0)
	{
		//SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
		SP_DS_Node* l_pcNode = m_pcNodeclass->GetElements()->front();
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		for(unsigned int i = 0; i < l_pcAttr->GetColCount(); i++ )
		{
			wxString l_sSetName = l_pcAttr->GetColLabel(i);
			m_pcFunctionGrid->AppendCols(1);
			m_pcFunctionGrid->SetColLabelValue(i, l_sSetName);
		}
	}
}

bool SP_DLG_ColStFunctionOverview::ExistSetName(const wxString& p_sName)
{

	for (int i = 0; i < m_pcFunctionGrid->GetNumberCols(); i++)
	{
		if (p_sName== m_pcFunctionGrid->GetColLabelValue(i) )
		{
			return true;
		}
	}

	return false;

}

void SP_DLG_ColStFunctionOverview::OnCellClick(wxGridEvent& ev)
{

	wxString l_pcReturnText = m_pcFunctionGrid->GetCellValue( ev.GetRow(), ev.GetCol());
	SP_DS_Node* l_pcNode = NULL;

	//SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	//SP_DS_Nodeclass* l_pcNodeclass;
	//l_pcNodeclass= l_pcGraph->GetNodeclass(wxT("Transition"));

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	for (l_itElem = m_pcNodeclass->GetElements()->begin(); l_itElem != m_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		if (l_nGridRowNumber++ == ev.GetRow() )
		{
			l_pcNode = ( *l_itElem );
			break;
		}
	}

	if ( !l_pcNode)
	{
		return;
	}

	SP_DLG_StFunctionAssistent l_cDlg(SP_ST_PARSER_TYPE_CHECK_FUNCTION,
			l_pcNode, l_pcReturnText, this);

	if (l_cDlg.ShowModal() == wxID_OK)
	{
		m_pcFunctionGrid->SetCellValue(ev.GetRow(), ev.GetCol(), l_cDlg.GetReturnText());
	}

	l_cDlg.Destroy();

	ev.Skip();

}

void SP_DLG_ColStFunctionOverview::OnCheckFunction(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	if( m_pcFunctionGrid->GetGridCursorCol() == 0)
	{
		SP_MESSAGEBOX(wxT("Please select function to be checked"), wxT("Notication"), wxOK | wxICON_INFORMATION );
		return;
	}

	SP_DS_StParser l_cParser;

	wxArrayString* l_plMissingVariableList = 0;

	wxArrayString* l_psUnusedModifierPlaceList = 0;
	wxArrayString* l_psUnusedPrePlaceList = 0;

	wxString l_sVariables = wxT("");
	wxString l_sFormula = m_pcFunctionGrid->GetCellValue(
			m_pcFunctionGrid->GetGridCursorRow(),
			m_pcFunctionGrid->GetGridCursorCol() );
	
	SP_ListNode::const_iterator l_itElem;

	SP_DS_Node* l_pcTransitionNode = NULL;

	int l_nGridRowNumber = 0;

	for (l_itElem = m_pcNodeclass->GetElements()->begin(); l_itElem != m_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_pcTransitionNode = ( *l_itElem );

		if (m_pcFunctionGrid->GetGridCursorRow() == l_nGridRowNumber++)
		{
			break;
		}
	}

	bool l_bFormulaCheck = false;

	//Colored SPN
	SP_DS_Graph* m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS )
	{
		
		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.Initialize())
			return;
		l_bFormulaCheck = l_cSyntaxChecking.CheckFormula(l_pcTransitionNode,l_sFormula,&l_cParser,true);

	}
	else
	{
		l_bFormulaCheck = l_cParser.CheckFormulaFunction(l_sFormula, l_pcTransitionNode);

	}

	
	if (!l_bFormulaCheck )	
	{

		l_plMissingVariableList = l_cParser.GetMissingVariableList();

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sFormula;
		l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

		for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
		{

			l_sVariables += ( *l_plMissingVariableList )[ i ];
			l_sVariables += wxT("\n");

		}

		SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR);

	}
	else
	{
		//Consistence checking results		
		l_psUnusedModifierPlaceList = l_cParser.GetUnusedModifierPlaceList();
		l_psUnusedPrePlaceList = l_cParser.GetUnusedPrePlaceList();

		if(l_psUnusedModifierPlaceList->size() > 0 || l_psUnusedPrePlaceList->size() > 0)
		{
			wxString l_sWarningVariables = wxT("");

			if(l_psUnusedModifierPlaceList->size() > 0 )
			{
				l_sWarningVariables = wxT("The formula has unused modifier places: \n ");

				for (unsigned int i = 0; i < l_psUnusedModifierPlaceList->Count(); i++)
				{

					l_sWarningVariables += (*l_psUnusedModifierPlaceList)[i];
					l_sWarningVariables += wxT("\n");
				}
			}

			
			if( l_psUnusedPrePlaceList->size() > 0) 
			{
				l_sWarningVariables += wxT("The formula has unused preplaces: \n ");

				for (unsigned int i = 0; i < l_psUnusedPrePlaceList->Count(); i++)
				{

					l_sWarningVariables += (*l_psUnusedPrePlaceList)[i];
					l_sWarningVariables += wxT("\n");
				}		
			}

			SP_MESSAGEBOX(l_sWarningVariables, wxT("Consistence checking"), wxOK | wxICON_WARNING );
		}
		/////////////////////////////

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sFormula;
		l_sVariables += wxT("\" is correct.");

		new wxTipWindow(this, l_sVariables, 1000);
	}

}

void SP_DLG_ColStFunctionOverview::OnFunctionAssistent(wxCommandEvent& p_cEvent)
{

	if (m_pcFunctionGrid->GetNumberRows() == 0)
	{
		return;
	}

	wxString l_pcReturnText = m_pcFunctionGrid->GetCellValue(m_pcFunctionGrid->GetGridCursorRow(), m_pcFunctionGrid->GetGridCursorCol());	

	SP_ListNode::const_iterator l_itElem;

	SP_DS_Node* l_pcTransitionNode = NULL;

	int l_nGridRowNumber = 0;

	l_nGridRowNumber = m_pcFunctionGrid->GetGridCursorRow();

	l_pcTransitionNode = m_mRow2Node[l_nGridRowNumber];

	if(!l_pcTransitionNode)
		return;

	/*
	for (l_itElem = m_pcNodeclass->GetElements()->begin(); l_itElem != m_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		l_pcTransitionNode = ( *l_itElem );
		if (m_pcFunctionGrid->GetGridCursorRow() == l_nGridRowNumber++)
		{
			break;
		}
	}
	*/

	SP_DLG_StFunctionAssistent* l_pcDlg = new SP_DLG_StFunctionAssistent(SP_ST_PARSER_TYPE_CHECK_FUNCTION,
			l_pcTransitionNode, l_pcReturnText, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		m_pcFunctionGrid->SetCellValue(m_pcFunctionGrid->GetGridCursorRow(),
									   m_pcFunctionGrid->GetGridCursorCol(), l_pcDlg->GetReturnText() );

	}

	l_pcDlg->Destroy();

}

void SP_DLG_ColStFunctionOverview::RefreshFrame()
{

	SP_Core::Instance()->GetRootDocument()->Refresh();

}
