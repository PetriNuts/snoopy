//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.40 $
// $Date: 2009/03/30 11:55:00 $
// $Modified: by George Assaf$
// $Date: 2011/08/14 11:20:00 $/* 
 
//////////////////////////////////////////////////////////////////////



#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStUnfolding.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"

IMPLEMENT_CLASS( SP_DLG_ColStUnfolding, wxDialog )

enum SP_CPN_ID
{
	SP_ID_UNFOLD_RADIO_BUTTON,
	SP_ID_LOAD_RADIO_BUTTON,
	SP_ID_LOADFILE_PICKER,
	SP_ID_CSPRADIOBOX,
	SP_ID_CSPDSFRADIOBOX,
	SP_ID_CSPBDBRADIOBOX,
	SP_ID_BUTTON_PAUSE,
	SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS,//by george
	SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS,
	SP_ID_BUTTON_MODIFY_COL_CONTFUN_SETS,
	SP_ID_BUTTON_MODIFY_COL_STOCHFUN_SETS,
	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS_STOCH,
	SP_ID_BUTTON_CHANGE_COL_MARKING_SETS,
	SP_ID_BUTTON_CHANGE_COL_FUN_SETS,
	SP_ID_BUTTON_CHANGE_COL_FUN_STOCH_SETS
};

BEGIN_EVENT_TABLE( SP_DLG_ColStUnfolding, wxDialog )
EVT_CLOSE(SP_DLG_ColStUnfolding::OnClose)
EVT_BUTTON( wxID_OK, SP_DLG_ColStUnfolding::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ColStUnfolding::OnDlgCancel )
EVT_BUTTON( SP_ID_BUTTON_PAUSE, SP_DLG_ColStUnfolding::OnPause )
///EVT_RADIOBUTTON(SP_ID_UNFOLD_RADIO_BUTTON, SP_DLG_ColStUnfolding::OnUnfoldRadioButton)
///EVT_RADIOBUTTON(SP_ID_LOAD_RADIO_BUTTON, SP_DLG_ColStUnfolding::OnUnLoadRadioButton)
EVT_CHOICE(SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS, SP_DLG_ColStUnfolding::OnColoringSetChanged)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, SP_DLG_ColStUnfolding::OnModyfyingConstants)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColStUnfolding::OnModyfyingMarkingGroup)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS_STOCH, SP_DLG_ColStUnfolding::OnModyfyingFunStochGroup)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_COL_CONTFUN_SETS, SP_DLG_ColStUnfolding::OnModyfyingFunContSetGroup)
EVT_CHOICE(SP_ID_BUTTON_CHANGE_COL_MARKING_SETS, SP_DLG_ColStUnfolding::OnMarkingsetChanged)
EVT_CHOICE(SP_ID_BUTTON_CHANGE_COL_FUN_SETS, SP_DLG_ColStUnfolding::OnFunctionChanged)
EVT_CHOICE(SP_ID_BUTTON_CHANGE_COL_FUN_STOCH_SETS, SP_DLG_ColStUnfolding::OnFunctionStochChanged)
END_EVENT_TABLE()

 
SP_DLG_ColStUnfolding::SP_DLG_ColStUnfolding( SP_DS_ColPN_Unfolding* p_pcColPN_Unfolding, wxWindow* p_pcParent,const wxString& p_sTitle, long p_nStyle) 
		:wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120),  wxSize( 1500, 400),
			p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	m_pcColPN_Unfolding = p_pcColPN_Unfolding;
	m_pcSizer = new wxBoxSizer( wxVERTICAL );
