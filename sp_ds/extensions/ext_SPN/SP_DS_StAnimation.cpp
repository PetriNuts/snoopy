//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/statline.h>

#include "sp_ds/extensions/ext_SPN/SP_DS_StAnimation.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_AnimationProperties.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//by sl
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"

#include "sp_ds/animators/ani_SPN/SP_DS_StPlaceAnimator.h"
#include "sp_ds/animators/ani_SPN/SP_DS_StTransAnimator.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"

#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "spsim/helpers/stochastic/animationinfo.h"

#ifndef __WXMSW__
#include "bitmaps/play_export_icon.xpm"
#include "bitmaps/pause_export_icon.xpm"
#include "bitmaps/recording_export_icon.xpm"
#endif

enum
{

	SP_ID_STATIC_TEXT_OUTPUT_LABEL,

	SP_ID_BUTTON_OPEN_SIMULATION,

	SP_ID_CHOICE_FUNCTION_SETS,
	SP_ID_CHOICE_MARKING_SETS,
	SP_ID_CHOICE_CONSTANT_SETS,
	SP_ID_CHOICE_WEIGHT_SETS,

	SP_ID_CHOICE_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_CHOICE_SCHEDULE_SETS,//By Liu on 6 Mar. 2009

	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_CONSTANT_SETS,

	SP_ID_BUTTON_MODIFY_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_WEIGHT_SETS,

	SP_ID_GRID_GROUP,
	//by george

	SP_ID_PEDSET = 12271,
	SP_ID_PEDKEEP,
	SP_ID_PEDEXPORT,
	SP_ID_PEDIMPORT,
	SP_ID_RADIO_BUTTON_RECORD=12283,
	SP_ID_RADIO_BUTTON_REPLAY

};
BEGIN_EVENT_TABLE( SP_DS_StAnimation, SP_DS_PedAnimation)

EVT_CHOICE( SP_ID_CHOICE_FUNCTION_SETS, SP_DS_StAnimation::OnSetsChanged )
EVT_CHOICE( SP_ID_CHOICE_CONSTANT_SETS, SP_DS_StAnimation::OnConstantSetsChanged )

EVT_CHOICE( SP_ID_CHOICE_WEIGHT_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009
EVT_CHOICE( SP_ID_CHOICE_DELAY_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009
EVT_CHOICE( SP_ID_CHOICE_SCHEDULE_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009

EVT_BUTTON( SP_ID_BUTTON_OPEN_SIMULATION, SP_DS_StAnimation :: OnOpenSimulation )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_StAnimation :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DS_StAnimation :: OnModifyFunctionSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DS_StAnimation :: OnModifyConstantSets )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DS_StAnimation :: OnModifyWeightSets ) //By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DS_StAnimation :: OnModifyDelaySets ) //By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DS_StAnimation :: OnModifyScheduleSets ) //By Liu on 6 Mar. 2009

EVT_UPDATE_UI(SP_ID_CHOICE_MARKING_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_FUNCTION_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_CONSTANT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_WEIGHT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_DELAY_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_CHOICE_SCHEDULE_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_OPEN_SIMULATION, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_CONSTANT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_PEDSET, SP_DS_StAnimation::OnUpdateUI)//by george
EVT_UPDATE_UI(SP_ID_PEDKEEP, SP_DS_StAnimation::OnUpdateUI)//by george
EVT_UPDATE_UI(SP_ID_PEDEXPORT, SP_DS_StAnimation::OnUpdateUI)//george on 10.2020
EVT_UPDATE_UI(SP_ID_RADIO_BUTTON_RECORD, SP_DS_StAnimation::OnUpdateRecordReply)//by george
EVT_UPDATE_UI(SP_ID_RADIO_BUTTON_REPLAY, SP_DS_StAnimation::OnUpdateRecordReply)//george on 10.2020
EVT_BUTTON(SP_ID_RADIO_BUTTON_RECORD, SP_DS_StAnimation::OnRecord)//george on 10.2020
EVT_BUTTON(SP_ID_RADIO_BUTTON_REPLAY, SP_DS_StAnimation::OnReplay)//george on 10.2020
EVT_BUTTON(SP_ID_PEDEXPORT, SP_DS_StAnimation::OnExport) // george on 10.2020
EVT_BUTTON(SP_ID_PEDIMPORT, SP_DS_StAnimation::OnImport) // george on 10.2020

END_EVENT_TABLE()


SP_DS_StAnimation::SP_DS_StAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
SP_DS_PedAnimation( p_nRefresh, p_nDuration, p_eStepping), m_pcSimulator (NULL)
{
	//george
	m_nStepCount = 0;
	m_mGroup2Position.clear();
	m_bExportFlag = false;
	m_bImportFlag=false;
	 m_buRecord1=NULL;
	 m_buReplay1=NULL;
	// m_cbKeep=NULL;
	// m_pcSimulator=NULL;
	 m_pcOutputLabelStaticText=NULL;



}

SP_DS_StAnimation::~SP_DS_StAnimation()
{

	//by george
		 if (m_cbKeep) {
		 if (m_cbKeep->IsChecked()) {
		 KeepMarking();
		 } else {
		 list<SP_DS_PlaceAnimator*>::iterator l_Iter;
		 for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
		 (*l_Iter)->ResetMarking();
		 Refresh();
		 }
		 wxGetApp().GetAnimationPrefs()->SetKeepMarking(m_cbKeep->IsChecked());
		// wxDELETE((m_cbKeep));
		 }
		 else {

			 list<SP_DS_PlaceAnimator*>::iterator l_Iter;

			 for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
			 {
				 (*l_Iter)->ResetMarking();
				 //by sl
				 (*l_Iter)->MarkingSourceEdges();
				 (*l_Iter)->MarkingTargetEdges();
			 }

			  if(m_pcSimulator)
			 wxDELETE(m_pcSimulator);
		 }


		if(m_buRecord1)
		 wxDELETE(m_buRecord1);

		if(m_buReplay1)
		 wxDELETE(m_buReplay1);

         if(m_cbKeep)
          wxDELETE((m_cbKeep));

         if(m_pcOutputLabelStaticText)
         	wxDELETE(m_pcOutputLabelStaticText);


 	Refresh();

}

SP_DS_Animation*
SP_DS_StAnimation::Clone()
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

			return dynamic_cast<SP_DS_Animation*>(new SP_DS_StAnimation(l_nRefres, l_nDuration, SP_ANIM_STEP_T(m_nStepState)));
		}

		return dynamic_cast<SP_DS_Animation*> (new SP_DS_StAnimation(m_nRefreshFrequ, m_nStepDuration, SP_ANIM_STEP_T(m_nStepState)));
}

