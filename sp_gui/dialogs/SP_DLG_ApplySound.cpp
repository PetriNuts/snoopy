//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/01 11:20:00 $
// Short Description: Dialog class to choose algorithms from, that
//				      add sounds to petri nets automatically
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/combobox.h>

#include "sp_gui/dialogs/SP_DLG_ApplySound.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_revision.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_core/SP_GPR_Animation.h"


enum
{

	SP_ID_RADIO_NETSTRUCTURE,
	SP_ID_RADIO_APPLYTOALL,
	SP_ID_RADIO_APPLYTOSELECTED,

	SP_ID_CHECKBOX_CONSUMING,
	SP_ID_CHECKBOX_PRODUCING,
	SP_ID_CHECKBOX_PRESERVING,

	SP_ID_BUTTON_CONSUMING,
	SP_ID_BUTTON_PRODUCING,
	SP_ID_BUTTON_PRESERVING,

	SP_ID_TEXTCTRL_CONSUMING,
	SP_ID_TEXTCTRL_PRODUCING,
	SP_ID_TEXTCTRL_PRESERVING

};

BEGIN_EVENT_TABLE( SP_DLG_ApplySound, wxDialog )

EVT_BUTTON( wxID_OK, SP_DLG_ApplySound::OnDlgOk )
EVT_BUTTON( wxID_CANCEL, SP_DLG_ApplySound::OnDlgCancel )

EVT_CHECKBOX(SP_ID_CHECKBOX_CONSUMING, SP_DLG_ApplySound::OnNetStructureCheckBoxClicked)
EVT_CHECKBOX(SP_ID_CHECKBOX_PRODUCING, SP_DLG_ApplySound::OnNetStructureCheckBoxClicked)
EVT_CHECKBOX(SP_ID_CHECKBOX_PRESERVING, SP_DLG_ApplySound::OnNetStructureCheckBoxClicked)

EVT_BUTTON( SP_ID_BUTTON_CONSUMING, SP_DLG_ApplySound::OnBrowseButtonClicked)
EVT_BUTTON( SP_ID_BUTTON_PRODUCING, SP_DLG_ApplySound::OnBrowseButtonClicked)
EVT_BUTTON( SP_ID_BUTTON_PRESERVING, SP_DLG_ApplySound::OnBrowseButtonClicked)

END_EVENT_TABLE()

SP_DLG_ApplySound::SP_DLG_ApplySound(const wxString& p_sAttrName, wxWindow* p_pcParent,
									 const wxString& p_sTitle, long p_nStyle) :

