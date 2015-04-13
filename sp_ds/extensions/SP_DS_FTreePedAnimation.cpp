//////////////////////////////////////////////////////////////////////
// $Source: mf$
// $Author (modification): akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/06/12 11:55:00 $
// Short Description: ped animation of fault trees
//////////////////////////////////////////////////////////////////////
#include <wx/statline.h>

#include "sp_ds/extensions/SP_DS_FTreePedAnimation.h"
#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeGateAnimator.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_core/tools/SP_FTreeNodeColourizer.h"
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeRepairableSystemAnalyser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeIrreparableSystemAnalyser.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_gui/dialogs/SP_DLG_ChooseFTreeCutSet.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCutSetsCreater.h"
#include "snoopy.h"
#include "sp_defines.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_gui/management/SP_GM_Docmanager.h"

enum
{
	SP_SHOW_CUTSET,
};

BEGIN_EVENT_TABLE(SP_DS_FTreePedAnimation, SP_DS_Animation)
EVT_BUTTON(SP_SHOW_CUTSET, SP_DS_FTreePedAnimation::OnShow)

EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOUR, SP_DS_FTreePedAnimation::OnColourSim)
EVT_RADIOBOX(SP_ID_RADIOBOX_FTREE_ANALYSIS, SP_DS_FTreePedAnimation::OnKlick)
END_EVENT_TABLE()

SP_DS_FTreePedAnimation::SP_DS_FTreePedAnimation(unsigned int p_nRefresh,
		unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
	SP_DS_Animation(p_nRefresh, p_nDuration),
	m_nStepState((int)p_eStepping),
	m_SystemType(0),
	m_AnalysisType(0),
	m_ChooseCutSetDialog(NULL),
	m_pcSingleStep(NULL),
	ftAnalysisIrreparable(NULL),
	ftAnalysisRepairable(NULL),
	m_pcDialog(NULL),
	haveNoValues(FALSE),
	m_nodeColourizer(NULL)
{
	SetSystemAttributes();

}

SP_DS_FTreePedAnimation::~SP_DS_FTreePedAnimation()
{

	if (!m_nodeColourizer)
		return;
	try
	{
		m_nodeColourizer->RestoreColours();

	}
	catch (...)
	{
		m_sMessage << wxT("An error occurred closing the animation") << wxT("\n");

	}

	list<SP_DS_FTreeEventAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllNodeAnimators.begin(); l_Iter
			!= m_lAllNodeAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();
	Refresh();

}

SP_DS_Animation* SP_DS_FTreePedAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_FTreePedAnimation(m_nRefreshFrequ, m_nStepDuration, SP_ANIM_STEP_T(m_nStepState)));
}

