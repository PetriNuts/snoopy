//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// @Modified : George Assaf 01/02/2020: Constant grouping + high-level simulation
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
#include <chrono>
#include <iomanip> 


 
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
	SP_ID_PEDIMPORT,//by george  10.2020
	SP_ID_SIMMDE,
	SP_ID_COLLAPSEPANEL_PROPERTY_SIZER,
	SP_ID_BUTTON_START_COLSIM,
	//SP_ID_BUTTON_ExportSIMTRACES_COLSIM,
	SP_ID_BUTTON_CHOOSE_PLACES,
	SP_ID_LISTBOX_VIEWS
};

BEGIN_EVENT_TABLE(SP_DS_ColStAnimation, SP_DS_Animation)
EVT_BUTTON(SP_ID_PEDSET, SP_DS_ColStAnimation::OnSet)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_ColStAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_ColStAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_SIMMDE, SP_DS_ColStAnimation::OnUpdateUI)

EVT_CHECKBOX(SP_ID_SIMMDE, SP_DS_ColStAnimation::OnSimMode)

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
EVT_BUTTON(SP_ID_BUTTON_START_COLSIM, SP_DS_ColStAnimation::OnSimulationStart) 
//EVT_BUTTON(SP_ID_BUTTON_ExportSIMTRACES_COLSIM, SP_DS_ColStAnimation::OnExportSimTraces)
EVT_BUTTON(SP_ID_BUTTON_CHOOSE_PLACES, SP_DS_ColStAnimation::OnChoosePlaces)
EVT_LISTBOX_DCLICK(SP_ID_LISTBOX_VIEWS, SP_DS_ColStAnimation::OnOpenSelectedGraphViews)

END_EVENT_TABLE()

SP_DS_ColStAnimation::SP_DS_ColStAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping, bool p_bColSimMode) :
	SP_DS_Animation(p_nRefresh, p_nDuration, p_bColSimMode),
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
    m_nColoringGroupCurrentSelectedValue = 0;
	m_nIntervalStart = 0.0;
	m_nIntervalEnd = 100;
	m_nNumberofPoints = 50;

	m_nIntervalSize = (m_nIntervalEnd - m_nIntervalStart) / (m_nNumberofPoints-1);
	m_dCurrentTime = m_nIntervalStart;
	m_bStopSimulation = true;
	
	m_bColSimMode = false;
	m_bInPreStep = false;
 
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
			//wxDELETE((m_cbKeep));
		}

		if (l_bIsKeep)
		{
		UpdateMarkingPlaces();
		Refresh();

 
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

						int l_nBegin = l_nGridRowNumber;
						wxString l_sMainMarking;

						l_vPlaceNodes[l_nPos]->Update(TRUE);


						SP_CPN_SyntaxChecking l_cSyntaxChecking;
						if (!l_cSyntaxChecking.Initialize())
							continue;

					l_sMainMarking = wxT("");
					map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
					if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false,true))
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


					SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast<SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
					if (l_pcMarkingAttr)
						l_pcMarkingAttr->SetValueString(l_sMainMarking);

					l_vPlaceNodes[l_nPos]->Update(TRUE);
					}


				  SP_Core::Instance()->GetRootDocument()->Modify(true);

				wxDELETE(m_pcUnfolding);
				}

 
 	    Refresh();

}

void SP_DS_ColStAnimation::LaodIntialNetState() {


	LoadDefaultConstantsGroups();//by george: in case a conatnt used as number of token, 
	//so we need to reset intial constant groups

	for (auto it : m_mpcTransitionAnimators) {
		it.second->Reset();
	}

	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;

	//reset marking places
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

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		//(*l_Iter) = nullptr;


	int l_nGridRowNumber = 0;
	for (unsigned int l_nPos = 0; l_nPos < l_vPlaceNodes.size(); l_nPos++)
	{
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(SP_DS_CPN_MARKINGLIST));

		//int l_nBegin = l_nGridRowNumber;
		wxString l_sMainMarking;

		l_vPlaceNodes[l_nPos]->Update(TRUE);


		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.Initialize())
			continue;

		l_sMainMarking = wxT("");
		map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_vPlaceNodes[l_nPos], l_mColorToMarkingMap, false, true))
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


		SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast<SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
		if (l_pcMarkingAttr)
			l_pcMarkingAttr->SetValueString(l_sMainMarking);

		l_vPlaceNodes[l_nPos]->Update(TRUE);
	}

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if (!l_cSyntaxChecking.Initialize())
		return;

	SP_Core::Instance()->GetRootDocument()->Modify(true);

	//Initialise(m_pcGraph);
	//StopTimer();
	//Initialise(m_pcGraph);
	//StartTimer();
	Refresh();



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
SP_DS_ColStAnimation::Clone()
{
	wxString l_sOptions = SP_Core::Instance()->GetActivatedRefreshDurationanim();
		if (l_sOptions != wxT(""))
		{
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
			return dynamic_cast<SP_DS_Animation*>(new SP_DS_ColStAnimation(l_nRefres, l_nDuration, SP_ANIM_STEP_T(m_nStepState)));
		}

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

void SP_DS_ColStAnimation::DoColSimulation()
{

}

double SP_DS_ColStAnimation::GenerateRandomVariableExpDistr(double p_nLambda)
{//determine next time point
	if (p_nLambda == 0)
	{
		wxString l_Msg = wxT("dead state detected at t=");
		//l_Msg << m_nCurrentTime;
		SP_LOGMESSAGE(l_Msg);
	}
	double l_nRandom = SP_RandomDouble();

	return -1 * log(l_nRandom) / p_nLambda;
}

double SP_DS_ColStAnimation::CalculateTotalHazard() {

	double l_dTotal = 0.0; 

	for (long i = 0; i < m_mpcTransitionAnimators.size(); ++i) {

		if (m_bStopSimulation) return -1;
		
		double l_dHaz = ComputeFunctionHazard(i);

		l_dTotal += l_dHaz;
	}
	
	return l_dTotal;

}

void SP_DS_ColStAnimation::SetSeed(unsigned long ulSeed)
{
	m_Generator.seed(ulSeed);
	if (m_pcRandGen) {
		m_pcRandGen->seed(ulSeed);
	}
	m_ulSeed = ulSeed;

}


double SP_DS_ColStAnimation::GenerateRandomVariableExpDistrBySeed(double p_nLambda)
{

	double l_nRandom = m_pcRandGen->randDblExc();//m_pcRandGen->randExc();
	wxString l_sRN; l_sRN<<"lambda Exp: " << p_nLambda;

	//SP_LOGMESSAGE(l_sRN);
	//m_ExportRandom.AddLine(l_sRN);

	return -1 * log(l_nRandom) / p_nLambda;
}

long SP_DS_ColStAnimation::GenerateRandomVariableDiscreteBySeed(double p_nSum)
{

	m_anHazardValues.clear();
	for (const auto& pair : m_mInstance2HazardValue) {
		
		double value = pair.second;
		m_anHazardValues.push_back(value);
	}
	
	 
	double r = m_pcRandGen->randDblExc();

	for (unsigned n = 0; n < 2; ++n)
	{
		double l_nU = p_nSum * r;
		SP_VectorDouble::iterator rit = m_RowSum.begin();
		SP_VectorDouble::iterator rbegin = m_RowSum.begin();
		SP_VectorDouble::iterator rend = m_RowSum.end();
		unsigned i = 0;
		for (; rit != rend && (l_nU -= *rit) > 0; ++rit, ++i)
		{
		}
		if (rit == rend)
		{
			--i;
			--rit;
		}
		l_nU += *rit;

		SP_VectorDouble::iterator it = m_anHazardValues.begin()
			+  (i * m_RowLength);// (i*m_anHazardValues.size());
		SP_VectorDouble::iterator begin = m_anHazardValues.begin();
		SP_VectorDouble::iterator end = m_anHazardValues.end();
		for (; it != end && (l_nU -= *it) > 0; ++it)
		{
		}
		if (l_nU <= 0)
		{
			return std::distance(begin, it) - (it == end);
		}
		else

		{
			m_nCombinedHazardValue = p_nSum = 0.0;
			std::fill(m_RowSum.begin(), m_RowSum.end(), 0.0);
			rit = rbegin;
			i = 0;
			for (it = begin; it != end; ++it)
			{
				p_nSum += *it;
				*rit += *it;
				++i;
				if (i == m_RowLength)
				{
					++rit;
					i = 0;
				}
			}
			m_nCombinedHazardValue = p_nSum;
		}
	}
	
	return -1;
	 
}


long SP_DS_ColStAnimation::GenerateRandomVariableDiscrete(double p_nSum)
{
	double r = m_pcRandGen->randDblExc();
	while (true)
	{
		double l_nU = p_nSum *  SP_RandomDouble();
		double l_nRunningSum = 0;
		SP_VectorDouble::const_iterator l_it;
		long l_nTransitionPos = 0;
		for (l_it = m_anHazardValues.begin(); l_it != m_anHazardValues.end(); ++l_it)
		{
			l_nRunningSum += *l_it;
			if (l_nRunningSum >= l_nU)
			{
				return l_nTransitionPos;
			}
			++l_nTransitionPos;
		}
	}
	return -1;
}

wxString SP_DS_ColStAnimation::GetColorTransitionName(long p_nPos)
{
	if (p_nPos >= m_mpcTransitionAnimators.size()) return wxT("");

	SP_DS_ColStTransAnimator* l_pcNodeTrans = m_mpcTransitionAnimators[p_nPos];

	
	if (!l_pcNodeTrans) return wxT("");

	SP_DS_Node* l_pcNode = l_pcNodeTrans->GetParentNode();


	if (!l_pcNode) return wxT("");

	SP_DS_Attribute* l_pcAttr = (l_pcNode)->GetAttribute(wxT("Name"));
	SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
	
	return l_pcNameAttr->GetValueString();


}


bool SP_DS_ColStAnimation::SubstituteRateFunction(const wxString& p_sRate, const wxString& colPlace, const wxString& p_sChosenBinding, wxString& substituted)
{
	SP_MapString2String l_mcolPlace2ColorMap;
	ExtractPlaceIntanceMarking(p_sRate, l_mcolPlace2ColorMap);
	substituted = p_sRate;
	for (auto it = l_mcolPlace2ColorMap.begin(); it != l_mcolPlace2ColorMap.end(); ++it)
	{
		wxString l_sMarking = ExtractInstanceMarking(it->first, it->second);
		
		wxString l_sToBeSubstituted = it->first + wxT("_") + it->second;//instnce
		substituted.Replace(l_sToBeSubstituted, l_sMarking);
	}

	return true;

}

void  SP_DS_ColStAnimation::ExtractPlaceIntanceMarking(const wxString& p_sRate, SP_MapString2String& l_mColorPlace2Color)
{
	 //buggy method
	wxStringTokenizer tokenizer(p_sRate, " +-*/=^()");

	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();

		for (const auto& place : m_vColPlaceNamesModified)
		{
			std::string l_scolor = ExtractSubstring(token, place);

			if (l_scolor != "")
			{
				l_mColorPlace2Color[place.BeforeFirst(wxChar('_'))] = l_scolor;
				break;//token found
			}
		 
		}
	 
	}

}

std::string SP_DS_ColStAnimation::ExtractSubstring(const std::string& lengthyString, const std::string& pattern) {
	/**
	size_t pos = lengthyString.find(pattern);
	if (pos != std::string::npos) {
		return lengthyString.substr(pos + pattern.length());
	}
	*/
	wxString str1 = lengthyString;
	wxString str2 = pattern;
	if (!str1.Contains(str2)) return "";
	wxString result = str1;

	result.Replace(str2, "");
	/**
	for (wxString::const_iterator it = str2.begin(); it != str2.end(); ++it)
	{
		wxString character = *it;
		result.Replace(character, wxEmptyString);
	}
	*/
	return result.ToStdString();
	//return ""; // Pattern not found
}

