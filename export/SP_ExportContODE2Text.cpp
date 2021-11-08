/*
 * SP_ExportContODE2Text.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 12.8.2010
 * $Modified: George Assaf$
 * $Modification date:20.11.2020$
 * Short Description:
 */
 //==============================================================================

#include "export/SP_ExportContODE2Text.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

SP_ExportContODE2Text::SP_ExportContODE2Text(): SP_ExportPT()
{
	m_pcMainSimulator = nullptr;
	m_asPlaceNames.clear();
}

SP_ExportContODE2Text::~SP_ExportContODE2Text()
{
	wxDELETE(m_pcMainSimulator);
}

bool SP_ExportContODE2Text::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_CONTINUOUSPED_CLASS || cName ==SP_DS_HYBRIDPN_CLASS);
}
bool SP_ExportContODE2Text::DoWrite()
{

	    SP_DS_PlaceODE* l_pcPlaceODEConstructor;
		wxString l_sResult = wxT("");
	    const SP_ListNode *l_places=m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	    l_pcPlaceODEConstructor=new SP_DS_PlaceODE(m_graph);

	    if(!l_pcPlaceODEConstructor->Initialize())
		{
			SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODEs"));
			return false;
		}

	    if (!InitialiseContSim()) return false;

	    long l_nPlacePos=0;
	    //Iterate for all the places
	    for(auto l_itPlace : *l_places)
		{
			wxString l_sName=(dynamic_cast<SP_DS_NameAttribute*>(l_itPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)))->GetValue();
			l_sResult<<wxT("d")<<l_sName<<wxT("/dt = ");
			if((dynamic_cast<SP_DS_BoolAttribute*>(l_itPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL)))->GetValue())
			{
				l_sResult << wxT("0");
			}
			else
			{
				//l_sResult << l_pcPlaceODEConstructor->ConstructODE(l_nPlacePos) << wxT("\n\n");
				l_sResult << (dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->GetPlaceODEString(l_nPlacePos) << wxT("\n\n");
			}
			l_nPlacePos++;
		}

	    m_pcMainSimulator->AbortSimulation();
		m_file.Write(l_sResult);

		wxDELETE(l_pcPlaceODEConstructor);

		return true;
}

bool SP_ExportContODE2Text::InitialiseContSim()
{
	m_pcMainSimulator= new spsim::CVODEContinuous(CV_BDF, CV_NEWTON);
	if (m_pcMainSimulator)
	{

		LoadParams();
		LoadContPlaces();
		LoadConTrans();
		LoadConnections();
		m_pcMainSimulator->SetOutputEndPoint(100.0);
		m_pcMainSimulator->SetOutputStartPoint(0.0);
		//m_pcMainSimulator->SetOutputSampleSize((l_nOutputEndPoint - l_nOutputStartPoint) / (m_nResultPointCount - 1));
		m_pcMainSimulator->SetOutputSampleSize(1);
		return  m_pcMainSimulator->Initialise(true);
	}
	else
	{
		return false;
	}
}


void SP_ExportContODE2Text::LoadContPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

	SP_VectorDouble l_anCurrentMarking;
	SP_VectorStdString l_asPlaceNames;
	unsigned long l_nPlaceCount;

	SP_ListNode::const_iterator l_itElem;
	SP_VectorDouble::iterator l_itCurrentMarking;
	SP_VectorStdString::iterator l_itPlaceName;

	//SP_DS_ColListAttribute* l_pcColList;
	SP_DS_DoubleMarkingAttribute* l_pcMarking;

	l_nPlaceCount = l_pcNodeclass->GetElements()->size();

	m_pcMainSimulator->SetPlaceCount(l_nPlaceCount);

	l_anCurrentMarking.assign(l_nPlaceCount, 0);

	l_asPlaceNames.assign(l_nPlaceCount, "");

	l_itCurrentMarking = l_anCurrentMarking.begin();

	l_itPlaceName = l_asPlaceNames.begin();

	m_asPlaceNames.clear();

	SP_VectorBool l_abIsFixed;

	l_abIsFixed.assign(l_nPlaceCount, false);


	unsigned long l_nPosition = 0;

	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		(*l_itPlaceName++) = l_sName;

		l_pcMarking = dynamic_cast<SP_DS_DoubleMarkingAttribute*>((*l_itElem)->GetAttribute(wxT("Marking")));
		(*l_itCurrentMarking++) = l_pcMarking->GetValue(true);

		m_asPlaceNames.push_back(l_sName);
		m_mPlaceName2Position[l_sName] = l_nPosition;

		//get fixed flag
		bool l_bFixedFlag = dynamic_cast<SP_DS_BoolAttribute*>((*l_itElem)->GetAttribute(wxT("Fixed")))->GetValue();

		l_abIsFixed[l_nPosition] = l_bFixedFlag;

		l_nPosition++;
	}

	m_pcMainSimulator->SetPlaceNames(l_asPlaceNames);

	(dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->SetInitialMarking(l_anCurrentMarking);

	//set fixed flag
	(dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);
}


