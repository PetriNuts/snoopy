//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// @Modified : George Assaf 01/02/2020
// Short Description: colored PN animation class
//////////////////////////////////////////////////////////////////////
#include <wx/statline.h>

#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStTransAnimator.h"

#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"

#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "spsim/helpers/stochastic/animationinfo.h"

//by george for constants harmonizing
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"

enum
{
	SP_ID_PEDRESET = SP_ID_LAST_ID + 1,
	SP_ID_PEDSET,
	SP_ID_PEDKEEP,

	SP_ID_STATIC_TEXT_OUTPUT_LABEL,

	SP_ID_BUTTON_OPEN_SIMULATION,

	SP_ID_CHOICE_FUNCTION_SETS,
	SP_ID_CHOICE_MARKING_SETS,
	SP_ID_CHOICE_PARAMETER_SETS,

	SP_ID_CHOICE_WEIGHT_SETS,
	SP_ID_CHOICE_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_CHOICE_SCHEDULE_SETS,//By Liu on 6 Mar. 2009

	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,

	SP_ID_BUTTON_MODIFY_WEIGHT_SETS,
	SP_ID_BUTTON_MODIFY_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, //By Liu on 6 Mar. 2009
	SP_ID_SHOWBINDDIALOGUE,
	SP_ID_RANDOMCOLOR,
	SP_ID_CHOICE_SETS, //by george on 1.2.20
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,  //by george on 1.2.20
	SP_ID_DESTROY_ANIM,
	SP_ID_PEDEXPORT,//by george 10.2020
	SP_ID_PEDIMPORT//by george  10.2020
};

BEGIN_EVENT_TABLE(SP_DS_ColStAnimation, SP_DS_Animation)
EVT_BUTTON(SP_ID_PEDSET, SP_DS_ColStAnimation::OnSet)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_ColStAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_ColStAnimation::OnUpdateUI)
EVT_BUTTON(SP_ID_BUTTON_OPEN_SIMULATION, SP_DS_ColStAnimation::OnOpenSimulation)

EVT_CHOICE(SP_ID_CHOICE_MARKING_SETS, SP_DS_ColStAnimation::OnMarkingSetChanged)
EVT_CHOICE(SP_ID_CHOICE_FUNCTION_SETS, SP_DS_ColStAnimation::OnFunctionSetChanged)
EVT_CHOICE(SP_ID_CHOICE_PARAMETER_SETS, SP_DS_ColStAnimation::OnSetsChanged)

EVT_CHOICE(SP_ID_CHOICE_WEIGHT_SETS, SP_DS_ColStAnimation::OnWeightSetChanged) //By Liu on 6 Mar. 2009
EVT_CHOICE(SP_ID_CHOICE_DELAY_SETS, SP_DS_ColStAnimation::OnDelaySetChanged) //By Liu on 6 Mar. 2009
EVT_CHOICE(SP_ID_CHOICE_SCHEDULE_SETS, SP_DS_ColStAnimation::OnScheduleSetChanged) //By Liu on 6 Mar. 2009

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_ColStAnimation::OnModifyMarkingSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DS_ColStAnimation::OnModifyFunctionSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DS_ColStAnimation::OnModifyParameterSets)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DS_ColStAnimation::OnModifyWeightSets) //By Liu on 6 Mar. 2009
EVT_BUTTON(SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DS_ColStAnimation::OnModifyDelaySets) //By Liu on 6 Mar. 2009
EVT_BUTTON(SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DS_ColStAnimation::OnModifyScheduleSets) //By Liu on 6 Mar. 2009

EVT_CHOICE(SP_ID_CHOICE_SETS, SP_DS_ColStAnimation::OnColConstantSetsChanged)// by george
EVT_BUTTON(SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DS_ColStAnimation:: SP_DS_ColStAnimation::OnModifyConstants) //by george

EVT_UPDATE_UI(SP_ID_PEDIMPORT, SP_DS_ColStAnimation::OnUpdateUI)//george on 10.2020
EVT_UPDATE_UI(SP_ID_PEDEXPORT, SP_DS_ColStAnimation::OnUpdateUI)//george on 10.2020
EVT_BUTTON(SP_ID_PEDEXPORT, SP_DS_ColStAnimation::OnExport)//george on 10.2020
EVT_BUTTON(SP_ID_PEDIMPORT, SP_DS_ColStAnimation::OnImport)//george on 10.2020

END_EVENT_TABLE()

SP_DS_ColStAnimation::SP_DS_ColStAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping) :
	SP_DS_Animation(p_nRefresh, p_nDuration),
	m_nStepState((int)p_eStepping),
	m_nBindingChoice(1),
	m_cbKeep(0),
	m_pcSingleStep(NULL),
	m_pcDialog(NULL),
	m_bChooseRandomColor(true),
	m_pcUnfolding(NULL),
	m_bFirstFolding(true)
{
	m_IsDestructor = false;//by george
	m_apcColListAttrForConstants.clear();
	m_bIsKeepClicked = false;

	m_nStart = 0;
	m_nEvery = 0;
	m_nStop = 0;
	m_nMarkingOption = 0;
	m_bAutoConcurrency = false;
	m_nStepCount = 0; //Keeps a count of step number
	m_bImport=false;
	m_bInvalid=false;
    m_sTriggiredUnfoldedTrans = wxT("");
    m_mGroup2Pos.clear();

 
}

SP_DS_ColStAnimation::~SP_DS_ColStAnimation()
{
	bool l_bIsKeep = m_bIsKeepClicked;
		if (m_cbKeep) {
			if (m_cbKeep->IsChecked()) {
				KeepMarking();
				l_bIsKeep = true;
			}
			else if (!l_bIsKeep) {
				list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
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
		Refresh();

		//if (m_pcUnfolding)
		wxDELETE(m_pcUnfolding);
		}

		if (m_nIsClose == SP_ID_DESTROY_ANIM&& !l_bIsKeep)
		{
			m_IsDestructor = true;
			LoadDefaultConstantsGroups();//by george
			list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

			for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
					(*l_Iter)->ResetMarking();

			SP_DS_ColListAttribute* l_pcColList;

					SP_ListNode::const_iterator l_itElem;
					vector<SP_DS_Node*> l_vPlaceNodes;
					SP_DS_Nodeclass* l_pcNodeclass;
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

						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST));

						//l_pcColList->Clear();
						//l_pcColList->RemoveAllColumns();

						int l_nBegin = l_nGridRowNumber;
						wxString l_sMainMarking;

						l_vPlaceNodes[l_nPos]->Update(TRUE);


						SP_CPN_SyntaxChecking l_cSyntaxChecking;
						if (!l_cSyntaxChecking.Initialize())
							continue;
				////////////////////////////////////////////////////////
					l_sMainMarking = wxT("");
					map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
					if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false))
					continue;
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

					SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast<SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
					if (l_pcMarkingAttr)
						l_pcMarkingAttr->SetValueString(l_sMainMarking);

					l_vPlaceNodes[l_nPos]->Update(TRUE);
					}


				  SP_Core::Instance()->GetRootDocument()->Modify(true);


					/*******************************/
					Refresh();

				//if (m_pcUnfolding)
				wxDELETE(m_pcUnfolding);
				}

		if (m_cbKeep)
			wxDELETE(m_cbKeep);

}

