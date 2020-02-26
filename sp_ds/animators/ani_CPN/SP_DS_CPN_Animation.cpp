//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// @ Updated: George Assaf
// @ Date:02/02/2020
// Short Description: colored PN animation class
//////////////////////////////////////////////////////////////////////
#include <wx/statline.h>

#include "sp_ds/animators/ani_CPN/SP_DS_CPN_Animation.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

enum
{
	SP_ID_PEDRESET = SP_ID_LAST_ID + 1,
	SP_ID_PEDSET,
	SP_ID_PEDKEEP,
	SP_ID_SHOWBINDDIALOGUE,
	SP_ID_RANDOMCOLOR,
	SP_ID_CHOICE_SETS, //by george
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS, //by george
	SP_ID_STATIC_TEXT_OUTPUT_LABEL,//by george
	SP_ID_DESTROY_ANIM
};

BEGIN_EVENT_TABLE(SP_DS_CPN_Animation, SP_DS_Animation)
EVT_BUTTON(SP_ID_PEDSET, SP_DS_CPN_Animation::OnSet)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_CPN_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_CPN_Animation::OnUpdateUI)
EVT_CHOICE(SP_ID_CHOICE_SETS, SP_DS_CPN_Animation::OnColConstantSetsChanged)// by george
EVT_BUTTON(SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DS_CPN_Animation::OnModifyConstants) //by george
//EVT_CLOSE(SP_DS_CPN_Animation::OnClose)
END_EVENT_TABLE()

SP_DS_CPN_Animation::SP_DS_CPN_Animation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
	SP_DS_Animation(p_nRefresh, p_nDuration),
	m_nStepState((int)p_eStepping),
	m_cbKeep(0),	
	m_pcSingleStep(NULL),
	m_nBindingChoice(1),
	m_bChooseRandomColor(true),
	m_pcDialog(NULL)
	{	 
	m_IsDestructor = false;//by george
	m_nIsClose = 0;
 

	}

 
SP_DS_CPN_Animation::~SP_DS_CPN_Animation()
{

	/***********************by george*******************/
	if (m_nIsClose == SP_ID_DESTROY_ANIM)
	{
		m_IsDestructor = true;
		LoadDefaultConstantsGroups();//reset default constants groups

	    //reset initial marking of places,
		//because the current marking will be in an unfolding style!!
		list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
		for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
			(*l_Iter)->ResetMarking();
 
		 
		SP_DS_ColListAttribute* l_pcColList;
		SP_ListNode::const_iterator l_itElem;
		vector<SP_DS_Node*> l_vPlaceNodes;
		SP_DS_Nodeclass* l_pcNodeclass;


		//go through all places located on the Ped and update its initial marking
		l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				l_vPlaceNodes.push_back(dynamic_cast<SP_DS_Node*>(*l_itElem));
			}
		}
		l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				l_vPlaceNodes.push_back(dynamic_cast<SP_DS_Node*>(*l_itElem));
			}
		}



		int l_nGridRowNumber = 0;
		for (unsigned int l_nPos = 0; l_nPos < l_vPlaceNodes.size(); l_nPos++)
		{
			//obtain the initial marking
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST));
			int l_nBegin = l_nGridRowNumber;
			wxString l_sMainMarking;
			l_vPlaceNodes[l_nPos]->Update(TRUE);

			//re-check: update constants values
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			if (!l_cSyntaxChecking.Initialize())
				return;

			//re-computation of the initial marking after updating the constants values
			l_sMainMarking = wxT("");
			map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
			if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false))
				return;
			map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;
			wxString l_sNodeClass = l_vPlaceNodes[l_nPos]->GetNodeclass()->GetName();

			if (l_sNodeClass == SP_DS_CONTINUOUS_PLACE)
			{
				double l_dMarking = 0;
				for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
				{
					l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
				}
				l_sMainMarking << l_dMarking;
			}
			else
			{
				long l_nMarking = 0;
				for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
				{
					l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
				}
				l_sMainMarking << l_nMarking;
			}


			SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast<SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
			if (l_pcMarkingAttr)
				l_pcMarkingAttr->SetValueString(l_sMainMarking);

			l_vPlaceNodes[l_nPos]->Update(TRUE);
		}

		//set modify flag and refresh the Canvas 
		SP_Core::Instance()->GetRootDocument()->Modify(true);
		Refresh();
		m_nIsClose = 0;
	}

	/******************************************/
 
	if (m_cbKeep) {
		if (m_cbKeep->IsChecked()) {
			KeepMarking();
		} else {
			list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
			for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
				(*l_Iter)->ResetMarking();
			Refresh();
		}
		wxGetApp().GetAnimationPrefs()->SetKeepMarking(m_cbKeep->IsChecked());
		wxDELETE((m_cbKeep));
	}
}

