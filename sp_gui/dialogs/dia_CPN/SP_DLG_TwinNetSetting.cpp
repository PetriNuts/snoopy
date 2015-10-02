//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/7/27 $
// Short Description: dialog to choose a master net
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/dia_CPN/SP_DLG_TwinNetSetting.h"
#include <wx/spinctrl.h>


IMPLEMENT_CLASS(SP_DLG_TwinNetSetting, wxDialog)

enum
{		
	SP_ID_LISTCTRL_ELEMENT,
	SP_ID_RADIO,
	SP_ID_COLORSETTXTCTRL,
	SP_ID_VARIABLETXTCTRL,
	SP_ID_TYPECOMBOBOX,
	SP_ID_COLORSTXTCTRL,
	SP_ID_FUSIONNODETXTCTRL,
	SP_ID_EXCHANGEPLACETXTCTRL,
	SP_ID_SCHEDULEDTRANSITIONETXTCTRL,
	SP_ID_NEWPLACETXTCTRL,
	SP_ID_NEWTRANSITIONTXTCTRL,
	SP_ID_PARAVALUETXTCTRL,
	SP_ID_PARANAMETXTCTRL,
	SP_ID_GENSIDECONDITIONCHECKLIST
};

BEGIN_EVENT_TABLE(SP_DLG_TwinNetSetting, wxDialog)
	EVT_BUTTON(wxID_OK, SP_DLG_TwinNetSetting::OnDlgOk)
	EVT_BUTTON(wxID_CLOSE, SP_DLG_TwinNetSetting::OnDlgClose)
	EVT_COMBOBOX(SP_ID_TYPECOMBOBOX, SP_DLG_TwinNetSetting::OnComboBoxSelected)
END_EVENT_TABLE()