void SP_DS_ColStAnimation::UpdateMarkingPlaces()
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
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodesinPed[l_nPos], l_mColorToMarkingMap, false))
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
SP_DS_ColStAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_ColStAnimation(m_nRefreshFrequ, m_nStepDuration, SP_ANIM_STEP_T(m_nStepState)));
}

bool
SP_DS_ColStAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
	m_nIsClose = SP_ID_DESTROY_ANIM;
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
				m_lAllPlaceAnimators.push_back(dynamic_cast<SP_DS_ColStPlaceAnimator*>(l_pcAnimator));
			else if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_TRANS)
				m_lAllTransAnimators.push_back(dynamic_cast<SP_DS_ColStTransAnimator*>(l_pcAnimator));
		}
	}

	//init animation history
	wxString nc = p_pcGraph->GetNetclass()->GetName();
	m_nHistorySize = wxGetApp().GetAnimationPrefs()->GetHistorySize(nc);
	m_llHistoryTransAnimators.clear();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SP_DS_ColStTransAnimator* l_pcTrans;
	SP_DS_ColStPlaceAnimator* l_pcPlace;

	list< SP_DS_ColStTransAnimator* >::iterator l_Iter2;
	list< SP_DS_ColStPlaceAnimator* >::iterator l_Iter1;

	int i = 0;

	for (l_Iter2 = m_lAllTransAnimators.begin(); l_Iter2 != m_lAllTransAnimators.end(); ++l_Iter2)
	{
		l_pcTrans = (*l_Iter2);

		m_mpcTransitionAnimators[i++] = l_pcTrans;
	}

	for (l_Iter1 = m_lAllPlaceAnimators.begin(); l_Iter1 != m_lAllPlaceAnimators.end(); ++l_Iter1)
	{
		l_pcPlace = (*l_Iter1);
		m_mpcPlaceAnimators[l_pcPlace->GetId()] = l_pcPlace;

		(*l_Iter1)->InitCanvas();
	}

	m_pcSimulator = new spsim::Gillespie();


	return l_bReturn;
}

bool
SP_DS_ColStAnimation::PreStep()
{
	list<list<SP_DS_ColStTransAnimator*> >::iterator l_itHistoryCurrent;
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
	unsigned int l_nSteps = m_nSteps / 2;

	m_lPossibleTransAnimators.clear();

	if (m_bExport == true)
			ExportMarkings();

		if (GetDirection() == FORWARD)
			m_nStepCount++; //Increment the step count

		else if (GetDirection() == BACKWARD)
			m_bExport = false; //If user interrupts the animation,export should be stopped

		if (m_bImport == false)
		{
	//selecting transitions (candidates) to fire ...
	if (dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep))
	{
		//...for steps that are triggered by clicking directly on a transition
		m_lPossibleTransAnimators.push_back(dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep));
		//if back-firing one transition directly, anim history is deleted
		if (GetDirection() == BACKWARD)
		{
			m_llHistoryTransAnimators.clear();
		}
	}
	else if (dynamic_cast<SP_DS_ColStCoarseTransAnimator*>(m_pcSingleStep))
	{
		//...for steps that are triggered by clicking directly on a coarse transition
		SP_DS_Coarse* l_pcCoarse = m_pcSingleStep->GetAnimObject()->GetCoarse();
		for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
		{
			if ((*l_itTrans)->GetAnimObject()->GetGraphicInSubnet(l_pcCoarse->GetNetnumber()))
			{
				m_lPossibleTransAnimators.push_back(*l_itTrans);
			}
		}
	}
	else if (GetDirection() == BACKWARD && !m_llHistoryTransAnimators.empty())
	{
		//...for backsteps using the history
		l_itHistoryCurrent = --(m_llHistoryTransAnimators.end());
		list<SP_DS_ColStTransAnimator*>& l_rlHistory = (*l_itHistoryCurrent);
		for (l_itTrans = l_rlHistory.begin(); l_itTrans != l_rlHistory.end(); ++l_itTrans) {
			m_lPossibleTransAnimators.push_back(*l_itTrans);
		}
		m_llHistoryTransAnimators.erase(l_itHistoryCurrent, m_llHistoryTransAnimators.end());
	}
	else
	{
		ChooseColTransitions();  //choose the transition to be fired

								 //...for stepping forward and random backsteps
								 /*		for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
								 {
								 m_lPossibleTransAnimators.push_back(*l_itTrans);
								 }
								 */
	}
		}

		else if (m_bImport == true)//take the possible trans from the input trace,by george
			ImportStepSequences();
	//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		SP_DS_ColStTransAnimator* l_pcColStTransAnimator = (SP_DS_ColStTransAnimator*)(*l_itTrans);
		l_pcColStTransAnimator->InformStPrePlaces();
	}
	//(*l_itTrans)->InformPrePlaces();

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		(*l_itPlace)->TestMarking();
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
		(*l_itTrans)->TestConcession();
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		//(*l_itPlace)->ResolveConflicts();
	}

	//selecting the transition that are actually able to fire
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		if ((*l_itTrans)->PreStep(l_nSteps, GetDirection()))
		{
			m_lStepTransAnimators.push_back((*l_itTrans));
		}
	}

	// all transitions with concession are resolved, now test the stepping option
	//ReduceTransitions();

	//	ReduceColTransitions();

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
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		if ((*l_itPlace)->PreStep(l_nSteps, GetDirection()))
		{
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
	}
	else {
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
				if (m_nStepCount > 0)
					{
						m_nStepCount--; //Dead state, thus decrement the m_nStepCount
					}
			}
			else {
				msg = wxT("Dead state: there are no enabled transitions.");

				if (m_nStepCount > 0)
								{
									m_nStepCount--; //Dead state, thus decrement the m_nStepCount
								}
			}
		}
		else {
			msg = wxT("Dead state: there are no more enabled transitions.");
			if (m_nStepCount > 0)
						{
							m_nStepCount--; //Dead state, thus decrement the m_nStepCount
						}
		}
		msg = msg + wxT("\n \n * Not enough tokens in preplaces; \n Or \n * Color is out of range in postplaces.");
		SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION);

		if (m_pcDialog) {
			m_pcDialog->ResetPlayButtons();
		}
	}
	else {
	}

	if (GetDirection() == BACKWARD && m_nStepCount > 0)
	{
			m_nStepCount--; //Decrease the step count
			m_bExport = false; //If user interrupts the animation,export should be stopped
	}

	if (m_bExport == true)
			ExportStepSequences();


	SetStepCounter();

	return l_bReturn;
}