wxDialog(p_pcParent, -1, p_sTitle, wxPoint( 120, 120), wxSize( 500, 300),
		 p_nStyle | wxSTAY_ON_TOP)
{
	m_pcParent = p_pcParent;
	m_lAttrList.clear();
	m_sLibrary = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS);

	m_pcSizer = new wxBoxSizer( wxVERTICAL );

	////////////// METHODS ///////////////

	wxStaticBoxSizer *l_pcMethodSectionSizer = new wxStaticBoxSizer(new wxStaticBox(
		this, -1, wxT("Method")), wxVERTICAL);

	////////////// METHOD1: NET STRUCTURE ///////////////

	wxBoxSizer* l_pcNetStructureSizer = new wxBoxSizer(wxVERTICAL);

	m_pcNetStructureRadioButton = new wxRadioButton(this, SP_ID_RADIO_NETSTRUCTURE, wxT("Net Structure"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	l_pcNetStructureSizer->Add(m_pcNetStructureRadioButton, 0, wxLEFT | wxTOP, 15);


	wxBoxSizer* l_pcNetStructureOptionsSizer = new wxBoxSizer(wxVERTICAL);

	m_pcNetStructureConsumingCheckBox = new wxCheckBox(this, SP_ID_CHECKBOX_CONSUMING,
			wxT("Token consuming transitions"));
	l_pcNetStructureOptionsSizer->Add(m_pcNetStructureConsumingCheckBox, 0, wxALL, 5);

	wxBoxSizer* l_pcNetStructureConsumingSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcNetStructureConsumingCtrl = new wxTextCtrl(this, SP_ID_TEXTCTRL_CONSUMING, wxEmptyString,
		wxDefaultPosition, wxSize(220, 22), 0);
	l_pcNetStructureConsumingSizer->Add(m_pcNetStructureConsumingCtrl, 0, wxALL, 0);

	m_pcNetStructureConsumingButton = new wxButton(this, SP_ID_BUTTON_CONSUMING, wxT("Browse"));
	l_pcNetStructureConsumingSizer->Add(m_pcNetStructureConsumingButton, 0, wxLEFT | wxBOTTOM, 5);

	l_pcNetStructureOptionsSizer->Add(l_pcNetStructureConsumingSizer, 0, wxALL, 5);


	m_pcNetStructureProducingCheckBox = new wxCheckBox(this, SP_ID_CHECKBOX_PRODUCING,
			wxT("Token producing transitions"));
	l_pcNetStructureOptionsSizer->Add(m_pcNetStructureProducingCheckBox, 0, wxALL, 5);

	wxBoxSizer* l_pcNetStructureProducingSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcNetStructureProducingCtrl = new wxTextCtrl(this, SP_ID_TEXTCTRL_PRODUCING, wxEmptyString,
		wxDefaultPosition, wxSize(220, 22), 0);
	l_pcNetStructureProducingSizer->Add(m_pcNetStructureProducingCtrl, 0, wxALL, 0);

	m_pcNetStructureProducingButton = new wxButton(this, SP_ID_BUTTON_PRODUCING, wxT("Browse"));
	l_pcNetStructureProducingSizer->Add(m_pcNetStructureProducingButton, 0, wxLEFT | wxBOTTOM, 5);

	l_pcNetStructureOptionsSizer->Add(l_pcNetStructureProducingSizer, 0, wxALL, 5);


	m_pcNetStructurePreservingCheckBox = new wxCheckBox(this, SP_ID_CHECKBOX_PRESERVING,
			wxT("Token preserving transitions"));
	l_pcNetStructureOptionsSizer->Add(m_pcNetStructurePreservingCheckBox, 0, wxALL, 5);

	wxBoxSizer* l_pcNetStructurePreservingSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcNetStructurePreservingCtrl = new wxTextCtrl(this, SP_ID_TEXTCTRL_PRESERVING, wxEmptyString,
		wxDefaultPosition, wxSize(220, 22), 0);
	l_pcNetStructurePreservingSizer->Add(m_pcNetStructurePreservingCtrl, 0, wxALL, 0);

	m_pcNetStructurePreservingButton = new wxButton(this, SP_ID_BUTTON_PRESERVING, wxT("Browse"));
	l_pcNetStructurePreservingSizer->Add(m_pcNetStructurePreservingButton, 0, wxLEFT | wxBOTTOM, 5);

	l_pcNetStructureOptionsSizer->Add(l_pcNetStructurePreservingSizer, 0, wxALL, 5);


	l_pcNetStructureSizer->Add(l_pcNetStructureOptionsSizer, 0, wxLEFT, 40);

	l_pcMethodSectionSizer->Add(l_pcNetStructureSizer, 0, wxALL, 10);

	m_pcSizer->Add(l_pcMethodSectionSizer, 0, wxALL, 10);


	// preselect options
	m_pcNetStructureConsumingCheckBox->SetValue(true);
	m_pcNetStructureProducingCheckBox->SetValue(true);
	m_pcNetStructurePreservingCheckBox->SetValue(true);


	//////////// general options  //////////////

	wxStaticBoxSizer *l_pcGeneralOptionsSizer = new wxStaticBoxSizer(new wxStaticBox(
		this, -1, wxT("General options")), wxVERTICAL);

	m_pcApplyToAllRadioButton = new wxRadioButton(this, SP_ID_RADIO_APPLYTOALL, wxT("Apply to all elements"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	//m_pcApplyToAllRadioButton->Enable(false);
	l_pcGeneralOptionsSizer->Add(m_pcApplyToAllRadioButton, 0, wxLEFT | wxALL, 5);

	m_pcApplyToSelectedRadioButton = new wxRadioButton(this, SP_ID_RADIO_APPLYTOSELECTED,
			wxT("Apply to selected elements only"));
	//m_pcApplyToSelectedRadioButton->Enable(false);
	l_pcGeneralOptionsSizer->Add(m_pcApplyToSelectedRadioButton, 0, wxLEFT | wxALL, 5);

	m_pcSizer->Add(l_pcGeneralOptionsSizer, 0, wxALL | wxEXPAND, 10);


	/////////// general dialog buttons /////////////

	wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

	m_pcSizer->Add(l_pcButtonSizer, 0, wxALIGN_RIGHT);

	SetSizerAndFit(m_pcSizer);

	LoadSettings();

}


void SP_DLG_ApplySound::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if (Validate() && TransferDataFromWindow() )
	{
		if (IsModal() )
		{
			ApplySoundsByNetStructure();
			SaveSettings();
			EndModal(wxID_OK);
		}
		else
		{
			SetReturnCode(wxID_OK);
			this->Show( FALSE);
		}
	}
}


void SP_DLG_ApplySound::OnDlgCancel(wxCommandEvent& p_cEvent)
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


void
SP_DLG_ApplySound::OnNetStructureCheckBoxClicked(wxCommandEvent& p_cEvent)
{
	bool l_bEnabled;
	switch (p_cEvent.GetId()) {

		case SP_ID_CHECKBOX_CONSUMING:
			l_bEnabled = m_pcNetStructureConsumingCheckBox->IsChecked();
			m_pcNetStructureConsumingButton->Enable(l_bEnabled);
			m_pcNetStructureConsumingCtrl->Enable(l_bEnabled);
			break;
		case SP_ID_CHECKBOX_PRODUCING:
			l_bEnabled = m_pcNetStructureProducingCheckBox->IsChecked();
			m_pcNetStructureProducingButton->Enable(l_bEnabled);
			m_pcNetStructureProducingCtrl->Enable(l_bEnabled);
			break;
		case SP_ID_CHECKBOX_PRESERVING:
			l_bEnabled = m_pcNetStructurePreservingCheckBox->IsChecked();
			m_pcNetStructurePreservingButton->Enable(l_bEnabled);
			m_pcNetStructurePreservingCtrl->Enable(l_bEnabled);
			break;
	}
}

void
SP_DLG_ApplySound::OnBrowseButtonClicked(wxCommandEvent& p_cEvent)
{

	wxString l_sOldDir;
	switch (p_cEvent.GetId()) {

		case SP_ID_BUTTON_CONSUMING:
			l_sOldDir = m_pcNetStructureConsumingCtrl->GetValue();
			break;
		case SP_ID_BUTTON_PRODUCING:
			l_sOldDir = m_pcNetStructureProducingCtrl->GetValue();;
			break;
		case SP_ID_BUTTON_PRESERVING:
			l_sOldDir = m_pcNetStructurePreservingCtrl->GetValue();
			break;
	}
	const wxString& dir = wxDirSelector(wxT("Choose a folder"), l_sOldDir, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);

	if (dir.empty())
		return;

	wxString l_sLibrary = wxGetApp().GetAnimationPrefs()->GetSoundPath(SP_DS_MUSICPN_CLASS);
	if (!dir.StartsWith(l_sLibrary))
	{
		SP_MESSAGEBOX(wxT("The directory needs to be within the Sound Library!"),
				wxT("Invalid directory"),
			wxOK | wxICON_ERROR);
		return;
	}

	// set text of the respective text control
	switch (p_cEvent.GetId()) {

		case SP_ID_BUTTON_CONSUMING:
			m_pcNetStructureConsumingCtrl->SetValue(dir);
			break;
		case SP_ID_BUTTON_PRODUCING:
			m_pcNetStructureProducingCtrl->SetValue(dir);
			break;
		case SP_ID_BUTTON_PRESERVING:
			m_pcNetStructurePreservingCtrl->SetValue(dir);
			break;
	}
}


bool
SP_DLG_ApplySound::ApplySounds(SP_ListNode &p_Transitions, const wxString& p_sSoundDir)
{
	// load the sounds from the directory into a list
	SP_ListString l_aSounds;
	wxDir dir(p_sSoundDir);
	wxString filename;

	wxChar l_Separator = wxFileName::GetPathSeparator();

	bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
	while (cont)
	{
		wxFileName l_Name(p_sSoundDir + l_Separator + filename);
		if (this->IsSupportedFile(l_Name))
			l_aSounds.push_back(p_sSoundDir + l_Separator + filename);
		cont = dir.GetNext(&filename);
	}

	if (l_aSounds.empty())
	{
		return false;
	}

	// helpers for the loop
	SP_ListNode::const_iterator l_TransIt;
	SP_ListString::iterator l_SoundIt;
	l_SoundIt = l_aSounds.begin();
	wxString l_sRowName;
	wxString l_sSound;
	for (l_TransIt = p_Transitions.begin(); l_TransIt != p_Transitions.end(); ++l_TransIt)
	{
		// reset sound iterator, if directory does not contain enough
		// files for the amount of transitions
		if (l_SoundIt == l_aSounds.end())
			l_SoundIt = l_aSounds.begin();

		SP_LOGDEBUG(wxString::Format(wxT("transition %d"), (*l_TransIt)->GetId()));
		SP_LOGDEBUG(wxT("sound") + (*l_SoundIt));

		if(SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*l_TransIt)->GetAttribute(wxT("Sound"))))
		{
			//l_sRowName = wxString::Format("List %d", l_pcColAttr->GetRowCount()+1);
			l_sSound = (*l_SoundIt);
			l_sSound.Mid(m_sLibrary.size()+1);//remove absolute path
			//l_pcColAttr->AppendRow(&l_sRowName);
			l_pcColAttr->SetCell(l_pcColAttr->GetRowCount()-1, 1, l_sSound);

		}

		++l_SoundIt;
	}




	return true;
}


