//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/clrpicker.h>

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StEditTablePlot.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Metadata.h"
#include "sp_core/SP_Core.h"

IMPLEMENT_CLASS( SP_DLG_StEditTablePlot, wxDialog )

enum
{

	SP_ID_CHECKLISTBOX_PLACE_CHOICE = SP_ID_LAST_ID + 1,
	SP_ID_PLOT_ADJUSTMENT,
	SP_ID_PLACETRANSITIONRADIOBOX,
	SP_ID_SUMOFNODESCHECKBOX,
	SP_ID_PLACERESULTSRADIOBOX,
	SP_ID_CHOOSEALLCHECKBOX

};

BEGIN_EVENT_TABLE( SP_DLG_StEditTablePlot, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_StEditTablePlot::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_StEditTablePlot::OnDlgCancel )

EVT_RADIOBOX( SP_ID_PLOT_ADJUSTMENT, SP_DLG_StEditTablePlot::OnChangedAdjustmentType )
EVT_RADIOBOX( SP_ID_PLACETRANSITIONRADIOBOX, SP_DLG_StEditTablePlot::OnChangedPlaceTransition )
EVT_RADIOBOX( SP_ID_PLACERESULTSRADIOBOX, SP_DLG_StEditTablePlot::OnChangedPlaceResults )

EVT_CHECKBOX( SP_ID_SUMOFNODESCHECKBOX, SP_DLG_StEditTablePlot::OnChangedSumOfNodes)
EVT_CHECKBOX( SP_ID_CHOOSEALLCHECKBOX, SP_DLG_StEditTablePlot::OnChooseAll)
EVT_CHECKLISTBOX(SP_ID_CHECKLISTBOX_PLACE_CHOICE, SP_DLG_StEditTablePlot::OnClickItem)


END_EVENT_TABLE()

