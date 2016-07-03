/*
 * SP_DLG_HybridMarkingOverview.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 5.7.2010
 * Short Description:
 */
 //======================================================================================

#include <wx/valgen.h>

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridMarkingOverview.h"

IMPLEMENT_CLASS( SP_DLG_HybridMarkingOverview, wxDialog )

enum
{

	SP_ID_GRID_MARKING = SP_ID_LAST_ID + 100,
	SP_ID_BUTTON_ADDSET,
	SP_ID_BUTTON_DELETESET,
	SP_ID_BUTTON_RENAMESET,
	SP_ID_CHECKBOX_DISCRETE_PLACE,
	SP_ID_CHECKBOX_CONTINUOUS_PLACE,

};

BEGIN_EVENT_TABLE( SP_DLG_HybridMarkingOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_HybridMarkingOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_HybridMarkingOverview::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_ADDSET, SP_DLG_HybridMarkingOverview::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_DELETESET, SP_DLG_HybridMarkingOverview::OnDeleteSet )
EVT_BUTTON( SP_ID_BUTTON_RENAMESET, SP_DLG_HybridMarkingOverview::OnRenameSet )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_HybridMarkingOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_HybridMarkingOverview::OnCellValueChanged )
#endif

EVT_GRID_SELECT_CELL( SP_DLG_HybridMarkingOverview::OnGridCellSelected )

EVT_CHECKBOX( SP_ID_CHECKBOX_DISCRETE_PLACE, SP_DLG_HybridMarkingOverview::OnDiscretePlaceCheckedUnchecked )
EVT_CHECKBOX( SP_ID_CHECKBOX_CONTINUOUS_PLACE, SP_DLG_HybridMarkingOverview::OnContinuousPlaceCheckedUnchecked )

END_EVENT_TABLE()

SP_DLG_HybridMarkingOverview::SP_DLG_HybridMarkingOverview(wxWindow* p_pcParent,
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


	m_pcMarkingGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcMarkingGrid->CreateGrid( 0, 0);

	m_pcMarkingGrid->EnableEditing( true);


	m_pcMarkingGrid->SetDefaultColSize( 80, TRUE);
	m_pcMarkingGrid->SetDefaultRowSize( 20, TRUE);

	m_pcMarkingGrid->SetColLabelSize( 16);
	m_pcMarkingGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcMarkingGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcMarkingGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer *l_pcTopButtonSizer = new wxBoxSizer( wxHORIZONTAL );

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_ADDSET, wxT("Add marking set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_DELETESET, wxT("Delete marking set") ), 0, wxEXPAND | wxALL, 5);

	l_pcTopButtonSizer->Add(new wxButton( this, SP_ID_BUTTON_RENAMESET, wxT("Rename marking set") ), 0, wxEXPAND | wxALL, 5);

	wxCheckBox* l_pcDiscretePlaceCheckBox=new wxCheckBox(this,SP_ID_CHECKBOX_DISCRETE_PLACE,wxT("Discrete"));
    wxCheckBox* l_pcContinuousPlaceCheckBox=new wxCheckBox(this,SP_ID_CHECKBOX_CONTINUOUS_PLACE,wxT("Continuous"));
    l_pcDiscretePlaceCheckBox->SetValue(true);
    l_pcContinuousPlaceCheckBox->SetValue(true);
    l_pcDiscretePlaceCheckBox->Enable(true);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(l_pcDiscretePlaceCheckBox, 0, wxALL, 5);
	l_pcRowSizer->Add(l_pcContinuousPlaceCheckBox, 0, wxALL, 5);

	wxSizer *l_pcCheckBoxSizer = new wxBoxSizer( wxVERTICAL );
	l_pcCheckBoxSizer->Add(l_pcDiscretePlaceCheckBox, 0, wxEXPAND | wxALIGN_TOP);
	l_pcCheckBoxSizer->Add(l_pcContinuousPlaceCheckBox, 0, wxEXPAND | wxALIGN_TOP);

	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer( wxHORIZONTAL );


	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,
			wxEXPAND | wxALL, 5);

	l_pcButtonSizer->Add(l_pcTopButtonSizer, 0, wxEXPAND | wxALIGN_TOP
			);
	l_pcButtonSizer->Add(l_pcCheckBoxSizer, 0, wxEXPAND | wxALIGN_TOP
				);

	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM
			);

	m_pcSizer->Add(l_pcGridSizer, 0, wxALIGN_CENTER | wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT | wxEXPAND);

	SetSizerAndFit(m_pcSizer);
	 m_pcMarkingGrid->SetRowMinimalAcceptableHeight(0);

	 //Initially all place names are visible
	 m_bDiscretePlacesVisible=true;
	 m_bContinuousPlacesVisiable=true;

}

