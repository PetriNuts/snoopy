//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/22 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////#

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

IMPLEMENT_CLASS( SP_DLG_StFunctionAssistent, wxDialog )

enum
{

	SP_ID_TEXTCTRL_FORMULA,
	SP_ID_BUTTON_CHECK,
	SP_ID_LISTCTRL_PLACE,
	SP_ID_LISTCTRL_PARAMETER,
	SP_ID_LISTCTRL_FUNCTION

};

BEGIN_EVENT_TABLE( SP_DLG_StFunctionAssistent, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_StFunctionAssistent::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_StFunctionAssistent::OnDlgCancel )

EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_StFunctionAssistent::OnCheckFormula )

END_EVENT_TABLE()

BEGIN_EVENT_TABLE( SP_DLG_StFunctionAssistent_ListCtrl, wxListCtrl )

EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_PLACE, SP_DLG_StFunctionAssistent_ListCtrl::OnDblClick )
EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_PARAMETER, SP_DLG_StFunctionAssistent_ListCtrl::OnDblClick )
EVT_LIST_ITEM_ACTIVATED( SP_ID_LISTCTRL_FUNCTION, SP_DLG_StFunctionAssistent_ListCtrl::OnDblClick )

END_EVENT_TABLE()

SP_DLG_StFunctionAssistent::SP_DLG_StFunctionAssistent(
		SP_StParserType p_nAssistentType,SP_DS_Node* p_pcParentNode,
		wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText,
		const wxString& p_sTitle, long p_nStyle)

:
	wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize(300, 500),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX )

{
	m_nAssistentType = p_nAssistentType;
	wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(p_pcParentNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	SetTitle(wxT("Function assistant (")+l_sName+wxT(")"));
	m_pcParentNode = p_pcParentNode;
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	wxString l_sTypeLabel = wxT("Function");
	wxString l_sPlaceLabel = wxT("Pre-places");

	m_sReturnText = p_psReturnText;

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	//formula
	wxBoxSizer* l_pcFormulaSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcFormulaSizer->Add(new wxStaticText(this, -1, l_sTypeLabel), 0, wxALL |  wxEXPAND, 5);
	m_pcFormulaTextCtrl = new wxTextCtrl(this, SP_ID_TEXTCTRL_FORMULA, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	l_pcFormulaSizer->Add(m_pcFormulaTextCtrl, 1, wxALL | wxEXPAND, 5);
	l_pcFormulaSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check")), 0, wxALL | wxEXPAND, 5);
	m_pcSizer->Add(l_pcFormulaSizer, 1, wxALL | wxEXPAND, 1);

	//listboxes
	wxBoxSizer* l_pcListBoxesSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* l_pcPlaceListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, l_sPlaceLabel), wxHORIZONTAL);
	m_pcPlaceListCtrl = new SP_DLG_StFunctionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_PLACE, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcPlaceListBoxSizer->Add(m_pcPlaceListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcPlaceListBoxSizer, 1, wxALL| wxEXPAND, 2);

	wxStaticBoxSizer* l_pcParameterListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Parameter")), wxHORIZONTAL);
	m_pcParameterListCtrl = new SP_DLG_StFunctionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_PARAMETER, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcParameterListBoxSizer->Add(m_pcParameterListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcParameterListBoxSizer, 1, wxALL| wxEXPAND, 2);

	wxStaticBoxSizer* l_pcFunctionListBoxSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Functions / Lookup-Tables")), wxHORIZONTAL);
	m_pcFunctionListCtrl = new SP_DLG_StFunctionAssistent_ListCtrl(this, this, SP_ID_LISTCTRL_FUNCTION, wxDefaultPosition,
								wxSize(200, 200), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES /*| wxLC_SORT_ASCENDING*/);
	l_pcFunctionListBoxSizer->Add(m_pcFunctionListCtrl, 1, wxALL| wxEXPAND, 2);
	l_pcListBoxesSizer->Add(l_pcFunctionListBoxSizer, 1, wxALL| wxEXPAND, 2);
	m_pcSizer->Add(l_pcListBoxesSizer, 2, wxALL | wxEXPAND, 1);

	//buttons
	m_pcSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);

	LoadData();

	SetSizerAndFit(m_pcSizer);

}

//------------------------------------------------------------------------
void SP_DLG_StFunctionAssistent::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_StFunctionAssistent::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_StFunctionAssistent::LoadPrePlaces()
{

	wxListItem l_cItem;
	int l_nPos = 0;

	SP_DS_Node* l_pcNode = m_pcParentNode;

	if (l_pcNode == NULL)
	{
		return;
	}

	SP_ListEdge::const_iterator l_itEdge;

	m_pcPlaceListCtrl->InsertColumn(0, wxT("Pre-places"), wxLIST_FORMAT_LEFT);

	for(l_itEdge = l_pcNode->GetTargetEdges()->begin();
		l_itEdge != l_pcNode->GetTargetEdges()->end(); ++l_itEdge)
	{
		SP_DS_Node* l_pcSourceNode = dynamic_cast<SP_DS_Node*> ((*l_itEdge)->GetSource());

		if (l_pcSourceNode == NULL)
		{
			break;
		}

		l_cItem.SetBackgroundColour(*wxWHITE);

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_col = 0;
		l_cItem.m_mask = wxLIST_MASK_TEXT;

		l_cItem.m_text = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		if((*l_itEdge)->GetClassName() == wxT("Modifier Edge"))
		{
			wxFont l_cFont = GetFont();
			l_cFont.SetStyle(wxFONTSTYLE_ITALIC);
			l_cItem.SetFont(l_cFont);
		}
		m_pcPlaceListCtrl->InsertItem(l_cItem);
	}
	m_pcPlaceListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	if(m_pcPlaceListCtrl->GetColumnWidth(0) < 180)
		m_pcPlaceListCtrl->SetColumnWidth(0, 180);

}