/*
	//first row
	wxSizer* l_pcSourceSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pcUnfoldRadioBut = new wxRadioButton(this,SP_ID_UNFOLD_RADIO_BUTTON,wxT("Unfold"),wxDefaultPosition, wxDefaultSize,wxRB_GROUP);
	m_pcLoadRadioBut = new wxRadioButton(this,SP_ID_LOAD_RADIO_BUTTON,wxT("Load a file"),wxDefaultPosition, wxDefaultSize);
	m_pcLoadFilePickerCtrl = new wxFilePickerCtrl( this, SP_ID_LOADFILE_PICKER, wxT(""), wxT("Browse"), wxT("Unfold files (*.ufd)|*.ufd"), wxDefaultPosition,
													wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);

	m_pcUnfoldRadioBut->SetValue(true);

	wxBoxSizer* l_pcSizer_1 = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer_1->Add( m_pcUnfoldRadioBut, 0, wxALL, 5 );

	wxBoxSizer* l_pcSizer_2 = new wxBoxSizer( wxHORIZONTAL );
	l_pcSizer_2->Add( m_pcLoadRadioBut, 0, wxALL, 5 );
	l_pcSizer_2->Add( m_pcLoadFilePickerCtrl, 0, wxALL, 5 );

	l_pcSourceSizer->Add( l_pcSizer_1, 0, wxALL, 5 );
	l_pcSourceSizer->Add( l_pcSizer_2, 0, wxALL, 5 );

	m_pcSizer->Add( l_pcSourceSizer, 0, wxALL| wxEXPAND, 5 );


	//second row
	wxSizer* l_pcWriteSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pcWriteCheckBox = new wxCheckBox( this, -1, wxT("Write to a file"), wxDefaultPosition, wxDefaultSize, 0 );
	//l_pcWriteSizer->Add(new wxStaticText( this, -1, wxT("Filename:") ), 0, wxALL | wxEXPAND, 5);
	m_pcSaveFilePickerCtrl = new wxFilePickerCtrl( this, -1, wxT(""), wxT("Browse"), wxT("Unfold files (*.ufd)|*.ufd"), wxDefaultPosition,
													wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);

	l_pcWriteSizer->Add( m_pcWriteCheckBox, 0, wxALL, 5 );
	l_pcWriteSizer->Add( m_pcSaveFilePickerCtrl, 0, wxALL, 5 );

	m_pcSizer->Add( l_pcWriteSizer, 0, wxALL| wxEXPAND, 5 );
	*/

	//third row
	//wxSizer* l_pcCSPSizer = new wxBoxSizer( wxVERTICAL );

	//m_pcCSPCheckBox =  new wxCheckBox( this, -1, wxT("CSP solver"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_pcCSPCheckBox->SetValue(true);
	//l_pcCSPSizer->Add( m_pcCSPCheckBox, 0, wxALL| wxEXPAND, 5 );
	//m_pcSizer->Add( l_pcCSPSizer, 0, wxALL| wxEXPAND, 5 );

	wxSizer* l_pcWriteSizer = new wxBoxSizer(wxHORIZONTAL);

	wxArrayString unfolderChoices;
	unfolderChoices.Add(wxT("IDD (dssd_util)"));
	unfolderChoices.Add(wxT("Gecode (dssd_util)"));
	unfolderChoices.Add(wxT("Gecode (intern)"));
	unfolderChoices.Add(wxT("Generic (intern)"));
	m_pcUnfolderRadioBox = new wxRadioBox(this, -1, wxT("Engine"), wxDefaultPosition, wxDefaultSize, unfolderChoices, 0, wxRA_SPECIFY_ROWS);
	m_pcUnfolderRadioBox->SetSelection(0);
	m_pcSizer->Add( m_pcUnfolderRadioBox, 0, wxALL| wxEXPAND, 5 );

	//fourth row
	wxSizer* l_pcThreadSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcThreadSizer->Add( new wxStaticText( this, -1, wxT("Thread count") ), 0 , wxALL, 5 );
	m_pcThreadCountComboBox = new wxChoice( this, -1 );
	int l_nCPUCount = wxThread::GetCPUCount();
	for(int i = 1; i <= l_nCPUCount; ++i)
	{
		m_pcThreadCountComboBox->Append(wxString() << i);
	}
	m_pcThreadCountComboBox->SetSelection(0);
	l_pcThreadSizer->Add( m_pcThreadCountComboBox, 0, wxALL, 5 );

	m_pcSizer->Add( l_pcThreadSizer, 0, wxALL| wxEXPAND, 5 );
	///////////////////by george///////////////////

	bool l_bIsColoredQualtativePN = false;

	if ((SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_COLPN_CLASS || SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS))
	{//there is no time (functions)
			l_bIsColoredQualtativePN = true;
	}

	if (IsExistColoringGroup())
		{
			m_SetChoices.insert(_T("coloring"));
			std::set<wxString>::iterator l_itChoice;
			for (l_itChoice = m_SetChoices.begin(); l_itChoice != m_SetChoices.end(); ++l_itChoice)
			{
				wxString l_sGroup = *l_itChoice;
				l_pcWriteSizer = new wxBoxSizer(wxHORIZONTAL);
				m_apcComboBoxes.push_back(new wxChoice(this, SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS, wxDefaultPosition, wxSize(100, -1), 0, NULL, 0, wxDefaultValidator, l_sGroup));
				l_pcWriteSizer->Add(new wxStaticText(this, -1, wxT("Coloring group:")), 1, wxALL, 5);
				l_pcWriteSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);// wxSizerFlags(0).Expand().Border(wxALL, 2));
				l_pcWriteSizer->Add(new wxButton(this, SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, wxT("modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
				m_pcSizer->Add(l_pcWriteSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

			}
			LoadColringGroupChoices();
		}
		SP_DS_Nodeclass* l_pcNodeclass;

			   l_pcNodeclass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(wxT("Place"));
			if (!l_pcNodeclass)
			{
				l_pcNodeclass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
			}
			else
			{
				 l_pcNodeclass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE);
				if(l_pcNodeclass)
				l_pcNodeclass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE);
			}
			if (l_pcNodeclass)
			{
				//TODO: by mos: we will need to fix this
				if (l_pcNodeclass->GetElements() !=nullptr)
				{
					// rows of SetsSizer

					wxSizer*	m_pcSizer1 = new wxBoxSizer(wxHORIZONTAL);
					m_pcSizer1->Add(new wxStaticText(this, -1, wxT("Marking set:")), 1, wxALL | wxEXPAND, 5);
					m_pcMarkingSetComboBox=new wxChoice(this, SP_ID_BUTTON_CHANGE_COL_MARKING_SETS, wxDefaultPosition, wxSize(100, -1));
					m_pcSizer1->Add(m_pcMarkingSetComboBox, 1, wxALL, 5);
					m_pcSizer1->Add(new wxButton(this, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("modify")), 0, wxALL, 5);
					m_pcSizer->Add(m_pcSizer1, wxSizerFlags(0).Expand().Border(wxALL, 2));

				}
			}
			if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS) && !l_bIsColoredQualtativePN)
			{
				if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size() > 0)
				{
					wxSizer*	m_pcSizer2 = new wxBoxSizer(wxHORIZONTAL);
					m_pcSizer2->Add(new wxStaticText(this, -1, wxT("Continuous function set:")), 1, wxALL | wxEXPAND, 5);

					m_pcFunctionSetComboBox = new wxChoice(this, SP_ID_BUTTON_CHANGE_COL_FUN_SETS, wxDefaultPosition, wxSize(100, -1));

					m_pcSizer2->Add(m_pcFunctionSetComboBox, 1, wxALL, 5);

					m_pcSizer2->Add(new wxButton(this, SP_ID_BUTTON_MODIFY_COL_CONTFUN_SETS, wxT("modify")), 0, wxALL, 5);

					m_pcSizer->Add(m_pcSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
				}
			}
			else
			{
				m_pcFunctionSetComboBox = NULL;
			}

			if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS) && !l_bIsColoredQualtativePN)
			{
				if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size() > 0)
				{
					wxSizer*	m_pcSizer2 = new wxBoxSizer(wxHORIZONTAL);
					m_pcSizer2->Add(new wxStaticText(this, -1, wxT("Stochastic function set:")), 1, wxALL | wxEXPAND, 5);

					m_pcFunctionStochSetComboBox = new wxChoice(this, SP_ID_BUTTON_CHANGE_COL_FUN_STOCH_SETS, wxDefaultPosition, wxSize(100, -1));

					m_pcSizer2->Add(m_pcFunctionStochSetComboBox, 1, wxALL, 5);

					m_pcSizer2->Add(new wxButton(this, SP_ID_BUTTON_MODIFY_FUNCTION_SETS_STOCH, wxT("modify")), 0, wxALL, 5);

					m_pcSizer->Add(m_pcSizer2, wxSizerFlags(0).Expand().Border(wxALL, 2));
				}
			}
			else
			{
				m_pcFunctionStochSetComboBox = NULL;
			}


		LoadOtherSets();
		////////////////////////////////

		//fifth row
		 wxSizer* l_TimeProgressSizer = new wxBoxSizer(wxVERTICAL);
		 m_pcUnfoldingProgressGauge = new wxGauge(this, -1, 100, wxDefaultPosition, wxDefaultSize, 0);
		 l_TimeProgressSizer->Add(m_pcUnfoldingProgressGauge, 1, wxALL | wxEXPAND, 5);

		wxSizer* l_TimeSizer = new wxBoxSizer( wxHORIZONTAL );
		l_TimeSizer->Add(new wxStaticText(this, -1, wxT("Unfolding run time: ")), 0, wxALL | wxEXPAND, 5);
		m_pcUnfoldingStopWatch = new wxStaticText(this, -1, wxT("0,0 sec"));
		l_TimeSizer->Add(m_pcUnfoldingStopWatch, 1, wxALL | wxEXPAND, 5);
		l_TimeProgressSizer->Add( l_TimeSizer, 0, wxALL | wxEXPAND, 5 );

		m_pcSizer->Add( l_TimeProgressSizer, 1, wxALL | wxEXPAND, 5 );


		//sixth row
		wxSizer* l_pcStartSizer = new wxBoxSizer(wxHORIZONTAL);
		m_pcStartButton = new wxButton(this, wxID_OK, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0);
		m_pcStartButton->SetBackgroundColour(*wxGREEN);
		l_pcStartSizer->Add(m_pcStartButton, 1, wxALL | wxEXPAND, 5);

		m_pcPauseButton = new wxButton(this, SP_ID_BUTTON_PAUSE, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0);
		m_pcPauseButton->SetBackgroundColour(*wxGREEN);
		l_pcStartSizer->Add(m_pcPauseButton, 1, wxALL | wxEXPAND, 5);

		m_pcSizer->Add(l_pcStartSizer, 0, wxALL | wxEXPAND, 5);

	    //close
		//wxSizer* l_pcCloseSizer = new wxBoxSizer( wxHORIZONTAL );
		//l_pcCloseSizer->Add( new wxButton( this, wxID_CANCEL, wxT("Close") ), 0, wxALL, 5 );
		//m_pcSizer->Add( l_pcCloseSizer, 0, wxALL | wxEXPAND, 5 );



		SetAutoLayout( TRUE);
		SetSizerAndFit(m_pcSizer);
		m_pcSizer->SetSizeHints( this);

		Layout();


}