bool SP_DS_StAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{

	bool l_bReturn = SP_DS_PedAnimation::Initialise(p_pcGraph);

	SP_DS_TransAnimator* l_pcTrans;
	SP_DS_PlaceAnimator* l_pcPlace;

	list<SP_DS_TransAnimator*>::iterator l_Iter;
	list<SP_DS_PlaceAnimator*>::iterator l_Iter1;

	int i = 0;

	for (l_Iter = m_lAllTransAnimators.begin(); l_Iter != m_lAllTransAnimators.end(); ++l_Iter)
	{
		l_pcTrans = (*l_Iter);

		m_mpcTransitionAnimators[i++] = l_pcTrans;
	}

	for (l_Iter1 = m_lAllPlaceAnimators.begin(); l_Iter1 != m_lAllPlaceAnimators.end(); ++l_Iter1)
	{
		l_pcPlace = (*l_Iter1);
		m_mpcPlaceAnimators[l_pcPlace->GetId()] = l_pcPlace;

		(*l_Iter1)->InitCanvas();
	}

	m_pcSimulator = new spsim::Gillespie();

	if (!CheckRateFunction())
	{
		SP_LOGERROR( wxT("Initialisation failed."));
		return false;
	}

	return l_bReturn;

}

void SP_DS_StAnimation::LoadConnections()
{
	//Map transition name to their positions
	m_pcSimulator->MapTransitionNames2Positions();

	m_pcSimulator->MapPlaceNames2Positions();

	//Edge
	LoadConnectionOfType(SP_DS_EDGE, spsim::STANDARD_ARC);

	//Read Arc
	LoadConnectionOfType(SP_DS_READ_EDGE, spsim::READ_ARC);

	//Inhibitor edge
	LoadConnectionOfType(SP_DS_INHIBITOR_EDGE, spsim::INHIBITOR_ARC);

	//equal
	LoadConnectionOfType(SP_DS_EQUAL_EDGE, spsim::EQUAL_ARC);

	//reset
	LoadConnectionOfType(SP_DS_RESET_EDGE, spsim::RESET_ARC);

	//Modifier
	LoadConnectionOfType(SP_DS_MODIFIER_EDGE, spsim::MODIFIER_ARC);
}