bool SP_DS_FTreePedAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
	m_pcGraph = p_pcGraph;

	m_nodeColourizer = new SP_FTreeNodeColourizer(p_pcGraph);
	bool l_bReturn = SP_DS_Animation::Initialise(p_pcGraph);
	SP_ListAnimator::iterator l_Iter;
	SP_ListGraphic::iterator l_itGraphic;

	if (l_bReturn)
	{
		m_lAllNodeAnimators.clear();
		m_lAllGateAnimators.clear();

		for (l_Iter = m_lAllAnimators.begin(); l_Iter != m_lAllAnimators.end(); ++l_Iter)
		{
			SP_DS_Animator* l_pcAnimator = *l_Iter;
			if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_EVENT)
				m_lAllNodeAnimators.push_back(dynamic_cast<SP_DS_FTreeEventAnimator*>(l_pcAnimator));
			else if ((*l_Iter)->GetAnimatorType() == SP_DS_ANIMATOR_GATE)
				m_lAllGateAnimators.push_back(dynamic_cast<SP_DS_FTreeGateAnimator*>(l_pcAnimator));
		}
		//If the quantitative analysis was selected, the reliability parameters of the lowest level
		//are calculated
		if (m_AnalysisType == SP_FT_QUANTITATIVE)
		{

			list<SP_DS_FTreeEventAnimator*>::iterator l_itNodes;
			for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
					!= m_lAllNodeAnimators.end(); ++l_itNodes)
			{
				if ((*l_itNodes)->GetNode()->GetClassName() == wxT("Basic Event") ||
					(*l_itNodes)->GetNode()->GetClassName() == wxT("Undeveloped Event"))
				{
					if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
					{
						ftAnalysisRepairable
								= new SP_DS_FTreeRepairableSystemAnalyser();
						ftAnalysisRepairable->DoAnalyseRepairableSystem((*l_itNodes)->GetNode());
					}
					else
					{
						ftAnalysisIrreparable
								= new SP_DS_FTreeIrreparableSystemAnalyser();
						ftAnalysisIrreparable->DoAnalyseIrreparableSystem((*l_itNodes)->GetNode());
					}
				}
			}
		}
		else
		{
			//If the qualitative analysis is selected, results of the quantitative analysis are not shown.
			//The gates are presented at beginning of the animation coloredly. The cut sets are now determined.
			list<SP_DS_FTreeEventAnimator*>::iterator l_itNodes;
			for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
					!= m_lAllNodeAnimators.end(); ++l_itNodes)
			{
				if ((*l_itNodes)->GetNode()->GetClassName()== wxT("Basic Event") ||
					(*l_itNodes)->GetNode()->GetClassName()== wxT("Undeveloped Event") ||
					(*l_itNodes)->GetNode()->GetClassName()== wxT("Intermediate Event") ||
					(*l_itNodes)->GetNode()->GetClassName()== wxT("Top Event"))
				{
					(*l_itNodes)->GetNode()->GetAttribute(wxT("Probability of Failure (%)"))->SetShow(FALSE);
				}
			}
			list<SP_DS_FTreeGateAnimator*>::iterator l_itGates;
			for (l_itGates = m_lAllGateAnimators.begin(); l_itGates
					!= m_lAllGateAnimators.end(); ++l_itGates)
			{
				SP_DS_Node* gate = (*l_itGates)->GetNode();
				m_nodeColourizer->ColourNode(gate, *wxBLUE);
				(*l_itGates)->SetNodeColourizer(m_nodeColourizer);
			}
		}
		//select all cut sets of the fault tree
		SP_DS_Nodeclass* nodeClass= p_pcGraph->GetNodeclass(wxT("Top Event"));
		if(!nodeClass->GetElements()->empty())
		{
			const SP_ListNode* n_pcAllNodes = nodeClass->GetElements();
			SP_ListNode::const_iterator topIt = n_pcAllNodes->begin();
			SP_DS_FTreeLogicalExpressionReader* readExp =
					new SP_DS_FTreeLogicalExpressionReader();
			logExpression = readExp->FindLogicalExpression((*topIt));
			vMap = readExp->DoDeMorgan();

			pDNF = new SP_DS_FTreeCutSetsCreater(p_pcGraph);
		}
	}

	return l_bReturn;
}

