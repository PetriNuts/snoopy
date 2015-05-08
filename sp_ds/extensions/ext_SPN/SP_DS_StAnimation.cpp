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
#include "sp_defines.h"
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "spsim/helpers/stochastic/animationinfo.h"

enum
{

	SP_ID_STATIC_TEXT_OUTPUT_LABEL,

	SP_ID_BUTTON_OPEN_SIMULATION,

	SP_ID_COMBOBOX_FUNCTION_SETS, SP_ID_COMBOBOX_MARKING_SETS, SP_ID_COMBOBOX_PARAMETER_SETS, SP_ID_COMBOBOX_WEIGHT_SETS,

	SP_ID_COMBOBOX_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_COMBOBOX_SCHEDULE_SETS,//By Liu on 6 Mar. 2009

	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,

	SP_ID_BUTTON_MODIFY_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_WEIGHT_SETS,

	SP_ID_GRID_GROUP
//bysl

};
BEGIN_EVENT_TABLE( SP_DS_StAnimation, SP_DS_PedAnimation)

EVT_COMBOBOX( SP_ID_COMBOBOX_MARKING_SETS, SP_DS_StAnimation::OnMarkingSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_FUNCTION_SETS, SP_DS_StAnimation::OnSetsChanged )
//EVT_COMBOBOX( SP_ID_COMBOBOX_PARAMETER_SETS, SP_DS_StAnimation::OnSetsChanged )

EVT_COMBOBOX( SP_ID_COMBOBOX_WEIGHT_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009
EVT_COMBOBOX( SP_ID_COMBOBOX_DELAY_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009
EVT_COMBOBOX( SP_ID_COMBOBOX_SCHEDULE_SETS, SP_DS_StAnimation::OnSetsChanged ) //By Liu on 6 Mar. 2009

EVT_BUTTON( SP_ID_BUTTON_OPEN_SIMULATION, SP_DS_StAnimation :: OnOpenSimulation )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_StAnimation :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DS_StAnimation :: OnModifyFunctionSets )
//EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DS_StAnimation :: OnModifyParameterSets )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DS_StAnimation :: OnModifyWeightSets ) //By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DS_StAnimation :: OnModifyDelaySets ) //By Liu on 6 Mar. 2009
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DS_StAnimation :: OnModifyScheduleSets ) //By Liu on 6 Mar. 2009

EVT_UPDATE_UI(SP_ID_COMBOBOX_MARKING_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_COMBOBOX_FUNCTION_SETS, SP_DS_StAnimation::OnUpdateUI)
//EVT_UPDATE_UI(SP_ID_COMBOBOX_PARAMETER_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_COMBOBOX_WEIGHT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_COMBOBOX_DELAY_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_COMBOBOX_SCHEDULE_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_OPEN_SIMULATION, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DS_StAnimation::OnUpdateUI)
//EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DS_StAnimation::OnUpdateUI)
EVT_UPDATE_UI(SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DS_StAnimation::OnUpdateUI)


END_EVENT_TABLE()


SP_DS_StAnimation::SP_DS_StAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping):
SP_DS_PedAnimation( p_nRefresh, p_nDuration, p_eStepping), m_pcSimulator (NULL)
{
}

SP_DS_StAnimation::~SP_DS_StAnimation()
{
	/*
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
	 wxDELETE((m_cbKeep));
	 }
	 */

	list<SP_DS_PlaceAnimator*>::iterator l_Iter;

	for (l_Iter = m_lAllPlaceAnimators.begin(); l_Iter != m_lAllPlaceAnimators.end(); ++l_Iter)
	{
		(*l_Iter)->ResetMarking();
		//by sl
		(*l_Iter)->MarkingSourceEdges();
		(*l_Iter)->MarkingTargetEdges();
	}

	wxDELETE(m_pcSimulator);

	Refresh();

}