void SP_DS_StAnimation::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
	const SP_ListEdge* l_pcEdgeList;
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sSourceNodeName, l_sDestNodeName;

	SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;


	SP_DS_NameAttribute* l_pcNameAttribute;

	CHECK_POINTER(m_pcGraph->GetEdgeclass(p_sArcType), return);

	l_pcEdgeList = m_pcGraph->GetEdgeclass(p_sArcType)->GetElements();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		//Get source Node Name
		l_pcSourceNode = dynamic_cast<SP_DS_Node*> ((*l_itEdge)->GetSource());

		CHECK_POINTER(l_pcSourceNode,return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*> (l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sSourceNodeName = l_pcNameAttribute->GetValue();

		//Get target Node Name
		l_pcTargetNode = dynamic_cast<SP_DS_Node*> ((*l_itEdge)->GetTarget());

		CHECK_POINTER(l_pcTargetNode,return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*> (l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sDestNodeName = l_pcNameAttribute->GetValue();

		//get the arc weight as a string
		wxString l_sArcWeight = GetEdgeWeight((*l_itEdge));

		//converted arc weight as a number
		double l_nNumericArcWeight = 0;

		//Get the arc Weight
		if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
		{
			if (l_sArcWeight.ToDouble(&l_nNumericArcWeight) == true)
			{
				m_pcSimulator->SetPostTransitionConnection(l_sSourceNodeName, l_sDestNodeName, l_nNumericArcWeight);
			}
			else
			{
				m_pcSimulator->SetPostSelfModifyingWeights(l_sSourceNodeName, l_sDestNodeName, l_sArcWeight);
			}

		}
		else
		{
			if (l_sArcWeight.ToDouble(&l_nNumericArcWeight) == true)
			{
				m_pcSimulator->SetPreTransitionConnection(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_nNumericArcWeight);
			}
			else
			{
				m_pcSimulator->SetPreSelfModifyingWeights(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_sArcWeight);
			}

		}
	}
}

wxString SP_DS_StAnimation::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{
	wxString l_sWeight;

	//Get the Edge weight
	SP_DS_Attribute* l_pcAtt = p_pcEdge->GetAttribute(wxT("Multiplicity"));

	CHECK_POINTER(l_pcAtt,return wxT("0.0"));

	l_sWeight = l_pcAtt->GetValueString();

	return l_sWeight;
}

void SP_DS_StAnimation::LoadPlaces()
{
	SP_VectorLong l_anCurrentMarking;
	SP_VectorStdString l_asPlaceNames;
	unsigned long l_nPosition = 0;
	wxString l_sName;

	l_anCurrentMarking.clear();
	l_asPlaceNames.clear();
	l_anCurrentMarking.clear();

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	CHECK_POINTER(l_pcNodeclass,return);

	unsigned long l_nPlaceSize = l_pcNodeclass->GetElements()->size();

	l_asPlaceNames.resize(l_nPlaceSize);

	l_anCurrentMarking.resize(l_nPlaceSize);

	for (SP_ListNode::const_iterator l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		l_sName = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_asPlaceNames[l_nPosition] = l_sName;

		SP_DS_Attribute* l_pcAttribute = (*l_itElem)->GetAttribute(wxT("Marking"));
		CHECK_POINTER(l_pcAttribute,return);

		SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*> (l_pcAttribute);
		long l_nValue = l_pcMarkAttr->GetValue(true);

		l_anCurrentMarking[l_nPosition] = l_nValue;

		l_nPosition++;
	}

	//set initial marking
	m_pcSimulator->SetInitialMarking(l_anCurrentMarking);

	//set place names
	m_pcSimulator->SetPlaceNames(l_asPlaceNames);
}
void SP_DS_StAnimation::LoadTransitions()
{
	//Stochastic Transitions
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	CHECK_POINTER(l_pcNodeclass,return);

	//Immediate Transitions
	SP_DS_Nodeclass* l_pcNodeclassforImmediateTrans;
	l_pcNodeclassforImmediateTrans = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	CHECK_POINTER(l_pcNodeclassforImmediateTrans,return);

	//Timed Transition
	SP_DS_Nodeclass* l_pcNodeclassforDeterminTrans;
	l_pcNodeclassforDeterminTrans = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	CHECK_POINTER(l_pcNodeclassforDeterminTrans,return);

	//Scheduled Transition
	SP_DS_Nodeclass* l_pcNodeclassforScheduedTrans;
	l_pcNodeclassforScheduedTrans = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	CHECK_POINTER(l_pcNodeclassforScheduedTrans,return);

	//bysl

	//SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	SP_DS_ColListAttribute* l_pcColList = NULL;

	SP_ListNode::const_iterator l_itElem;

	SP_VectorString l_asTransitionNames;

	m_pcSimulator->ClearTransitions();

	l_asTransitionNames.clear();

	//if the user wants to run the drawn net as continuous nets, then stochastic transitions need to be added as continuous
	spsim::TransitionType l_nTransitionType = spsim::TRANSITION_TYPE_STOCHASTIC;

	//Stochastic Transition
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		l_asTransitionNames.push_back(l_sTransitionName);

		m_pcSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, l_nTransitionType);
	}

	//get the constant list
	//l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewMetadata->GetAttribute(wxT("ConstantList")));


	//Immediate Transition
	for (l_itElem = l_pcNodeclassforImmediateTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforImmediateTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

		l_asTransitionNames.push_back(l_sTransitionName);

		m_pcSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, spsim::TRANSITION_TYPE_IMMEDIATE);
	}

	//Timed Transition
	for (l_itElem = l_pcNodeclassforDeterminTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforDeterminTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;
		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("DelayList")));

		wxString l_delayvalue = l_pcColList->GetActiveCellValue(1);

		l_asTransitionNames.push_back(l_sTransitionName);
		m_pcSimulator->AddTransition(l_sTransitionName, l_delayvalue, spsim::TRANSITION_TYPE_DETERMINISTIC);
	}

	//Scheduled transition
	for (l_itElem = l_pcNodeclassforScheduedTrans->GetElements()->begin(); l_itElem != l_pcNodeclassforScheduedTrans->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcNode = *l_itElem;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*> (l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("PeriodicList")));
		wxString l_Begin = l_pcColList->GetActiveCellValue(1);
		wxString l_Repetition = l_pcColList->GetActiveCellValue(2); //FixedTimedFiring_Single( . ) not realized
		wxString l_End = l_pcColList->GetActiveCellValue(3); //FixedTimedFiring_Periodic( _SimStart, . , _SimEnd )
		wxString l_sTransitionFunction = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");

		l_asTransitionNames.push_back(l_sTransitionName);

		m_pcSimulator->AddTransition(l_sTransitionName, l_sTransitionFunction, spsim::TRANSITION_TYPE_SCHEDULED);
	}
}

