/*
 * SP_DLG_NewObserverDefinition.cpp
 *
 *  Created on: 07.05.2017
 *      Author: Patrick Henschel
 */

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/SP_DLG_NewObserverDefinition.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_GridCellChoiceEditor.h"

#include "sp_core/tools/CyclicGraph.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Attribute.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h>
#include <wx/textdlg.h>


//by george
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_core/tools/SP_NetUnfolder.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionAssistent.h"
#include "sp_utilities.h"

IMPLEMENT_CLASS( SP_DLG_NewObserverDefinition, wxDialog)

enum
{
	SHOW = 0,
	NAME,
	TYPE,
	COMMENT,
	BODY
};

enum
{
	SP_ID_BUTTON_ADD,
	SP_ID_BUTTON_DELETE,
	SP_ID_BUTTON_CHECK,
	SP_ID_GRID_MARKING,
	SP_ID_BUTTON_OBSERVER_ASSISTANT

};
BEGIN_EVENT_TABLE( SP_DLG_NewObserverDefinition, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_NewObserverDefinition::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_NewObserverDefinition::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_ADD, SP_DLG_NewObserverDefinition::OnAddSet )
EVT_BUTTON( SP_ID_BUTTON_CHECK, SP_DLG_NewObserverDefinition::OnCheckFunction )
EVT_BUTTON( wxID_APPLY, SP_DLG_NewObserverDefinition::OnDlgApply )
EVT_BUTTON( SP_ID_BUTTON_DELETE, SP_DLG_NewObserverDefinition::OnDeleteSet )
EVT_BUTTON(SP_ID_BUTTON_OBSERVER_ASSISTANT, SP_DLG_NewObserverDefinition::OnAssistant)

#if wxABI_VERSION > 30000
    EVT_GRID_CELL_CHANGED(SP_DLG_NewObserverDefinition::OnGridCellValueChanged )
#else
    EVT_GRID_CELL_CHANGE(SP_DLG_NewObserverDefinition::OnGridCellValueChanged )
#endif

EVT_GRID_SELECT_CELL(SP_DLG_NewObserverDefinition::OnGridCellSelected )
EVT_GRID_EDITOR_SHOWN(SP_DLG_NewObserverDefinition::OnEditorShown )

END_EVENT_TABLE()

SP_DLG_NewObserverDefinition::SP_DLG_NewObserverDefinition(wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :
		wxDialog(p_pcParent, -1, p_sTitle, wxPoint(120, 120), wxSize(1500, 400), p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)		
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_pcObservers = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	m_pcParent = p_pcParent;

	m_pcSizer = new wxBoxSizer(wxVERTICAL);

	// set / name components
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcGridSizer = new wxBoxSizer(wxVERTICAL);

	// new row
	m_pcObserverSetGrid = new wxGrid(this, SP_ID_GRID_MARKING, wxDefaultPosition, wxSize(700, 300), wxSUNKEN_BORDER);
	m_pcObserverSetGrid->CreateGrid(0, 0);
	m_pcObserverSetGrid->EnableEditing(true);

	m_pcObserverSetGrid->SetDefaultColSize(70, TRUE);
	m_pcObserverSetGrid->SetDefaultRowSize(20, TRUE);

	m_pcObserverSetGrid->SetColLabelSize(16);
	m_pcObserverSetGrid->SetRowLabelSize(0); //do not let it show.

	l_pcRowSizer->Add(m_pcObserverSetGrid, 1, wxALL | wxEXPAND, 2);
	l_pcGridSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	m_pcObserverSetGrid->EnableEditing(true);
	m_pcObserverSetGrid->SetSelectionMode(wxGrid::wxGridSelectCells);
	m_mObserve2Type.clear();

	l_bWhite = false;
	LoadSetNames();
	LoadData();
	LoadNodes();


	SP_AutoSizeRowLabelSize(m_pcObserverSetGrid);

	wxBoxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSizer *l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_ADD, wxT("Add")), 1, wxALL , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_DELETE, wxT("Delete")), 1, wxALL , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_CHECK, wxT("Check")), 1, wxALL , 5);
	l_pcSizer->Add(new wxButton(this, SP_ID_BUTTON_OBSERVER_ASSISTANT, wxT("Observer assistant")), 1, wxALL, 5);

	wxSizer* l_pcBottomButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);

	l_pcButtonSizer->Add(l_pcSizer, 0, wxEXPAND);
	l_pcButtonSizer->Add(l_pcBottomButtonSizer, 0, wxEXPAND);

	m_pcSizer->Add(l_pcGridSizer, 1, wxEXPAND);
	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);

	SetAutoLayout(TRUE);
	SetSizer(m_pcSizer);

	m_pcSizer->Fit(this);
	m_pcSizer->SetSizeHints(this);

	Layout();

}

