//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#include "sp_gui/dialogs/dia_ColSPN/SP_WDG_ColStMarkingList.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"

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
#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"


enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_IMPORT,
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_OVERVIEW,
	SP_ID_BUTTON_CHECK,
	SP_ID_BUTTON_SHOWMARKING
	

};
BEGIN_EVENT_TABLE( SP_WDG_ColStMarkingList, SP_WDG_DialogBase )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_WDG_ColStMarkingList::OnGridCellValueChanged ) 
#else
    EVT_GRID_CELL_CHANGE( SP_WDG_ColStMarkingList::OnGridCellValueChanged ) 
#endif

EVT_GRID_SELECT_CELL( SP_WDG_ColStMarkingList::OnGridCellSelected )

END_EVENT_TABLE()

SP_WDG_ColStMarkingList::SP_WDG_ColStMarkingList(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_ColStMarkingList::~SP_WDG_ColStMarkingList()
{
}

SP_WDG_DialogBase* SP_WDG_ColStMarkingList::Clone()
{

	return new SP_WDG_ColStMarkingList(GetName(), GetDialogOrdering());

}

bool SP_WDG_ColStMarkingList::AddToDialog(
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

	m_pcMarkingGrid = new wxGrid(l_pcPage, SP_ID_GRID_MARKING + m_nDialogID
			+ wxID_HIGHEST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );

	m_pcMarkingGrid->CreateGrid(0, 0);

	m_pcMarkingGrid->EnableEditing(true);
	m_pcMarkingGrid->SetSelectionMode(wxGrid::wxGridSelectCells);

	m_pcMarkingGrid->SetDefaultColSize(120, TRUE );
	m_pcMarkingGrid->SetDefaultRowSize(20, TRUE );

	m_pcMarkingGrid->SetColLabelSize(16);
	m_pcMarkingGrid->SetRowLabelSize(0);

	m_pcMarkingGrid->AppendCols(3);

	m_pcMarkingGrid->SetColLabelValue(0, wxT("Main: Color(s)") );
	m_pcMarkingGrid->SetColSize(0, 100);

	m_pcMarkingGrid->SetColLabelValue(1, wxT("Product Color"));//by george
	m_pcMarkingGrid->SetColSize(1, 100);//by george


	m_pcMarkingGrid->SetColLabelValue(2, wxT("Main: Marking"));
	m_pcMarkingGrid->SetColSize(2, 100);

	LoadData();

	l_pcSizer->Add(m_pcMarkingGrid, 1, wxALL | wxEXPAND, 5);
	l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_ADD + m_nDialogID
			+ wxID_HIGHEST, wxT("Add marking") ), 1, wxALL, 5);
	
	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_DELETE + m_nDialogID
			+ wxID_HIGHEST, wxT("Delete marking") ), 1, wxALL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_SHOWMARKING + m_nDialogID
			+ wxID_HIGHEST, wxT("Show marking") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_CHECK + m_nDialogID
			+ wxID_HIGHEST, wxT("Check marking") ), 1, wxALL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_IMPORT + m_nDialogID
			+ wxID_HIGHEST, wxT("Colorset overview") ), 1, wxALL, 5);

	l_pcSizer->Add(new wxButton(l_pcPage, SP_ID_BUTTON_OVERVIEW + m_nDialogID
			+ wxID_HIGHEST, wxT("Marking overview") ), 1, wxALL, 5);

	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

//	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

//	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	ConnectEvents();
	p_pcDlg->PushEventHandler(this);

	return TRUE;

}

bool SP_WDG_ColStMarkingList::OnDlgOk()
{
 
	SaveData();
	UpdatePlaceColor();
	return SP_WDG_DialogBase::OnDlgOk();

}

