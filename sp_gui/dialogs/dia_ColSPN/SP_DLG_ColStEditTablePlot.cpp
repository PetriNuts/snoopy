//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/clrpicker.h>

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStEditTablePlot.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Metadata.h"
#include "sp_core/SP_Core.h"



IMPLEMENT_CLASS( SP_DLG_ColStEditTablePlot, wxDialog )

enum
{

	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_PLOT_ADJUSTMENT,
	SP_ID_PLACETRANSITIONRADIOBOX,
	SP_ID_OUTPUTTYPERADIOBOX,
	SP_ID_SUMOFNODESCHECKBOX,
	SP_ID_PLACERESULTSRADIOBOX,
	SP_ID_CHOOSEALLCHECKBOX,
	SP_ID_COMBOBOX_COLORPLACE_CHOICE

};

BEGIN_EVENT_TABLE( SP_DLG_ColStEditTablePlot, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ColStEditTablePlot::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColStEditTablePlot::OnDlgCancel )

EVT_RADIOBOX( SP_ID_PLOT_ADJUSTMENT, SP_DLG_ColStEditTablePlot::OnChangedAdjustmentType )
EVT_RADIOBOX( SP_ID_PLACETRANSITIONRADIOBOX, SP_DLG_ColStEditTablePlot::OnChangedPlaceTransition )
EVT_RADIOBOX( SP_ID_OUTPUTTYPERADIOBOX, SP_DLG_ColStEditTablePlot::OnChangedOutType )
EVT_RADIOBOX( SP_ID_PLACERESULTSRADIOBOX, SP_DLG_ColStEditTablePlot::OnChangedPlaceResults )

EVT_CHECKBOX( SP_ID_SUMOFNODESCHECKBOX, SP_DLG_ColStEditTablePlot::OnChangedSumOfNodes)
EVT_CHECKBOX( SP_ID_CHOOSEALLCHECKBOX, SP_DLG_ColStEditTablePlot::OnChooseAll)
EVT_CHECKLISTBOX(SP_ID_CHECKLISTBOX_PLACE_CHOICE, SP_DLG_ColStEditTablePlot::OnClickItem)
EVT_COMBOBOX(SP_ID_COMBOBOX_COLORPLACE_CHOICE, SP_DLG_ColStEditTablePlot::OnComBoxSel)

END_EVENT_TABLE()

SP_DLG_ColStEditTablePlot::SP_DLG_ColStEditTablePlot(SP_StTablePlotEditType p_eType, const wxString p_sPlaceType,
							SP_MapString2String* p_pmsPlaceNamesById,SP_MapString2String* p_pmsTransitionNamesById,
							SP_VectorString* p_psColoredPlaceNames, SP_VectorString* p_psColoredTransitionNames,
							vector<wxString>* p_pMixexPlaceNameArray,vector<wxString>* p_pMixexTransitionNameArray,
		SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle) :

	wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxSize(500, 300), p_nStyle | wxRESIZE_BORDER | wxMAXIMIZE_BOX ),

	m_bSumOfNodes(false),
	m_pcEditMetadata(p_pcEditMetadata),
	m_sOutputType(wxT("Colored")),
	m_sPlaceTransition(wxT("Place")),
	m_sPlaceResults(wxT("Marking")),
	m_sPlaceType(p_sPlaceType)
	
