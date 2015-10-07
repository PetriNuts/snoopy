//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2011/08/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>
#include <wx/regex.h>
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_PlacesSelection.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

IMPLEMENT_CLASS( SP_DLG_PlacesSelection, wxDialog )

enum
{
	SP_ID_LISTBOX_IN,
	SP_ID_BUTTON_LF2RT,
	SP_ID_BUTTON_RT2LF,
	SP_ID_BUTTON_LF2RTALL,
	SP_ID_BUTTON_RT2LFALL,

	SP_ID_LISTBOX_OUT,
	SP_ID_BUTTON_ADD,
	
	SP_ID_COMBOBOX_COLORPLACE_CHOICE,
	SP_ID_BUTTON_EDITAUXVAR,

	SP_ID_PLACETRANSITIONRADIOBOX,
	SP_ID_OUTPUTTYPERADIOBOX,
	SP_ID_CHECKLISTBOX_PLACE_CHOICE,
	SP_ID_BUTTON_REGEX_COMPILE_SELECT,
	SP_ID_CHECKBOX_REGEX_INVERT_COMPILE,
	SP_ID_CHECKBOX_NODE_COLOUR
};

BEGIN_EVENT_TABLE( SP_DLG_PlacesSelection, wxDialog )

	EVT_BUTTON( wxID_OK, SP_DLG_PlacesSelection::OnDlgOk )
	EVT_BUTTON( wxID_CANCEL, SP_DLG_PlacesSelection::OnDlgCancel )
	
	EVT_BUTTON(SP_ID_BUTTON_LF2RT, SP_DLG_PlacesSelection::OnLeftToRight)
	EVT_BUTTON(SP_ID_BUTTON_RT2LF, SP_DLG_PlacesSelection::OnRightToLeft)

	EVT_BUTTON(SP_ID_BUTTON_LF2RTALL, SP_DLG_PlacesSelection::OnLeftToRightAll)
	EVT_BUTTON(SP_ID_BUTTON_RT2LFALL, SP_DLG_PlacesSelection::OnRightToLeftAll)

	EVT_TEXT(SP_ID_BUTTON_REGEX_COMPILE_SELECT, SP_DLG_PlacesSelection::OnRegexSelection)
	EVT_CHECKBOX(SP_ID_CHECKBOX_REGEX_INVERT_COMPILE, SP_DLG_PlacesSelection::OnRegexSelection)

	EVT_BUTTON(SP_ID_BUTTON_EDITAUXVAR, SP_DLG_PlacesSelection::OnEditAuxVariable)

	EVT_COMBOBOX(SP_ID_COMBOBOX_COLORPLACE_CHOICE, SP_DLG_PlacesSelection::OnColPlaceSel)

	EVT_RADIOBOX( SP_ID_PLACETRANSITIONRADIOBOX, SP_DLG_PlacesSelection::OnChangedPlaceTransition )
	EVT_RADIOBOX( SP_ID_OUTPUTTYPERADIOBOX, SP_DLG_PlacesSelection::OnChangedOutType )

END_EVENT_TABLE()



SP_DLG_PlacesSelection::SP_DLG_PlacesSelection(SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,
		const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 1500, 600), p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
		 m_sPlaceTransition(wxT("Place")),
		 m_bComSingleRun(false),
         m_pcEditMetadata(p_pcEditMetadata),
		 m_sOutputType(wxT("Unfolded"))
{
	m_sNodeType = m_pcEditMetadata->GetAttribute(wxT("Nodeclass"))->GetValueString();
	SP_DS_Attribute* l_pcAttr =	m_pcEditMetadata->GetAttribute(wxT("OutputType"));
	if(l_pcAttr)
	{
		m_sOutputType = l_pcAttr->GetValueString();
	}

	SetCommonLayout1();

	Initialize();	

	InitilizeFromMetaData();

	LoadData();

	Layout();	

}