void SP_DLG_HybridMarkingOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_HybridMarkingOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_HybridMarkingOverview::OnAddSet(wxCommandEvent& p_cEvent)
{

	bool l_bWhite = false;
	wxTextEntryDialog l_pcDialog( this, _T("Name of the new marking set:"), wxT("Add marking set"));
	wxString l_sName= wxT("M-Set ");

	l_sName.Append(wxString::Format(wxT("%d"), m_pcMarkingGrid->GetNumberCols() + 1) );

	l_pcDialog.SetValue(l_sName);

	int l_nModalResult = l_pcDialog.ShowModal();

	if (wxID_OK == l_nModalResult)
	{

		if (ExistMarkingSetName(l_pcDialog.GetValue() ) )
		{

			SP_MESSAGEBOX(wxT("The inputed name is already existing."),
					wxT("Invalid marking set name"),
					wxOK | wxICON_ERROR);

			return;

		}

		l_sName = l_pcDialog.GetValue();

		m_pcMarkingGrid->AppendCols( 1);
		m_pcMarkingGrid->SetColLabelValue(m_pcMarkingGrid->GetNumberCols() - 1,
				l_sName);

		int l_nGridCol = m_pcMarkingGrid->GetNumberCols() - 1;
		int l_nGridRow = 0;

		if ( 0 < l_nGridCol)
		{

			for (l_nGridRow = 0; l_nGridRow < m_pcMarkingGrid->GetNumberRows(); l_nGridRow++)
			{

				m_pcMarkingGrid->SetCellValue(l_nGridRow, l_nGridCol,
						m_pcMarkingGrid->GetCellValue(l_nGridRow, l_nGridCol
								- 1) );

				m_pcMarkingGrid->SetCellAlignment(l_nGridRow, l_nGridCol,
						wxALIGN_RIGHT, wxALIGN_CENTER);

				m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRow,
						l_nGridCol, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));
				(l_bWhite ? l_bWhite = false : l_bWhite = true );

			}

		}

	}

}

void SP_DLG_HybridMarkingOverview::OnDeleteSet(wxCommandEvent& p_cEvent)
{

	wxArrayInt l_cSelCols = m_pcMarkingGrid->GetSelectedCols();

	if ( ! (l_cSelCols.Count() > 0 ))
	{

		SP_MESSAGEBOX(wxT("Select column(s) to delete."),
				wxT("Invalid marking marking choice"), wxOK | wxICON_ERROR);
		return;

	}

	if (l_cSelCols.Item( 0) == 0)
	{

		SP_MESSAGEBOX(wxT("Main marking set cannot be removed."), wxT("Error"),
				wxOK | wxICON_ERROR);

		if (l_cSelCols.Count() == 1)
		{

			return;

		}
		else
		{

			m_pcMarkingGrid->DeleteCols( 1, l_cSelCols.Count() - 1);
			return;

		}

	}

	m_pcMarkingGrid->DeleteCols(l_cSelCols.Item( 0), l_cSelCols.Count() );

}

