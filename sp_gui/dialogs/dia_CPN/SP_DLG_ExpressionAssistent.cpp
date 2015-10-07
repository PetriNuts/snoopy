//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ExpressionAssistent.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"



IMPLEMENT_CLASS( SP_DLG_ExpressionAssistent, wxDialog )

enum
{
	SP_ID_TEXTCTRL_FORMULA,
	SP_ID_BUTTON_CHECK,
	SP_ID_LISTCTRL_VARIABLE,
	SP_ID_LISTCTRL_CONSTANT,
	SP_ID_LISTCTRL_FUNCTION

};

BEGIN_EVENT_TABLE( SP_DLG_ExpressionAssistent, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ExpressionAssistent::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ExpressionAssistent::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_ExpressionAssistent::OnCheckFormula )

END_EVENT_TABLE()

BEGIN_EVENT_TABLE( SP_DLG_ExpressionAssistent_ListCtrl, wxListCtrl )

EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_VARIABLE, SP_DLG_ExpressionAssistent_ListCtrl::OnDblClick )
EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_CONSTANT, SP_DLG_ExpressionAssistent_ListCtrl::OnDblClick )
EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_FUNCTION, SP_DLG_ExpressionAssistent_ListCtrl::OnDblClick )

END_EVENT_TABLE()

SP_DLG_ExpressionAssistent::SP_DLG_ExpressionAssistent(SP_CPN_ExprAssistType p_ExprAssistType, wxString p_psColorsetName, wxString p_sPlaceType,SP_CPN_ColorSetClass& p_pcColorSetClass,
		wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText,
		const wxString& p_sTitle, long p_nStyle)

:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize(300, 500),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX )

{	
	m_sColorsetName = p_psColorsetName;
	m_sPlaceType = p_sPlaceType;
	m_pcColorSetClass = &p_pcColorSetClass;
	SetTitle(wxT("Expression assistent"));	

	m_ExprAssistType = p_ExprAssistType;

	wxString l_sTypeLabel = wxT("Expression");
	wxString l_sVariableLabel = wxT("Variables");

	m_sReturnText = p_psReturnText;

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	//formula
	wxBoxSizer* l_pcFormulaSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcFormulaSizer->Add(new wxStaticText(this, -1, l_sTypeLabel), 0, wxALL | wxALIGN_CENTER | wxEXPAND, 5);
	m_pcFormulaTextCtrl = new wxTextCtrl(this, SP_ID_TEXTCTRL_FORMULA, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	l_pcFormulaSizer->Add(m_pcFormulaTextCtrl, 1, wxALL | wxEXPAND, 5);
	l_pcFormulaSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check")), 0, wxALL | wxEXPAND, 5);
	m_pcSizer->Add(l_pcFormulaSizer, 1, wxALL | wxEXPAND, 1);

	//listboxes
	wxBoxSizer* l_pcListBoxesSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* l_pcPlaceListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, l_sVariableLabel), wxHORIZONTAL);
	m_pcVariableListCtrl = new SP_DLG_ExpressionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_VARIABLE, wxDefaultPosition,
								wxSize(150, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcPlaceListBoxSizer->Add(m_pcVariableListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcPlaceListBoxSizer, 1, wxALL| wxEXPAND, 2);

	wxStaticBoxSizer* l_pcParameterListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Constants")), wxHORIZONTAL);
	m_pcConstantListCtrl = new SP_DLG_ExpressionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_CONSTANT, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcParameterListBoxSizer->Add(m_pcConstantListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcParameterListBoxSizer, 1, wxALL| wxEXPAND, 2);

	wxStaticBoxSizer* l_pcFunctionListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Functions / Operators")), wxHORIZONTAL);
	m_pcFunctionListCtrl = new SP_DLG_ExpressionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_FUNCTION, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcFunctionListBoxSizer->Add(m_pcFunctionListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcFunctionListBoxSizer, 1, wxALL| wxEXPAND, 2);
	m_pcSizer->Add(l_pcListBoxesSizer, 2, wxALL | wxEXPAND, 1);

	//buttons
	wxBoxSizer* l_pcButtonSizer =	new wxBoxSizer(wxHORIZONTAL);
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, 5);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxALL | wxALIGN_RIGHT, 1);

	LoadData();

	//  SetAutoLayout( TRUE );
	SetSizer(m_pcSizer);

	//  m_pcSizer->Fit( this );
	m_pcSizer->SetSizeHints(this);

	Layout();

}