void SP_DLG_PlacesSelection::SetCommonLayout1()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!m_pcGraph)
		return;
	wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();	

	m_pcMainSizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* l_pcMainContentSizer = new wxBoxSizer(wxHORIZONTAL);

	wxSizer* l_pcControlButtonSizer = new wxBoxSizer(wxHORIZONTAL);  //for save and cancel buttons

	m_pcMainSizer->Add(l_pcMainContentSizer, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer->Add(l_pcControlButtonSizer, 0, wxALL | wxEXPAND, 5);

	m_pcPlaceChoiceStaticBox = new wxStaticBox(this, -1,  wxT("The overall ")+ m_sNodeType + wxT("s"));           //the second column

	wxSizer* l_pcPlaceChoiceSizer = new wxStaticBoxSizer(m_pcPlaceChoiceStaticBox, wxVERTICAL);

	wxSizer* l_pcThirdColumSizer = new wxBoxSizer(wxVERTICAL);

	m_pcFourthColumStaticBox = new wxStaticBox(this, -1,  wxT("The selected ")+ m_sNodeType + wxT("s") );           //the fourth column, selected places
	wxSizer* l_pcFourthColumSizer = new wxStaticBoxSizer(m_pcFourthColumStaticBox, wxVERTICAL);

	wxSizer* l_pcLeftContentSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Properties:")), wxVERTICAL);  //the first column
	wxSizer* l_pcRightContentSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Selection:")), wxVERTICAL);

	l_pcMainContentSizer->Add(l_pcLeftContentSizer, 0, wxALL | wxEXPAND, 5);
	l_pcMainContentSizer->Add(l_pcRightContentSizer, 1, wxALL | wxEXPAND, 5);

	wxSizer* l_pcTopContentSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcBottomContentSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRightContentSizer->Add(l_pcTopContentSizer, 1, wxLEFT | wxEXPAND, 5);
	l_pcRightContentSizer->Add(l_pcBottomContentSizer, 7, wxALL | wxEXPAND, 5);

	l_pcBottomContentSizer->Add(l_pcPlaceChoiceSizer, 1, wxALL | wxEXPAND, 5);
	l_pcBottomContentSizer->Add(l_pcThirdColumSizer);
	l_pcBottomContentSizer->Add(l_pcFourthColumSizer, 1, wxALL | wxEXPAND, 5);

	wxSizer* l_pcRowSizer;

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Name:")), 0, wxALL, 5);
	m_pcNameTextCtrl = new wxTextCtrl(this, -1, m_sPlotName, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcNameTextCtrl, 1, wxALL, 5);
	l_pcLeftContentSizer->Add(l_pcRowSizer);


	//node class
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_asPlaceTransitionChoices[] = { wxT("Place"), wxT("Transition")};
	m_pcPlaceTransitionRadioBox = new wxRadioBox(this, SP_ID_PLACETRANSITIONRADIOBOX, wxT("Observers"), wxDefaultPosition, wxDefaultSize, 2, l_asPlaceTransitionChoices, 2, wxRA_SPECIFY_ROWS | wxEXPAND);
	l_pcRowSizer->Add(m_pcPlaceTransitionRadioBox, 1, wxALL | wxEXPAND, 5);

	l_pcLeftContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	if(m_sNodeType == wxT("Place"))
	{
		m_pcPlaceTransitionRadioBox->SetSelection(0);
	}
	else
	{
		m_pcPlaceTransitionRadioBox->SetSelection(1);
	}

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Colour")), wxHORIZONTAL);
	m_pcPlaceChoiceColour = new wxCheckBox(this, SP_ID_CHECKBOX_NODE_COLOUR, wxT("Use node colour"));
	l_pcRowSizer->Add(m_pcPlaceChoiceColour, 1, wxALL | wxEXPAND, 5);
	l_pcLeftContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("RegEx")), wxHORIZONTAL);
	m_pcPlaceChoiceRegexInvert = new wxCheckBox(this, SP_ID_CHECKBOX_REGEX_INVERT_COMPILE, wxT("Invert"));
	l_pcRowSizer->Add(m_pcPlaceChoiceRegexInvert, 1, wxALL | wxEXPAND, 5);
	l_pcLeftContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	if(l_sNetClass.Contains(wxT("Colored")))
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		wxString l_asOutputTypeChoices[] = { wxT("Unfolded"), wxT("Colored"), wxT("Auxiliary variables")};
		m_pcOutputTypeRadioBox = new wxRadioBox(this, SP_ID_OUTPUTTYPERADIOBOX, wxT("OutType"), wxDefaultPosition, wxDefaultSize, 3, l_asOutputTypeChoices, 3, wxRA_SPECIFY_ROWS | wxEXPAND);
		if(m_sNodeType == wxT("Transition"))
		{
			m_pcOutputTypeRadioBox->Show(2,false);
		}
		if(m_sOutputType == wxT("Unfolded"))
		{
			m_pcOutputTypeRadioBox->SetSelection(0);
		}
		else if(m_sOutputType == wxT("Colored"))
		{
			m_pcOutputTypeRadioBox->SetSelection(1);
		}
		else
		{
			m_pcOutputTypeRadioBox->SetSelection(2);
		}
		l_pcRowSizer->Add(m_pcOutputTypeRadioBox, 1, wxALL | wxEXPAND, 5);
		l_pcLeftContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxButton( this, SP_ID_BUTTON_EDITAUXVAR, wxT( "Edit Auxilary" ) ), 0, wxALL | wxEXPAND, 5);
		l_pcLeftContentSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	}

	/**************top row****************/

	l_pcRowSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("RegEx")), wxHORIZONTAL);
	m_pcPlaceChoiceOutRegex = new wxTextCtrl(this, SP_ID_BUTTON_REGEX_COMPILE_SELECT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	l_pcRowSizer->Add(m_pcPlaceChoiceOutRegex, 1, wxALL | wxEXPAND, 5);
	l_pcTopContentSizer->Add(l_pcRowSizer, 1, wxALL | wxEXPAND, 5);

	/**********************the second column**************************/

	wxSizer* l_pcWholePlaceSizer = new wxBoxSizer(wxHORIZONTAL | wxVERTICAL);
	m_pListBoxIn = new wxListBox(this, SP_ID_LISTBOX_IN, wxDefaultPosition,wxSize( 150, 400 ),0,NULL,wxLB_EXTENDED);
	l_pcWholePlaceSizer->Add(m_pListBoxIn, 1, wxALL | wxEXPAND, 5);

	l_pcPlaceChoiceSizer->Add(l_pcWholePlaceSizer, 1, wxALL | wxEXPAND);


	/*****************the third column***************************/
	wxBoxSizer* l_pcSizerColumn3 = new wxBoxSizer(wxVERTICAL);
	wxButton* l_pButton1=new wxButton( this, SP_ID_BUTTON_LF2RT, wxT( ">" ) );
	wxButton* l_pButton2=new wxButton( this, SP_ID_BUTTON_RT2LF, wxT( "<" ) );
	wxButton* l_pButton3=new wxButton( this, SP_ID_BUTTON_LF2RTALL, wxT( ">>" ) );
	wxButton* l_pButton4=new wxButton( this, SP_ID_BUTTON_RT2LFALL, wxT( "<<" ) );

	l_pcSizerColumn3->Add(l_pButton1, 0,  wxALIGN_CENTER| wxTOP, 25);
	l_pcSizerColumn3->Add(l_pButton2, 0, wxALIGN_CENTER| wxALL, 5);
	l_pcSizerColumn3->Add(l_pButton3, 0,  wxALIGN_CENTER| wxALL, 5);
	l_pcSizerColumn3->Add(l_pButton4, 0, wxALIGN_CENTER| wxALL, 5);
	//advanced selection
	if(l_sNetClass.Contains(wxT("Colored")))
	{
		l_pcRowSizer = new wxBoxSizer( wxVERTICAL );
		l_pcRowSizer->Add( new wxStaticText( this, -1, wxT("") ), 1, wxALL | wxEXPAND, 5 );
		l_pcRowSizer->Add( new wxStaticText( this, -1, wxT("Advanced:") ), 1, wxALL | wxEXPAND, 5 );
		m_pcChooseColoredPlace = new wxComboBox( this, SP_ID_COMBOBOX_COLORPLACE_CHOICE, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY);
		l_pcRowSizer->Add(m_pcChooseColoredPlace, 0, wxALIGN_CENTER| wxALL, 5);
		m_pcChooseColoredPlace->Append(wxT("")); //

		l_pcSizerColumn3->Add(l_pcRowSizer, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL);
	}	
	l_pcThirdColumSizer->Add(l_pcSizerColumn3, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL);


	/***************************the fourth column**************************/
	wxSizer* l_pcSelectedPlaceSizer = new wxBoxSizer(wxHORIZONTAL | wxVERTICAL);
	m_pListBoxOut = new wxListBox(this, SP_ID_LISTBOX_OUT,wxDefaultPosition,wxSize( 150, 400 ),0,NULL,wxLB_EXTENDED);
	l_pcSelectedPlaceSizer->Add(m_pListBoxOut, 1, wxALL | wxEXPAND, 5);
	l_pcFourthColumSizer->Add(l_pcSelectedPlaceSizer, 1, wxALL | wxEXPAND);


	//the save or close button
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxButton(this, wxID_OK, wxT("Save")), 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 1, wxALL, 5);

	l_pcControlButtonSizer->Add(l_pcRowSizer);	

	SetSizerAndFit(m_pcMainSizer);

}

