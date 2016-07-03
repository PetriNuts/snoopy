/*
 * SP_DLG_SimulatorSetting.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 19.10.2011
 * Short Description:
 *///==============================================================================/

#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_SimulatorSetting.h"
#include "spsim/spsim.h"


BEGIN_EVENT_TABLE(SP_DLG_SimulatorSetting,wxDialog)
EVT_BUTTON(wxID_OK,SP_DLG_SimulatorSetting::OnOK)
END_EVENT_TABLE()

SP_DLG_SimulatorSetting::SP_DLG_SimulatorSetting(wxWindow* p_pcParentWnd,spsa::SteeringClient* p_pcGUIClient,const wxString& p_sSimName)
:wxDialog(p_pcParentWnd,wxID_ANY, wxT("Simulator Setting"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
m_pcGUIClient(p_pcGUIClient),
m_bInitialised(false),
m_sSimulatorName(p_sSimName)
{
	wxSizer* l_pcButtonSizer=new wxBoxSizer(wxHORIZONTAL);

	m_pcSizer=new wxBoxSizer(wxVERTICAL);

	m_pcSimulatorAttribute=new spsa::SimulatorAttribute();

	m_pcSimulatorAttribute->Clear();

	if((m_pcGUIClient->ReceiveSimulatorSetting(p_sSimName,m_pcSimulatorAttribute))!=spsa::SUCCESS)
	{
	  Close();
	  return;
	}

	CreateCtrls();

	m_bInitialised=true;

	m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND );

	l_pcButtonSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND );


	//alignment the sizer
	SetSizerAndFit(m_pcSizer);
	Center();
}

SP_DLG_SimulatorSetting::~SP_DLG_SimulatorSetting()
{
	      wxDELETE(m_pcSimulatorAttribute);
}

bool SP_DLG_SimulatorSetting::CreateCtrls()
{
unsigned int l_nAttributeCount;

long l_nVal=0;

spsa::VectorString::iterator l_itStr;

          if(m_pcSimulatorAttribute==NULL)
          {
        	 return false;
          }

          //get the attribute count
          l_nAttributeCount=m_pcSimulatorAttribute->size();

          m_apcPropertiesCtrl.assign(l_nAttributeCount,NULL);

	        for(unsigned int l_nAttribute=0;l_nAttribute<l_nAttributeCount;l_nAttribute++)
			 {
	        	 wxString l_sName;
	        	 wxString l_sValue;
	        	 unsigned int l_nGUIType;
	        	 wxString l_sCaption;
	        	 double l_nMinValue;
	        	 double l_nMaxValue;


	        	 m_pcSimulatorAttribute->GetAttribute(l_nAttribute,
	        			                              l_sName,
	        			                              l_sCaption,
	        			                              l_sValue,
	        			                              l_nGUIType,
	        			                              l_nMinValue,
	        			                              l_nMaxValue
	        			                             );
	        	 //possible values (for combo box)
	        	 spsa::VectorString l_asPossibleValues=m_pcSimulatorAttribute->GetPossibleValues(l_nAttribute);

	              switch(l_nGUIType)
	              {
	                   case spsim::GUI_TYPE_TXTBOX:
								   m_apcPropertiesCtrl[l_nAttribute]=new wxTextCtrl( this, wxID_ANY,l_sValue, wxDefaultPosition, wxDefaultSize, 0 );
								   break;

	                   case spsim::GUI_TYPE_CHECKBOX:

	                	            m_apcPropertiesCtrl[l_nAttribute]=new wxCheckBox(this,wxID_ANY,wxT(""));
									l_sValue.ToLong(&l_nVal);
									dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nAttribute])->SetValue(l_nVal);
								    break;

	                   case spsim::GUI_TYPE_COMBOBOX:

	                	            m_apcPropertiesCtrl[l_nAttribute]=new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize(100,-1) );
								    dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nAttribute])->Clear();

								     //Add the possible values to this combo box
								    for(l_itStr=l_asPossibleValues.begin();l_itStr!=l_asPossibleValues.end();l_itStr++)
								    {
									   dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nAttribute])->Append((*l_itStr));
								    }

								    dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nAttribute])->SetStringSelection(l_sValue);
								    break;

	                   default:
								   m_apcPropertiesCtrl[l_nAttribute]=new wxTextCtrl( this, -1,l_sValue, wxDefaultPosition, wxDefaultSize, 0 );
								   break;

	              }

	              wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
				  l_pcRowSizer->Add(new wxStaticText( this, wxID_ANY,l_sCaption ), 1, wxALL | wxEXPAND, 5);
				  l_pcRowSizer->Add( m_apcPropertiesCtrl[l_nAttribute], 0, wxALL, 5);
				  m_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);
			 }

return true;
}

void SP_DLG_SimulatorSetting::OnOK(wxCommandEvent& WXUNUSED(event))
{
		if ( Validate() && TransferDataFromWindow() )
		{
			DoOK();
		}

		if(m_pcGUIClient->SendSimulatorSetting(m_sSimulatorName,m_pcSimulatorAttribute)!=spsa::SUCCESS)
		{
			wxMessageBox(wxT("Remote simulator setting could not be saved"));
		}

		EndModal(wxID_OK);
}

void SP_DLG_SimulatorSetting::DoOK()
{
unsigned int l_nAttributeCount;

double l_nVal=0;


	      if(m_pcSimulatorAttribute==NULL) return;

	      //get the attribute count
	      l_nAttributeCount=m_pcSimulatorAttribute->size();


	      for(unsigned int l_nAttribute=0;l_nAttribute<l_nAttributeCount;l_nAttribute++)
		  {
	    	  wxString l_sName;
			  wxString l_sValue;
			  unsigned int l_nGUIType;
			  wxString l_sCaption;
			  double l_nMinValue;
			  double l_nMaxValue;

			  m_pcSimulatorAttribute->GetAttribute(l_nAttribute,
												  l_sName,
												  l_sCaption,
												  l_sValue,
												  l_nGUIType,
												  l_nMinValue,
												  l_nMaxValue
												 );


			   switch(l_nGUIType)
			   {
					case spsim::GUI_TYPE_TXTBOX:

						      l_sValue=dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nAttribute])->GetValue();

						      l_sValue.ToDouble(&l_nVal);

							  if(l_nVal<l_nMinValue || l_nVal>l_nMaxValue)
							  {
								  wxMessageBox(wxT("Invalid input. The value of ")+l_sCaption+
											   wxT(" Should be between ")+ wxString::Format(wxT("%g"),l_nMinValue)
											   + wxT(" and ")+ wxString::Format(wxT("%g"),l_nMinValue));
								  return;
							  }
							  break;

					case spsim::GUI_TYPE_CHECKBOX:

							  l_sValue=wxString::Format(wxT("%d"),dynamic_cast<wxCheckBox*>(m_apcPropertiesCtrl[l_nAttribute])->GetValue());
							  break;
					case spsim::GUI_TYPE_COMBOBOX:

							  l_sValue=dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nAttribute])->GetString(
									  	  dynamic_cast<wxChoice*>(m_apcPropertiesCtrl[l_nAttribute])->GetSelection());
							  break;
					default:

							  l_sValue=dynamic_cast<wxTextCtrl*>(m_apcPropertiesCtrl[l_nAttribute])->GetValue();
							  break;

			   }


			  //update the current value
			  m_pcSimulatorAttribute->UpdateAttribute(l_nAttribute,
													  l_sName,
													  l_sCaption,
													  l_sValue,
													  l_nGUIType,
													  l_nMinValue,
													  l_nMaxValue
													 );

		  }


}