void SP_DS_StAnimation::LoadParameters()
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorStdString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if(l_FE.IsOk())
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
				if(l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{l_pcFR, l_Function}();
				}
				else if(l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_Function}();
				}
			}
			//****************************************************************************************
			l_asParameterNames.push_back(l_sName);
			l_anParameterValue.push_back(l_nValue);
		}
	}

	m_pcSimulator->SetParameterNames(l_asParameterNames);
	m_pcSimulator->SetParameterValues(l_anParameterValue);
}
bool SP_DS_StAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer)
{
	if (!p_pcCtrl || !p_pcSizer)
	{
		return FALSE;
	}

	m_pcDialog = p_pcCtrl;

	wxSizer* l_pcOutputLabelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcSetsSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcSimulationControlSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	//////////
	//by george
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDSET, wxT("Keep Marking")), 1, wxALL | wxEXPAND, 5);
	m_cbKeep = new wxCheckBox(p_pcCtrl, SP_ID_PEDKEEP, wxT("Always keep marking when closing."));
	m_cbKeep->SetValue(wxGetApp().GetAnimationPrefs()->GetKeepMarking());
	p_pcSizer->Add(m_cbKeep, 0, wxALL, 5);
	////////////s////////
	//by george
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	wxString l_tmp;
	l_tmp << m_nStepCount;
	m_pcStepCounter = new wxBoxSizer(wxHORIZONTAL);
	m_pcStepCounterText = new wxStaticText(p_pcCtrl, wxID_ANY, wxT("Step Counter"));
	m_pcStepCounterValue = new wxStaticText(p_pcCtrl, wxID_ANY, l_tmp);

	m_pcStepCounter->Add(m_pcStepCounterText, 1, wxEXPAND | wxALL, 5);
	m_pcStepCounter->Add(m_pcStepCounterValue, 1, wxEXPAND | wxALL, 5);

	p_pcSizer->Add(m_pcStepCounter, 0, wxEXPAND);


	///p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	///////////////////
	  m_pcOutputLabelStaticText = new wxStaticText(p_pcCtrl, SP_ID_STATIC_TEXT_OUTPUT_LABEL, wxT("Start ..."), wxDefaultPosition, wxDefaultSize);

	l_pcRowSizer->Add(m_pcOutputLabelStaticText, 1, wxALL, 5);
	l_pcOutputLabelSizer->Add(l_pcRowSizer, 0, wxEXPAND);
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	wxSizer*  m_pcExportImportSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDEXPORT, wxT("Export")), 1, wxALL | wxEXPAND, 5);
	m_pcExportImportSizer->Add(new wxButton(p_pcCtrl, SP_ID_PEDIMPORT, wxT("Import")), 1, wxALL | wxEXPAND, 5);
	p_pcSizer->Add(m_pcExportImportSizer, 0, wxALL, 5);

	//p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
#if defined(__WXMSW__)
	m_bitmapicons1[0] = wxBitmap(wxT("BITMAP_ANIM_RECORD_TRACE"), wxBITMAP_TYPE_BMP_RESOURCE);
	m_bitmapicons1[1] = wxBitmap(wxT("BITMAP_ANIM_REPLAY_TRACE"), wxBITMAP_TYPE_BMP_RESOURCE);
	m_bitmapicons1[2] = wxBitmap(wxT("BITMAP_ANIM_PAUSE_RECORD"), wxBITMAP_TYPE_BMP_RESOURCE);

#else // !WIN
	m_bitmapicons1[0] = wxBitmap(recording_icon);
	m_bitmapicons1[1] = wxBitmap(pause_icon);
	m_bitmapicons1[2] = wxBitmap(play_icon);
#endif // !WIN

	m_buRecord1 = new wxBitmapButton(p_pcCtrl, SP_ID_RADIO_BUTTON_RECORD,
		m_bitmapicons1[0], wxDefaultPosition, wxDefaultSize);
	m_buRecord1->SetToolTip(wxT("Record the trace to an external file"));
	m_buRecord1->Enable(true);
	m_buReplay1 = new wxBitmapButton(p_pcCtrl, SP_ID_RADIO_BUTTON_REPLAY,
		m_bitmapicons1[2], wxDefaultPosition, wxDefaultSize);
	m_buReplay1->SetToolTip(wxT("Replay the animation from an external trace file"));
	m_buReplay1->Enable(true);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(m_buRecord1, wxSizerFlags(0).Border(wxALL, 10));
	l_pcRowSizer->Add(m_buReplay1, wxSizerFlags(0).Border(wxALL, 10));
	p_pcSizer->Add(l_pcRowSizer);

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	//-------------------------------------------

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////

	if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0)
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Marking overview:")), wxSizerFlags(1).Expand().Border(wxALL, 5));
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("")), wxSizerFlags(1).Expand().Border(wxALL, 5));
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 5));
		l_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 5));
	}
	if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0)
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100, -1)));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 0, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);
		l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

		m_mGroup2Position[wxT("Rate")] = m_apcComboBoxes.size() - 1;
	}

	if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0)
	{//weitht
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Weight set:")), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_WEIGHT_SETS, wxDefaultPosition, wxSize(100, -1)));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 0, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_WEIGHT_SETS, wxT("Modify")), 0, wxALL, 5);
		l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

		m_mGroup2Position[wxT("Weight")] = m_apcComboBoxes.size() - 1;
	}

	if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0)
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Delay set:")), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_DELAY_SETS, wxDefaultPosition, wxSize(100, -1)));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 0, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), 0, wxALL, 5);
		l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

		m_mGroup2Position[wxT("Delay")] = m_apcComboBoxes.size() - 1;
	}

	if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0)
	{
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Schedule set:")), 1, wxALL | wxEXPAND, 5);
		m_apcComboBoxes.push_back(new wxChoice(p_pcCtrl, SP_ID_CHOICE_SCHEDULE_SETS, wxDefaultPosition, wxSize(100, -1)));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 0, wxALL, 5);
		l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, wxT("Modify")), 0, wxALL, 5);
		l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

		m_mGroup2Position[wxT("Schedule")] = m_apcComboBoxes.size() - 1;
	}

	UpdateChoices();
	SP_SetString::iterator l_itChoice;
	for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
		wxString l_sGroup = *l_itChoice;
		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add( new wxStaticText( p_pcCtrl, -1, l_sGroup + wxT(':') ), 1, wxALL | wxEXPAND, 5 );
		m_apcComboBoxes.push_back(new wxChoice( p_pcCtrl, SP_ID_CHOICE_CONSTANT_SETS , wxDefaultPosition, wxSize(100,-1), 0, NULL, 0, wxDefaultValidator, l_sGroup ));
		l_pcRowSizer->Add( m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5 );
		l_pcRowSizer->Add( new wxButton( p_pcCtrl, SP_ID_BUTTON_MODIFY_CONSTANT_SETS, wxT("Modify") ), 0, wxALL, 5 );
		l_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND);

		m_mGroup2Position[l_sGroup] = m_apcComboBoxes.size() - 1;
	}

	l_pcSimulationControlSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_OPEN_SIMULATION, wxT("Stochastic simulation"), wxDefaultPosition, wxDefaultSize, 0), 0, wxALL, 5);

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(l_pcOutputLabelSizer, 0, wxALL, 5);
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(l_pcSetsSizer, 0, wxALL, 5);
	p_pcSizer->Add(new wxStaticLine(p_pcCtrl, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND);
	p_pcSizer->Add(l_pcSimulationControlSizer, 0, wxALL, 5);

	LoadSets();
	LoadCurrentMarking();

	p_pcCtrl->PushEventHandler(this);
	m_pcDialog->EnableBackStepping(false);

	return TRUE;

}