SP_DS_Animation*
SP_DS_CPN_Animation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_CPN_Animation(m_nRefreshFrequ, m_nStepDuration, SP_ANIM_STEP_T(m_nStepState)));
}

bool
SP_DS_CPN_Animation::Initialise(SP_DS_Graph* p_pcGraph)
{
	m_nIsClose = SP_ID_DESTROY_ANIM;//
	bool l_bReturn = SP_DS_Animation::Initialise(p_pcGraph);
	SP_ListAnimator::iterator l_Iter;
	m_lAllPlaceAnimators.clear();
	m_lAllTransAnimators.clear();
	if (l_bReturn)
	{
		for (l_Iter = m_lAllAnimators.begin(); l_Iter != m_lAllAnimators.end(); ++l_Iter)
		{
			SP_DS_Animator* l_pcAnimator = *l_Iter;
			if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_PLACE)
				m_lAllPlaceAnimators.push_back(dynamic_cast<SP_DS_CPN_PlaceAnimator*>(l_pcAnimator));
			else if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_TRANS)
				m_lAllTransAnimators.push_back(dynamic_cast<SP_DS_CPN_TransAnimator*>(l_pcAnimator));
		}
	}
	
	//init animation history
	wxString nc = p_pcGraph->GetNetclass()->GetName();
	m_nHistorySize = wxGetApp().GetAnimationPrefs()->GetHistorySize(nc);
	m_llHistoryTransAnimators.clear();

	if( nc.Cmp(SP_DS_TIMEPN_CLASS) == 0 ){
		SP_MESSAGEBOX(wxT("Attention: time constraints are not considered in animation"));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Here add the codes to check syntax before animation
	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.SyntaxChecking())
		return false;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return l_bReturn;
}