SP_DLG_ColStUnfolding::~SP_DLG_ColStUnfolding()
{}


void SP_DLG_ColStUnfolding::OnDlgOk(wxCommandEvent& p_cEvent)
{
	//start or abort unfolding
	if (m_pcColPN_Unfolding->IsUnfoldingRunning() )
	{
		m_pcColPN_Unfolding->AbortUnfolding();
		return;
	}

	//configure unfolding
	if (Validate() && TransferDataFromWindow())
	{
		/*
		if(m_pcUnfoldRadioBut->GetValue())
		{
			*/
			m_pcColPN_Unfolding->SetUnfoldSource( wxT("Unfold") );

			switch (m_pcUnfolderRadioBox->GetSelection()) {
				case 0:
					m_pcColPN_Unfolding->SetIddDsszUtilSolveFlag(true);
					m_pcColPN_Unfolding->SetGecodeDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetCSPSolveFlag(false);
					break;
				case 1:
					m_pcColPN_Unfolding->SetIddDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetGecodeDsszUtilSolveFlag(true);
					m_pcColPN_Unfolding->SetCSPSolveFlag(false);
					break;
				case 2:
					m_pcColPN_Unfolding->SetIddDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetGecodeDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetCSPSolveFlag(true);
					break;
				case 3:
				default:
					m_pcColPN_Unfolding->SetIddDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetGecodeDsszUtilSolveFlag(false);
					m_pcColPN_Unfolding->SetCSPSolveFlag(false);
					break;
					break;
			}

			//m_pcColPN_Unfolding->SetWriteFlag( m_pcWriteCheckBox->GetValue() );
			//if( m_pcWriteCheckBox->GetValue() )
			//	m_pcColPN_Unfolding->SetSaveFilename( m_pcSaveFilePickerCtrl->GetPath() );

			long l_nValue;
			if(m_pcThreadCountComboBox->GetStringSelection().ToLong(&l_nValue) && l_nValue > 0)
				m_pcColPN_Unfolding->SetThreadCount( l_nValue );
			else
				return;
	//	}
	//	else
	//	{
		//	m_pcColPN_Unfolding->SetUnfoldSource( wxT("Load") );
		//	m_pcColPN_Unfolding->SetLoadFilename( m_pcLoadFilePickerCtrl->GetPath() );
			//check if the file path is valid or not
		//}
	}

	//Initialize
	SetUnfoldingProgressGauge(0);
	m_pcStartButton->SetLabel(wxT("Abort"));
	m_pcStartButton->SetBackgroundColour( *wxRED);
	Update();
#if wxABI_VERSION > 30000
	wxStandardID l_eReturn = wxID_OK;
#else
	int l_eReturn = wxID_OK;
#endif
	if( !m_pcColPN_Unfolding->Start() )
	{
		l_eReturn = wxID_CANCEL;
	}
		

	if (IsModal())
	{
		EndModal(l_eReturn);
	}
	else
	{
		SetReturnCode(l_eReturn);
		this->Show(FALSE);
	}
}

