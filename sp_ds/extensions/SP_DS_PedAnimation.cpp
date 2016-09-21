//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

//by avinash
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filepicker.h>
#include <wx/filefn.h>
#include <wx/tokenzr.h>
#include <wx/gbsizer.h>

#include "sp_ds/extensions/SP_DS_PedAnimation.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
//bysl
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_gui/dialogs/SP_DLG_MarkingOverview.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include<iostream>
using namespace std;

enum
{
	SP_ID_PEDRESET = SP_ID_LAST_ID + 1,
	SP_ID_PEDSET,
	SP_ID_PEDKEEP,
	SP_ID_PEDAUTOCONCURRENCY,
	SP_ID_PEDEXPORT,
	SP_ID_PEDIMPORT,
	SP_ID_PEDEXPORT_CANCEL,
	SP_ID_PEDIMPORT_CANCEL,
	SP_ID_TIMER,
	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_CHOICE_CONSTANTS_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANTS_SETS,
	SP_ID_STATIC_TEXT_OUTPUT_LABEL
};

BEGIN_EVENT_TABLE(SP_DS_PedAnimation, SP_DS_Animation)
EVT_BUTTON(SP_ID_PEDSET, SP_DS_PedAnimation::OnSet)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDIMPORT, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDEXPORT, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_CONSTANTS_SETS, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_CONSTANTS_SETS, SP_DS_PedAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_PedAnimation::OnUpdateUI)
EVT_BUTTON(SP_ID_PEDEXPORT,SP_DS_PedAnimation::OnExport)
EVT_BUTTON(SP_ID_PEDIMPORT,SP_DS_PedAnimation::OnImport)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_PedAnimation :: OnModifyMarkingSets )
EVT_BUTTON(SP_ID_BUTTON_MODIFY_CONSTANTS_SETS, SP_DS_PedAnimation :: OnModifyConstantSets )
EVT_CHOICE(SP_ID_CHOICE_CONSTANTS_SETS, SP_DS_PedAnimation::OnConstantSetsChanged )
END_EVENT_TABLE()

SP_DS_PedAnimation::SP_DS_PedAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
	SP_DS_Animation(p_nRefresh, p_nDuration),
	m_nStepState((int)p_eStepping),
	m_bAutoConcurrency(false),
	m_pcChoice(NULL),
	m_cbKeep(NULL),
	m_cbAutoConcurrency(NULL),
	m_pcSingleStep(NULL),
	m_nStepCount (0),
	m_nStart(0),
	m_nEvery(0),
	m_nStop(0),
	m_bImport(false),
	m_bInvalid(false),
	m_pcDialog(NULL)
{
}

SP_DS_PedAnimation::~SP_DS_PedAnimation()
{
	if (m_cbKeep)
	{
		if (m_cbKeep->IsChecked())
		{
			KeepMarking();
		}
		else
		{
			list<SP_DS_PlaceAnimator*>::iterator l_Iter;
			for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
			{
				(*l_Iter)->ResetMarking();
				//by sl
				(*l_Iter)->MarkingSourceEdges();
				(*l_Iter)->MarkingTargetEdges();
			}
		}
		wxGetApp().GetAnimationPrefs()->SetKeepMarking(m_cbKeep->IsChecked());
		wxDELETE((m_cbKeep));
	}

	//m_pcGraph->GetParentDoc()->DrawAllElements(false,true);
	//refresh the editor
	Refresh();
}

SP_DS_Animation*
SP_DS_PedAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*> (new SP_DS_PedAnimation(m_nRefreshFrequ, m_nStepDuration, SP_ANIM_STEP_T(m_nStepState)));
}

bool SP_DS_PedAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
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
				m_lAllPlaceAnimators.push_back(dynamic_cast<SP_DS_PlaceAnimator*> (l_pcAnimator));
			else
				if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_TRANS)
					m_lAllTransAnimators.push_back(dynamic_cast<SP_DS_TransAnimator*> (l_pcAnimator));
		}
	}

	//init animation history
	wxString nc = p_pcGraph->GetNetclass()->GetName();
	m_nHistorySize = wxGetApp().GetAnimationPrefs()->GetHistorySize(nc);
	m_llHistoryTransAnimators.clear();

	if (nc.Cmp(SP_DS_TIMEPN_CLASS) == 0)
	{
		SP_MESSAGEBOX(wxT("Attention: time constraints are not considered in animation"));
	}

	LoadInitialMarking();

	return l_bReturn;
}