bool
SP_DS_CPN_Animation::PreStep()
{
	list<list<SP_DS_CPN_TransAnimator*> >::iterator l_itHistoryCurrent;
	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;
	unsigned int l_nSteps = m_nSteps / 2;

	m_lPossibleTransAnimators.clear();
	//selecting transitions (candidates) to fire ...
	if (dynamic_cast<SP_DS_CPN_TransAnimator*>(m_pcSingleStep)) {
		//...for steps that are triggered by clicking directly on a transition
		m_lPossibleTransAnimators.push_back(dynamic_cast<SP_DS_CPN_TransAnimator*>(m_pcSingleStep));
		//if back-firing one transition directly, anim history is deleted
		if (GetDirection() == BACKWARD) {
			m_llHistoryTransAnimators.clear();
		}
	} else if (dynamic_cast<SP_DS_CPN_CoarseTransAnimator*>(m_pcSingleStep) )
	{
		//...for steps that are triggered by clicking directly on a coarse transition
		SP_DS_Coarse* l_pcCoarse = m_pcSingleStep->GetAnimObject()->GetCoarse();
		for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
		{
			if((*l_itTrans)->GetAnimObject()->GetGraphicInSubnet(l_pcCoarse->GetNetnumber()))
			{
				m_lPossibleTransAnimators.push_back(*l_itTrans);
			}
		}
	} else if (GetDirection() == BACKWARD && !m_llHistoryTransAnimators.empty()) {
		//...for backsteps using the history
		l_itHistoryCurrent = --(m_llHistoryTransAnimators.end());
		list<SP_DS_CPN_TransAnimator*>& l_rlHistory = (*l_itHistoryCurrent);
		for (l_itTrans = l_rlHistory.begin(); l_itTrans != l_rlHistory.end(); ++ l_itTrans) {
			m_lPossibleTransAnimators.push_back(*l_itTrans);
		}
		m_llHistoryTransAnimators.erase(l_itHistoryCurrent, m_llHistoryTransAnimators.end());
	} else {
		//...for stepping forward and random backsteps
		for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans) {
			m_lPossibleTransAnimators.push_back(*l_itTrans);
		}
	}

	//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
		(*l_itTrans)->InformPrePlaces();
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		(*l_itPlace)->TestMarking();
	//for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	//	(*l_itTrans)->TestConcession(); //do not need for new animator
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		(*l_itPlace)->ResolveConflicts();

	//selecting the transition that are actually able to fire
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans) {
		if ((*l_itTrans)->PreStep(l_nSteps, GetDirection())) {
			m_lStepTransAnimators.push_back((*l_itTrans));
		}
	}

	// all transitions with concession are resolved, now test the stepping option
	ReduceTransitions();

	m_cFiredTransitions.clear();
	for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans) {
		m_cFiredTransitions.push_back((*l_itTrans)->GetAnimObject());
	}
	if (!m_cFiredTransitions.empty()) {
		SP_DS_Graph *graph = m_cFiredTransitions.front()->GetClassObject()->GetParentGraph();
		if (wxGetApp().GetIAManager() && wxGetApp().GetIAManager()->IAModeEnabled(graph)) {
			if (GetDirection() == FORWARD) {
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMPRESTEP_FWD));
			}
		}
	}
	//adding the corresponding places which take part in the process
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace) {
		if ((*l_itPlace)->PreStep(l_nSteps, GetDirection())) {
			m_lStepPlaceAnimators.push_back((*l_itPlace));
		}
	}

	//save current place and transition animators for backstep history
	if (m_nHistorySize > 0 && m_lStepTransAnimators.size() > 0 && GetDirection() == FORWARD) {
		m_llHistoryTransAnimators.push_back(m_lStepTransAnimators);
		//obey history limit
		if (m_llHistoryTransAnimators.size() > m_nHistorySize) {
			m_llHistoryTransAnimators.pop_front();
		}
	}

	// if there are only transitions w/out pre-places...
	if (m_lStepPlaceAnimators.size() == 0) {
		//...skip first part of animation (place to transition),
		m_nActStep = m_nSteps / 2;
	} else {
		//...else just start at the beginning
		m_nActStep++;
	}

	bool l_bReturn = (m_lStepPlaceAnimators.size() + m_lStepTransAnimators.size() > 0);

	if (!l_bReturn) {
		m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
		wxString msg;
		if (m_bOneShot) {
			if (GetSingleStep()) {
				msg = wxT("This transition is not enabled!");
			} else {
				msg =wxT("Dead state: there are no enabled transitions");
			}
		} else {
			msg =wxT("Dead state: there are no more enabled transitions");
		}
		msg = msg + wxT("\n \n * Not enough tokens in preplaces; \n Or \n * Color is out of range in postplaces.");
		SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION );

		if (m_pcDialog) {
			m_pcDialog->ResetPlayButtons();
		}
	} else {
	}

	return l_bReturn;
}

bool
SP_DS_CPN_Animation::Step()
{
	bool l_bReturn = TRUE;
	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;

	if (m_nActStep < (m_nSteps / 2))
	{
		for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
			l_bReturn &= (*l_itPlace)->Step(m_nActStep);
	}
	if (m_nActStep == (m_nSteps / 2))
	{
		for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
			l_bReturn &= (*l_itPlace)->PostStep();
		m_lStepPlaceAnimators.clear();
	}
	if (m_nActStep > (m_nSteps / 2))
	{
		for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
			l_bReturn &= (*l_itTrans)->Step(m_nActStep);
	}

	m_nActStep++;

	return l_bReturn;
}


bool
SP_DS_CPN_Animation::PostStep()
{
	bool l_bReturn = TRUE;
	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;
	m_cFiredTransitions.clear();

	for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
		l_bReturn &= (*l_itPlace)->PostStep();
	for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans) {
		l_bReturn &= (*l_itTrans)->PostStep();
		m_cFiredTransitions.push_back((*l_itTrans)->GetAnimObject());
	}

	m_lStepPlaceAnimators.clear();
	m_lStepTransAnimators.clear();
	m_nActStep = 0;
	m_pcSingleStep = NULL;
	Refresh();

	//submitting an interaction event
	if (!m_cFiredTransitions.empty()) {
		//if any transition fired...
		SP_DS_Graph *graph = m_cFiredTransitions.front()->GetClassObject()->GetParentGraph();
		if (wxGetApp().GetIAManager() && wxGetApp().GetIAManager()->IAModeEnabled(graph)) {
			//... and IAMode is enabled for this animated graph, we submit that event to the SP_IA_Manager
			if (GetDirection() == FORWARD) {
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMSTEP_FWD));
			} else {
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMSTEP_BWD));
			}
		}
	}

	return l_bReturn;
}

