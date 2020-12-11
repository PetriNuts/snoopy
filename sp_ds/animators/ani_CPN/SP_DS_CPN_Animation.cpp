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


#ifndef __WXMSW__
#include "bitmaps/play_export_icon.xpm"
#include "bitmaps/pause_export_icon.xpm"
#include "bitmaps/recording_export_icon.xpm"
#endif

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
	SP_ID_DESTROY_ANIM,
	SP_ID_PEDEXPORT,//by george 10.2020
	SP_ID_PEDIMPORT,//by george  10.2020
	SP_ID_RADIO_BUTTON_RECORD,
	SP_ID_RADIO_BUTTON_REPLAY
};

BEGIN_EVENT_TABLE(SP_DS_CPN_Animation, SP_DS_Animation)
EVT_BUTTON(SP_ID_PEDSET, SP_DS_CPN_Animation::OnSet)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_CPN_Animation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_CPN_Animation::OnUpdateUI)
EVT_CHOICE(SP_ID_CHOICE_SETS, SP_DS_CPN_Animation::OnColConstantSetsChanged)// by george
EVT_BUTTON(SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DS_CPN_Animation::OnModifyConstants) //by george
EVT_UPDATE_UI(SP_ID_PEDIMPORT, SP_DS_CPN_Animation::OnUpdateUI)//george on 10.2020
EVT_UPDATE_UI(SP_ID_PEDEXPORT, SP_DS_CPN_Animation::OnUpdateUI)//george on 10.2020
EVT_BUTTON(SP_ID_PEDEXPORT, SP_DS_CPN_Animation::OnExport)//george on 10.2020
EVT_BUTTON(SP_ID_PEDIMPORT, SP_DS_CPN_Animation::OnImport)//george on 10.2020
EVT_UPDATE_UI(SP_ID_RADIO_BUTTON_RECORD, SP_DS_CPN_Animation::OnUpdateUI)//george on 10.2020
EVT_UPDATE_UI(SP_ID_RADIO_BUTTON_REPLAY, SP_DS_CPN_Animation::OnUpdateUI)//george on 10.2020
EVT_BUTTON(SP_ID_RADIO_BUTTON_RECORD, SP_DS_CPN_Animation::OnRecord)//george on 10.2020
EVT_BUTTON(SP_ID_RADIO_BUTTON_REPLAY, SP_DS_CPN_Animation::OnReplay)//george on 10.2020
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
	m_IsDestructor = false;
	m_nIsClose = 0;
	m_bIsKeepClicked = false;
	m_nStart=0;
	m_nEvery=0;
	m_nStop=0;
	m_nMarkingOption=0;
	m_bAutoConcurrency = false;
	m_nStepCount=0; //Keeps a count of step number
	m_bImport=false;
    m_bInvalid = false;
    m_ncoloringChoiceValue = 0;
    m_bExport=false;
    m_bExportFlag=false;
    m_bImportFlag=false;
	}

 
SP_DS_CPN_Animation::~SP_DS_CPN_Animation()
{

	bool l_bIsKeep = m_bIsKeepClicked;
		if (m_cbKeep) {
			if (m_cbKeep->IsChecked()) {
				KeepMarking();
				l_bIsKeep = true;
			}
			else if(!l_bIsKeep){
				list<SP_DS_CPN_PlaceAnimator*>::iterator l_Iter;
				for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
					(*l_Iter)->ResetMarking();
				Refresh();
			}
			wxGetApp().GetAnimationPrefs()->SetKeepMarking(m_cbKeep->IsChecked());
			wxDELETE((m_cbKeep));
		}
		if (l_bIsKeep)
		{
			UpdateMarkingPlaces();
	    }
		/***********************by george*******************/
		if (m_nIsClose == SP_ID_DESTROY_ANIM && !l_bIsKeep)
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
				if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false,true))
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

		if(m_cbKeep)
		wxDELETE((m_cbKeep));

}

void SP_DS_CPN_Animation::UpdateMarkingPlaces()
{
	//update marking and the canvas layout of the places after they get updated
	SP_DS_ColListAttribute* l_pcColList1;
	SP_ListNode::const_iterator l_itElement;
	vector<SP_DS_Node*> l_vPlaceNodesinPed;
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElement = l_pcNodeclass->GetElements()->begin(); l_itElement != l_pcNodeclass->GetElements()->end(); ++l_itElement)
		{
			l_vPlaceNodesinPed.push_back(dynamic_cast<SP_DS_Node*>(*l_itElement));
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElement = l_pcNodeclass->GetElements()->begin(); l_itElement != l_pcNodeclass->GetElements()->end(); ++l_itElement)
		{
			l_vPlaceNodesinPed.push_back(dynamic_cast<SP_DS_Node*>(*l_itElement));
		}
	}
	for (unsigned int l_nPos = 0; l_nPos < l_vPlaceNodesinPed.size(); l_nPos++)
	{

		l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*>(l_vPlaceNodesinPed[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST));
		l_vPlaceNodesinPed[l_nPos]->Update(TRUE);

		if (l_pcColList1->GetRowCount() == 0)
	 	{
		   l_pcColList1->AppendEmptyRow();
			l_pcColList1->SetCell(0, 0, _T("all()"));
			l_pcColList1->SetCell(0, 1, _T("0"));
			l_pcColList1->SetCell(0, 2, _T("()"));
		}
		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.Initialize())
			continue;


		////////////////////////////////////////////////////////
		wxString l_sMainMarking;
		l_sMainMarking = wxT("");
		map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodesinPed[l_nPos], l_mColorToMarkingMap, false,true))
			continue;
		map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;

		wxString l_sNodeClass = l_vPlaceNodesinPed[l_nPos]->GetNodeclass()->GetName();

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

		SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast<SP_DS_Attribute*>(l_vPlaceNodesinPed[l_nPos]->GetAttribute(wxT("Marking")));
		if (l_pcMarkingAttr)
			l_pcMarkingAttr->SetValueString(l_sMainMarking);

		l_vPlaceNodesinPed[l_nPos]->Update(TRUE);

	}
}