void SP_DLG_PlacesSelection::OnRegexSelection(wxCommandEvent& p_cEvent )
{
	NodesThroughRegEx();
}

void SP_DLG_PlacesSelection::NodesThroughRegEx()
{
	wxString l_nRegexString = m_pcPlaceChoiceOutRegex->GetValue();

	if(l_nRegexString.IsEmpty()) return;

	bool l_bRegexInvert	 = m_pcPlaceChoiceRegexInvert->GetValue();

	wxArrayString l_placeOut, l_placeIn;

	wxRegEx l_nRegex;

	if (l_nRegex.Compile(l_nRegexString, wxRE_DEFAULT))
	{
		for (int i = 0; i < m_pListBoxIn->GetCount(); i++)
		{
			wxString l_tempString = m_pListBoxIn->GetString(i);
			bool match = l_nRegex.Matches(l_tempString);
			if(l_bRegexInvert)
			{
				match = !match;
			}
			if (match)
			{
				l_placeOut.Add(l_tempString, 1);
				RemovefromCurLBInStrings(l_tempString);
			}
			else
			{
				l_placeIn.Add(l_tempString, 1);
			}
		}

		for (int i = 0; i < m_pListBoxOut->GetCount(); i++)
		{
			wxString l_tempString = m_pListBoxOut->GetString(i);

			bool match = l_nRegex.Matches(l_tempString);
			if(l_bRegexInvert)
			{
				match = !match;
			}
			if (match)
			{
				l_placeOut.Add(l_tempString, 1);
			}
			else
			{
				l_placeIn.Add(l_tempString, 1);
				AddtoCurLBInStrings(l_tempString);
			}
		}
		m_pListBoxIn->Clear();
		m_pListBoxOut->Clear();
		l_placeOut.Sort(false);
		m_pListBoxIn->InsertItems(l_placeIn,0);
		m_pListBoxOut->InsertItems(l_placeOut, 0);

	}
}

