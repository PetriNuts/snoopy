//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>

#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"

IMPLEMENT_CLASS( SP_DLG_ColListOverview, wxDialog )

enum
{

	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET

};

BEGIN_EVENT_TABLE( SP_DLG_ColListOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColListOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColListOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_ColListOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_ColListOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_ColListOverview::OnRenameSet )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_ColListOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_ColListOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_ColListOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_ColListOverview::SP_DLG_ColListOverview(const wxString& p_sAttrName, wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
m_sAttrName(p_sAttrName), m_chSeperator(wxT(':'))
{
	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	m_pcValueGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcValueGrid->CreateGrid( 0, 0);
	m_pcValueGrid->EnableEditing( true);

	m_pcValueGrid->SetDefaultColSize( 120, TRUE);
	m_pcValueGrid->SetDefaultRowSize( 20, TRUE);

	m_pcValueGrid->SetColLabelSize( 16);
	m_pcValueGrid->SetRowLabelSize( 80);

	l_pcGridSizer->Add(m_pcValueGrid, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcValueGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_ADDSET, wxT("Add List") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_DELETESET, wxT("Delete List") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_RENAMESET, wxT("Rename List") ), 0, wxEXPAND | wxALL, 5);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND );
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND );

	m_pcSizer->Add(l_pcGridSizer, 1, wxALIGN_CENTER | wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

	SetSizerAndFit(m_pcSizer);
}