{

	m_eType = p_eType;

	 m_pmsPlaceNamesById = p_pmsPlaceNamesById ; 
	 m_pmsTransitionNamesById = p_pmsTransitionNamesById ; 

	 m_pmsPlaceNames = p_psColoredPlaceNames;
	 m_psTransitionNames = p_psColoredTransitionNames;

	 m_pMixexPlaceNameArray = p_pMixexPlaceNameArray;
	 m_pMixexTransitionNameArray = p_pMixexTransitionNameArray;

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
		if(dynamic_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("SumOfNodes")))->GetValue())
		{
			m_bSumOfNodes = true;
		}

		if(m_pcEditMetadata->GetAttribute(wxT("Results"))->GetValueString() != m_sPlaceResults)
		{
			m_sPlaceResults = wxT("Standard Deviation");
		}

		if(m_pcEditMetadata->GetAttribute(wxT("OutputType"))->GetValueString() != m_sOutputType)
		{
			if( m_pcEditMetadata->GetAttribute(wxT("OutputType"))->GetValueString() == wxT("Unfolded") )
				m_sOutputType = wxT("Unfolded");
			else
				m_sOutputType = wxT("Both");
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

	//Output type selection
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_asOutputTypeChoices[] = { wxT("Colored"), wxT("Unfolded"),wxT("Both")};
	m_pcOutputTypeRadioBox = new wxRadioBox(this, SP_ID_OUTPUTTYPERADIOBOX, wxT("OutType"), wxDefaultPosition, wxDefaultSize, 3, l_asOutputTypeChoices, 3, wxRA_SPECIFY_ROWS | wxEXPAND);
	l_pcRowSizer->Add(m_pcOutputTypeRadioBox, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 5);
	l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	if(m_sOutputType == wxT("Colored"))
	{
		m_pcOutputTypeRadioBox->SetSelection(0);
	}
	else if(m_sOutputType == wxT("Unfolded"))
	{
		m_pcOutputTypeRadioBox->SetSelection(1);
	}
	else
	{
		m_pcOutputTypeRadioBox->SetSelection(2);
	}

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_asPlaceTransitionChoices[] = { m_sPlaceType, wxT("Transition")};
	m_pcPlaceTransitionRadioBox = new wxRadioBox(this, SP_ID_PLACETRANSITIONRADIOBOX, wxT("Nodeclass"), wxDefaultPosition, wxDefaultSize, 2, l_asPlaceTransitionChoices, 2, wxRA_SPECIFY_ROWS | wxEXPAND);
	l_pcRowSizer->Add(m_pcPlaceTransitionRadioBox, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 5);
	l_pcContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	if(m_sPlaceTransition == wxT("Place") )
	{
		m_pcPlaceTransitionRadioBox->SetSelection(0);
	}
	else
	{
		m_pcPlaceTransitionRadioBox->SetSelection(1);
	}

	//
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
	if(m_sPlaceTransition == wxT("Transition"))
	{
		m_pcPlaceResultsRadioBox->Show(false);
	}
	//


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

	//m_pcPlaceChoiceCheckListBox = new wxCheckListBox(this, SP_ID_CHECKLISTBOX_PLACE_CHOICE, wxDefaultPosition, wxSize(200, 350));

	wxSizer* l_pcSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcChooseAllCheckBox = new wxCheckBox(this, SP_ID_CHOOSEALLCHECKBOX, wxT("Select all"));
	l_pcSelectSizer->Add(m_pcChooseAllCheckBox, 1, wxLEFT | wxRIGHT | wxTOP, 5);
	l_pcPlaceChoiceSizer->Add(l_pcSelectSizer, 0, wxEXPAND);

	l_pcSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSelectSizer->Add( new wxStaticText( this, -1, wxT("Select a colored place:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcChooseColoredPlace = new wxComboBox( this, SP_ID_COMBOBOX_COLORPLACE_CHOICE, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY);
	m_pcChooseColoredPlace->Append(wxT("")); //
	for(unsigned i = 0; i < p_psColoredPlaceNames->size(); i++)
	{
		m_pcChooseColoredPlace->Append( (*p_psColoredPlaceNames)[i]);
	}
	m_pcChooseColoredPlace->SetSelection(0);
	l_pcSelectSizer->Add(m_pcChooseColoredPlace, 1, wxLEFT | wxRIGHT | wxTOP, 5);
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

void SP_DLG_ColStEditTablePlot::OnDlgOk(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColStEditTablePlot::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_ColStEditTablePlot::LoadData()
{
	wxArrayString l_asPlaces;

	m_mnPlaceChoiceId.clear();
	m_mnPlaceChoicePos.clear();

	if(m_bSumOfNodes)
	{
	}
	else
	{	

		if( m_sPlaceTransition == wxT("Place") && m_sOutputType == wxT("Unfolded"))
		{
			for( unsigned i = 0; i < m_pmsPlaceNamesById->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_pmsPlaceNamesById)[wxString::Format(wxT("%d"),i)]);
			}	

		}
		else if ( m_sPlaceTransition == wxT("Transition") && m_sOutputType == wxT("Unfolded"))
		{
			for( unsigned i = 0; i < m_pmsTransitionNamesById->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_pmsTransitionNamesById)[wxString::Format(wxT("%d"),i)]);
			}	
		}
		else if ( m_sPlaceTransition == wxT("Place") && m_sOutputType == wxT("Colored"))
		{
			for( unsigned i = 0; i < m_pmsPlaceNames->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_pmsPlaceNames)[i]);
			}	
		
		}
		else if ( m_sPlaceTransition == wxT("Transition") && m_sOutputType == wxT("Colored"))
		{
			for( unsigned i = 0; i < m_psTransitionNames->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_psTransitionNames)[i]);
			}	
		
		}
		else if ( m_sPlaceTransition == wxT("Place") && m_sOutputType == wxT("Both"))
		{
			for( unsigned i = 0; i < m_pMixexPlaceNameArray->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_pMixexPlaceNameArray)[i]);
			}	
		
		}
		else if ( m_sPlaceTransition == wxT("Transition") && m_sOutputType == wxT("Both"))
		{
			for( unsigned i = 0; i < m_pMixexTransitionNameArray->size(); i++ )
			{
				m_mnPlaceChoicePos[wxString::Format(wxT("%d"),i)] = i;
				m_mnPlaceChoiceId[i] = wxString::Format(wxT("%d"),i);
				l_asPlaces.Add((*m_pMixexTransitionNameArray)[i]);
			}	
		
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

void SP_DLG_ColStEditTablePlot::LoadPlot(SP_DS_Metadata* p_pcMetadata)
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

void SP_DLG_ColStEditTablePlot::LoadTable(SP_DS_Metadata* p_pcMetadata)
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

bool SP_DLG_ColStEditTablePlot::SaveData()
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

		l_pcNewMetadata->GetAttribute(wxT("OutputType"))->SetValueString(m_sOutputType);

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

		m_pcEditMetadata->GetAttribute(wxT("OutputType"))->SetValueString(m_sOutputType);

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

void SP_DLG_ColStEditTablePlot::OnChangedAdjustmentType(wxCommandEvent& p_cEvent)
{
	EnableIntervalComponents();
}

void SP_DLG_ColStEditTablePlot::EnableIntervalComponents()
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

void SP_DLG_ColStEditTablePlot::OnChangedPlaceTransition(wxCommandEvent& p_cEvent)
{
	if(!m_pcPlaceTransitionRadioBox->GetSelection())
	{
		m_sPlaceTransition = wxT("Place");
	}
	else
	{
		m_sPlaceTransition = wxT("Transition");
	}
	m_pcPlaceChoiceStaticBox->SetLabel(m_sPlaceTransition + wxT(" choice:"));
	LoadData();
}

void SP_DLG_ColStEditTablePlot::OnChangedSumOfNodes(wxCommandEvent& p_cEvent)
{
	m_bSumOfNodes = m_pcSumOfNodesCheckBox->GetValue();
	LoadData();
}

void SP_DLG_ColStEditTablePlot::OnChangedOutType(wxCommandEvent& p_cEvent)
{
	if(m_pcOutputTypeRadioBox->GetSelection() == 0)
	{
		m_sOutputType = wxT("Colored");
	}
	else if(m_pcOutputTypeRadioBox->GetSelection() == 1)
	{
		m_sOutputType = wxT("Unfolded");
	}
	else
	{
		m_sOutputType = wxT("Both");
	}
	
	LoadData();

}


void SP_DLG_ColStEditTablePlot::OnChangedPlaceResults(wxCommandEvent& p_cEvent)
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


void SP_DLG_ColStEditTablePlot::OnChooseAll(wxCommandEvent& p_cEvent)
{
	m_pcChooseColoredPlace->SetSelection(0);

	bool l_bChecked = m_pcChooseAllCheckBox->GetValue();
	
	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{
		m_pcPlaceChoiceCheckListBox->Check(i, l_bChecked);
	}		
	
}

void SP_DLG_ColStEditTablePlot::OnClickItem( wxCommandEvent& p_cEvent )
{
	if( m_pcChooseAllCheckBox->GetValue() )
		m_pcChooseAllCheckBox->SetValue(false);
}

void SP_DLG_ColStEditTablePlot::OnComBoxSel( wxCommandEvent& p_cEvent )
{
	wxString l_sSelColPlace = m_pcChooseColoredPlace->GetValue();

	if(l_sSelColPlace == wxT(""))
		return;

	if( m_pcChooseAllCheckBox->GetValue() )
		m_pcChooseAllCheckBox->SetValue(false);	

	for (unsigned int i = 0; i < m_pcPlaceChoiceCheckListBox->GetCount(); i++)
	{
		wxString l_sCurrplace = m_pcPlaceChoiceCheckListBox->GetString(i);
		if(l_sCurrplace.size()==l_sSelColPlace.size())
		{
			if(l_sCurrplace == l_sSelColPlace )		
				m_pcPlaceChoiceCheckListBox->Check(i, true);		
		}
		else if(l_sCurrplace.size()>l_sSelColPlace.size())
		{
			wxString l_sSubName = l_sCurrplace.Mid(0,l_sSelColPlace.size()+1);			
			if(l_sSubName == l_sSelColPlace+wxT("_") )
				m_pcPlaceChoiceCheckListBox->Check(i, true);		
		}		
	}	
}
