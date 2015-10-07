/*
 * SP_GUI_SteeringOptions.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.03.2011
 * Short Description:
 ==============================================================================*/
#include "spsa/spsa.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringOptions.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

using namespace spsa;

enum
{
	SP_ID_RADIOBOX_MODEL_OPTIONS,
	SP_ID_COMBOBOX_MODEL_NAMES
};

BEGIN_EVENT_TABLE(SP_GUI_SteeringOptions,wxDialog)

 EVT_RADIOBOX(SP_ID_RADIOBOX_MODEL_OPTIONS,SP_GUI_SteeringOptions::OnModelOptionChanged)

 EVT_BUTTON(wxID_OK,SP_GUI_SteeringOptions::OnOkButtonClicked)

 EVT_COMBOBOX(SP_ID_COMBOBOX_MODEL_NAMES, SP_GUI_SteeringOptions::OnExistingModelBoxDropDown)

END_EVENT_TABLE()


SP_GUI_SteeringOptions::SP_GUI_SteeringOptions(wxWindow* p_pcParent,SteeringClient* p_pcGUIClient,
		                                       const bool& p_bShowTwoOptions,const wxString& p_sTitle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP),
 m_pcGUIClient(p_pcGUIClient),
 m_bShowTwoOptions(p_bShowTwoOptions)
{
	//Main Sizer
	wxBoxSizer* l_pcSizer = new wxBoxSizer( wxVERTICAL );
	if(m_bShowTwoOptions==true)
	{

		wxBoxSizer* l_pcTopSizer = new wxBoxSizer( wxHORIZONTAL);
		wxBoxSizer* l_pcBottomSizer=new wxBoxSizer( wxHORIZONTAL);
		l_pcSizer->Add(l_pcTopSizer, 0, wxEXPAND | wxALIGN_CENTER);
		l_pcSizer->Add(l_pcBottomSizer, 0, wxEXPAND | wxALIGN_CENTER);
		wxBoxSizer* l_pcLeftSizer=new wxBoxSizer( wxVERTICAL );
		wxBoxSizer* l_pcRightSizer=new wxBoxSizer( wxVERTICAL );

		l_pcBottomSizer->Add(l_pcLeftSizer, 0, wxEXPAND | wxALIGN_CENTER);
		l_pcBottomSizer->Add(l_pcRightSizer, 0, wxEXPAND | wxALIGN_CENTER);


		wxString l_sChoices[]={wxT("Send a New Model"),wxT("Use an Existing Model")};
		m_pcModelOptions=new wxRadioBox(this,SP_ID_RADIOBOX_MODEL_OPTIONS, wxT(""),wxDefaultPosition, wxDefaultSize, 2, l_sChoices, 2, wxRA_SPECIFY_COLS);
		l_pcTopSizer->Add(m_pcModelOptions, 1, wxEXPAND | wxALL, 5);

		m_pcExistingModelBox=new wxComboBox( this, SP_ID_COMBOBOX_MODEL_NAMES, wxT(""), wxDefaultPosition, wxSize(200,-1), 0, NULL, wxCB_READONLY );

		//Add the model names to the list box
		UpdateModelnames();

		//m_pcExistingModelBox->SetSelection(0);
		m_pcExistingModelBox->Enable(false);

		l_pcRightSizer->Add(new wxStaticText( this, -1, wxT("Running Models")), 1, wxEXPAND | wxALL, 5);
		l_pcRightSizer->Add(m_pcExistingModelBox, 1, wxEXPAND | wxALL, 5);


		l_pcLeftSizer->Add(new wxStaticText( this, wxID_ANY, wxT("Model Name")), 1, wxEXPAND | wxALL, 5);

		wxString l_sDefaultName=SP_Core::Instance()->GetRootDocument()->GetFilename();
#if __WXMSW__
		   l_sDefaultName=l_sDefaultName.AfterLast(wxT('\\'));
#else             
		   l_sDefaultName=l_sDefaultName.AfterLast(wxT('/'));
#endif

		m_pcModelNameTextCtrl=new wxTextCtrl( this,wxID_ANY,l_sDefaultName,wxDefaultPosition, wxSize(200,-1));

		l_pcLeftSizer->Add(m_pcModelNameTextCtrl, 1, wxEXPAND | wxALL, 5);
	}
	else
	{
		wxBoxSizer* l_pcBottomSizer=new wxBoxSizer( wxHORIZONTAL );
		l_pcSizer->Add(l_pcBottomSizer, 0, wxEXPAND | wxALIGN_CENTER);

		m_pcExistingModelBox=new wxComboBox( this, SP_ID_COMBOBOX_MODEL_NAMES, wxT(""), wxDefaultPosition, wxSize(200,-1), 0, NULL, wxCB_READONLY );

		//Add the model names to the list box
		UpdateModelnames();

		//m_pcExistingModelBox->SetSelection(0);
		m_pcExistingModelBox->Enable(true);

		l_pcBottomSizer->Add(new wxStaticText( this, -1, wxT("Running Models")), 0, wxEXPAND | wxALL, 5);
		l_pcBottomSizer->Add(m_pcExistingModelBox, 0, wxEXPAND | wxALL, 5);

		l_pcSizer->AddSpacer(20);
	}

	//add a button sizer with ok and cancel
	l_pcSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALIGN_CENTER);

	//Set the final alignment
	SetSizerAndFit(l_pcSizer);
	Centre(wxBOTH);
}