void SP_DLG_ColListOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColListOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColListOverview::OnAddSet(wxCommandEvent& p_cEvent)
{
	
	wxString l_sName= _T("List ");

	SP_DS_ColListAttribute* l_pcAttr = NULL;

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	if( l_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		
		SP_DS_Nodeclass* l_pcNodeClass;
		
		if( m_sAttrName == wxT("DelayList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
			
			if(l_pcNodeClass->GetElements()->size() > 0)
			{		
				SP_DS_Node* l_pcNode = l_pcNodeClass->GetElements()->front();
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
				l_sName.Append(wxString::Format(wxT("%d"), (m_pcValueGrid->GetNumberCols()/(l_pcAttr->GetColCount()-1)) ) );
			}
		}
		if( m_sAttrName == wxT("PeriodicList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
			
			if(l_pcNodeClass->GetElements()->size() > 0)
			{		
				SP_DS_Node* l_pcNode = l_pcNodeClass->GetElements()->front();
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
				l_sName.Append(wxString::Format(wxT("%d"), (m_pcValueGrid->GetNumberCols()/(l_pcAttr->GetColCount()-1)) + 1) );
			}
		}
		
	}
	else
	{
		l_pcAttr = m_vAttributes[0];
		l_sName.Append(wxString::Format(wxT("%d"), (m_pcValueGrid->GetNumberCols()/(l_pcAttr->GetColCount()-1)) + 1) );
	}

	bool l_bWhite = true;
	wxTextEntryDialog l_pcDialog( this, _T("Name of the new list:"), wxT("Add list"));

	

	l_pcDialog.SetValue(l_sName);

	int l_nModalResult = l_pcDialog.ShowModal();

	if (wxID_OK == l_nModalResult)
	{

		if (ExistName(l_pcDialog.GetValue()) )
		{

			SP_MESSAGEBOX(wxT("The inputed name is already existing."),
					wxT("Invalid parameter set name"),
					wxOK | wxICON_ERROR);

			return;

		}

		l_sName = l_pcDialog.GetValue();

		for(unsigned int l_nCol = 1; l_nCol < l_pcAttr->GetColCount(); l_nCol++)
		{
			wxString l_ColLabel = l_sName;
			m_pcValueGrid->AppendCols(1);
			//
			wxString l_sColLabel = l_pcAttr->GetColLabel(l_nCol);
			l_sColLabel = l_sColLabel.AfterFirst(wxT(':'));
			//
			l_ColLabel << m_chSeperator << l_sColLabel; //l_pcAttr->GetColLabel(l_nCol);
			m_pcValueGrid->SetColLabelValue(m_pcValueGrid->GetNumberCols() - 1,	l_ColLabel);
		}

		for (int l_nGridRow = 0; l_nGridRow < m_pcValueGrid->GetNumberRows(); l_nGridRow++)
		{
			for( unsigned int l_nCol = 1; l_nCol < l_pcAttr->GetColCount(); l_nCol++)
			{
				int l_nNumberAttr = l_pcAttr->GetColCount()-1;
				int l_nGridCol = m_pcValueGrid->GetNumberCols()-1-l_nNumberAttr+l_nCol;
				m_pcValueGrid->SetCellValue(l_nGridRow, l_nGridCol,
						m_pcValueGrid->GetCellValue(l_nGridRow, l_nGridCol-l_nNumberAttr) );

				m_pcValueGrid->SetCellAlignment(l_nGridRow, l_nGridCol,
						wxALIGN_RIGHT, wxALIGN_CENTER);

				m_pcValueGrid->SetCellBackgroundColour(l_nGridRow, l_nGridCol,
						(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
			}
			(l_bWhite ? l_bWhite = false : l_bWhite = true );
		}
	}
}

//TODO: Attention sort function for wxArrayInt
static int wxCMPFUNC_CONV IntCompareFunction(int *first, int *second)
{
return (*first) > (*second);
}

void SP_DLG_ColListOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcValueGrid->GetSelectedCols();

	if ( ! (l_cSelCols.Count() > 0 ))
	{

		SP_MESSAGEBOX(wxT("Select column(s) to delete."),
				wxT("Invalid list choice"), wxOK | wxICON_ERROR);
		return;

	}
	//sortfunction see above
	l_cSelCols.Sort(IntCompareFunction);

	int l_nDeleteFrom=0;
	int l_nDeleteTo=0;
	for(int l_nCol = 0; l_nCol < m_pcValueGrid->GetNumberCols(); l_nCol++)
	{
		if(!l_nDeleteFrom && m_pcValueGrid->GetColLabelValue(l_nCol).BeforeFirst(m_chSeperator).IsSameAs(m_pcValueGrid->GetColLabelValue(l_cSelCols.Item(0)).BeforeFirst(m_chSeperator)))
		{
			l_nDeleteFrom = l_nCol;
		}
		if(m_pcValueGrid->GetColLabelValue(l_nCol).BeforeFirst(m_chSeperator).IsSameAs(m_pcValueGrid->GetColLabelValue(l_cSelCols.Item(l_cSelCols.GetCount()-1)).BeforeFirst(m_chSeperator)))
		{
			l_nDeleteTo = l_nCol;
		}
	}

	for(int i = l_nDeleteFrom; i<= l_nDeleteTo; i++)
	{
		if(m_pcValueGrid->GetColLabelValue(i).BeforeFirst(m_chSeperator).IsSameAs(wxT("Main")))
			l_nDeleteFrom++;
	}
	if(l_nDeleteFrom<=l_nDeleteTo)
	{
		m_pcValueGrid->DeleteCols(l_nDeleteFrom, l_nDeleteTo-l_nDeleteFrom+1);
	}

}

void SP_DLG_ColListOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{

		wxArrayInt l_pcSelCols = m_pcValueGrid->GetSelectedCols();

		if (l_pcSelCols.Count() != 1)
		{

			SP_MESSAGEBOX(wxT("Select a single column to rename."),
					wxT("Invalid list choice"), wxOK | wxICON_ERROR);
			return;

		}

		if(m_pcValueGrid->GetColLabelValue(l_pcSelCols.Item(0)).BeforeFirst(m_chSeperator).IsSameAs(wxT("Main")))
		{

			SP_MESSAGEBOX(wxT("Main list cannot be renamed."),
					wxT("Invalid list choice"), wxOK | wxICON_ERROR);
			return;

		}
		wxString l_sOldlabel = m_pcValueGrid->GetColLabelValue(l_pcSelCols.Item(0)).BeforeFirst(m_chSeperator);
		wxTextEntryDialog l_pcDialog( this, wxT("New name of the list: ") + l_sOldlabel, wxT("Rename list"));

		l_pcDialog.SetValue(l_sOldlabel);

		int l_nModalResult = l_pcDialog.ShowModal();

		if (wxID_OK == l_nModalResult)
		{
			if (l_pcDialog.GetValue().Find(wxT("@")) != -1)
			{
				SP_MESSAGEBOX(
						wxT("Do not use the symbol @ in the list name."),
						wxT("Invalid list name"), wxOK | wxICON_ERROR);
				return;
			}
			if (ExistName(l_pcDialog.GetValue() ) )
			{
				SP_MESSAGEBOX(
						wxT("Do not use an already existing list name."),
						wxT("Invalid list name"),
						wxOK | wxICON_ERROR);
				return;
			}
			for(int l_nCol=0; l_nCol<m_pcValueGrid->GetNumberCols(); l_nCol++)
			{
				if(l_sOldlabel.IsSameAs(m_pcValueGrid->GetColLabelValue(l_nCol).BeforeFirst(m_chSeperator)))
					m_pcValueGrid->SetColLabelValue(l_nCol, l_pcDialog.GetValue() + m_chSeperator + m_pcValueGrid->GetColLabelValue(l_nCol).AfterFirst(m_chSeperator));
			}
		}
	}
}

void SP_DLG_ColListOverview::LoadData()
{

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();


	if( l_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		//Load set first
		SP_DS_Nodeclass* l_pcNodeClass;
		SP_DS_ColListAttribute* l_pcColList;

		if( m_sAttrName == wxT("DelayList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
			
			if(l_pcNodeClass->GetElements()->size() > 0)
			{		
				SP_DS_Node* l_pcNode = l_pcNodeClass->GetElements()->front();
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));

				for(unsigned int i = 0; i < l_pcColList->GetColCount(); i++ )
				{
					wxString l_sSetName = l_pcColList->GetColLabel(i);
					m_pcValueGrid->AppendCols(1);
					m_pcValueGrid->SetColLabelValue(i, l_sSetName);
				}
			}
		}

		if( m_sAttrName == wxT("PeriodicList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
			
			if(l_pcNodeClass->GetElements()->size() > 0)
			{		
				SP_DS_Node* l_pcNode = l_pcNodeClass->GetElements()->front();
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));

				for(unsigned int i = 0; i < l_pcColList->GetColCount(); i++ )
				{
					wxString l_sSetName = l_pcColList->GetColLabel(i);
					m_pcValueGrid->AppendCols(1);
					m_pcValueGrid->SetColLabelValue(i, l_sSetName);
				}
			}
		}

		//Load data
		
		//SP_DS_ColListAttribute* l_pcColList;	
		SP_ListNode::const_iterator l_itElem;
		int l_nGridRowNumber = 0;

		for (l_itElem = l_pcNodeClass->GetElements()->begin(); l_itElem != l_pcNodeClass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransitionNode = ( *l_itElem );

			if( m_sAttrName == wxT("DelayList"))
			{
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransitionNode->GetAttribute(wxT("DelayList")));
			}
			if( m_sAttrName == wxT("PeriodicList"))
			{
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransitionNode->GetAttribute(wxT("PeriodicList")));
			}			

			wxString l_sFunctionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransitionNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_pcValueGrid->AppendRows( 1);
			m_pcValueGrid->SetRowLabelValue(l_nGridRowNumber, l_sFunctionName);
			

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				if(i >= 1)
				{
					m_pcValueGrid->AppendRows( 1);
					m_pcValueGrid->SetRowLabelValue(l_nGridRowNumber, wxT(""));
				}

				for (unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
				{				
					m_pcValueGrid->SetCellValue(l_nGridRowNumber, j, l_pcColList->GetCell(i, j));				
				}
			
				l_nGridRowNumber++;
			}		

		}

		if ( (m_pcValueGrid->GetNumberRows() > 0 ) && (m_pcValueGrid->GetNumberCols() > 0 ))
		{
			m_sOldCellValue = m_pcValueGrid->GetCellValue( 0, 0);
		}

	}

	else
	{

		for(SP_ListNodeclass::const_iterator itNC=l_pcGraph->GetNodeclasses()->begin();	itNC!=l_pcGraph->GetNodeclasses()->end(); itNC++)
		{
			for(SP_ListNode::const_iterator itNode=(*itNC)->GetElements()->begin();	itNode!=(*itNC)->GetElements()->end(); itNode++)
			{
				if(SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*itNode)->GetAttribute(m_sAttrName)))
				{
					m_vNodes.push_back((*itNode));
					m_vAttributes.push_back(l_pcColAttr);
					m_eColListType = l_pcColAttr->GetColListType();
				}
			}
		}

		if(m_vAttributes.empty()) return;

		bool l_bWhite = true;
		for(size_t i=0; i<m_vNodes.size(); i++)
		{
			SP_DS_Node* l_pcNode = m_vNodes[i];
			wxString l_sRowLabel = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_pcValueGrid->AppendRows( 1);
			m_pcValueGrid->SetRowLabelValue(i, l_sRowLabel);
			int l_nGridColNumber = 0;
			SP_DS_ColListAttribute* l_pcAttr = m_vAttributes[i];
			for(unsigned int l_nRow=0; l_nRow < l_pcAttr->GetRowCount(); l_nRow++)
			{
				if((unsigned int)(m_pcValueGrid->GetNumberCols()) < l_pcAttr->GetRowCount())
					m_pcValueGrid->AppendCols(l_pcAttr->GetRowCount()*(l_pcAttr->GetColCount()-1));
				for(unsigned int l_nCol = 1; l_nCol < l_pcAttr->GetColCount(); l_nCol++)
				{
					wxString l_sColLabel=wxT("");
					l_sColLabel << l_pcAttr->GetCell(l_nRow,0) << m_chSeperator << l_pcAttr->GetColLabel(l_nCol);
					m_pcValueGrid->SetColLabelValue(l_nGridColNumber, l_sColLabel);
					m_pcValueGrid->SetCellValue(i, l_nGridColNumber, l_pcAttr->GetCell(l_nRow,l_nCol));

					m_pcValueGrid->SetCellBackgroundColour(i, l_nGridColNumber,	(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));
					m_pcValueGrid->SetCellAlignment(i, l_nGridColNumber, wxALIGN_RIGHT,	wxALIGN_CENTER);
					l_nGridColNumber++;
				}
			}
			(l_bWhite ? l_bWhite = false : l_bWhite = true );
		}
	}
}