bool
SP_DS_ColStAnimation::Step()
{
	bool l_bReturn = TRUE;
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;

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
SP_DS_ColStAnimation::PostStep()
{
	bool l_bReturn = TRUE;
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
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
			}
			else {
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMSTEP_BWD));
			}
		}
	}

	return l_bReturn;
}

bool
SP_DS_ColStAnimation::ReduceTransitions()
{
	if (dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep))
		return TRUE;

	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
	SP_DS_ColStTransAnimator* l_pcAnim = NULL;
	if (m_nStepState == SP_ANIM_STEP_SINGLE)
	{
		if (m_lStepTransAnimators.size() > 0) {
			int l_nChose = SP_RandomLong(m_lStepTransAnimators.size());
			int i = 0;
			for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
				if (l_nChose == i++)
					l_pcAnim = (*l_itTrans);
				else
					(*l_itTrans)->Reset();

			m_lStepTransAnimators.clear();
			m_lStepTransAnimators.push_back(l_pcAnim);
		}
	}
	else if (m_nStepState == SP_ANIM_STEP_INTERMEDIATE)
	{
		// two iterations, first choose one random entry, second choose a random set of the rest
		SP_LOGDEBUG(wxString::Format(wxT("have %d"), m_lStepTransAnimators.size()));

		list<SP_DS_ColStTransAnimator*> l_lNewTrans;
		//first round choose one transition randomly
		if (m_lStepTransAnimators.size() > 0) {
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
			m_lStepTransAnimators.push_back((*l_itTrans));
		SP_LOGDEBUG(wxString::Format(wxT("chose %d"), m_lStepTransAnimators.size()));
	}

	return TRUE;
}

bool
SP_DS_ColStAnimation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
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


	////////////////////////////
	m_cbChooseRandomColor = new wxCheckBox(p_pcDlg, SP_ID_RANDOMCOLOR, wxT("Choose a random token."));
	m_cbChooseRandomColor->SetValue(m_bChooseRandomColor);

	p_pcSizer->Add(m_cbChooseRandomColor, 0, wxALL, 5);
	//

	return TRUE;
}

bool
SP_DS_ColStAnimation::OnDialogOk()
{
	if (!m_pcChoice)
		return FALSE;

	m_nStepState = m_pcChoice->GetSelection() + 1;

	m_nBindingChoice = m_pcBindingChoice->GetSelection();

	m_bChooseRandomColor = m_cbChooseRandomColor->GetValue();

	return SP_DS_Animation::OnDialogOk();
}

bool
SP_DS_ColStAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer)
{
	if (!p_pcCtrl || !p_pcSizer)
		return FALSE;

	m_pcDialog = p_pcCtrl;

	/*********by george**********/
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
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		wxString l_tmp;
		l_tmp << m_nStepCount;
		m_pcStepCounter = new wxBoxSizer(wxHORIZONTAL);
		m_pcStepCounterText = new wxStaticText(p_pcCtrl, wxID_ANY, wxT("Step Counter"));
		m_pcStepCounterValue = new wxStaticText(p_pcCtrl, wxID_ANY, l_tmp);

		m_pcStepCounter->Add(m_pcStepCounterText, 1, wxEXPAND | wxALL, 5);
		m_pcStepCounter->Add(m_pcStepCounterValue, 1, wxEXPAND | wxALL, 5);

		p_pcSizer->Add(m_pcStepCounter, 0, wxEXPAND);
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		//-------------------------------------------
		//p_pcSizer->Add( new wxButton( p_pcCtrl, SP_ID_BUTTON_OPEN_SIMULATION, wxT("Stochastic simulation"),
		//wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );
		/////////////////////////////
		//wxSizer* l_pcOutputLabelSizer = new wxBoxSizer(wxVERTICAL);

		wxSizer* l_pcSetsSizer = new wxBoxSizer(wxVERTICAL);


		wxSizer* l_pcRowSizer;// = new wxBoxSizer(wxHORIZONTAL);

		//m_pcOutputLabelStaticText = new wxStaticText(p_pcCtrl, SP_ID_STATIC_TEXT_OUTPUT_LABEL, wxT("Start ..."), wxDefaultPosition, wxDefaultSize);

		///l_pcRowSizer->Add(m_pcOutputLabelStaticText, 1, wxALL, 5);

		//l_pcOutputLabelSizer->Add(l_pcRowSizer, 0, wxEXPAND);


		if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0)//by george, preventing of occuring empty v-sets
		{

			    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			    l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Marking set:")), 1, wxALL | wxEXPAND, 5);

			    m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_MARKING_SETS, wxDefaultPosition, wxSize(100, -1)));

			    l_pcRowSizer->Add(m_apcComboBoxes[0], 0, wxALL, 5);

			    l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);

			    l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

			    m_mGroup2Pos[wxT("Marking")] = m_apcComboBoxes.size()-1;
		}

		if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size())
		{

				l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

				l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);

				m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100, -1)));

				l_pcRowSizer->Add(m_apcComboBoxes[1], 0, wxALL, 5);

				l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);

				l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

				m_mGroup2Pos[wxT("Rate")] = m_apcComboBoxes.size() - 1;

		}


		if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0)
		{
				//Weight
				l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

				l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Weight set:")), 1, wxALL | wxEXPAND, 5);

				m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_WEIGHT_SETS, wxDefaultPosition, wxSize(100, -1)));

				l_pcRowSizer->Add(m_apcComboBoxes[2], 0, wxALL, 5);

				l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_WEIGHT_SETS, wxT("Modify")), 0, wxALL, 5);

				l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

				m_mGroup2Pos[wxT("Weight")] = m_apcComboBoxes.size() - 1;

		}


		if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0)
		{
				//Delay
				l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

				l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Delay set:")), 1, wxALL | wxEXPAND, 5);

				m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_DELAY_SETS, wxDefaultPosition, wxSize(100, -1)));

				l_pcRowSizer->Add(m_apcComboBoxes[3], 0, wxALL, 5);

				l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), 0, wxALL, 5);

				l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

				m_mGroup2Pos[wxT("Delay")] = m_apcComboBoxes.size() - 1;


		}


		if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0)
		{
				l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

				l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Schedule set:")), 1, wxALL | wxEXPAND, 5);

				m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_SCHEDULE_SETS, wxDefaultPosition, wxSize(100, -1)));

				l_pcRowSizer->Add(m_apcComboBoxes[4], 0, wxALL, 5);

				l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, wxT("Modify")), 0, wxALL, 5);

				l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

				m_mGroup2Pos[wxT("Schedule")] = m_apcComboBoxes.size() - 1;

		}


		/**************george**************/
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

		//m_msMarkingSets

		/*****************************/

		//p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		//p_pcSizer->Add(l_pcOutputLabelSizer, 0, wxALL, 5);
		//p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		p_pcSizer->Add(l_pcSetsSizer, 0, wxALL, 5);
		p_pcSizer->Add( new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND);
		//p_pcSizer->Add( l_pcSimulationControlSizer, 0, wxALL, 5 );


		LoadSets();

		p_pcCtrl->PushEventHandler(this);

		//allow backstepping
		m_pcDialog->EnableBackStepping(true);

		return TRUE;
}

