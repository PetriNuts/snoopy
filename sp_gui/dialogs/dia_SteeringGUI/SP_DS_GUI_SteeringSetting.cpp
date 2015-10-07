/*
 * SP_DS_GUI_SteeringSetting.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 24.06.2011
 * Short Description:
 *///=================================================

#include "sp_gui/dialogs/dia_SteeringGUI/SP_DS_GUI_SteeringSetting.h"
#include "sp_utilities.h"

BEGIN_EVENT_TABLE(SP_DS_GUI_SteeringSetting,wxDialog)
EVT_BUTTON(wxID_OK,SP_DS_GUI_SteeringSetting::OnOK)
END_EVENT_TABLE();

SP_DS_GUI_SteeringSetting::SP_DS_GUI_SteeringSetting(wxWindow* p_pcParent)
:wxDialog(p_pcParent, -1, wxT("Setting"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER)
{
	      wxSizer* l_pcMainSizer=new wxBoxSizer(wxVERTICAL);

		  wxSizer* l_pcTopSizer=new wxBoxSizer(wxVERTICAL);
		  wxSizer* l_pcBottomSizer=new wxBoxSizer(wxVERTICAL);
		  l_pcMainSizer->Add(l_pcTopSizer, 1, wxEXPAND);
		  l_pcMainSizer->Add(l_pcBottomSizer, 0, wxEXPAND);

		  //runtime
		  wxBoxSizer* l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("Runtime statistics") ), wxVERTICAL);
		  wxBoxSizer* l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);

		  m_pcRefreshRuntimeChkBox=new wxCheckBox(this,wxID_ANY,wxT("Refresh periodically"));
		  m_pcRefreshRuntimeChkBox->SetValue(m_bRefreshRuntime);
		  l_pcTempSizer->Add(m_pcRefreshRuntimeChkBox, 0, wxEXPAND | wxALIGN_CENTER);
		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);

		  l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);

		  l_pcTempSizer->Add(new wxStaticText( this,wxID_ANY, wxT("Refresh:") ),1, wxEXPAND | wxALL);
		  m_pcRefreshRuntimeSlider=new wxSlider( this,wxID_ANY,m_nRefreshRuntimeDuration,1,500,
	        		                            wxDefaultPosition,wxDefaultSize,wxSL_HORIZONTAL|wxSL_LABELS);
		  l_pcTempSizer->Add(m_pcRefreshRuntimeSlider, 0, wxEXPAND | wxALIGN_CENTER);

		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);
		  l_pcTopSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER);

		  //results
		  l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("Results") ), wxVERTICAL);

		  l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);
		  m_pcRefreshResultChkBox=new wxCheckBox(this,wxID_ANY,wxT("Refresh periodically"));
		  m_pcRefreshResultChkBox->SetValue(m_bRefreshResult);

		  l_pcTempSizer->Add(m_pcRefreshResultChkBox, 0, wxEXPAND | wxALIGN_CENTER);
		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);

		  l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);

		  l_pcTempSizer->Add(new wxStaticText( this,wxID_ANY, wxT("Refresh:") ),1, wxEXPAND | wxALL);
		  m_pcRefreshResultSlider=new wxSlider( this,wxID_ANY,m_nRefreshResultDuration,1,500,
												wxDefaultPosition,wxDefaultSize,wxSL_HORIZONTAL|wxSL_LABELS);
		  l_pcTempSizer->Add(m_pcRefreshResultSlider, 0, wxALL);

		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);
		  l_pcTopSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER);


		  //time out
		  l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("Timeout") ), wxHORIZONTAL);
		  l_pcRowSizer->Add(new wxStaticText( this,wxID_ANY, wxT("Operation Timeout(s):") ),1, wxEXPAND | wxALL);
		  m_pcTimeoutSlider=new wxSlider( this,wxID_ANY,m_nTimeout,5,500,
	        		                            wxDefaultPosition,wxDefaultSize,wxSL_HORIZONTAL|wxSL_LABELS);
		  l_pcRowSizer->Add(m_pcTimeoutSlider, 0, wxALL);
		  l_pcTopSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER);

		  //enable refresh calculation time
		  l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("General") ), wxVERTICAL);

		  l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);

		  m_pcEnableRefreshCalculationTimeChkBox=new wxCheckBox(this,wxID_ANY,wxT("Auto adjust the result's refresh time"));
		  m_pcEnableRefreshCalculationTimeChkBox->SetValue(m_bEnableRefreshCalculationTime);
		  l_pcTempSizer->Add(m_pcEnableRefreshCalculationTimeChkBox, 0, wxALL);
		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);

		  l_pcRowSizer->AddSpacer(10);

		  l_pcTempSizer= new wxBoxSizer(wxHORIZONTAL);
		  l_pcTempSizer->Add(new wxStaticText( this,wxID_ANY, wxT("Spin tick value:") ),1, wxEXPAND | wxALL);
		  m_pcSteeringConstant=new wxTextCtrl(this,wxID_ANY,wxString::Format(wxT("%g"),m_nSteeringConstant));
		  l_pcTempSizer->Add(m_pcSteeringConstant, 0, wxEXPAND | wxALIGN_CENTER);

		  l_pcRowSizer->Add(l_pcTempSizer,1, wxEXPAND | wxALIGN_CENTER);


		  l_pcTopSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER);


		  l_pcBottomSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALIGN_CENTER);

		  //alignment the sizer
		  SetSizerAndFit(l_pcMainSizer);
		  Center();
}

SP_DS_GUI_SteeringSetting::~SP_DS_GUI_SteeringSetting()
{

}
void SP_DS_GUI_SteeringSetting::OnOK(wxCommandEvent& WXUNUSED(event))
{
	      
	      m_bRefreshResult=m_pcRefreshResultChkBox->GetValue();

	      m_bRefreshRuntime=m_pcRefreshRuntimeChkBox->GetValue();

	      m_nRefreshResultDuration=m_pcRefreshResultSlider->GetValue();

		  m_nRefreshRuntimeDuration=m_pcRefreshRuntimeSlider->GetValue();

	      m_nTimeout=m_pcTimeoutSlider->GetValue();

	      m_bEnableRefreshCalculationTime=m_pcEnableRefreshCalculationTimeChkBox->GetValue();

	      wxString l_sSpinConstant=m_pcSteeringConstant->GetValue();

	      l_sSpinConstant.ToDouble(&m_nSteeringConstant);

		  EndModal(wxID_OK);
}

void SP_DS_GUI_SteeringSetting::Update()
{
	    m_pcRefreshResultChkBox->SetValue(m_bRefreshResult);

	    m_pcRefreshRuntimeChkBox->SetValue(m_bRefreshRuntime);

	    m_pcRefreshResultSlider->SetValue(m_nRefreshResultDuration);

	    m_pcRefreshRuntimeSlider->SetValue(m_nRefreshRuntimeDuration);

	    m_pcEnableRefreshCalculationTimeChkBox->SetValue(m_bEnableRefreshCalculationTime);

	    m_pcSteeringConstant->SetValue(wxString::Format(wxT("%g"),m_nSteeringConstant));

	    m_pcTimeoutSlider->SetValue(m_nTimeout);
}