void SP_DLG_PlacesSelection::LoadData()
{
	m_ArrayString.clear();

	m_ArrayString = m_msaCurLBInStrings[m_sNodeType+wxT(":")+m_sOutputType];

	m_pListBoxIn->Clear();
	if (!m_ArrayString.IsEmpty())
	{
	  m_pListBoxIn->InsertItems(m_ArrayString,0);
    }
	
	NodesThroughRegEx();
}


void SP_DLG_PlacesSelection::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow())
	{
		GetSelResults();
		SaveMetaData();
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

void SP_DLG_PlacesSelection::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void SP_DLG_PlacesSelection::OnLeftToRight(wxCommandEvent& p_cEvent )
{
	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	wxArrayString l_ArrayTemp;
	int l_number=m_pListBoxIn->GetSelections(l_Selections);
	for (unsigned int i = 0; i < m_pListBoxOut->GetCount(); i++) {
		l_ArrayTemp.Add(m_pListBoxOut->GetString(i));
	}
	if(l_number>0)
	{
		for(int Counter = 0; Counter <l_number; Counter++ )
		{
		    int l_nNum = l_Selections.Item(Counter);
			l_SelectionItem = m_pListBoxIn->GetString(l_nNum);

			l_ArrayTemp.Add(l_SelectionItem);

			RemovefromCurLBInStrings(l_SelectionItem);
		}	
	}
	m_pListBoxOut->Clear();
	l_ArrayTemp.Sort();
	m_pListBoxOut->InsertItems(l_ArrayTemp, 0);
	LoadData();
}

void SP_DLG_PlacesSelection::OnLeftToRightAll(wxCommandEvent& p_cEvent )
{	
	wxArrayInt l_Selections;	
	wxArrayString l_ArrayTemp;
	for (unsigned int i = 0; i < m_pListBoxOut->GetCount(); i++) {
		l_ArrayTemp.Add(m_pListBoxOut->GetString(i));
	}
	for(unsigned int Counter = 0; Counter <m_pListBoxIn->GetCount(); Counter++ )
	{
		wxString l_SelectionItem=m_pListBoxIn->GetString(Counter);
		l_ArrayTemp.Add(l_SelectionItem);
	}
	m_pListBoxOut->Clear();
	l_ArrayTemp.Sort(false);
	m_pListBoxOut->InsertItems(l_ArrayTemp, 0);
	map<wxString, wxArrayString>::iterator itMap;		
	itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+m_sOutputType);

	if(itMap != m_msaCurLBInStrings.end())
		itMap->second.clear();

	m_pListBoxIn->Clear();
}