bool SP_DS_PedAnimation::PreStep()
{
	list<list<SP_DS_TransAnimator*> >::iterator l_itHistoryCurrent;
	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;

	unsigned int l_nSteps = m_nSteps / 2;

	// setting the right edge value
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		(*l_itPlace)->MarkingSourceEdges();
		(*l_itPlace)->MarkingTargetEdges();
	}

	if(GetDirection() == FORWARD)
		m_nStepCount++; //Increment the step count

	else if(GetDirection() == BACKWARD)
		m_bExport = false; //If user interrupts the animation,export should be stopped

	if(m_bExport == true)
		ExportMarkings();

	m_lPossibleTransAnimators.clear();

	if (m_bImport == false)
	{
		//selecting transitions (candidates) to fire ...
		if (dynamic_cast<SP_DS_TransAnimator*> (m_pcSingleStep))
		{
			//...for steps that are triggered by clicking directly on a transition
			m_lPossibleTransAnimators.push_back(dynamic_cast<SP_DS_TransAnimator*> (m_pcSingleStep));
			//if back-firing one transition directly, anim history is deleted
			if (GetDirection() == BACKWARD)
			{
				m_llHistoryTransAnimators.clear();
			}
		}
		else
			if (dynamic_cast<SP_DS_CoarseTransAnimator*> (m_pcSingleStep))
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

			else
				if (GetDirection() == BACKWARD && !m_llHistoryTransAnimators.empty())
				{
					//...for backsteps using the history
					l_itHistoryCurrent = --(m_llHistoryTransAnimators.end());
					list<SP_DS_TransAnimator*>& l_rlHistory = (*l_itHistoryCurrent);
					for (l_itTrans = l_rlHistory.begin(); l_itTrans != l_rlHistory.end(); ++l_itTrans)
					{
						m_lPossibleTransAnimators.push_back(*l_itTrans);
					}
					m_llHistoryTransAnimators.erase(l_itHistoryCurrent, m_llHistoryTransAnimators.end());
				}

				else
				{
					//...for stepping forward and random backsteps
					for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
					{
						m_lPossibleTransAnimators.push_back(*l_itTrans);
					}
				}
	}

	else if (m_bImport == true)
		ImportStepSequences();

	//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		(*l_itTrans)->InformPrePlaces();
	}
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		(*l_itPlace)->TestMarking();
	}
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		//TODO check, whether SetEnabled(true) could be called inside TestConcession()
		if ((*l_itTrans)->TestConcession())
		{
			(*l_itTrans)->SetEnabled(true);
		}
	}

	if (m_nStepState != SP_ANIM_STEP_SINGLE && m_pcSingleStep == NULL)
	{
		// Changed by Erik Jongsma: fixing a bug in the conflict resolution.
		// First, build a list of transitions that are in conflict. The rest is enabled.
		for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		{
			(*l_itPlace)->BuildConflictList(m_lConflictTransAnimators);
		}
		/**
		 * this is essential for correct animation, the list of conflict transitions has to be unique
		 */
		m_lConflictTransAnimators.sort();
		m_lConflictTransAnimators.unique();

		// Then, we solve the conflicts
		SolveConflicts();
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
	ReduceTransitions();

	// Check if autoconcurrency is enabled, and if so, maximize firings of the enabled transitions
	if (m_bAutoConcurrency)
	{
		MaximizeFirings();
	}

	m_cFiredTransitions.clear();
	for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
	{
		m_cFiredTransitions.push_back((*l_itTrans)->GetAnimObject());
	}
	if (!m_cFiredTransitions.empty())
	{
		SP_DS_Graph *graph = m_cFiredTransitions.front()->GetClassObject()->GetParentGraph();
		if (wxGetApp().GetIAManager() && wxGetApp().GetIAManager()->IAModeEnabled(graph))
		{
			if (GetDirection() == FORWARD)
			{
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
	if (m_nHistorySize > 0 && m_lStepTransAnimators.size() > 0 && GetDirection() == FORWARD)
	{
		m_llHistoryTransAnimators.push_back(m_lStepTransAnimators);
		//obey history limit
		if (m_llHistoryTransAnimators.size() > m_nHistorySize)
		{
			m_llHistoryTransAnimators.pop_front();
		}
	}

	// if there are only transitions w/out pre-places...
	if (m_lStepPlaceAnimators.size() == 0)
	{
		//...skip first part of animation (place to transition),
		m_nActStep = m_nSteps / 2;
	}
	else
	{
		//...else just start at the beginning
		m_nActStep++;
	}

	bool l_bReturn = (m_lStepPlaceAnimators.size() + m_lStepTransAnimators.size() > 0);

	if (!l_bReturn)
	{
		m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
		wxString msg;
		if (m_bOneShot)
		{
			if (GetSingleStep())
			{
				msg = wxT("This transition is not enabled!");

				if(m_nStepCount > 0)
					m_nStepCount--; //This transition was counted as a step, which is actually not possible
			}
			else
			{
				msg = wxT("Dead state: there are no enabled transitions");

				if(m_nStepCount > 0)
					m_nStepCount--; //Dead state, thus decrement the m_nStepCount
			}

			//SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION);
		}

		else
		{
			msg = wxT("Dead state: there are no more enabled transitions");

			if(m_nStepCount > 0)
			{
				m_nStepCount--; //Dead state, thus decrement the m_nStepCount
			}

		}

		SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION);

		if (m_pcDialog)
		{
			m_pcDialog->ResetPlayButtons();
		}
	}

	else
	{
	}

	if(m_bExport == true)
		ExportStepSequences();

	//This is to be put after ExportMarkings() & ExportStepSequences() are called.
	if(GetDirection() == BACKWARD && m_nStepCount > 0)
	{
		m_nStepCount--; //Decrement the step count
		m_bExport = false; //If user interrupts the animation,export should be stopped
	}

	SetStepCounter();

	return l_bReturn;
}

bool SP_DS_PedAnimation::Step()
{
	bool l_bReturn = TRUE;
	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;

	if (m_nActStep < (m_nSteps / 2))
	{
		for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
		{
			l_bReturn &= (*l_itPlace)->Step(m_nActStep);
		}
	}

	if (m_nActStep == (m_nSteps / 2))
	{
		for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
		{
			l_bReturn &= (*l_itPlace)->PostStep();
		}
		m_lStepPlaceAnimators.clear();
	}

	if (m_nActStep > (m_nSteps / 2))
	{
		for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
		{
			l_bReturn &= (*l_itTrans)->Step(m_nActStep);
		}
	}

	m_nActStep++;

	return l_bReturn;
}

bool SP_DS_PedAnimation::PostStep()
{
	bool l_bReturn = TRUE;
	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;

	m_cFiredTransitions.clear();

	for (l_itPlace = m_lStepPlaceAnimators.begin(); l_itPlace != m_lStepPlaceAnimators.end(); ++l_itPlace)
	{
		l_bReturn &= (*l_itPlace)->PostStep();
	}
	for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
	{
		l_bReturn &= (*l_itTrans)->PostStep();
		m_cFiredTransitions.push_back((*l_itTrans)->GetAnimObject());
	}

	// setting the right edge value
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		(*l_itPlace)->MarkingSourceEdges();
		(*l_itPlace)->MarkingTargetEdges();
	}

	m_lStepPlaceAnimators.clear();
	m_lStepTransAnimators.clear();
	m_lConflictTransAnimators.clear();
	m_nActStep = 0;
	m_pcSingleStep = NULL;
	Refresh();

	//submitting an interaction event
	if (!m_cFiredTransitions.empty())
	{
		//if any transition fired...
		SP_DS_Graph *graph = m_cFiredTransitions.front()->GetClassObject()->GetParentGraph();
		if (wxGetApp().GetIAManager() && wxGetApp().GetIAManager()->IAModeEnabled(graph))
		{
			//... and IAMode is enabled for this animated graph, we submit that event to the SP_IA_Manager
			if (GetDirection() == FORWARD)
			{
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMSTEP_FWD));
			}
			else
			{
				wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMSTEP_BWD));
			}
		}
	}

	//cout<<"In Post Step"<<endl;

	return l_bReturn;
}