void SP_DS_StAnimation::LoadSets()
{
	SP_DS_Node* l_pcNode = NULL;
	m_apcColListAttr.clear();

	m_apcColListAttr.resize(m_apcComboBoxes.size());

	auto itFind = m_mGroup2Position.find(wxT("Rate"));

	if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0 && itFind!= m_mGroup2Position.end())
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
		m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList")));
		//m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList"))));
	}

	itFind = m_mGroup2Position.find(wxT("Weight"));

	if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0 && itFind!= m_mGroup2Position.end())
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->front();
		m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList")));
		//m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList"))));
	}

	 itFind = m_mGroup2Position.find(wxT("Delay"));

	if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0 && itFind!= m_mGroup2Position.end())
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->front();
		m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("DelayList")));
		//m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("DelayList"))));
	}


	itFind = m_mGroup2Position.find(wxT("Scheduled"));

	if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0 && itFind!= m_mGroup2Position.end())
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->front();
		m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("PeriodicList")));
		//m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("PeriodicList"))));
	}


	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_ListMetadata::const_iterator l_itElem;

	SP_SetString::iterator l_itChoice;
	for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
		wxString l_sChoice = *l_itChoice;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			if (l_sChoice == l_sGroup)
			{
				itFind = m_mGroup2Position.find(l_sGroup);

				if (itFind != m_mGroup2Position.end())
				{
				   //m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				   m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
				   break;
				}
			}
		}
	}

	//assign the active sets for each group
	for (auto itMap = m_mGroup2Position.begin(); itMap != m_mGroup2Position.end(); ++itMap)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[itMap->second];

		if (!l_pcAttr) continue;

		wxChoice* l_pcCombobox = m_apcComboBoxes[itMap->second];

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

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;
}

bool SP_DS_StAnimation::OnDialogOk()
{
	if (!m_pcChoice)
		return FALSE;

	m_nStepState = m_pcChoice->GetSelection() + 1;

	return SP_DS_Animation::OnDialogOk();
}

void SP_DS_StAnimation::OnReset()
{
	m_bRestartAnimationFlag = true;
	m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));

	list<SP_DS_PlaceAnimator*>::iterator l_Iter;
	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->ResetMarking();
		//by sl
		(*l_Iter)->MarkingSourceEdges();
		(*l_Iter)->MarkingTargetEdges();
	}
	m_llHistoryTransAnimators.clear();

	m_nStepCount = 0;
	SetStepCounter();
	m_nLineCount = 1;
	ResetTransSequenceFile();
	//by sl
	LoadCurrentMarking();
	Refresh();

}

bool SP_DS_StAnimation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	if (!SP_DS_Animation::AddToDialog(p_pcDlg, p_pcSizer))
		return FALSE;

	wxString l_asChoices[] = { wxT("Maximum"), wxT("Intermediate"), wxT("Single") };
	m_pcChoice = new wxRadioBox(p_pcDlg, -1, wxT("Stepping"), wxDefaultPosition, wxDefaultSize, 3, l_asChoices, 1, wxRA_SPECIFY_ROWS);

	m_pcChoice->SetSelection(m_nStepState - 1);

	p_pcSizer->Add(m_pcChoice, 0, wxEXPAND | wxALL, 5);

	return TRUE;
}

void SP_DS_StAnimation::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Transition"), m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyWeightSets( wxCommandEvent& p_cEvent )
{
	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Immediate Transition"), m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyDelaySets( wxCommandEvent& p_cEvent )
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyScheduleSets( wxCommandEvent& p_cEvent )
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), m_pcDialog);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void SP_DS_StAnimation::LoadCurrentMarking()
{
	if (m_lAllPlaceAnimators.size() <= 0)
	{
		return;
	}

	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	SP_DS_Node* l_pcNode;
	SP_DS_Attribute* l_pcAttr;

	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		l_pcNode = (*l_itPlace)->GetNode();
		l_pcAttr = l_pcNode->GetAttribute(wxT("Marking"));

		SP_DS_MarkingAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_MarkingAttribute*> (l_pcAttr);
		long l_nLong = l_pcMarkAttr->GetValue(true);

		(*l_itPlace)->SetMarking(l_nLong);

		//default value
		(*l_itPlace)->SetDefaultMarking(l_nLong);

		l_pcAttr->Update(true);

		Refresh();
	}
}