void SP_DLG_PlacesSelection::OnRightToLeft(wxCommandEvent& p_cEvent )
{
	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	int l_number=m_pListBoxOut->GetSelections(l_Selections);
	if(l_number>0)
	{
		for(int Counter = l_number-1; Counter >=0; Counter-- )
		{
		    int l_nNum = l_Selections.Item(Counter);
			wxString l_SelectionItem=m_pListBoxOut->GetString(l_nNum);
			
			m_pListBoxOut->Delete(l_nNum);
			AddtoCurLBInStrings(l_SelectionItem);
		}
	}

	LoadData();
}

void SP_DLG_PlacesSelection::OnRightToLeftAll(wxCommandEvent& p_cEvent )
{
	m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
	m_msaCurLBInStrings[wxT("Place:Colored")] = m_ArrayColPlaces;
	m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;
	m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
	m_msaCurLBInStrings[wxT("Transition:Colored")] = m_ArrayColTransitions;
	m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;

	m_pListBoxOut->Clear();

	LoadData();
}





void SP_DLG_PlacesSelection::OnEditAuxVariable(wxCommandEvent& p_cEvent )
{
}

void SP_DLG_PlacesSelection::GetSelResults()
{
	m_vmSelectedNodes.clear();

	wxArrayInt l_Selections;
	wxString l_SelectionItem;

	for(unsigned int Counter = 0; Counter <m_pListBoxOut->GetCount(); Counter++ )
	{
		l_SelectionItem = m_pListBoxOut->GetString(Counter);	

		SP_ColPN_Position_OutputType l_Pos_Type;
		long l_nPos;
		wxString l_OutType;
		bool l_bNotFound = true;

		if( m_sNodeType == wxT("Place") )
		{
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_ArrayPlaceAuxVar.size(); i++ )
				{
					wxString l_sPlace = m_ArrayPlaceAuxVar[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Auxiliary variables");
						break;
					}
				}
			}

			if(l_bNotFound)
			{	
				l_OutType = wxT("Unfolded");
				l_nPos = m_mPlaceName2ID[l_SelectionItem];				
			}
		}

		if( m_sNodeType == wxT("Transition") )
		{
			if(l_bNotFound)
			{				
				for( unsigned int i = 0; i < m_ArrayTransAuxVar.size(); i++ )
				{
					wxString l_sPlace = m_ArrayTransAuxVar[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Auxiliary variables");
						break;
					}
				}
			}
			if(l_bNotFound)
			{
				l_OutType = wxT("Unfolded");
				l_nPos = m_mTransName2ID[l_SelectionItem];	
			}
		}	
		l_Pos_Type.m_nPosition = l_nPos;
		l_Pos_Type.m_OutputType = l_OutType;
		l_Pos_Type.m_sName=m_pListBoxOut->GetString(Counter);
		m_vmSelectedNodes.push_back(l_Pos_Type);
	}		

}