wxString  SP_DS_ColStAnimation::ExtractInstanceMarking(const wxString& p_sColPlaceId, const wxString& p_color)
{

	long l_ntoken_value = 0;

	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;

	for (l_itPlace = (m_lAllPlaceAnimators).begin(); l_itPlace != (m_lAllPlaceAnimators).end(); ++l_itPlace)
	{
		//ColPlace2InstancesStates currentStateCol2InstancesMap;

		l_ntoken_value = (*l_itPlace)->GetMarking();

		SP_DS_Node*  itNode = (*l_itPlace)->GetNode();

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(itNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		if (l_sPlaceName != p_sColPlaceId) continue;

		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(itNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcNameAttibute)
			continue;
		wxString l_sColorSetName = l_pcNameAttibute->GetValue();

		SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(l_sColorSetName);
		if (!l_pcColorSet)
		{
			wxString l_sError = wxT("Place: ") + l_sPlaceName + wxT(": ") + l_sColorSetName + wxT(" is not right.");
			SP_LOGERROR(l_sError);
			return wxT("");
		}

		SP_CPN_ColorSet p_cColorSet = *l_pcColorSet;
		vector<wxString> l_ColorVector = p_cColorSet.GetStringValue();
		vector<wxString> l_ActualColorVector;
		bool l_bIsAll = false;
	//	currentStateCol2InstancesMap.colPlace = l_sPlaceName;

		SP_DS_ColListAttribute* l_pcCollis = dynamic_cast<SP_DS_ColListAttribute*>((itNode)->GetAttribute(SP_DS_CPN_MARKINGLIST));
		wxString l_sMulti;
		if (l_pcCollis)
		{
			SP_MapString2String instances;
			for (unsigned int i = 0; i < l_pcCollis->GetRowCount(); i++)
			{
				wxString color = l_pcCollis->GetCell(i, 0);
				wxString instance_id;
				wxString marking;

				if (color != wxT("all()"))
				{
					if(color== p_color){
						//if (p_cColorSet.GetDataType() != CPN_PRODUCT) {

							instance_id = l_sPlaceName + wxT("_") + p_color;

							marking << l_pcCollis->GetCell(i, 1);

							return marking;
					 
						}

					}
				else {
					//deal with initial marking x`all()
					l_bIsAll = true;
					if (color == wxT("all()") || color.IsEmpty()) {

						for (auto entry : l_ColorVector) {
							if (entry == p_color) {

								wxString marking; marking << l_pcCollis->GetCell(i, 1);

								return marking;


							}

						}

					}

				}

			}

		}
	}
	return wxT("0");
}

bool  SP_DS_ColStAnimation::ExtractPlaceIds(const wxString& expression, SP_VectorString& p_vResultVector)
{
	wxStringTokenizer tokenizer(expression, " +-*/=^()");

	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		if(std::find(m_vColPlaceNames.begin(), m_vColPlaceNames.end(),token)!= m_vColPlaceNames.end())
		{

			p_vResultVector.push_back(token);
		}
	}
	if (p_vResultVector.size() > 0)
	{
		return true;
	}
	else {
		return false;
	}
}

long SP_DS_ColStAnimation::GetTransitionPosByName(const wxString& p_sName)
{
	//if (!m_mpcTransitionAnimators[l_nTransition]->IsEnabled(b)) return 0.0;

	//l_nTransition is enabled under binding <<b>>
	for (long i = 0;i<m_mpcTransitionAnimators.size();++i)
	{
		SP_DS_Node* pc_Transnode = (SP_DS_Node*)(m_mpcTransitionAnimators[i])->GetParentNode();
		SP_DS_Attribute* l_pcAttrTran = (pc_Transnode)->GetAttribute(wxT("Name"));
		SP_DS_NameAttribute* l_pcNameAttrTr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttrTran);
		wxString l_sTrnasName = l_pcNameAttrTr->GetValueString();

		if (l_sTrnasName == p_sName) return i;
	}

	return -1;//not found
}


void SP_DS_ColStAnimation::RemoveRemainingTranistionInstancesFromFiringSchedule(const wxString& l_sColTransName, const std::vector<wxString>& p_vBinding)
{
	// we need to go to the all instances that are not enabled 
	// reset their corresponding Hazard values
	for (const auto& pair : m_mInstance2HazardValue) {
		const wxString& key = pair.first;
		double l_dHz = pair.second;

		if (key.StartsWith(l_sColTransName)) {
			// Key matches the given prefix, process it accordingly
			wxString l_sColor;

			//wxString fullString = wxT("t_m_1_1_1");

			wxString prefix = l_sColTransName + wxT("_");  // Given prefix
			wxString substring = key.Mid(prefix.Length()); // fetch the corresponding binding

			if (std::find(p_vBinding.begin(), p_vBinding.end(), substring) != p_vBinding.end()) continue;

			//if (key.EndsWith(l_sColor)) {
			m_nCombinedHazardValue -= l_dHz;

			auto l_it = m_mTransInstance2Id.find(key);
			if (l_it != m_mTransInstance2Id.end())
			{
				unsigned l_nTransInstId = l_it->second;
				m_RowSum[l_nTransInstId / m_RowLength] -= l_dHz;
				m_mInstance2HazardValue[key] = 0;
			}

			if (m_nCombinedHazardValue < 0)
			{
				m_nCombinedHazardValue = 0;
			}

			wxString l_sInst;
			l_sInst << l_sColTransName << wxT("|") << substring;
			auto it = std::find(m_vColTransInstances.begin(), m_vColTransInstances.end(), l_sInst);
			if (it != m_vColTransInstances.end()) {// remove this instance from firing candidates
				m_vColTransInstances.erase(it);
				//std::cout << "Element removed successfully!" << std::endl;
			}

		}
	}
}

void SP_DS_ColStAnimation::RemoveTranistionFromFiringSchedule(const wxString& l_sColTransName)
{
	// we need to go to the all instances that are not enabled 
	// reset their corresponding Hazard values
	for (const auto& pair : m_mInstance2HazardValue) {
		const wxString& key = pair.first;
		double l_dHz = pair.second;

		if (key.StartsWith(l_sColTransName)) {
			// Key matches the given prefix, process it accordingly
			m_nCombinedHazardValue -= l_dHz;

			auto l_it = m_mTransInstance2Id.find(key);
			if (l_it != m_mTransInstance2Id.end())
			{
				unsigned l_nTransInstId = l_it->second;
				m_RowSum[l_nTransInstId / m_RowLength] -= l_dHz;
				m_mInstance2HazardValue[key] = 0;

				wxString l_spref = l_sColTransName + wxT("_");
				wxString substring = key.Mid(l_spref.Length());

				wxString l_sInst;
				l_sInst << l_sColTransName << wxT("|") << substring;
				auto it = std::find(m_vColTransInstances.begin(), m_vColTransInstances.end(), l_sInst);
				if (it != m_vColTransInstances.end()) {// remove this instance from firing candidates
					m_vColTransInstances.erase(it);
					//std::cout << "Element removed successfully!" << std::endl;
				}
				 
			}
			
			if (m_nCombinedHazardValue < 0)
			{
				m_nCombinedHazardValue = 0;
			}
		}
	}

}


double SP_DS_ColStAnimation::ComputeFunctionHazard(int l_nTransition)
{

	SP_DS_ColStTransAnimator* l_pcNodeTrans = m_mpcTransitionAnimators[l_nTransition];

	if (l_pcNodeTrans==nullptr) return 0.0;

	double l_dHaz = 0.0;

	wxString l_sTrnasName = ColTransName(l_nTransition);  
	

	SP_VectorString b;
	if (!m_mpcTransitionAnimators[l_nTransition]->IsEnabled(b))
	{//we need to remove the relevant instance from firing schedule
		RemoveTranistionFromFiringSchedule(l_sTrnasName);
		return 0.0;
	}


	if (m_bStopSimulation) { return -1; }


		for (auto binding : b) {

			if (m_bStopSimulation) { return -1; }

			double l_dEvaluatedRateFunction = RateFunction(l_nTransition, binding);


			if (l_dEvaluatedRateFunction <= 0 && !m_bStopSimulation) {
				SP_MESSAGEBOX(wxT("Stop"));
			}

		 
			l_dHaz = l_dEvaluatedRateFunction;

			
            wxString l_sInstance = l_sTrnasName + wxT("_") + binding;

			auto itFindInst = m_mTransInstance2Id.find(l_sInstance);
			unsigned l_nInstTNum = 0;


			if (itFindInst == m_mTransInstance2Id.end())
			{
				m_mTransInstance2Id[l_sInstance] = m_nInstanceId++;
				l_nInstTNum = m_nInstanceId;
				if (m_nInstanceId > 100) {
					m_RowLength = floor(sqrt((double)m_nInstanceId));
					m_RowCount = m_nInstanceId / m_RowLength;
					while (m_RowLength * m_RowCount < m_nInstanceId)
					{
						++m_RowCount;
					}
				}
				else {
					m_RowLength = m_nInstanceId;
					m_RowCount = 1;
				}

			}
			else {
				l_nInstTNum = itFindInst->second;
			}

		 
				if (m_RowSum.size() ==0) {
					m_RowSum.assign(m_RowCount, 0.0);
					std::fill(m_RowSum.begin(), m_RowSum.end(), 0.0);
				}

				if (m_RowCount > 1) {
					SP_VectorDouble l_vnewVector;
					l_vnewVector.assign(m_RowCount, 0.0);

					for (unsigned k = m_RowSum.size(); k < m_RowCount; ++k) {
						l_vnewVector.push_back(0.0);
					}

					m_RowSum = l_vnewVector;
				}
			
			
		

			double l_oldHazard = 0.0;
			auto l_it = m_mInstance2HazardValue.find(l_sInstance);

			if (l_it != m_mInstance2HazardValue.end()) {
				l_oldHazard = l_it->second;
				m_nCombinedHazardValue -= l_oldHazard;
			}
		 

			l_dHaz = l_dHaz < 0 ? 0 : l_dHaz;
			wxString l_sInst; l_sInst << l_sTrnasName << wxT("|") << binding;
			m_anHazardValues.push_back(l_dHaz);
			if (std::find(m_vColTransInstances.begin(), m_vColTransInstances.end(), l_sInst) == m_vColTransInstances.end())
			{
				m_vColTransInstances.push_back(l_sInst);
			}
			 
			m_mInstance2HazardValue[l_sInstance] = l_dHaz;


			//Add the new value to the cumulative propensity
			m_nCombinedHazardValue += l_dHaz;

			//ToDo: l_nTransition should be instance number not ColTrans
			unsigned sId;
			if (l_nInstTNum == 0)
			{
				sId = (l_nInstTNum) / m_RowLength;
			}
			else {
				sId  = (l_nInstTNum - 1) / m_RowLength;
			}
			
			m_RowSum[sId] -= l_oldHazard;
			m_RowSum[sId] += l_dHaz;

			if (m_nCombinedHazardValue < 0)
			{
				m_nCombinedHazardValue = 0;
			}
 
		}

		// here we need to check the complement set of <b>, refferes to <b`>
		//<b`>:  the binding set, that the colored transition of index l_nTransition is not enabled using them
		RemoveRemainingTranistionInstancesFromFiringSchedule(l_sTrnasName,b);
 
	 
	return 1;
}



double SP_DS_ColStAnimation::SimulateSingleStep(double p_dcurrentTime) {

	//double l_dcurrentTime;

	double l_dTotalHazard = 0.0;

	int    l_nSelectedtransition = 0;

	l_dTotalHazard = CalculateTotalHazard();

	if (m_anHazardValues.size() == 0 || m_bStopSimulation || l_dTotalHazard==-1) {
		return -1;
	}

	double tau = GenerateRandomVariableExpDistrBySeed(m_nCombinedHazardValue);

	p_dcurrentTime += tau;


	long l_nSelectedTransition = GenerateRandomVariableDiscreteBySeed(m_nCombinedHazardValue);

	wxString l_sIndex2; l_sIndex2 << "selected transition is: " << l_nSelectedTransition;// << " with name  " << GetColorTransitionName(l_nSelectedTransition);

	//SP_LOGMESSAGE(l_sIndex2);

	
	wxString l_sTransInstance;
	long l_nIndex = 0;
	for (auto pair : m_mInstance2HazardValue) {
	
		if (l_nIndex == l_nSelectedTransition)
		{
			l_sTransInstance = pair.first;
			break;
		}
		++l_nIndex;
	}
	wxString l_sInstance = l_sTransInstance; 
	
 
	wxString l_sIndex; 
	l_sIndex << "selected transition instance is : " << l_sInstance;
 
	m_ExportBindings.AddLine(l_sIndex);


	wxString l_sColTransName = l_sTransInstance.BeforeFirst(wxChar('_'));//BeforeFirst(wxChar('|'))

 
	SP_VectorString l_vbindingVector;

	l_vbindingVector.push_back(l_sTransInstance.AfterFirst(wxChar('_')));


	l_nSelectedTransition = GetTransitionPosByName(l_sColTransName);

	//fire l_nSelectedTransition
	FireOneTransition(l_nSelectedTransition, l_vbindingVector);

	return p_dcurrentTime;
}

