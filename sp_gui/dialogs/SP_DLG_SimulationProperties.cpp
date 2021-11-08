//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "spsim/helpers/simulationOptions.h"
#include "sp_ds/SP_DS_Graph.h"

IMPLEMENT_CLASS(SP_DLG_SimulationProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_SimulationProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_SimulationProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_SimulationProperties::OnDlgCancel)
END_EVENT_TABLE()
//TODO: Clear out all the old code after the end of implementing the old simulator for all the classes
SP_DLG_SimulationProperties::SP_DLG_SimulationProperties(spsim::Simulator* p_pcMainSimulator,
                                                       wxWindow* p_pcParent,
	                                                   const int& p_nSimAlg,
                                                       const wxString& p_sTitle,
                                                       long p_nStyle)
  : wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, 
	     p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
    m_pcMainSimulator(p_pcMainSimulator)
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);
	m_nSimAlgo = p_nSimAlg;
    if(m_pcMainSimulator!=NULL)
    {
    	SetProperties();
    }

	
    /* Buttons in the lower right hand corner */
	m_pcSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

    SetSizerAndFit(m_pcSizer);
    
    Centre(wxBOTH);
}

//------------------------------------------------------------------------
void
SP_DLG_SimulationProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if(m_pcMainSimulator!=NULL)
        {
        	GetProperties();
        }

        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
		    SetReturnCode(wxID_OK);
		    this->Show(FALSE);
        }
	}
}

void
SP_DLG_SimulationProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
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

void SP_DLG_SimulationProperties::SetProperties()
{
	spsim::VectorProperty::iterator l_itProperty;

	spsim::VectorProperty* l_pcProperties=m_pcMainSimulator->GetSimulatorOptions()->GetAllOptions();
	SP_VectorStdString::const_iterator l_itStr;

	unsigned int l_nPropertyPos=0;
	wxSizer* l_pcRowSizer;
	long l_nVal=0;
	wxString l_sClassSim;
	l_sClassSim=m_pcMainSimulator->GetSimulatorClass();
	if (l_sClassSim.Contains("Stochastic"))
	{
		unsigned l_size = l_pcProperties->size();
		if (l_size< 4 && m_nSimAlgo==0)
		{
			m_pcMainSimulator->GetSimulatorOptions()->AddNewOption(new spsim::Property("Refreshrate", "5000", "Refresh rate(ms)", spsim::GUI_TYPE_TXTBOX));
			m_pcMainSimulator->GetSimulatorOptions()->AddNewOption(new spsim::Property("In Between Visualization", "1", "Visualize in between results", spsim::GUI_TYPE_CHECKBOX));
		}
	}
	m_apcPropertiesCtrl.assign(l_pcProperties->size(),NULL);

	for(l_itProperty=l_pcProperties->begin();l_itProperty!=l_pcProperties->end();l_itProperty++,l_nPropertyPos++)
	{
		switch((*l_itProperty)->GetCtrlGuiType())
		{
		case spsim::GUI_TYPE_TXTBOX:
			  m_apcPropertiesCtrl[l_nPropertyPos]=new wxTextCtrl( this, wxID_ANY,(*l_itProperty)->GetValue(), wxDefaultPosition, wxDefaultSize, 0 );
			  break;

		case spsim::GUI_TYPE_CHECKBOX:
			   m_apcPropertiesCtrl[l_nPropertyPos]=new wxCheckBox(this,wxID_ANY,wxT(""), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
			  // (*l_itProperty)->GetValue().ToLong(&l_nVal);
			   l_nVal=std::stol((*l_itProperty)->GetValue());
			   dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nPropertyPos])->SetValue(l_nVal);
			   break;

		case spsim::GUI_TYPE_COMBOBOX:
			  m_apcPropertiesCtrl[l_nPropertyPos]=new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize(100,-1) );
			  dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nPropertyPos])->Clear();

			  //Add the possible values to this combo box
			  for(l_itStr=(*l_itProperty)->GetPossibleValues()->begin();l_itStr!=(*l_itProperty)->GetPossibleValues()->end();l_itStr++)
			  {
				   dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nPropertyPos])->Append((*l_itStr));
			  }

			  dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nPropertyPos])->SetStringSelection((*l_itProperty)->GetValue());
			  break;

		default:
			  m_apcPropertiesCtrl[l_nPropertyPos]=new wxTextCtrl( this, -1,(*l_itProperty)->GetValue(), wxDefaultPosition, wxDefaultSize, 0 );
			  break;

		}

		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add(new wxStaticText( this, -1,(*l_itProperty)->GetDisplayedText() ), 1, wxALL | wxEXPAND, 5);
		l_pcRowSizer->Add( m_apcPropertiesCtrl[l_nPropertyPos], 0, wxALL, 5);
		m_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);
	}
}

void SP_DLG_SimulationProperties::GetProperties()
{
	spsim::VectorProperty::iterator l_itProperty;

	spsim::VectorProperty* l_pcProperties=m_pcMainSimulator->GetSimulatorOptions()->GetAllOptions();
	SP_VectorString::const_iterator l_itStr;

	unsigned int l_nPropertyPos=0;

	double l_nValue=0;
	wxString l_sUserInput;

	for(l_itProperty=l_pcProperties->begin();l_itProperty!=l_pcProperties->end();l_itProperty++,l_nPropertyPos++)
	{
		switch((*l_itProperty)->GetCtrlGuiType())
		{
		case spsim::GUI_TYPE_TXTBOX:

			  l_sUserInput=dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetValue();

			  l_sUserInput.ToDouble(&l_nValue);

			  if(l_nValue<(*l_itProperty)->GetMinValue() || l_nValue>(*l_itProperty)->GetMaxValue())
			  {
				  SP_MESSAGEBOX(wxT("Invalid input. The value of ")+(*l_itProperty)->GetDisplayedText()+
							   wxT(" Should be between ")+ wxString::Format(wxT("%g"),(*l_itProperty)->GetMinValue())
							   + wxT(" and ")+ wxString::Format(wxT("%g"),(*l_itProperty)->GetMaxValue()));
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
				wxString::Format(wxT("%d"),dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nPropertyPos])->GetValue())
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
}