void SP_DLG_PlacesSelection::LoadNodeColours(const wxString& p_nNodeName,SP_VectorString& p_asColours)
{
	SP_DS_Nodeclass *l_pcNodeclass;
	const SP_ListNode *l_pcNodeList;

	SP_ListNode::const_iterator l_itNode;

	l_pcNodeclass = m_pcGraph->GetNodeclass(p_nNodeName);

	if (l_pcNodeclass == NULL) return;

	l_pcNodeList = l_pcNodeclass->GetElements();

	if (l_pcNodeList == NULL) return;

	bool l_bUseNodeColour = m_pcPlaceChoiceColour->GetValue();

	for (l_itNode = l_pcNodeList->begin(); l_itNode != l_pcNodeList->end(); l_itNode++) {
		SP_ListGraphic *l_pcGraphics = (*l_itNode)->GetGraphics();

		if (l_pcGraphics == NULL) return;

		wxColour l_cColour = *wxWHITE;
		for(SP_ListGraphic::const_iterator itG = l_pcGraphics->begin();
			itG != l_pcGraphics->end() && l_cColour == *wxWHITE; ++itG)
		{
			if((*itG)->GetGraphicState() == SP_STATE_NORMAL ||
			   (*itG)->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
			{
				l_cColour = (*itG)->GetBrush()->GetColour();
			}
		}

		if (l_cColour != *wxWHITE && l_bUseNodeColour) {
			p_asColours.push_back(l_cColour.GetAsString(wxC2S_HTML_SYNTAX));
		}
		else {
			p_asColours.push_back(wxT(""));
		}
	}
}



void SP_DLG_PlacesSelection::SaveMetaData()
{
	m_pcEditMetadata->GetAttribute(wxT("Nodeclass"))->SetValueString(m_sNodeType);
	m_pcEditMetadata->GetAttribute(wxT("Name"))->SetValueString(m_pcNameTextCtrl->GetValue());

	wxString l_nRegexString = m_pcPlaceChoiceOutRegex->GetValue();
	m_pcEditMetadata->GetAttribute(wxT("RegEx"))->SetValueString(l_nRegexString);
	static_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("RegExInvert")))->SetValue(m_pcPlaceChoiceRegexInvert->GetValue());

	static_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("NodeColour")))->SetValue(m_pcPlaceChoiceColour->GetValue());

	//check if this net is a coloured one
	bool l_bColouredNet=false;
	wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();

	if(l_sNetClass.Contains(wxT("Colored")))
	{
		l_bColouredNet=true;
		m_pcEditMetadata->GetAttribute(wxT("OutputType"))->SetValueString(m_sOutputType);
	}

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*> (m_pcEditMetadata->GetAttribute(wxT("CurveInfo")));
	l_pcCurveInfoList->Clear();

	SP_VectorString l_asColours;

	if(m_sNodeType==wxT("Place"))
	{
		 LoadNodeColours(SP_DS_CONTINUOUS_PLACE,l_asColours);
		 LoadNodeColours(SP_DS_DISCRETE_PLACE,l_asColours);
	}
	else if(m_sNodeType==wxT("Transition"))
	{
		LoadNodeColours(SP_DS_CONTINUOUS_TRANS,l_asColours);
		LoadNodeColours(SP_DS_DISCRETE_TRANS,l_asColours);
	}

	for(unsigned int i = 0; i < m_vmSelectedNodes.size(); i++)
	{
		wxString l_sPos;
		l_sPos  = wxString::Format(wxT("%ld"),m_vmSelectedNodes[i].m_nPosition);
		wxString l_sType;
		l_sType =  m_vmSelectedNodes[i].m_OutputType;
		wxString l_sCheckState;
		l_sCheckState << true;
		wxString l_sCurveColor;

		unsigned long l_nPos=m_vmSelectedNodes[i].m_nPosition;

		//get either the node colour or assign an arbitrary colour
		if(l_bColouredNet == false && l_nPos < l_asColours.size() && l_asColours[l_nPos] != wxT(""))
		{
			l_sCurveColor << l_asColours[l_nPos];
		}
		else if( l_sType == wxT("Colored") && l_nPos < l_asColours.size() && l_asColours[l_nPos] != wxT(""))
		{
			l_sCurveColor << l_asColours[l_nPos];
		}
		else
		{
			l_sCurveColor << SP_DLG_Simulation::GetColourString(i);
		}

		wxString l_sLinewidth;
		l_sLinewidth <<-1;
		wxString l_sLineStype;
		l_sLineStype << -1;

		wxString l_sName=m_vmSelectedNodes[i].m_sName;

		unsigned int l_nRow = l_pcCurveInfoList->AppendEmptyRow();
		l_pcCurveInfoList->SetCell(l_nRow,0,l_sPos);
		l_pcCurveInfoList->SetCell(l_nRow,1,l_sType);
		l_pcCurveInfoList->SetCell(l_nRow,2,l_sCheckState);
		l_pcCurveInfoList->SetCell(l_nRow,3,l_sCurveColor);
		l_pcCurveInfoList->SetCell(l_nRow,4,l_sLinewidth);
		l_pcCurveInfoList->SetCell(l_nRow,5,l_sLineStype);
		l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);
	}
	
}