bool
SP_DS_CPN_Animation::ReduceTransitions()
{
	if (dynamic_cast<SP_DS_CPN_TransAnimator*>(m_pcSingleStep))
		return TRUE;

	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;
	SP_DS_CPN_TransAnimator* l_pcAnim = NULL;
	if (m_nStepState == SP_ANIM_STEP_SINGLE)
	{
		if (m_lStepTransAnimators.size() > 0) 
		{
			int l_nChose = SP_RandomLong(m_lStepTransAnimators.size());
			int i = 0;
			for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
			{
				if (l_nChose == i++)
					l_pcAnim = (*l_itTrans);
				else
					(*l_itTrans)->Reset();
			}

			vector<wxString> l_vSelBindings;
			int l_nBindSel = SP_RandomLong(l_pcAnim->GetBind2Edge2Mult2Color()->size());
			map<wxString, map<SP_DS_Edge*, map<wxString, int> > >::iterator itMap;
			itMap = l_pcAnim->GetBind2Edge2Mult2Color()->begin();
			for (i = 0; i < l_nBindSel; i++)
				itMap++;
			l_vSelBindings.push_back(itMap->first);
			l_pcAnim->SetSelBindings(l_vSelBindings);

			m_lStepTransAnimators.clear();
			m_lStepTransAnimators.push_back(l_pcAnim);
		}
	}
	else if (m_nStepState == SP_ANIM_STEP_INTERMEDIATE)
	{
		// two iterations, first choose one random entry, second choose a random set of the rest
		SP_LOGDEBUG(wxString::Format(wxT("have %d"), m_lStepTransAnimators.size()));

		list<SP_DS_CPN_TransAnimator*> l_lNewTrans;
		//first round choose one transition randomly
		if (m_lStepTransAnimators.size() > 0) 
		{
			int l_nChose = SP_RandomLong(m_lStepTransAnimators.size());
			int i = 0;
			for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
			{
				if (l_nChose == i++)
				{
					l_lNewTrans.push_back(*l_itTrans);
					m_lStepTransAnimators.erase(l_itTrans);
					break;
				}
			}
		}
		//second round for the other transitions to choose a random set out of them
		for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); l_itTrans = m_lStepTransAnimators.begin())
		{
			if (SP_RandomLong(2) == 1)
			{
				(*l_itTrans)->Reset();
			}
			else
			{
				l_lNewTrans.push_back((*l_itTrans));
			}
			m_lStepTransAnimators.erase(l_itTrans);
		}

		m_lStepTransAnimators.clear();
		
		for (l_itTrans = l_lNewTrans.begin(); l_itTrans != l_lNewTrans.end(); ++l_itTrans)
		{
			m_lStepTransAnimators.push_back((*l_itTrans));

			int l_nBindSel = SP_RandomLong((*l_itTrans)->GetBind2Edge2Mult2Color()->size());
			map<wxString, map<SP_DS_Edge*, map<wxString, int> > >::iterator itMap;
			itMap = (*l_itTrans)->GetBind2Edge2Mult2Color()->begin();
			for (int i = 0; i < l_nBindSel; i++)
				itMap++;		
			vector<wxString> l_vSelBindings;
			l_vSelBindings.push_back(itMap->first);	
			(*l_itTrans)->SetSelBindings(l_vSelBindings);
		}		

		SP_LOGDEBUG(wxString::Format(wxT("chose %d"), m_lStepTransAnimators.size()));
	}
	else
	{
		if (m_lStepTransAnimators.size() > 0) 
		{
			list<SP_DS_CPN_TransAnimator*>::iterator l_itTransAnimator;
			for (l_itTransAnimator = m_lStepTransAnimators.begin(); l_itTransAnimator != m_lStepTransAnimators.end(); ++l_itTransAnimator)
			{				
				map<wxString, map<SP_DS_Edge*, map<wxString, int> > >::iterator itMap;
				vector<wxString> l_vSelBindings;
				for(itMap = (*l_itTransAnimator)->GetBind2Edge2Mult2Color()->begin(); itMap != (*l_itTransAnimator)->GetBind2Edge2Mult2Color()->end(); itMap++)
				{
					l_vSelBindings.push_back(itMap->first);	
				}				
				(*l_itTransAnimator)->SetSelBindings(l_vSelBindings);
			}		
		}
	}

	return TRUE;
}