void
SP_DS_ColStAnimation::OnReset()
{

	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();

	LoadCurrentMarking();


	m_llHistoryTransAnimators.clear();
	//by george
	m_nStepCount = 0;
	SetStepCounter();
	Refresh();
}

void
SP_DS_ColStAnimation::OnSet(wxCommandEvent& p_cEvent)
{
	m_bIsKeepClicked = true;
	KeepMarking();
}

void
SP_DS_ColStAnimation::KeepMarking()
{
	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->SetMarking();
	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

}

void
SP_DS_ColStAnimation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
	if (p_cEvent.GetId() == SP_ID_PEDSET) p_cEvent.Enable(!m_bRunning && !m_cbKeep->IsChecked());
	else p_cEvent.Enable(!m_bRunning);
}



void
SP_DS_ColStAnimation::OnOpenSimulation(wxCommandEvent& p_cEvent)
{
	//Restore initial markings
	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();

	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

	//show simulation result dialogue
	m_pcDialog->Show(false);

	SP_DLG_StSimulationResults* l_pcDlg = new SP_DLG_StSimulationResults(m_pcGraph, NULL);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();

	//show Animationdialog after Simulationresults close
	m_pcDialog->Show(true);

	LoadSets();

	//Synchronize the marking in the collist and the animation.  
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->UpdateDefaultMarking();

	m_bRestartAnimationFlag = true;

}




void SP_DS_ColStAnimation::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();

	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);


	SP_DLG_ColStMarkingOverview* l_pcDlg = new SP_DLG_ColStMarkingOverview(NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			m_bRestartAnimationFlag = false;  // let it not animate
			return;
		}

		//m_anNetMarkings = *(l_cUnfolding.GetNetMarkings());

		//m_pcSimulator->SetNetMarkings(*(l_cUnfolding.GetNetMarkings()))	;
	}

	l_pcDlg->Destroy();

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->UpdateDefaultMarking();
	}



}

void SP_DS_ColStAnimation::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(wxT("Transition"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
			return;
	}

	l_pcDlg->Destroy();
}


void
SP_DS_ColStAnimation::OnModifyWeightSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(wxT("Immediate Transition"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
			return;
	}

	l_pcDlg->Destroy();
}


void
SP_DS_ColStAnimation::OnModifyDelaySets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
			return;
	}

	l_pcDlg->Destroy();
}

void
SP_DS_ColStAnimation::OnModifyScheduleSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
			return;
	}

	l_pcDlg->Destroy();
}

void SP_DS_ColStAnimation::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_StParameterOverview* l_pcDlg = new SP_DLG_StParameterOverview(NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}


void
SP_DS_ColStAnimation::OnSetsChanged(wxCommandEvent& p_cEvent)
{

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	m_apcColListAttr[5]->SetActiveList(m_apcComboBoxes[5]->GetSelection());

}


void
SP_DS_ColStAnimation::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	auto itFind = m_mGroup2Pos.find(wxT("Marking"));

    if (itFind != m_mGroup2Pos.end())
	{
	  m_apcColListAttr[itFind->second]->SetActiveColumn(m_apcComboBoxes[itFind->second]->GetSelection());
	}

}


void
SP_DS_ColStAnimation::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
{

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	auto itFind = m_mGroup2Pos.find(wxT("Rate"));

	if (itFind != m_mGroup2Pos.end())
	{
	  m_apcColListAttr[itFind->second]->SetActiveColumn(m_apcComboBoxes[itFind->second]->GetSelection());
	}

}

void
SP_DS_ColStAnimation::OnWeightSetChanged(wxCommandEvent& p_cEvent)
{

	///m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	auto itFind = m_mGroup2Pos.find(wxT("Weight"));

	if (itFind != m_mGroup2Pos.end())
	{
		m_apcColListAttr[itFind->second]->SetActiveColumn(m_apcComboBoxes[itFind->second]->GetSelection());
	}

}

void
SP_DS_ColStAnimation::OnDelaySetChanged(wxCommandEvent& p_cEvent)
{

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	auto itFind = m_mGroup2Pos.find(wxT("Delay"));

	if (itFind != m_mGroup2Pos.end())
	{
	  m_apcColListAttr[itFind->second]->SetActiveColumn(m_apcComboBoxes[itFind->second]->GetSelection());
	}

}

void
SP_DS_ColStAnimation::OnScheduleSetChanged(wxCommandEvent& p_cEvent)
{

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	auto itFind = m_mGroup2Pos.find(wxT("Schedule"));

	if (itFind != m_mGroup2Pos.end())
	{
	  m_apcColListAttr[itFind->second]->SetActiveColumn(m_apcComboBoxes[itFind->second]->GetSelection());
	}

}



void SP_DS_ColStAnimation::LoadSets()
{
	if (m_apcColListAttr.empty())
	{

		m_apcColListAttr.resize(m_apcComboBoxes.size());

		auto itFind = m_mGroup2Pos.find(wxT("Marking"));

		SP_DS_Node* l_pcNode = NULL;
		if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0 && itFind !=m_mGroup2Pos.end())
		{
			l_pcNode = m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->front();
			m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));

		}

		itFind = m_mGroup2Pos.find(wxT("Rate"));

		if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0 && itFind !=m_mGroup2Pos.end())
		{
			l_pcNode = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
			m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));

		}

		itFind = m_mGroup2Pos.find(wxT("Weight"));

		if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Pos.end())
		{
			l_pcNode = m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->front();
			m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
			//to be added
		}

		itFind = m_mGroup2Pos.find(wxT("Delay"));

		if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Pos.end())
		{
			l_pcNode = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->front();
			m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
		}

		itFind = m_mGroup2Pos.find(wxT("Scheduled"));

		if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Pos.end())
		{
			l_pcNode = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->front();
			m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
		}

	}


	 //assign value sets of each group
	for (auto itMap = m_mGroup2Pos.begin(); itMap != m_mGroup2Pos.end(); ++itMap)
		{
			SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[itMap->second];

			if (!l_pcAttr) continue;

			wxChoice* l_pcCombobox = m_apcComboBoxes[itMap->second];

			if (!l_pcCombobox) continue;

			l_pcCombobox->Clear();

			//add value sets of marking group
			if (itMap->first == wxT("Marking"))
			{
				for (unsigned int i = 0; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					if (l_sSetName.Contains(wxT(":")))//by george
					{
						l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
						if (l_pcCombobox)
							l_pcCombobox->Append(l_sSetName);
					}
					i = i + 2;

				}
			}

			//add value sets of stoch rates
			if (itMap->first == wxT("Rate") || itMap->first == wxT("Weight"))
			{
				for (unsigned int i = 1; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					l_sSetName = l_sSetName.BeforeFirst(wxT(':'));

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);
				}
			}

			if (itMap->first == wxT("Delay"))
			{
				for (unsigned i = 1; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					l_sSetName = l_sSetName.BeforeFirst(wxT(':'));

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);

					i++;
					i++;
				}
			}

			if (itMap->first == wxT("Schedule"))
			{
				for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetCell(i, 0);

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);
				}
				if (l_pcCombobox)
					l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
			}

			//set the active sets
			if (l_pcCombobox->GetCount() <= l_pcAttr->GetActiveColumn())
			{
				l_pcCombobox->SetSelection(0);
				l_pcAttr->SetActiveColumn(0);
			}
			else
				l_pcCombobox->SetSelection(l_pcAttr->GetActiveColumn());
		}