void SP_DS_StAnimation::OnSetsChanged(wxCommandEvent& p_cEvent)
{

	SP_DS_PedAnimation::OnConstantSetsChanged(p_cEvent);//
	m_bRestartAnimationFlag = true;

}

void SP_DS_StAnimation::OnConstantSetsChanged(wxCommandEvent& p_cEvent)
{
	SP_DS_PedAnimation::OnConstantSetsChanged(p_cEvent);

	//m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;
}

bool SP_DS_StAnimation::ReduceTransitions()
{

	if (dynamic_cast<SP_DS_TransAnimator*> (m_pcSingleStep))
	{
		return TRUE;
	}
	else
		if (dynamic_cast<SP_DS_CoarseTransAnimator*> (m_pcSingleStep))
		{
			SP_DS_PedAnimation::ReduceTransitions();
			return TRUE;
		}

	list<SP_DS_TransAnimator*>::iterator l_itTrans;

	SP_DS_TransAnimator* l_pcChosenTransition = NULL;

	if (m_lStepTransAnimators.size() > 0)
	{
		int l_nChooseTransition = ChooseTransition();
		if (l_nChooseTransition >= 0)
			l_pcChosenTransition = m_mpcTransitionAnimators[l_nChooseTransition];

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
	}

	return TRUE;

}

int SP_DS_StAnimation::ChooseTransition()
{

	wxString l_sOutputLabel = wxT("");
	list<SP_DS_TransAnimator*>::iterator l_itTrans;

	SetCurrentMarking();

	SP_DS_StAnimationRunElem* l_pcRunElem = NULL;

	spsim::AnimationInfo* l_pcAnimationInfo = m_pcSimulator->GetNextAnimationRunElem();

	if (l_pcAnimationInfo != NULL)
	{
		l_pcRunElem = new SP_DS_StAnimationRunElem();

		l_pcRunElem->timepoint = l_pcAnimationInfo->GetCurrentTime();

		l_pcRunElem->transitionArrayPos = l_pcAnimationInfo->GetTransitionPosition();

		l_pcRunElem->actionDescription->push_back(l_pcAnimationInfo->GetActionDescription());

		l_pcRunElem->type = (SP_DS_StAnimationRunElemType) (l_pcAnimationInfo->GetActionType());
	}

	if (!l_pcRunElem)
	{
		return -1;
	}

	if (l_pcRunElem->type == SP_ST_ANIM_RUN_FIRE_TRANSITION)
	{
		l_sOutputLabel << wxString::Format(wxT("%f"), l_pcRunElem->timepoint) << wxT("  :  ") << *l_pcRunElem->actionDescription->begin();

		m_pcOutputLabelStaticText->SetLabel(l_sOutputLabel);

		int l_nReturn = l_pcRunElem->transitionArrayPos;

		//has to be deleted, because of new in m_pcSimulator->GetNextAnimationRunElem()
		wxDELETE(l_pcRunElem);

		return l_nReturn;
	}

	return -1;

}