bool SP_DS_PedAnimation::SolveConflicts()
{
	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	bool l_bDone = false;

	// First, we disable all transitions with a conflict
	for (l_itTrans = m_lConflictTransAnimators.begin(); l_itTrans != m_lConflictTransAnimators.end(); ++l_itTrans)
	{
		(*l_itTrans)->SetEnabled(false);
	}

	// We build a maximal step by enabling transitions one by one.
	while (!l_bDone)
	{
		if (m_lConflictTransAnimators.size() > 0)
		{
			int l_nChose = SP_RandomLong(m_lConflictTransAnimators.size());
			int i = 0;
			for (l_itTrans = m_lConflictTransAnimators.begin(); l_itTrans != m_lConflictTransAnimators.end(); ++l_itTrans)
			{
				if (l_nChose == i++)
				{
					(*l_itTrans)->SetEnabled(true);
					(*l_itTrans)->SimulateFiring(false);
					l_itTrans = m_lConflictTransAnimators.erase(l_itTrans);
					break;
				}
			}
			l_itTrans = m_lConflictTransAnimators.begin();
			while (l_itTrans != m_lConflictTransAnimators.end())
			{
				if (!(*l_itTrans)->TestConcession())
				{
					l_itTrans = m_lConflictTransAnimators.erase(l_itTrans);
				}
				else
				{
					++l_itTrans;
				}
			}
		}
		else
		{
			l_bDone = true;
		}
	}
	return true;
}