SP_DS_Animation*
SP_DS_CPN_Animation::Clone()
{
	wxString l_sOptions = SP_Core::Instance()->GetActivatedRefreshDurationanim();
		if (l_sOptions != wxT(""))
		{//by george to recall the refresh rate and duration values
			long l_nRefres, l_nDuration;
			wxString l_sRef = l_sOptions.BeforeFirst(wxChar('|'));
			wxString l_sDuration = l_sOptions.AfterFirst(wxChar('|'));
			if (!l_sRef.ToLong(&l_nRefres))
			{
				l_nRefres = m_nRefreshFrequ;
			}
			if (!l_sDuration.ToLong(&l_nDuration))
			{
				l_nDuration = m_nStepDuration;
			}

			return dynamic_cast<SP_DS_Animation*>(new SP_DS_CPN_Animation(l_nRefres, l_nDuration, SP_ANIM_STEP_T(m_nStepState)));
		}

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


	if (m_bExport == true && m_bExportFlag)
		ExportMarkings();

	m_lPossibleTransAnimators.clear();
	if (!m_bImportFlag)
	{

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

	}

	else if (m_bImportFlag)//by george
		if(!ImportStepSequences())
			{
			//SP_MESSAGEBOX(wxT("No more entries in the file"), wxT("Notification"), wxOK | wxICON_INFORMATION);
					ResetReplayButton();
					int choose = SP_MESSAGEBOX(wxT("No more entries in the file. Close the imported file?"), wxT("Notification"), wxOK | wxNO | wxICON_INFORMATION);
					if (choose == 2)
					{
						m_bImport = false;

						// m_pcRadioBox->Enable(1, false);
					}
					m_bImportFlag = false;

					m_bRunning = true;
					if (m_pcDialog)
					{
						m_pcDialog->ResetPlayButtons();
						//m_pcDialog->EnablePlayButton();

						m_bOneShot = false;
						SetDirection(FORWARD);
						m_bRunning = true;
						wxNotifyEvent();
						wxSleep(1);
						m_pcDialog->EnableBackStepping(true);

						m_bRunning = false;
						wxNotifyEvent();

					}


					return false;
			}

	//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		if (m_bImport)
		{
			SP_DS_Attribute* l_sNodeNAme = (*l_itTrans)->GetParentNode()->GetAttribute(wxT("Name"));
			int l_nCount = l_sNodeNAme->GetValueString().Freq(wxChar('_'));


			++l_nCount;


			auto itFind = m_mTransID2Color.find(m_nStepCount + 1);
			if (itFind != m_mTransID2Color.end())
			{

				wxString l_sChosenColor;
				if (itFind->second.Freq(wxChar('_')) == l_nCount)
				{
					l_sChosenColor = itFind->second.AfterLast(wxChar('_'));
				}
				else
				{
					wxString l_srawColor = wxT("");
					l_srawColor = itFind->second.AfterFirst(wxChar('_'));
					l_srawColor.Replace(wxT("_"), wxT(","));
					l_sChosenColor = wxT("(") + l_srawColor + wxT(")");
				}

				(*l_itTrans)->InformPrePlaces(l_sChosenColor);
			//	wxString l_sChosenColor = itFind->second.AfterLast(wxChar('_'));


			}
			else
			{
				(*l_itTrans)->InformPrePlaces();
			}
		}
		else
		{
			(*l_itTrans)->InformPrePlaces();
		}

	}
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

	if (GetDirection() == FORWARD)
			m_nStepCount++; //Increment the step count

		if (GetDirection() == BACKWARD && m_nStepCount > 0)
		{
			m_nStepCount--; //Decrement the step count
			m_bExport = false; //If user interrupts the animation,export should be stopped
		}

	bool l_bReturn = (m_lStepPlaceAnimators.size() + m_lStepTransAnimators.size() > 0);

	if (!l_bReturn) {
		m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
		wxString msg;
		if (m_bOneShot) {
			if (GetSingleStep()) {
				msg = wxT("This transition is not enabled!");
				if (m_nStepCount > 0)
									m_nStepCount--; //Dead state, thus decrement the m_nStepCount
			} else {

				if (m_nStepCount > 0)
					m_nStepCount--; //This transition was counted as a step, which is actually not possible

				msg =wxT("Dead state: there are no enabled transitions");
			}
		} else {
			if (m_nStepCount > 0)
							m_nStepCount--; //This transition was counted as a step, which is actually not possible

			msg =wxT("Dead state: there are no more enabled transitions");
		}
		msg = msg + wxT("\n \n * Not enough tokens in preplaces; \n Or \n * Color is out of range in postplaces.");
		SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION );

		if (m_pcDialog) {
			m_pcDialog->ResetPlayButtons();
		}
	} else {
	}

	if (m_bExport == true && m_bExportFlag)
			ExportStepSequences();

	if (GetDirection() == BACKWARD)
		m_bExport = false; //If user interrupts the animation,export should be stopped

		SetStepCounter();

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
		//------------------------------------------
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		wxSizer* m_pcExportImportSizer = new wxBoxSizer(wxHORIZONTAL);
		m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDEXPORT, wxT("Export")), 1, wxALL | wxEXPAND, 5);
		m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDIMPORT, wxT("Import")), 1, wxALL | wxEXPAND, 5);
		p_pcSizer->Add(m_pcExportImportSizer, 0, wxALL, 5);
		//------------------------------------------
