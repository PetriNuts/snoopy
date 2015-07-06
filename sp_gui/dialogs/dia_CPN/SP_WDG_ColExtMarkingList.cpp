//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#include "sp_gui/dialogs/dia_CPN/SP_WDG_ColExtMarkingList.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetOverview.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"


#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include <wx/event.h>

#include <wx/tipwin.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_IMPORT,
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_CHECK

};
BEGIN_EVENT_TABLE( SP_WDG_ColExtMarkingList, SP_WDG_DialogBase )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_WDG_ColExtMarkingList::OnGridCellValueChanged )
    EVT_GRID_SELECT_CELL( SP_WDG_ColExtMarkingList::OnGridCellSelected )
#else
    EVT_GRID_CELL_CHANGE( SP_WDG_ColExtMarkingList::OnGridCellValueChanged )
    EVT_GRID_SELECT_CELL( SP_WDG_ColExtMarkingList::OnGridCellSelected )
#endif

EVT_TEXT( 0, SP_WDG_ColExtMarkingList::OnTextValueChanged )
END_EVENT_TABLE()

SP_WDG_ColExtMarkingList::SP_WDG_ColExtMarkingList(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_ColExtMarkingList::~SP_WDG_ColExtMarkingList()
{
}

SP_WDG_DialogBase*
SP_WDG_ColExtMarkingList::Clone()
{

	return new SP_WDG_ColExtMarkingList(GetName(), GetDialogOrdering());

}

bool SP_WDG_ColExtMarkingList::AddToDialog(
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

	//wxStaticText* l_StaticText=new wxStaticText(l_pcPage, -1, wxT("ColorSet:"));
	//m_pcColorSetTextCtrl= new wxTextCtrl(l_pcPage, -1, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	//l_pcSizer->Add(l_StaticText, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	//l_pcSizer->Add(m_pcColorSetTextCtrl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	//l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("") ), 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcMarkingGrid = new wxGrid(l_pcPage, SP_ID_GRID_MARKING + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER );

	m_pcMarkingGrid->CreateGrid(0, 0);

	m_pcMarkingGrid->EnableEditing(true);
	m_pcMarkingGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcMarkingGrid->SetDefaultColSize(120, TRUE );
	m_pcMarkingGrid->SetDefaultRowSize(20, TRUE );

	m_pcMarkingGrid->SetColLabelSize(16);
	m_pcMarkingGrid->SetRowLabelSize(0);

	m_pcMarkingGrid->AppendCols(2);

	m_pcMarkingGrid->SetColLabelValue(0, wxT("Color/Predicate/Function") );
	m_pcMarkingGrid->SetColSize(0, 150);

	m_pcMarkingGrid->SetColLabelValue(1, wxT("Marking") );
	m_pcMarkingGrid->SetColSize(1, 100);

	//m_pcMarkingGrid->SetColLabelValue(1, wxT("Colors") );
	//m_pcMarkingGrid->SetColSize(2, 100);

	LoadData();

	l_pcSizer->Add(m_pcMarkingGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ADD + m_nDialogID
			+ wxID_HIGHEST, wxT("Add marking") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	
	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_DELETE + m_nDialogID
			+ wxID_HIGHEST, wxT("Delete marking") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	
	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Check marking") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_IMPORT + m_nDialogID
			+ wxID_HIGHEST, wxT("Colorset overview") ), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_ColExtMarkingList::OnDlgOk()
{
	SaveData();
	UpdatePlaceColor();
	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_ColExtMarkingList::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
	CHECK_BOOL( SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

	bool l_bWhite = false;	
  
	if (m_pcMarkingGrid->GetNumberRows() > 0)
	{
		m_pcMarkingGrid->DeleteRows(0, m_pcMarkingGrid->GetNumberRows());
	}	

	//Get the maximum number of rows for one place
	unsigned int l_nMaxRow = 0;
	if(m_bMultiple)
	{
		SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		SP_DS_Nodeclass* l_pcNodeclass;
		l_pcNodeclass= l_pcGraph->GetNodeclass( wxT("Place") );
		SP_ListNode::const_iterator l_itElem;		
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			SP_DS_ColListAttribute* l_pcColListAll = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
			if( l_nMaxRow < l_pcColListAll->GetRowCount() )
				l_nMaxRow = l_pcColListAll->GetRowCount();
		}
	}
	else
	{
		l_nMaxRow = l_pcColList->GetRowCount();
	}	

	//append rows
	if(l_nMaxRow > 0)
	{		
		m_pcMarkingGrid->AppendRows( l_nMaxRow );
	}

	//load data
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		//load color value
		wxString l_sColorValue = SP_WILDCARD;
		if(!m_bMultiple)
		{
			l_sColorValue = l_pcColList->GetCell(i,0);
		}
		m_pcMarkingGrid->SetCellValue(i, 0, l_sColorValue);
		m_pcMarkingGrid->SetCellAlignment(i, 0, wxALIGN_LEFT, wxALIGN_CENTER);
		m_pcMarkingGrid->SetCellBackgroundColour(i, 0, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

		//load token number
		wxString l_sTokenValue = SP_WILDCARD;
		if(!m_bMultiple)
		{
			l_sTokenValue = l_pcColList->GetCell(i,1);
		}
		m_pcMarkingGrid->SetCellValue(i, 1, l_sTokenValue);
		m_pcMarkingGrid->SetCellAlignment(i, 1, wxALIGN_LEFT, wxALIGN_CENTER);

		m_pcMarkingGrid->SetCellBackgroundColour(i, 1, (l_bWhite ? *wxWHITE	: *wxLIGHT_GREY));
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}
	return true;
}

bool SP_WDG_ColExtMarkingList::SaveData()
{
	m_pcMarkingGrid->SaveEditControlValue();

	if( m_bMultiple )
	{
		for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
			{
				SP_DS_Attribute* l_pcAttr = *it;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
				//for (int l_nRow = 0; l_nRow < m_pcMarkingGrid->GetNumberRows(); l_nRow++)
				for (unsigned l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
				{
					for(int l_nCol = 0; l_nCol < m_pcMarkingGrid->GetNumberCols(); l_nCol++)
					{
						wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nRow, l_nCol);
						if((m_bMultiple && l_sValue != SP_WILDCARD) )
						{
							l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);
						}
					}
				}

				//update display of marking				

				SP_Data* l_pcDataNode = l_pcAttr->GetParent();
				if (l_pcDataNode->GetElementType() == SP_ELEMENT_NODE)
				{
					SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcDataNode);

					SP_CPN_SyntaxChecking l_cSyntaxChecking;
					if( !l_cSyntaxChecking.Initialize())
						return false;
					
					int l_nMarking = 0;				

					//new marking solving 
					map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
					if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
						return false;
					map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;
					for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
					{
						l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
					}
					//end 

					wxString l_sMainMarking = wxT("");	
					l_sMainMarking <<l_nMarking;

					SP_DS_Attribute* l_pcMarkingAttr = l_pcNode->GetAttribute(wxT("Marking"));
					if(l_pcMarkingAttr)
					{
						l_pcMarkingAttr->SetValueString(l_sMainMarking);
					}
				}
				//end update display of marking


			}
		return true;
	}


	SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();
	SP_DS_Attribute* l_pcMarkingAttr = *l_Iter;

	CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

	wxString l_sMainMarking;

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMarkingAttr);	

	

	l_pcColList->Clear();

	bool l_bFirst=true;
	long l_lMarkingSum=0;

	for (int i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
	{

		// the first time do not append empty row.
		//if(!l_bFirst)
		{
			l_pcColList->AppendEmptyRow();
		}
		l_bFirst=false;
		l_pcColList->SetCell(i, 0, m_pcMarkingGrid->GetCellValue(i, 0));
		l_pcColList->SetCell(i, 1, m_pcMarkingGrid->GetCellValue(i, 1));

		l_sMainMarking=l_pcColList->GetCell(i,1).c_str();
		long l_lMarking = 0;
		if(l_sMainMarking.ToLong(&l_lMarking))		
			l_lMarkingSum += l_lMarking;

	}

	////
	

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return false;
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

	if(!l_pcNode)
		return false;

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return false;
	
	int l_nMarking = 0;
	
	//new marking solving 
	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
		return false;
	map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;
	for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
	{
		l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
	}
	//end 



	////

	l_sMainMarking = wxT("");	
	l_sMainMarking <<l_nMarking;



	SP_Data* l_pcDataNode = l_pcMarkingAttr->GetParent();

	if (l_pcDataNode->GetElementType() != SP_ELEMENT_NODE)
	{

		return FALSE;

	}

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcDataNode);
	CHECK_POINTER( l_pcNode, return FALSE );

	const SP_ListAttribute* l_plAttributes = l_pcNode->GetAttributes();

	CHECK_POINTER( l_plAttributes, return FALSE );

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		l_pcMarkingAttr = (*l_Iter);

		if ((l_pcMarkingAttr->GetName()) == wxT("Marking"))
		{
			l_pcMarkingAttr->SetValueString(l_sMainMarking);
		}
	}
	
	
	return true;

}