bool SP_DS_PedAnimation::MaximizeFirings()
{
	// For auto-concurrency, first, we copy the list of enabled transitions
	list<SP_DS_TransAnimator*>::iterator l_itTrans, l_itConflictTrans;
	list<SP_DS_TransAnimator*> l_lConflictTransAnimators;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	bool l_bDone = false;

	// First, we must build a list of transitions which might have a conflict when firing auto-concurrently
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		(*l_itPlace)->BuildAutoConflictList(l_lConflictTransAnimators);
	}

	/**
	 * this is essential for correct animation, the list of conflict transitions has to be unique
	 */
	l_lConflictTransAnimators.sort();
	l_lConflictTransAnimators.unique();

	for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
	{
		// Transitions without real preplaces only fire once, so they are not handled here.
		if ((*l_itTrans)->HasRealPrePlace())
		{
			// If the transition is not in the conflict set, we can compute its multiplicity in one go.
			bool l_bFound = false;
			for (l_itConflictTrans = l_lConflictTransAnimators.begin(); l_itConflictTrans != l_lConflictTransAnimators.end(); ++l_itConflictTrans)
			{
				if ((*l_itTrans) == (*l_itConflictTrans))
				{
					l_bFound = true;
				}
			}
			if (!l_bFound)
			{
				(*l_itTrans)->CalculateMultiplicity();
			}
			else
			{
				(*l_itTrans)->SimulateFiring(true);
			}
		}
	}

	while (!l_bDone)
	{
		for (l_itConflictTrans = l_lConflictTransAnimators.begin(); l_itConflictTrans != l_lConflictTransAnimators.end(); ++l_itConflictTrans)
		{
			if (!(*l_itConflictTrans)->IsAutoEnabled())
			{
				l_lConflictTransAnimators.erase(l_itConflictTrans);
			}
		}
		if (l_lConflictTransAnimators.size() > 0)
		{
			int l_nChose = SP_RandomLong(l_lConflictTransAnimators.size());
			int i = 0;
			for (l_itConflictTrans = l_lConflictTransAnimators.begin(); l_itConflictTrans != l_lConflictTransAnimators.end(); ++l_itConflictTrans)
			{
				if (l_nChose == i++)
				{
					(*l_itConflictTrans)->SimulateFiring(true);
					(*l_itConflictTrans)->IncreaseMultiplicity();
				}
			}
		}
		else
		{
			l_bDone = true;
		}
	}
	return true;
}