void SP_DLG_HybridMarkingOverview::OnRenameSet(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{

		wxArrayInt l_pcSelCols = m_pcMarkingGrid->GetSelectedCols();

		if (l_pcSelCols.Count() != 1)
		{

			SP_MESSAGEBOX(wxT("Select a single column to rename."),
					wxT("Invalid marking marking choice"), wxOK | wxICON_ERROR);
			return;

		}

		if (l_pcSelCols.Item( 0) == 0)
		{

			SP_MESSAGEBOX(wxT("Main marking set cannot be renamed."),
					wxT("Invalid marking set choice"), wxOK | wxICON_ERROR);
			return;

		}

		wxTextEntryDialog l_pcDialog( this, wxT("New name of the marking set: ") +
		m_pcMarkingGrid->GetColLabelValue( l_pcSelCols.Item( 0 ) ), wxT("Rename marking set"));

		l_pcDialog.SetValue(m_pcMarkingGrid->GetColLabelValue(l_pcSelCols.Item( 0) ) );

		int l_nModalResult = l_pcDialog.ShowModal();

		if (wxID_OK == l_nModalResult)
		{

			if (l_pcDialog.GetValue().Find(wxT("@")) != -1)
			{

				SP_MESSAGEBOX(
						wxT("Do not use the symbol @ in the marking set name."),
						wxT("Invalid marking set name"), wxOK | wxICON_ERROR);
				return;

			}

			if (ExistMarkingSetName(l_pcDialog.GetValue() ) )
			{

				SP_MESSAGEBOX(
						wxT("Do not use an already existing marking set name."),
						wxT("Invalid marking set name"),
						wxOK | wxICON_ERROR);

				return;

			}

			m_pcMarkingGrid->SetColLabelValue(l_pcSelCols.Item( 0),
					l_pcDialog.GetValue() );

		}

	}

}

void SP_DLG_HybridMarkingOverview::LoadData()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Nodeclass* l_pcDiscreteNodeclass=l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	SP_DS_Nodeclass* l_pcContinuousNodeclass=l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	SP_ListNode::const_iterator l_itElem;
	bool l_bWhite = false;
	wxString l_sPlaceName;
	wxString l_sLoadName = wxT("-1");

	int l_nGridRowNumber = 0;

	//Get the Discrete and Continuous places Number
	m_nDiscretePlacesNumber=l_pcDiscreteNodeclass->GetElements()->size();
	m_nContinuousPlacesNumber=l_pcContinuousNodeclass->GetElements()->size();

	//Load names
	LoadSetNames();

	//load Discrete places
	for (l_itElem = l_pcDiscreteNodeclass->GetElements()->begin(); l_itElem != l_pcDiscreteNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_pcMarkingGrid->AppendRows( 1);
		m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);
		m_sPlacesNammes.push_back(l_sPlaceName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("MarkingList")) );

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcMarkingGrid->SetCellValue(l_nGridRowNumber, i, l_pcColList->GetCell(i, 1));
			m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRowNumber, i,
					(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcMarkingGrid->SetCellAlignment(l_nGridRowNumber, i,
					wxALIGN_RIGHT, wxALIGN_CENTER);

		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;

		( *l_itElem )->Update( true);
	}

	//load Continuous places
	for (l_itElem = l_pcContinuousNodeclass->GetElements()->begin(); l_itElem != l_pcContinuousNodeclass->GetElements()->end(); ++l_itElem)
	{
	  l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	  m_pcMarkingGrid->AppendRows( 1);
	  m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);
	  m_sPlacesNammes.push_back(l_sPlaceName);
	  l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("MarkingList")) );

	  for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	  {
		  m_pcMarkingGrid->SetCellValue(l_nGridRowNumber, i, l_pcColList->GetCell(i, 1));
		  m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRowNumber, i,
						(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

		 m_pcMarkingGrid->SetCellAlignment(l_nGridRowNumber, i,
						wxALIGN_RIGHT, wxALIGN_CENTER);

	  }

	 (l_bWhite ? l_bWhite = false : l_bWhite = true );
	 l_nGridRowNumber++;

	 ( *l_itElem )->Update( true);
  }


	if ( (m_pcMarkingGrid->GetNumberRows() > 0 )
			&& (m_pcMarkingGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcMarkingGrid->GetCellValue( 0, 0);

	}

}