SP_DLG_TwinNetSetting::SP_DLG_TwinNetSetting(wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	m_pcColorSetTextCtrl(NULL),
	m_pcVariableTextCtrl(NULL),
	m_pcColorsTextCtrl(NULL),
	m_pcFusionPlaceTextCtrl(NULL),
	m_pcScheduledTransitionTextCtrl(NULL),
	m_pcExchangeCoarsePlaceTextCtrl(NULL),
	m_pcNewPlaceTextCtrl(NULL),
	m_pcNewTransitionTextCtrl(NULL),
	m_pcTypeComboBoxCtrl(NULL),
	m_pcParameterNameTextCtrl(NULL),
	m_pcParameterValueTextCtrl(NULL),
	m_cbGenerateSideConditions(NULL),
	m_bGenerateSideConditions(false)
{

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);	

	//Set color set and variable
	wxBoxSizer* l_pcColorSetSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a Color Set Name:") ), 0, wxEXPAND | wxALL, 5);
	m_pcColorSetTextCtrl = new wxTextCtrl(this, SP_ID_COLORSETTXTCTRL, wxT("CS"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcColorSetTextCtrl, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Type:") ), 0, wxEXPAND | wxALL, 5);
	wxArrayString l_arDataTypes;
	l_arDataTypes.Add(wxT("int"));
	l_arDataTypes.Add(wxT("enum"));
	l_arDataTypes.Add(wxT("string"));
	l_arDataTypes.Add(wxT("bool"));
	m_pcTypeComboBoxCtrl = new wxComboBox(this, SP_ID_TYPECOMBOBOX, wxT("enum"), wxDefaultPosition, wxDefaultSize, l_arDataTypes, wxCB_DROPDOWN);
	l_pcRowSizer->Add(m_pcTypeComboBoxCtrl, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Colors:") ), 0, wxEXPAND | wxALL, 5);
	m_pcColorsTextCtrl = new wxTextCtrl(this, SP_ID_COLORSTXTCTRL, wxT("a-b"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcColorsTextCtrl, 1, wxEXPAND | wxALL, 5);

	l_pcColorSetSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a Variable Name:") ), 0, wxEXPAND | wxALL, 5);
	m_pcVariableTextCtrl = new wxTextCtrl(this, SP_ID_VARIABLETXTCTRL, wxT("x"), wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcVariableTextCtrl, 1, wxEXPAND | wxALL, 5);

	l_pcColorSetSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);

	
	topSizer->Add(l_pcColorSetSizer, 0, wxALL | wxEXPAND, 1);


	if( p_title == wxT("Settings for generating twin nets") )
	{
		wxBoxSizer* l_pcNodesSizer = new wxBoxSizer( wxVERTICAL );

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a Name for the Fusion Place:") ), 0, wxEXPAND | wxALL, 5);
		m_pcFusionPlaceTextCtrl = new wxTextCtrl(this, SP_ID_FUSIONNODETXTCTRL, wxT("Fusion"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_pcFusionPlaceTextCtrl, 1, wxEXPAND | wxALL, 5);
		l_pcNodesSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a Name for the Scheduled Transition:") ), 0, wxEXPAND | wxALL, 5);
		m_pcScheduledTransitionTextCtrl = new wxTextCtrl(this, SP_ID_SCHEDULEDTRANSITIONETXTCTRL, wxT("Schedule"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_pcScheduledTransitionTextCtrl, 1, wxEXPAND | wxALL, 5);
		l_pcNodesSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set a Name for the Coarse Place:") ), 0, wxEXPAND | wxALL, 5);
		m_pcExchangeCoarsePlaceTextCtrl = new wxTextCtrl(this, SP_ID_EXCHANGEPLACETXTCTRL, wxT("Exchange"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_pcExchangeCoarsePlaceTextCtrl, 1, wxEXPAND | wxALL, 5);
		l_pcNodesSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	


		topSizer->Add(l_pcNodesSizer, 0, wxALL | wxEXPAND, 1);



		wxBoxSizer* l_pcNewNodeSizer = new wxBoxSizer( wxVERTICAL );

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set Names for new Places:") ), 0, wxEXPAND | wxALL, 5);
		m_pcNewPlaceTextCtrl = new wxTextCtrl(this, SP_ID_NEWPLACETXTCTRL, wxT("ExP"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_pcNewPlaceTextCtrl, 1, wxEXPAND | wxALL, 5);
		l_pcNewNodeSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1, wxT("Set Names for new Transitions:") ), 0, wxEXPAND | wxALL, 5);
		m_pcNewTransitionTextCtrl = new wxTextCtrl(this, SP_ID_NEWTRANSITIONTXTCTRL, wxT("ExT"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_pcNewTransitionTextCtrl, 1, wxEXPAND | wxALL, 5);
		l_pcNewNodeSizer->Add(l_pcRowSizer, 0, wxALL | wxEXPAND, 1);	

		topSizer->Add(l_pcNewNodeSizer, 0, wxALL | wxEXPAND, 1);


		//Set parameter name and value
		wxBoxSizer* l_pcParaSizer = new wxBoxSizer( wxVERTICAL );

		wxBoxSizer* l_pcParaRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcParaRowSizer->Add(new wxStaticText( this, -1, wxT("Set a parameter Name:") ), 0, wxEXPAND | wxALL, 5);
		m_pcParameterNameTextCtrl = new wxTextCtrl(this, SP_ID_PARANAMETXTCTRL, wxT("k"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcParaRowSizer->Add(m_pcParameterNameTextCtrl, 1, wxEXPAND | wxALL, 5);

		l_pcParaRowSizer->Add(new wxStaticText( this, -1, wxT("Value:") ), 0, wxEXPAND | wxALL, 5);
		m_pcParameterValueTextCtrl = new wxTextCtrl(this, SP_ID_PARAVALUETXTCTRL, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcParaRowSizer->Add(m_pcParameterValueTextCtrl, 1, wxEXPAND | wxALL, 5);

		l_pcParaSizer->Add(l_pcParaRowSizer, 0, wxALL | wxEXPAND, 1);		
		
		topSizer->Add(l_pcParaSizer, 0, wxALL | wxEXPAND, 1);


		//generate side conditions for all transitions
		wxBoxSizer* l_pcSideConditionSizer = new wxBoxSizer( wxVERTICAL );

		wxBoxSizer* l_pcSideConditionRowSizer = new wxBoxSizer( wxHORIZONTAL );
		m_cbGenerateSideConditions = new wxCheckBox(this, SP_ID_GENSIDECONDITIONCHECKLIST, wxT("Generate side conditions for all transitions."));
		m_cbGenerateSideConditions->SetValue(false);
		l_pcSideConditionRowSizer->Add(m_cbGenerateSideConditions, 1, wxEXPAND | wxALL, 5);
		l_pcSideConditionSizer->Add(l_pcSideConditionRowSizer, 0, wxALL | wxEXPAND, 1);		

		topSizer->Add(l_pcSideConditionSizer, 0, wxALL | wxEXPAND, 1);		
	}



	/////////////////////////////////////////////////////////////////////	
	wxBoxSizer* l_pcOKCancelSizer = new wxBoxSizer( wxVERTICAL );
	l_pcOKCancelSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0,wxALL, 5);

	topSizer->Add(l_pcOKCancelSizer, 0, wxALL | wxEXPAND, 1);


	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

SP_DLG_TwinNetSetting::~SP_DLG_TwinNetSetting()
{

}

void SP_DLG_TwinNetSetting::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
	
			// 0 -- color set name
		m_sColorSetName = m_pcColorSetTextCtrl->GetValue();	

		// 1 -- color set type
		m_sType = m_pcTypeComboBoxCtrl->GetValue();		

		// 2 -- color set colors
		m_sColors = m_pcColorsTextCtrl->GetValue();
		

		// 3 -- variable name
		m_sVariableName = m_pcVariableTextCtrl->GetValue();
		

		// 4 -- fusion place name
		if(m_pcFusionPlaceTextCtrl)
			m_sFusionPlaceName = m_pcFusionPlaceTextCtrl->GetValue();
		

		// 5 -- scheduled transition name
		if(m_pcScheduledTransitionTextCtrl)
			m_sScheduleTransitionName = m_pcScheduledTransitionTextCtrl->GetValue();
		

		// 6 -- coarse place name
		if(m_pcExchangeCoarsePlaceTextCtrl)
			m_sCoarsePlaceName = m_pcExchangeCoarsePlaceTextCtrl->GetValue();
		
		// 7 -- new place name
		if(m_pcNewPlaceTextCtrl)
			m_sNewPlaceName = m_pcNewPlaceTextCtrl->GetValue();
		

		// 8 -- new transtion name
		if(m_pcNewTransitionTextCtrl)
			m_sNewTransitionName = m_pcNewTransitionTextCtrl->GetValue();

		// 9 -- Parmeter name
		if(m_pcParameterNameTextCtrl)
			m_sParameterName = m_pcParameterNameTextCtrl->GetValue();

		

		// 10 -- Parameter value
		if(m_pcParameterValueTextCtrl)
			m_sParameterValue = m_pcParameterValueTextCtrl->GetValue();

		//11

		if(m_cbGenerateSideConditions)
			m_bGenerateSideConditions = m_cbGenerateSideConditions->GetValue();
	


		if (IsModal() )
		{
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}
}



void SP_DLG_TwinNetSetting::OnDlgClose(wxCommandEvent& p_cEvent)
{
	SetReturnCode(wxID_CANCEL);
	this->Show( FALSE);
}

void SP_DLG_TwinNetSetting::OnComboBoxSelected(wxCommandEvent& p_cEvent)
{
	wxString l_sSelectedType = m_pcTypeComboBoxCtrl->GetValue();

	if( l_sSelectedType == wxT("int"))
	{
		m_pcColorsTextCtrl->SetValue(wxT("1-2"));		
	}

	if( l_sSelectedType == wxT("enum"))
	{
		m_pcColorsTextCtrl->SetValue(wxT("a-b"));	
	}

	if( l_sSelectedType == wxT("string"))
	{
		m_pcColorsTextCtrl->SetValue(wxT("a-b"));	
	}

	if( l_sSelectedType == wxT("bool"))
	{
		m_pcColorsTextCtrl->SetValue(wxT("true,false"));	
	}

	m_pcColorsTextCtrl->TransferDataToWindow();


}