void SP_DLG_PlacesSelection::OnChangedPlaceTransition(wxCommandEvent& p_cEvent)
{
	if(m_pcPlaceTransitionRadioBox->GetSelection() == 0 )
	{
		m_sNodeType = wxT("Place");

		m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;		
		m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;
	}
	else
	{
		m_sNodeType = wxT("Transition");

		m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;		
		m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;
	}

	m_pListBoxOut->Clear();
	
	m_pcPlaceChoiceStaticBox->SetLabel(wxT("The overall ") + m_sNodeType+ wxT("s"));
	m_pcFourthColumStaticBox->SetLabel(wxT("The selected ") + m_sNodeType+ wxT("s"));
	
	LoadData();

}


void SP_DLG_PlacesSelection::OnChangedOutType(wxCommandEvent& p_cEvent)
{
	m_msaCurLBInStrings[m_sNodeType+wxT(":")+m_sOutputType] = m_pListBoxIn->GetStrings();

	if(m_pcOutputTypeRadioBox->GetSelection() == 0)
	{
		m_sOutputType = wxT("Unfolded");
	}	
	else
	{
		m_sOutputType = wxT("Auxiliary variables");
	}

	LoadData();

}

void SP_DLG_PlacesSelection::Initialize()
{
	m_ArrayUnPlaces.clear();
	m_ArrayUnTranstions.clear();
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;

	long l_nPos = 0;

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_mPlaceID2Name[l_nPos] = l_sPlaceName;
			m_mPlaceName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnPlaces.Add(l_sPlaceName);
			l_nPos++;
		}
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mPlaceID2Name[l_nPos] = l_sPlaceName;	
			m_mPlaceName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnPlaces.Add(l_sPlaceName);	
			l_nPos++;
		}	
	}

	l_nPos = 0;

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mTransID2Name[l_nPos] = l_sPlaceName;	
			m_mTransName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnTranstions.Add(l_sPlaceName);
			l_nPos++;			
		}	
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mTransID2Name[l_nPos] = l_sPlaceName;	
			m_mTransName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnTranstions.Add(l_sPlaceName);	
			l_nPos++;			
		}	
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mTransID2Name[l_nPos] = l_sPlaceName;	
			m_mTransName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnTranstions.Add(l_sPlaceName);	
			l_nPos++;
		}	
	}
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mTransID2Name[l_nPos] = l_sPlaceName;	
			m_mTransName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnTranstions.Add(l_sPlaceName);	
			l_nPos++;
		}	
	}

	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if(l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;	
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_mTransID2Name[l_nPos] = l_sPlaceName;	
			m_mTransName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnTranstions.Add(l_sPlaceName);	
			l_nPos++;
		}	
	}	

	m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
	m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;
	m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
	m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;
}