void SP_DLG_ColStUnfolding::OnDlgCancel(wxCommandEvent& p_cEvent)
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

/*
void SP_DLG_ColStUnfolding::OnUnfoldRadioButton( wxCommandEvent& p_cEvent )
{
	m_pcLoadFilePickerCtrl->Enable(false);

	m_pcWriteCheckBox->Enable();
	m_pcSaveFilePickerCtrl->Enable();
	//m_pcCSPCheckBox->Enable();
	m_pcUnfolderRadioBox->Enable();
	m_pcThreadCountComboBox->Enable();

}

void SP_DLG_ColStUnfolding::OnUnLoadRadioButton( wxCommandEvent& p_cEvent )
{
	m_pcLoadFilePickerCtrl->Enable();

	m_pcWriteCheckBox->Enable(false);
	m_pcSaveFilePickerCtrl->Enable(false);
	//m_pcCSPCheckBox->Enable(false);
	m_pcUnfolderRadioBox->Enable(false);
	m_pcThreadCountComboBox->Enable(false);
}
*/

void SP_DLG_ColStUnfolding::OnPause( wxCommandEvent& p_cEvent )
{	
	m_pcColPN_Unfolding->SetResumePause( ! m_pcColPN_Unfolding->IsUnfoldingPausing() );	

	if(!m_pcColPN_Unfolding->IsUnfoldingPausing())
	{
		m_pcPauseButton->SetLabel(wxT("Pause"));
		m_pcPauseButton->SetBackgroundColour( *wxGREEN);
	}
	else
	{
		m_pcPauseButton->SetLabel(wxT("Resume"));
		m_pcPauseButton->SetBackgroundColour( *wxRED);
	}

	Update();

}