bool SP_DS_ColStAnimation::InitiliseSimulator() {

	if (m_vSelectedPlaces.size() == 0)
	{
		SP_MESSAGEBOX(wxT("No place traces are chosen to export!"), wxT("Notification"), wxOK | wxICON_ERROR);
		return false;
	}
	if (!m_cbSimMode->IsChecked()) {
		SP_MESSAGEBOX(wxT("Simulation Mode is not selected!"), wxT("Notification"), wxOK | wxICON_ERROR);
		return false;
	}

	wxString l_sLogmsg;
	double dblValue;
	long l_nVal;
	bool success;
	m_nProgress = 0;
	Traces.clear();
	m_RowLength = 0;
	m_mTransInstance2Id.clear();
	m_mTransInstance2Hazard.clear();
	m_nCombinedHazardValue = 0;
	m_mInstance2HazardValue.clear();
	wxString l_sIntervalStart = m_pcIntervalStartTextCtrl->GetValue();
	success = l_sIntervalStart.ToDouble(&dblValue);

	if (!success) {
		// Conversion failed
		l_sLogmsg << "Interval Start value must be double";
		SP_LOGMESSAGE(l_sLogmsg);
		return success;
	
	}
	else {
		m_nIntervalStart = dblValue;
	}



	wxString l_sIntervalsplitting = m_pcResultPointCountTextCtrl->GetValue();

	success = l_sIntervalsplitting.ToLong(&l_nVal);

	if (!success) {
		// Conversion failed
		l_sLogmsg << "Interval Splitting value must be long integer";
		SP_LOGMESSAGE(l_sLogmsg);
		return success;

	}
	else {
		m_nNumberofPoints = l_nVal;
	}

	wxString l_sIntervalEnd = m_pcIntervalEndTextCtrl->GetValue();


	success = l_sIntervalEnd.ToDouble(&dblValue);

	if (!success) {
		// Conversion failed
		l_sLogmsg << "Interval End value must be double";
		SP_LOGMESSAGE(l_sLogmsg);
		return success;

	}
	else {
		m_nIntervalEnd = dblValue;
	}


	wxString l_sSeed = m_pcSeedTextCtrl->GetValue();

	unsigned long  l_nSeed;
	success = l_sSeed.ToULong(&l_nSeed);

	m_pcRandGen = new MTRand();// necessary here before we set the seed for the random number

	if (!success) {
		// generate a random Seed 
		std::random_device rd;
		SetSeed(rd());
		wxString l_sSeed;
		l_sSeed << m_ulSeed;
		m_pcSeedTextCtrl->SetValue(l_sSeed);
	}
	else {//feed with user's seed
		SetSeed(l_nSeed);
	}

	m_nIntervalSize = (m_nIntervalEnd - m_nIntervalStart) / (m_nNumberofPoints - 1);
	m_dCurrentTime = m_nIntervalStart;

	m_nRefreshFrequ = 25;
	m_nStepDuration = 100;
	m_nProgress = 0;
	gauge->SetValue(m_nProgress);

	//extract colplace name
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
	m_vColPlaceNames.clear();
	m_vColPlaceNamesModified.clear();


	for (l_itPlace = (m_lAllPlaceAnimators).begin(); l_itPlace != (m_lAllPlaceAnimators).end(); ++l_itPlace)
	{
		ColPlace2InstancesStates currentStateCol2InstancesMap;



		SP_DS_Node*  itNode = (*l_itPlace)->GetNode();

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(itNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_vColPlaceNames.push_back(l_sPlaceName);
		m_vColPlaceNamesModified.push_back(l_sPlaceName+wxT("_"));
	}

	//Initilise color defintions
	if (!m_cValueAssign.InitializeColorset(m_cColorSetClass))
		return false;

	m_mPlaceInstance2Marking.clear();

	wxString l_sLog; l_sLog << "Seed: " << m_ulSeed;

	SP_LOGMESSAGE(l_sLog);
	
	m_nInstanceId = 0;
	

	return true;

}

void SP_DS_ColStAnimation::Simulate() {

	if (!InitiliseSimulator()) { return; }

	m_nCombinedHazardValue = 0.0;
	m_anHazardValues.clear();
	m_RowSum.clear();
	m_vColTrans2Binding.clear();
	m_vColTransInstances.clear();
	 
	m_pcBtn->SetBackgroundColour(wxColour(255, 0, 0));
	m_pcBtn->SetLabel(wxT("Abort Simulation"));

	m_stopwatch.Start();
	SetSimulationStopWatch(m_stopwatch.Time());

	//init grid time 
	double l_dGridTime = m_nIntervalStart;

	// init output grid rows
	unsigned long l_nCurrentRow = 1;

	//wxString lmsg; lmsg << wxT("current time: ") << l_dGridTime;

    //write initial state to output trace file
	OutputStateAt(m_nIntervalStart);

	l_dGridTime = l_dGridTime + m_nIntervalSize;

	long l_nStep = 0;// init num of steps with 0

	m_nProgress = (l_nCurrentRow ) * 100 / m_nNumberofPoints;
	gauge->SetValue(m_nProgress);
	//m_pcHint->SetLabel(wxString::Format(wxT("%i %%"), m_nProgress));
	m_sHint.clear();
	m_sHint<<m_nProgress;
	m_pcHint->SetLabel(m_sHint);
	wxYield();

	while ((m_dCurrentTime < m_nIntervalEnd) && (!m_bStopSimulation) && (l_nCurrentRow <= m_nNumberofPoints - 1)) {
		
		SetSimulationStopWatch(m_stopwatch.Time());

		m_dCurrentTime = SimulateSingleStep(m_dCurrentTime);
		
		if (m_dCurrentTime == -1)
		{
			SP_LOGERROR_(wxT("Simulation Aborted!"));
			break;
		}
		wxYield();//update GUI
		l_nStep++;

		if (m_dCurrentTime < 0) {

			m_dCurrentTime = m_nIntervalEnd;

			for (unsigned long s = l_nCurrentRow; s < m_nNumberofPoints; s++) {

				OutputStateAt(l_dGridTime);

				l_dGridTime += m_nIntervalSize;

				//update progress bar
				m_nProgress = (s ) * 100 / m_nNumberofPoints;
				gauge->SetValue(m_nProgress);
				m_sHint.clear();
				m_sHint << m_nProgress;
				m_pcHint->SetLabel(m_sHint);
				wxYield();

			}
			return;
		}

		while ((m_dCurrentTime>= l_dGridTime) && (l_nCurrentRow <= m_nNumberofPoints-1))
		{
			OutputStateAt(l_dGridTime);

			l_nCurrentRow++;

			l_dGridTime = l_dGridTime + m_nIntervalSize;


			m_nProgress = (l_nCurrentRow ) * 100 / m_nNumberofPoints;
			gauge->SetValue(m_nProgress);
			m_sHint.clear();
			m_sHint<< m_nProgress;
			m_pcHint->SetLabel(m_sHint);
			wxYield();
			if (l_nCurrentRow > m_nNumberofPoints - 1)
				break;
		}
	}
	//m_bStopSimulation = false;
	m_pcBtn->SetBackgroundColour(wxColour(0, 255, 0));
	m_pcBtn->SetLabel(wxT("Start"));

}


bool
SP_DS_ColStAnimation::PreStep(int p_nColTrans, const std::vector<wxString>& p_vBinding)
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
			if (p_nColTrans == -1)
				ChooseColTransitions();  //choose the transition to be fired
			else
				ChooseColTransGillespiesSSA(p_nColTrans);

			//...for stepping forward and random backsteps
			/*		for (l_itTrans = m_lAllTransAnimators.begin(); l_itTrans != m_lAllTransAnimators.end(); ++l_itTrans)
			{
			m_lPossibleTransAnimators.push_back(*l_itTrans);
			}
			*/
		}
	}


	else if (m_bImport == true)//take the possible trans from the input trace,by george
		if (!ImportStepSequences())
		{
			SP_MESSAGEBOX(wxT("No more entries in the file"), wxT("Notification"), wxOK | wxICON_INFORMATION);
			if (!m_cFiredTransitions.empty()) {
				SP_DS_Graph *graph = m_cFiredTransitions.front()->GetClassObject()->GetParentGraph();
				if (wxGetApp().GetIAManager() && wxGetApp().GetIAManager()->IAModeEnabled(graph)) {
					if (GetDirection() == FORWARD) {
						wxGetApp().GetIAManager()->SubmitEvent(new SP_IA_Event(&m_cFiredTransitions, SP_IA_EVT_PEDANIMPRESTEP_FWD));
					}
				}
			}
			if (m_pcDialog) {
				m_pcDialog->ResetPlayButtons();
			}
			return false;
		}



	//OutputStateAt(0.0);

//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
	{
		SP_DS_ColStTransAnimator* l_pcColStTransAnimator = (SP_DS_ColStTransAnimator*)(*l_itTrans);
		SP_DS_Attribute* l_sNodeNAme = (*l_itTrans)->GetParentNode()->GetAttribute(wxT("Name"));
		int l_nCount = l_sNodeNAme->GetValueString().Freq(wxChar('_'));

		++l_nCount;

		auto itFind = m_mTransID2Color.find(m_nStepCount);
		if (itFind != m_mTransID2Color.end() && m_bImport)
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

			l_pcColStTransAnimator->InformStPrePlaces(l_sChosenColor);

		}
		else
		{
			wxString l_sEmpty = wxT("");
			l_pcColStTransAnimator->InformStPrePlaces(l_sEmpty,p_vBinding);
		}
	}


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

			if ((*l_itTrans)->GetChosenBinding().size() > 0 && m_sTriggiredUnfoldedTrans == wxT(""))
			{
				SP_DS_Attribute* l_pcAttr = (*l_itTrans)->GetParentNode()->GetAttribute(wxT("Name"));
				SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
				if ((*l_itTrans)->GetChosenBinding()[0].Contains("?"))
				{
					m_sTriggiredUnfoldedTrans = l_pcNameAttr->GetValue() + wxT("_") + (*l_itTrans)->GetChosenBinding()[0].BeforeFirst(wxChar('?'));
				}
				else
					m_sTriggiredUnfoldedTrans = l_pcNameAttr->GetValue() + wxT("_") + (*l_itTrans)->GetChosenBinding()[0];
			}
		}
	}

	//SP_LOGMESSAGE(wxT("Fired instance: "+ m_sTriggiredUnfoldedTrans));
	m_sTriggiredUnfoldedTrans.Clear();
	// all transitions with concession are resolved, now test the stepping option
 
	if (m_bExport == true)
		ExportStepSequences();

 
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
		if(!m_bColSimMode)
		SP_MESSAGEBOX(msg, wxT("Notification"), wxOK | wxICON_INFORMATION);
		else {
			SP_LOGMESSAGE(msg);
		}
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



	SetStepCounter();
	m_bInPreStep = true;
	return l_bReturn;
}

void SP_DS_ColStAnimation::FireOneTransition(int p_nColTrans, const std::vector<wxString>& valuess) {

	bool res = TRUE;

   if (m_nActStep == 0)
	{
		res = PreStep(p_nColTrans, valuess);
	}

   while (m_nActStep != 0) {//model OnTimerevent
	   res &= Step();
	   Refresh();

	   if (m_nActStep == m_nSteps)
	   {
		   res &= PostStep();
		   if (m_bOneShot)
		   {
			   StopTimer();
			   m_bOneShot = FALSE;

			   return;
		   }
	   }
   }

   if (res)
   {
	   m_cTimer.Start(m_nRefreshFrequ, wxTIMER_ONE_SHOT);
   }
   else
   {
	   StopTimer();
   }

}