bool
SP_DS_CPN_Animation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	if (!SP_DS_Animation::AddToDialog(p_pcDlg, p_pcSizer))
		return FALSE;

	wxString l_asChoices[] = { wxT("Maximum"), wxT("Intermediate"), wxT("Single") };
	m_pcChoice = new wxRadioBox(
			p_pcDlg,
			-1,
			wxT("Stepping"),
			wxDefaultPosition,
			wxDefaultSize,
			3,
			l_asChoices,
			1,
			wxRA_SPECIFY_ROWS);

	m_pcChoice->SetSelection(m_nStepState - 1);

	p_pcSizer->Add(m_pcChoice, 0, wxEXPAND | wxALL, 5);


	////////////////binding dialogue
	wxString l_asBindingChoices[] = { wxT("Never"), wxT("More than one binding"), wxT("Always") };
	m_pcBindingChoice = new wxRadioBox(
			p_pcDlg,
			-1,
			wxT("Show binding dialogue"),
			wxDefaultPosition,
			wxDefaultSize,
			3,
			l_asBindingChoices,
			1,
			wxRA_SPECIFY_ROWS);

	m_pcBindingChoice->SetSelection(m_nBindingChoice);

	p_pcSizer->Add(m_pcBindingChoice, 0, wxEXPAND | wxALL, 5);


	//
	m_cbChooseRandomColor = new wxCheckBox(p_pcDlg, SP_ID_RANDOMCOLOR, wxT("Choose random colors."));
	m_cbChooseRandomColor->SetValue(m_bChooseRandomColor);

	p_pcSizer->Add(m_cbChooseRandomColor, 0, wxALL, 5);
	//



	return TRUE;
}

bool
SP_DS_CPN_Animation::OnDialogOk()
{
	if (!m_pcChoice)
		return FALSE;

	m_nStepState = m_pcChoice->GetSelection() + 1;

	m_nBindingChoice = m_pcBindingChoice->GetSelection();

	m_bChooseRandomColor = m_cbChooseRandomColor->GetValue();

	return SP_DS_Animation::OnDialogOk();
}

bool
SP_DS_CPN_Animation::AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer)
{
	if (!p_pcCtrl || !p_pcSizer)
		return FALSE;

	m_pcDialog = p_pcCtrl;

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDSET, wxT("Keep Marking")), 1, wxALL | wxEXPAND, 5);
	m_cbKeep = new wxCheckBox(p_pcCtrl, SP_ID_PEDKEEP, wxT("Always keep marking when closing."));
	m_cbKeep->SetValue(wxGetApp().GetAnimationPrefs()->GetKeepMarking());
	p_pcSizer->Add(m_cbKeep, 0, wxALL, 5);

	p_pcCtrl->PushEventHandler(this);

	//allow backstepping
	m_pcDialog->EnableBackStepping(true);


	/*****************george*********************/
	wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcOutputLabelSizer = new wxBoxSizer(wxVERTICAL);

	m_pcOutputLabelStaticText = new wxStaticText(p_pcCtrl, SP_ID_STATIC_TEXT_OUTPUT_LABEL, wxT("Start ..."), wxDefaultPosition, wxDefaultSize);

	
	UpdateChoices();
	SP_SetString::iterator l_itChoice;
	for (l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
		wxString l_sGroup = *l_itChoice;
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, l_sGroup + wxT(':')), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes1.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_SETS, wxDefaultPosition, wxSize(100, -1), 0, NULL, 0, wxDefaultValidator, l_sGroup));
		l_pcRowSizer->Add(m_apcComboBoxes1[m_apcComboBoxes1.size() - 1], 0, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_CONSTANT_SETS, wxT("Modify")), 0, wxALL, 5);
		l_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	}
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(m_pcOutputLabelStaticText, 1, wxALL, 5);
	p_pcSizer->Add(l_pcOutputLabelSizer, 0, wxALL, 5);
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(l_pcSetsSizer, 0, wxALL, 5);
	 
	LoadSets();
	/**************************************/
	
	return TRUE;
}

void
SP_DS_CPN_Animation::OnReset()
{
	
	/////////////george/////////////////////
	for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
	{
		if (m_apcColListAttrForConstants[i])
		{
			int vsel = m_apcComboBoxes1[i]->GetSelection();

			m_apcColListAttrForConstants[i]->SetActiveList(vsel);

		}
	}

	UpdateColMarking();


	/////////////////////////////////////////
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		//(*l_Iter)->ResetMarking();/commented for reseting the initial marking to the coressponding selected value set

	m_llHistoryTransAnimators.clear();

	Refresh();
}