void SP_DLG_ColListOverview::SaveData()
{
	m_pcValueGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	if( l_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{			
		SP_DS_ColListAttribute* l_pcColList;
		SP_DS_Nodeclass* l_pcNodeClass;
		SP_ListNode::const_iterator l_itElem;
		int l_nGridRowNumber = 0;

		if( m_sAttrName == wxT("DelayList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
		}
		if( m_sAttrName == wxT("PeriodicList"))
		{
			l_pcNodeClass = l_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
		}

		for (l_itElem = l_pcNodeClass->GetElements()->begin(); l_itElem != l_pcNodeClass->GetElements()->end(); ++l_itElem)
		{

			if( m_sAttrName == wxT("DelayList"))
				l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("DelayList")) );
			if( m_sAttrName == wxT("PeriodicList"))
				l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("PeriodicList")) );

			l_pcColList->Clear();

			int l_nBegin = l_nGridRowNumber;		

			l_pcColList->RemoveAllColumns(); //for colored Petri Net

			for(unsigned i = 0; i < (unsigned int)(m_pcValueGrid->GetNumberCols()); i++ )
			{
				l_pcColList->AppendEmptyColum();
				l_pcColList->SetColLabel(i,m_pcValueGrid->GetColLabelValue(i));
			}
		

			l_pcColList->AppendEmptyRow();

			for(int j = l_nBegin; j < m_pcValueGrid->GetNumberRows(); j++)
			{						
				l_nGridRowNumber++;

				for (int i = 0; i < m_pcValueGrid->GetNumberCols(); i++)
				{
					wxString l_sValue = m_pcValueGrid->GetCellValue(j, i);
					l_pcColList->SetCell(j-l_nBegin, i, l_sValue);
				}
				if( j < m_pcValueGrid->GetNumberRows()-1 )
				{
					wxString l_sRowLabelValue = m_pcValueGrid->GetRowLabelValue(j+1);
					if(l_sRowLabelValue == wxT(""))
						l_pcColList->AppendEmptyRow();
					else
						break;
				}			

			}

		}
	}
	else
	{

		for(size_t i=0; i<m_vAttributes.size(); i++)
		{
			SP_DS_ColListAttribute* l_pcAttr = m_vAttributes[i];
			l_pcAttr->Clear();
			wxString l_sListName=wxT("");
			unsigned int l_nColNumber=0;
			unsigned int l_nRowNumber=0;
			for(int l_nCol = 0; l_nCol < m_pcValueGrid->GetNumberCols(); l_nCol++)
			{
				if(l_sListName.IsSameAs(m_pcValueGrid->GetColLabelValue(l_nCol).BeforeFirst(m_chSeperator)))
				{
					l_nColNumber++;
					l_pcAttr->SetCell(l_nRowNumber, l_nColNumber, m_pcValueGrid->GetCellValue(i,l_nCol));
				}
				else
				{
					l_nRowNumber = l_pcAttr->AppendEmptyRow();
					l_nColNumber=0;
					l_sListName=m_pcValueGrid->GetColLabelValue(l_nCol).BeforeFirst(m_chSeperator);
					l_pcAttr->SetCell(l_nRowNumber, l_nColNumber, l_sListName);
					l_nColNumber++;
					l_pcAttr->SetCell(l_nRowNumber, l_nColNumber, m_pcValueGrid->GetCellValue(i,l_nCol));
				}
			}
		}
	}
	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_ColListOverview::OnCellValueChanged(wxGridEvent& ev)
{
//TODO: temporary disable check, need to be tested for correctness
#if 0
	wxString l_sValue = m_pcValueGrid->GetCellValue(ev.GetRow(), ev.GetCol() );

	if(m_eColListType == SP_COLLIST_BOOL)
	{
		if(l_sValue != wxT("0") && l_sValue != wxT("1"))
		{
			SP_MESSAGEBOX(
					wxT("The value must be \"0\" or \"1\". The inputed value is invalid."),
					wxT("Error"), wxOK | wxICON_ERROR);

			m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
	else if(m_eColListType == SP_COLLIST_CHAR)
	{
		if(l_sValue.length() > 1)
		{
			SP_MESSAGEBOX(
					wxT("The value must be a single char. The inputed value is invalid."),
					wxT("Error"), wxOK | wxICON_ERROR);

			m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
	else if(m_eColListType == SP_COLLIST_DOUBLE)
	{
		double l_nDouble = -1;
		if ( ! (l_sValue.ToDouble( &l_nDouble) ))
		{
			SP_MESSAGEBOX(
					wxT("The value must be a real value. The inputed value is invalid."),
					wxT("Error"), wxOK | wxICON_ERROR);

			m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
	else if(m_eColListType == SP_COLLIST_INTEGER)
	{
		long l_nValue = -1;
		if ( ! (l_sValue.ToLong( &l_nValue) ))
		{
			SP_MESSAGEBOX(
					wxT("The value must be an integer value. The inputed value is invalid."),
					wxT("Error"), wxOK | wxICON_ERROR);

			m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
	else if(m_eColListType == SP_COLLIST_UNSIGNED_INTEGER)
	{
		unsigned long l_nValue = -1;
		if ( ! (l_sValue.ToULong( &l_nValue) ))
		{
			SP_MESSAGEBOX(
					wxT("The value must be an unsigned integer value. The inputed value is invalid."),
					wxT("Error"), wxOK | wxICON_ERROR);

			m_pcValueGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		}
	}
#endif
	ev.Skip();
}

void SP_DLG_ColListOverview::OnGridCellSelected(wxGridEvent& ev)
{
	if ((m_pcValueGrid->GetNumberRows() > 0 ) &&
		(m_pcValueGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcValueGrid->GetCellValue(ev.GetRow(), ev.GetCol() );
	}
	ev.Skip();
}

bool SP_DLG_ColListOverview::ExistName(const wxString& p_sName)
{

	for (int i = 0; i < m_pcValueGrid->GetNumberCols(); i++)
	{
		if (p_sName.IsSameAs(m_pcValueGrid->GetColLabelValue(i).BeforeFirst(m_chSeperator) ) )
		{
			return true;
		}
	}

	return false;
}

void SP_DLG_ColListOverview::RefreshFrame()
{

	SP_Core::Instance()->GetRootDocument()->Refresh();

}
