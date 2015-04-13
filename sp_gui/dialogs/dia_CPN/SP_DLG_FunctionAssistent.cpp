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

#include "sp_gui/dialogs/dia_CPN/SP_DLG_FunctionAssistent.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

IMPLEMENT_CLASS( SP_DLG_FunctionAssistent, wxDialog )


BEGIN_EVENT_TABLE( SP_DLG_FunctionAssistent, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_FunctionAssistent::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_FunctionAssistent::OnDlgCancel )

END_EVENT_TABLE()


SP_DLG_FunctionAssistent::SP_DLG_FunctionAssistent(vector<wxString> p_vParameter, wxString p_psColorsetName, SP_CPN_ColorSetClass& p_pcColorSetClass,
		wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText,
		const wxString& p_sTitle, long p_nStyle)
:SP_DLG_ExpressionAssistent( EXPRESSION_ARCGUARD, p_psColorsetName,wxT("Place"), p_pcColorSetClass,p_psReturnText,p_pcParent)
{
	m_vParameter.clear();
	m_vParameter = p_vParameter;

	LoadData();
}

//------------------------------------------------------------------------
void SP_DLG_FunctionAssistent::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_FunctionAssistent::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_FunctionAssistent::LoadFunctions()
{	
	m_pcFunctionListCtrl->ClearAll();

	wxListItem l_cItem;
	int l_nPos = 0;
	long l_nIndex = 0;

	m_pcFunctionListCtrl->InsertColumn(0, wxT("Operators"), wxLIST_FORMAT_LEFT);
	m_pcFunctionListCtrl->InsertColumn(1, wxT("Description"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);
	l_cItem.m_col = 0;
	l_cItem.m_mask = wxLIST_MASK_TEXT; 

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("abs(.)");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Return the absolute value"));

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("++");
	l_nIndex = m_pcFunctionListCtrl->InsertItem(l_cItem);
	m_pcFunctionListCtrl->SetItem(l_nIndex,1,wxT("Multiset addition"));

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


	m_pcFunctionListCtrl->SetColumnWidth(0, 50);
	m_pcFunctionListCtrl->SetColumnWidth(1, 120);
}

void SP_DLG_FunctionAssistent::LoadVariable()
{
	m_pcVariableListCtrl->ClearAll();

	wxListItem l_cItem;
	int l_nPos = 0;
	
	//Load variables
	m_pcVariableListCtrl->InsertColumn(0, wxT("Variable"), wxLIST_FORMAT_LEFT);
	m_pcVariableListCtrl->InsertColumn(1, wxT("Colorset"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);	
	l_cItem.m_mask = wxLIST_MASK_TEXT;

	if( m_vParameter.size()%2 != 0)
		return;
	
	for(unsigned i = 0; i < m_vParameter.size(); i++)
	{
		l_cItem.m_col = 0;		
		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << m_vParameter[i+1];
		long l_nIndex = m_pcVariableListCtrl->InsertItem(l_cItem);

		l_cItem.m_text = wxT("");
		l_cItem.m_text << m_vParameter[i];
		m_pcVariableListCtrl->SetItem(l_nIndex,1,l_cItem.m_text);
		i++;
	}
}



void SP_DLG_FunctionAssistent::OnCheckFormula(wxCommandEvent& p_cEvent)
{	
	SP_MESSAGEBOX(wxT("Please return to the function edit dialogue to check."), wxT("Function expression checking"), wxOK | wxICON_ERROR);
}



