//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: george assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2021/01/08 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>
#include "sp_gui/dialogs/dia_FPN/SP_DLG_SamplingProperties.h"
//#include "spsim/helpers/simulationOptions.h"
#include "sp_ds/SP_DS_Graph.h"

IMPLEMENT_CLASS(SP_DLG_SamplingProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_SamplingProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_SamplingProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_SamplingProperties::OnDlgCancel)
END_EVENT_TABLE()
//TODO: Clear out all the old code after the end of implementing the old simulator for all the classes
SP_DLG_SamplingProperties::SP_DLG_SamplingProperties(
	wxWindow* p_pcParent,
	fuzzification::FuzzyReasoning* p_fr,
	const int& p_nSamAlg,
	const wxString& p_sTitle,
	long p_nStyle)
	: wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
		p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX
	)
	//m_pcMainSimulator(p_pcMainSimulator)
{
	/* top level sizer */
	m_pcSizer = new wxBoxSizer(wxVERTICAL);
	m_fr = p_fr;
    m_nSmplingAlgo = p_nSamAlg;
	if (m_nSmplingAlgo >1)
	{
		SetProperties();
	}


	/* Buttons in the lower right hand corner */
	m_pcSizer->Add(this->CreateButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);

	SetSizerAndFit(m_pcSizer);

	Centre(wxBOTH);
}

//------------------------------------------------------------------------
void
SP_DLG_SamplingProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow())
	{
		if (m_nSmplingAlgo >1)
		{
			GetProperties();
		}

		if (IsModal())
			EndModal(wxID_OK);
		else
		{
			SetReturnCode(wxID_OK);
			this->Show(FALSE);
		}
	}
}

void
SP_DLG_SamplingProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_SamplingProperties::SetProperties()
{
	wxSizer* l_pcRowSizer;
	wxSizer* l_pcMain;

	if (!m_fr) return;

	fuzzification::SP_SamplingPropertyVector* l_pcPropertyVector = m_fr->GetSamplingOptions();
	m_apcPropertiesCtrl.assign(l_pcPropertyVector->size(), NULL);
	l_pcMain= new wxBoxSizer(wxVERTICAL);
	//l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	fuzzification::SP_SamplingPropertyVector::iterator itProp;
	unsigned l_nPos = 0;
	for (itProp= l_pcPropertyVector->begin();itProp!= l_pcPropertyVector->end();++itProp, l_nPos++)
	{
		switch ((*itProp)->m_tGuiType)
		{
		case fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX:
			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcRowSizer->Add(new wxStaticText(this, -1, (*itProp)->m_sDisplayText), 1, wxALL | wxEXPAND, 5);
			m_apcPropertiesCtrl[l_nPos] = new wxTextCtrl(this, wxID_ANY, (*itProp)->m_sValue, wxDefaultPosition, wxDefaultSize, 0);
			l_pcRowSizer->Add(m_apcPropertiesCtrl[l_nPos], 0, wxALL, 5);
			l_pcMain->Add(l_pcRowSizer);
			break;

		case fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_CHECKBOX:
			l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcRowSizer->Add(new wxStaticText(this, -1, (*itProp)->m_sDisplayText), 1, wxALL | wxEXPAND, 5);
			m_apcPropertiesCtrl[l_nPos] = new  wxCheckBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
			if ((*itProp)->m_sValue != _T("0"))
			{
				///m_apcPropertiesCtrl[l_nPos]->I
				long l_nVal;
				(*itProp)->m_sValue.ToLong(&l_nVal);
				dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nPos])->SetValue(l_nVal);
				 
			}
			l_pcRowSizer->Add(m_apcPropertiesCtrl[l_nPos], 0, wxALL, 5);
			l_pcMain->Add(l_pcRowSizer);
			break;
		}
	}

	/**
	switch (m_nSmplingAlgo)
	{
	case 2:
		m_apcPropertiesCtrl.assign(3, NULL);
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Seed1")), 1, wxALL | wxEXPAND, 5);
		m_apcPropertiesCtrl[0] = new wxTextCtrl(this, wxID_ANY, wxT("3333"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_apcPropertiesCtrl[0], 0, wxALL, 5);
		l_pcMain->Add(l_pcRowSizer);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Seed2")), 1, wxALL | wxEXPAND, 5);
		m_apcPropertiesCtrl[1] = new wxTextCtrl(this, wxID_ANY, wxT("4444"), wxDefaultPosition, wxDefaultSize, 0);
		l_pcRowSizer->Add(m_apcPropertiesCtrl[1], 0, wxALL, 5);
		l_pcMain->Add(l_pcRowSizer);

		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("preserve Draw")), 1, wxALL | wxEXPAND, 5);
		m_apcPropertiesCtrl[2] = new wxCheckBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
		l_pcRowSizer->Add(m_apcPropertiesCtrl[2], 0, wxALL, 5);
		l_pcMain->Add(l_pcRowSizer);
		break;
	case 3:

		break;
	}
	*/
	
	
	
	m_pcSizer->Add(l_pcMain, 1, wxEXPAND | wxALL, 5);
	

}