bool SP_DS_FTreePedAnimation::PreStep()
{
	list<SP_DS_FTreeEventAnimator*>::iterator l_itNodes;
	//If the quantitative analysis is selected, all reliability parameters of each event
	//in the fault tree is calculated.
	if (m_AnalysisType == SP_FT_QUANTITATIVE)
	{

		for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
				!= m_lAllNodeAnimators.end(); ++l_itNodes)
		{
			if ((*l_itNodes)->GetNode()->GetClassName() == wxT("Basic Event") ||
				(*l_itNodes)->GetNode()->GetClassName() == wxT("Undeveloped Event"))
			{

				SP_DS_ListAttribute* l_pcListAttr =
						dynamic_cast<SP_DS_ListAttribute*> ((*l_itNodes)->GetNode()->GetAttribute(wxT("Failure Rate")));

				SP_DS_DoubleListAttribute* l_pcFAttr =
						dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);

				double m_nValue = l_pcFAttr->GetValueDouble();
				if (m_nValue == 0)
				{
					SP_MESSAGEBOX(wxT("Check the values of failure rates!"),
							wxT("Notification"), wxOK | wxICON_INFORMATION);
					return FALSE;
				}
			}
			if ((*l_itNodes)->GetNode()->GetClassName()== wxT("Basic Event") ||
				(*l_itNodes)->GetNode()->GetClassName()== wxT("Undeveloped Event") ||
				(*l_itNodes)->GetNode()->GetClassName()== wxT("Intermediate Event") ||
				(*l_itNodes)->GetNode()->GetClassName()== wxT("Top Event"))
			{
				if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
					ftAnalysisRepairable->DoAnalyseRepairableSystem((*l_itNodes)->GetNode());
				else
					ftAnalysisIrreparable->DoAnalyseIrreparableSystem((*l_itNodes)->GetNode());
			}
		}
		RefreshFrame();
		return FALSE;
	}
	else
	{
		//Combinations of gates which represent a NAND- or NOR-function
		//cannot be still represented correctly. It has to be checked whether
		//these combinations still exist in the faul tree.
		if (IsAnimationExecutable())
		{
			//If the qualitative analysis is selected, the animation of the fault tree begins now.
			list<SP_DS_FTreeGateAnimator*>::iterator l_itGates;
			SP_ListGraphic::iterator l_itGraphic;
			unsigned int l_nSteps = m_nSteps / 2;

			m_lPossibleGateAnimators.clear();
			//selecting gates (candidates) to fire ...
			if (m_pcSingleStep)
			{
				//...for steps that are triggered by clicking directly on a gate
				m_lPossibleGateAnimators.push_back(m_pcSingleStep);
			}
			else
			{

				//...for stepping forward and random backsteps
				list<SP_DS_FTreeGateAnimator*>::iterator l_itNeg;
				list<SP_DS_FTreeGateAnimator*> negList;
				for (l_itGates = m_lAllGateAnimators.begin(); l_itGates
						!= m_lAllGateAnimators.end(); ++l_itGates)
					m_lPossibleGateAnimators.push_back(*l_itGates);
			}

			//running the usual tests and preparing everything
			for (l_itGates = m_lPossibleGateAnimators.begin(); l_itGates
					!= m_lPossibleGateAnimators.end(); ++l_itGates)
				(*l_itGates)->InformPreNodes();

			for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
					!= m_lAllNodeAnimators.end(); ++l_itNodes)
				(*l_itNodes)->TestMarking();

			for (l_itGates = m_lPossibleGateAnimators.begin(); l_itGates
					!= m_lPossibleGateAnimators.end(); ++l_itGates)
				(*l_itGates)->TestConcession();

			for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
					!= m_lAllNodeAnimators.end(); ++l_itNodes)
				(*l_itNodes)->SetEnable();

			//selecting the gate that are actually able to fire
			for (l_itGates = m_lPossibleGateAnimators.begin(); l_itGates
					!= m_lPossibleGateAnimators.end(); ++l_itGates)
			{
				if ((*l_itGates)->PreStep(l_nSteps, GetDirection()))
				{
					m_lStepGateAnimators.push_back((*l_itGates));
				}
			}

			// all gates with concession are resolved, now test the stepping option
			ReduceGates();

			//adding the corresponding nodes which take part in the process
			for (l_itNodes = m_lAllNodeAnimators.begin(); l_itNodes
					!= m_lAllNodeAnimators.end(); ++l_itNodes)
			{
				if ((*l_itNodes)->PreStep(l_nSteps, GetDirection()))
				{
					m_lStepNodeAnimators.push_back((*l_itNodes));
				}
			}

			//start at the beginning
			m_nActStep++;

			bool l_bReturn = (m_lStepNodeAnimators.size()
					+ m_lStepGateAnimators.size() > 0);

			if (!l_bReturn)
			{
				m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
				wxString msg;

				if (m_bOneShot)
				{
					if (GetSingleStep())
					{
						msg
								= wxT("The requirements to switch this gate are not given!");
					}
					else
					{
						msg =wxT("There are no enabled gates to switch.");
					}
				}
				else
				{
					msg =wxT("There are no more enabled gates!");
				}
				SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION);

				if (m_pcDialog)
				{
					m_pcDialog->ResetPlayButtons();
				}
			}
			return l_bReturn;
		}
		else
		{
			SP_MESSAGEBOX(
					wxString::Format(wxT("In the fault tree, combinations are contained by gates\n")
						wxT("which perform NAND- or NOR-function. These cannot be represented correctly. \n")
						wxT("Please, apply first - Use deMorgan - (not possible for Extended Fault Trees). \n")
						wxT("Be careful! Intermediate Events are deleted.")),
					wxT("Application of de Morgan"),
					wxICON_INFORMATION);
			return FALSE;
		}
	}
	return TRUE;
}