void SP_DLG_ColStUnfolding::LoadColringGroupChoices()
{
	if (m_apcColListAttr.size() > 0)
	{
		m_apcColListAttr.clear();
	}
	m_SetChoices.clear();
	//l_pcGraph->GetMetadataclass();
	SP_DS_Netclass* l_pcNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass();
	SP_DS_Metadataclass* mc;
	if(!l_pcNetClass->GetName().Contains(_T("Fuzzy")))
	 mc= SP_Core::Instance()->GetRootDocument()->GetGraph()->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	else
	mc= SP_Core::Instance()->GetRootDocument()->GetGraph()->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc)
	{
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata = NULL;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Group"));
			if (l_pcAttr)
			{
				wxString l_sGroup = l_pcAttr->GetValueString();
				if(l_sGroup==wxT("coloring"))
				m_SetChoices.insert(l_sGroup);
			}
		}
	}
	int ss = m_apcColListAttr.size();

	//SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);

	SP_ListMetadata::const_iterator l_itElem;

	std::set<wxString>::iterator l_itChoice;
	for (l_itChoice = m_SetChoices.begin(); l_itChoice != m_SetChoices.end(); ++l_itChoice)
	{
		wxString l_sChoice = *l_itChoice;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = mc->GetElements()->begin(); l_itElem != mc->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			if (l_sChoice == l_sGroup  )
			{
				m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				break;
			}
		}
	}


	if(m_apcComboBoxes.size()>0)
	for (size_t j = ss; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];

		if (!l_pcAttr) continue;

		wxChoice* l_pcCombobox = m_apcComboBoxes[j];

		if (!l_pcCombobox) continue;

		l_pcCombobox->Clear();
		if (l_pcAttr)
		{
			for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				wxString l_sSetName = l_pcAttr->GetCell(i, 0);
				l_pcCombobox->Append(l_sSetName);
			}
			l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
		}
	}
}