void
SP_DS_ColStAnimation::OnTimer(wxTimerEvent& p_cEvent)
{

	if (m_bColSimMode)  //for colsimulation 
	{
		if (!m_bStopSimulation)
		{
 
		}
		else {
		//	m_stopwatch.Pause();
		//	m_bStopSimulation = false;
		}
		if (m_nProgress == 100)
		{
			m_pcDialog->SetClose(true);
			m_pcDialog->SetSimMode(false);
			m_nProgress = 0;
		}
			
		return;
	}


	bool res = TRUE;

	if (m_nActStep == 0 && !m_bColSimMode)
	{
		res = PreStep();
	}
 
	res &= Step();
	Refresh();

	if (m_nActStep == m_nSteps)
	{
		res &= PostStep();
		if (m_bOneShot)
		{
			StopTimer();
			m_bOneShot = FALSE;

			return;
		}
	}

	if (res)
	{
		m_cTimer.Start(m_nRefreshFrequ, wxTIMER_ONE_SHOT);
	}
	else
	{
		StopTimer();
	}
}

bool
SP_DS_ColStAnimation::Step()
{
	bool l_bReturn = TRUE;
	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;

	if (m_nActStep < (m_nSteps / 2) )
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
	if (m_nActStep > (m_nSteps / 2) )
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

	int width = p_pcCtrl->GetSize().GetWidth();
	int height = p_pcCtrl->GetSize().GetHeight() + 100;
	p_pcCtrl->SetSize(wxSize(width, height));

	m_pcDialog = p_pcCtrl;

		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		p_pcSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDSET, wxT("Keep Marking")));// , 1, wxALL | wxEXPAND, 5);
		m_cbKeep = new wxCheckBox(p_pcCtrl, SP_ID_PEDKEEP, wxT("Always keep marking when closing."));
		m_cbKeep->SetValue(wxGetApp().GetAnimationPrefs()->GetKeepMarking());
		p_pcSizer->Add(m_cbKeep);// 0, wxALL, 5);

		//------------------------------------------
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		wxSizer* m_pcExportImportSizer = new wxBoxSizer(wxHORIZONTAL);
		m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDEXPORT, wxT("Export")), 1, wxALL | wxEXPAND, 5);
		m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDIMPORT, wxT("Import")), 1, wxALL | wxEXPAND, 5);
		p_pcSizer->Add(m_pcExportImportSizer);// 0, wxALL, 5);
		//------------------------------------------
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		wxString l_tmp;
		l_tmp << m_nStepCount;
		m_pcStepCounter = new wxBoxSizer(wxHORIZONTAL);
		m_pcStepCounterText = new wxStaticText(p_pcCtrl, wxID_ANY, wxT("Step Counter: "));
		m_pcStepCounterValue = new wxStaticText(p_pcCtrl, wxID_ANY, l_tmp);

		m_pcStepCounter->Add(m_pcStepCounterText);// 1, wxEXPAND | wxALL, 5);
		m_pcStepCounter->Add(m_pcStepCounterValue);// 1, wxEXPAND | wxALL, 5);

		p_pcSizer->Add(m_pcStepCounter, 0, wxEXPAND);
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		/**************************************************************/
		// Create the sizer for the scrollable content
		wxBoxSizer* scrolledConfigSizer = new wxBoxSizer(wxVERTICAL);

		wxSizer* l_pcSetsSizer = new wxBoxSizer(wxVERTICAL);


		wxSizer* l_pcRowSizer; 


		if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0)//by george, preventing of occuring empty v-sets
		{

			    l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

			    l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Marking set:")), 1, wxALL | wxEXPAND, 5);

			    m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_MARKING_SETS, wxDefaultPosition, wxSize(100, -1)));

			    l_pcRowSizer->Add(m_apcComboBoxes[0], 0, wxALL, 5);

			    l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);

			    l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

			    m_mGroup2Pos[wxT("Marking")] = m_apcComboBoxes.size()-1;

				scrolledConfigSizer->Add(l_pcSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));

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

				scrolledConfigSizer->Add(l_pcSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));


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

				scrolledConfigSizer->Add(l_pcSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));


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

				scrolledConfigSizer->Add(l_pcSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));



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

				scrolledConfigSizer->Add(l_pcSetsSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));


		}

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

			scrolledConfigSizer->Add(l_pcSetsSizer);// wxSizerFlags(0).Expand().Border(wxALL, 2));

		}

		/************************************************/
		// Set the sizer for the scrolled window
		//scrolleConfigdWindow->SetSizer(scrolledConfigSizer);

		// Calculate and set the virtual size for the scrolled window
		//wxSize virtualSizeConfig = scrolledConfigSizer->GetMinSize();
		//scrolleConfigdWindow->SetVirtualSize(virtualSizeConfig);

		// Optionally, set the scrolling rate (horizontal_rate, vertical_rate)
		//scrolleConfigdWindow->SetScrollRate(5, 5);


		// Add the wxScrolledWindow to the main sizer
		//p_pcSizer->Add(collapsiblePaneModelConfig);

		/************************************************************/
		//m_msMarking Sets
	 
		p_pcSizer->Add(l_pcSetsSizer);// , 0, wxALL, 5);
		p_pcSizer->Add( new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND);
 

		LoadSets();
		/**********************************************************/

		m_cbSimMode = new wxCheckBox(p_pcCtrl, SP_ID_SIMMDE, wxT("Color Simulation Mode"));
		m_cbSimMode->SetValue(false);
		p_pcSizer->Add(m_cbSimMode, 0, wxALL, 5);
		p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);

		/*********************************************************/
		// Create the wxCollapsiblePane
		 wxCollapsiblePane* collapsiblePane = new wxCollapsiblePane(p_pcCtrl, wxID_ANY, "Expand/Collapse Sim. Configuration",
			 wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

		 // Get the pane (a wxWindow) inside the collapsible pane
		 wxWindow* pane = collapsiblePane->GetPane();

		 // Create the wxScrolledWindow to hold the scrollable content
		 wxScrolledWindow* scrolledWindow = new wxScrolledWindow(p_pcCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);

		 // Create the sizer for the scrollable content
		 wxBoxSizer* scrolledSizer = new wxBoxSizer(wxVERTICAL);

	
		 // Add your UI elements to the scrolledSizer
		 /***********************************************************************/
		 wxSizer* l_pcRowSizerSimConfigIntStart = new wxBoxSizer(wxHORIZONTAL);
		 l_pcRowSizerSimConfigIntStart->Add(new wxStaticText(pane, -1, wxT("interval start:")),wxSizerFlags(1).Expand().Border(wxALL, 2));

		 m_pcIntervalStartTextCtrl = new wxTextCtrl(pane, -1, "0", wxDefaultPosition, wxDefaultSize, 0);
		 l_pcRowSizerSimConfigIntStart->Add(m_pcIntervalStartTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));

		// p_pcSizer->Add(l_pcRowSizerSimConfigIntStart, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 scrolledSizer->Add(l_pcRowSizerSimConfigIntStart, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 /**************************************************************************/
		 //scrolledSizer->Add(new wxStaticLine(pane, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
		 l_pcRowSizerSimConfigIntStart = new wxBoxSizer(wxHORIZONTAL);
		 l_pcRowSizerSimConfigIntStart->Add(new wxStaticText(pane, -1, wxT("interval end:")),wxSizerFlags(1).Expand().Border(wxALL, 2));
		 m_pcIntervalEndTextCtrl = new wxTextCtrl(pane, -1, "100", wxDefaultPosition, wxDefaultSize, 0);
		 l_pcRowSizerSimConfigIntStart->Add(m_pcIntervalEndTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 scrolledSizer->Add(l_pcRowSizerSimConfigIntStart,wxSizerFlags(0).Expand().Border(wxALL, 2));

		 /**************************************************************************/
 		 
		 wxSizer* l_pcRowSizerSimConfig3 = new wxBoxSizer(wxHORIZONTAL);
		 l_pcRowSizerSimConfig3->Add(new wxStaticText(pane, -1, wxT("interval splitting:")), wxSizerFlags(1).Expand().Border(wxALL, 2));
		 m_pcResultPointCountTextCtrl = new wxTextCtrl(pane, -1, "100", wxDefaultPosition, wxDefaultSize, 0);
		 l_pcRowSizerSimConfig3->Add(m_pcResultPointCountTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 scrolledSizer->Add(l_pcRowSizerSimConfig3, wxSizerFlags(0).Expand().Border(wxALL, 2));

		 /**************************************************************************/
		 wxSizer* l_pcRowSizerSimConfigSeed = new wxBoxSizer(wxHORIZONTAL);
		 l_pcRowSizerSimConfigSeed->Add(new wxStaticText(pane, -1, wxT("Seed Value:")),wxSizerFlags(1).Expand().Border(wxALL, 2));
		 m_pcSeedTextCtrl = new wxTextCtrl(pane, -1, "Random Seed", wxDefaultPosition, wxDefaultSize, 0);
		 l_pcRowSizerSimConfigSeed->Add(m_pcSeedTextCtrl, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 scrolledSizer->Add(l_pcRowSizerSimConfigSeed, wxSizerFlags(0).Expand().Border(wxALL, 2));

		 /**************************************************************************/
 
		 scrolledSizer->Add(new wxStaticLine(pane, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);

		 wxSizer* l_pcRowSizerSimConfig4 = new wxBoxSizer(wxHORIZONTAL);
		 //l_pcRowSizerSimConfig4->Add(new wxButton(pane, SP_ID_BUTTON_ExportSIMTRACES_COLSIM, wxT("Trace File")), 0, wxALL, 5);

		 /**************************************************************************/
		 l_pcRowSizerSimConfig4->Add(new wxButton(pane, SP_ID_BUTTON_CHOOSE_PLACES, wxT("Choose Places")), 0, wxALL, 5);
		 scrolledSizer->Add(l_pcRowSizerSimConfig4);// wxSizerFlags(0).Expand().Border(wxALL, 2));

		 /*************************************************************************/
		 //view
		 wxSizer* l_pcViewSizer = new wxStaticBoxSizer(new wxStaticBox(pane, -1, wxT("View")), wxVERTICAL);
		 wxListBox* m_pcListboxShowAllGraphViewName = new wxListBox(pane, SP_ID_LISTBOX_VIEWS, wxDefaultPosition, wxSize(-1, 50), 0, NULL, wxLB_EXTENDED | wxLB_HSCROLL);
		 l_pcViewSizer->Add(m_pcListboxShowAllGraphViewName, wxSizerFlags(1).Expand().Border(wxALL, 5));
		 m_pcListboxShowAllGraphViewName->SetToolTip(wxT("Double click to open selected view(s)"));
		 scrolledSizer->Add(l_pcViewSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
		 m_pcListboxShowAllGraphViewName->Insert(wxT("Plot Viewer"), m_pcListboxShowAllGraphViewName->GetCount());
		

		 /**************************************************************************/
		 wxSizer* l_pcProgressSizer = new wxBoxSizer(wxVERTICAL);
		 wxSizer* l_pcHinSizer = new wxBoxSizer(wxHORIZONTAL);
		 wxSize size(300, 20);
		 gauge = new wxGauge(pane, wxID_ANY, 100, wxDefaultPosition, size, wxGA_HORIZONTAL);//wxDefaultPosition
		 l_pcProgressSizer->Add(gauge);
		 m_pcHint = new wxStaticText(pane, -1, wxT("0"));
		 m_pcPercentage = new wxStaticText(pane, -1, wxT("   %"));
		 wxString l_sTime;
		 l_sTime = wxString::Format("%*s%s", 90, "", "0.0s");
		 m_pcHintTime = new wxStaticText(pane, -1, l_sTime);
		 l_pcHinSizer->Add(m_pcHint);
		 l_pcHinSizer->Add(m_pcPercentage);

		 l_pcHinSizer->Add(m_pcHintTime, wxSizerFlags(0).Align(wxALIGN_RIGHT));


		 scrolledSizer->Add(l_pcProgressSizer);
		 scrolledSizer->Add(l_pcHinSizer);
		 m_pcBtn = new wxButton(pane, SP_ID_BUTTON_START_COLSIM, wxT("Start"), wxDefaultPosition, wxSize(100, 40));
		 scrolledSizer->Add(m_pcBtn, 0, wxALL, 5);
		 /**************************************************************************/
		 // Set the sizer for the scrolled window
		 scrolledWindow->SetSizer(scrolledSizer);

		 // Calculate and set the virtual size for the scrolled window
		 wxSize virtualSize = scrolledSizer->GetMinSize();
		 scrolledWindow->SetVirtualSize(virtualSize);

		 // Optionally, set the scrolling rate (horizontal_rate, vertical_rate)
		 scrolledWindow->SetScrollRate(5, 5);
 

		 // Add the wxScrolledWindow to the main sizer
		 p_pcSizer->Add(collapsiblePane, 1, wxEXPAND | wxALL, 5);

		/*********************************************************/


		p_pcCtrl->PushEventHandler(this);

		//allow backstepping
		m_pcDialog->EnableBackStepping(true);

		return TRUE;
}

void
SP_DS_ColStAnimation::OnReset(bool p_bIsAnimMode)
{

	list<SP_DS_ColStPlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		(*l_Iter)->ResetMarking();

	LoadCurrentMarking();


	m_llHistoryTransAnimators.clear();
	//by george
	m_nStepCount = 0;
	m_nLineCount = 1;//read from tseq file, reset to the begining
	if (m_bExport)
	{
		ResetTransSequenceFile();//reset exported trans seq file
	}

	if (p_bIsAnimMode)
	{
		SetStepCounter();
	}
	
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
SP_DS_ColStAnimation::OnSimMode(wxCommandEvent& event) {
	if (m_cbSimMode->IsChecked()) {
		//if (m_bColSimMode) return;
		m_bColSimMode = true;
		m_pcDialog->DisapleAnimMode();
		m_pcDialog->SetSimMode(true);

	}
	else
	{
		m_bColSimMode = false;
		m_pcDialog->EnableAnimMode();
		m_pcDialog->SetSimMode(false);

	}
}

void
SP_DS_ColStAnimation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{

	if (m_bColSimMode)
	{
		if (m_bStopSimulation)
		{
			m_pcDialog->SetClose(true);
			m_pcDialog->SetSimMode(true);
		}
		else
		{
			m_pcDialog->SetClose(false);

		}
		/**
		if (m_nProgress == 100)
		{
			m_pcDialog->SetClose(true);
			m_pcDialog->SetSimMode(false);
		}*/
	}
	else {
		m_pcDialog->SetClose(true);

	}
	//if (m_cbSimMode) return;
	if (p_cEvent.GetId() == SP_ID_PEDSET) p_cEvent.Enable(!m_bRunning && !m_cbKeep->IsChecked());
	else p_cEvent.Enable(!m_bRunning);

	    int l_nColoringPosition = 0;
		bool l_bFound = false;
		for (auto it = m_choices.begin(); it != m_choices.end(); ++it)
		{
			if (*it == wxT("coloring"))
			{
				l_bFound = true;
				break;
			}
			l_nColoringPosition++;
		}

		if (l_bFound)
		{
			m_nColoringGroupCurrentSelectedValue = m_apcComboBoxes1[l_nColoringPosition]->GetSelection();
		}

		if (m_cbSimMode == NULL) return;

		if (m_cbSimMode->IsChecked()) {
			//if (m_bColSimMode) return;
			m_bColSimMode = true;
			m_pcDialog->DisapleAnimMode();

		}
		else
		{
		 

			m_bColSimMode = false;
			m_pcDialog->EnableAnimMode();
			
		}
		
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

void SP_DS_ColStAnimation::OnClose(wxCommandEvent& p_cEvent)
{
	if (!m_bStopSimulation)
	{
		return;
	}
	
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

bool SP_DS_ColStAnimation::StartTimerForColSim() {

	if (m_bRestartAnimationFlag)
	{

		//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
		m_bRestartAnimationFlag = false;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Here add the codes to check syntax before animation

		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.SyntaxChecking())
			return false;
	}
	return SP_DS_Animation::StartTimer();
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

	SP_VectorStdString* l_vUnfoldedPlaces = (m_pcUnfolding)->GetPlaceNames();

	for (const auto& place : *l_vUnfoldedPlaces) {
		ar_PlaceList.Add(place);
	}
	
	return SP_DS_Animation::StartTimer();

}



void  SP_DS_ColStAnimation::CalculateHazardValue(long p_nTransitionNumber)
{

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
			///
			SP_DS_ColListAttribute* l_pcColList = nullptr;
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcChosenTransition->GetAttribute(wxT("FunctionList")));
			 
			for (unsigned j = 1; j < l_pcColList->GetColCount(); j++)
			{
				 
					wxString l_sRatefunction = l_pcColList->GetCell(0, j);
				 
			}
			//l_pcChosenTransition->
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


void SP_DS_ColStAnimation::LoadMarkingForcolorSimulation(map<long, SP_CPN_CountInterval> p_vColPlacesInterval, SP_VectorStdString p_ColorofPlacesVector, vector<SP_VectorLong>  p_ColorVector) {
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

	for (unsigned i = 0; i < p_ColorVector.size(); i++)
	{
		m_anCurrentMarking.push_back(p_ColorVector[i][l_nSel]);//push back marking of each individual unfolded place
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

		unsigned l_nLow = p_vColPlacesInterval[l_nColoredPlacePos].m_nLow;
		unsigned l_nUp = p_vColPlacesInterval[l_nColoredPlacePos].m_nUp;

		for (unsigned j = l_nLow; j <= l_nUp; j++)
		{
			wxString l_sColor = p_ColorofPlacesVector[j];
			long l_nMarking = m_anCurrentMarking[j];
			l_pcPlaceMultiSet->AddMultiSet(l_sColor, l_nMarking);
		}

	//	(*l_itPlace)->UpdateMarking(); //Update the marking of a place

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
		m_anCurrentMarking.push_back(m_anNetMarkings[i][l_nSel]);//push back marking of each individual unfolded place
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
			if (m_msColorsOfPlace.size() == 0 || m_anCurrentMarking.size() == 0) {
				return;
			}
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

	//SP_VectorLong& l_anCurrentMarking = m_pcSimulator->GetCurrentMarking();

	//wxString l_sTest ;
	//for (unsigned i = 0; i < m_anCurrentMarking.size(); i++)
	//{
	//	l_anCurrentMarking[i] = m_anCurrentMarking[i];
	//}



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
	SP_VectorStdString l_asParameterNames;
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

	SP_VectorStdString* l_pcTransitionName = m_pcUnfolding->GetTransitionNames();
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

bool SP_DS_ColStAnimation::ChooseColTransGillespiesSSA(int p_nChooseTransition)
{

	if (dynamic_cast<SP_DS_ColStTransAnimator*>(m_pcSingleStep))
	{
		return TRUE;
	}

	SetCurrentMarking();

	

	list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;

	SP_DS_ColStTransAnimator* l_pcChosenTransition = NULL;

	 

	if (p_nChooseTransition >= 0)
	{
		map<long, SP_CPN_CountInterval>::iterator itMap;
		for (itMap = m_pnsTransitionCountById.begin(); itMap != m_pnsTransitionCountById.end(); itMap++)
		{
			if ((unsigned int)p_nChooseTransition >= itMap->second.m_nLow && (unsigned int)p_nChooseTransition <= itMap->second.m_nUp)
			{
				int l_nChooseColoredTransition = p_nChooseTransition;// itMap->first;
				wxString l_sColoredTransName;

				{

					SP_DS_ColStTransAnimator* l_pcNodeTrans = m_mpcTransitionAnimators[p_nChooseTransition];

					if (!l_pcNodeTrans) return false;

					SP_DS_Node* l_pcNode = l_pcNodeTrans->GetParentNode();

					SP_DS_Attribute* l_pcAttr = (l_pcNode)->GetAttribute(wxT("Name"));
					SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);
					l_sColoredTransName = l_pcNameAttr->GetValueString();

				}

				//wxString l_sColoredTransName = m_msColoredTransitionNames[l_nChooseColoredTransition];
			//	SP_MESSAGEBOX(l_sColoredTransName);
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
	    bool l_bIsColoringChanged = false;
		int l_nColoring = 0;
		bool l_bFound = false;
		for (auto it = m_choices.begin(); it != m_choices.end(); ++it)
		{
			if (*it == wxT("coloring"))
			{
				l_bFound = true;
				break;
			}
			l_nColoring++;
		}

		for (size_t i = 0; i < m_apcColListAttrForConstants.size(); i++)
		{
			if (m_apcColListAttrForConstants[i])
			{
				int vsel = m_apcComboBoxes1[i]->GetSelection();

				m_apcColListAttrForConstants[i]->SetActiveList(vsel);
				if (i == l_nColoring  && l_bFound)
				{
					if (m_nColoringGroupCurrentSelectedValue != vsel)
					{
						l_bIsColoringChanged = true;
					}
				}


			}
		}

		if (l_bIsColoringChanged)//do unfolding only if the coloring group has been changed
		{
			UpdateColMarking();
		}

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
		 

		SP_DS_Attribute* l_pcMarkingAttr = dynamic_cast< SP_DS_Attribute*>(l_vPlaceNodes[l_nPos]->GetAttribute(wxT("Marking")));
		if (l_pcMarkingAttr)
			l_pcMarkingAttr->SetValueString(l_sMainMarking);

		l_vPlaceNodes[l_nPos]->Update(TRUE);
	}


	SP_Core::Instance()->GetRootDocument()->Modify(true);

	 
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

		for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		{
			(*l_Iter)->UpdateDefaultMarking();
		}
	
		SP_Core::Instance()->GetRootDocument()->Refresh();
		Refresh();
}

void SP_DS_ColStAnimation::SetStepCounter()
{
	if (m_pcStepCounterValue!=nullptr)
	m_pcStepCounterValue->SetLabel(wxString::Format(wxT("%ld"), m_nStepCount));
}

void SP_DS_ColStAnimation::ExportDetailsCPN(ExportStochCPN *export_frame)
{

	if (m_bColSimMode) {

		m_ExportFilename = export_frame->m_pc_Browse->GetPath();
		return;
	}


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

void SP_DS_ColStAnimation::ComputeResultMatrix()
{
	m_vvResultMatrix.clear();

	m_vxAxisVieverDate.clear();

	m_vvResultMatrix.resize(Traces.size(), std::vector<double>(m_vSelectedPlaces.size()));

	for (long i = 1; i < Traces.size(); ++i) {

		m_vxAxisVieverDate.push_back(Traces[i-1].timepoint);

		for (auto l_sPlace : m_vSelectedPlaces)
		{
			wxString placeInstance = l_sPlace;

			if (std::find(m_vSelectedPlaces.begin(), m_vSelectedPlaces.end(), placeInstance) == m_vSelectedPlaces.end())
				continue;

			for (long k = 0; k < Traces[i-1].instancePlaces.size(); ++k)
			{
				SP_MapString2String instancesmap = Traces[i-1].instancePlaces[k].color2Marking;
				wxString l_sTemp;
				SP_MapString2String::iterator it;
				it = instancesmap.find(placeInstance);
				if (it != instancesmap.end()) {
					l_sTemp << it->second; 
					double result;
					if (l_sTemp.ToDouble(&result)) {
						m_vvResultMatrix[i - 1][m_mselectedPlace2Index[placeInstance]] = result;
							break;
					}
				}
			}

		}
	}

}

void SP_DS_ColStAnimation::DoExport()
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

	//clear the content intially 
	m_ExportTextfile.Clear();

	wxString l_sTemp = wxT("t\t");


	for (auto l_sPlace : m_vSelectedPlaces)
	{
		auto it = m_mPlaceInstance2Marking.find(l_sPlace);
		if (it != m_mPlaceInstance2Marking.end())
		{
			l_sTemp << it->first << wxT("\t");
 		}
	}

	//Add colour places if selected (to the header)
	SP_VectorStdString* l_vColPlaces = (m_pcUnfolding)->GetColoredPlaceNames();


	l_sTemp << wxT("\n");

	m_ExportTextfile.AddLine(l_sTemp);
	

	//now we add net net evolution over time
	l_sTemp.Clear();

	l_sTemp << wxT("0\t");
	//write initial marking

	for (auto l_sPlace : m_vSelectedPlaces)
	{
		auto it = m_mPlaceInstance2Marking.find(l_sPlace);
		if (it != m_mPlaceInstance2Marking.end())
		{
			l_sTemp << it->second << wxT("\t");
 		}
	}
	
 
	m_ExportTextfile.AddLine(l_sTemp);


		for (long i = 1; i < Traces.size(); ++i) {

			l_sTemp.Clear();

			l_sTemp << Traces[i].timepoint << "\t";

			for (auto l_sPlace : m_vSelectedPlaces)
 			{
				wxString placeInstance = l_sPlace;

				if (std::find(m_vSelectedPlaces.begin(), m_vSelectedPlaces.end(), placeInstance) == m_vSelectedPlaces.end())
					continue;


				for (long k = 0; k < Traces[i].instancePlaces.size(); ++k)
				{
					SP_MapString2String instancesmap = Traces[i].instancePlaces[k].color2Marking;

					SP_MapString2String::iterator it;
					it = instancesmap.find(placeInstance);
					if (it != instancesmap.end()) {
						l_sTemp << it->second << wxT("\t");


						break;
					}
				}

			}

			m_ExportTextfile.AddLine(l_sTemp);

		}
	

	m_ExportTextfile.Write();
	m_ExportTextfile.Close();
}

void SP_DS_ColStAnimation::OnChoosePlaces(wxCommandEvent &p_pc_Event)
{

	if (m_cbSimMode->IsChecked())
	{

		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.SyntaxChecking())
			return ;


		if (!LoadUnfoldingInformation())
			return ;


		m_pcSimulator->SetOutputStartPoint(0);
		m_pcSimulator->SetOutputEndPoint(100);

		LoadCurrentMarking();

	SP_VectorStdString* l_vUnfoldedPlaces = (m_pcUnfolding)->GetPlaceNames();

	for (const auto& place : *l_vUnfoldedPlaces) {
		ar_PlaceList.Add(place);
	}

	//Add coloured places as well
	SP_VectorStdString* l_vColPlaces = (m_pcUnfolding)->GetColoredPlaceNames();

	for (const auto& place : *l_vColPlaces) {
		ar_PlaceList.Add(place);
	}

	ps = new SP_DS_ColSPN_PlaceSelection(NULL,wxT("Choose Place Instance"), ar_PlaceList);
	 
	if (ps->ShowModal() == wxID_OK)
	{
		m_vSelectedPlaces = ps->GetChosenVariables();

		for (long i = 0; i < m_vSelectedPlaces.size(); i++) {
 			m_mselectedPlace2Index[m_vSelectedPlaces[i]] = i;
		}

	}
	}
	else {
		SP_MESSAGEBOX(wxT("Simulation mode is not selected."), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}



	
}

void SP_DS_ColStAnimation::OnExportSimTraces(wxCommandEvent &p_pc_Event)
{
	if (m_cbSimMode->IsChecked()) {

	ExportStochCPN *export_frame = new ExportStochCPN(wxT("Export Details"), this, true);

	export_frame->Show(true);
    }
	else {
		SP_MESSAGEBOX(wxT("Simulation mode is not selected."), wxT("Error"), wxOK | wxICON_ERROR);
		return;
	}
	 
 
	 
 
	//ToDO: extend this parser by variable supporting, error handling and comparision operations
	/**
	std::string l_sExpression;
	
	wxTextEntryDialog l_pcDialog(NULL, _T("enter your expression to be evaluted"), wxT("Add Exp"));

 
	
	    std::string res_log;
		int l_nModalResult = l_pcDialog.ShowModal();
		if (wxID_OK == l_nModalResult) {
			l_sExpression = l_pcDialog.GetValue().ToStdString();
		}
	    
		std::stringstream ss(l_sExpression);

		using iterator_type = boost::spirit::istream_iterator;
		iterator_type begin(ss >> std::noskipws), end;

		 ExpressionEvaluator evaluator;
		 evaluator.setVariable("k", 2);
		 evaluator.setVariable("s1", 0.5);
		 evaluator.setVariable("k_1", 1);
		ExpressionGrammar<iterator_type> grammar(evaluator);

		double result;
		bool success = boost::spirit::qi::phrase_parse(begin, end, grammar, boost::spirit::ascii::space, result);

		if (success && begin == end)
		{
			res_log = "Result of: " + l_sExpression + " is: "+ std::to_string(result);
		}
		else
		{
			
			res_log = "Parsing of the expression" + l_sExpression +" failed at position: ";// +std::to_string(position) + "\n";
	
		}
		SP_LOGMESSAGE(res_log);
		*/
		

}

void SP_DS_ColStAnimation::OnSimulationStart(wxCommandEvent &p_pc_Event)
{

	if (m_bStopSimulation)
	{//simulator starts

	  m_bStopSimulation = false;

	 //transition firing counter
	  m_nStepCount = 0;
	  SetStepCounter();

	  //do simulation
	  Simulate();

	  ComputeResultMatrix();
	  
	  m_bInPreStep = false;

	  m_nProgress = 100;
	  gauge->SetValue(m_nProgress);
	  m_sHint.clear();
	  m_sHint << m_nProgress;
	  m_pcHint->SetLabel(m_sHint);

	}
	else {
		m_bStopSimulation = true;
		m_nProgress = 100;
		gauge->SetValue(m_nProgress);
		m_sHint.clear();
		m_sHint << m_nProgress;
		m_pcHint->SetLabel(m_sHint);
		m_pcBtn->SetLabel(wxT("Start"));
		m_pcBtn->SetBackgroundColour(wxColour(0, 255, 0));

		//refresh the parent window
		wxYield();
		m_stopwatch.Pause();
	}

	OnReset(false);
	m_bStopSimulation = true;
 
}

void SP_DS_ColStAnimation::OnExport(wxCommandEvent &p_pc_Event)
{
	
	ExportStochCPN *export_frame = new ExportStochCPN(wxT("Export Details"), this);

	export_frame->Show(true);
}

int SP_DS_ColStAnimation::TokenNumerOfAnimatorPlace(wxString p_sName, SP_VectorString p_vBindingVector)
{
	if (p_vBindingVector.size() == 0) return -1;
	
	long l_n_token_value;

	wxString l_sGivenColor = p_vBindingVector[0];
	

	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;

	for (l_itPlace = (m_lAllPlaceAnimators).begin(); l_itPlace != (m_lAllPlaceAnimators).end(); ++l_itPlace)
	{

		SP_DS_Node*  itNode = (*l_itPlace)->GetNode();

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(itNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		if (p_sName != l_sPlaceName) continue;

		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(itNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcNameAttibute)
			continue;
		wxString l_sColorSetName = l_pcNameAttibute->GetValue();

		SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(l_sColorSetName);
		if (!l_pcColorSet)
		{
			wxString l_sError = wxT("Place: ") + l_sPlaceName + wxT(": ") + l_sColorSetName + wxT(" is not right.");
			SP_LOGERROR(l_sError);
			return 0;
		}

		SP_CPN_ColorSet p_cColorSet = *l_pcColorSet;
		vector<wxString> l_ColorVector = p_cColorSet.GetStringValue();

		SP_DS_ColListAttribute* l_pcCollis = dynamic_cast<SP_DS_ColListAttribute*>((itNode)->GetAttribute(SP_DS_CPN_MARKINGLIST));
		wxString l_sMulti;
		if (l_pcCollis)
		{
			SP_MapString2String instances;
			for (unsigned int i = 0; i < l_pcCollis->GetRowCount(); i++)
			{
				wxString color = l_pcCollis->GetCell(i, 0);

				//deal with tuple color (c1,c2,..)
				color.Replace(wxT(","), "_");
				color.Replace(wxT("("), "");
				color.Replace(wxT(")"), "");
				color.Replace(wxT(" "), "");
				wxString marking;

				if (color != wxT("all()") && color == l_sGivenColor)
				{
					if (p_cColorSet.GetDataType() != CPN_PRODUCT) {

						marking << l_pcCollis->GetCell(i, 1);
						long l_ntokenNumer;

						if (!marking.ToLong(&l_ntokenNumer)) {
							wxString logError = wxT("Determining Number of tokens of the color ") + l_sGivenColor;
							SP_LOGERROR(logError);
							return -1;
						}
						return l_ntokenNumer;
						
					}
					else {
						if(color == l_sGivenColor)

						marking << l_pcCollis->GetCell(i, 1);
						long l_ntokenNumer;

						if (!marking.ToLong(&l_ntokenNumer)) {
							wxString logError = wxT("Determining Number of tokens of the color ") + l_sGivenColor;
							SP_LOGERROR(logError);
							return -1;
						}

						return l_ntokenNumer;
					}

				}
				else{
					//deal with initial marking x`all()
					 
					if (color == wxT("all()")|| color.IsEmpty()) {


							wxString marking; marking << l_pcCollis->GetCell(i, 1);

							marking << l_pcCollis->GetCell(i, 1);
							long l_ntokenNumer;

							if (!marking.ToLong(&l_ntokenNumer)) {
								wxString logError = wxT("Determining Number of tokens of the color ") + l_sGivenColor;
								SP_LOGERROR(logError);
								return -1;
							}
							return l_ntokenNumer;


					}

				}
		
			}

			if (l_pcCollis->GetRowCount() == 0) {
				
				return 0;
			}

		}

	}
	wxString logError = wxT("Determining Number of tokens of the color ") + l_sGivenColor;
	//SP_LOGERROR(logError);
	return 0;
	 

}

bool   SP_DS_ColStAnimation::IsTransitionEnabled(SP_DS_Node* m_pcNode) {

	if (!m_pcNode)
		return FALSE;

	//IsEnabled(SP_VectorString& v)

	vector<SP_CPN_ExpressionInfo> m_StExprInfoVector;
	SP_CPN_ExpressionInfo m_cStExprInfo;
	SP_DS_ColListAttribute* m_pcColList;
	SP_DS_Animation* m_pcAnimation;
	int m_nBindingChoice;
	m_pcAnimation = this;
	// i inform all nodes at my incoming edges of wanting their markings
	SP_ListEdge::const_iterator l_itEdges;
	SP_DS_ColStPlaceAnimator* l_pcAnim;
	map<SP_DS_ColStPlaceAnimator*, SP_ListEdge > m_mlPrePlaces;

	//push back the preplace and edges

	m_StExprInfoVector.clear();

	//push back the preplace and edges
	for (l_itEdges = m_pcNode->GetTargetEdges()->begin(); l_itEdges != m_pcNode->GetTargetEdges()->end(); ++l_itEdges)
	{
		if ((*l_itEdges)->GetSource())
		{
			l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetSource(), SP_DS_ANIMATOR_PLACE));
			
			if (l_pcAnim)
			{
				m_mlPrePlaces[l_pcAnim].push_back((*l_itEdges));

				m_cStExprInfo.m_eExprType = CPN_INPUT_EXPR;
				m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
				m_cStExprInfo.m_pcEdge = *l_itEdges;
				m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
				if (!m_pcColList)
					return false;
				if (m_pcColList->GetCell(0, 1) == wxT(""))
				{
					wxString l_sError;
					l_sError = wxT("Arc exprssion should not be empty");
					SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);
					return false;
				}

				m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);
				m_StExprInfoVector.push_back(m_cStExprInfo);
			}
		}
	}


	//push back the postplace and edges
	for (l_itEdges = m_pcNode->GetSourceEdges()->begin(); l_itEdges != m_pcNode->GetSourceEdges()->end(); ++l_itEdges)
	{
		if ((*l_itEdges)->GetTarget())
		{
			l_pcAnim = dynamic_cast<SP_DS_ColStPlaceAnimator*>(m_pcAnimation->GetAnimator((*l_itEdges)->GetTarget(), SP_DS_ANIMATOR_PLACE));
			if (l_pcAnim)
			{
				m_cStExprInfo.m_eExprType = CPN_OUTPUT_EXPR;
				m_cStExprInfo.m_pcPlAnimator = dynamic_cast<SP_DS_Animator*>(l_pcAnim);
				m_cStExprInfo.m_pcEdge = *l_itEdges;
				m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdges)->GetAttribute(SP_DS_CPN_INSCRIPTION));
				if (!m_pcColList)
					return false;
				if (m_pcColList->GetCell(0, 1) == wxT(""))
					return false;
				m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

				m_StExprInfoVector.push_back(m_cStExprInfo);
			}
		}
	}


	//push back the guard
	m_cStExprInfo.m_eExprType = CPN_GUARD_EXPR;
	m_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(m_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	if (m_pcColList)
		m_cStExprInfo.m_sExpression = m_pcColList->GetCell(0, 1);

	if (m_cStExprInfo.m_sExpression != wxT(""))
		m_StExprInfoVector.push_back(m_cStExprInfo);

	//begin to binding and enable test
	SP_CPN_Binding l_cBinding;
	SP_DS_Animator* l_pcAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(this);// (this);
	map<wxString, map<SP_DS_Edge*, map<wxString, int> > > l_mmmBind2Edge2Mult2Color;
	bool l_bEnableTest = l_cBinding.EnableTestForColorSimulation(&m_StExprInfoVector, false, wxT("dummy"), m_nBindingChoice, l_mmmBind2Edge2Mult2Color);


	m_pcAnimation->StopTimer();

	return l_bEnableTest;

}

void SP_DS_ColStAnimation::OutputStateAt(double p_dTime) {

	
	long l_n_token_value;
	
	StepState stateCurrent;
	std::vector<ColPlace2InstancesStates> l_vCol2State;

	stateCurrent.timepoint = p_dTime;

	wxString l_tempStepTrace(wxT(""));

	l_tempStepTrace << p_dTime << wxT('\t');

	wxString l_sColMarking;

	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;

	for (l_itPlace = (m_lAllPlaceAnimators).begin(); l_itPlace != (m_lAllPlaceAnimators).end(); ++l_itPlace)
	{
		ColPlace2InstancesStates currentStateCol2InstancesMap;

		l_n_token_value = (*l_itPlace)->GetMarking();

		l_sColMarking.Empty();

		l_sColMarking << l_n_token_value;

		SP_DS_Node*  itNode = (*l_itPlace)->GetNode();

		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(itNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(itNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcNameAttibute)
			continue;
		wxString l_sColorSetName = l_pcNameAttibute->GetValue();

		SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(l_sColorSetName);
		if (!l_pcColorSet)
		{
			wxString l_sError = wxT("Place: ") + l_sPlaceName + wxT(": ") + l_sColorSetName + wxT(" is not right.");
			SP_LOGERROR(l_sError);
			return ;
		}

		SP_CPN_ColorSet p_cColorSet = *l_pcColorSet;
		vector<wxString> l_ColorVector = p_cColorSet.GetStringValue();
		vector<wxString> l_ActualColorVector;
		bool l_bIsAll = false;
		currentStateCol2InstancesMap.colPlace = l_sPlaceName;

		SP_DS_ColListAttribute* l_pcCollis = dynamic_cast<SP_DS_ColListAttribute*>((itNode)->GetAttribute(SP_DS_CPN_MARKINGLIST));
		wxString l_sMulti;
		if (l_pcCollis)
		{
			SP_MapString2String instances;
			for (unsigned int i = 0; i < l_pcCollis->GetRowCount(); i++)
			{
				wxString color = l_pcCollis->GetCell(i, 0);
				wxString instance_id;
				wxString marking;

				if (color != wxT("all()"))
				{
					if (p_cColorSet.GetDataType() != CPN_PRODUCT) {

						instance_id = l_sPlaceName + wxT("_") + l_pcCollis->GetCell(i, 0);

						marking << l_pcCollis->GetCell(i, 1);

						instances[instance_id] = marking;
						l_ActualColorVector.push_back(color);

					}
					else {
						wxString l_sTupleColor = l_pcCollis->GetCell(i, 0);
						l_ActualColorVector.push_back(l_sTupleColor);

						l_sTupleColor.Replace(wxT(","), "_");
						l_sTupleColor.Replace(wxT("("), "");
						l_sTupleColor.Replace(wxT(")"), "");
						l_sTupleColor.Replace(wxT(" "), "");



						instance_id = l_sPlaceName + wxT("_") + l_sTupleColor;

						marking << l_pcCollis->GetCell(i, 1);

						instances[instance_id] = marking;
						
					}
					

					if (m_mPlaceInstance2Marking.find(instance_id) == m_mPlaceInstance2Marking.end()) {
						m_mPlaceInstance2Marking[instance_id] = marking;
 					}
				}
				else{
					//deal with initial marking x`all()
					l_bIsAll = true;
					if (color == wxT("all()")|| color.IsEmpty()) {

						for (auto entry : l_ColorVector) {

							wxString instance_id = l_sPlaceName + wxT("_") + entry;

							instance_id.Replace(wxT(","), "_");
							instance_id.Replace(wxT("("), "");
							instance_id.Replace(wxT(")"), "");

							wxString marking; marking << l_pcCollis->GetCell(i, 1);

							instances[instance_id] = marking;

							if (m_mPlaceInstance2Marking.find(instance_id) == m_mPlaceInstance2Marking.end()) {
								m_mPlaceInstance2Marking[instance_id] = marking;
							}

						}

					}

				}
				
		
			
			
			}

			if (l_pcCollis->GetRowCount() == 0) {
				for (auto entry : l_ColorVector) {

					wxString instance_id = l_sPlaceName + wxT("_") + entry;

					wxString marking; marking << wxT("0");

					instance_id.Replace(wxT(","), "_");
					instance_id.Replace(wxT("("), "");
					instance_id.Replace(wxT(")"), "");

					instances[instance_id] = marking;

					if (m_mPlaceInstance2Marking.find(instance_id) == m_mPlaceInstance2Marking.end()) {
						m_mPlaceInstance2Marking[instance_id] = marking;
					}
				}
			}

			//l_sMulti << l_pcCollis->GetCell(i, 1) << wxT("`") << l_pcCollis->GetCell(i, 0) << wxT("++");
			if (!l_bIsAll ) {// in case marking is not all(), we need to complete the rest of colors of instances with zeros
				if (l_ActualColorVector.size() != l_ColorVector.size()) {
					//ToDO: deal with product colors
					for (auto color : l_ColorVector) {
						if (std::find(l_ActualColorVector.begin(), l_ActualColorVector.end(), color) == l_ActualColorVector.end()) {
							wxString instance_id = l_sPlaceName + wxT("_") + color;

							instance_id.Replace(wxT(","), "_");
							instance_id.Replace(wxT("("), "");
							instance_id.Replace(wxT(")"), "");

							wxString marking = wxT("0");
							instances[instance_id] = marking;

							if (m_mPlaceInstance2Marking.find(instance_id) == m_mPlaceInstance2Marking.end()) {
								m_mPlaceInstance2Marking[instance_id] = marking;
							}
						}
					}
				}
			}
		
			instances[l_sPlaceName] = l_sColMarking;
			if (m_mPlaceInstance2Marking.find(l_sPlaceName) == m_mPlaceInstance2Marking.end())
			{
				m_mPlaceInstance2Marking[l_sPlaceName] = l_sColMarking;
			}
			
			currentStateCol2InstancesMap.color2Marking = instances;
			l_vCol2State.push_back(currentStateCol2InstancesMap);
			

		}


	}
	stateCurrent.instancePlaces = l_vCol2State;
	Traces.push_back(stateCurrent);
}

void SP_DS_ColStAnimation::RemoveIsolatedPlaces() {

/*
struct ColPlace2InstancesStates {
wxString colPlace;
SP_MapString2String color2Marking;
};


struct StepState {

double timepoint;
std::vector<ColPlace2InstancesStates> instancePlaces;

};
*/
	std::vector<wxString> l_vExcluded;

	for (auto it = m_mPlaceInstance2Marking.begin(); it != m_mPlaceInstance2Marking.end(); ++it) {

		for (long i = 1; i < Traces.size(); i++) {
			for (long j = 0; j < Traces[i].instancePlaces.size(); j++)
			{
				auto  instance2Marking = Traces[i].instancePlaces[j].color2Marking;

				auto it1 = instance2Marking.find(it->first);

				if (it1 != instance2Marking.end()) {
					if (it1->second != it->second) {
						//is not an isolated place
						l_vExcluded.push_back(it->first);
					}
				}
			}

		}
	}

	for (auto it = m_mPlaceInstance2Marking.begin(); it != m_mPlaceInstance2Marking.end(); ++it) {
		if (std::find(l_vExcluded.begin(), l_vExcluded.end(),it->first) != l_vExcluded.end()) {
			
			l_vIsolated.push_back(it->first);
			wxString isolated = wxT("isolated: ") + it->first;
			SP_LOGMESSAGE(isolated);
		}
	}

}


void SP_DS_ColStAnimation::ExportMarkings()
{
	list<SP_DS_ColStPlaceAnimator*>::iterator l_itPlace;
  
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

						if (!m_sTriggiredUnfoldedTrans.Contains(wxT("Fire"))&& (*l_itTrans)->GetChosenBinding().size() > 1)
						{
							l_sName = l_pcNameAttr->GetValue();
						}
						else
						{
							if ((*l_itTrans)->GetChosenBinding().size() == 1)
							{
								l_sName=l_pcNameAttr->GetValue();
								l_sName << wxT("_") << (*l_itTrans)->GetChosenBinding()[0];
							}
							else
							{
								l_sName = m_sTriggiredUnfoldedTrans;
							}

						}

						l_pcAttr = l_pcNode->GetAttribute(wxT("ID"));

						SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr);
						long int l_sId = l_pcIdAttr->GetValue();

						if ((*l_itTrans)->GetChosenBinding().size() ==1 && m_sTriggiredUnfoldedTrans==wxT(""))
							l_sName = wxT("Fire transition : ") + l_pcNameAttr->GetValueString() + wxT("_") + (*l_itTrans)->GetChosenBinding()[0];


						if ((*l_itTrans)->GetChosenBinding().size() > 1 && m_sTriggiredUnfoldedTrans.Freq(wxChar('_')) == 1)
							l_sName = wxT("Fire transition : ")+l_pcNameAttr->GetValueString() + wxT("_") + (*l_itTrans)->GetChosenBinding()[0];



						if ((*l_itTrans)->GetChosenBinding().size() > 1 && m_sTriggiredUnfoldedTrans.Freq(wxChar('_'))==1)
						{//tupel colors
							for (unsigned int i = 1; i < (*l_itTrans)->GetChosenBinding().size(); i++)
							{
								l_sName << wxT("_") << (*l_itTrans)->GetChosenBinding()[i];
							}

						}

						(*l_itTrans)->GetChosenBinding().clear();
						if (l_itTrans == m_lStepTransAnimators.begin())
						{
							l_temp << wxT(" | ");
						}

						else
						{
							l_temp << wxT("\n  | ");
						}


						wxString l_sTemp = l_temp;
						l_temp << l_sId << wxT(".") << l_sName;

						if (!l_temp.Contains(wxT("Fire")))
						{
							l_temp = wxT("");
							l_temp<< l_sTemp  << l_sId << wxT(".Fire Transition : ") << l_sName;
						}

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
			m_sTriggiredUnfoldedTrans = wxT("");
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

				wxString l_sToken,l_sIdToken;
				l_sToken=	l_pc_tkz.GetNextToken();
				l_sToken=l_pc_tkz.GetNextToken();
				l_sToken = l_pc_tkz.GetNextToken();
				if (l_sToken.Contains(wxT("Fire")))
				{
					l_sIdToken = l_sToken.BeforeFirst(wxChar('.'));
				}

				l_sToken = l_pc_tkz.GetNextToken();
				l_sToken = l_pc_tkz.GetNextToken();
				l_sToken = l_pc_tkz.GetNextToken();
				//Get the third token, tokenize it with "." as delimiter and take the first token from it
				//wxStringTokenizer l_pc_trans(l_pc_tkz.GetNextToken(), wxT("."));
				//wxStringTokenizer l_pc_trans(l_pc_tkz.GetNextToken(), wxT("."));
				if (l_sToken == wxT(""))
				{
					m_bInvalid = true;
				}

				m_mTransID2Color[l_nLineCount - 1] = l_sToken;

				wxString l_sTName = l_sToken;

				if (!l_sIdToken.ToLong(&l_trans_id))
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
					if (l_sColor != wxT(""))
					{
						SP_IMPORT_STATE l_state = (*l_itTrans)->CheckColour(l_sColor);
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


bool SP_DS_ColStAnimation::ImportStepSequences()
{
	bool l_bIsOk = true;
	/**
	if (m_bInvalid || m_ImportTextfile.Open(m_ImportFilename) == false)
		{
			m_lPossibleTransAnimators.clear();
			SP_MESSAGEBOX(wxT("Error in opening file ") + m_ImportFilename, wxT("Notification"), wxOK | wxICON_ERROR);
		    return false;
		}
		else
		{
			list<SP_DS_ColStTransAnimator*>::iterator l_itTrans;
			long l_trans_id;
			wxString l_stransName;

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
					{

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
	*/
	return l_bIsOk;
}
 
void SP_DS_ColStAnimation::ResetTransSequenceFile()
{
	int l_nCounter = 0;
	bool l_bIsReset = false;

	if(m_ExportFilename==wxT("")){return;}

	if (m_ExportTextfile.Open(m_ExportFilename))
	{
		while (l_nCounter <= m_ExportTextfile.GetLineCount() && m_ExportTextfile.GetLineCount() != 1)
		{

			if (l_nCounter > 1)
			{
				m_ExportTextfile.RemoveLine(l_nCounter - 1);
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
	if (l_bIsReset)
		m_ExportTextfile.Close();
}


 void SP_DS_ColStAnimation::SetSimulationStopWatch(long p_nTime)
{
	float se = (float)p_nTime / 1000;
	int secs = (int)(se);
	int hr = secs / 3600;
	int mi = (secs % 3600) / 60;
	int secsi = secs - (hr * 3600 + mi * 60);
	se -= secs;
	se += (secsi);
	wxString l_sTime;
	if (hr != 0) {
		l_sTime = wxString::Format(wxT("%d h %d m %.3f s"), hr, mi, se);
		//m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%d h %d m %.3f s"), hr, mi, se));
	}
	else if (mi != 0) {
		//m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%d m %.3f s"), mi, se));
		l_sTime = (wxString::Format(wxT("%d m %.3f s"), mi, se));
	}
	else {
		//m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), se));
		l_sTime = wxString::Format(wxT("%.3f s"), se);
	}

	wxString l_sTimeFormat = wxString::Format("%*s%s", 80, "", l_sTime);
	m_pcHintTime->SetLabel(l_sTimeFormat);
}

 wxString SP_DS_ColStAnimation::ColTransName(unsigned long p_nTranNum)
 {

	 SP_DS_ColStTransAnimator* p_pcTrans = m_mpcTransitionAnimators[p_nTranNum];

	 if (!p_pcTrans) return wxT("");

	 SP_DS_ColListAttribute* l_pcColList = nullptr;
	 SP_DS_Node* l_pcNode = p_pcTrans->GetParentNode();

	 SP_DS_Node* pc_Transnode = (SP_DS_Node*)(p_pcTrans)->GetParentNode();
	 SP_DS_Attribute* l_pcAttrTran = (pc_Transnode)->GetAttribute(wxT("Name"));
	 SP_DS_NameAttribute* l_pcNameAttrTr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttrTran);

	 return l_pcNameAttrTr->GetValueString();
 }

 wxString SP_DS_ColStAnimation::SubstituteToken(const wxString& expression, const wxString& token, const wxString& value)
 {

	 wxString result = expression;

	 size_t pos = result.Find(token);
	 size_t tokenLength = token.length();
	 size_t valueLength = value.length();

	 while (pos != wxString::npos)
	 {
		 result = result.Left(pos) + value + result.Mid(pos + tokenLength);
		 pos = result.Find(token);
		 if (pos != wxString::npos)
			 pos += valueLength;
	 }

	 return result;
 }




 bool SP_DS_ColStAnimation::IsVariable(const wxString& p_sVar)
 {
	 auto  it = m_cColorSetClass.GetVariableMap()->find(p_sVar);
	 if (it != m_cColorSetClass.GetVariableMap()->end())
	 {
		 return true;
	 }

	 return false;
 }

 bool SP_DS_ColStAnimation::EvaluatePredicate(const wxString& p_sPredicate, const wxString& p_sBinding)
 {
	 wxString l_sEmptyCheck = p_sPredicate;
	 l_sEmptyCheck.Replace(wxT(" "), wxT(""));
	 l_sEmptyCheck.Replace(wxT("\t"), wxT(""));
	 l_sEmptyCheck.Replace(wxT("\n"), wxT(""));

	 if (l_sEmptyCheck == wxT("true"))
	 {
		 return true;
	 }
	 wxString l_sSubstitued = l_sEmptyCheck;
	 wxStringTokenizer tokenizer(p_sPredicate, wxT("=><&|()+-*/"), wxTOKEN_RET_EMPTY_ALL);

	 while (tokenizer.HasMoreTokens())
	 {
		 wxString token = tokenizer.GetNextToken().Trim();
		 if (IsVariable(token))
		 {
			
			 l_sSubstitued =  SubstituteToken(l_sSubstitued, token, p_sBinding);
		 }
	 }
	 

	 std::stringstream ss(l_sSubstitued);

	 using iterator_type = boost::spirit::istream_iterator;
	 iterator_type begin(ss >> std::noskipws), end;
	 ExpressionEvaluator evaluator;
	

	 std::vector<SP_CPN_ColorSet*>* l_pcColorSetVector = m_cColorSetClass.GetColorSetVector(); //.LookupColorSet(l_sColorSetName);

	 for (int i = 0; i < l_pcColorSetVector->size(); ++i) {
		 SP_CPN_ColorSet l_pcCs = *l_pcColorSetVector->at(i);
		 SP_VectorString l_vColoValues = l_pcCs.GetStringValue();


		 if (l_pcCs.GetDataType() == CPN_ENUM) {
		 for (unsigned i = 0; i < l_vColoValues.size(); ++i) {
			 evaluator.setVariable(l_vColoValues[i], i);
		 }
	 }
		 
		  

	 }

	 ExpressionGrammar<iterator_type> grammar(evaluator);
	 double result;
	 wxString res_log;
	 bool success = boost::spirit::qi::phrase_parse(begin, end, grammar, boost::spirit::ascii::space, result);

	 if (success && begin == end)
	 {
		 return (bool)result;
	 }
	 else
	 {

		 res_log = "Parsing of the expression" + l_sSubstitued + " failed at position: ";//

	 }

	 SP_LOGERROR(res_log);

	 return false;
 }

 double SP_DS_ColStAnimation::RateFunction(unsigned long p_nTranNumber, const wxString& b) {

	 SP_DS_ColStTransAnimator* l_pcNodeTrans = m_mpcTransitionAnimators[p_nTranNumber];

	 if (l_pcNodeTrans == nullptr) return 0.0;

	 SP_DS_ColListAttribute* l_pcColList = nullptr;

	 SP_DS_Node* l_pcNode = l_pcNodeTrans->GetParentNode();

	 SP_MapString2String l_funMap = m_mpcTransitionAnimators[p_nTranNumber]->GetResolvedRateFunction();

	 double val = 0.0;
	 auto itFind = l_funMap.find(b);
	 if (itFind != l_funMap.end())
	 {
		 wxString l_sSubstituedRF;
		 wxString nosub = itFind->second;
		 SubstituteRateFunction(itFind->second, "", "", l_sSubstituedRF);

		 SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
		 auto parse_tree = l_pcFR->parseFunctionString(l_sSubstituedRF);

		 if (parse_tree == nullptr) {

			 wxString l_sLoGError = wxT("Error in parsing the expression: ") + l_sSubstituedRF;
			 SP_LOGMESSAGE(l_sLoGError);
			 return false;
		 }

		 SP_FunctionPtr l_pcFunction(parse_tree);
		 if (!l_pcFunction)
		 {
			 wxString l_sLoGError = wxT("Error in parsing the expression: ") + l_sSubstituedRF;
			 SP_LOGMESSAGE(l_sLoGError);
			 return false;
		 }

		 SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));


		 val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, val }();
	 }

	 return val;
 }


 /*get pre places of a col transition
 SP_ListEdge::const_iterator l_it;
 const SP_ListEdge* sourceEdges = l_pcNode->GetTargetEdges();
 for (l_it = sourceEdges->begin(); l_it != sourceEdges->end(); ++l_it) {
 //iterate over the pre-places of the chosen transition
 SP_DS_Node* node = (SP_DS_Node*)(*l_it)->GetSource();
 SP_DS_Attribute* l_pcAttr = (node)->GetAttribute(wxT("Name"));
 SP_DS_NameAttribute* l_pcNameAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcAttr);

 SP_VectorString l_vBind;
 l_vBind.push_back(binding);
 int state = 1;//TokenNumerOfAnimatorPlace(l_pcNameAttr->GetValueString(), l_vBind);
 l_dHaz *= state;

 }
 */

 void SP_DS_ColStAnimation::OnOpenSelectedGraphViews(wxCommandEvent &p_pc_Event)
 {

	 m_ArrayColPlaces.Clear();
	 // m_ArrayColTranstions.Clear();
	 for (unsigned int i = 0; i < m_vSelectedPlaces.size(); i++) {
		 m_ArrayColPlaces.Add(m_vSelectedPlaces[i]);
	 }
	 wxString l_sExportName = m_pcGraph->GetParentDoc()->GetFilename().BeforeFirst(wxChar('.'));
	// m_vvResultMatrix
	 SP_DLG_ColSimViewerWindow* l_pcView = new SP_DLG_ColSimViewerWindow(m_pcDialog, m_mselectedPlace2Index, m_vvResultMatrix, m_vxAxisVieverDate, l_sExportName);
	 l_pcView->Show();

 }


 bool SP_DS_ColStAnimation::LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces)
 {
	 wxString l_sName;

	 CHECK_POINTER(p_pcView, return false);

	 //get the current nodeclass type
	 SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	 CHECK_POINTER(l_pcAttribute, return false);

	 wxString l_sElementType = l_pcAttribute->GetValueString();

	 CHECK_POINTER(p_pcViewer, return false);

	 //remove old curves
	 p_pcViewer->ClearCurves();

	 //remove shown curves
	 p_pcViewer->ClearShownCurves();

	 SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcView->GetAttribute(wxT("CurveInfo")));
	 CHECK_POINTER(l_pcCurveInfoList, return false);

	 m_ArrayColPlaces.Clear();
	// m_ArrayColTranstions.Clear();
	 for (unsigned int i = 0; i < m_vSelectedPlaces.size(); i++) {
		 m_ArrayColPlaces.Add(m_vSelectedPlaces[i]);
	 }

	// for (unsigned int i = 0; i < m_asTransitionNames.size(); i++) {
	//	 m_ArrayColTranstions.Add(m_asTransitionNames[i]);
	// }

	 //CreateViewerDataFromRegex(p_pcView);
	 
	 /**
	 for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	 {
		 wxString l_sPosition = l_pcCurveInfoList->GetCell(l_nRow, 0);
		 unsigned long l_nPosition = 0;
		 if (!l_sPosition.ToULong(&l_nPosition))
		 {
			 return false;
		 }

		 wxString l_sOutType = l_pcCurveInfoList->GetCell(l_nRow, 1);

		 if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_asColPlaceNames.size()) //unfolded place
		 {
			 l_sName = m_asColPlaceNames[l_nPosition];

			 p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
		 }
		 else if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_asTransitionNames.size()) //unfolded transitions
		 {
			 l_sName = m_asTransitionNames[l_nPosition];

			 p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
		 }
		 else
		 {
			 SP_LOGERROR(wxT("Invalid node names, we stop loading the rest of the file"));

			 //invalid row index, therefore we ignore the remaining rows
			 l_pcCurveInfoList->RemoveRemainingRows(l_nRow);

			 break;
		 }

		 wxString l_sOrignialName = l_pcCurveInfoList->GetCell(l_nRow, 6);

		 if (l_sOrignialName != l_sName)
		 {
			 SP_LOGWARNING(l_sOrignialName + wxT("  ") + l_sName + wxT("Name for position (") + wxString::Format(wxT("%d"), l_nRow) + wxT(") is changed to ") + l_sName);
		 }

		 //update curve name
		 l_pcCurveInfoList->SetCell(l_nRow, 6, l_sName);
		 p_asPlaces.Add(l_sName);
	 }

	 */
	 return true;
 }