void SP_WDG_ColExtMarkingList::OnAdd(wxCommandEvent& p_cEvent)
{
	m_pcMarkingGrid->AppendRows(1);
}


void SP_WDG_ColExtMarkingList::OnDelete( wxCommandEvent& p_cEvent )
{
	if ( m_pcMarkingGrid->IsSelection() )
    {
		m_pcMarkingGrid->BeginBatch();
        for ( int n = 0; n < m_pcMarkingGrid->GetNumberRows(); )
        {
            if ( m_pcMarkingGrid->IsInSelection( n , 0 ) )
            	m_pcMarkingGrid->DeleteRows( n, 1 );
            else
                n++;
        }
        m_pcMarkingGrid->EndBatch();
    }
}

void SP_WDG_ColExtMarkingList::OnImport(wxCommandEvent& p_cEvent)
{

	wxString l_sColorSetName ;
	SP_DLG_ColorSetOverview l_cColorSetDlg(NULL);
	if ( l_cColorSetDlg.ShowModal() == wxID_OK )
	{
		l_sColorSetName = l_cColorSetDlg.GetColorSetName();
	}
	l_cColorSetDlg.Destroy();	

	
	SP_WDG_DialogText* l_pcWDG_DialogText ;
	list<SP_WDG_DialogBase*>* l_ptlWidgets = m_pcDlg->GettlWidgets();
	list<SP_WDG_DialogBase*>::iterator itList;

	for(itList = l_ptlWidgets->begin(); itList != l_ptlWidgets->end(); itList++)
	{
		l_pcWDG_DialogText = dynamic_cast< SP_WDG_DialogText* >( *itList );

		if(l_pcWDG_DialogText && l_pcWDG_DialogText->GetName() == wxT("Markings"))
		{
			l_pcWDG_DialogText->GetTextCtrl()->SetValue( l_sColorSetName );
			l_pcWDG_DialogText->GetTextCtrl()->TransferDataToWindow();
		}
	}	

}