void SP_DLG_HybridMarkingOverview::SaveData()
{
	m_pcMarkingGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_ColListAttribute* l_pcColList;

	SP_DS_Nodeclass* l_pcDiscreteNodeclass=l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	SP_DS_Nodeclass* l_pcContinuousNodeclass=l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	for (l_itElem = l_pcDiscreteNodeclass->GetElements()->begin(); l_itElem != l_pcDiscreteNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("MarkingList")) );

		l_pcColList->Clear();

		for (int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			wxString l_sLabel = m_pcMarkingGrid->GetColLabelValue(i);
			wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nGridRowNumber, i);
			l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);

			l_pcColList->SetCell(l_nNewRow, 1, l_sValue);
			if(l_sLabel == wxT("Main"))
			{

				SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast< SP_DS_Attribute*>(( *l_itElem )->GetAttribute( wxT("Marking") ) );
				if(l_pcMarkingAttr)
				{
				  l_pcMarkingAttr->SetValueString(l_sValue);
				}

			}
		}
		l_nGridRowNumber++;

		(*l_itElem)->Update();
	}

   //Continuous
	for (l_itElem = l_pcContinuousNodeclass->GetElements()->begin(); l_itElem != l_pcContinuousNodeclass->GetElements()->end(); ++l_itElem)
		{

			l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(wxT("MarkingList")) );

			l_pcColList->Clear();

			for (int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
			{
				unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
				wxString l_sLabel = m_pcMarkingGrid->GetColLabelValue(i);
				wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nGridRowNumber, i);
				l_pcColList->SetCell(l_nNewRow, 0, l_sLabel);

				l_pcColList->SetCell(l_nNewRow, 1, l_sValue);
				if(l_sLabel == wxT("Main"))
				{

					SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast< SP_DS_Attribute*>(( *l_itElem )->GetAttribute( wxT("Marking") ) );
					if(l_pcMarkingAttr)
					{
					  l_pcMarkingAttr->SetValueString(l_sValue);
					}

				}
			}
			l_nGridRowNumber++;

			(*l_itElem)->Update();
		}

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_HybridMarkingOverview::OnCellValueChanged(wxGridEvent& ev)
{

	long l_nLong = -1;
    double l_nDouble=-1.0;
	wxString l_sValue =
			m_pcMarkingGrid->GetCellValue(ev.GetRow(), ev.GetCol() );
    wxString l_sNodeType;

   l_sNodeType=(ev.GetRow()<m_nDiscretePlacesNumber)? SP_DS_DISCRETE_PLACE: SP_DS_CONTINUOUS_PLACE;

	if ( ! (l_sValue.ToLong( &l_nLong) && (l_nLong >= 0 ) ) && l_sNodeType==SP_DS_DISCRETE_PLACE)
	{

		SP_MESSAGEBOX(
				wxT("The marking value must be a non-negative integer value. The inputed value is invalid."),
				wxT("Error"), wxOK | wxICON_ERROR);

		m_pcMarkingGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);

	}
	else if(! (l_sValue.ToDouble( &l_nDouble) && (l_nDouble >= 0 ) ) && l_sNodeType==SP_DS_CONTINUOUS_PLACE)
	{
		SP_MESSAGEBOX(
						wxT("The marking value must be a non-negative real value. The inputed value is invalid."),
						wxT("Error"), wxOK | wxICON_ERROR);

		m_pcMarkingGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
	}

	ev.Skip();

}

void SP_DLG_HybridMarkingOverview::OnGridCellSelected(wxGridEvent& ev)
{

	if ( (m_pcMarkingGrid->GetNumberRows() > 0 )
			&& (m_pcMarkingGrid->GetNumberCols() > 0 ))
	{

		m_sOldCellValue = m_pcMarkingGrid->GetCellValue(ev.GetRow(),
				ev.GetCol() );

	}

	ev.Skip();

}

void SP_DLG_HybridMarkingOverview::LoadSetNames()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(m_nDiscretePlacesNumber> 0)
	{
		//Discrete
		SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->front();
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
		{
			wxString l_sSetName = l_pcAttr->GetCell(i,0);
			m_pcMarkingGrid->AppendCols( 1);
			m_pcMarkingGrid->SetColLabelValue(i, l_sSetName);
		}
	}
	else if(m_nContinuousPlacesNumber> 0)
	{
		//Continuous
		SP_DS_Node* l_pcNode = l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->front();
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
		{
		  wxString l_sSetName = l_pcAttr->GetCell(i,0);
		  m_pcMarkingGrid->AppendCols( 1);
		  m_pcMarkingGrid->SetColLabelValue(i+m_nDiscretePlacesNumber, l_sSetName);
		}
	}
}