bool SP_WDG_ColStMarkingList::LoadData()
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
		CHECK_BOOL( SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST == l_pcAttr->GetAttributeType(), return FALSE );

		SP_DS_Node* l_pcNode;
		l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
		if(!l_pcNode)
			return false;
		m_sNodeClassName = l_pcNode->GetNodeclass()->GetName();


		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcAttr);

		if (m_pcMarkingGrid->GetNumberRows() > 0)
		{
			m_pcMarkingGrid->DeleteRows(0, m_pcMarkingGrid->GetNumberRows());
		}

		if (m_pcMarkingGrid->GetNumberCols() > 0)
		{
			m_pcMarkingGrid->DeleteCols(0, m_pcMarkingGrid->GetNumberCols());
		}

	    	unsigned l_nColnum = l_pcColList->GetColCount();

			if (l_nColnum % 3 != 0)
			{
				wxString lastcol = l_pcColList->GetColLabel(l_nColnum-1);
				unsigned l_nNewCol=l_pcColList->AppendEmptyColum();
				l_pcColList->SetColLabel(l_nNewCol, lastcol);
				if (lastcol.Contains(wxT(":")))
				{
					wxString l_sSetName = lastcol.BeforeFirst(wxChar(':'));
					l_sSetName << wxT(": ") << wxT("Product Color");
					l_pcColList->SetColLabel(l_nNewCol-1, l_sSetName);
				}
			}

			m_pcMarkingGrid->AppendCols(l_pcColList->GetColCount());
			//Get the maximum number of rows for one place
			unsigned int l_nMaxRow = 0;
			if(m_bMultiple)
			{
				SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass= l_pcGraph->GetNodeclass( m_sNodeClassName );
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

			for (unsigned i = 0; i < l_pcColList->GetColCount(); i++)
			{
				bool l_bWhite = true;

				if (i % 3 ==0)
				{
					m_pcMarkingGrid->SetColLabelValue(i, l_pcColList->GetColLabel(i));
				}
				else if (i % 3 == 1)
				{
					m_pcMarkingGrid->SetColLabelValue(i, l_pcColList->GetColLabel(i+1));
				}
				else
				{
					m_pcMarkingGrid->SetColLabelValue(i, l_pcColList->GetColLabel(i -1));
				}
				for (unsigned int j = 0; j < l_pcColList->GetRowCount(); j++)
				{
					wxString l_sColorValue = SP_WILDCARD;
					if (!m_bMultiple)
					{
						if (i % 3 == 0)
						{
							l_sColorValue = l_pcColList->GetCell(j, i);
						}
						else if (i % 3 == 1)
						{
							l_sColorValue = l_pcColList->GetCell(j, i+1);
							if (l_sColorValue.IsEmpty())
							{
								l_sColorValue = wxT("()");
							}
						}
						else
						{
							l_sColorValue = l_pcColList->GetCell(j, i-1);
						}

					}

					if (i % 3 == 0)
					{
						m_pcMarkingGrid->SetCellValue(j, i, l_sColorValue);
						m_pcMarkingGrid->SetCellAlignment(j, i, wxALIGN_LEFT, wxALIGN_CENTER);
						m_pcMarkingGrid->SetCellBackgroundColour(j, i, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
					}
					else if (i % 3 == 1)
					{
						m_pcMarkingGrid->SetCellValue(j, i, l_sColorValue);
						m_pcMarkingGrid->SetCellAlignment(j, i, wxALIGN_LEFT, wxALIGN_CENTER);
						m_pcMarkingGrid->SetCellBackgroundColour(j, i, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
					}
					else
					{
						m_pcMarkingGrid->SetCellValue(j, i, l_sColorValue);
						m_pcMarkingGrid->SetCellAlignment(j, i, wxALIGN_LEFT, wxALIGN_CENTER);
						m_pcMarkingGrid->SetCellBackgroundColour(j, i, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
					}
					(l_bWhite ? l_bWhite = false : l_bWhite = true);
				}
			}

			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
			if (!l_pcNameAttibute)
				return false;

			SP_CPN_ColorSetClass l_cColorSetClass;
			SP_CPN_ColorSet* l_pcColorSet;
			SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
			wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();
			SP_CPN_ValueAssign l_cValueAssign;

			if (!l_cValueAssign.InitializeColorset(l_cColorSetClass))
				return false;
			//check if the color set of the place is not product, then make the tuple column read-olnly
			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			l_pcColorSet = l_cColorSetClass.LookupColorSet(l_sColorSetName);
			if (!l_pcColorSet) return true;
			if (l_pcColorSet->GetDataType() != CPN_PRODUCT)
			{
				for (unsigned i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
				{
					m_pcMarkingGrid->SetReadOnly(i, 1);
				}

			}

			return true;
}

bool SP_WDG_ColStMarkingList::SaveData()
{

	m_pcMarkingGrid->SaveEditControlValue();

	 	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());
		wxString l_sColorSetName;
		SP_DS_Node* l_pcNode;

		if(!l_pcAttr)
			return false;
		l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());

		if(!l_pcNode)
			return false;

		//wxString l_sNetClassName = l_pcNode->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName();

		if( m_bMultiple )
		{
			for(SP_ListAttribute::const_iterator it = m_tlAttributes.begin(); it != m_tlAttributes.end(); it++)
				{
					SP_DS_Attribute* l_pcAttr = *it;
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
					for (unsigned l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
					{
						//l_pcColList->AppendEmptyRow();
						for(int l_nCol = 0; l_nCol < m_pcMarkingGrid->GetNumberCols(); l_nCol++)
						{
							wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nRow, l_nCol);
							if((m_bMultiple && l_sValue != SP_WILDCARD) )
							{
								if (l_nCol == 1)
								{
									l_pcColList->SetCell(l_nRow, 2, l_sValue);//product
								}
								else if (l_nCol == 2)
								{
									l_pcColList->SetCell(l_nRow, 1, l_sValue);//tokens
								}
								else
								{
									l_pcColList->SetCell(l_nRow, l_nCol, l_sValue);//main marking
								}
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


						/////////////////////////////////////////////////////////
						wxString l_sMainMarking = wxT("");
						map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
						if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
							return false;
						map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;

						if( m_sNodeClassName == SP_DS_CONTINUOUS_PLACE)
						{
							double l_dMarking = 0;
							for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
							{
								l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
							}
							l_sMainMarking <<l_dMarking;
						}
						else
						{
							long l_nMarking = 0;
							for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
							{
								l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
							}
							l_sMainMarking <<l_nMarking;
						}

						///////////////////////////////////////////////////////////


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

		SP_ListAttribute::const_iterator l_Iter = m_tlAttributes.begin();;
		SP_DS_Attribute* l_pcMarkingAttr = *l_Iter;

		CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

		wxString l_sMainMarking;

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMarkingAttr);

		l_pcColList->Clear();
		

		for(int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
		{
			wxString l_sColLabel;
			wxString l_sCellVal;
			l_sColLabel = m_pcMarkingGrid->GetColLabelValue(i);//
			if (i % 3 == 0)
			{
				l_pcColList->SetColLabel(i, m_pcMarkingGrid->GetColLabelValue(i));//color col
			}
			else if (i % 3 == 1)
			{
				l_pcColList->SetColLabel(i+1, m_pcMarkingGrid->GetColLabelValue(i));//product col
			}
			else
			{
				l_pcColList->SetColLabel(i - 1, m_pcMarkingGrid->GetColLabelValue(i));//number of token col
			}



			for(int j = 0; j < m_pcMarkingGrid->GetNumberRows(); j++)
			{
				if(i == 0)
					l_pcColList->AppendEmptyRow();

				l_sCellVal = m_pcMarkingGrid->GetCellValue(j, i);//

				if (i % 3 == 0)
				{
					l_pcColList->SetCell(j, i , m_pcMarkingGrid->GetCellValue(j, i));//color coloumn
				}
				else if (i % 3 == 1)
				{
					l_pcColList->SetCell(j, i+1, m_pcMarkingGrid->GetCellValue(j, i));//product coloumn
				}
				else
				{
					l_pcColList->SetCell(j, i - 1, m_pcMarkingGrid->GetCellValue(j, i));//product coloumn
				}

			}
		}


		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.Initialize())
				return false;


	    ////	new computation
		l_sMainMarking = wxT("");
		map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
		if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
			return false;
		map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;

		if( m_sNodeClassName == SP_DS_CONTINUOUS_PLACE)
		{
			double l_dMarking = 0;
			for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
			}
			l_sMainMarking <<l_dMarking;
		}
		else
		{
			long l_nMarking = 0;
			for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
			}
			l_sMainMarking <<l_nMarking;
		}
		///


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
				l_pcMarkingAttr->SetValueString(l_sMainMarking.c_str());
			}
		}


		return true;

}

void SP_WDG_ColStMarkingList::OnAdd(wxCommandEvent& p_cEvent)
{

	m_pcMarkingGrid->AppendRows(1);
	unsigned l_nLastRow = m_pcMarkingGrid->GetNumberRows() - 1;
	m_pcMarkingGrid->SetCellValue(l_nLastRow, 1, wxT("()"));
}


void SP_WDG_ColStMarkingList::OnDelete( wxCommandEvent& p_cEvent )
{
	wxGridCellCoordsArray  l_GridCellCoordsArrayTop = m_pcMarkingGrid->GetSelectionBlockTopLeft();
	wxGridCellCoordsArray  l_GridCellCoordsArrayBottom = m_pcMarkingGrid->GetSelectionBlockBottomRight();
	
	if(l_GridCellCoordsArrayTop.size()>0 || l_GridCellCoordsArrayBottom.size()>0)
	{
		int l_nRowUp = 0;
		if(l_GridCellCoordsArrayTop.size()>0)
			l_nRowUp = l_GridCellCoordsArrayTop[l_GridCellCoordsArrayTop.size()-1].GetRow();

		int l_nRowBottom = m_pcMarkingGrid->GetNumberRows()-1;
		if(l_GridCellCoordsArrayBottom.size()>0)
			l_nRowBottom= l_GridCellCoordsArrayBottom[0].GetRow();
		
		for(int i= l_nRowUp; i <= l_nRowBottom; i++)
		{
			m_pcMarkingGrid->DeleteRows(i);
		}
	}
}

void SP_WDG_ColStMarkingList::OnImport(wxCommandEvent& p_cEvent)
{	
	wxString l_sColorSetName ;
	SP_DLG_ColorSetOverview l_cColorSetDlg(NULL);
	if ( l_cColorSetDlg.ShowModal() == wxID_OK )
	{
		l_sColorSetName = l_cColorSetDlg.GetColorSetName();
	}
	l_cColorSetDlg.Destroy();	

	
 SP_WDG_DialogChoice * l_pcWDG_DialogText ;
	list<SP_WDG_DialogBase*>* l_ptlWidgets = m_pcDlg->GettlWidgets();
	list<SP_WDG_DialogBase*>::iterator itList;

	for(itList = l_ptlWidgets->begin(); itList != l_ptlWidgets->end(); itList++)
	{
		l_pcWDG_DialogText = dynamic_cast< SP_WDG_DialogChoice* >( *itList );

		if(l_pcWDG_DialogText && l_pcWDG_DialogText->GetName() == wxT("Markings"))
		{
			wxString ss=l_pcWDG_DialogText->GetCurentSelection();
		//	l_pcWDG_DialogText->GetTextCtrl()->TransferDataToWindow();
		}
	}

}

void SP_WDG_ColStMarkingList::ConnectEvents()
{

	Connect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnAdd);

	Connect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnDelete);

	Connect(
			SP_ID_BUTTON_IMPORT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnImport);

	Connect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnOverview);

	Connect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnCheck);
	
	Connect(
			SP_ID_BUTTON_SHOWMARKING + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnShowMarking);
}