#if defined(__WXMSW__)
	m_bitmapicons[0] = wxBitmap(wxT("BITMAP_ANIM_RECORD_TRACE"), wxBITMAP_TYPE_BMP_RESOURCE);
	m_bitmapicons[1] = wxBitmap(wxT("BITMAP_ANIM_PAUSE_RECORD"), wxBITMAP_TYPE_BMP_RESOURCE);
	m_bitmapicons[2] = wxBitmap(wxT("BITMAP_ANIM_REPLAY_TRACE"), wxBITMAP_TYPE_BMP_RESOURCE);
#else // !WIN
m_bitmapicons[0] = wxBitmap(recording_icon);
m_bitmapicons[1] = wxBitmap(pause_icon);
m_bitmapicons[2] = wxBitmap(play_icon);
#endif // !WIN

			m_buRecord = new wxBitmapButton(p_pcCtrl, SP_ID_RADIO_BUTTON_RECORD,
				m_bitmapicons[0], wxDefaultPosition, wxDefaultSize);
			m_buRecord->SetToolTip(wxT("Record the trace to an external file"));
			m_buRecord->Enable(true);
			m_buReplay = new wxBitmapButton(p_pcCtrl, SP_ID_RADIO_BUTTON_REPLAY,
				m_bitmapicons[2], wxDefaultPosition, wxDefaultSize);
			m_buReplay->SetToolTip(wxT("Replay the animation from an external trace file"));
			m_buReplay->Enable(true);
			wxArrayString l_aChoices; //= new wxArrayString;
			l_aChoices.Add("Record");
			l_aChoices.Add("Replay");


			wxSizer* 	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			l_pcRowSizer->Add(m_buRecord, wxSizerFlags(0).Border(wxALL, 10));
			l_pcRowSizer->Add(m_buReplay, wxSizerFlags(0).Border(wxALL, 10));
			p_pcSizer->Add(l_pcRowSizer);


		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		wxString l_tmp;
		l_tmp << m_nStepCount;
	    m_pcStepCounter = new wxBoxSizer(wxHORIZONTAL);
		m_pcStepCounterText = new wxStaticText(p_pcCtrl, wxID_ANY, wxT("Step Counter"));
		m_pcStepCounterValue = new wxStaticText(p_pcCtrl, wxID_ANY, l_tmp);

		m_pcStepCounter->Add(m_pcStepCounterText, 1, wxEXPAND | wxALL, 5);
		m_pcStepCounter->Add(m_pcStepCounterValue, 1, wxEXPAND | wxALL, 5);

		p_pcSizer->Add(m_pcStepCounter, 0, wxEXPAND);

		//-------------------------------------------
		p_pcCtrl->PushEventHandler(this);

		//allow backstepping
		m_pcDialog->EnableBackStepping(true);


		/*****************george*********************/
	    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		wxSizer* l_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
		wxSizer* l_pcOutputLabelSizer = new wxBoxSizer(wxVERTICAL);

		//m_pcOutputLabelStaticText = new wxStaticText(p_pcCtrl, SP_ID_STATIC_TEXT_OUTPUT_LABEL, wxT("Start ..."), wxDefaultPosition, wxDefaultSize);


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
		//p_pcSizer->Add(m_pcOutputLabelStaticText, 1, wxALL, 5);
	///	p_pcSizer->Add(l_pcOutputLabelSizer, 0, wxALL, 5);
		///p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
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

	//by george
	m_nStepCount = 0;
	m_nLineCount =1;//read from file switch to the begining of the file

	ResetTransSequenceFile();//reset exported trans seq file

	SetStepCounter();
	Refresh();
}