void SP_ExportContODE2Text::LoadConnections()
{
	//Map transition name to their positions
	m_pcMainSimulator->MapTransitionNames2Positions();

	m_pcMainSimulator->MapPlaceNames2Positions();

	//Edge
	LoadConnectionOfType(SP_DS_EDGE, spsim::STANDARD_ARC);

	//Read Arc
	LoadConnectionOfType(SP_DS_READ_EDGE, spsim::READ_ARC);

	//Inhibitor edge
	LoadConnectionOfType(SP_DS_INHIBITOR_EDGE, spsim::INHIBITOR_ARC);

	//Modifier
	LoadConnectionOfType(SP_DS_MODIFIER_EDGE, spsim::MODIFIER_ARC);
}



void SP_ExportContODE2Text::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
	const SP_ListEdge* l_pcEdgeList;
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sSourceNodeName, l_sDestNodeName;

	SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;
	;

	SP_DS_NameAttribute* l_pcNameAttribute;

	CHECK_POINTER(m_graph->GetEdgeclass(p_sArcType), return);

	l_pcEdgeList = m_graph->GetEdgeclass(p_sArcType)->GetElements();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		//Get source Node Name
		l_pcSourceNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

		CHECK_POINTER(l_pcSourceNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sSourceNodeName = l_pcNameAttribute->GetValue();

		//Get target Node Name
		l_pcTargetNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

		CHECK_POINTER(l_pcTargetNode, return);

		l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

		l_sDestNodeName = l_pcNameAttribute->GetValue();

		//get the arc weight as a string
		//wxString l_sArcWeight = GetEdgeWeight((*l_itEdge));
		//////////////////////////////////
		wxString l_sWeight;

		//Get the Edge weight
		SP_DS_Attribute* l_pcAtt = (*l_itEdge)->GetAttribute(wxT("Multiplicity"));


		if (l_pcAtt == NULL) //e.g., modifier arc has no Multiplicity
		{
			l_sWeight=wxT("0.0");
		}

		if (p_ArcType != 6)//not equal to modifier arc
		l_sWeight = l_pcAtt->GetValueString();
		//////////////////////////////////
		//converted arc weight as a number
		double l_nNumericArcWeight = 0;

		//Get the arc Weight
		if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
		{

			//if (IsConstantArcWeight(l_sArcWeight,l_nNumericArcWeight)==true)
			if (IsEvaluatedArcWeight(l_sWeight, l_nNumericArcWeight) == true)
			{
				m_pcMainSimulator->SetPostTransitionConnection(l_sSourceNodeName, l_sDestNodeName, l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPostSelfModifyingWeights(l_sSourceNodeName, l_sDestNodeName, l_sWeight);
			}

		}
		else
		{
			bool l_bIsConstant = false;
			//l_bIsConstant=IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight);
			l_bIsConstant = IsEvaluatedArcWeight(l_sWeight, l_nNumericArcWeight);
			if (l_bIsConstant == true)
			{
				m_pcMainSimulator->SetPreTransitionConnection(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPreSelfModifyingWeights(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_sWeight);
			}

		}
	}
}

bool SP_ExportContODE2Text::IsEvaluatedArcWeight(const wxString& p_sArcWeight, double& p_dVal)
{
	double dValue = 0.0;
	std::string strValue = p_sArcWeight;

	SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();
	wxString l_sArcWeight = p_sArcWeight;
	SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_sArcWeight);
	wxString l_sExpanded;
	if (l_pcFunction)
	{
		SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
		l_sExpanded = l_pcExpanded->toString();

		if (l_sExpanded.ToDouble(&dValue))
		{
			p_dVal = dValue;
			return true; //constant
		}
		else
		{
			return false;
		}
	}
	else
	{

		l_sExpanded = l_sArcWeight;
		return false;
	}
	return true;

}


void SP_ExportContODE2Text::LoadConTrans()
{

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	SP_ListNode::const_iterator l_itElem;

	SP_DS_ColListAttribute* l_pcColList;

	m_pcMainSimulator->SetTransitionCount(l_pcNodeclass->GetElements()->size());
	unsigned long l_nPosition = 0;

	//clear old transitions
	m_pcMainSimulator->ClearTransitions();
	SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();

	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));

		wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);
		SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_sTransitionFunction);
		wxString l_sExpanded;
		if (l_pcFunction)
		{
			SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
			l_sExpanded = l_pcExpanded->toString();
		}
		else
		{
			l_sExpanded = l_sTransitionFunction;
		}

		//add a transition
		m_pcMainSimulator->AddTransition(l_sName, l_sExpanded, spsim::TRANSITION_TYPE_CONTINUOUS);
		m_asTransitionNames.push_back(l_sName);
		m_mTransitionName2Position[l_sName] = l_nPosition;

		l_nPosition++;
	}

}

void SP_ExportContODE2Text::LoadParams()
{
	SP_DS_Metadataclass* mc = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;

	SP_VectorStdString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	m_msParameterName2Value.clear();

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

		l_asParameterNames.push_back(l_sName);

		SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if (l_FE.IsOk())
		{
			SP_FunctionPtr l_Function = l_FE.getFunction();
			double l_nValue = 0;
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
				l_pcFR->registerFunction(l_sName, to_string(l_nValue));
			}
			//****************************************************************************************

			l_anParameterValue.push_back(l_nValue);

			m_msParameterName2Value[l_sName] = l_nValue;


		}
	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);
}