void SP_DLG_SamplingProperties::GetProperties()
{
	if (!m_fr) return;

	fuzzification::SP_SamplingPropertyVector* l_pcPropertyVector = m_fr->GetSamplingOptions();
	fuzzification::SP_SamplingPropertyVector::iterator l_itProperty;
	unsigned l_nPos = 0;
	double l_nValue = 0;
	wxString l_sUserInput;

	for (l_itProperty = l_pcPropertyVector->begin(); l_itProperty != l_pcPropertyVector->end(); l_itProperty++, l_nPos++)
	{
		switch ((*l_itProperty)->m_tGuiType)
		{
		case fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_TXTBOX:

			l_sUserInput = dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nPos])->GetValue();

			l_sUserInput.ToDouble(&l_nValue);

			if (l_nValue<(*l_itProperty)->m_dMin || l_nValue>(*l_itProperty)->m_dMax && (*l_itProperty)->m_sDisplayText!=wxT("criterium"))
			{
				SP_MESSAGEBOX(wxT("Invalid input. The value of ") + (*l_itProperty)->m_sDisplayText +
					wxT(" Should be between ") + wxString::Format(wxT("%g"), (*l_itProperty)->m_dMin)
					+ wxT(" and ") + wxString::Format(wxT("%g"), (*l_itProperty)->m_dMax));
				return;
			}

			(*l_itProperty)->m_sValue = l_sUserInput;
		 
			break;

		case fuzzification::GUI_PROPORTY_TYPE::GUI_TYPE_CHECKBOX:
			(*l_itProperty)->m_sValue =

				wxString::Format(wxT("%d"), dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nPos])->GetValue());
			
			break;
		}
	}

	/**
	spsim::VectorProperty::iterator l_itProperty;

	spsim::VectorProperty* l_pcProperties = m_pcMainSimulator->GetSimulatorOptions()->GetAllOptions();
	SP_VectorString::const_iterator l_itStr;

	unsigned int l_nPropertyPos = 0;

	double l_nValue = 0;
	wxString l_sUserInput;

	for (l_itProperty = l_pcProperties->begin(); l_itProperty != l_pcProperties->end(); l_itProperty++, l_nPropertyPos++)
	{
		switch ((*l_itProperty)->GetCtrlGuiType())
		{
		case spsim::GUI_TYPE_TXTBOX:

			l_sUserInput = dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetValue();

			l_sUserInput.ToDouble(&l_nValue);

			if (l_nValue<(*l_itProperty)->GetMinValue() || l_nValue>(*l_itProperty)->GetMaxValue())
			{
				SP_MESSAGEBOX(wxT("Invalid input. The value of ") + (*l_itProperty)->GetDisplayedText() +
					wxT(" Should be between ") + wxString::Format(wxT("%g"), (*l_itProperty)->GetMinValue())
					+ wxT(" and ") + wxString::Format(wxT("%g"), (*l_itProperty)->GetMaxValue()));
				return;
			}

			(*l_itProperty)->SetValue
			(
				l_sUserInput
			);
			break;

		case spsim::GUI_TYPE_CHECKBOX:
			(*l_itProperty)->SetValue
			(
				wxString::Format(wxT("%d"), dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetValue())
			);
			break;
		case spsim::GUI_TYPE_COMBOBOX:
			(*l_itProperty)->SetValue
			(
				dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetStringSelection()
			);

			break;
		default:
			(*l_itProperty)->SetValue
			(
				dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetValue()
			);
			break;

		}
	}
	*/
}