void
SP_DS_CPN_Animation::OnSet(wxCommandEvent& p_cEvent)
{
	m_bIsKeepClicked = true;
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
	else if(p_cEvent.GetId() != SP_ID_RADIO_BUTTON_RECORD && p_cEvent.GetId() != SP_ID_RADIO_BUTTON_REPLAY)
	{
		p_cEvent.Enable(!m_bRunning);
	}

	switch (p_cEvent.GetId())
		{
		case SP_ID_RADIO_BUTTON_RECORD:

			if (m_bExport == false)
			{
				p_cEvent.Enable(false);
			}
			else
			{
				if (m_bExport)
				{
					p_cEvent.Enable(true);
				}


			}
			break;
		case SP_ID_RADIO_BUTTON_REPLAY:
			if (m_bImport == false)
			{
				p_cEvent.Enable(false);
			}
			else
			{
				p_cEvent.Enable(true);
			}
			break;
		}


	int l_nColoringGroup = 0;
	bool l_bFound=false;
	for (auto it = m_choices.begin(); it != m_choices.end(); ++it)
	{
		if ((*it) == wxT("coloring"))
		{
			l_bFound=true;
			break;
		}
		l_nColoringGroup++;
	}

	if(l_bFound)
	{
		m_ncoloringChoiceValue = m_apcComboBoxes1[l_nColoringGroup]->GetSelection();
	}
}


void SP_DS_CPN_Animation::OnExport(wxCommandEvent &p_pc_Event)//george
{
	ExportCPN *export_frame = new ExportCPN(wxT("Export Details"),this);

	 export_frame->Show(true);
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

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
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
	bool l_bIsColoringGroupChanged = false;
		int l_nColoringGroup = 0;
		bool l_bFound=false;
		for (auto it = m_choices.begin(); it != m_choices.end(); ++it)
		{
			if ((*it) == wxT("coloring"))
			{
				l_bFound=true;
				break;
			}
			l_nColoringGroup++;
		}


		for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
		{
			if (m_apcColListAttrForConstants[i])
			{

				int vsel = m_apcComboBoxes1[i]->GetSelection();

				m_apcColListAttrForConstants[i]->SetActiveList(vsel);


				if (l_nColoringGroup == i && l_bFound)
				{
					if (vsel != m_ncoloringChoiceValue)
					{
						l_bIsColoringGroupChanged = true;
					}
				}

			}
		}

		if (l_bIsColoringGroupChanged)//only modify the model if the coloring group has been changed
		{
			UpdateColMarking();
		}
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
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false,true))
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

void SP_DS_CPN_Animation::ExportDetailsCPN(ExportCPN *export_frame)
{
	wxString l_start_str = export_frame->m_pc_StartText->GetLineText(0);
	wxString l_every_str = export_frame->m_pc_EveryText->GetLineText(0);
	wxString l_stop_str = export_frame->m_pc_StopText->GetLineText(0);
	wxString l_temp;
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;
	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;

	m_ExportFilename = export_frame->m_pc_Browse->GetPath();
	m_bExport = true;
	m_bExportFlag=false;
	m_bExportComplete = false;

	bool l_bError = 0;

	if (!l_start_str.ToLong(&m_nStart))
		l_bError = true;

	if (!l_stop_str.ToLong(&m_nStop))
		l_bError = true;

	if (!l_every_str.ToLong(&m_nEvery))
		l_bError = true;

	if (l_bError == true)
	{
		SP_MESSAGEBOX(wxT("Only integer values are accepted."), wxT("Error"), wxOK | wxICON_ERROR);
	}

	if (export_frame->m_pc_RadioChoice->GetSelection() == 0)
		m_nMarkingOption = 0;

	else
		if (export_frame->m_pc_RadioChoice->GetSelection() == 1)
			m_nMarkingOption = 1;

	if (m_nMarkingOption == 0)
	{
		l_temp = wxT(".mseq");
		m_ExportFilename = m_ExportFilename + l_temp;
	}

	else
		if (m_nMarkingOption == 1)
		{
			l_temp = wxT(".tseq");
			m_ExportFilename = m_ExportFilename + l_temp;
		}

	if (l_bError == false)
	{

		if (wxFileExists(m_ExportFilename) == true)
		{
			int l_Answer = SP_MESSAGEBOX(wxT("File already exists. Do you want to overwrite it?"), wxT("Overwrite"), wxYES_NO);
			if (l_Answer == wxYES || l_Answer == wxID_YES)
			{
				if (m_ExportTextfile.Open(m_ExportFilename))
				{
					m_ExportTextfile.Clear();
				}
				else
				{
					SP_MESSAGEBOX(wxT("Error in opening file."), wxT("Error"), wxOK | wxICON_ERROR);
				}
			}
			else
			{
				SP_MESSAGEBOX(wxT("Export failed"), wxT("Error"), wxOK | wxICON_ERROR);
			}
		}

		else
		{
			if (m_ExportTextfile.Create(m_ExportFilename) == false)
			{
				SP_MESSAGEBOX(wxT("Error in creating file."), wxT("Error"), wxOK | wxICON_ERROR);
			}
		}

		if (m_nMarkingOption == 0)
		{
			l_temp = wxT("Step Counter,");

			for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
			{
				if (l_itPlace != m_lAllPlaceAnimators.begin())
				{
					l_temp << wxT(",");
				}

				SP_DS_Node* l_pcNode = (*l_itPlace)->GetNode();
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Name"));

				SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);

				wxString l_sName = l_pcNameAttr->GetValue();
				l_temp << l_sName;
			}
		}

		if (m_nMarkingOption == 1)
		{
			if (m_pcChoice != NULL)
			{
				if (m_nStepState == 1)
				{
					l_temp = wxT("Stepping:Maximum,");
				}
				else if (m_nStepState == 2)
				{
					l_temp = wxT("Stepping:Intermediate,");
				}
				else if (m_nStepState == 3)
				{
					l_temp = wxT("Stepping:Single,");
				}
			}
			else
			{
				l_temp = wxT("Stepping:Intermediate ");
			}

			if (m_bAutoConcurrency)
			{
				l_temp = l_temp + wxT("AC:Enabled,transition=");
			}
			else
			{
				l_temp = l_temp + wxT("AC:Disabled,transition=");
			}
		}

		m_ExportTextfile.AddLine(l_temp);
		m_ExportTextfile.Write();
		m_ExportTextfile.Close();

	}

	if (m_bExport && !l_bError)
	{
		m_bImport = false;
		m_bImportFlag = false;
		m_ImportFilename = wxT("");
	}
}

