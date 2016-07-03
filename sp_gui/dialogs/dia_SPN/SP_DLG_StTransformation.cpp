//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/03/28 10:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StTransformation.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

IMPLEMENT_CLASS( SP_DLG_StTransformation, wxDialog )

enum
{

	SP_ID_GRID_MARKING


};

BEGIN_EVENT_TABLE( SP_DLG_StTransformation, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_StTransformation::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_StTransformation::OnDlgCancel )


END_EVENT_TABLE()


SP_DLG_StTransformation::SP_DLG_StTransformation( SP_ListNode* TransformingNodes,
	    wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
	m_TransformingNodes(TransformingNodes)

{

	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* l_pcGridSizer = new wxBoxSizer( wxVERTICAL );

	// new row
	m_pcFunctionGrid = new wxGrid( this, SP_ID_GRID_MARKING,
			wxDefaultPosition, wxSize( 700, 300 ), wxSUNKEN_BORDER );
	m_pcFunctionGrid->CreateGrid( 0, 0);
	m_pcFunctionGrid->EnableEditing( true);	

	m_pcFunctionGrid->SetDefaultColSize( 220, TRUE);
	m_pcFunctionGrid->SetDefaultRowSize( 20, TRUE);

	m_pcFunctionGrid->SetColLabelSize( 16);
	m_pcFunctionGrid->SetRowLabelSize( 80);

	l_pcRowSizer->Add(m_pcFunctionGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	LoadTransformData();	

	const wxString choices[] =
    {
        _T("Not Transform"),
		_T("Transition"),
        _T("Immediate Transition"),
		_T("Deterministic Transition"),
        _T("Scheduled Transition"),
    };
    m_ComboBoxValue=GetComboBoxDefaultValue();

	int j=m_pcFunctionGrid->GetNumberCols()-1;
	for(unsigned m=0;m<m_ComboBoxValue.size();m++)
	{    
		m_pcFunctionGrid->SetCellEditor(m, j, new wxGridCellChoiceEditor(WXSIZEOF(choices), choices));
		m_pcFunctionGrid->SetCellSize(m, j, 1, 1);
		m_pcFunctionGrid->SetCellValue(m, j, m_ComboBoxValue[m]);
		m_pcFunctionGrid->SetCellOverflow(m, j, false);
	}
    
   SP_AutoSizeRowLabelSize(m_pcFunctionGrid);



	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxSizer *l_pcBottomButtonSizer = new wxBoxSizer(wxHORIZONTAL );

	l_pcBottomButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxEXPAND | wxALL, 5);
	
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND | wxALIGN_BOTTOM );

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND | wxALIGN_RIGHT);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_RIGHT);

	SetSizerAndFit(m_pcSizer);

}

void SP_DLG_StTransformation::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow() )
	{
		if (IsModal() )
		{			
			//RefreshFrame();

			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}

}

void SP_DLG_StTransformation::OnDlgCancel(wxCommandEvent& p_cEvent)
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

vector<wxString> SP_DLG_StTransformation::GetComboBoxValue()
{
	int i=m_pcFunctionGrid->GetNumberRows();	 
	int j=m_pcFunctionGrid->GetNumberCols()-1;
	vector<wxString> l_ComboBoxValue;

	for(int m=0;m<i;m++)
	{
		wxString l_Value= m_pcFunctionGrid->GetCellValue(m, j);
		l_ComboBoxValue.push_back(l_Value);

	}
	return l_ComboBoxValue;
}


void SP_DLG_StTransformation::LoadTransformData()
{
	
	SP_DS_ColListAttribute* l_pcColList;
	
	SP_ListNode::const_iterator l_itElem;

	bool l_bWhite = false;

	int l_nGridRowNumber = 0;

	LoadTransformSetNames();

	for (l_itElem = m_TransformingNodes->begin(); l_itElem != m_TransformingNodes->end(); ++l_itElem)
	{

		SP_DS_Node* l_pcTransitionNode = ( *l_itElem );

		wxString l_sFunctionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransitionNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		m_pcFunctionGrid->AppendRows( 1);
		m_pcFunctionGrid->SetRowLabelValue(l_nGridRowNumber, l_sFunctionName);

		l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransitionNode->GetAttribute(wxT("FunctionList")) );

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			m_pcFunctionGrid->SetCellValue(l_nGridRowNumber, i, l_pcColList->GetCell(i, 1));
			m_pcFunctionGrid->SetCellBackgroundColour(l_nGridRowNumber, i, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY ));

			m_pcFunctionGrid->SetCellAlignment(l_nGridRowNumber, i,	wxALIGN_RIGHT, wxALIGN_CENTER);
		}

		(l_bWhite ? l_bWhite = false : l_bWhite = true );
		l_nGridRowNumber++;

	}

	if ( (m_pcFunctionGrid->GetNumberRows() > 0 ) && (m_pcFunctionGrid->GetNumberCols() > 0 ))
	{
		m_sOldCellValue = m_pcFunctionGrid->GetCellValue( 0, 0);
	}

}
void SP_DLG_StTransformation::LoadTransformSetNames()
{
	SP_ListNode::const_iterator l_itElem;	
	l_itElem = m_TransformingNodes->begin();
	{
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("FunctionList")) );
				
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++ )
		{
			wxString l_sSetName = l_pcAttr->GetCell(i,0);
			
			m_pcFunctionGrid->AppendCols( 1);					
			m_pcFunctionGrid->SetColLabelValue(i, l_sSetName);
		}
		
	}	
	
	m_pcFunctionGrid->AppendCols( 1);
	int m=m_pcFunctionGrid->GetNumberCols();
	m_pcFunctionGrid->SetColLabelValue(m-1, wxT("Please select transformation"));

}

vector<wxString> SP_DLG_StTransformation::GetComboBoxDefaultValue()
{
	SP_ListNode::const_iterator l_itElem;
	vector<wxString> l_ComboBoxValue;
	for (l_itElem = m_TransformingNodes->begin(); l_itElem != m_TransformingNodes->end(); ++l_itElem)
	{
		SP_DS_ColListAttribute* l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("FunctionList")) );
		wxString l_Function=l_pcColList->GetCell(0, 1);
		l_Function.Replace(wxT(" "), wxT(""));
		l_Function.Replace(wxT("\t"), wxT(""));
		l_Function.Replace(wxT("\r"), wxT(""));
		l_Function.Replace(wxT("\n"), wxT(""));

		if (l_Function.Find(wxT("ImmediateFiring(")) != -1)
			l_ComboBoxValue.push_back(wxT("Immediate Transition"));
		else if(l_Function.Find(wxT("TimedFiring(")) != -1)
			l_ComboBoxValue.push_back(wxT("Deterministic Transition"));
		else if(l_Function.Find(wxT("FixedTimedFiring_Single")) != -1)
			l_ComboBoxValue.push_back(wxT("Scheduled Transition"));
		else if(l_Function.Find(wxT("FixedTimedFiring_Periodic")) != -1)
			l_ComboBoxValue.push_back(wxT("Scheduled Transition"));
		else
			l_ComboBoxValue.push_back(wxT("Transition"));
	}
	return l_ComboBoxValue;
}