bool SP_DS_StAnimation::PreStep()
{
	list<list<SP_DS_TransAnimator*> >::iterator l_itHistoryCurrent;
	list<SP_DS_TransAnimator*>::iterator l_itTrans;
	list<SP_DS_PlaceAnimator*>::iterator l_itPlace;
	unsigned int l_nSteps = m_nSteps / 2;

	if (GetDirection() == FORWARD)
		m_nStepCount++; //Increment the step count

	/*
	 * by sl
	 * for markingdepended edges
	 */

	// setting the right edge value
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
	{
		(*l_itPlace)->MarkingSourceEdges();
		(*l_itPlace)->MarkingTargetEdges();
	}

	if (m_bExport == true && m_bExportFlag)//by george
		ExportMarkings();

	m_lPossibleTransAnimators.clear();

	if (!m_bImportFlag)
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
				list<SP_DS_TransAnimator*>& l_rlHistory = *l_itHistoryCurrent;
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
		else if (m_bImportFlag)
			if (!ImportStepSequences())
					{
						if (m_bImport)
						{
							m_nStepCount--; m_nLineCount--;
						}
						int choose = SP_MESSAGEBOX(wxT("No more entries in the file. Close the imported file?"), wxT("Notification"), wxOK | wxNO | wxICON_INFORMATION);
						if (choose == 2)
						{
							m_bImport = false;
							// m_pcRadioBox->Enable(1, false);
						}
						ResetReplayButton();
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

					}
	//running the usual tests and preparing everything
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
		(*l_itTrans)->InformPrePlaces();
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		(*l_itPlace)->TestMarking();
	for (l_itTrans = m_lPossibleTransAnimators.begin(); l_itTrans != m_lPossibleTransAnimators.end(); ++l_itTrans)
		(*l_itTrans)->TestConcession();
	for (l_itPlace = m_lAllPlaceAnimators.begin(); l_itPlace != m_lAllPlaceAnimators.end(); ++l_itPlace)
		(*l_itPlace)->ResolveConflicts1();

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

	if (!l_bReturn && m_ImportFilename == wxT(""))
	{
		m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
		wxString msg;
		if (m_bOneShot)
		{
			if (GetSingleStep())
			{
				msg = wxT("This transition is not enabled!");

				if (m_nStepCount > 0)
					m_nStepCount--; //This transition was counted as a step, which is actually not possible
			}
			else
			{
				msg = wxT("Dead state: there are no enabled transitions");
				if (m_nStepCount > 0)
					m_nStepCount--; //This transition was counted as a step, which is actually not possible
			}
		}
		else
		{
			msg = wxT("Dead state: there are no more enabled transitions");

			if (m_nStepCount > 0)
				m_nStepCount--; //Dead state, thus decrement the m_nStepCount
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

	if (m_bExport == true && m_bExportFlag)
	ExportStepSequences();

	//This is to be put after ExportMarkings() & ExportStepSequences() are called.
	if (GetDirection() == BACKWARD && m_nStepCount > 0)
	{
		m_nStepCount--; //Decrement the step count
		m_bExport = false; //If user interrupts the animation,export should be stopped
	}

	SetStepCounter();//by george
	return l_bReturn;
}

void SP_DS_StAnimation::SetCurrentMarking()
{

	SP_VectorLong& l_anCurrentMarking = m_pcSimulator->GetCurrentMarking();

	map<int, SP_DS_PlaceAnimator*>::iterator l_iter;
	int l_nPos = 0;

	for (l_iter = m_mpcPlaceAnimators.begin(); l_iter != m_mpcPlaceAnimators.end(); l_iter++)
	{
		l_anCurrentMarking[l_nPos++] = l_iter->second->GetMarking();
	}

}

void SP_DS_StAnimation::UpdateCurrentMarking()
{

	SP_VectorLong& l_anCurrentMarking = m_pcSimulator->GetCurrentMarking();

	map<int, SP_DS_PlaceAnimator*>::iterator l_iter;
	int l_nPos = 0;

	for (l_iter = m_mpcPlaceAnimators.begin(); l_iter != m_mpcPlaceAnimators.end(); l_iter++)
	{

		l_iter->second->SetMarking(l_anCurrentMarking[l_nPos++]);

	}

}

bool SP_DS_StAnimation::StartTimer()
{

	/*  m_pcOutputLabelStaticText->SetLabel( wxT("Animation is temporarily deactivated.") );
	 return false;
	 */
	if (m_bRestartAnimationFlag)
	{

		m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
		m_bRestartAnimationFlag = false;

		//LoadCurrentMarking();

		for (size_t i = 0; i < m_apcColListAttr.size(); i++)
		{
			if (m_apcColListAttr[i])
			{
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
			}
		}

		m_pcSimulator->SetOutputStartPoint(0);
		m_pcSimulator->SetOutputEndPoint(1000);

		//Initialize the simulator
		LoadPlaces();
		LoadTransitions();
		LoadConnections();
		LoadParameters();

		if (!m_pcSimulator->Initialise(true))
		{
			StopTimer();
			m_bRestartAnimationFlag = true;

			return false;

		}

	}

	return SP_DS_Animation::StartTimer();

}

void SP_DS_StAnimation::OnOpenSimulation(wxCommandEvent& p_cEvent)
{
	//hide Animationdialog because of wxSTAY_ON_TOP
	m_pcDialog->Show(false);

	SP_DLG_StSimulationResults* l_pcDlg = new SP_DLG_StSimulationResults(m_pcGraph, NULL);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();
	//show Animationdialog after Simulationresults close
	m_pcDialog->Show(true);

	LoadSets();

	m_bRestartAnimationFlag = true;
	LoadCurrentMarking();
}

bool SP_DS_StAnimation::CheckRateFunction()
{
	list<SP_DS_TransAnimator*>::iterator l_Iter;
	SP_DS_StParser l_cParser;

	bool l_bError = false;
	for (l_Iter = m_lAllTransAnimators.begin(); l_Iter != m_lAllTransAnimators.end(); ++l_Iter)
	{
		SP_DS_TransAnimator* l_pcTrans = (*l_Iter);
		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*> (l_pcTrans->GetAnimObject());
		if (!l_pcTransNode)
			continue;

		wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*> (l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		wxString l_sNodeClassName = l_pcTransNode->GetNodeclass()->GetName();

		SP_DS_ColListAttribute* l_pcColList = NULL;

		if (l_sNodeClassName == wxT("Transition"))
		{
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcTransNode->GetAttribute(wxT("FunctionList")));
		}
		else
			if (l_sNodeClassName == wxT("Immediate Transition"))
			{
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcTransNode->GetAttribute(wxT("FunctionList")));
			}
			else
				if (l_sNodeClassName == wxT("Deterministic Transition"))
				{
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcTransNode->GetAttribute(wxT("DelayList")));
				}
				else
					if (l_sNodeClassName == wxT("Scheduled Transition"))
					{
						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcTransNode->GetAttribute(wxT("PeriodicList")));
					}
		if (!l_pcColList)
			continue;

		wxString l_sFormula = wxT("");
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if (l_sNodeClassName == wxT("Transition"))
			{

				l_sFormula = l_pcColList->GetCell(i, 1);
			}
			if (l_sNodeClassName == wxT("Immediate Transition"))
			{
				l_sFormula = wxT("ImmediateFiring(") + l_pcColList->GetCell(i, 1) + wxT(")");
			}
			if (l_sNodeClassName == wxT("Deterministic Transition"))
			{
				l_sFormula = wxT("TimedFiring(") + l_pcColList->GetCell(i, 1) + wxT(")");

			}
			if (l_sNodeClassName == wxT("Scheduled Transition"))
			{
				wxString l_Begin = l_pcColList->GetCell(i, 1);
				wxString l_Repetition = l_pcColList->GetCell(i, 2);
				wxString l_End = l_pcColList->GetCell(i, 3);

				l_sFormula = wxT("FixedTimedFiring_Periodic(") + l_Begin + wxT(",") + l_Repetition + wxT(",") + l_End + wxT(")");
			}

			wxArrayString* l_plMissingVariableList = 0;
			if (!(l_cParser.CheckFormulaFunction(l_sFormula, l_pcTransNode)))
			{
				l_plMissingVariableList = l_cParser.GetMissingVariableList();

				wxString l_sVariables = wxT("The formula \"");
				l_sVariables += l_sFormula;
				l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

				for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
				{
					l_sVariables += (*l_plMissingVariableList)[i];
					l_sVariables += wxT("\n");
				}

				l_sVariables = l_sTransitionName + wxT(": ") + l_sVariables;
				//SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR );
				SP_LOGERROR( l_sVariables);
				l_bError = true;
				//return false;
			}
		}
	}

	return !l_bError;
}