void
SP_DLG_ApplySound::ApplySoundsByNetStructure()
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(wxGetApp().GetDocmanager()->GetCurrentDocument());
	SP_DS_Graph* l_pcGraph = l_pcDoc->GetGraph();

	SP_DS_Edgeclass* l_pcEdgeClass =  l_pcGraph->GetEdgeclass(wxT("Edge"));

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plEdges = l_pcEdgeClass->GetElements();


	map<SP_DS_Node*, long> l_mIn; //transitions and number of their incoming edges
	map<SP_DS_Node*, long> l_mOut; //transitions and number of their outgoing edges
	SP_ListNode l_lAllTransitions;

	// now walk through every edge and put source and target transitions
	// into the respactive maps. if a transition is already in one of these
	// maps, we found another edge, thus incrementing the number found edges.
	// meanwhile, store all found transitions in a list for later

	//little helper
	SP_DS_Node* l_pCurrent;
	wxString l_sSource;
	wxString l_sTarget;
	for (l_Iter = l_plEdges->begin(); l_Iter != l_plEdges->end(); ++l_Iter)
	{

		if(SP_DS_MultiplicityAttribute* l_pMultiAttr = dynamic_cast<SP_DS_MultiplicityAttribute*>(
			(*l_Iter)->GetAttribute(wxT("Multiplicity"))))
		{
			long l_nMultipl = l_pMultiAttr->GetValue();

			l_sSource = (*l_Iter)->GetSource()->GetClassObject()->GetName();
			if (l_sSource.Cmp(wxT("Transition")) == 0)
			{
				l_pCurrent = dynamic_cast<SP_DS_Node*>((*l_Iter)->GetSource());

				if (l_mOut.find(l_pCurrent) != l_mOut.end())
					l_mOut[l_pCurrent] += l_nMultipl;
				else l_mOut[l_pCurrent] = l_nMultipl;

				if (SP_Find(l_lAllTransitions, l_pCurrent) == l_lAllTransitions.end())
					l_lAllTransitions.push_back(l_pCurrent);
			}

			l_sTarget = (*l_Iter)->GetTarget()->GetClassObject()->GetName();
			if (l_sTarget.Cmp(wxT("Transition")) == 0)
			{
				l_pCurrent = dynamic_cast<SP_DS_Node*>((*l_Iter)->GetTarget());

				if (l_mIn.find(l_pCurrent) != l_mIn.end())
					l_mIn[l_pCurrent] += l_nMultipl;
				else l_mIn[l_pCurrent] = l_nMultipl;

				if (SP_Find(l_lAllTransitions, l_pCurrent) == l_lAllTransitions.end())
					l_lAllTransitions.push_back(l_pCurrent);
			}
		}
	}

	// add a new row (to the collist attribute) to all transitions
	SP_ListNode::const_iterator l_TransIt;
	wxString l_sRowName;
	for (l_TransIt = l_lAllTransitions.begin(); l_TransIt != l_lAllTransitions.end(); ++l_TransIt)
	{
		if(SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>((*l_TransIt)->GetAttribute(wxT("Sound"))))
		{
			l_sRowName = wxString::Format(wxT("List %d"), l_pcColAttr->GetRowCount()+1);
			unsigned int l_nNewRow = l_pcColAttr->AppendEmptyRow();
			l_pcColAttr->SetCell(l_nNewRow, 0, l_sRowName);
			l_pcColAttr->SetCell(l_nNewRow, 1, wxT(""));
		}
	}

	if (this->m_pcApplyToSelectedRadioButton->GetValue())
	{

		//TODO get selected elements
		SP_Data *l_pcElement;
		wxNode *l_pcNode = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();
		while (l_pcNode) {
			SP_Graphic *l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
			if (l_pcGraphic && !l_pcGraphic->GetPrimitive()->Selected())
			{
				l_pcElement = l_pcGraphic->GetParent();

				if (l_pcElement->GetElementType() == SP_ELEMENT_NODE)
				{
					wxString l_sName = l_pcElement->GetClassObject()->GetName();
					if (l_sName.Cmp(wxT("Transition")) == 0)
					{
						l_pCurrent = dynamic_cast<SP_DS_Node*>(l_pcElement);
						l_lAllTransitions.remove(l_pCurrent);
					}
				}
			}
			l_pcNode = l_pcNode->GetNext();
		}
	}
	/////////////////////////////////

	SP_ListNode l_plConsuming;
	SP_ListNode l_plProducing;
	SP_ListNode l_plPreserving;

	// now walk through the list of all transitions, find out if they are
	// token consuming, preserving or producing, by checking the maps and
	// put them into the respective lists

	SP_ListNode::const_iterator l_TransIter;
	int l_nIn = 0;
	int l_nOut = 0;
	for (l_TransIter = l_lAllTransitions.begin(); l_TransIter != l_lAllTransitions.end(); ++l_TransIter)
	{
		// get number of incoming edges
		if (l_mIn.find(*l_TransIter) != l_mIn.end())
			l_nIn = l_mIn[*l_TransIter];

		// get number of outgoing edges
		if (l_mOut.find(*l_TransIter) != l_mOut.end())
			l_nOut = l_mOut[*l_TransIter];

		// append transition to respective list
		if (l_nIn > l_nOut)
			l_plConsuming.push_back(*l_TransIter);
		else if (l_nIn < l_nOut)
			l_plProducing.push_back(*l_TransIter);
		else
			l_plPreserving.push_back(*l_TransIter);

		l_nIn = 0;
		l_nOut = 0;
	}

	if (m_pcNetStructureConsumingCheckBox->IsChecked())
		ApplySounds(l_plConsuming, m_pcNetStructureConsumingCtrl->GetValue());

	if (m_pcNetStructureProducingCheckBox->IsChecked())
		ApplySounds(l_plProducing, m_pcNetStructureProducingCtrl->GetValue());

	if (m_pcNetStructurePreservingCheckBox->IsChecked())
		ApplySounds(l_plPreserving, m_pcNetStructurePreservingCtrl->GetValue());

	SP_LOGDEBUG(wxString::Format(wxT("consuming %d"), l_plConsuming.size()));
	SP_LOGDEBUG(wxString::Format(wxT("producing %d"), l_plProducing.size()));
	SP_LOGDEBUG(wxString::Format(wxT("preserving %d"), l_plPreserving.size()));

}