void SP_DS_CPN_Animation::ExportMarkings()
{
	list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;

	if (m_ExportFilename.Cmp(wxT("")) != 0)
	{
		m_ExportTextfile.Open(m_ExportFilename);
		long l_n_token_value;

		if (m_nMarkingOption == 0)
		{
			if (m_nStepCount >= m_nStart && m_nStepCount <= m_nStop && m_bRunning == true && ((m_nStepCount - m_nStart) % m_nEvery == 0))
			{

				wxString l_temp(wxT(""));
				l_temp << m_nStepCount << wxT(",");

				for (l_itPlace = (m_lAllPlaceAnimators).begin(); l_itPlace != (m_lAllPlaceAnimators).end(); ++l_itPlace)
				{
					l_n_token_value = (*l_itPlace)->GetMarking();
					SP_DS_Node*  itNode = (*l_itPlace)->GetNode();
					SP_DS_ColListAttribute* l_pcCollis = dynamic_cast<SP_DS_ColListAttribute*>((itNode)->GetAttribute(SP_DS_CPN_MARKINGLIST));
					wxString l_sMulti;
					if (l_pcCollis)
					{
						for (unsigned int i = 0; i < l_pcCollis->GetRowCount(); i++)
						{
							l_sMulti << l_pcCollis->GetCell(i, 1) << wxT("`") << l_pcCollis->GetCell(i, 0) << wxT("++");

						}

						if (l_sMulti.IsEmpty())
						{
							l_sMulti << wxT("0`all()");
						}
					}
					if (!l_sMulti.Contains(wxT("all")))
					{
					l_sMulti = l_sMulti.BeforeLast('+');
					l_sMulti = l_sMulti.BeforeLast('+');
			     	}
					//wxString l_sMulti;
					//if ((*l_itPlace)->GetPlaceMultiSet())
					//{
					//	l_sMulti=(*l_itPlace)->GetPlaceMultiSet()->GetMultiSetMap()->begin()->first;
					//}
					wxString l_sMark;
					if (!l_sMulti.IsEmpty())
					{
						l_sMark << l_sMulti;// l_sMulti << wxT("`") << l_n_token_value;
					}
					if (++l_itPlace == m_lAllPlaceAnimators.end())
					{
						l_temp << l_sMulti;// l_sMark;
					}

					else
					{
						l_temp << l_sMark << wxT(",");
					}

					--l_itPlace;
				}

				m_ExportTextfile.AddLine(l_temp);
			}

			if (m_nStepCount == m_nStop + 1)
				m_bExportComplete = true;

			if (m_bExportComplete == true)
			{
				SP_MESSAGEBOX(wxT("Export completed."), wxT("Notification"), wxOK | wxICON_INFORMATION);
				m_bExportComplete = false; //This is required otherwise message dialog keeps poping till the dead state
			}
		}

		m_ExportTextfile.Write();
		m_ExportTextfile.Close();
	}
}