void
SP_DS_CPN_Animation::OnSet(wxCommandEvent& p_cEvent)
{
	KeepMarking();
}

void
SP_DS_CPN_Animation::KeepMarking()
{
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->SetMarking();
	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

}

void
SP_DS_CPN_Animation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
	if (p_cEvent.GetId() == SP_ID_PEDSET)
	{
		p_cEvent.Enable(!m_bRunning && !m_cbKeep->IsChecked());
	}
	else
	{
		p_cEvent.Enable(!m_bRunning);
	}
}



void  SP_DS_CPN_Animation::UpdateChoices()//george
{
	m_choices.clear();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
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
				m_choices.insert(l_sGroup);
			}
		}
	}
}



void SP_DS_CPN_Animation::LoadSets()
{
	m_apcColListAttrForConstants.clear();
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);

	SP_ListMetadata::const_iterator l_itElem;

	SP_SetString::iterator l_itChoice;
	for (l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
	   

		wxString l_sChoice = *l_itChoice;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			if (l_sChoice == l_sGroup)
			{
				m_apcColListAttrForConstants.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				//m_bUserInteract = true;
				break;
			}
		}
	}


	for (size_t j = 0; j < m_apcColListAttrForConstants.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttrForConstants[j];
		wxChoice* l_pcCombobox = m_apcComboBoxes1[j];
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

	/**********************************/

	m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	//m_bRestartAnimationFlag = true;
}

void SP_DS_CPN_Animation::OnModifyConstants(wxCommandEvent& p_cEvent)
{
	SP_DLG_ConstantDefinition * l_pcConstantDialog = new SP_DLG_ConstantDefinition(NULL);

	if (l_pcConstantDialog->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

}

void SP_DS_CPN_Animation::OnColConstantSetsChanged(wxCommandEvent& p_cEvent)
{
	for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
	{
		if (m_apcColListAttrForConstants[i])
		{
			int vsel = m_apcComboBoxes1[i]->GetSelection();

			m_apcColListAttrForConstants[i]->SetActiveList(vsel);

		}
	}

	UpdateColMarking();
	 
}

void SP_DS_CPN_Animation::UpdateColMarking()
{

	list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();

	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

	//////////////////////////////////////////////////
	//update marking here

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if (!l_pcGraph)
		return;
	SP_DS_ColListAttribute* l_pcColList;

	SP_ListNode::const_iterator l_itElem;
	vector<SP_DS_Node*> l_vPlaceNodes;
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = l_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back(dynamic_cast<SP_DS_Node*>(*l_itElem));
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			l_vPlaceNodes.push_back(dynamic_cast<SP_DS_Node*>(*l_itElem));
		}
	}



	int l_nGridRowNumber = 0;


	for (unsigned int l_nPos = 0; l_nPos < l_vPlaceNodes.size(); l_nPos++)
	{

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST));

		//update it the first time
		l_vPlaceNodes[l_nPos]->Update(TRUE);


		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.Initialize())
			return;


		////////////////////////////////////////////////////////
		wxString l_sMainMarking = wxT("");
		map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false))
			return;
		map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;

		wxString l_sNodeClass = l_vPlaceNodes[l_nPos]->GetNodeclass()->GetName();

		if (l_sNodeClass == SP_DS_CONTINUOUS_PLACE)
		{
			double l_dMarking = 0;
			for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
			}
			l_sMainMarking << l_dMarking;
		}
		else
		{
			long l_nMarking = 0;
			for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
			}
			l_sMainMarking << l_nMarking;
		}
		////////////////////////////////////////////////////////////////

		SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast< SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
		if (l_pcMarkingAttr)
			l_pcMarkingAttr->SetValueString(l_sMainMarking);

		l_vPlaceNodes[l_nPos]->Update(TRUE);
	}


	SP_Core::Instance()->GetRootDocument()->Modify(true);

	////////////////////////////////////////////////////////
	if (!m_IsDestructor)
		LoadSets();
	 



	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->UpdateDefaultMarking();
	}
	SP_Core::Instance()->GetRootDocument()->Refresh();
	Refresh();
}


void SP_DS_CPN_Animation::LoadDefaultConstantsGroups()
{
	for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
	{
		if (m_apcColListAttrForConstants[i])
		{
			int vsel = 0;

			m_apcColListAttrForConstants[i]->SetActiveList(vsel);

		}
	}
//	UpdateColMarking();
}