void SP_DLG_StFunctionAssistent::LoadParameter()
{

	wxListItem l_cItem;
	int l_nPos = 0;

	wxString l_sParameterName;
	wxString l_sParamType;

	//Colored SPN
	if( m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")) )
	{
		SP_DS_Nodeclass* l_pcNodeclass;

		l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_PARAM);

		SP_ListNode::const_iterator l_itElem;

		m_pcParameterListCtrl->InsertColumn(0, wxT("Parameter"), wxLIST_FORMAT_LEFT);

		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem
				!= l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{

			l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			l_cItem.SetBackgroundColour(*wxWHITE);

			l_cItem.m_itemId = l_nPos++;
			l_cItem.m_col = 0;
			l_cItem.m_mask = wxLIST_MASK_TEXT;
			l_cItem.m_text = l_sParameterName;

			m_pcParameterListCtrl->InsertItem(l_cItem);

		}
	}
	else
	{
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		SP_ListMetadata::const_iterator it;

		m_pcParameterListCtrl->InsertColumn(0, wxT("Constants"), wxLIST_FORMAT_LEFT);

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcConstant = *it;
			wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			//l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			l_cItem.SetBackgroundColour(*wxWHITE);

			l_cItem.m_itemId = l_nPos++;
			l_cItem.m_col = 0;
			l_cItem.m_mask = wxLIST_MASK_TEXT;
			l_cItem.m_text = l_sParameterName;

			m_pcParameterListCtrl->InsertItem(l_cItem);
		}
	}

	m_pcParameterListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	if(m_pcParameterListCtrl->GetColumnWidth(0) < 180)
	{
		m_pcParameterListCtrl->SetColumnWidth(0, 180);
	}
}