void
SP_DLG_ApplySound::SaveSettings()
{

	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
	config.Write(wxString::Format(wxT("ConsumingPath")), m_pcNetStructureConsumingCtrl->GetValue());
	config.Write(wxString::Format(wxT("ProducingPath")), m_pcNetStructureProducingCtrl->GetValue());
	config.Write(wxString::Format(wxT("PreservingPath")), m_pcNetStructurePreservingCtrl->GetValue());
}

void
SP_DLG_ApplySound::LoadSettings()
{

	wxString l_sTemp;
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);

	config.Read(wxString::Format(wxT("ConsumingPath")), &l_sTemp, m_sLibrary);
	m_pcNetStructureConsumingCtrl->SetValue(l_sTemp);
	SP_LOGDEBUG(l_sTemp);

	config.Read(wxString::Format(wxT("ProducingPath")), &l_sTemp, m_sLibrary);
	m_pcNetStructureProducingCtrl->SetValue(l_sTemp);
	SP_LOGDEBUG(l_sTemp);

	config.Read(wxString::Format(wxT("PreservingPath")), &l_sTemp, m_sLibrary);
	m_pcNetStructurePreservingCtrl->SetValue(l_sTemp);
	SP_LOGDEBUG(l_sTemp);
}

bool
SP_DLG_ApplySound::IsSupportedFile(wxFileName& p_fileName)
{
	// supported formats
	wxArrayString l_aFormats;
	l_aFormats.push_back(wxT("wav"));
	l_aFormats.push_back(wxT("ogg"));
	l_aFormats.push_back(wxT("mod"));
	l_aFormats.push_back(wxT("it"));
	l_aFormats.push_back(wxT("s3m"));
	l_aFormats.push_back(wxT("xm"));

	// check file extension against list above
	for (size_t i = 0; i < l_aFormats.size(); ++i)
	{
		if (p_fileName.GetExt().CmpNoCase(l_aFormats[i]) == 0)
		{
			return true;
		}
	}
	return false;
}