//assign constant groups and their v-sets
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
	    	    SP_DS_ColListAttribute* l_pcCollistVlist=dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
	    	    if(l_pcCollistVlist!=NULL)
	         {
	    	    	  m_apcColListAttrForConstants.push_back(l_pcCollistVlist);
	    	      break;
	         }

	     }
	   }
	}


	for (size_t j = 0; j < m_apcColListAttrForConstants.size(); j++)
	{
	   SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttrForConstants[j];
	   wxChoice* l_pcCombobox = m_apcComboBoxes1[j];


	  if (l_pcAttr && l_pcCombobox)
	   {
		  l_pcCombobox->Clear();

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
	m_bRestartAnimationFlag = true;
}


bool
SP_DS_ColStAnimation::StartTimer()
{
	if (m_bRestartAnimationFlag)
	{

		//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
		m_bRestartAnimationFlag = false;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Here add the codes to check syntax before animation

		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.SyntaxChecking())
			return false;


		if (!LoadUnfoldingInformation())
			return false;



		////////////////////////////////////////////////////////////////////////////////////////////////////////////

		m_pcSimulator->SetOutputStartPoint(0);
		m_pcSimulator->SetOutputEndPoint(100);

		LoadCurrentMarking();

		if (!m_pcSimulator->Initialise(true))
		{
			StopTimer();
			m_bRestartAnimationFlag = true;
			return false;
		}
	}

	return SP_DS_Animation::StartTimer();

}


int
SP_DS_ColStAnimation::ChooseTransition()
{

	wxString l_sOutputLabel = wxT("");
	list< SP_DS_ColStTransAnimator* > ::iterator l_itTrans;

	SetCurrentMarking();

	SP_DS_StAnimationRunElem* l_pcRunElem = NULL;

	spsim::AnimationInfo* l_pcAnimationInfo = m_pcSimulator->GetNextAnimationRunElem();

	if (l_pcAnimationInfo != NULL)
	{
		l_pcRunElem = new SP_DS_StAnimationRunElem();

		l_pcRunElem->timepoint = l_pcAnimationInfo->GetCurrentTime();

		l_pcRunElem->transitionArrayPos = l_pcAnimationInfo->GetTransitionPosition();

		l_pcRunElem->actionDescription->push_back(l_pcAnimationInfo->GetActionDescription());

		l_pcRunElem->type = (SP_DS_StAnimationRunElemType)(l_pcAnimationInfo->GetActionType());
	}

	if (!l_pcRunElem)
	{
		return -1;
	}

	if (l_pcRunElem->type == SP_ST_ANIM_RUN_FIRE_TRANSITION)
	{
		l_sOutputLabel << wxString::Format(wxT("%f"), l_pcRunElem->timepoint) << wxT("  :  ") <<
			*l_pcRunElem->actionDescription->begin();

		//m_pcOutputLabelStaticText->SetLabel(l_sOutputLabel);

		int l_nReturn = l_pcRunElem->transitionArrayPos;

		//has to be deleted, because of new in m_pcSimulator->GetNextAnimationRunElem()
		wxDELETE(l_pcRunElem);

		return l_nReturn;
	}

	return -1;

}


bool
SP_DS_ColStAnimation::ReduceColTransitions()
{

	if (dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep))
	{
		return TRUE;
	}
	/*	else if(dynamic_cast<SP_DS_CoarseTransAnimator*>(m_pcSingleStep))
	{
	SP_DS_PedAnimation::ReduceTransitions();
	return TRUE;
	}
	*/
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;

	SP_DS_ColStTransAnimator* l_pcChosenTransition = NULL;

	wxString l_sTransitionName;

	if (m_lStepTransAnimators.size() > 0)
	{
		int l_nChooseTransition = ChooseTransition();
		int l_nChooseColoredTransition;
		if (l_nChooseTransition >= 0)
		{
			map<long, SP_CPN_CountInterval>::iterator itMap;
			for (itMap = m_pnsTransitionCountById.begin(); itMap != m_pnsTransitionCountById.end(); itMap++)
			{
				if ((unsigned int)l_nChooseTransition >= itMap->second.m_nLow && (unsigned int)l_nChooseTransition <= itMap->second.m_nUp)
				{
					l_nChooseColoredTransition = itMap->first;
					break;
				}
			}
			l_pcChosenTransition = m_mpcTransitionAnimators[l_nChooseColoredTransition];
			l_sTransitionName = m_msTransitionNamesById[wxString::Format(wxT("%d"), l_nChooseTransition)];
		}

		for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
		{
			if (l_pcChosenTransition != (*l_itTrans))
			{
				(*l_itTrans)->Reset();
			}
		}
		m_lStepTransAnimators.clear();
		if (l_pcChosenTransition)
			m_lStepTransAnimators.push_back(l_pcChosenTransition);


		//choose the multiplicies of the arcs to be fired 
		if (l_nChooseTransition >= 0)
		{
			SP_DS_Node* l_pcParentNode = l_pcChosenTransition->GetParentNode();
			ChooseArcMulitplicity(l_pcParentNode, l_sTransitionName);
		}

	}

	return TRUE;

}