void SP_WDG_ColExtMarkingList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnAdd);

	Connect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnDelete);

	Connect(
			SP_ID_BUTTON_IMPORT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnImport);
	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnCheck);
}

void SP_WDG_ColExtMarkingList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnAdd);

	Disconnect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnDelete);

	Disconnect(
			SP_ID_BUTTON_IMPORT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnImport);
	
	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColExtMarkingList::OnCheck);
}

void SP_WDG_ColExtMarkingList::OnGridCellSelected(wxGridEvent& ev)
{

	m_sOldCellValue = m_pcMarkingGrid->GetCellValue(ev.GetRow(), ev.GetCol());
	ev.Skip();

}

void SP_WDG_ColExtMarkingList::OnGridCellValueChanged(wxGridEvent& ev)
{
/*
	SP_CPN_ValueAssign l_cValueAssign;	
	SP_CPN_ColorSetClass l_cColorSetClass;
	SP_CPN_ColorSet* l_pcColorSet;
	l_cValueAssign.InitializeColorset(l_cColorSetClass);

	/////////////////
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	wxString l_sColorSetName;
	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return;
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

	if(!l_pcNode)
		return;
	m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));

	//check name	
	if(!m_pcNameAttibute)
		return;

	l_sColorSetName = m_pcNameAttibute->GetValue();
	l_sColorSetName.Replace(wxT(" "),wxT(""));
	l_sColorSetName.Replace(wxT("\t"),wxT(""));

	l_pcColorSet = l_cColorSetClass.LookupColorSet(l_sColorSetName);		
	if(!l_pcColorSet)
	{
		wxString l_sError = l_sColorSetName + wxT(" is not defined");
		SP_MESSAGEBOX(l_sError,wxT("Error"), wxOK| wxICON_ERROR );
		return;
	}	

	////////////////
	wxString l_sColor;
	wxString l_nMarking;
	long l_nColor;	

	for (int i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
	{		
		l_sColor = m_pcMarkingGrid->GetCellValue(i, 0);
		l_nMarking = m_pcMarkingGrid->GetCellValue(i, 1);

		//analyze colors
		l_sColor.Replace(wxT(" "),wxT(""));

		if(l_pcColorSet->GetDataType() == CPN_INTEGER)
		{
			if(!l_pcColorSet->IsColor(l_sColor))
			{
				wxString l_sError = l_sColor + wxT(" is not a color of colorset: ") + l_sColorSetName;
				SP_MESSAGEBOX(l_sError,wxT("Error"), wxOK| wxICON_ERROR );
				return;
			}
		}
		else if(l_pcColorSet->GetDataType() == CPN_BOOLEAN)
		{
			if(!(l_sColor == wxT("false") || l_sColor == wxT("true")))
			{
				wxString l_sError = l_sColor + wxT(" is not a color of colorset: ") + l_sColorSetName;
				SP_MESSAGEBOX(l_sError,wxT("Error"), wxOK| wxICON_ERROR );
				return;
			}
		}
		else if(l_pcColorSet->GetDataType() == CPN_UNION)
		{
			if( !l_pcColorSet->IsColor(l_sColor))
			{
				wxString l_sError = l_sColor + wxT(" is not a color of colorset: ") + l_sColorSetName;
				SP_MESSAGEBOX(l_sError,wxT("Error"), wxOK| wxICON_ERROR );
				return;
			}
		}
		else
		{
			if(!l_pcColorSet->IsColor(l_sColor))
			{
				wxString l_sError = l_sColor + wxT(" is not a color of colorset: ") + l_sColorSetName;
				SP_MESSAGEBOX(l_sError,wxT("Error"), wxOK| wxICON_ERROR );
				return;
			}
		}

		if (l_nMarking == wxT(""))
			return;
		else
		{
			if(!l_nMarking.ToLong(&l_nColor))
			{
				SP_MESSAGEBOX(
					wxT("The value of the marking must be a non-negative integer value. The inputed value is invalid."),
					wxT("Error"), wxOK| wxICON_ERROR );
				return;
			}
		}		

	}

*/
	ev.Skip();

}