void SP_WDG_ColStMarkingList::DisconnectEvents()
{

	Disconnect(
			SP_ID_BUTTON_ADD + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnAdd);

	Disconnect(
			SP_ID_BUTTON_DELETE + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnDelete);

	Disconnect(
			SP_ID_BUTTON_IMPORT + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnImport);

	Disconnect(
			SP_ID_BUTTON_OVERVIEW + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnOverview);
	
	Disconnect(
			SP_ID_BUTTON_CHECK + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnCheck);

	Disconnect(
			SP_ID_BUTTON_SHOWMARKING + m_nDialogID + wxID_HIGHEST,
			wxEVT_COMMAND_BUTTON_CLICKED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_ColStMarkingList::OnShowMarking);

	
}

void SP_WDG_ColStMarkingList::OnGridCellSelected(wxGridEvent& ev)
{

	m_sOldCellValue = m_pcMarkingGrid->GetCellValue(ev.GetRow(), ev.GetCol());
	 
	 
	SP_WDG_DialogChoice * l_pcWDG_DialogText;
	list<SP_WDG_DialogBase*>* l_ptlWidgets = m_pcDlg->GettlWidgets();
	list<SP_WDG_DialogBase*>::iterator itList;
	wxString l_sSelectedCs;
	for (itList = l_ptlWidgets->begin(); itList != l_ptlWidgets->end(); itList++)
	{
		l_pcWDG_DialogText = dynamic_cast< SP_WDG_DialogChoice* >(*itList);

		if (l_pcWDG_DialogText && l_pcWDG_DialogText->GetName() == wxT("Markings"))
		{
			l_sSelectedCs = l_pcWDG_DialogText->GetCurentSelection();
			 
		}
	}

	SP_CPN_ColorSetClass l_cColorSetClass;
	SP_CPN_ColorSet* l_pcColorSet;
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();
	SP_CPN_ValueAssign l_cValueAssign;

	if (!l_cValueAssign.InitializeColorset(l_cColorSetClass))
		return;
	//check if the color set of the place is not product, then make the tuple column read-olnly
	wxString l_sColorSetName = l_sSelectedCs;// l_pcNameAttibute->GetValue();
	l_pcColorSet = l_cColorSetClass.LookupColorSet(l_sColorSetName);

	if (!l_pcColorSet) return;

	if (l_pcColorSet->GetDataType() != CPN_PRODUCT)
	{
		for (unsigned i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
		{
			m_pcMarkingGrid->SetReadOnly(i, 1);
		}

	}
	else
	{
		for (unsigned i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
		{
			m_pcMarkingGrid->SetReadOnly(i, 1, false);
		}
	}
		
	//make product coloum editable only if it contains marking condition
	for (unsigned i = 0; i < m_pcMarkingGrid->GetNumberRows(); i++)
	{
		wxString l_sMarking = m_pcMarkingGrid->GetCellValue(i, 0);
		if (l_sMarking.Contains(_T("&")) || l_sMarking.Contains(_T("|")) || l_sMarking.Contains(_T("=<"))
			|| l_sMarking.Contains(_T(">="))||l_sMarking.Contains(_T("=")) || l_sMarking.Contains(_T(">")) || l_sMarking.Contains(_T("<")))
		{
			m_pcMarkingGrid->SetReadOnly(i, 1, false);
		}
		else
		{
			SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
			bool l_bIsFunctionCall = false;

			if (l_pcMetadataclass)
			 {
					SP_DS_Metadata*  l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();
					if (l_pcNewMetadata) {
						SP_DS_ColListAttribute*	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));

						for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
							{
								wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
								if (l_sMarking.Contains(l_sName))
								{
									l_bIsFunctionCall = true;
									break;
								}
							}
							}
			 }

		if(l_bIsFunctionCall)//because function call most probably contains boolean guard
			m_pcMarkingGrid->SetReadOnly(i, 1,false);
				else
			 m_pcMarkingGrid->SetReadOnly(i, 1);
		}
		
	}
	
	////
	ev.Skip();

 
}