SP_DS_Animation*
SP_DS_StAnimation::Clone()
{

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
	;

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
	SP_VectorString l_asPlaceNames;
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

	SP_VectorString l_asParameterNames;
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

	m_pcOutputLabelStaticText = new wxStaticText(p_pcCtrl, SP_ID_STATIC_TEXT_OUTPUT_LABEL, wxT("Start ..."), wxDefaultPosition, wxDefaultSize);

	l_pcRowSizer->Add(m_pcOutputLabelStaticText, 1, wxALL, 5);
	l_pcOutputLabelSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxComboBox(p_pcCtrl, SP_ID_COMBOBOX_FUNCTION_SETS, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);
	l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	//weitht
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Weight set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxComboBox(p_pcCtrl, SP_ID_COMBOBOX_WEIGHT_SETS, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_WEIGHT_SETS, wxT("Modify")), 0, wxALL, 5);
	l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Delay set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxComboBox(p_pcCtrl, SP_ID_COMBOBOX_DELAY_SETS, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), 0, wxALL, 5);
	l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(p_pcCtrl, -1, wxT("Schedule set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxComboBox(p_pcCtrl, SP_ID_COMBOBOX_SCHEDULE_SETS, wxT(""), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(p_pcCtrl, SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, wxT("Modify")), 0, wxALL, 5);
	l_pcSetsSizer->Add(l_pcRowSizer, 0, wxEXPAND);

	UpdateChoices();
	SP_SetString::iterator l_itChoice;
	for(l_itChoice = m_choices.begin(); l_itChoice != m_choices.end(); ++l_itChoice)
	{
		wxString l_sGroup = *l_itChoice;
		l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pcRowSizer->Add( new wxStaticText( p_pcCtrl, -1, l_sGroup + wxT(':') ), 1, wxALL | wxEXPAND, 5 );
		m_apcComboBoxes.push_back(new wxComboBox( p_pcCtrl, SP_ID_COMBOBOX_MARKING_SETS , wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY, wxDefaultValidator, l_sGroup ));
		l_pcRowSizer->Add( m_apcComboBoxes[m_apcComboBoxes.size()-1], 0, wxALL, 5 );
		l_pcRowSizer->Add( new wxButton( p_pcCtrl, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify") ), 0, wxALL, 5 );
		l_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND);
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

	if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}
	if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("FunctionList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}
	if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("DelayList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
	}
	if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0)
	{
		l_pcNode = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->front();
		m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("PeriodicList"))));
	}
	else
	{
		m_apcColListAttr.push_back(NULL);
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
				m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				break;
			}
		}
	}

	for (size_t j = 0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		wxComboBox* l_pcCombobox = m_apcComboBoxes[j];
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

	m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
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

	//by sl
	LoadCurrentMarking();
	Refresh();

}

bool SP_DS_StAnimation::AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	if (!SP_DS_Animation::AddToDialog(p_pcDlg, p_pcSizer))
		return FALSE;

	wxString l_asChoices[] =
	{ wxT("Maximum"), wxT("Intermediate"), wxT("Single") };
	m_pcChoice = new wxRadioBox(p_pcDlg, -1, wxT("Stepping"), wxDefaultPosition, wxDefaultSize, 3, l_asChoices, 1, wxRA_SPECIFY_ROWS);

	m_pcChoice->SetSelection(m_nStepState - 1);

	p_pcSizer->Add(m_pcChoice, 0, wxEXPAND | wxALL, 5);

	return TRUE;
}

void SP_DS_StAnimation::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_NewConstantDefinition* l_pcDlg = new SP_DLG_NewConstantDefinition(NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void SP_DS_StAnimation::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Transition"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyWeightSets( wxCommandEvent& p_cEvent )
{
	SP_DLG_StFunctionOverview* l_pcDlg = new SP_DLG_StFunctionOverview(wxT("Immediate Transition"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyDelaySets( wxCommandEvent& p_cEvent )
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	l_pcDlg->Destroy();
}

void
SP_DS_StAnimation::OnModifyScheduleSets( wxCommandEvent& p_cEvent )
{
	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), NULL);

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

	m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

}

void SP_DS_StAnimation::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{

	/*
	 * bysl work around for active list
	 */
	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{
		if (m_apcColListAttr[i])
		{
			m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
		}
	}

	m_pcOutputLabelStaticText->SetLabel(wxT("Start new animation ..."));
	m_bRestartAnimationFlag = true;

	LoadCurrentMarking();

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

	m_lPossibleTransAnimators.clear();
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

	if (!l_bReturn)
	{
		m_nActStep = m_nSteps - 1; // Step increments m_nActStep;
		wxString msg;
		if (m_bOneShot)
		{
			if (GetSingleStep())
			{
				msg = wxT("This transition is not enabled!");
			}
			else
			{
				msg = wxT("Dead state: there are no enabled transitions");
			}
		}
		else
		{
			msg = wxT("Dead state: there are no more enabled transitions");
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