bool SP_DS_PedAnimation::ReduceTransitions()
{
	if (dynamic_cast<SP_DS_TransAnimator*> (m_pcSingleStep))
		return TRUE;

	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	SP_DS_TransAnimator* l_pcAnim = NULL;
	if (m_nStepState == SP_ANIM_STEP_SINGLE)
	{
		if (m_lStepTransAnimators.size() > 0)
		{
			unsigned int l_nChose = SP_RandomLong(m_lStepTransAnimators.size());
			unsigned int i = 0;
			for (l_itTrans = m_lStepTransAnimators.begin(); l_itTrans != m_lStepTransAnimators.end(); ++l_itTrans)
			{
				if (l_nChose == i++)
				{
					l_pcAnim = (*l_itTrans);
				}
				else
				{
					(*l_itTrans)->Reset();
				}
			}
			//SP_LOGDEBUG(wxString::Format(wxT("fire transition %s"), l_pcAnim->GetAnimObject()->GetAttribute(wxT("Name"))->GetValueString().c_str()));
			m_lStepTransAnimators.clear();
			m_lStepTransAnimators.push_back(l_pcAnim);
		}
	}
	else
		if (m_nStepState == SP_ANIM_STEP_INTERMEDIATE)
		{
			// two iterations, first choose one random entry, second choose a random set of the rest
			//SP_LOGDEBUG(wxString::Format(wxT("have %d"), m_lStepTransAnimators.size()));

			list<SP_DS_TransAnimator*> l_lNewTrans;
			//first round choose one transition randomly
			if (m_lStepTransAnimators.size() > 0)
			{
				unsigned int l_nChose = SP_RandomLong(m_lStepTransAnimators.size());
				unsigned int i = 0;
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
			}
		}
	return TRUE;
}

bool SP_DS_PedAnimation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	if (!SP_DS_Animation::AddToDialog(p_pcDlg, p_pcSizer))
		return FALSE;

	// Added by Erik Jongsma (MaximumAuto, number of options changed to 4)
	wxString l_asChoices[] =
	{ wxT("Maximum"), wxT("Intermediate"), wxT("Single") };
	m_pcChoice = new wxRadioBox(p_pcDlg, -1, wxT("Stepping"), wxDefaultPosition, wxDefaultSize, 3, l_asChoices, 1, wxRA_SPECIFY_ROWS);

	m_pcChoice->SetSelection(m_nStepState - 1);

	m_cbAutoConcurrency = new wxCheckBox(p_pcDlg, SP_ID_PEDAUTOCONCURRENCY, wxT("Enable Auto-concurrency."));

	m_cbAutoConcurrency->SetValue(m_bAutoConcurrency);

	p_pcSizer->Add(m_pcChoice, 0, wxEXPAND | wxALL, 5);
	p_pcSizer->Add(m_cbAutoConcurrency, 0, wxALL, 5);

	return TRUE;
}

bool SP_DS_PedAnimation::OnDialogOk()
{
	if (!m_pcChoice || !m_cbAutoConcurrency)
		return FALSE;

	m_nStepState = m_pcChoice->GetSelection() + 1;
	m_bAutoConcurrency = m_cbAutoConcurrency->GetValue();

	return SP_DS_Animation::OnDialogOk();
}

bool SP_DS_PedAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	wxString l_tmp;

	if (!p_pcCtrl || !p_pcSizer)
		return FALSE;

	m_pcDialog = p_pcCtrl;

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDSET, wxT("Keep Marking")), 1, wxALL | wxEXPAND, 5);
	m_cbKeep = new wxCheckBox(p_pcCtrl, SP_ID_PEDKEEP, wxT("Always keep marking when closing."));
	m_cbKeep->SetValue(wxGetApp().GetAnimationPrefs()->GetKeepMarking());
	p_pcSizer->Add(m_cbKeep, 0, wxALL, 5);

	//by avinash
	m_pcExportImportSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDEXPORT, wxT("Export")), 1, wxALL | wxEXPAND, 5);
	m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDIMPORT, wxT("Import")), 1, wxALL | wxEXPAND, 5);

	p_pcSizer->Add(m_pcExportImportSizer, 0, wxEXPAND);
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);

	l_tmp << m_nStepCount;
	m_pcStepCounter = new wxBoxSizer(wxHORIZONTAL);
	m_pcStepCounterText = new wxStaticText(p_pcCtrl,wxID_ANY,wxT("Step Counter"));
	m_pcStepCounterValue = new wxStaticText(p_pcCtrl,wxID_ANY,l_tmp);

	m_pcStepCounter->Add(m_pcStepCounterText,1,wxEXPAND | wxALL ,5);
	m_pcStepCounter->Add(m_pcStepCounterValue,1,wxEXPAND | wxALL ,5);

	p_pcSizer->Add(m_pcStepCounter,0,wxEXPAND);

   /*-----------------------------------------------------------------------------------*/

	/*
	 * bysl
	 * *******************************************************************************************************
	 */

	auto l_pcSetsSizer = new wxGridBagSizer(5,5);

	l_pcSetsSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Marking overview:")), wxGBPosition{0,0}, wxGBSpan{1,2});
	//l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("")), wxSizerFlags(0).Expand().Border(wxALL, 5));
	l_pcSetsSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), wxGBPosition{0,2}, wxDefaultSpan);

	//all other sets
	if (mc->GetElements()->size() > 0)
	{
		LoadGroups();

		for (unsigned int i = 0; i < m_choices.GetCount(); i++)
		{
			int row = i+1;
			l_pcSetsSizer->Add(new wxStaticText(p_pcCtrl, -1, m_choices[i] + wxT(':')), wxGBPosition(row,0));
			m_apcChoices.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_CONSTANTS_SETS, wxDefaultPosition, wxSize(100, -1), 0, NULL, 0, wxDefaultValidator, m_choices[i]));
			l_pcSetsSizer->Add(m_apcChoices[i], wxGBPosition(row,1), wxDefaultSpan,  wxEXPAND);
			l_pcSetsSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_CONSTANTS_SETS, wxT("Modify")), wxGBPosition(row,2), wxDefaultSpan);
		}
	}
	l_pcSetsSizer->AddGrowableCol(1);

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(l_pcSetsSizer, 1, wxALL | wxEXPAND, 5);

	LoadSets();
	LoadCurrentMarking();

	/*
	 * ***********************************************************************************************************
	 */

	p_pcCtrl->PushEventHandler(this);

	//allow backstepping
	m_pcDialog->EnableBackStepping(true);

	return TRUE;
}