void SP_DLG_NewObserverDefinition::OnDlgApply(wxCommandEvent& p_cEvent)
{
	//if there are some errors, we do not apply changes
	if (!CheckInput())
	{
		return;
	}
	SaveData();
}

bool SP_DLG_NewObserverDefinition::CheckInput()
{
	//check user input values for parameters
	for (int l_nRow = 0; l_nRow < m_pcObserverSetGrid->GetNumberRows(); l_nRow++)
	{
		wxString l_sBody = m_pcObserverSetGrid->GetCellValue(l_nRow, BODY);
		wxString l_sName = m_pcObserverSetGrid->GetCellValue(l_nRow, NAME);
		wxString l_sType = m_pcObserverSetGrid->GetCellValue(l_nRow, TYPE);

		if (l_sType == _T("Place instance") || l_sType == _T("Transition instance")|| l_sType == _T("Mixed"))
		{
			if (m_vUnfoldedPlacenames.size() == 0)
			{
				UnfoldNet();
			}
		}

		if (!DoCheckFunction(l_sName, l_sType, l_sBody,false)) {
			return false;
		}
	}

	return true; //no problems found
}

void SP_DLG_NewObserverDefinition::OnDlgOk(wxCommandEvent& p_cEvent)
{

	//if there are some errors, we do not close the dialog
	if (!CheckInput())
	{
		return;
	}

	if (Validate() && TransferDataFromWindow())
	{
		if (IsModal())
		{
			if (SaveData())
			{
				wxBusyInfo wait(wxT("Please wait, updating marking..."), this);
				//by sl

				/*
				 * todo update for other netclasses to show in declaration tree
				 */
				//by george
				if (m_pcGraph->GetNetclass()->GetName().Contains(_T("Colored")))
				{
					m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
					SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					EndModal(wxID_OK);
					return;
				}

				m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

				SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
				EndModal(wxID_OK);
			}
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void SP_DLG_NewObserverDefinition::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	m_deleted.clear();

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

void SP_DLG_NewObserverDefinition::OnDeleteSet(wxCommandEvent& p_cEvent)
{
	if (m_pcObserverSetGrid->GetNumberRows() == 0)
	{
		return;
	}
    if ( m_pcObserverSetGrid->IsSelection() )
    {
        wxGridUpdateLocker locker(m_pcObserverSetGrid);
        for ( int n = 0; n < m_pcObserverSetGrid->GetNumberRows(); )
        {
            if ( m_pcObserverSetGrid->IsInSelection( n , 0 ) )
            {
        		wxString l_sName = m_pcObserverSetGrid->GetCellValue(n, NAME);
        		for (SP_DS_Metadata* l_pcMeta : *(m_pcObservers->GetElements()))
        		{
        			if(l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString()
        				== l_sName)
        			{
        				m_deleted.push_back(l_pcMeta);
        			}
        		}
            	m_pcObserverSetGrid->DeleteRows( n, 1 );
            }
            else
            {
            	++n;
            }
        }
    }
}

void SP_DLG_NewObserverDefinition::OnAddSet(wxCommandEvent& p_cEvent)
{
	m_pcObserverSetGrid->AppendRows(1);

	int l_nEditRowPos = m_pcObserverSetGrid->GetNumberRows() - 1;

	auto l_pcBoolEditor = new wxGridCellBoolEditor();
	l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
	m_pcObserverSetGrid->SetCellEditor(l_nEditRowPos, SHOW, l_pcBoolEditor);
	m_pcObserverSetGrid->SetCellRenderer(l_nEditRowPos, SHOW, new wxGridCellBoolRenderer());
	m_pcObserverSetGrid->SetCellSize(l_nEditRowPos, SHOW, 1, 1);
	m_pcObserverSetGrid->SetCellAlignment(l_nEditRowPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
	m_pcObserverSetGrid->SetCellValue(l_nEditRowPos, SHOW, wxT("0"));
	m_pcObserverSetGrid->SetCellOverflow(l_nEditRowPos, SHOW, false);
	m_pcObserverSetGrid->SetCellBackgroundColour(l_nEditRowPos, SHOW, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	m_pcObserverSetGrid->SetCellBackgroundColour(l_nEditRowPos, NAME, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcObserverSetGrid->SetCellBackgroundColour(l_nEditRowPos, BODY, (l_bWhite ? *wxWHITE : *wxLIGHT_GREY));

	m_pcObserverSetGrid->SetCellEditor(l_nEditRowPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
	m_pcObserverSetGrid->SetCellSize(l_nEditRowPos, TYPE, 1, 1);
	m_pcObserverSetGrid->SetCellValue(l_nEditRowPos, TYPE, wxT("Place"));
	m_pcObserverSetGrid->SetCellOverflow(l_nEditRowPos, TYPE, false);
	m_pcObserverSetGrid->SetCellBackgroundColour(l_nEditRowPos, TYPE, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));

	m_pcObserverSetGrid->SetCellBackgroundColour(l_nEditRowPos, COMMENT, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
}

bool SP_DLG_NewObserverDefinition::LoadData()
{
	InitializeDataTypes();

	unsigned int l_nPos = 0;
	//bool l_bWhite = false;
	l_bWhite = false;
	for (SP_DS_Metadata* l_pcMetadata : *(m_pcObservers->GetElements()))
	{
		wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
		wxString l_sMetadataComment = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Comment")))->GetValue();
		wxString l_sMetadataBody = dynamic_cast<SP_DS_EquationAttribute*>(l_pcMetadata->GetAttribute(wxT("Body")))->GetValue();
		wxString l_sMetadataShow = l_pcMetadata->GetShow() ? wxT("1") : wxT("0");


		if (m_pcGraph->GetNetclass()->GetDisplayName().Contains(wxT("Colored")))//george
			{
				wxString l_sColType = l_pcMetadata->GetAttribute(wxT("ColPNType"))->GetValueString();

				if (l_sColType == wxT("Place instance")  && l_sMetadataType == wxT("Place"))
				{
					l_sMetadataType = wxT("Place instance");
				}
				else if (l_sColType == wxT("Transition instance") && l_sMetadataType == wxT("Transition"))
				{
					l_sMetadataType = wxT("Transition instance");
				}
				else if (l_sColType == _T("Mixed"))
				{
					l_sMetadataType = wxT("Mixed");
				}
			}

			m_mObserve2Type[l_sMetadataName] = l_sMetadataType;//in order to get pre-defined observers


		m_pcObserverSetGrid->AppendRows(1);
		//show the observer
		auto l_pcBoolEditor = new wxGridCellBoolEditor();
		l_pcBoolEditor->UseStringValues(wxT("1"), wxT("0"));
		m_pcObserverSetGrid->SetCellEditor(l_nPos, SHOW, l_pcBoolEditor);
		m_pcObserverSetGrid->SetCellRenderer(l_nPos, SHOW, new wxGridCellBoolRenderer());
		m_pcObserverSetGrid->SetCellValue(l_nPos, SHOW, l_sMetadataShow);
		m_pcObserverSetGrid->SetCellAlignment(l_nPos, SHOW, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcObserverSetGrid->SetCellBackgroundColour(l_nPos, SHOW, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//name of the observer
		m_pcObserverSetGrid->SetCellValue(l_nPos, NAME, l_sMetadataName);
		m_pcObserverSetGrid->SetCellAlignment(l_nPos, NAME, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcObserverSetGrid->SetCellBackgroundColour(l_nPos, NAME, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//type of the observer
		m_pcObserverSetGrid->SetCellEditor(l_nPos, TYPE, new wxGridCellChoiceEditor(m_datatypes.GetCount(), datatypes));
		m_pcObserverSetGrid->SetCellValue(l_nPos, TYPE, l_sMetadataType);
		m_pcObserverSetGrid->SetCellAlignment(l_nPos, TYPE, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcObserverSetGrid->SetCellBackgroundColour(l_nPos, TYPE, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		//comment of the observer
		m_pcObserverSetGrid->SetCellValue(l_nPos, COMMENT, l_sMetadataComment);
		m_pcObserverSetGrid->SetCellAlignment(l_nPos, COMMENT, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcObserverSetGrid->SetCellBackgroundColour(l_nPos, COMMENT, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		// body of the observer
		m_pcObserverSetGrid->SetCellValue(l_nPos, BODY, l_sMetadataBody);
		m_pcObserverSetGrid->SetCellAlignment(l_nPos, BODY, wxALIGN_CENTER, wxALIGN_TOP);
		m_pcObserverSetGrid->SetCellBackgroundColour(l_nPos, BODY, (l_bWhite ? *wxWHITE: *wxLIGHT_GREY));
		
		l_nPos++;
		(l_bWhite ? l_bWhite = false : l_bWhite = true);
	}

	return true;

}

bool SP_DLG_NewObserverDefinition::SaveData()
{
	m_pcObserverSetGrid->SaveEditControlValue();

	SP_DS_Metadata* l_pcObserver;

	// delete observer
	for(auto l_pcObsv : m_deleted)
	{
		m_pcObservers->RemoveElement(l_pcObsv);
		wxDELETE(l_pcObsv);
	}

	while (m_pcObservers->GetElements()->size() < m_pcObserverSetGrid->GetNumberRows())
	{
		m_pcObservers->NewElement(1);
	}

	SP_ListMetadata::const_iterator l_itElem;
	l_itElem = m_pcObservers->GetElements()->begin();

	for (int l_nRow = 0; l_nRow < m_pcObserverSetGrid->GetNumberRows(); l_nRow++)
	{
		l_pcObserver = *l_itElem;

		wxString l_sName = m_pcObserverSetGrid->GetCellValue(l_nRow, NAME);
		wxString l_sBody = m_pcObserverSetGrid->GetCellValue(l_nRow, BODY);
		wxString l_sType = m_pcObserverSetGrid->GetCellValue(l_nRow, TYPE);
		wxString l_sComment = m_pcObserverSetGrid->GetCellValue(l_nRow, COMMENT);

		//colored nets case
		if (l_sType == _T("Place instance")|| l_sType == _T("Mixed"))
		{
			l_sType = _T("Place");
		}
		if (l_sType == _T("Transition instance"))
	    {
			l_sType = _T("Transition");
		}

		l_pcObserver->GetAttribute(wxT("Name"))->SetValueString(l_sName);
		l_pcObserver->GetAttribute(wxT("Body"))->SetValueString(l_sBody);
		l_pcObserver->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

		bool l_bValue = l_pcObserver->GetAttribute(wxT("Type"))->SetValueString(l_sType);

		auto l_it = m_mObserve2Type.find(l_sName);

		if (l_it != m_mObserve2Type.end())
		{
			//SP_MESSAGEBOX(wxT("before"));
			wxString l_sObserverTupe;
			l_sObserverTupe<<m_mObserve2Type[l_sName];


			SP_DS_Attribute* l_pcAttrib=l_pcObserver->GetAttribute(wxT("ColPNType"));//->SetValueString(l_sObserverTupe);

			if(l_pcAttrib)

			{
				l_pcAttrib->SetValueString(l_sObserverTupe);
			}
			else
			{

			break;
			}
		}

		if (!l_bValue)
		{
			SP_MESSAGEBOX(wxT("datatype '") + l_sType + wxT("' for observer '") + l_sName + wxT("' is not allowed (use one of '") + m_sAvailableDatatypes + wxT("')"), wxT("Error"),
					wxOK | wxICON_ERROR);
			return false;
		}

		bool l_bShow = m_pcObserverSetGrid->GetCellValue(l_nRow, SHOW) == wxT("1") ? true : false;
		l_pcObserver->SetShow(l_bShow);
		for(SP_DS_Attribute* l_pcAttr : *l_pcObserver->GetAttributes())
		{
			l_pcAttr->SetShow(l_bShow);
		}
		l_pcObserver->Update();

		++l_itElem;
	}

	m_pcGraph->Update();

	return true;

}

void SP_DLG_NewObserverDefinition::OnCheckFunction(wxCommandEvent& p_cEvent)
{
	int row = m_pcObserverSetGrid->GetGridCursorRow();

	wxString l_sBody = m_pcObserverSetGrid->GetCellValue(row, BODY);
	wxString l_sName = m_pcObserverSetGrid->GetCellValue(row, NAME);
	wxString l_sType = m_pcObserverSetGrid->GetCellValue(row, TYPE);

	DoCheckFunction(l_sName, l_sType, l_sBody);
}


bool SP_DLG_NewObserverDefinition::DoCheckFunction(const wxString& p_sName, const wxString& p_sType, const wxString& p_sBody,const bool& b_showTip)
{

	if (p_sName.IsEmpty())
		{
			SP_MESSAGEBOX(wxT("the observer name is empty"), wxT("Check Observer"), wxOK | wxICON_ERROR);
			return false;
		}
		if (p_sBody.IsEmpty())
		{
			SP_MESSAGEBOX(wxT("the observer ") + p_sName + wxT(" is empty "), wxT("Check Observer"), wxOK | wxICON_ERROR);
			return false;
		}
		if (SP_Find(SP_KEYWORDLIST, p_sName)!= SP_KEYWORDLIST.end())
		{
			SP_MESSAGEBOX(wxT("the observer ") + p_sName + wxT(" can not be a keyword "), wxT("Check Observer"), wxOK | wxICON_ERROR);
			return false;
		}
		SP_DS_FunctionRegistry* l_pcFRname = m_pcGraph->GetFunctionRegistry();
		SP_FunctionPtr l_pcFunctionName(l_pcFRname->parseFunctionString(p_sName));

		if (!l_pcFunctionName || !l_pcFunctionName->isVariable())
		{//check that the observer name is not an existing variable (e.g., place or trans name)
			SP_MESSAGEBOX(wxT("the name of observer ") + p_sName + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
			return false;
		}

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sBody));
		if (!l_pcFunction)
		{
			SP_MESSAGEBOX(wxT("the observer ") + p_sName + wxT(" with body ") + p_sBody + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
			return false;
		}

		// check the function in the body
		SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

		std::set<std::string> l_Vars;
		l_pcExpanded->getVariables(l_Vars);


		if (!l_Vars.empty())
		{
			std::set<std::string> l_Diff;

			if (p_sType == wxT("Place"))
			{
				std::set_difference(l_Vars.begin(), l_Vars.end(), m_Places.begin(), m_Places.end(), std::inserter(l_Diff, l_Diff.begin()));
			}
			else if(p_sType == wxT("Transition")) {
				std::set_difference(l_Vars.begin(), l_Vars.end(), m_Transitions.begin(), m_Transitions.end(), std::inserter(l_Diff, l_Diff.begin()));
			}
			else if (p_sType == wxT("Place instance"))
			{
				if (m_vUnfoldedPlacenames.size() == 0)
				{
					UnfoldNet();
				}

				m_mObserve2Type[p_sName] = p_sType;

				std::set_difference(l_Vars.begin(), l_Vars.end(), m_vUnfoldedPlacenames.begin(), m_vUnfoldedPlacenames.end(), std::inserter(l_Diff, l_Diff.begin()));

		   }
			else if (p_sType == wxT("Transition instance"))
			{
				if (m_vUnfoldedTransnames.size() == 0)
				{
					UnfoldNet();
				}

				m_mObserve2Type[p_sName] = p_sType;

				std::set_difference(l_Vars.begin(), l_Vars.end(), m_vUnfoldedTransnames.begin(), m_vUnfoldedTransnames.end(), std::inserter(l_Diff, l_Diff.begin()));
			}
			else
			{//Mixed observers
				if (m_vUnfoldedTransnames.size() == 0 || m_vUnfoldedPlacenames.size() == 0 || m_vAllElements.size()==0)
				{
					UnfoldNet();

					std::merge(m_vUnfoldedTransnames.begin(), m_vUnfoldedTransnames.end(),
						m_vUnfoldedPlacenames.begin(), m_vUnfoldedPlacenames.end(),
						std::inserter(m_vAllElements, m_vAllElements.begin()));

					std::merge(m_Places.begin(), m_Places.end(),
						m_Transitions.begin(), m_Transitions.end(),
						std::inserter(m_vAllElements, m_vAllElements.begin()));
				}

				for (auto itMap = m_mObserve2Type.begin(); itMap != m_mObserve2Type.end(); ++itMap)
				{//append the pre-defined observer names
					m_vAllElements.insert(itMap->first.ToStdString());
				}


				std::set_difference(l_Vars.begin(), l_Vars.end(), m_vAllElements.begin(), m_vAllElements.end(), std::inserter(l_Diff, l_Diff.begin()));

			}

			if (!l_Diff.empty())
			{
				wxString l_sFunction(l_pcFunction->toString().c_str(), wxConvUTF8);
				wxString l_sExpanded(l_pcExpanded->toString().c_str(), wxConvUTF8);

				wxString l_sMsg = wxT("\nThe following identifiers are not valid: ");
				for (std::set<std::string>::const_iterator it = l_Diff.begin(); it != l_Diff.end(); ++it)
				{
					if (it != l_Diff.begin())
					{
						l_sMsg << wxT(", ");
					}
					l_sMsg << wxString((*it).c_str(), wxConvUTF8);
				}


				SP_MESSAGEBOX(l_sMsg, wxT("Check Observer \"") + p_sName + wxT("\""), wxOK | wxICON_ERROR);

				return false;
			}
		}

		if (m_pcGraph->GetNetclass()->GetDisplayName().Contains(wxT("Colored")))
		{
			if (p_sType == wxT("Place"))
			{
				m_mObserve2Type[p_sName] = wxT("Col|Place");
			}
			else if (p_sType == wxT("Transition"))
			{
				m_mObserve2Type[p_sName] = wxT("Col|Transition");
			}
			else
			{
				m_mObserve2Type[p_sName] = p_sType;
			}

		}
		if (b_showTip)
		{// if everything is ok
			new wxTipWindow(this, wxT("the observer ") + p_sName + wxT(" is correct"), 1000);
		}
		return true;

}

void SP_DLG_NewObserverDefinition::OnGridCellValueChanged(wxGridEvent& p_gEvent)
{
	int col = p_gEvent.GetCol();
	int row = p_gEvent.GetRow();

	wxString colLabel = m_pcObserverSetGrid->GetColLabelValue(col);
	wxString l_sCellValue = m_pcObserverSetGrid->GetCellValue(row, col);

	if (colLabel == wxT("Observer"))
	{

		if (ExistConstant(l_sCellValue, row))
		{
			SP_MESSAGEBOX(wxT("the observer '") + l_sCellValue + wxT("' already exists !"), wxT("Error"), wxOK | wxICON_ERROR);
			//SP_LOG(wxLOG_Error, wxT("constant already exists"));

			m_pcObserverSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}

		if (m_Places.find(l_sCellValue) != m_Places.end())
		{
			SP_MESSAGEBOX(wxT("the observer '") + l_sCellValue + wxT("' already exists as place!"), wxT("Error"), wxOK | wxICON_ERROR);
			//SP_LOG(wxLOG_Error, wxT("constant name already exists as place"));

			m_pcObserverSetGrid->SetCellValue(row, col, m_sOldCellValue);
			return;
		}
		return;
	}

	if (colLabel == wxT("Type"))
	{
		return;
	}

	if (colLabel == wxT("Body"))
	{
		return;
	}

	if (colLabel == wxT("Comment"))
	{
		return;
	}

	if (colLabel == wxT("Show"))
	{
		return;
	}
}

bool SP_DLG_NewObserverDefinition::ExistConstant(const wxString& p_sName, const int row)
{

	for (int i = 0; i < m_pcObserverSetGrid->GetNumberRows(); i++)
	{
		if (i != row)
		{
			if (p_sName.IsSameAs(m_pcObserverSetGrid->GetCellValue(i, NAME)))
			{
				return true;
			}
		}
	}

	return false;
}

void SP_DLG_NewObserverDefinition::LoadSetNames()
{
	m_pcObserverSetGrid->AppendCols(5);

	m_pcObserverSetGrid->SetColLabelValue(NAME, wxT("Observer"));
	m_pcObserverSetGrid->SetColSize(NAME, 100);

	m_pcObserverSetGrid->SetColLabelValue(TYPE, wxT("Type"));
	m_pcObserverSetGrid->SetColSize(TYPE, 100);

	m_pcObserverSetGrid->SetColLabelValue(COMMENT, wxT("Comment"));
	m_pcObserverSetGrid->SetColSize(COMMENT, 120);

	m_pcObserverSetGrid->SetColLabelValue(SHOW, wxT("Show"));
	m_pcObserverSetGrid->SetColSize(SHOW, 50);

	m_pcObserverSetGrid->SetColLabelValue(BODY, wxT("Body"));
	m_pcObserverSetGrid->SetColSize(BODY, 200);
}

bool SP_DLG_NewObserverDefinition::FindString(wxArrayString& p_asStrArray, wxString& p_sValue)
{
	for (unsigned long l_nPos = 0; l_nPos < p_asStrArray.GetCount(); l_nPos++)
	{
		if (p_asStrArray[l_nPos] == p_sValue)
		{
			return true; //found
		}
	}

	return false; //not found
}

void SP_DLG_NewObserverDefinition::InitializeDataTypes()
{
	SP_DS_Metadata* l_pcMetadata;

	l_pcMetadata = m_pcObservers->GetPrototype();

	set<wxString> l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetPossibleValues();

	if (m_pcGraph->GetNetclass()->GetDisplayName().Contains(wxT("Colored")))//george
	{
	    l_sType.insert("Place instance");
		l_sType.insert("Transition instance");
		l_sType.insert("Mixed");
	}

	set<wxString>::iterator it;

	m_sAvailableDatatypes = wxT(" ");

	m_datatypes.Alloc(l_sType.size());
	m_datatypes.Empty();

	for (it = l_sType.begin(); it != l_sType.end(); it++)
	{
		m_datatypes.Add(*it);
		m_sAvailableDatatypes << *it;
		m_sAvailableDatatypes << wxT(" ");
	}

	size_t count = m_datatypes.GetCount();
	datatypes = new wxString[count];
	for (size_t n = 0; n < count; n++)
	{

		datatypes[n] = m_datatypes[n];
		wxString s = datatypes[n];
	}

}

void SP_DLG_NewObserverDefinition::OnGridCellSelected(wxGridEvent& ev)
{
	ev.Skip();
}

void SP_DLG_NewObserverDefinition::OnEditorShown(wxGridEvent& ev)
{
	m_sOldCellValue = m_pcObserverSetGrid->GetCellValue(ev.GetRow(), ev.GetCol());

	ev.Skip();
}

void SP_DLG_NewObserverDefinition::LoadNodes()
{
	//load discrete nodes
	LoadNodeOfType(SP_DS_DISCRETE_PLACE, m_Places);
	LoadNodeOfType(SP_DS_DISCRETE_TRANS, m_Transitions);

	//load continuous nodes
	LoadNodeOfType(SP_DS_CONTINUOUS_PLACE, m_Places);
	LoadNodeOfType(SP_DS_CONTINUOUS_TRANS, m_Transitions);

	//load immediate nodes
	LoadNodeOfType(SP_DS_IMMEDIATE_TRANS, m_Transitions);

	//load deterministic nodes
	LoadNodeOfType(SP_DS_DETERMINISTIC_TRANS, m_Transitions);

	//load scheduled nodes
	LoadNodeOfType(SP_DS_SCHEDULED_TRANS, m_Transitions);
}

void SP_DLG_NewObserverDefinition::LoadNodeOfType(const wxString& p_sPlaceType, std::set<std::string>& p_names)
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(p_sPlaceType);

	if (l_pcNodeclass == NULL)
	{
		return;
	}

	SP_ListNode::const_iterator l_itElem;
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		p_names.insert(l_sNodeName);
	}
}


void SP_DLG_NewObserverDefinition::UnfoldNet()
{
	SP_IddNetUnfolder unfolder;
	dssd::andl::Net_ptr andl_net;


	if (unfolder(m_pcGraph, false, false, false))
	{

		andl_net = unfolder.GetUnfoldedNet();
	}
	else
	{
		//unfolding error
		return;
	}

	if (andl_net)
	{

		for (auto& place : *andl_net->places_)
		{
			m_vUnfoldedPlacenames.insert(place->name_);
		}
		for (auto& trans : *andl_net->transitions_)
		{

			m_vUnfoldedTransnames.insert(trans->name_);
		}
	}
}


void SP_DLG_NewObserverDefinition::OnAssistant(wxCommandEvent& p_cEvent)
{
	SP_VectorString* l_vpcUnfoldedPlaces=new SP_VectorString;
	SP_VectorString* l_vpcUnfoldedTrans=new SP_VectorString;
	SP_IddNetUnfolder unfolder;
	dssd::andl::Net_ptr andl_net;
	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName().Contains(_T("Colored")))
	{//only for colored nets
		wxBusyInfo* wait = new wxBusyInfo(wxT("Please wait..."), this);
		if (unfolder(m_pcGraph, false, false, false))//unfold the net to get the instances
		{
			wxDELETE(wait);
			andl_net = unfolder.GetUnfoldedNet();
		}
		else
		{
			//unfolding error
			return;
		}
	}
	if (andl_net)
	{

		for (auto& place : *andl_net->places_)
		{
			wxString l_sPlaceId = place->name_;
			l_vpcUnfoldedPlaces->push_back(l_sPlaceId);


		}
		for (auto& trans : *andl_net->transitions_)
		{
			wxString l_sPlaceId = trans->name_;
			l_vpcUnfoldedTrans->push_back(l_sPlaceId);

		}
	}

    //feed the observer assistant with the instances of the unfolded net
	wxString l_pcReturnText;
	SP_DLG_StFunctionAssistent* l_pcDlg = new SP_DLG_StFunctionAssistent(SP_ST_PARSER_TYPE_CHECK_FUNCTION,
		NULL,l_pcReturnText, this,_T(""), wxT("Assistent"),wxDEFAULT_DIALOG_STYLE, l_vpcUnfoldedTrans, l_vpcUnfoldedPlaces);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{

		m_pcObserverSetGrid->SetCellValue(m_pcObserverSetGrid->GetGridCursorRow(), BODY,
			l_pcDlg->GetReturnText());

	}

	l_pcDlg->Destroy();
}


bool SP_DLG_NewObserverDefinition::IsConst(const wxString& p_sVar)
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	SP_ListMetadata::const_iterator it;


	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		if (l_sName == p_sVar) return true;

	}
	return false;
}