bool SP_DS_FTreePedAnimation::Step()
{
	bool l_bReturn= TRUE;
	list<SP_DS_FTreeGateAnimator*>::iterator l_itGates;
	list<SP_DS_FTreeEventAnimator*>::iterator l_itNodes;

	if (m_nActStep < (m_nSteps / 2))
	{
		for (l_itNodes = m_lStepNodeAnimators.begin(); l_itNodes
				!= m_lStepNodeAnimators.end(); ++l_itNodes)
			l_bReturn &= (*l_itNodes)->Step(m_nActStep);
	}
	if (m_nActStep == (m_nSteps / 2))
	{
		for (l_itNodes = m_lStepNodeAnimators.begin(); l_itNodes
				!= m_lStepNodeAnimators.end(); ++l_itNodes)
			l_bReturn &= (*l_itNodes)->PostStep();
		m_lStepNodeAnimators.clear();
	}
	if (m_nActStep > (m_nSteps / 2))
	{
		for (l_itGates = m_lStepGateAnimators.begin(); l_itGates
				!= m_lStepGateAnimators.end(); ++l_itGates)
		{

			l_bReturn &= (*l_itGates)->Step(m_nActStep);
		}
	}

	m_nActStep++;

	return l_bReturn;
}

bool SP_DS_FTreePedAnimation::PostStep()
{
	bool l_bReturn= TRUE;
	list<SP_DS_FTreeGateAnimator*>::iterator l_itGates;
	list<SP_DS_FTreeEventAnimator*>::iterator l_itNodes;

	for (l_itNodes = m_lStepNodeAnimators.begin(); l_itNodes
			!= m_lStepNodeAnimators.end(); ++l_itNodes)
		l_bReturn &= (*l_itNodes)->PostStep();
	for (l_itGates = m_lStepGateAnimators.begin(); l_itGates
			!= m_lStepGateAnimators.end(); ++l_itGates)
		l_bReturn &= (*l_itGates)->PostStep();

	m_lStepNodeAnimators.clear();
	m_lStepGateAnimators.clear();
	m_nActStep = 0;
	m_pcSingleStep = NULL;
	Refresh();

	return l_bReturn;
}

bool SP_DS_FTreePedAnimation::ReduceGates()
{
	if (m_pcSingleStep)
		return TRUE;

	SP_ListGraphic::iterator l_itGraphic;
	list<SP_DS_FTreeGateAnimator*>::iterator l_itGates;
	SP_DS_FTreeGateAnimator* l_pcAnim = NULL;
	if (m_nStepState == SP_ANIM_STEP_SINGLE)
	{
		if (m_lStepGateAnimators.size() > 0)
		{
			int l_nChose = SP_RandomLong(m_lStepGateAnimators.size());
			int i = 0;
			for(l_itGates = m_lStepGateAnimators.begin(); l_itGates != m_lStepGateAnimators.end(); ++l_itGates)
				if (l_nChose == i++)
					l_pcAnim = (*l_itGates);
				else
					(*l_itGates)->Reset();

			m_lStepGateAnimators.clear();
			m_lStepGateAnimators.push_back(l_pcAnim);
		}
	}
	else if (m_nStepState == SP_ANIM_STEP_INTERMEDIATE)
	{
		// two iterations, first choose one random entry, second choose a random set of the rest
		SP_LOGDEBUG(wxString::Format(wxT("have %d"), m_lStepGateAnimators.size()));

		list<SP_DS_FTreeGateAnimator*> l_lNewGates;
		//first round choose one transition randomly
		if (m_lStepGateAnimators.size() > 0)
		{
			int l_nChose = SP_RandomLong(m_lStepGateAnimators.size());
			int i = 0;
			for(l_itGates = m_lStepGateAnimators.begin(); l_itGates != m_lStepGateAnimators.end(); ++l_itGates)
			{
				if (l_nChose == i++)
				{
					l_lNewGates.push_back(*l_itGates);
					m_lStepGateAnimators.erase(l_itGates);
					break;
				}
			}
		}
		//second round for the other transitions to choose a random set out of them
		for(l_itGates = m_lStepGateAnimators.begin(); l_itGates != m_lStepGateAnimators.end(); l_itGates = m_lStepGateAnimators.begin())
		{
			if (SP_RandomLong(2) == 1)
			{
				(*l_itGates)->Reset();
			}
			else
			{
				l_lNewGates.push_back((*l_itGates));
			}
			m_lStepGateAnimators.erase(l_itGates);
		}

		m_lStepGateAnimators.clear();
		for (l_itGates = l_lNewGates.begin(); l_itGates != l_lNewGates.end(); ++l_itGates)
		{
			m_lStepGateAnimators.push_back((*l_itGates));
		}
		SP_LOGDEBUG(wxString::Format(wxT("chose %d"), m_lStepGateAnimators.size()));
	}

	return TRUE;
}