void SP_DS_ColStAnimation::ChooseArcMulitplicity(SP_DS_Node* p_pcNode, wxString p_sTransitionName)
{
	SP_DS_Node* l_pcNode = p_pcNode;

	if (!l_pcNode)
		return;

	SP_ListEdge::const_iterator l_itEdges;

	SP_DS_ColListAttribute* l_pcColList;



	for (l_itEdges = l_pcNode->GetTargetEdges()->begin(); l_itEdges != l_pcNode->GetTargetEdges()->end(); ++l_itEdges)
	{
		vector<wxString> l_ColorVector;
		vector<wxString> l_MultiplicityVector;

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if (!l_pcColList)
			return;
		for (unsigned i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if (i == 0)
			{
				l_ColorVector.push_back(l_pcColList->GetCell(0, 0));
				l_MultiplicityVector.push_back(l_pcColList->GetCell(0, 1));
			}
			if (l_pcColList->GetCell(i, 0) == p_sTransitionName)
			{
				long l_nNumber;
				if (!l_pcColList->GetCell(i, 1).ToLong(&l_nNumber))
					return;
				for (unsigned j = i + 1; j <= i + l_nNumber; j++)
				{
					l_ColorVector.push_back(l_pcColList->GetCell(j, 0));
					l_MultiplicityVector.push_back(l_pcColList->GetCell(j, 1));
				}
				break;
			}
		}
		//modify the collist

		l_pcColList->Clear();
		for (unsigned i = 0; i <l_ColorVector.size(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, l_ColorVector[i]);
			l_pcColList->SetCell(l_nNewRow, 1, l_MultiplicityVector[i]);
		}
	}



	for (l_itEdges = l_pcNode->GetSourceEdges()->begin(); l_itEdges != l_pcNode->GetSourceEdges()->end(); ++l_itEdges)
	{
		vector<wxString> l_ColorVector;
		vector<wxString> l_MultiplicityVector;

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));

		if (!l_pcColList)
			return;
		for (unsigned i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if (i == 0)
			{
				l_ColorVector.push_back(l_pcColList->GetCell(0, 0));
				l_MultiplicityVector.push_back(l_pcColList->GetCell(0, 1));
			}
			if (l_pcColList->GetCell(i, 0) == p_sTransitionName)
			{
				long l_nNumber;
				if (!l_pcColList->GetCell(i, 1).ToLong(&l_nNumber))
					return;
				for (unsigned j = i + 1; j <= i + l_nNumber; j++)
				{
					l_ColorVector.push_back(l_pcColList->GetCell(j, 0));
					l_MultiplicityVector.push_back(l_pcColList->GetCell(j, 1));
				}
				break;
			}
		}
		//modify the collist

		l_pcColList->Clear();
		for (unsigned i = 0; i <l_ColorVector.size(); i++)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, l_ColorVector[i]);
			l_pcColList->SetCell(l_nNewRow, 1, l_MultiplicityVector[i]);
		}
	}

}

void SP_DS_ColStAnimation::LoadCurrentMarking()
{
	int l_nSel;

	if (m_apcComboBoxes.size() == 0) //the first time to open animation
	{
		l_nSel = 0;
	}
	else
	{
		if (!m_apcComboBoxes[0])
			return;
		l_nSel = m_apcComboBoxes[0]->GetSelection();
	}

	m_anCurrentMarking.clear();

	for (unsigned i = 0; i < m_anNetMarkings.size(); i++)
	{
		m_anCurrentMarking.push_back(m_anNetMarkings[i][l_nSel]);
	}

	list< SP_DS_ColStPlaceAnimator* >::iterator l_itPlace;

	if (m_lAllPlaceAnimators.size() <= 0)
	{
		return;
	}

	SP_DS_Node* l_pcNode;

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		l_pcNode = (*l_itPlace)->GetNode();

		SP_CPN_PlaceMultiSet* l_pcPlaceMultiSet = (*l_itPlace)->GetPlaceMultiSet();
		l_pcPlaceMultiSet->clear();

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		unsigned int l_nColoredPlacePos = 0;
		for (unsigned i = 0; i < m_msColoredPlaceNames.size(); i++)
		{
			if (m_msColoredPlaceNames[i] == l_sPlaceName)
			{
				l_nColoredPlacePos = i;
				break;
			}
		}

		unsigned l_nLow = m_nsPlaceCountById[l_nColoredPlacePos].m_nLow;
		unsigned l_nUp = m_nsPlaceCountById[l_nColoredPlacePos].m_nUp;

		for (unsigned j = l_nLow; j <= l_nUp; j++)
		{
			wxString l_sColor = m_msColorsOfPlace[j];
			long l_nMarking = m_anCurrentMarking[j];
			l_pcPlaceMultiSet->AddMultiSet(l_sColor, l_nMarking);
		}

		(*l_itPlace)->UpdateMarking(); //Update the marking of a place

	}
}


void
SP_DS_ColStAnimation::SetCurrentMarking()
{

	for (unsigned i = 0; i < m_anCurrentMarking.size(); i++)
	{
		m_anCurrentMarking[i] = 0;
	}


	list< SP_DS_ColStPlaceAnimator* >::iterator l_itPlace;

	if (m_lAllPlaceAnimators.size() <= 0)
	{
		return;
	}

	SP_DS_Node* l_pcNode;

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		l_pcNode = (*l_itPlace)->GetNode();

		SP_CPN_PlaceMultiSet* l_pcPlaceMultiSet = (*l_itPlace)->GetPlaceMultiSet();


		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_MapString2Int* m_pMultiSetMap = l_pcPlaceMultiSet->GetMultiSetMap();
		SP_MapString2Int::iterator itMap;
		for (itMap = m_pMultiSetMap->begin(); itMap != m_pMultiSetMap->end(); itMap++)
		{
			wxString l_sColor = itMap->first;
			int l_nAppearance = itMap->second;
			long l_nPos = 0;
			wxString l_sUnfoldedPlaceName = l_sPlaceName + wxT("_") + l_sColor;

			l_sUnfoldedPlaceName.Replace(wxT("("), wxT("_"));
			l_sUnfoldedPlaceName.Replace(wxT(")"), wxT("_"));
			l_sUnfoldedPlaceName.Replace(wxT(","), wxT("_"));

			if (m_mnPlaceArrayPos.find(l_sUnfoldedPlaceName) != m_mnPlaceArrayPos.end())
				l_nPos = m_mnPlaceArrayPos[l_sUnfoldedPlaceName];

			m_anCurrentMarking[l_nPos] = l_nAppearance;
		}
	}

	SP_VectorLong& l_anCurrentMarking = m_pcSimulator->GetCurrentMarking();

	//wxString l_sTest ;
	for (unsigned i = 0; i < m_anCurrentMarking.size(); i++)
	{
		l_anCurrentMarking[i] = m_anCurrentMarking[i];
	}



}