bool SP_DLG_HybridMarkingOverview::ExistMarkingSetName(const wxString& p_sName)
{

	for (int i = 0; i < m_pcMarkingGrid->GetNumberCols(); i++)
	{

		if (p_sName ==  m_pcMarkingGrid->GetColLabelValue(i) )
		{

			return true;

		}

	}

	return false;

}

void SP_DLG_HybridMarkingOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}

void SP_DLG_HybridMarkingOverview::OnDiscretePlaceCheckedUnchecked(wxCommandEvent& ev)
{


  for(long l_nRow=0;l_nRow<m_nDiscretePlacesNumber+m_nContinuousPlacesNumber;l_nRow++)
   {
	 m_pcMarkingGrid->SetRowLabelValue(l_nRow,wxT(""));
	}
	if(!ev.IsChecked())
	{
	 for(long l_nRow=0;l_nRow<m_nDiscretePlacesNumber;l_nRow++)
	  {
		  m_pcMarkingGrid->SetRowLabelValue(l_nRow,wxT(""));
		  m_pcMarkingGrid->SetRowMinimalHeight(l_nRow,m_pcMarkingGrid->GetRowMinimalAcceptableHeight());

	  }
	 m_bDiscretePlacesVisible=false;
	}
	else
	{
		for(long l_nRow=0;l_nRow<m_nDiscretePlacesNumber;l_nRow++)
		{
		 m_pcMarkingGrid->SetRowMinimalHeight(l_nRow,m_pcMarkingGrid->GetDefaultRowSize());
		}
		 m_bDiscretePlacesVisible=true;
	}
	CheckPlacesNamesVisiability();

}

void SP_DLG_HybridMarkingOverview::OnContinuousPlaceCheckedUnchecked(wxCommandEvent& ev)
{
	for(long l_nRow=0;l_nRow<m_nDiscretePlacesNumber+m_nContinuousPlacesNumber;l_nRow++)
	   {
		 m_pcMarkingGrid->SetRowLabelValue(l_nRow,wxT(""));
		}
	if(!ev.IsChecked())
    {
      for(long l_nRow=m_nDiscretePlacesNumber;l_nRow<m_nContinuousPlacesNumber+m_nDiscretePlacesNumber;l_nRow++)
      {
	    m_pcMarkingGrid->SetRowMinimalHeight(l_nRow,m_pcMarkingGrid->GetRowMinimalAcceptableHeight());
	    m_pcMarkingGrid->SetRowLabelValue(1,wxT(""));

      }

      m_bContinuousPlacesVisiable=false;
    }
	else
	 {
		for(long l_nRow=m_nDiscretePlacesNumber;l_nRow<m_nContinuousPlacesNumber+m_nDiscretePlacesNumber;l_nRow++)
		 {
		  m_pcMarkingGrid->SetRowMinimalHeight(l_nRow,m_pcMarkingGrid->GetDefaultRowSize());
		 }

		 m_bContinuousPlacesVisiable=true;
	 }
	CheckPlacesNamesVisiability();
}

void SP_DLG_HybridMarkingOverview::CheckPlacesNamesVisiability()
{
    //Both are visible
	if(m_bDiscretePlacesVisible && m_bContinuousPlacesVisiable)
		for(long l_nRow=0;l_nRow<m_nContinuousPlacesNumber+m_nDiscretePlacesNumber;l_nRow++)
		    m_pcMarkingGrid->SetRowLabelValue(l_nRow,m_sPlacesNammes[l_nRow]);

	//Only Discrete
	else if(m_bDiscretePlacesVisible && !m_bContinuousPlacesVisiable)
			for(long l_nRow=0;l_nRow<m_nDiscretePlacesNumber;l_nRow++)
					    m_pcMarkingGrid->SetRowLabelValue(l_nRow,m_sPlacesNammes[l_nRow]);
	//Only Continuous
	else if(!m_bDiscretePlacesVisible && m_bContinuousPlacesVisiable)
		for(long l_nRow=m_nDiscretePlacesNumber;l_nRow<m_nContinuousPlacesNumber+m_nDiscretePlacesNumber;l_nRow++)
				    m_pcMarkingGrid->SetRowLabelValue(l_nRow,m_sPlacesNammes[l_nRow]);

}