void SP_DS_CPN_Animation::ExportStepSequences()
{
	list<SP_DS_CPN_TransAnimator*>::iterator l_itTrans;

		if (m_ExportFilename.Cmp(wxT("")) != 0)
		{
			m_ExportTextfile.Open(m_ExportFilename);

			if (m_nMarkingOption == 1)
			{
				if (m_nStepCount >= m_nStart && m_nStepCount <= m_nStop && m_bRunning == true && ((m_nStepCount - m_nStart) % m_nEvery == 0))
				{

					wxString l_temp(wxT(""));

					if (m_lStepTransAnimators.begin() != m_lStepTransAnimators.end())
						l_temp << m_nStepCount;

					for (l_itTrans = (m_lStepTransAnimators).begin(); l_itTrans != (m_lStepTransAnimators).end(); ++l_itTrans)
					{
						SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();

						if (!l_pcNode) continue;
						std::vector<wxString>* l_vBindingColor = (*l_itTrans)->GetSelBindings();
						wxString l_sBinding;
						if (l_vBindingColor)
						{
							if (l_vBindingColor->size() >= 1)
							{

									for (auto itBinding = l_vBindingColor->begin(); itBinding != l_vBindingColor->end(); ++itBinding)
									{
										wxString l_sChosenBiding = *itBinding;
										SP_VectorString l_vColors;
										while (l_sChosenBiding.Freq(wxChar(';')) > 0)
										{
											wxString l_sPop = l_sChosenBiding.BeforeFirst(wxChar(';'));
											wxString l_scol = l_sPop;
											l_scol.Replace(wxT(" "), wxT(""));

											l_vColors.push_back(l_scol.AfterFirst(wxChar('=')));
											l_sPop << wxT(";");
											l_sChosenBiding.Replace(l_sPop, wxT(""));
										}
										for (int i = 0; i < l_vColors.size(); i++)
										{
											l_sBinding << wxT("_") << l_vColors[i];
										}
								}
							}
						}

						SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Name"));
						SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
						wxString l_sName = l_pcNameAttr->GetValue();
						if (!l_sBinding.IsEmpty())
						{

							l_sName  << l_sBinding;
						}

						l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
						SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
						long int l_sId = l_pcIdAttr->GetValue();

						if (l_itTrans == m_lStepTransAnimators.begin())
						{
							l_temp << wxT(" | ");
						}

						else
						{
							l_temp << wxT("\n  | ");
						}

						l_temp << l_sId << wxT(".") << l_sName;

						if (++l_itTrans != m_lStepTransAnimators.end())
						{
							l_temp << wxT(" :1,");
						}

						else
						{
							l_temp << wxT(" :1");
						}

						--l_itTrans;
					}

					m_ExportTextfile.AddLine(l_temp);
				}

				if (m_nStepCount == m_nStop + 1)
					m_bExportComplete = true;

				if (m_bExportComplete == true)
				{
					SP_MESSAGEBOX(wxT("Export completed."), wxT("Notification"), wxOK | wxICON_INFORMATION);
					m_bExportComplete = false; //This is required otherwise message dialog keeps poping till the dead state
				}
			}

			m_ExportTextfile.Write();
			m_ExportTextfile.Close();
		}
}

void SP_DS_CPN_Animation::SetStepCounter()
{
	m_pcStepCounterValue->SetLabel(wxString::Format(wxT("%ld"), m_nStepCount));
}

void SP_DS_CPN_Animation::OnImport(wxCommandEvent &event)
{
	ImportColAnim *import_frame = new ImportColAnim(wxT("Import Details"), this);

	import_frame->Show(true);
}