void SP_DLG_ColStUnfolding::OnColoringSetChanged(wxCommandEvent& p_cEvent)
{
	if (m_apcComboBoxes.size() == 0 || m_apcColListAttr.size() == 0)
		return;

	unsigned int l_nchosenSet  = m_apcComboBoxes[0]->GetSelection();
	m_apcColListAttr[0]->SetActiveList(l_nchosenSet);
	m_apcComboBoxes[0]->SetSelection(l_nchosenSet);
	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if (!l_cSyntaxChecking.Initialize())
		return;
	l_cSyntaxChecking.UpdateNetMarking();

	SP_Core::Instance()->GetRootDocument()->RefreshAllViews();

}

void SP_DLG_ColStUnfolding::OnModyfyingConstants(wxCommandEvent& p_cEvent)
{
	SP_DLG_ConstantDefinition * l_pcConstantDialog = new SP_DLG_ConstantDefinition(this);

	if (l_pcConstantDialog->ShowModal() == wxID_OK)
	{
		LoadColringGroupChoices();
	}
}

void SP_DLG_ColStUnfolding::OnClose(wxCloseEvent& event)//by george
{
	wxDialog::EndModal(wxID_CANCEL);
}


void SP_DLG_ColStUnfolding::LoadOtherSets()
{
	SP_DS_Node* l_pcNode = NULL;
	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE))
	{
		if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size() > 0)
		{
			l_pcNode = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->front();

			m_apcColListAttrMarking = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
		}
	}
	else
	{
		if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE))
		{
			if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size() > 0)
			{
				l_pcNode = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->front();

				m_apcColListAttrMarking = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
			}
		}
	}

	if (!(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_COLPN_CLASS || SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS))
	{
	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)) {
		if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size() > 0)
		{
			l_pcNode = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->front();

			m_apcColListAttrFunctions = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
		}
	}

	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)) {
		if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size() > 0)
		{
			l_pcNode = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->front();

			m_apcColListAttrFunctionsStoch = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
		}
	}
	}
	else
	{
		m_apcColListAttrFunctionsStoch = NULL;
		m_apcColListAttrFunctions = NULL;
	}
   if (m_apcColListAttrMarking&&m_pcMarkingSetComboBox)
   {
	   m_pcMarkingSetComboBox->Clear();
	   for (unsigned int i = 0; i < m_apcColListAttrMarking->GetColCount(); i++)
	   {
		   wxString l_sSetName = m_apcColListAttrMarking->GetColLabel(i); //(i, 0);

		   if (!l_sSetName.IsEmpty() && !l_sSetName.Contains(_T(":"))) l_sSetName = wxT("Main:");

		   l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));
		   m_pcMarkingSetComboBox->Append(l_sSetName);
		   i++;
		   i++;
	   }
	   m_pcMarkingSetComboBox->SetSelection(m_apcColListAttrMarking->GetActiveList());

   }

   if (m_apcColListAttrFunctions&&m_pcFunctionSetComboBox)
   {
	   m_pcFunctionSetComboBox->Clear();
	   for (unsigned int i = 1; i < m_apcColListAttrFunctions->GetColCount(); i++)
	   {
		   wxString l_sSetName = m_apcColListAttrFunctions->GetColLabel(i);
		  // if (!l_sSetName.Contains(":"))continue;
		   l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));

		   m_pcFunctionSetComboBox->Append(l_sSetName);

	   }
	   m_pcFunctionSetComboBox->SetSelection(m_apcColListAttrFunctions->GetActiveList());
   }

   if (m_apcColListAttrFunctionsStoch&&m_pcFunctionStochSetComboBox)
   {
	   m_pcFunctionStochSetComboBox->Clear();
	   for (unsigned int i = 1; i < m_apcColListAttrFunctionsStoch->GetColCount(); i++)
	   {
		   wxString l_sSetName = m_apcColListAttrFunctionsStoch->GetColLabel(i);
		   // if (!l_sSetName.Contains(":"))continue;
		   l_sSetName = l_sSetName.BeforeFirst(wxChar(':'));

		   m_pcFunctionStochSetComboBox->Append(l_sSetName);

	   }
	   m_pcFunctionStochSetComboBox->SetSelection(m_apcColListAttrFunctionsStoch->GetActiveList());
   }

}