//bysl
void SP_DS_StAnimation::UpdateChoices()
{
	m_choices.clear();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(mc)
	{
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata = NULL;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Group"));
			if(l_pcAttr)
			{
				wxString l_sGroup = l_pcAttr->GetValueString();
				m_choices.insert(l_sGroup);
			}
		}
	}
}

void SP_DS_StAnimation::SetStepCounter()
{
	m_pcStepCounterValue->SetLabel(wxString::Format(wxT("%ld"), m_nStepCount));
}

void SP_DS_StAnimation::OnExport(wxCommandEvent &p_pc_Event)
{
	ExportSPN *export_frame = new  ExportSPN(wxT("Export Details"),this);

	export_frame->Show(true);
}

void SP_DS_StAnimation::OnImport(wxCommandEvent &p_pcEvent)
{
	ImportSPN *import_frame = new ImportSPN(wxT("Import Details"), this);

	import_frame->Show(true);
}

void SP_DS_StAnimation::ExportDetails(ExportSPN *export_frame)
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

	if (m_bExport)
	{
	  m_bImport = false;
	  m_bImportFlag = false;
	}
}

void SP_DS_StAnimation::ExportMarkings()
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

void SP_DS_StAnimation::ImportDetails(ImportSPN *import_frame)
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
				m_nStepCount = 0;
				m_bImport = true;
				m_bExport = false;
				m_ExportFilename = wxT("");
				ResetRecordButton();
			}
		}
	}
}

bool SP_DS_StAnimation::ImportStepSequences()
{
	if (m_bInvalid || m_ImportTextfile.Open(m_ImportFilename) == false)
	{
		m_lPossibleTransAnimators.clear();
		SP_MESSAGEBOX(wxT("Error in opening file ") + m_ImportFilename, wxT("Notification"), wxOK | wxICON_ERROR);
		m_bImport = false;
		ResetReplayButton();

		return false;
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
		else{
			return false;
		}
		m_ImportTextfile.Close();

	}
	return true;
}

void SP_DS_StAnimation::ExportStepSequences()
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


void SP_DS_StAnimation::ResetTransSequenceFile()
{
	if (m_ExportFilename == wxT("") ||!m_bExport) return;
	int l_nCounter = 0;
	bool l_bIsReset = false;
	if (m_ExportTextfile.Open(m_ExportFilename))
	{
		int choose = SP_MESSAGEBOX(wxT("The recorded file will be Overwriten, Do you want to overwitten?"), wxT("Overwrite"), wxOK | wxNO | wxICON_INFORMATION);
		if (choose == 8)//no choice
		{
			return;
		}
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
	{
		m_ExportTextfile.Close();
		SP_LOGMESSAGE(wxT("The recording file has been reset"));
	}
}

void SP_DS_StAnimation::OnReplay(wxCommandEvent& p_cEvent)
{
	if (m_bImport)
	{
		m_bImportFlag = !m_bImportFlag;
		if (m_bImportFlag)
		{
			m_buReplay1->SetBitmapLabel(m_bitmapicons1[1]);
			m_buReplay1->SetToolTip(wxT("Pause Animation from the external file"));
			m_buReplay1->Refresh();
		}
		else
		{
			m_buReplay1->SetBitmapLabel(m_bitmapicons1[2]);
			m_buReplay1->SetToolTip(wxT("Replay Animation from the external file"));
			m_buReplay1->Refresh();
		}
	}
}


void SP_DS_StAnimation::OnRecord(wxCommandEvent &p_pc_Event)
{
	if (m_bExport)
	{
		m_bExportFlag = !m_bExportFlag;
		if (m_bExportFlag)
		{
			m_buRecord1->SetBitmapLabel(m_bitmapicons1[1]);
			m_buRecord1->SetToolTip(wxT("Pause trace recording"));
			m_buRecord1->Refresh();
		}
		else
		{
			m_buRecord1->SetBitmapLabel(m_bitmapicons1[0]);
			m_buRecord1->SetToolTip(wxT("Satrt trace recording"));
			m_buRecord1->Refresh();
		}
	}
}

void SP_DS_StAnimation::ResetRecordButton()
{
	m_buRecord1->SetBitmapLabel(m_bitmapicons1[0]);
	m_buRecord1->Refresh();
	m_bExportFlag = false;
}

void SP_DS_StAnimation::ResetReplayButton()
{
	m_buReplay1->SetBitmapLabel(m_bitmapicons1[2]);
	m_buReplay1->Refresh();
	m_bImportFlag = false;
}

void SP_DS_StAnimation::OnUpdateRecordReply(wxUpdateUIEvent& p_cEvent)
{
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
			if (m_bImport)
			{
				p_cEvent.Enable(true);
			}
		}
		break;
	}
}