void SP_DS_CPN_Animation::ImportDetails(ImportColAnim *import_frame)
{

	list<SP_DS_CPN_TransAnimator *>::iterator l_itTrans;
	m_bImport = true;
	m_bInvalid = false;
	m_nLineCount = 1;
	m_nStepCount  = 0;
	m_ImportFilename = import_frame->m_pc_Browse->GetPath();

	//It is important to get the last token to check the file format
	//For eg filename.abc.tseq is a valid file format, thus we need the last token

	if (m_ImportFilename.AfterLast('.') != wxT("tseq"))
	{
		SP_MESSAGEBOX(wxT("Invalid file format."), wxT("Error"), wxOK | wxICON_ERROR);
	}
	else
	{
		if (m_ImportTextfile.Open(m_ImportFilename) == false)
		{
			SP_MESSAGEBOX(wxT("File does not exist."), wxT("Error"), wxOK | wxICON_ERROR);
		}
		else
		{
			wxStringTokenizer l_tkz(m_ImportTextfile.GetFirstLine(), wxT(","));

			wxString l_StepDetails = l_tkz.GetNextToken();
			wxString l_AutoConcurrency = l_tkz.GetNextToken();

			if (l_StepDetails.Cmp("Stepping:Maximum") == 0)
				m_nStepState = 1;
			else if (l_StepDetails.Cmp("Stepping:Intermediate") == 0)
				m_nStepState = 2;
			else if (l_StepDetails.Cmp("Stepping:Single") == 0)
				m_nStepState = 3;

			if (l_AutoConcurrency.Cmp("AC:Enabled") == 0)
				m_bAutoConcurrency = true;
			else if (l_AutoConcurrency.Cmp("AC:Disabled") == 0)
				m_bAutoConcurrency = false;

			unsigned int l_nLineCount = m_nLineCount;

			long l_trans_id;

			while (l_nLineCount < m_ImportTextfile.GetLineCount())
			{
				//Get the line and tokenize it with " " as delimiter
				wxStringTokenizer l_pc_tkz(m_ImportTextfile.GetLine(l_nLineCount++), wxT(" "));

				l_pc_tkz.GetNextToken();
				l_pc_tkz.GetNextToken();

				wxString l_sToken;
				l_sToken = l_pc_tkz.GetNextToken();
				m_mTransID2Color[l_nLineCount - 1] = l_sToken;
				wxString l_sTName = l_sToken;
				//Get the third token, tokenize it with "." as delimiter and take the first token from it
				wxStringTokenizer l_pc_trans(l_sToken, wxT("."));
				if (!l_pc_trans.GetNextToken().ToLong(&l_trans_id))
					;

				for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
				{
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();
					SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
					SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
					long int l_sId = l_pcIdAttr->GetValue();

					SP_DS_Attribute* l_pcAtt = l_pcNode->GetAttribute(wxT("Name"));
					wxString l_sNameVal = l_pcAtt->GetValueString();
					int l_nCount = l_sNameVal.Freq(wxChar('_'));
					++l_nCount;
					wxString l_sColor;
					if (l_sId == l_trans_id)
					{
						wxString l_sPre = l_sTName.AfterFirst(wxChar('.'));

						l_sPre.Replace(l_sNameVal, wxT(""));

						if(l_sPre.StartsWith(wxT("_")))
						{
							l_sColor = l_sPre.AfterFirst(wxChar('_'));
							if (l_sColor.Freq(wxChar('_')) > 0)
							{
								wxString l_sStr = wxT("(") + l_sColor + wxT(")");
								l_sStr.Replace(wxChar('_'), wxChar(','));
								l_sColor = l_sStr;
							}
						}
						else
						{
							m_bInvalid = true;
							SP_LOGMESSAGE(wxT("the unfolded name of the colored transition is not correct: ") + l_sTName);
							SP_MESSAGEBOX(wxT("Import file does not match with the given petri net."), wxT("Error"), wxOK | wxICON_ERROR);
							return;
						}


					}
					if (l_sColor != wxT(""))
					{
						SP_IMPORT_STATE l_state = (*l_itTrans)->CheckColor(l_sColor);
						if (l_state == SP_IMPORT_STATE::SP_IMPORT_ANIM_INVALID)
						{
							m_bInvalid = true;
							SP_LOGMESSAGE(wxT("the unfolded name of the colored transition is not correct: ") + l_sTName);
							SP_MESSAGEBOX(wxT("Import file does not match with the given petri net."), wxT("Error"), wxOK | wxICON_ERROR);
							return;
						}
						else if (l_state == SP_IMPORT_STATE::SP_IMPORT_ANIM_NOT_MATCHING_STATE)
						{
							m_bInvalid = true;
							//SP_LOGMESSAGE(wxT("the unfolded name of the colored transition is not correct: ") + l_sTName);
							SP_MESSAGEBOX(wxT("The current state of the model does not match the recorded trace"), wxT("Error"), wxOK | wxICON_ERROR);
							return;
						}
					}

					if (l_sId == l_trans_id)
						break;
				}

				if (l_itTrans == m_lAllTransAnimators.end())
					m_bInvalid = true;

				while (l_nLineCount < m_ImportTextfile.GetLineCount())
				{
					//Get the line and tokenize it with " " as delimiter
					wxStringTokenizer l_tkz(m_ImportTextfile.GetLine(l_nLineCount++), wxT(" "));

					//Copy the first token into a buffer
					wxString l_buf = l_tkz.GetNextToken();

					if (l_buf.Cmp("|") != 0)
					{
						l_nLineCount--;
						break;
					}

					//Get the second token, tokenize it with "." as delimiter and take the first token from it
					wxStringTokenizer l_trans(l_tkz.GetNextToken(), wxT("."));
					if (!l_trans.GetNextToken().ToLong(&l_trans_id))
						;

					for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
					{
						SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();
						SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
						SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
						long int l_sId = l_pcIdAttr->GetValue();

						int l_nCount = l_pcNode->GetAttribute(wxT("Name"))->GetValueString().Freq(wxChar('_'));
						++l_nCount;
						wxString l_sColor;
						if (l_sId == l_trans_id)
						{
							if (l_sTName.Freq(wxChar('_')) == l_nCount)
							{
								l_sColor = l_sTName.AfterLast(wxChar('_'));
							}
							else
							{
								l_sColor = wxT("(");
								l_sColor << l_sTName.AfterFirst(wxChar('_'));
								l_sColor << wxT(")");
								l_sColor.Replace(wxT("_"), wxT(","));

							}
						}

						if (!(*l_itTrans)->CheckColor(l_sColor))
						{
							m_bInvalid = true;
							SP_LOGMESSAGE(wxT("the unfolded name of the colored transition is not correct: ") + l_sTName);
							SP_MESSAGEBOX(wxT("Import file does not match with the given petri net."), wxT("Error"), wxOK | wxICON_ERROR);
							return;
						}

						if (l_sId == l_trans_id)
							break;
					}

					if (l_itTrans == m_lAllTransAnimators.end())
						m_bInvalid = true;
				}
			}

			m_ImportTextfile.Close();

			if (m_bInvalid == true)
			{
				m_bImport = false; //No import is possible because the file is invalid
				m_bImportFlag=false;
				SP_MESSAGEBOX(wxT("Import file does not match with the given petri net."), wxT("Error"), wxOK | wxICON_ERROR);
			}
			else
			{
				SP_LOGMESSAGE(wxT("Import file '") + m_ImportFilename + wxT("' successfully."));

				if (m_bExport)
				{
					m_bExport = false;
					m_bExportFlag = false;
					ResetRecordButton();
					m_ExportFilename = wxT("");
				}

			}
		}
	}

}