SP_GUI_SteeringOptions::~SP_GUI_SteeringOptions()
{

}

void SP_GUI_SteeringOptions::OnModelOptionChanged(wxCommandEvent& event)
{
	int l_nOption=m_pcModelOptions->GetSelection();

	      switch(l_nOption)
	      {
	        case 0:
	        	    m_pcExistingModelBox->Enable(false);
	        	    m_pcModelNameTextCtrl->Enable(true);
	        	    break;
	        case 1:
	        	   m_pcExistingModelBox->Enable(true);
	        	   m_pcModelNameTextCtrl->Enable(false);
	        	   if(m_pcExistingModelBox->GetCount()==0)
	        	   {
	        		   m_pcModelOptions->SetSelection(0);
	        	   }
	        	   UpdateModelnames();
	        	   break;
	      }

}

void SP_GUI_SteeringOptions::OnOkButtonClicked(wxCommandEvent& event)
{
	if(m_bShowTwoOptions==true)
	{
		if(m_pcModelNameTextCtrl->IsEmpty() && m_pcModelOptions->GetSelection()==0)
		{
			SP_MESSAGEBOX(wxT("Model's Name can't be empty"));
			return;
		}

		wxString l_sModelName=m_pcModelNameTextCtrl->GetValue();

		//Check if this name is already exist
		if(m_pcModelOptions->GetSelection()==0 && IsModelNameExist(l_sModelName))
		{
			int l_nAnswer=SP_MESSAGEBOX(wxT("A model with this name is already exist, Do you want to overwrite it?"),
																						wxT("Steering client"),
																						wxYES_NO,
																						this);
			if(l_nAnswer!=wxYES)
			{
				return;
			}
			else
			{
				int l_nReply=0;

				//delete existing model
				int l_nReturn=m_pcGUIClient->RemoveModel(l_sModelName,l_nReply);

				if(l_nReturn!=SUCCESS || l_nReply==0)
				{
					SP_MESSAGEBOX(wxT("This model can not be overwritten, It might be used by another user"));

					return;
				}
			}

		}
	}

	  //Return ID Ok
	  EndModal(wxID_OK);

}

bool SP_GUI_SteeringOptions::IsModelNameExist(const wxString& p_sModelName)
{
	VectorString l_sModelNames;
	VectorString::const_iterator l_itName;
	bool l_bNameExist=false;

	  //Get the Model names
	  m_pcGUIClient->GetRuningModelNames(l_sModelNames);

	  for(l_itName=l_sModelNames.begin();l_itName!=l_sModelNames.end();l_itName++)
	 {

		if((*l_itName)==p_sModelName)
		{
			l_bNameExist=true;

		}

	 }

	return l_bNameExist;
}

void SP_GUI_SteeringOptions::UpdateModelnames()
{
	VectorString l_sModelNames;
	VectorString::const_iterator l_itName;
	int l_nCurrentSelection;


	//Get the Model names
	m_pcGUIClient->GetRuningModelNames(l_sModelNames);

	l_nCurrentSelection=m_pcExistingModelBox->GetCurrentSelection();

	m_pcExistingModelBox->Clear();
	for(l_itName=l_sModelNames.begin();l_itName!=l_sModelNames.end();l_itName++)
	{
		m_pcExistingModelBox->Append((*l_itName));
	}

	m_pcExistingModelBox->SetSelection(l_nCurrentSelection);
}

void SP_GUI_SteeringOptions::OnExistingModelBoxDropDown(wxCommandEvent& event)
{
	//TODO: When wxWidget will be updated to version 2.9, use wxEVT_COMMAND_COMBOBOX_DROPDOWN event type
	if(event.GetEventType()==wxEVT_ENTER_WINDOW)
	{
		UpdateModelnames();
	}
}
