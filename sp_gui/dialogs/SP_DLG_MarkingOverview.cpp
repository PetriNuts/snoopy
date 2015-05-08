//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2013/09/19 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

IMPLEMENT_CLASS( SP_DLG_MarkingOverview, wxDialog )

enum
{

	SP_ID_GRID_MARKING

};

BEGIN_EVENT_TABLE( SP_DLG_MarkingOverview, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_MarkingOverview::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_MarkingOverview::OnDlgCancel )

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED( SP_DLG_MarkingOverview::OnCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE( SP_DLG_MarkingOverview::OnCellValueChanged )
#endif

    EVT_GRID_EDITOR_SHOWN( SP_DLG_MarkingOverview::OnGridCellSelected )

END_EVENT_TABLE()

SP_DLG_MarkingOverview::SP_DLG_MarkingOverview(wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
			p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	wxSizer* l_pcGridSizer = new wxStaticBoxSizer( new wxStaticBox( this,
					-1, wxT("") ), wxVERTICAL );

	// new row


	m_pcMarkingGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 780, 550 ), wxSUNKEN_BORDER );

	m_pcMarkingGrid->CreateGrid( 0, 0);

	m_pcMarkingGrid->EnableEditing( true);
	//  m_pcMarkingGrid->SetSelectionMode( wxGrid::wxGridSelectRows );

	m_pcMarkingGrid->SetDefaultColSize( 80, TRUE);
	m_pcMarkingGrid->SetDefaultRowSize( 20, TRUE);

	m_pcMarkingGrid->SetColLabelSize( 16);
	m_pcMarkingGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcMarkingGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadData();

	SP_AutoSizeRowLabelSize(m_pcMarkingGrid);

	m_pcSizer->Add(l_pcGridSizer, 1, wxALIGN_CENTER | wxEXPAND);
	m_pcSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_RIGHT | wxEXPAND);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_MarkingOverview::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_MarkingOverview::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_MarkingOverview::LoadData()
{
	m_nNumDiscretePlaces = 0;
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	SP_ListNode::const_iterator l_itElem;
	bool l_bWhite = false;

	int l_nGridRowNumber = 0;
	m_pcMarkingGrid->AppendCols(1);
	m_pcMarkingGrid->SetColLabelValue(0, wxT("Marking"));

	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{

			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_pcMarkingGrid->AppendRows( 1);
			m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);

			SP_DS_Attribute* l_pcAttr = (*l_itElem)->GetAttribute(wxT("Marking"));
			wxString l_sToken = l_pcAttr->GetValueString();

			m_pcMarkingGrid->SetCellValue(l_nGridRowNumber, 0, l_sToken);
			m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,
					(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcMarkingGrid->SetCellAlignment(l_nGridRowNumber, 0,
					wxALIGN_RIGHT, wxALIGN_CENTRE);

			(l_bWhite ? l_bWhite = false : l_bWhite = true );
			l_nGridRowNumber++;

			(*l_itElem)->Update( true);
			++m_nNumDiscretePlaces;
		}
	}

	m_nNumContinuousPlaces = 0;
	l_pcNodeclass = l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{

			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			m_pcMarkingGrid->AppendRows( 1);
			m_pcMarkingGrid->SetRowLabelValue(l_nGridRowNumber, l_sPlaceName);

			SP_DS_Attribute* l_pcAttr = (*l_itElem)->GetAttribute(wxT("Marking"));
			wxString l_sToken = l_pcAttr->GetValueString();

			m_pcMarkingGrid->SetCellValue(l_nGridRowNumber, 0, l_sToken);
			m_pcMarkingGrid->SetCellBackgroundColour(l_nGridRowNumber, 0,
					(l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcMarkingGrid->SetCellAlignment(l_nGridRowNumber, 0,
					wxALIGN_RIGHT, wxALIGN_CENTRE);

			(l_bWhite ? l_bWhite = false : l_bWhite = true );
			l_nGridRowNumber++;

			(*l_itElem)->Update( true);
			++m_nNumContinuousPlaces;
		}
	}

}

void SP_DLG_MarkingOverview::SaveData()
{
	m_pcMarkingGrid->SaveEditControlValue();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	SP_ListNode::const_iterator l_itElem;

	int l_nGridRowNumber = 0;

	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nGridRowNumber, 0);
			SP_DS_Attribute* l_pcMarkingAttr = (*l_itElem)->GetAttribute(wxT("Marking"));
			l_pcMarkingAttr->SetValueString(l_sValue);

			l_nGridRowNumber++;

			(*l_itElem)->Update();
		}
	}

	l_pcNodeclass = l_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			wxString l_sValue = m_pcMarkingGrid->GetCellValue(l_nGridRowNumber, 0);
			SP_DS_Attribute* l_pcMarkingAttr = (*l_itElem)->GetAttribute(wxT("Marking"));
			l_pcMarkingAttr->SetValueString(l_sValue);

			l_nGridRowNumber++;

			(*l_itElem)->Update();
		}
	}

	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

void SP_DLG_MarkingOverview::OnCellValueChanged(wxGridEvent& ev)
{
	wxString l_sValue =
			m_pcMarkingGrid->GetCellValue(ev.GetRow(), ev.GetCol() );

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sValue));
	if(!l_pcFunction)
	{
		m_pcMarkingGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
		m_pcMarkingGrid->Update();
		SP_MESSAGEBOX(wxT("the value ") + l_sValue + wxT(" is not allowed"), wxT("Error"), wxOK | wxICON_ERROR);
	}
	else
	{
		if(m_nNumDiscretePlaces > 0 && ev.GetRow() < m_nNumDiscretePlaces)
		{

			long l_Val = SP_DS_FunctionEvaluatorLong{l_pcFR, l_pcFunction, -1}();
			if ( l_Val < 0)
			{
				m_pcMarkingGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
				m_pcMarkingGrid->Update();
				SP_MESSAGEBOX(wxT("the value ") + l_sValue + wxT(" is not allowed"), wxT("Error"), wxOK | wxICON_ERROR);
			}
		}
		else
		{
			double l_Val = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_pcFunction, -1}();
			if ( l_Val < 0)
			{
				m_pcMarkingGrid->SetCellValue(ev.GetRow(), ev.GetCol(), m_sOldCellValue);
				m_pcMarkingGrid->Update();
				SP_MESSAGEBOX(wxT("the value ") + l_sValue + wxT(" is not allowed"), wxT("Error"), wxOK | wxICON_ERROR);
			}
		}
	}
	ev.Skip();
}

void SP_DLG_MarkingOverview::OnGridCellSelected(wxGridEvent& ev)
{

	if ( (m_pcMarkingGrid->GetNumberRows() > 0 )
			&& (m_pcMarkingGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcMarkingGrid->GetCellValue(ev.GetRow(), ev.GetCol() );
	}

	ev.Skip();
}

void SP_DLG_MarkingOverview::RefreshFrame()
{
	SP_Core::Instance()->GetRootDocument()->Refresh();
}