bool SP_DS_ColStAnimation::LoadUnfoldingInformation()
{
	wxDELETE(m_pcUnfolding);
	m_pcUnfolding = new SP_DS_ColPN_Unfolding;
	if (!m_pcUnfolding->Unfolding())
	{
		return false;
	}

	m_pnsTransitionCountById = *(m_pcUnfolding->GetTransitionCountById());
	m_msTransitionNamesById = *(m_pcUnfolding->GetAnimationTransitionNamesById()); //only used here
	m_anNetMarkings = *(m_pcUnfolding->GetNetMarkings());
	m_msColorsOfPlace = *(m_pcUnfolding->GetColorsOfPlace());
	m_msColoredPlaceNames = *(m_pcUnfolding->GetColoredPlaceNames());
	m_nsPlaceCountById = *(m_pcUnfolding->GetPlaceCountById());
	m_mnPlaceArrayPos = *(m_pcUnfolding->GetPlaceArrayPos());

	m_msColoredTransitionNames = *(m_pcUnfolding->GetColoredTransitionNames());


	//Set folding information to m_pcSimulator
	LoadPlaces();

	LoadTransitions();

	LoadConnections();

	LoadParameters();

	return true;
}
void SP_DS_ColStAnimation::LoadPlaces()
{

	m_pcSimulator->SetPlaceCount(m_pcUnfolding->GetPlaceNames()->size());


	m_pcSimulator->SetPlaceNames(*m_pcUnfolding->GetPlaceNames());

	m_pcSimulator->SetInitialMarking(*m_pcUnfolding->GetCurColStMarking());

}

void SP_DS_ColStAnimation::LoadParameters()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_PARAM);
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	l_anParameterValue.clear();
	l_asParameterNames.clear();

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	SP_ListMetadata::const_iterator it;

	 

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if (l_FE.IsOk())
		{
			double l_nValue = 0;
			SP_FunctionPtr l_Function = l_FE.getFunction();
			if (l_Function->isValue())
			{
				l_nValue = l_Function->getValue();
			}
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if (l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();
				}
				else if (l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
				}
			}
			//****************************************************************************************
			l_asParameterNames.push_back(l_sName);
			l_anParameterValue.push_back(l_nValue);
		}
	}

	m_pcSimulator->SetParameterNames(l_asParameterNames);
	m_pcSimulator->SetParameterValues(l_anParameterValue);
	/*
	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		//Set the transition name
		l_asParameterNames.push_back(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());

		//Get the transition rate function
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("ParameterList")));
		l_anParameterValue.push_back(l_pcColList->GetActiveCellValueDouble(1));
	}

	m_pcSimulator->SetParameterNames(l_asParameterNames);
	m_pcSimulator->SetParameterValues(l_anParameterValue);
	*/

}

void SP_DS_ColStAnimation::LoadTransitions()
{

	m_pcSimulator->ClearTransitions();

	m_pcSimulator->SetTransitionCount(m_pcUnfolding->GetTransitionNames()->size());

	SP_VectorString* l_pcTransitionName = m_pcUnfolding->GetTransitionNames();
	SP_VectorString* l_pcTransitionFunction = m_pcUnfolding->GetCurRateFunction();
	SP_VectorString* l_pcTransitionType = m_pcUnfolding->GetTransNodeType();

	//bug fix:by george, fillout the empty rate funs with zeros, this is special case when unfolding using idd
	for (unsigned i = 0; i < l_pcTransitionFunction->size(); i++)
	{
		if (l_pcTransitionFunction->at(i).IsEmpty())
		{
			l_pcTransitionFunction->at(i) = wxT("0");
		}
	}

	for (unsigned long l_nTransPos = 0; l_nTransPos<l_pcTransitionName->size(); l_nTransPos++)
	{

		m_pcSimulator->AddTransition(
			(*l_pcTransitionName)[l_nTransPos],

			(*l_pcTransitionFunction)[l_nTransPos],

			GetTransitionType((*l_pcTransitionType)[l_nTransPos])
		);
	}
}

spsim::TransitionType SP_DS_ColStAnimation::GetTransitionType(const wxString& p_sTransitionType)
{
	if (p_sTransitionType == SP_DS_CONTINUOUS_TRANS)
	{
		return spsim::TRANSITION_TYPE_CONTINUOUS;
	}
	else if (p_sTransitionType == SP_DS_STOCHASTIC_TRANS)
	{
		return spsim::TRANSITION_TYPE_STOCHASTIC;
	}
	else if (p_sTransitionType == SP_DS_DETERMINISTIC_TRANS)
	{
		return spsim::TRANSITION_TYPE_DETERMINISTIC;
	}
	else if (p_sTransitionType == SP_DS_IMMEDIATE_TRANS)
	{
		return spsim::TRANSITION_TYPE_IMMEDIATE;
	}
	else if (p_sTransitionType == SP_DS_SCHEDULED_TRANS)
	{
		return spsim::TRANSITION_TYPE_SCHEDULED;
	}
	else
	{
		return spsim::TRANSITION_TYPE_UNKNOWN;
	}
}


void SP_DS_ColStAnimation::LoadConnections()
{
	vector<SP_CPN_ContinuousArc>* l_pcPreConnection;
	vector<SP_CPN_ContinuousArc>* l_pcPostConnection;
	vector<SP_CPN_ContinuousArc>::const_iterator l_itConnection;

	//Map transition name to their positions
	m_pcSimulator->MapTransitionNames2Positions();

	m_pcSimulator->MapPlaceNames2Positions();

	//Load transitions' pre place
	l_pcPreConnection = m_pcUnfolding->GetArcs_Place2Transition();
	for (l_itConnection = l_pcPreConnection->begin(); l_itConnection != l_pcPreConnection->end(); l_itConnection++)
	{
		double l_nWeight = 0;

		l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		m_pcSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, GetConnectionType(l_itConnection->m_sArcClass), l_nWeight);
	}

	//Load transitions' post place
	l_pcPostConnection = m_pcUnfolding->GetArcs_Transition2Place();
	for (l_itConnection = l_pcPostConnection->begin(); l_itConnection != l_pcPostConnection->end(); l_itConnection++)
	{
		double l_nWeight = 0;

		l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		m_pcSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nWeight);
	}
}

spsim::ConnectionType SP_DS_ColStAnimation::GetConnectionType(const wxString& p_sConnectionType)
{
	if (p_sConnectionType == SP_DS_EDGE)
	{
		return spsim::STANDARD_ARC;
	}
	else if (p_sConnectionType == SP_DS_READ_EDGE)
	{
		return spsim::READ_ARC;
	}
	else if (p_sConnectionType == SP_DS_INHIBITOR_EDGE)
	{
		return spsim::INHIBITOR_ARC;
	}
	else if (p_sConnectionType == SP_DS_MODIFIER_EDGE)
	{
		return spsim::MODIFIER_ARC;
	}
	else if (p_sConnectionType == SP_DS_RESET_EDGE)
	{
		return spsim::RESET_ARC;
	}
	else if (p_sConnectionType == SP_DS_EQUAL_EDGE)
	{
		return spsim::EQUAL_ARC;
	}
	else
	{
		SP_LOGERROR(wxT("Unknown connection type,") + p_sConnectionType);
		return spsim::OTHER_CONNECTION;
	}
}