bool SP_DS_CPN_Animation::ImportStepSequences()
{
	bool l_bIsOk = true;
	if (m_bInvalid || m_ImportTextfile.Open(m_ImportFilename) == false)
	{
		m_lPossibleTransAnimators.clear();
		SP_MESSAGEBOX(wxT("Error in opening file ") + m_ImportFilename, wxT("Notification"), wxOK | wxICON_ERROR);
		return false;
	}
	else
	{
		list<SP_DS_CPN_TransAnimator *>::iterator l_itTrans;
		long l_trans_id;

		if (m_nLineCount < m_ImportTextfile.GetLineCount())
		{
			m_lPossibleTransAnimators.clear();

			SP_LOGDEBUG(wxString::Format("Line Count = %u - ", m_nLineCount));

			//Get the line and tokenize it with " " as delimiter
			wxStringTokenizer l_pc_tkz(m_ImportTextfile.GetLine(m_nLineCount++), wxT(" "));

			wxString l_temp = l_pc_tkz.GetNextToken();

			SP_LOGDEBUG(wxString::Format("Step Number = %s\n", l_temp.c_str()));

			l_pc_tkz.GetNextToken();

			//Get the third token, tokenize it with "." as delimiter and take the first token from it
			wxStringTokenizer l_pc_trans(l_pc_tkz.GetNextToken(), wxT("."));
			if (!l_pc_trans.GetNextToken().ToLong(&l_trans_id))
				;

			SP_LOGDEBUG(wxString::Format("Transition ID: %ld\n", l_trans_id));

			for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
			{
				SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
				SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
				long int l_sId = l_pcIdAttr->GetValue();

				if (l_sId == l_trans_id)
				{
				m_lPossibleTransAnimators.push_back(*l_itTrans);
				l_bIsOk = true;
				}
			}

			while (m_nLineCount < m_ImportTextfile.GetLineCount())
			{
				//Get the line and tokenize it with " " as delimiter
				wxStringTokenizer l_tkz(m_ImportTextfile.GetLine(m_nLineCount++), wxT(" "));

				//Copy the first token into a buffer
				wxString l_buf = l_tkz.GetNextToken();

				if (l_buf.Cmp("|") != 0)
				{
					m_nLineCount--;
					break;
				}

				//Get the second token, tokenize it with "." as delimiter and take the first token from it
				wxStringTokenizer l_trans(l_tkz.GetNextToken(), wxT("."));
				if (!l_trans.GetNextToken().ToLong(&l_trans_id))
					;

				for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
				{
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();
					SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
					SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
					long int l_sId = l_pcIdAttr->GetValue();

					if (l_sId == l_trans_id)
					{	m_lPossibleTransAnimators.push_back(*l_itTrans);
					    l_bIsOk = true;
				     }
				}

				SP_LOGDEBUG(wxString::Format("Transition ID: %ld\n", l_trans_id));

			}
		}
		else
		{
			l_bIsOk = false;
		}
		m_ImportTextfile.Close();

	}
	return l_bIsOk;
}



void SP_DS_CPN_Animation::ResetTransSequenceFile()
{
	int l_nCounter = 0;
	bool l_bIsReset = false;


	if(m_ExportFilename==wxT("")|| !m_bExport){return;}

	if (m_ExportTextfile.Open(m_ExportFilename))
	{
		int choose = SP_MESSAGEBOX(wxT("The recorded file will be Overwritten, Do you want to overwitten?"), wxT("Overwrite"), wxOK | wxNO | wxICON_INFORMATION);
		if (choose == 8)
		{
					return;
		}
		while (l_nCounter <= m_ExportTextfile.GetLineCount() && m_ExportTextfile.GetLineCount()!=1)
		{

			if (l_nCounter > 1)
			{
				m_ExportTextfile.RemoveLine(l_nCounter-1);
				m_ExportTextfile.Write();
				m_ExportTextfile.Close();
				l_bIsReset = true;
				l_nCounter = 0;
				if (!m_ExportTextfile.Open(m_ExportFilename))
				{
					break;
				}
			}

			l_nCounter++;
		}
	}
	if(l_bIsReset)
	m_ExportTextfile.Close();
}

void SP_DS_CPN_Animation::ResetRecordButton()
{
	m_buRecord->SetBitmapLabel(m_bitmapicons[0]);
	m_buRecord->Refresh();
	m_bExportFlag = false;
}

void SP_DS_CPN_Animation::ResetReplayButton()
{
	m_buReplay->SetBitmapLabel(m_bitmapicons[2]);
	m_buReplay->Refresh();
	m_bImportFlag = false;
}

void SP_DS_CPN_Animation::OnReplay(wxCommandEvent& p_cEvent)
{
	if (m_bImport)
	{
		m_bImportFlag = !m_bImportFlag;
		if (m_bImportFlag)
		{
			m_buReplay->SetBitmapLabel(m_bitmapicons[1]);
			m_buReplay->SetToolTip(wxT("Pause Animation from the external file"));
			m_buReplay->Refresh();
		}
		else
		{
			m_buReplay->SetBitmapLabel(m_bitmapicons[2]);
			m_buReplay->SetToolTip(wxT("Replay Animation from the external file"));
			m_buReplay->Refresh();
		}
	}
}


void SP_DS_CPN_Animation::OnRecord(wxCommandEvent &p_pc_Event)
{
	if (m_bExport)
	{
		m_bExportFlag = !m_bExportFlag;
		if (m_bExportFlag)
		{
			m_buRecord->SetBitmapLabel(m_bitmapicons[1]);
			m_buRecord->SetToolTip(wxT("Pause trace recording"));
			m_buRecord->Refresh();
		}
		else
		{
			m_buRecord->SetBitmapLabel(m_bitmapicons[0]);
			m_buRecord->SetToolTip(wxT("Start trace recording"));
			m_buRecord->Refresh();
		}
	}
}