void SP_WDG_ColStMarkingList::OnGridCellValueChanged(wxGridEvent& ev)
{

	ev.Skip();

}

wxGrid* SP_WDG_ColStMarkingList::GetColorGrid()
{
	return m_pcMarkingGrid;
}


SP_DS_TextAttribute* SP_WDG_ColStMarkingList::GetNameAttibute()
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


void SP_WDG_ColStMarkingList::OnOverview(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColStMarkingOverview l_cDlg(m_pcDlg);

	if (l_cDlg.ShowModal() == wxID_OK)
	{

		LoadData();

	}

	l_cDlg.Destroy();

}

void SP_WDG_ColStMarkingList::OnCheck(wxCommandEvent& p_cEvent)
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
	if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
		return;
	//end 


	wxString l_sVariables = wxT("");
	l_sVariables = wxT("The marking definition");
	//l_sVariables += l_sPredicate;
	l_sVariables += wxT(" is correct.");
	new wxTipWindow(m_pcDlg, l_sVariables, 1000);

}


void SP_WDG_ColStMarkingList::UpdatePlaceColor()
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

void SP_WDG_ColStMarkingList::OnShowMarking(wxCommandEvent& p_cEvent)
{
	SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());	
	SP_DS_Node* l_pcNode;
	if(!l_pcAttr)
		return;

	l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcAttr->GetParent());
	if(!l_pcNode)
		return;

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return;

	/////////////////////////////////////////////////////////

	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	if( ! l_cSyntaxChecking.ComputeInitialMarking(l_pcNode,l_mColorToMarkingMap,false) )
		return;

	wxString l_sMainMarkingNumber, l_sMarkingDetails;
	l_sMarkingDetails << wxT("Color  :  Number\n");
	map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;					
	if( m_sNodeClassName == SP_DS_CONTINUOUS_PLACE)
	{
		double l_dMarking = 0;
		for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
		{		
			l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
			if(itMap->second[0].m_DoubleMultiplicity >= 0.00000000000001)
			{
				l_sMarkingDetails << itMap->first;
				l_sMarkingDetails << wxT(" : ");
				l_sMarkingDetails << itMap->second[0].m_DoubleMultiplicity;
				l_sMarkingDetails << wxT("\n");
			}
		}
		l_sMainMarkingNumber <<l_dMarking;
	}
	else
	{
		long l_nMarking = 0;
		for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
		{			
			l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;	
			if(itMap->second[0].m_intMultiplicity > 0)
			{
				l_sMarkingDetails << itMap->first;
				l_sMarkingDetails << wxT(" : ");
				l_sMarkingDetails << itMap->second[0].m_intMultiplicity;
				l_sMarkingDetails << wxT("\n");
			}
		}
		l_sMainMarkingNumber <<l_nMarking;
	}

	SP_LOGMESSAGE(wxT("There are ")+ l_sMainMarkingNumber + wxT(" tokens, which are as follows:\n") + l_sMarkingDetails);

}