//------------------------------------------------------------------------
void SP_DLG_ExpressionAssistent::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow())
	{
		m_sReturnText = m_pcFormulaTextCtrl->GetValue();
		if (IsModal())
		{
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}

	}

}

void SP_DLG_ExpressionAssistent::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_ExpressionAssistent::LoadConstant()
{
	m_pcConstantListCtrl->ClearAll();

	if( !m_pcColorSetClass)
		return;
	
	wxListItem l_cItem;
	int l_nPos = 0;
	
	//Load constants
	m_pcConstantListCtrl->InsertColumn(0, wxT("Constant"), wxLIST_FORMAT_LEFT);
	m_pcConstantListCtrl->InsertColumn(1, wxT("Colorset"), wxLIST_FORMAT_LEFT);
	m_pcConstantListCtrl->InsertColumn(2, wxT("Value"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);	
	l_cItem.m_mask = wxLIST_MASK_TEXT;

	map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	for( itMap = m_pcColorSetClass->GetConstantMap()->begin(); itMap != m_pcColorSetClass->GetConstantMap()->end(); itMap++)
	{
		l_cItem.m_col = 0;
		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << itMap->first;
		long l_nIndex = m_pcConstantListCtrl->InsertItem(l_cItem);

		l_cItem.m_text = wxT("");	
		if( itMap->second.m_ColorSet != wxT(""))
			l_cItem.m_text << itMap->second.m_ColorSet;
		else
		{
			SP_CPN_DATATYPE l_DataType = itMap->second.m_DataType;
			if(l_DataType == 0)
				l_cItem.m_text << wxT("dot");
			else if(l_DataType == 1)
				l_cItem.m_text << wxT("int");
			else if(l_DataType == 2)
				l_cItem.m_text << wxT("string");
			else if(l_DataType == 3)
				l_cItem.m_text << wxT("bool");
			else if(l_DataType == 4)
				l_cItem.m_text << wxT("enum");
			else if(l_DataType == 5)
				l_cItem.m_text << wxT("index");
			else if(l_DataType == 6)
				l_cItem.m_text << wxT("product");
			else
			{}
		}
		m_pcConstantListCtrl->SetItem(l_nIndex,1,l_cItem.m_text);


		l_cItem.m_text = wxT("");
		if(itMap->second.m_DataType == CPN_INTEGER)
			l_cItem.m_text << itMap->second.m_IntegerValue;
		else if(itMap->second.m_DataType == CPN_BOOLEAN)
			l_cItem.m_text << itMap->second.m_BooeanValue;
		else
			l_cItem.m_text << itMap->second.m_StringValue;
		
		m_pcConstantListCtrl->SetItem(l_nIndex,2,l_cItem.m_text);
	}
}

void SP_DLG_ExpressionAssistent::LoadFunctions()
{	
	wxListItem l_cItem;
	int l_nPos = 0;
	long l_nIndex = 0;

	m_pcFunctionListCtrl->InsertColumn(0, wxT("Operators"), wxLIST_FORMAT_LEFT);
	m_pcFunctionListCtrl->InsertColumn(1, wxT("Description"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);
	l_cItem.m_col = 0;
	l_cItem.m_mask = wxLIST_MASK_TEXT; 

	if(m_ExprAssistType == EXPRESSION_ARCGUARD)
	{

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("all()");
		l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
		m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Return all colors of a color set"));

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("++");
		l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
		m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Multiset addition"));
	}

	if(m_ExprAssistType == EXPRESSION_TRACEANALYSIS)
	{
		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("Neighbor1D()");
		l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
		m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Return the number of neighbors for a cell"));

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("Neighbor2D()");
		l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
		m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Return the number of neighbors for a cell"));
	}

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("abs(.)");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Return the absolute value"));


	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("[ ]");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("[Predicate]"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("`");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Separate a pair of coefficient & color"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("@");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Extract the index from an index color"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("+");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Successor"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("-");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Predecessor"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT(":");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Extract component element from a product color"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("+");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Add"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("-");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Substract"));


	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("*");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Multiply"));


	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("/");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Divide"));


	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("%");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Modulus"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("^");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Power"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("&");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("And"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("|");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Or"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("!");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Not"));	

		l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("<");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Less than"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("<=");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Less than or Equal to"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT(">=");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Greater than or equal to"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT(">");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Greater than"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("<>");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Unequal to"));


	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("=");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Equal to"));

	m_pcFunctionListCtrl->SetColumnWidth(0, 80);
	m_pcFunctionListCtrl->SetColumnWidth(1, 120);
}

void SP_DLG_ExpressionAssistent::LoadVariable()
{

	if( !m_pcColorSetClass)
		return;
	
	wxListItem l_cItem;
	int l_nPos = 0;
	
	//Load variables
	m_pcVariableListCtrl->InsertColumn(0, wxT("Variable"), wxLIST_FORMAT_LEFT);
	m_pcVariableListCtrl->InsertColumn(1, wxT("Colorset"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);	
	l_cItem.m_mask = wxLIST_MASK_TEXT;

	map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	for( itMap = m_pcColorSetClass->GetVariableMap()->begin(); itMap != m_pcColorSetClass->GetVariableMap()->end(); itMap++)
	{
		l_cItem.m_col = 0;		
		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << itMap->first;
		long l_nIndex = m_pcVariableListCtrl->InsertItem(l_cItem);

		l_cItem.m_text = wxT("");
		l_cItem.m_text << itMap->second.m_ColorSet;
		m_pcVariableListCtrl->SetItem(l_nIndex,1,l_cItem.m_text);

	}
}

void SP_DLG_ExpressionAssistent::LoadData()
{

	if (!m_sReturnText.IsEmpty())
	{
		m_pcFormulaTextCtrl->SetValue(m_sReturnText);
		m_pcFormulaTextCtrl->SetSelection(-1,-1);
	}

	LoadConstant();
    LoadVariable();
	LoadFunctions();

}

void SP_DLG_ExpressionAssistent::OnCheckFormula(wxCommandEvent& p_cEvent)
{	
	wxString l_sExpression = m_pcFormulaTextCtrl->GetValue();
	//Prepare to parse and check arc expression
	SP_CPN_Parse_Context_ptr l_pcParseContext;
	SP_CPN_Driver_ptr  l_pcDriver;

	l_pcParseContext = make_CPN_Parse_Context();
	l_pcDriver = make_CPN_Driver(*l_pcParseContext);

	//Set the ColorSetClass and colorset name for the parsing context
	l_pcParseContext->SetColorSetClass(m_pcColorSetClass);	
	l_pcParseContext->SetColorSetName(m_sColorsetName);
	l_pcParseContext->SetFunctionFlag(false);
	//l_pcParseContext->SetNetClassName( SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() );
	l_pcParseContext->SetPlaceType(m_sPlaceType);
	

	std::string l_ParseString;
	l_ParseString = string( l_sExpression.mb_str());
	bool l_bSuccess = l_pcDriver->parse_string(l_ParseString, "input");		
	if(!l_bSuccess)
	{
		wxString l_sParseError = l_pcDriver->GetError();
		SP_MESSAGEBOX(l_sParseError, wxT("Expression checking"), wxOK | wxICON_ERROR);
		new wxTipWindow(this, wxT("The expresson is not correct."), 1000);
		return;
	}
	
	// call the check function
	if( ! l_pcParseContext->CheckExpression())
		return;

	if(m_sColorsetName == wxT(""))     //check guard as boolean value
	{		
		if( l_sExpression != wxT("") && l_pcParseContext->GetHighLevelNodeDataType() != CPN_BOOLEAN )
		{	
			SP_MESSAGEBOX(wxT("Guard should be a boolean expression"), wxT("Gurard checking"), wxOK | wxICON_ERROR);
			new wxTipWindow(this, wxT("The expresson is not correct."), 1000);
			return;
		}
	}

	new wxTipWindow(this, wxT("The expresson is correct."), 1000);

}

void SP_DLG_ExpressionAssistent::InsertIntoFormulaText(const wxString& p_sInsertText)
{
	long l_nInsertFrom = 0;
	long l_nInsertTo = 0;

	m_pcFormulaTextCtrl->GetSelection(&l_nInsertFrom, &l_nInsertTo);

	m_pcFormulaTextCtrl->Replace(l_nInsertFrom, l_nInsertTo, p_sInsertText);
}

void SP_DLG_ExpressionAssistent_ListCtrl::OnDblClick(wxListEvent& event)
{

	m_pcParentDlg->InsertIntoFormulaText(GetItemText(event.m_itemIndex));

	

}