void SP_DLG_PlacesSelection::InitilizeFromMetaData()
{
	if(m_sNodeType == wxT("Place"))
	{
		m_pcPlaceTransitionRadioBox->SetSelection(0);
	}
	else
	{
		m_pcPlaceTransitionRadioBox->SetSelection(1);
	}

	m_pcPlaceChoiceStaticBox->SetLabel(wxT("The overall ") + m_sNodeType+ wxT("s"));
	m_pcFourthColumStaticBox->SetLabel(wxT("The selected ") + m_sNodeType+ wxT("s"));

	m_pcNameTextCtrl->SetValue((m_pcEditMetadata->GetAttribute(wxT("Name")))->GetValueString());

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*> (m_pcEditMetadata->GetAttribute(wxT("CurveInfo")));


	bool l_UseNodeColour = static_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("NodeColour")))->GetValue();
	m_pcPlaceChoiceColour->SetValue(l_UseNodeColour);

	//load data to listboxout
	wxString l_RegExString = m_pcEditMetadata->GetAttribute(wxT("RegEx"))->GetValueString();
	m_pcPlaceChoiceOutRegex->SetValue(l_RegExString);
	bool l_RegExInvert = static_cast<SP_DS_BoolAttribute*>(m_pcEditMetadata->GetAttribute(wxT("RegExInvert")))->GetValue();
	m_pcPlaceChoiceRegexInvert->SetValue(l_RegExInvert);
	wxRegEx l_RegEx;
	wxArrayString l_arSelected;
	if (l_RegExString == wxT("")) {
		for(unsigned int i = 0; i < l_pcCurveInfoList->GetRowCount(); i++)
		{
			wxString l_sPlTrName = l_pcCurveInfoList->GetCell(i,6);
			l_arSelected.Add(l_sPlTrName);

		}
		m_pListBoxOut->Clear();
		if (!l_arSelected.IsEmpty())
		{
		  m_pListBoxOut->InsertItems(l_arSelected,0);
		}

		//initialize array for showing

		for(unsigned int i = 0; i < l_pcCurveInfoList->GetRowCount(); i++)
		{
			wxString l_sPlTrID = l_pcCurveInfoList->GetCell(i,0);
			long l_nPlTrID;
			if( !l_sPlTrID.ToLong(&l_nPlTrID) )
				continue;

			wxString l_sOutput = l_pcCurveInfoList->GetCell(i,1);

			map<wxString, wxArrayString>::iterator itMap;

			itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+l_sOutput);
			if(itMap == m_msaCurLBInStrings.end())
				continue;

			wxString l_sPlTrName = l_pcCurveInfoList->GetCell(i,6);

			itMap->second.Remove(l_sPlTrName);

		}
	}
	else
	{
		wxArrayString l_arSelected;
		wxString l_sPlTrName;
		map<wxString, wxArrayString>::iterator itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+m_sOutputType);

		if (l_RegEx.Compile(l_RegExString, wxRE_DEFAULT)) {
			if (m_sNodeType == wxT("Place")) {
				if (m_sOutputType == wxT("Unfolded")) {
					for (unsigned int i = 0; i < m_ArrayUnPlaces.GetCount(); i++) {
						l_sPlTrName = m_ArrayUnPlaces[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				} else if (m_sOutputType == wxT("Colored")) {
					for (unsigned int i = 0; i < m_ArrayColPlaces.GetCount(); i++) {
						l_sPlTrName = m_ArrayColPlaces[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				} else {
					for (unsigned int i = 0; i < m_ArrayPlaceAuxVar.GetCount(); i++) {
						l_sPlTrName = m_ArrayPlaceAuxVar[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				}
			} else if (m_sNodeType == wxT("Transition")) {
				if (m_sOutputType == wxT("Unfolded")) {
					for (unsigned int i = 0; i < m_ArrayUnTranstions.GetCount(); i++) {
						l_sPlTrName = m_ArrayUnTranstions[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				} else if (m_sOutputType == wxT("Colored")) {
					for (unsigned int i = 0; i < m_ArrayColTransitions.GetCount(); i++) {
						l_sPlTrName = m_ArrayColTransitions[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				} else {
					for (unsigned int i = 0; i < m_ArrayTransAuxVar.GetCount(); i++) {
						l_sPlTrName = m_ArrayTransAuxVar[i];
						bool match = l_RegEx.Matches(l_sPlTrName);
						if(l_RegExInvert) { match = !match; }
						if(match) {
							l_arSelected.Add(l_sPlTrName);
							if(itMap == m_msaCurLBInStrings.end())
								continue;
							itMap->second.Remove(l_sPlTrName);
						}
					}
				}
			}
			m_pListBoxOut->Clear();
			if (!l_arSelected.IsEmpty())
			{
			  l_arSelected.Sort(false);
			  m_pListBoxOut->InsertItems(l_arSelected,0);
			}
		}
	}
}

void SP_DLG_PlacesSelection::AddtoCurLBInStrings(wxString p_sName)
{
	wxString l_sOutputType;

	if(m_sNodeType == wxT("Place") )
	{
		if( m_mPlaceName2ID.find(p_sName) != m_mPlaceName2ID.end() )
			l_sOutputType = wxT("Unfolded");
	}

	
	
	if(m_sNodeType == wxT("Transition") && m_sOutputType == wxT("Unfolded"))
	{
		if( m_mTransName2ID.find(p_sName) != m_mTransName2ID.end() )
			l_sOutputType = wxT("Unfolded");
	}

	map<wxString, wxArrayString>::iterator itMap;
		
	itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+l_sOutputType);

	if(itMap != m_msaCurLBInStrings.end())
		itMap->second.Add(p_sName);
	
}

void SP_DLG_PlacesSelection::RemovefromCurLBInStrings(wxString p_sName)
{

	map<wxString, wxArrayString>::iterator itMap;
		
	itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+m_sOutputType);

	if(itMap != m_msaCurLBInStrings.end())
		itMap->second.Remove(p_sName);
	
}