bool SP_DS_ColStAnimation::ChooseColTransitions()
{
	if (dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep))
	{
		return TRUE;
	}

	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;

	SP_DS_ColStTransAnimator* l_pcChosenTransition = NULL;

	int l_nChooseTransition = ChooseTransition();
	if (l_nChooseTransition >= 0)
	{
		map<long, SP_CPN_CountInterval>::iterator itMap;
		for (itMap = m_pnsTransitionCountById.begin(); itMap != m_pnsTransitionCountById.end(); itMap++)
		{
			if ((unsigned int)l_nChooseTransition >= itMap->second.m_nLow && (unsigned int)l_nChooseTransition <= itMap->second.m_nUp)
			{
				int l_nChooseColoredTransition = itMap->first;
				wxString l_sColoredTransName = m_msColoredTransitionNames[l_nChooseColoredTransition];
				map< int, SP_DS_ColStTransAnimator* >::iterator itMapAni;
				for (itMapAni = m_mpcTransitionAnimators.begin(); itMapAni != m_mpcTransitionAnimators.end(); itMapAni++)
				{
					wxString ll = dynamic_cast<SP_DS_NameAttribute*>(itMapAni->second->GetParentNode()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if (dynamic_cast<SP_DS_NameAttribute*>(itMapAni->second->GetParentNode()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue() == l_sColoredTransName)
					{
						l_pcChosenTransition = itMapAni->second;
						break;
					}
				}
				//l_pcChosenTransition = m_mpcTransitionAnimators[l_nChooseColoredTransition];
				break;
			}
		}
	}

	m_lPossibleTransAnimators.clear();
	if (l_pcChosenTransition)
	{
		m_lPossibleTransAnimators.push_back(l_pcChosenTransition);
	}

	return TRUE;

}


void  SP_DS_ColStAnimation::UpdateChoices()//george
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


void SP_DS_ColStAnimation::OnColConstantSetsChanged(wxCommandEvent& p_cEvent)
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
	/**
	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

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
	 	LoadSets();

		if (m_pcUnfolding)
			wxDELETE(m_pcUnfolding);

		if (!LoadUnfoldingInformation())
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			m_bRestartAnimationFlag = false;  // let it not animate
			return;
		}

		 
	 

	 
	//////////////////////////////////////////////////////////////////
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->UpdateDefaultMarking();
	}


	*/
}


void SP_DS_ColStAnimation::OnModifyConstants(wxCommandEvent& p_cEvent)
{
		SP_DLG_ConstantDefinition * l_pcConstantDialog = new SP_DLG_ConstantDefinition(NULL);

		if (l_pcConstantDialog->ShowModal() == wxID_OK)
		{
			LoadSets();
		}

	 
	}

void SP_DS_ColStAnimation::LoadDefaultConstantsGroups()
{
	for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
	{
		if (m_apcColListAttrForConstants[i])
		{
			int vsel = 0;

			m_apcColListAttrForConstants[i]->SetActiveList(vsel);

		}
	}
}

void SP_DS_ColStAnimation::UpdateColMarking()
{

	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

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
	if(!m_IsDestructor)
	LoadSets();

	if (m_pcUnfolding)
		wxDELETE(m_pcUnfolding);

	if (!LoadUnfoldingInformation())
	{
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
		m_bRestartAnimationFlag = false;  // let it not animate
		return;
	}

	 
	//////////////////////////////////////////////////////////////////
	 


		for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		{
			(*l_Iter)->UpdateDefaultMarking();
		}
	
		SP_Core::Instance()->GetRootDocument()->Refresh();
		Refresh();
}

void SP_DS_ColStAnimation::SetStepCounter()
{
	m_pcStepCounterValue->SetLabel(wxString::Format(wxT("%ld"), m_nStepCount));
}

void SP_DS_ColStAnimation::ExportDetailsCPN(ExportStochCPN *export_frame)
{
	wxString l_start_str = export_frame->m_pc_StartText->GetLineText(0);
	wxString l_every_str = export_frame->m_pc_EveryText->GetLineText(0);
	wxString l_stop_str = export_frame->m_pc_StopText->GetLineText(0);
	wxString l_temp;
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;

	m_ExportFilename = export_frame->m_pc_Browse->GetPath();
	m_bExport = true;
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
}

void SP_DS_ColStAnimation::OnExport(wxCommandEvent &p_pc_Event)
{
	ExportStochCPN *export_frame = new ExportStochCPN(wxT("Export Details"), this);

	export_frame->Show(true);
}

void SP_DS_ColStAnimation::ExportMarkings()
{
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
	//list<SP_DS_CPN_PlaceAnimator*>::iterator l_itPlace;

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

void SP_DS_ColStAnimation::ExportStepSequences()
{
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;

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

					SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Name"));
					SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
					wxString l_sName;//l_pcNameAttr->GetValue();
					l_sName = m_sTriggiredUnfoldedTrans;
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

void SP_DS_ColStAnimation::OnImport(wxCommandEvent &event)
{
	ImportColAnim *import_frame = new ImportColAnim(wxT("Import Details"), this);

	import_frame->Show(true);
}

void SP_DS_ColStAnimation::ImportDetails(ImportColAnim *import_frame)
{

	list<SP_DS_ColStTransAnimator *>::iterator l_itTrans;
	m_bImport = true;
	m_bInvalid = false;
	m_nLineCount = 1;
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

				//Get the third token, tokenize it with "." as delimiter and take the first token from it
				wxStringTokenizer l_pc_trans(l_pc_tkz.GetNextToken(), wxT("."));
				if (!l_pc_trans.GetNextToken().ToLong(&l_trans_id))
					;

				for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
				{
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetParentNode();
					SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));
					SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
					long int l_sId = l_pcIdAttr->GetValue();

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
				SP_MESSAGEBOX(wxT("Import file does not match with the given petri net."), wxT("Error"), wxOK | wxICON_ERROR);
			}
			else
			{
				SP_LOGMESSAGE(wxT("Import file '") + m_ImportFilename + wxT("' successfully."));
			}
		}
	}

}

void SP_DS_ColStAnimation::ImportStepSequences()
{
	if (m_bInvalid || m_ImportTextfile.Open(m_ImportFilename) == false)
	{
		m_lPossibleTransAnimators.clear();
		SP_MESSAGEBOX(wxT("Error in opening file ") + m_ImportFilename, wxT("Notification"), wxOK | wxICON_ERROR);
	}
	else
	{
		list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
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
					m_lPossibleTransAnimators.push_back(*l_itTrans);
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
						m_lPossibleTransAnimators.push_back(*l_itTrans);
				}

				SP_LOGDEBUG(wxString::Format("Transition ID: %ld\n", l_trans_id));

			}
		}

		m_ImportTextfile.Close();

	}
}