/*
 * bysl
 */
void SP_DS_PedAnimation::LoadGroups()
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_ListMetadata::const_iterator it;
	SP_DS_Metadata* l_pcMetadata;

	m_choices.Clear();

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		bool flag = false;

		if (it == mc->GetElements()->begin())
		{
			l_pcMetadata = *it;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*> (l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			m_choices.Add(l_sGroup);
		}
		else
		{
			l_pcMetadata = *it;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*> (l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			for (unsigned int j = 0; j < m_choices.GetCount(); j++)
			{
				if (m_choices[j].Cmp(l_sGroup) != 0)
				{
					//group not in list
					flag = true;
				}
				else
				{
					//group is in list
					flag = false;
				}
			}
			if (flag == true)
				m_choices.Add(l_sGroup);
		}
	}

}

void SP_DS_PedAnimation::LoadSets()
{

	wxArrayString l_sGroupArray;

	if (m_apcColListAttr.empty())
	{
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

		m_apcColListAttr.resize(m_apcChoices.size());

		unsigned int l_nIndex = 0;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*> (l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			for (unsigned int i = 0; i < m_choices.GetCount(); i++)
			{
				if (m_choices[i].Cmp(l_sGroup) == 0)
				{
					bool flag = false;
					for (unsigned int j = 0; j < l_sGroupArray.GetCount(); j++)
					{
						if (l_sGroupArray[j].Cmp(l_sGroup) == 0)
						{
							flag = true;
						}
						else
						{
							flag = false;
						}
					}
					if (flag == false)
					{
						l_sGroupArray.Add(l_sGroup);
						m_apcColListAttr[l_nIndex] = dynamic_cast<SP_DS_ColListAttribute*> (l_pcConstant->GetAttribute(wxT("ValueList")));
						l_nIndex++;
					}
					//break;
				}
			}
		}
	}

	for (size_t j = 0; j < m_apcColListAttr.size(); j++)
	{

		auto l_pcAttr = m_apcColListAttr[j];
		auto l_pcCombobox = m_apcChoices[j];
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

void SP_DS_PedAnimation::LoadInitialMarking()
{
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	SP_DS_Node* l_pcNode;
	SP_DS_Attribute* l_pcAttr;

	if (m_lAllPlaceAnimators.size() <= 0)
	{
		return;
	}

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		l_pcNode = (*l_itPlace)->GetNode();
		l_pcAttr = l_pcNode->GetAttribute(wxT("Marking"));

		SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*> (l_pcAttr);
		long l_nLong = l_pcMarkAttr->GetValue(true);

		(*l_itPlace)->SetMarking(l_nLong);

		(*l_itPlace)->SetDefaultMarking(l_nLong);

		l_pcAttr->Update(true);

		Refresh();
	}
}

void SP_DS_PedAnimation::LoadCurrentMarking()
{
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	SP_DS_Node* l_pcNode;
	SP_DS_Attribute* l_pcAttr;

	if (m_lAllPlaceAnimators.size() <= 0)
	{
		return;
	}

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		l_pcNode = (*l_itPlace)->GetNode();
		l_pcAttr = l_pcNode->GetAttribute(wxT("Marking"));

		SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*> (l_pcAttr);
		long l_nLong = l_pcMarkAttr->GetValue(true);

		(*l_itPlace)->SetMarking(l_nLong);

		(*l_itPlace)->SetDefaultMarking(l_nLong);

		l_pcAttr->Update(true);

		Refresh();
	}
}