wxGrid* SP_WDG_ColExtMarkingList::GetColorGrid()
{
	return m_pcMarkingGrid;
}


void SP_WDG_ColExtMarkingList::OnTextValueChanged( wxCommandEvent& ev )
{
}



SP_DS_TextAttribute* SP_WDG_ColExtMarkingList::GetNameAttibute()
{
	

	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	

	SP_DS_Node* l_pcNode;

	if(!l_pcAttr)
		return 0;
	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

	if(!l_pcNode)
		return 0;
	m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	
	if(!m_pcNameAttibute)
		return 0;

	return m_pcNameAttibute;

	
}



void SP_WDG_ColExtMarkingList::OnCheck( wxCommandEvent& p_cEvent )
{
	
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
	
	//new marking solving 
	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap) )
		return;
	//end 


	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The marking definition");
	//l_sVariables += l_sPredicate;
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(m_pcDlg, l_sVariables, 1000);

}

void SP_WDG_ColExtMarkingList::UpdatePlaceColor()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	SP_DS_Node* l_pcNode;
	if(!l_pcAttr)
		return;

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
	if(!l_pcNode)
		return;

	m_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));	
	if(!m_pcNameAttibute)
		return;

	wxString l_sColorSetName = m_pcNameAttibute->GetValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return;
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> ((*(l_pcMetadataclass->GetElements()->begin()))->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i,0);
		if(l_sCSName == l_sColorSetName)
		{
			wxString l_sPlaceColor = l_pcColList->GetCell(i,3);
			wxString l_sShow = l_pcColList->GetCell(i,4);
			if(l_sShow == wxT("") )
				l_sPlaceColor = wxT("white");
			
			wxColour l_PlaceRColor = wxColour( l_sPlaceColor);
			SP_ListGraphic::iterator l_Iter;
			SP_ListGraphic* l_PlaceGraphicsList = l_pcNode->GetGraphics();				
			for (l_Iter = l_PlaceGraphicsList->begin(); l_Iter != l_PlaceGraphicsList->end(); ++l_Iter)
			{
				wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(l_PlaceRColor, (*l_Iter)->GetBrush()->GetStyle());
				(*l_Iter)->SetBrush(l_pcBrush);
				(*l_Iter)->Update(true);			
			}	
			
			return;
		}
	}


	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> ((*(l_pcMetadataclass->GetElements()->begin()))->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sCSName = l_pcColList->GetCell(i,0);
		if(l_sCSName == l_sColorSetName)
		{
			wxString l_sPlaceColor = l_pcColList->GetCell(i,4);
			wxString l_sShow = l_pcColList->GetCell(i,5);
			if(l_sShow == wxT("") )
				l_sPlaceColor = wxT("white");
			
			wxColour l_PlaceRColor = wxColour( l_sPlaceColor);
			SP_ListGraphic::iterator l_Iter;
			SP_ListGraphic* l_PlaceGraphicsList = l_pcNode->GetGraphics();				
			for (l_Iter = l_PlaceGraphicsList->begin(); l_Iter != l_PlaceGraphicsList->end(); ++l_Iter)
			{
				wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(l_PlaceRColor, (*l_Iter)->GetBrush()->GetStyle());
				(*l_Iter)->SetBrush(l_pcBrush);
				(*l_Iter)->Update(true);			
			}	
			
			return;
		}
	}
}