bool SP_DS_FTreePedAnimation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg,
		wxSizer* p_pcSizer)
{
	if (!SP_DS_Animation::AddToDialog(p_pcDlg, p_pcSizer))
		return FALSE;

	wxString l_asAnalysisChoices[] =
	{ wxT("qualitative"), wxT(" quantitative") };
	wxString l_asTypes[] =
	{ wxT("repairable"), wxT("irreparable") };

	wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(p_pcDlg, wxID_ANY,
			_("FaultTree - Analysis"));

	wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
	p_pcSizer->Add(itemStaticBoxSizer3, 1, wxGROW|wxALL, 5);

	wxBoxSizer* l_pcSizerAnalysis = new wxBoxSizer(wxVERTICAL);

	m_pcAnalysisChoice = new wxRadioBox(
			p_pcDlg,
			SP_ID_RADIOBOX_FTREE_ANALYSIS,
			wxT("Type"),
			wxDefaultPosition,
			wxDefaultSize,
			2,
			l_asAnalysisChoices,
			1,
			wxRA_SPECIFY_ROWS);

	m_pcAnalysisChoice->SetSelection(m_AnalysisType);

	itemStaticBoxSizer3->Add(m_pcAnalysisChoice, 0, wxEXPAND | wxALL, 5);

	m_showCutSets = new wxButton(p_pcDlg, SP_SHOW_CUTSET, wxT("Cut Sets"));
	if (m_pcAnalysisChoice->GetSelection() == SP_FT_QUALITATIVE)
		m_showCutSets->Enable(TRUE);
	else
		m_showCutSets->Enable(FALSE);
	itemStaticBoxSizer3->Add(m_showCutSets, 1, wxALL | wxEXPAND, 5);

	m_pcSystemType = new wxRadioBox(
			p_pcDlg,
			-1,
			wxT("System"),
			wxDefaultPosition,
			wxDefaultSize,
			2,
			l_asTypes,
			1,
			wxRA_SPECIFY_COLS);

	m_pcSystemType->SetSelection(m_SystemType);

	if (m_pcAnalysisChoice->GetSelection() == SP_FT_QUANTITATIVE)
	{
		m_pcSystemType->Enable(true);
	}
	else
	{
		m_pcSystemType->Enable(false);
	}

	l_pcSizerAnalysis->Add(m_pcSystemType, 0, wxEXPAND, 5);

	itemStaticBoxSizer3->Add(l_pcSizerAnalysis, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* l_pcSizerColourSim = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizerColourSim->Add(new wxStaticText(p_pcDlg, -1, wxT("Colour: ")), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

	m_pcButtonColourSim = new wxColourPickerCtrl(p_pcDlg, SP_ID_BUTTON_COLOUR, *wxBLUE,
			wxDefaultPosition, wxDefaultSize,
			wxCLRP_DEFAULT_STYLE);
	RefreshColourSim();

	l_pcSizerColourSim->Add(m_pcButtonColourSim, 0, wxEXPAND | wxALL, 5);
	itemStaticBoxSizer3->Add(l_pcSizerColourSim, 0, wxEXPAND | wxALL, 5);

	p_pcDlg->PushEventHandler(this);

	return TRUE;
}

bool SP_DS_FTreePedAnimation::OnDialogOk()
{
	list<SP_DS_FTreeEventAnimator*>::iterator l_Iter;
	m_AnalysisType = m_pcAnalysisChoice->GetSelection();
	//If the quantitative analysis is selected, all calculated parameters
	//of each event in the fault tree and the token in each node are put back.
	//Afterwards the reliability parameters are calculated for the events in
	//the lowest level.
	if (m_AnalysisType == SP_FT_QUANTITATIVE)
	{
		m_SystemType = m_pcSystemType->GetSelection();

		for (l_Iter = m_lAllNodeAnimators.begin(); l_Iter
				!= m_lAllNodeAnimators.end(); ++l_Iter)
		{
			if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
				ftAnalysisRepairable->ResetValues((*l_Iter)->GetNode());
			else
				ftAnalysisIrreparable->ResetValues((*l_Iter)->GetNode());
			if ((*l_Iter)->GetNode()->GetClassName()== wxT("Basic Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Undeveloped Event"))
			{
				if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
				{
					if (!ftAnalysisRepairable)
						ftAnalysisRepairable
								= new SP_DS_FTreeRepairableSystemAnalyser();
					ftAnalysisRepairable->DoAnalyseRepairableSystem((*l_Iter)->GetNode());
				}
				else
				{
					if (!ftAnalysisIrreparable)
						ftAnalysisIrreparable
								= new SP_DS_FTreeIrreparableSystemAnalyser();
					ftAnalysisIrreparable->DoAnalyseIrreparableSystem((*l_Iter)->GetNode());
				}
			}
			(*l_Iter)->ResetMarking();
		}

		m_nodeColourizer->RestoreColours();

	}
	else
	{
		//If the qualitative analysis is selected, the calculated probability of failure of
		//each event is not shown.
		for (l_Iter = m_lAllNodeAnimators.begin(); l_Iter
				!= m_lAllNodeAnimators.end(); ++l_Iter)
			if ((*l_Iter)->GetNode()->GetClassName()== wxT("Basic Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Undeveloped Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Intermediate Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Top Event"))
			{
				(*l_Iter)->GetNode()->GetAttribute(wxT("Probability of Failure (%)"))->SetShow(FALSE);
			}
	}

	RefreshFrame();
	return SP_DS_Animation::OnDialogOk();
}

void SP_DS_FTreePedAnimation::OnKlick(wxCommandEvent& p_cEvent)
{

	if (m_pcAnalysisChoice->GetSelection() == SP_FT_QUANTITATIVE)
	{
		m_pcSystemType->Enable(true);
		m_showCutSets->Enable(false);
	}
	else
	{
		m_pcSystemType->Enable(false);
		m_showCutSets->Enable(true);
	}
}

void SP_DS_FTreePedAnimation::RefreshColourSim()
{

	m_pcButtonColourSim->Refresh();

	SP_ListNode* nList = new SP_ListNode();
	SP_ListNode* nodeList = m_nodeColourizer->GetNodeList();
	SP_ListNode::const_iterator sIt;

	for (sIt = nodeList->begin(); sIt != nodeList->end(); ++sIt)
	{
		SP_DS_Node* newNode = (*sIt);
		nList->push_back(dynamic_cast<SP_DS_Node*>(newNode));
	}
	m_nodeColourizer->RestoreColours();

	m_nodeColourizer->ColourNodes(nList, m_pcButtonColourSim->GetColour());
}

void SP_DS_FTreePedAnimation::OnColourSim(wxColourPickerEvent& p_cEvent)
{
	RefreshColourSim();
}

bool SP_DS_FTreePedAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl,
		wxSizer* p_pcSizer)
{

	return TRUE;
}

void SP_DS_FTreePedAnimation::OnShow(wxCommandEvent& p_cEvent)
{
	if (!m_ChooseCutSetDialog)
		m_ChooseCutSetDialog = new SP_DLG_ChooseFTreeCutSet(m_pcGraph, m_pcDialog, pDNF->GetCutSets());

	m_ChooseCutSetDialog->ShowModal();

}

void SP_DS_FTreePedAnimation::OnReset()
{
	list<SP_DS_FTreeEventAnimator*>::iterator l_Iter;
	list<SP_DS_FTreeGateAnimator*>::iterator g_Iter;
	for (l_Iter = m_lAllNodeAnimators.begin(); l_Iter
			!= m_lAllNodeAnimators.end(); ++l_Iter)
	{

		if (m_AnalysisType == SP_FT_QUANTITATIVE)
		{
			//puts back all calculated reliability parameters and calculates the reliability
			//parameters for the nodes in the lowest level again
			if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
				ftAnalysisRepairable->ResetValues((*l_Iter)->GetNode());
			else
				ftAnalysisIrreparable->ResetValues((*l_Iter)->GetNode());

			if ((*l_Iter)->GetNode()->GetClassName()== wxT("Basic Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Undeveloped Event"))
			{
				if (m_SystemType == SP_FT_SYSTEM_REPAIRABLE)
					ftAnalysisRepairable->DoAnalyseRepairableSystem((*l_Iter)->GetNode());
				else
					ftAnalysisIrreparable->DoAnalyseIrreparableSystem((*l_Iter)->GetNode());
			}
		}
		else
		{
			//set the visibility from the value of the probability of failure on false
			//and puts back the tokens of each node.
			if ((*l_Iter)->GetNode()->GetClassName() == wxT("Basic Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Undeveloped Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Intermediate Event") ||
				(*l_Iter)->GetNode()->GetClassName()== wxT("Top Event"))
			{
				(*l_Iter)->GetNode()->GetAttribute(wxT("Probability of Failure (%)"))->SetShow(FALSE);
			}
			(*l_Iter)->ResetMarking();
		}
	}

	for (g_Iter = m_lAllGateAnimators.begin(); g_Iter
			!= m_lAllGateAnimators.end(); ++g_Iter)
		m_nodeColourizer->ColourNode((*g_Iter)->GetNode(), m_pcButtonColourSim->GetColour());

	RefreshFrame();
}

void SP_DS_FTreePedAnimation::SetSystemAttributes()
{
	wxString m_sNetClass = SP_Core::Instance()->GetRootDocument()->GetNetclassName();
	m_AnalysisType = wxGetApp().GetAnimationPrefs()->GetAnalysisType(m_sNetClass);
	m_SystemType = wxGetApp().GetAnimationPrefs()->GetSystemType(m_sNetClass);
}

void SP_DS_FTreePedAnimation::RefreshFrame()
{
	//Refresh the Frames
	SP_Core::Instance()->GetRootDocument()->Refresh();
}

bool SP_DS_FTreePedAnimation::IsAnimationExecutable()
{
	SP_DS_Nodeclass* negNodeclass = m_pcGraph->GetNodeclass(wxT("NEG"));

	const SP_ListNode* l_pcAllNodes = negNodeclass->GetElements();

	if (l_pcAllNodes->size() == 0)
		return TRUE;
	else
	{
		SP_ListNode::const_iterator nIt;
		for (nIt = l_pcAllNodes->begin(); nIt != l_pcAllNodes->end(); ++nIt)
		{
			const SP_ListEdge* trgEdges = (*nIt)->GetTargetEdges();
			SP_ListEdge::const_iterator eIt;
			for (eIt = trgEdges->begin(); eIt != trgEdges->end(); ++eIt)
			{
				SP_DS_Node* sourceNode = (SP_DS_Node*) (*eIt)->GetSource();
				if (sourceNode->GetClassName() != wxT("Basic Event") &&
					sourceNode->GetClassName() != wxT("Undeveloped Event") &&
					sourceNode->GetClassName() != wxT("NEG") &&
					sourceNode->GetClassName() != wxT("Intermediate Event"))
					return FALSE;
			}
		}
	}
	return TRUE;
}