SP_DLG_StEditTablePlot::SP_DLG_StEditTablePlot(SP_StTablePlotEditType p_eType, const wxString p_sPlaceType,SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize(500, 300), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX ),

	m_bSumOfNodes(false),
	m_pcEditMetadata(p_pcEditMetadata),
	m_sPlaceTransition(p_sPlaceType),
	m_sPlaceResults(wxT("Marking")),
	m_sPlaceType(p_sPlaceType)
{

	m_eType = p_eType;

	switch (m_eType)
	{

	case SP_ST_TABLE_PLOT_TYPE_NEW_TABLE:

		this->SetLabel(wxT("New table"));

		break;

	case SP_ST_TABLE_PLOT_TYPE_NEW_PLOT:

		this->SetLabel(wxT("New plot"));

		break;

	case SP_ST_TABLE_PLOT_TYPE_EDIT_TABLE:

		this->SetLabel(wxT("Edit table"));

		break;

	case SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT:

		this->SetLabel(wxT("Edit plot"));

		break;

	}


	if(m_pcEditMetadata)
	{
		if(m_pcEditMetadata->GetAttribute(wxT("Nodeclass"))->GetValueString() != m_sPlaceTransition && m_sPlaceType!=SP_DS_CONTINUOUS_PLACE)
		{
			m_sPlaceTransition = wxT("Transition");
		}
		if(m_pcEditMetadata->GetAttribute(wxT("Results"))->GetValueString() != m_sPlaceResults)
		{
			m_sPlaceResults = wxT("Standard Deviation");
		}
		if(dynamic_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("SumOfNodes")))->GetValue())
		{
			m_bSumOfNodes = true;
		}
	}
	m_pcMainSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxVERTICAL);

	wxSizer* l_pcMainContentSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);

	wxSizer* l_pcControlButtonSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("")), wxHORIZONTAL);

	m_pcMainSizer1->Add(l_pcMainContentSizer);
	m_pcMainSizer1->Add(l_pcControlButtonSizer);

	wxSizer* l_pcContentSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Properties:")), wxVERTICAL);

	m_pcPlaceChoiceStaticBox = new wxStaticBox(this, -1, m_sPlaceTransition + wxT(" choice:"));
	wxSizer* l_pcPlaceChoiceSizer = new wxStaticBoxSizer(m_pcPlaceChoiceStaticBox, wxVERTICAL);

	l_pcMainContentSizer->Add(l_pcContentSizer);
	l_pcMainContentSizer->Add(l_pcPlaceChoiceSizer);

	wxSizer* l_pcRowSizer;

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Name:")), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	m_pcNameTextCtrl = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcNameTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcContentSizer->Add(l_pcRowSizer);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcSumOfNodesCheckBox = new wxCheckBox(this, SP_ID_SUMOFNODESCHECKBOX, wxT("Calculate Sum of Nodes\nwith the same Type"));
	l_pcRowSizer->Add(m_pcSumOfNodesCheckBox, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 5);
	l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	m_pcSumOfNodesCheckBox->SetValue(m_bSumOfNodes);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_asPlaceTransitionChoices[] = { m_sPlaceType, wxT("Transition")};
	m_pcPlaceTransitionRadioBox = new wxRadioBox(this, SP_ID_PLACETRANSITIONRADIOBOX, wxT("Nodeclass"), wxDefaultPosition, wxDefaultSize, 2, l_asPlaceTransitionChoices, 2, wxRA_SPECIFY_ROWS | wxEXPAND);
	l_pcRowSizer->Add(m_pcPlaceTransitionRadioBox, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 5);
	l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	if(m_sPlaceTransition == m_sPlaceType)
	{
		m_pcPlaceTransitionRadioBox->SetSelection(0);
	}
	else
	{
		m_pcPlaceTransitionRadioBox->SetSelection(1);
	}

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_asPlaceResultsChoices[] = { wxT("Marking"), wxT("Standard Deviation")};
	m_pcPlaceResultsRadioBox = new wxRadioBox(this, SP_ID_PLACERESULTSRADIOBOX, wxT("Results"), wxDefaultPosition, wxDefaultSize, 2, l_asPlaceResultsChoices, 2, wxRA_SPECIFY_ROWS | wxEXPAND);
	l_pcRowSizer->Add(m_pcPlaceResultsRadioBox, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 5);
	l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	if(m_sPlaceResults == wxT("Marking"))
	{
		m_pcPlaceResultsRadioBox->SetSelection(0);
	}
	else
	{
		m_pcPlaceResultsRadioBox->SetSelection(1);
	}
	if(m_sPlaceTransition != m_sPlaceType)
	{
		m_pcPlaceResultsRadioBox->Show(false);
	}
	if ((m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_TABLE) || (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_TABLE))
	{

	}
	else
		if ((m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_PLOT) || (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT))
		{
			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			wxString l_asAdjustmentChoices[] =
			{ wxT("Automatically"), wxT("Fixed") };

			m_pcFixedPlotAdjustment = new wxRadioBox(this, SP_ID_PLOT_ADJUSTMENT, wxT("Adjustment type"), wxDefaultPosition, wxDefaultSize, 2, l_asAdjustmentChoices, 2, wxRA_SPECIFY_ROWS | wxEXPAND);

			l_pcRowSizer->Add(m_pcFixedPlotAdjustment, 1, wxLEFT | wxRIGHT | wxTOP| wxEXPAND, 5);

			l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);

			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("X-Axis minimum:")), 1, wxLEFT | wxRIGHT | wxTOP, 5);

			m_pcXAxisMinTextCtrl = new wxTextCtrl(this, -1, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);

			l_pcRowSizer->Add(m_pcXAxisMinTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);

			l_pcContentSizer->Add(l_pcRowSizer);

			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("X-Axis maximum:")), 1, wxLEFT | wxRIGHT | wxTOP, 5);

			m_pcXAxisMaxTextCtrl = new wxTextCtrl(this, -1, wxT("100"), wxDefaultPosition, wxDefaultSize, 0);

			l_pcRowSizer->Add(m_pcXAxisMaxTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);

			l_pcContentSizer->Add(l_pcRowSizer);

			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Y-Axis minimum:")), 1, wxLEFT | wxRIGHT | wxTOP, 5);

			m_pcYAxisMinTextCtrl = new wxTextCtrl(this, -1, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);

			l_pcRowSizer->Add(m_pcYAxisMinTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);

			l_pcContentSizer->Add(l_pcRowSizer);

			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Y-Axis maximum:")), 1, wxLEFT | wxRIGHT | wxTOP, 5);

			m_pcYAxisMaxTextCtrl = new wxTextCtrl(this, -1, wxT("100"), wxDefaultPosition, wxDefaultSize, 0);

			l_pcRowSizer->Add(m_pcYAxisMaxTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5);

			l_pcContentSizer->Add(l_pcRowSizer);

		}

	
	wxSizer* l_pcSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcChooseAllCheckBox = new wxCheckBox(this, SP_ID_CHOOSEALLCHECKBOX, wxT("Select all"));
	l_pcSelectSizer->Add(m_pcChooseAllCheckBox, 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcPlaceChoiceSizer->Add(l_pcSelectSizer, 0, wxEXPAND);

	l_pcSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcPlaceChoiceCheckListBox = new wxCheckListBox(this, SP_ID_CHECKLISTBOX_PLACE_CHOICE, wxDefaultPosition, wxSize(200, 350));
	l_pcSelectSizer->Add(m_pcPlaceChoiceCheckListBox, 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcPlaceChoiceSizer->Add(l_pcSelectSizer, 0, wxEXPAND);
	
	//l_pcPlaceChoiceSizer->Add(m_pcPlaceChoiceCheckListBox, 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxButton(this, wxID_OK, wxT("Save")), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Close")), 1, wxLEFT | wxRIGHT | wxTOP, 5);

	l_pcControlButtonSizer->Add(l_pcRowSizer);

	LoadData();

	SetSizerAndFit(m_pcMainSizer1);

	if(m_sPlaceType==SP_DS_CONTINUOUS_PLACE)
		m_pcPlaceTransitionRadioBox->Enable(false);

}

void SP_DLG_StEditTablePlot::OnDlgOk(wxCommandEvent& p_cEvent)
{

	if (Validate() && TransferDataFromWindow())
	{

		if (IsModal())
		{

			if (SaveData())
			{

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

void SP_DLG_StEditTablePlot::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_StEditTablePlot::LoadData()
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(m_sPlaceTransition);
	SP_ListNode::const_iterator l_itElem;

	wxArrayString l_asPlaces;

	int l_nPos = 0;
	m_mnPlaceChoiceId.clear();
	m_mnPlaceChoicePos.clear();
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		if(m_bSumOfNodes)
		{
			l_sName = l_sName.Mid(0, l_sName.Find(wxT("::")));
			if(l_asPlaces.Index(l_sName) == wxNOT_FOUND)
			{
				wxString l_sId = wxString::Format(wxT("%ld"), (*l_itElem)->GetId());
				m_mnPlaceChoicePos[l_sId] = l_nPos;
				m_mnPlaceChoiceId[l_nPos] = l_sId;

				l_nPos++;

				l_asPlaces.Add(l_sName);
			}
		}
		else
		{
			wxString l_sId = wxString::Format(wxT("%ld"), (*l_itElem)->GetId());
			m_mnPlaceChoicePos[l_sId] = l_nPos;
			m_mnPlaceChoiceId[l_nPos] = l_sId;

			l_nPos++;

			l_asPlaces.Add(l_sName);
		}
	}

	m_pcPlaceChoiceCheckListBox->Set(l_asPlaces);

	if (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_TABLE)
	{
		LoadTable(m_pcEditMetadata);
	}

	if (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT)
	{
		LoadPlot(m_pcEditMetadata);
	}

	if (m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_PLOT)
	{
		m_pcFixedPlotAdjustment->SetSelection(0);
		EnableIntervalComponents();
	}
}

void SP_DLG_StEditTablePlot::LoadPlot(SP_DS_Metadata* p_pcMetadata)
{

	m_pcNameTextCtrl->SetValue(dynamic_cast<SP_DS_NameAttribute*>(p_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());

	m_pcFixedPlotAdjustment->SetSelection(dynamic_cast<SP_DS_BoolAttribute*> (p_pcMetadata->GetAttribute(wxT("FixedAdjustment")))->GetValue());

	EnableIntervalComponents();

	m_pcXAxisMinTextCtrl->SetValue(p_pcMetadata->GetAttribute(wxT("X_Axis_Min"))->GetValueString());
	m_pcXAxisMaxTextCtrl->SetValue(p_pcMetadata->GetAttribute(wxT("X_Axis_Max"))->GetValueString());

	m_pcYAxisMinTextCtrl->SetValue(p_pcMetadata->GetAttribute(wxT("Y_Axis_Min"))->GetValueString());
	m_pcYAxisMaxTextCtrl->SetValue(p_pcMetadata->GetAttribute(wxT("Y_Axis_Max"))->GetValueString());

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*> (p_pcMetadata->GetAttribute(wxT("PlaceList")));

	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{
		m_pcPlaceChoiceCheckListBox->Check(i, false);
	}
	for (unsigned int j = 0; j < l_pcPlaceIdList->GetRowCount(); j++)
	{
		m_pcPlaceChoiceCheckListBox->Check(m_mnPlaceChoicePos[l_pcPlaceIdList->GetCell(j,0)], true);
	}

}

void SP_DLG_StEditTablePlot::LoadTable(SP_DS_Metadata* p_pcMetadata)
{

	m_pcNameTextCtrl->SetValue(dynamic_cast<SP_DS_NameAttribute*>(p_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());

	if (m_pcNameTextCtrl->GetValue() == wxT("Main table"))
	{
		m_pcNameTextCtrl->SetEditable(false);
	}

	SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*> (p_pcMetadata->GetAttribute(wxT("PlaceList")));

	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{
		m_pcPlaceChoiceCheckListBox->Check(i, false);
	}
	for (unsigned int j = 0; j < l_pcPlaceIdList->GetRowCount(); j++)
	{
		m_pcPlaceChoiceCheckListBox->Check(m_mnPlaceChoicePos[l_pcPlaceIdList->GetCell(j,0)], true);
	}

}

bool SP_DLG_StEditTablePlot::SaveData()
{

	double l_nDouble0;
	double l_nDouble1;

	if ((m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_PLOT) || (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT))
	{
		if (m_pcNameTextCtrl->GetValue() == wxT(""))
		{
			SP_MESSAGEBOX(wxT("There must be a name. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR );
			return false;
		}
		if (m_pcXAxisMinTextCtrl->GetValue().ToDouble(&l_nDouble0))
		{
			if (m_pcXAxisMaxTextCtrl->GetValue().ToDouble(&l_nDouble1))
			{
				if (l_nDouble0 > l_nDouble1)
				{
					SP_MESSAGEBOX(wxT("The X-axis minimum value must be smaller or equal than the X-axis maximum value. The inputed values are invalid."), wxT("Error"), wxOK | wxICON_ERROR );
					return false;
				}
			}
			else
			{
				SP_MESSAGEBOX(wxT("The X-axis maximum value must be a real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR );
				return false;
			}
		}
		else
		{
			SP_MESSAGEBOX(wxT("The X-axis minimum value must be a real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR );
			return false;
		}
		if (m_pcYAxisMinTextCtrl->GetValue().ToDouble(&l_nDouble0))
		{
			if (m_pcYAxisMaxTextCtrl->GetValue().ToDouble(&l_nDouble1))
			{
				if (l_nDouble0 > l_nDouble1)
				{
					SP_MESSAGEBOX(wxT("The Y-axis minimum value must be smaller or equal than the Y-axis maximum value. The inputed values are invalid."), wxT("Error"), wxOK | wxICON_ERROR );
					return false;
				}
			}
			else
			{
				SP_MESSAGEBOX(wxT("The Y-axis maximum value must be a real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR );
				return false;
			}
		}
		else
		{
			SP_MESSAGEBOX(wxT("The Y-axis minimum value must be a real number. The inputed value is invalid."), wxT("Error"), wxOK | wxICON_ERROR );
			return false;
		}
	}

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass;

	if ((m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_TABLE) || (m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_PLOT))
	{
		if (m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_TABLE)
		{
			l_pcMetadataclass = l_pcGraph->GetMetadataclass(wxT("Table"));
		}
		else
		{
			l_pcMetadataclass = l_pcGraph->GetMetadataclass(wxT("Plot"));
		}

		SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->NewElement(1);

		l_pcNewMetadata->GetAttribute(wxT("Name"))->SetValueString(m_pcNameTextCtrl->GetValue());

		l_pcNewMetadata->GetAttribute(wxT("Nodeclass"))->SetValueString(m_sPlaceTransition);

		l_pcNewMetadata->GetAttribute(wxT("Results"))->SetValueString(m_sPlaceResults);

		dynamic_cast<SP_DS_BoolAttribute*>(l_pcNewMetadata->GetAttribute(wxT("SumOfNodes")))->SetValue(m_bSumOfNodes);

		if (m_eType == SP_ST_TABLE_PLOT_TYPE_NEW_PLOT)
		{
			bool l_bFlag = (m_pcFixedPlotAdjustment->GetSelection() == 0 ? false : true);

			dynamic_cast<SP_DS_BoolAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FixedAdjustment")))->SetValue(l_bFlag);

			l_pcNewMetadata->GetAttribute(wxT("X_Axis_Min"))->SetValueString(m_pcXAxisMinTextCtrl->GetValue());
			l_pcNewMetadata->GetAttribute(wxT("X_Axis_Max"))->SetValueString(m_pcXAxisMaxTextCtrl->GetValue());
			l_pcNewMetadata->GetAttribute(wxT("Y_Axis_Min"))->SetValueString(m_pcYAxisMinTextCtrl->GetValue());
			l_pcNewMetadata->GetAttribute(wxT("Y_Axis_Max"))->SetValueString(m_pcYAxisMaxTextCtrl->GetValue());
		}

		SP_DS_ColListAttribute * l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("PlaceList")));
		l_pcPlaceIdList->Clear();

		for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
		{
			if (m_pcPlaceChoiceCheckListBox->IsChecked(i))
			{
				unsigned int l_nNewRow = l_pcPlaceIdList->AppendEmptyRow();
				l_pcPlaceIdList->SetCell(l_nNewRow, 0, m_mnPlaceChoiceId[i]);
			}
		}

	}

	if ((m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_TABLE) || (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT))
	{

		m_pcEditMetadata->GetAttribute(wxT("Name"))->SetValueString(m_pcNameTextCtrl->GetValue());

		m_pcEditMetadata->GetAttribute(wxT("Nodeclass"))->SetValueString(m_sPlaceTransition);

		dynamic_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("SumOfNodes")))->SetValue(m_bSumOfNodes);

		if (m_eType == SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT)
		{
			bool l_bFlag = (m_pcFixedPlotAdjustment->GetSelection() == 0 ? false : true);

			dynamic_cast<SP_DS_BoolAttribute*> (m_pcEditMetadata->GetAttribute(wxT("FixedAdjustment")))->SetValue(l_bFlag);

			m_pcEditMetadata->GetAttribute(wxT("X_Axis_Min"))->SetValueString(m_pcXAxisMinTextCtrl->GetValue());
			m_pcEditMetadata->GetAttribute(wxT("X_Axis_Max"))->SetValueString(m_pcXAxisMaxTextCtrl->GetValue());
			m_pcEditMetadata->GetAttribute(wxT("Y_Axis_Min"))->SetValueString(m_pcYAxisMinTextCtrl->GetValue());
			m_pcEditMetadata->GetAttribute(wxT("Y_Axis_Max"))->SetValueString(m_pcYAxisMaxTextCtrl->GetValue());
		}

		SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*> (m_pcEditMetadata->GetAttribute(wxT("PlaceList")));
		l_pcPlaceIdList->Clear();

		for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
		{
			if (m_pcPlaceChoiceCheckListBox->IsChecked(i))
			{
				unsigned int l_nNewRow = l_pcPlaceIdList->AppendEmptyRow();
				l_pcPlaceIdList->SetCell(l_nNewRow, 0, m_mnPlaceChoiceId[i]);
			}
		}

	}

	return true;

}

void SP_DLG_StEditTablePlot::OnChangedAdjustmentType(wxCommandEvent& p_cEvent)
{
	EnableIntervalComponents();
}

void SP_DLG_StEditTablePlot::EnableIntervalComponents()
{

	bool l_bEnable = true;

	if (m_pcFixedPlotAdjustment->GetSelection() == 0)
	{
		l_bEnable = false;
	}

	m_pcXAxisMinTextCtrl->Enable(l_bEnable);
	m_pcXAxisMaxTextCtrl->Enable(l_bEnable);

	m_pcYAxisMinTextCtrl->Enable(l_bEnable);
	m_pcYAxisMaxTextCtrl->Enable(l_bEnable);

}

void SP_DLG_StEditTablePlot::OnChangedPlaceTransition(wxCommandEvent& p_cEvent)
{
	if(!m_pcPlaceTransitionRadioBox->GetSelection())
	{
		m_sPlaceTransition =m_sPlaceType;
		m_pcPlaceResultsRadioBox->Show(true);
	}
	else
	{
		m_sPlaceTransition = wxT("Transition");
		m_pcPlaceResultsRadioBox->Show(false);
	}
	m_pcPlaceChoiceStaticBox->SetLabel(m_sPlaceTransition + wxT(" choice:"));
	LoadData();
}

void SP_DLG_StEditTablePlot::OnChangedPlaceResults(wxCommandEvent& p_cEvent)
{
	if(!m_pcPlaceResultsRadioBox->GetSelection())
	{
		m_sPlaceResults = wxT("Marking");
	}
	else
	{
		m_sPlaceResults = wxT("Standard Deviation");
	}
}

void SP_DLG_StEditTablePlot::OnChangedSumOfNodes(wxCommandEvent& p_cEvent)
{
	m_bSumOfNodes = m_pcSumOfNodesCheckBox->GetValue();
	LoadData();
}


void SP_DLG_StEditTablePlot::OnChooseAll(wxCommandEvent& p_cEvent)
{
	bool l_bChecked = m_pcChooseAllCheckBox->GetValue();
	
	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{
		m_pcPlaceChoiceCheckListBox->Check(i, l_bChecked);
	}		
	
}

void SP_DLG_StEditTablePlot::OnClickItem( wxCommandEvent& p_cEvent )
{
	if( m_pcChooseAllCheckBox->GetValue() )
		m_pcChooseAllCheckBox->SetValue(false);
}