void SP_DS_PedAnimation::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_MarkingOverview* l_pcDlg = new SP_DLG_MarkingOverview(m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}
void SP_DS_PedAnimation::OnModifyConstantSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}


void SP_DS_PedAnimation::OnConstantSetsChanged(wxCommandEvent& p_cEvent)
{
	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{

		if (m_apcColListAttr[i])
		{
			m_apcColListAttr[i]->SetActiveList(m_apcChoices[i]->GetSelection());
		}
	}

	LoadCurrentMarking();
}

void SP_DS_PedAnimation::OnReset()
{
	list<SP_DS_PlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->ResetMarking();
		//by sl
		(*l_Iter)->MarkingSourceEdges();
		(*l_Iter)->MarkingTargetEdges();
	}

	m_llHistoryTransAnimators.clear();

	//by sl
	LoadCurrentMarking();
	Refresh();

	//Set the step count to zero
	m_nStepCount = 0;
	SetStepCounter();
}

void SP_DS_PedAnimation::OnSet(wxCommandEvent& p_cEvent)
{
	KeepMarking();
}

void SP_DS_PedAnimation::KeepMarking()
{
	list<SP_DS_PlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->SetMarking();
	}
	SP_Core::Instance()->GetRootDocument()->Modify(TRUE);

}

void SP_DS_PedAnimation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
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

void SP_DS_PedAnimation::OnExport(wxCommandEvent &p_pc_Event)
{
	Export *export_frame = new Export(wxT("Export Details"), this);

	export_frame->Show(true);
}

void SP_DS_PedAnimation::OnImport(wxCommandEvent &event)
{
	Import *import_frame = new Import(wxT("Import Details"), this);

	import_frame->Show(true);
}

void SP_DS_PedAnimation::ExportDetails(Export *export_frame)
{
	wxString l_start_str = export_frame->m_pc_StartText->GetLineText(0);
	wxString l_every_str = export_frame->m_pc_EveryText->GetLineText(0);
	wxString l_stop_str = export_frame->m_pc_StopText->GetLineText(0);
	wxString l_temp;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	list<SP_DS_TransAnimator*>::iterator l_itTrans;

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
			l_temp = wxT("Time,");

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

void SP_DS_PedAnimation::ExportMarkings()
{
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;

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

					if (++l_itPlace == m_lAllPlaceAnimators.end())
					{
						l_temp << l_n_token_value;
					}

					else
					{
						l_temp << l_n_token_value << wxT(",");
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

void SP_DS_PedAnimation::ExportStepSequences()
{
	list<SP_DS_TransAnimator*>::iterator l_itTrans;

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
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetNode();
					SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Name"));
					SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
					wxString l_sName = l_pcNameAttr->GetValue();

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

void SP_DS_PedAnimation::ImportDetails(Import *import_frame)
{
	list<SP_DS_TransAnimator *>::iterator l_itTrans;
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
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetNode();
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
						SP_DS_Node* l_pcNode = (*l_itTrans)->GetNode();
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

void SP_DS_PedAnimation::ImportStepSequences()
{
	if (m_bInvalid || m_ImportTextfile.Open(m_ImportFilename) == false)
	{
		m_lPossibleTransAnimators.clear();
		SP_MESSAGEBOX(wxT("Error in opening file ") + m_ImportFilename, wxT("Notification"), wxOK | wxICON_ERROR);
	}
	else
	{
		list<SP_DS_TransAnimator *>::iterator l_itTrans;
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
				SP_DS_Node* l_pcNode = (*l_itTrans)->GetNode();
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
					SP_DS_Node* l_pcNode = (*l_itTrans)->GetNode();
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

void SP_DS_PedAnimation::SetStepCounter()
{
	m_pcStepCounterValue->SetLabel(wxString::Format(wxT("%ld"), m_nStepCount));
}