bool  SP_DLG_ColStUnfolding::IsExistColoringGroup()
{
	SP_DS_Netclass* l_pcNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass();
	SP_DS_Metadataclass* mc;
	if (!l_pcNetClass->GetName().Contains(_T("Fuzzy")))
		mc = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	else
		mc = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc)
	{
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata = NULL;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Group"));
			if (l_pcAttr)
			{
				wxString l_sGroup = l_pcAttr->GetValueString();
				if (l_sGroup == wxT("coloring"))
					return true;
			}
		}
	}
	return false;
}



void SP_DLG_ColStUnfolding::OnModyfyingMarkingGroup(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStMarkingOverview l_cDlg(this);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadOtherSets();


	}
}


void SP_DLG_ColStUnfolding::OnModyfyingFunStochGroup(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_STOCHASTIC_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadOtherSets();
	}
}


void SP_DLG_ColStUnfolding::OnModyfyingFunContSetGroup(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_CONTINUOUS_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadOtherSets();
	}
}

void SP_DLG_ColStUnfolding::OnFunctionStochChanged(wxCommandEvent& p_cEvent)
{
	if (!m_pcFunctionStochSetComboBox || !m_apcColListAttrFunctionsStoch)
		return;

	m_apcColListAttrFunctionsStoch->SetActiveList(m_pcFunctionStochSetComboBox->GetSelection());
	m_apcColListAttrFunctionsStoch->SetActiveColumn(m_pcFunctionStochSetComboBox->GetSelection());
}

void SP_DLG_ColStUnfolding::OnFunctionChanged(wxCommandEvent& p_cEvent)
{
	if (!m_pcFunctionSetComboBox || !m_apcColListAttrFunctions)
		return;

	m_apcColListAttrFunctions->SetActiveList(m_pcFunctionSetComboBox->GetSelection());
	m_apcColListAttrFunctions->SetActiveColumn(m_pcFunctionSetComboBox->GetSelection());

}

void SP_DLG_ColStUnfolding::OnMarkingsetChanged(wxCommandEvent& p_cEvent)
{
	if (!m_pcMarkingSetComboBox || !m_apcColListAttrMarking)
		return;
	 int l_nCurrentFunctionSet = m_pcMarkingSetComboBox->GetSelection();
	m_apcColListAttrMarking->SetActiveList(m_pcMarkingSetComboBox->GetSelection());
	m_apcColListAttrMarking->SetActiveColumn(m_pcMarkingSetComboBox->GetSelection());

}