void SP_DLG_StFunctionAssistent::LoadFunctions()
{

	SP_DS_StParser l_pcParser;

	InfixOperator* l_pcInfixOperator = l_pcParser.GetInfixOperator();
	PrefixOperator* l_pcPrefixOperator = l_pcParser.GetPrefixOperator();

	BinaryOperator* l_pcBinaryOperator = l_pcParser.GetBinaryOperator();
	Function3ParamOperator* l_pcFunction3ParamOperator =
			l_pcParser.GetFunction3ParamOperator();
	Function2ParamOperator* l_pcFunction2ParamOperator =
				l_pcParser.GetFunction2ParamOperator();
	Function0ParamOperator* l_pcFunction0ParamOperator =
			l_pcParser.GetFunction0ParamOperator();

	wxListItem l_cItem;
	int l_nPos = 0;

	m_pcFunctionListCtrl->InsertColumn(0, wxT("Function"), wxLIST_FORMAT_LEFT);

	l_cItem.SetBackgroundColour(*wxWHITE);
	l_cItem.m_col = 0;
	l_cItem.m_mask = wxLIST_MASK_TEXT;

	//Colored SPN
	if( m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")) )
	{
		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("#(_PrePlaces)");
		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("Move2DGrid(_X,_Y,_GridMin,_GridMax,_Param)");
	    m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << wxT("CountXY(_X,_Y,_X0,_Xn,_Y0,_Yn)");
	    m_pcFunctionListCtrl->InsertItem(l_cItem);
	}
	
    l_cItem.m_itemId = l_nPos++;
    l_cItem.m_text = wxT("");
    l_cItem.m_text << wxT("MassAction( . )");
   	m_pcFunctionListCtrl->InsertItem(l_cItem);
/*
	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("LevelInterpretation( . , . )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("Summation( _PrePlaces )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("Product( _PrePlaces )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("Multiplicity( . )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);
*/
	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("MichaelisMenten( , )");
    m_pcFunctionListCtrl->InsertItem(l_cItem);

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("linlog( . )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);

	l_cItem.m_itemId = l_nPos++;
	l_cItem.m_text = wxT("");
	l_cItem.m_text << wxT("nlinlog( . )");
	m_pcFunctionListCtrl->InsertItem(l_cItem);

	while (l_pcFunction0ParamOperator->ptr)
	{

		if (l_pcFunction0ParamOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD ||
			l_pcFunction0ParamOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE)
		{
			l_pcFunction0ParamOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcFunction0ParamOperator->string << wxT("()");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcFunction0ParamOperator++;

	}

	while (l_pcFunction2ParamOperator->ptr)
	{

		if (l_pcFunction2ParamOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD ||
			l_pcFunction2ParamOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE)
		{
			l_pcFunction2ParamOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcFunction2ParamOperator->string << wxT("( . , . )");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcFunction2ParamOperator++;

	}

	while (l_pcFunction3ParamOperator->ptr)
	{

		if (l_pcFunction3ParamOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD ||
			l_pcFunction3ParamOperator->visibleGroup 
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE)
		{
			l_pcFunction3ParamOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcFunction3ParamOperator->string << wxT("( . , . , . )");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcFunction3ParamOperator++;

	}

	while (l_pcInfixOperator->ptr)
	{

		if (l_pcInfixOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD)
		{
			l_pcInfixOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcInfixOperator->chr << wxT("");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcInfixOperator++;

	}

	while (l_pcPrefixOperator->ptr)
	{

		if (l_pcPrefixOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC
				|| l_pcPrefixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN
				|| l_pcPrefixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE
				|| l_pcPrefixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD)
		{
			l_pcPrefixOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcPrefixOperator->string << wxT("( . )");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcPrefixOperator++;

	}

	while (l_pcBinaryOperator->ptr)
	{

		if (l_pcInfixOperator->visibleGroup
				== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE
				|| l_pcInfixOperator->visibleGroup
						== SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD)
		{
			l_pcBinaryOperator++;
			continue;
		}

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_text = wxT("");
		l_cItem.m_text << l_pcBinaryOperator->string << wxT("( . )");

		m_pcFunctionListCtrl->InsertItem(l_cItem);

		l_pcBinaryOperator++;

	}
	m_pcFunctionListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void SP_DLG_StFunctionAssistent::LoadLookupTables()
{

	wxListItem l_cItem;
	int l_nPos = m_pcParameterListCtrl->GetItemCount();

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sLookupTableName;

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = l_pcGraph->GetNodeclass(wxT("LookupTable"));
	//if this class does not support lookup table
	    if(!l_pcNodeclass)
	    	return;
	SP_ListNode::const_iterator l_itElem;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem
			!= l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{

		l_sLookupTableName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_sLookupTableName += wxT("( . )");

		l_cItem.SetBackgroundColour(*wxWHITE);

		l_cItem.m_itemId = l_nPos++;
		l_cItem.m_col = 0;
		l_cItem.m_mask = wxLIST_MASK_TEXT;
		l_cItem.m_text = l_sLookupTableName;

		m_pcParameterListCtrl->InsertItem(l_cItem);

	}
	m_pcParameterListCtrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
	if(m_pcParameterListCtrl->GetColumnWidth(0) < 180)
		m_pcParameterListCtrl->SetColumnWidth(0, 180);

}

void SP_DLG_StFunctionAssistent::LoadData()
{

	if (!m_sReturnText.IsEmpty())
	{
		m_pcFormulaTextCtrl->SetValue(m_sReturnText);
		m_pcFormulaTextCtrl->SetSelection(-1,-1);
	}

	LoadPrePlaces();
	LoadParameter();
	LoadFunctions();
	LoadLookupTables();

}

void SP_DLG_StFunctionAssistent::OnCheckFormula(wxCommandEvent& p_cEvent)
{

	SP_DS_StParser l_cParser;

	wxArrayString* l_plMissingVariableList = 0;

	
	wxArrayString* l_psUnusedModifierPlaceList = 0;
	wxArrayString* l_psUnusedPrePlaceList = 0;

	wxString l_sVariables = wxT("");
	wxString l_sFormula = m_pcFormulaTextCtrl->GetValue();

	wxString l_sCheckFormula = m_pcFormulaTextCtrl->GetValue();

	bool l_bFormulaCheck = false;

	//Colored SPN
	SP_DS_Graph* m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS )
	{		
		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.Initialize())
			return;
		l_bFormulaCheck = l_cSyntaxChecking.CheckFormula(m_pcParentNode,l_sCheckFormula,&l_cParser,true);

	}
	else
	{
		l_bFormulaCheck = l_cParser.CheckFormulaFunction(l_sCheckFormula, m_pcParentNode);

	}

	
	if (!l_bFormulaCheck )
	{

		l_plMissingVariableList = l_cParser.GetMissingVariableList();

		l_sVariables = wxT("The formula \"");
		l_sVariables += l_sFormula;
		l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

		for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
		{

			l_sVariables += (*l_plMissingVariableList)[i];
			l_sVariables += wxT("\n");
		}

		SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR );

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

void SP_DLG_StFunctionAssistent::InsertIntoFormulaText(const wxString& p_sInsertText)
{
	long l_nInsertFrom = 0;
	long l_nInsertTo = 0;

	m_pcFormulaTextCtrl->GetSelection(&l_nInsertFrom, &l_nInsertTo);

	m_pcFormulaTextCtrl->Replace(l_nInsertFrom, l_nInsertTo, p_sInsertText);
}

void SP_DLG_StFunctionAssistent_ListCtrl::OnDblClick(wxListEvent& event)
{

	m_pcParentDlg->InsertIntoFormulaText(GetItemText(event.m_itemIndex));

}

