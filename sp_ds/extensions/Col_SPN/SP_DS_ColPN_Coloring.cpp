
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Coloring.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include <algorithm>
#include <ctime>

#include <wx/busyinfo.h> 
#include <wx/regex.h>


#include "dssz/extern/alphanum.hpp"


bool alphanum_comparerNc::operator() (const wxString& left, const wxString& right) const
{
	std::string l(left.mbc_str());
	std::string r(right.mbc_str());

	return doj::alphanum_comp(l, r) < 0;
}

SP_DS_ColPN_Coloring::SP_DS_ColPN_Coloring()
{
	m_nLastUpdateTime = 0;
}

SP_DS_ColPN_Coloring::~SP_DS_ColPN_Coloring()
{
}
bool SP_DS_ColPN_Coloring::Start()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	m_sNetClass = m_pcGraph->GetNetclass()->GetName();

		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if (!l_cSyntaxChecking.SyntaxChecking())
			return false;

		//initialize
		if (!Initialize())
			return false;

	   LoadColoredNet();
	   ReportColoringNet();

	return true;
}


bool SP_DS_ColPN_Coloring::Coloring()
{
	
	return Start();

}
void SP_DS_ColPN_Coloring::ReportColoringNet()
{
	

	//report the size of the colored net
	wxString l_sPlaceNumber = wxString::Format(wxT("%ld"), m_nColPlaceCount);
	l_sPlaceNumber = FormatByComma(l_sPlaceNumber);
	wxString l_sTransNumber = wxString::Format(wxT("%ld"), m_nColTransitionCount);
	l_sTransNumber = FormatByComma(l_sTransNumber);
	wxString l_sStandardArcNumber = wxString::Format(wxT("%ld"), m_nOrdinaryArcNum);
	l_sStandardArcNumber = FormatByComma(l_sStandardArcNumber);
	wxString l_sReadArcNumber = wxString::Format(wxT("%ld"), m_nReadArcNum);
	l_sReadArcNumber = FormatByComma(l_sReadArcNumber);
	wxString l_sInhibitorArcNumber = wxString::Format(wxT("%ld"), m_nInhibitorArcNum);
	l_sInhibitorArcNumber = FormatByComma(l_sInhibitorArcNumber);
	wxString l_sEqualArcNumber = wxString::Format(wxT("%ld"), m_nEqualArcNum);
	l_sEqualArcNumber = FormatByComma(l_sEqualArcNumber);
	wxString l_sResetArcNumber = wxString::Format(wxT("%ld"), m_nResetArcNum);
	l_sResetArcNumber = FormatByComma(l_sResetArcNumber);
	wxString l_sModifierArcNumber = wxString::Format(wxT("%ld"), m_nModifierArcNum);
	l_sModifierArcNumber = FormatByComma(l_sModifierArcNumber);

	SP_LOGMESSAGE(wxT("\n The size of the colored net: \n Colored Place number: ") + l_sPlaceNumber +
		wxT("\n Colored Transition number: ") + l_sTransNumber +
		wxT("\n Standard arc number: ") + l_sStandardArcNumber +
		wxT("\n Read arc number: ") + l_sReadArcNumber +
		wxT("\n Inhibitor arc number: ") + l_sInhibitorArcNumber +
		/*wxT("\n Equal arc number: ") + l_sEqualArcNumber +
		wxT("\n Reset arc number: ") + l_sResetArcNumber +*/
		wxT("\n Modifier arc number: ") + l_sModifierArcNumber +
		wxT("\n"));
}


bool SP_DS_ColPN_Coloring::Initialize()
{
	m_nLastUpdateTime = 0;
	

	if (!m_ValueAssign.InitializeColorset(m_ColorSetClass))
		return false;

	return true;
}
bool SP_DS_ColPN_Coloring::GetColTransitions()
{
	//m_vTransExprs.clear();
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;

	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vColTransitions.push_back(l_pcTransNode);
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vColTransitions.push_back(l_pcTransNode);
		}
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass(wxT("Immediate Transition"));
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vColTransitions.push_back(l_pcTransNode);
		}
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"));
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vColTransitions.push_back(l_pcTransNode);
		}
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"));
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			m_vColTransitions.push_back(l_pcTransNode);
		}
	}

	return true;
}

bool SP_DS_ColPN_Coloring::AssignplacesInfo()
{
	SP_DS_Nodeclass* l_pcNodeclass;
//	unsigned long l_nPlaceCount;
	SP_ListNode::const_iterator l_itElem;
	

	SP_DS_DoubleMarkingAttribute* l_pcMarking;
	
	SP_VectorDouble l_anCurrentMarking;


	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			m_vcolplaceNode.push_back(l_pcNode);
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			for (auto l_itpreEdge = l_pcNode->GetTargetEdges()->begin(); l_itpreEdge != l_pcNode->GetTargetEdges()->end(); ++l_itpreEdge)
			{
				m_pcPreEdgesofplace.push_back((*l_itpreEdge));
			}
			for (auto l_itpostEdge = l_pcNode->GetSourceEdges()->begin(); l_itpostEdge != l_pcNode->GetSourceEdges()->end(); ++l_itpostEdge)
			{
				m_pcPostEdgesofplace.push_back((*l_itpostEdge));
			}
			
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));

			unsigned long l_nLow = m_msColoredPlaceNames.size();
			unsigned long l_nUp;

			

			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		
			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColorSetOfPlace.push_back(l_sColorSetName);
			m_vsallPlaces.push_back(l_sColorSetName);
			wxString l_sColplacWcolsetNa = l_sColPlaceName + wxT("_") + l_sColorSetName;

			m_msColoredPlaceNames.push_back(l_sColPlaceName);
			
			m_msUnColoredPlacWColorSet.push_back(GuardeffectonPlaceName(l_pcNode));
			

			vector<wxString> l_ColorVector;
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
			
			l_ColorVector = l_pcColorSet->GetStringValue();

			wxString l_snumberofcol = wxString::Format(wxT("%ld"), l_ColorVector.size());
			double l_ndnumberofcol;
			l_snumberofcol.ToDouble(&l_ndnumberofcol);

			l_pcMarking = dynamic_cast<SP_DS_DoubleMarkingAttribute*>((*l_itElem)->GetAttribute(wxT("Marking")));
			//(*l_itCurrentMarking++) = l_pcMarking->GetValue(true);
			wxString l_smarking = l_pcMarking->GetValueString();
			//wxString l_smarking = wxString::Format(wxT("%ld"), *l_itCurrentMarking);
			m_msMarking.push_back(l_smarking);
			//double l_ndmarking = std::strtod(l_smarking, NULL);
			double l_ndmarking;
			l_smarking.ToDouble(&l_ndmarking);
			double l_ndallMarkinginplace = l_ndmarking /*/ l_ndnumberofcol*/;
			double l_ndMarkingbycolor = l_ndmarking / l_ndnumberofcol;
					
			//m_mDMarking.push_back(l_ndallMarkinginplace);
			
			m_mDMarking.push_back(l_ndMarkingbycolor);
			
			l_nUp = m_msColoredPlaceNames.size();
			//l_nUp = m_vsallPlaces.size();
			SP_CPN_ColCountInterval l_PlInterval;
			l_PlInterval.m_nLow = l_nLow;
			l_PlInterval.m_nUp = l_nUp - 1;
			m_nsPlaceCountById[m_msColoredPlaceNames.size() - 1] = l_PlInterval;


			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			if (!l_cSyntaxChecking.Initialize())
				return false;

			/*map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
			if (!l_cSyntaxChecking.ComputeInitialMarking(l_pcNode, l_mColorToMarkingMap, false))
				return false;
			//SP_CPN_UncoloredPlace l_mSingleUnPlace;*/
			
			for (unsigned int l_nPos = 0; l_nPos < l_ColorVector.size(); l_nPos++)
			{
				wxString l_sColor = l_ColorVector[l_nPos];
				//ModifyName(l_sColor);
				wxString  l_sUnColoredPlaceName = l_sColPlaceName + wxT("_") + l_sColor;
				m_vsUncoloredPlaces.push_back(l_sUnColoredPlaceName);
				m_vsallPlaces.push_back(l_sUnColoredPlaceName);
				//m_mDMarking.push_back(l_ndMarkingbycolor);
			}
			m_svColored2UnColoredPlaceNames[l_sColPlaceName] = m_vsUncoloredPlaces;
		}
	}



	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			m_vcolstplaceNode.push_back(l_pcNode);
			wxString l_sColstPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));


			unsigned long l_nLow = m_msColoredPlaceNames.size();
			//unsigned long l_nLow = m_vsallPlaces.size();
			unsigned long l_nUp;

		

			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));

			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColorSetOfstPlace.push_back(l_sColorSetName);
			//m_vsallPlaces.push_back(l_sColorSetName);
			wxString l_sColstplacWcolsetNa = l_sColstPlaceName + wxT("_") + l_sColorSetName;

			m_msColoredPlaceNames.push_back(l_sColstPlaceName);
			m_msUnColoredPlacWColorSet.push_back(l_sColstPlaceName + wxT("_") + l_sColorSetName);

			vector<wxString> l_ColorVector;
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);

			
			l_ColorVector = l_pcColorSet->GetStringValue();

			wxString l_snumberofcol = wxString::Format(wxT("%ld"), l_ColorVector.size());
			double l_ndnumberofcol;
			l_snumberofcol.ToDouble(&l_ndnumberofcol);

			
			SP_DS_Attribute* l_pcAttribute = (*l_itElem)->GetAttribute(wxT("Marking"));
			

			l_nUp = m_msColoredPlaceNames.size();
			//l_nUp = m_vsallPlaces.size();
			SP_CPN_ColCountInterval l_PlInterval;
			l_PlInterval.m_nLow = l_nLow;
			l_PlInterval.m_nUp = l_nUp - 1;
			m_nsPlaceCountById[m_msColoredPlaceNames.size() - 1] = l_PlInterval;


			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			if (!l_cSyntaxChecking.Initialize())
				return false;

			for (unsigned int l_nPos = 0; l_nPos < l_ColorVector.size(); l_nPos++)
			{
				wxString l_sColor = l_ColorVector[l_nPos];
				//ModifyName(l_sColor);
				wxString  l_sUnColoredPlaceName = l_sColstPlaceName + wxT("_") + l_sColor;
				m_vsUncoloredPlaces.push_back(l_sUnColoredPlaceName);	
			}
			m_svColored2UnColoredPlaceNames[l_sColstPlaceName] = m_vsUncoloredPlaces;
		}
	}

	m_nColPlaceCount = m_msColoredPlaceNames.size();
	return true;
}

/*bool SP_DS_ColPN_Coloring::Testcolorsetofallplaces()
{
	

	SP_DS_Nodeclass* l_pcNodeclass;
	
	SP_ListNode::const_iterator l_itElem;
	
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));


			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast<SP_DS_TextAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));

			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColorSetOfPlacetest.push_back(l_sColorSetName);
		
		}

	}
	for (long i = 0; i < m_msColorSetOfPlacetest.size(); i++)
	{
		if (m_msColorSetOfPlacetest.size() == 1)
			return true;
		else
		if (m_msColorSetOfPlacetest[i] == m_msColorSetOfPlacetest[i + 1])
			return true;
		else
		{
			SP_MESSAGEBOX("all places not have the same color set");

			return false;
		}
	}
	//return true;
}*/



bool SP_DS_ColPN_Coloring::AssignTransInfo()
{
	
	SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;

	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			m_vcolTransNode.push_back(l_pcTransNode);

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));
			if (!l_pcColList)
				return false;

			wxString l_sTranGuard = wxString(l_pcColList->GetCell(0, 1).c_str());
			m_pcGuardsExpression.push_back(l_sTranGuard);

			unsigned long l_nLow = m_msColoredTransitionNames.size();
			unsigned long l_nUp;

			m_msColoredTransitionNames.push_back(l_sColTranName);
			m_msColoredTransGuard.push_back(l_sTranGuard);
	
			l_nUp = m_msColoredTransitionNames.size();
			SP_CPN_ColCountInterval l_Interval;
			l_Interval.m_nLow = l_nLow;
			l_Interval.m_nUp = l_nUp - 1;
			m_nsTransitionCountById[m_msColoredTransitionNames.size() - 1] = l_Interval;

			/*l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(wxT("FunctionList")));
			wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);*/

			// Transition after parsing
			wxString l_TransRatFunction = ColTransitionsRatfn(l_pcTransNode);


		/*	if (l_pcNodeclass->GetElements()->size() > 0)
			{
				unsigned int l_nContTrCurrentColumn;
				l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(l_pcNodeclass->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
				l_nContTrCurrentColumn = l_pcColList->GetActiveColumn();
			    wxString l_sTransitionFunctions = wxString::Format(wxT("%ld"), l_nContTrCurrentColumn);
				l_sTransitionFunctions = FormatByComma(l_sTransitionFunctions);
				m_msColTransRatFun.push_back(l_sTransitionFunctions);
			}*/
			//SP_LOGERROR(wxT("================================="));
			//SP_LOGERROR(CollectPreArcsExprs(l_pcTransNode)+wxT(":")+ l_sTranGuard +wxT(":")+CollectPostArcsExprs(l_pcTransNode));
			m_msColTransRatFun.push_back(l_TransRatFunction);

		}
	}




	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_TRANS);
	if (l_pcNodeclass)
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = *l_itElem;
			wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_vcolTransNode.push_back(l_pcTransNode);

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));
			if (!l_pcColList)
				return false;

			//wxString l_sTranGuard = wxString(l_pcColList->GetCell(0, 1).c_str());
			//m_pcGuardsExpression.push_back(l_sTranGuard);

			unsigned long l_nLow = m_msColoredTransitionNames.size();
			unsigned long l_nUp;

			m_msColoredTransitionNames.push_back(l_sColTranName);
			//m_msColoredTransGuard.push_back(l_sTranGuard);

			l_nUp = m_msColoredTransitionNames.size();
			SP_CPN_ColCountInterval l_Interval;
			l_Interval.m_nLow = l_nLow;
			l_Interval.m_nUp = l_nUp - 1;
			m_nsstTransitionCountById[m_msColoredTransitionNames.size() - 1] = l_Interval;

			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("FunctionList")));
			wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);


			m_msColTransRatFun.push_back(l_TransRatFunction);
		}
	}
	m_nColTransitionCount = m_msColoredTransitionNames.size(); 
	return true;
}
bool SP_DS_ColPN_Coloring::AssignArcInfo()
{
	SP_DS_Edgeclass* l_pcEdgeclass;
	SP_ListEdge::const_iterator l_itEdge;
	//SP_DS_TextAttribute* l_pcCSAttibute;

	l_pcEdgeclass = m_pcGraph->GetEdgeclass(wxT("Edge"));
	if (l_pcEdgeclass)
	{
		for (l_itEdge = l_pcEdgeclass->GetElements()->begin(); l_itEdge != l_pcEdgeclass->GetElements()->end(); ++l_itEdge)
		{
			m_pcEdgeVector.push_back(*l_itEdge);

			SP_DS_Edge* p_pcEdge = *l_itEdge;
			SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
			if (!l_pcColList11)
				return false;
			wxString l_sExpression = l_pcColList11->GetCell(0, 1);
			
			m_pcEdgeExpression.push_back(l_sExpression);
			m_pcEdgeName.push_back(GetEdgeName(*l_itEdge));

		}
	}
	m_nOrdinaryArcNum = m_pcEdgeVector.size();

	l_pcEdgeclass = m_pcGraph->GetEdgeclass(wxT("Read Edge"));
	if (l_pcEdgeclass)
	{
		for (l_itEdge = l_pcEdgeclass->GetElements()->begin(); l_itEdge != l_pcEdgeclass->GetElements()->end(); ++l_itEdge)
		{
			//m_pcEdgeVector.push_back(*l_itEdge);

			SP_DS_Edge* p_pcEdge = *l_itEdge;
			SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
			if (!l_pcColList11)
				return false;
			wxString l_sExpression = l_pcColList11->GetCell(0, 1);

		
			m_pcReadEdgeExpression.push_back(l_sExpression);
			m_pcReadEdgeName.push_back(GetEdgeName(*l_itEdge));
		}
	}
	m_nReadArcNum = m_pcReadEdgeName.size();

	l_pcEdgeclass = m_pcGraph->GetEdgeclass(wxT("Inhibitor Edge"));
	if (l_pcEdgeclass)
	{
		for (l_itEdge = l_pcEdgeclass->GetElements()->begin(); l_itEdge != l_pcEdgeclass->GetElements()->end(); ++l_itEdge)
		{
			//m_pcEdgeVector.push_back(*l_itEdge);

			SP_DS_Edge* p_pcEdge = *l_itEdge;
			SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
			if (!l_pcColList11)
				return false;
			wxString l_sExpression = l_pcColList11->GetCell(0, 1);


			m_pcModifierEdgeExpression.push_back(l_sExpression);

			m_pcInhibitorEdgeName.push_back(GetEdgeName(*l_itEdge));

		}
	}
	m_nInhibitorArcNum = m_pcInhibitorEdgeName.size();


	l_pcEdgeclass = m_pcGraph->GetEdgeclass(wxT("Modifier Edge"));
	if (l_pcEdgeclass)
	{
		for (l_itEdge = l_pcEdgeclass->GetElements()->begin(); l_itEdge != l_pcEdgeclass->GetElements()->end(); ++l_itEdge)
		{
			//m_pcEdgeVector.push_back(*l_itEdge);

			SP_DS_Edge* p_pcEdge = *l_itEdge;
			SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
			if (!l_pcColList11)
				return false;
			wxString l_sExpression = l_pcColList11->GetCell(0, 1);


			m_pcInhibitorEdgeExpression.push_back(l_sExpression);

			m_pcModifierEdgeName.push_back(GetEdgeName(*l_itEdge));

		}
	}
	
	m_nModifierArcNum = m_pcModifierEdgeName.size();
	return true;
}
wxString SP_DS_ColPN_Coloring::GetEdgeName(SP_DS_Edge* p_pcEdge)
{
	wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sEdgeName = l_sSourceNodeName + wxT(" -> ") + l_sTargetNodeName;
	//SP_LOGERROR(wxT(" Edge Direction:")+ l_sEdgeName);
	return l_sEdgeName;
}
wxString SP_DS_ColPN_Coloring::ArcsInformation()
{
	wxString l_sArcsinfo = wxT("");
	
	for (unsigned i = 0; i < GetEdgeName()->size(); i++)
	{
		l_sArcsinfo += (*(GetEdgeName()))[i] + wxT(":") + (*(GetEdgeExpression()))[i] + wxT("\n");

	}
	m_sArcsInformation = wxT("Arcs Information \n") + l_sArcsinfo;
	return m_sArcsInformation;
}
wxString SP_DS_ColPN_Coloring::TranswithGuards()
{
	wxString l_sGuards = wxT("");
	for (unsigned i = 0; i < GetColoredTransitionNames()->size(); i++)
	{
		l_sGuards += (*(GetColoredTransitionNames()))[i] + wxT(": ") + (*(GetGuardExpression()))[i] + wxT("\n") ;

	}
	m_sTranswithGuard = wxT("Transitions with Guards \n") + l_sGuards;
	return m_sTranswithGuard;
}
wxString SP_DS_ColPN_Coloring::FormatByComma(wxString p_sNumber)
{
	//for example, 2000000 to 2,000,000
	wxString l_sPlaceNumber = p_sNumber;
	vector<wxString> l_vPlacesegments;
	if (l_sPlaceNumber.Len()>3)
	{
		unsigned int l_nMo = l_sPlaceNumber.Len() % 3;
		unsigned int l_n3times = l_sPlaceNumber.Len() / 3;
		l_vPlacesegments.push_back(l_sPlaceNumber.Mid(0, l_nMo));
		for (unsigned i = 0; i < l_n3times; i++)
		{
			wxString l_sNew3str = l_sPlaceNumber.Mid(l_nMo + 3 * i, 3);
			l_vPlacesegments.push_back(l_sNew3str);
		}
		l_sPlaceNumber = wxT("");
		for (unsigned i = 0; i < l_vPlacesegments.size(); i++)
		{
			l_sPlaceNumber = l_sPlaceNumber + wxT(",") + l_vPlacesegments[i];
		}
		l_sPlaceNumber = l_sPlaceNumber.AfterFirst(wxT(','));
		if (l_sPlaceNumber.Mid(0, 1) == wxT(","))
		{
			l_sPlaceNumber = l_sPlaceNumber.AfterFirst(wxT(','));
		}
	}

	return  l_sPlaceNumber;
}

bool SP_DS_ColPN_Coloring::LoadColoredNet()
{
	AssignTransInfo();
	AssignplacesInfo();
	AssignArcInfo();
	return true;
}

bool SP_DS_ColPN_Coloring::ClearColoredNet()
{
	m_msColoredPlaceNames.clear();
	m_msColoredTransitionNames.clear();
	m_pcEdgeVector.clear();
	return true;
}
wxString SP_DS_ColPN_Coloring::GuardeffectonPlaceName(SP_DS_Node* p_pcPlaceNode)
{
	wxString l_sColPlacename=("");

	l_sColPlacename = GuardeffectonprePlaceName(p_pcPlaceNode);
	if(l_sColPlacename !=wxT(""))
		return l_sColPlacename;

	l_sColPlacename = GuardeffectonpostPlaceName(p_pcPlaceNode);

	return l_sColPlacename;
}

wxString SP_DS_ColPN_Coloring::GuardeffectonprePlaceName(SP_DS_Node* p_pcPlaceNode)
{

	wxString l_sColorSetName = wxT("");
	wxString l_sColorSetName1 = wxT("");
	wxString l_sColPlaceName = wxT("");

	for (auto l_itEdge = p_pcPlaceNode->GetSourceEdges()->begin(); l_itEdge != p_pcPlaceNode->GetSourceEdges()->end(); ++l_itEdge)
	//for (auto l_itpostEdge :m_pcPostEdgesofplace)
	{

		
		SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >((*l_itEdge)->GetAttribute(SP_DS_CPN_INSCRIPTION));
		wxString l_sArcExpression = l_pcColList1->GetCell(0, 1);
		wxString l_sArcVariable = GetArcVariable((*l_itEdge));
		

		SP_DS_Node* l_pcPlacesNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
		l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType
		                                                    (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_sColPlaceName += wxT("__");

		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute
		                                                                             (SP_DS_CPN_COLORSETNAME));
		
		l_sColorSetName = l_pcNameAttibute->GetValue();

		vector<wxString> l_ColorVector;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
		l_ColorVector = l_pcColorSet->GetStringValue();
		long l_nColorSetSize = l_ColorVector.size();
		wxString l_sColorSetSize = wxString::Format(wxT("%ld"), l_nColorSetSize);

		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
		l_sColPlaceName += GuardParsingprePlace(l_pcTransNode, l_sArcVariable, l_nColorSetSize,
			                             l_ColorVector, l_sColorSetName);
		

	}
	return l_sColPlaceName;
}

wxString SP_DS_ColPN_Coloring::GuardParsingprePlace(SP_DS_Node* p_pcTransNode,wxString& p_sArcVariable,
	                                        long& p_nColorSetSize,vector<wxString>& p_ncolorSize, wxString& p_sColorSetName)
{
	
	wxString l_sColorSetName1 = wxT("");
	wxString l_sColPlaceName = wxT("");

	SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(p_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());

	l_sTranGuard.Replace(wxT(" "), wxT(""));

	std::size_t foundAVGN = l_sTranGuard.find(wxT("="));
	
	wxString l_sArcVariable1;

	SP_ListEdge::const_iterator l_itEdge1;
	//for (auto 
		l_itEdge1 = p_pcTransNode->GetSourceEdges()->begin();
		//l_itEdge1 != p_pcTransNode->GetSourceEdges()->end(); ++l_itEdge1)
	//{
		//l_itEdge1 = p_pcTransNode->GetSourceEdges()->begin();


		
		l_sArcVariable1 = GetArcVariable((*l_itEdge1));
		SP_DS_Node* l_pcPlaceNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetTarget());


		SP_DS_TextAttribute* l_pcNameAttibute1 = dynamic_cast<SP_DS_TextAttribute*>(l_pcPlaceNode1->GetAttribute
		(SP_DS_CPN_COLORSETNAME));

		l_sColorSetName1 = l_pcNameAttibute1->GetValue();

		vector<wxString> l_ColorVector1;
		SP_CPN_ColorSet* l_pcColorSet1 = m_ColorSetClass.LookupColorSet(l_sColorSetName1);
		l_ColorVector1 = l_pcColorSet1->GetStringValue();
		long l_nColorSetSize1 = l_ColorVector1.size();
		wxString l_sColorSetSize1 = wxString::Format(wxT("%ld"), l_nColorSetSize1);

		wxString l_sGuardEx = p_sArcVariable + wxT("=") + l_sArcVariable1;
		wxString l_sGuardEx1 = l_sArcVariable1 + wxT("=") + p_sArcVariable;

		if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 < p_nColorSetSize)
		{
			l_sColPlaceName += l_sColorSetName1;
			return l_sColPlaceName;
		}
		else
			if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 > p_nColorSetSize)
			{
				l_sColPlaceName += p_sColorSetName;
				return l_sColPlaceName;
			}
			else
				if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 == p_nColorSetSize)
				{
					l_sColPlaceName += p_sColorSetName;
					return l_sColPlaceName;
				}

		std::size_t foundAVG = l_sTranGuard.find(p_sArcVariable);
		if (foundAVG != std::string::npos)
		{
			for (unsigned int l_nPos = 0; l_nPos < p_ncolorSize.size(); l_nPos++)
			{
				wxString l_sColor = p_ncolorSize[l_nPos];


				std::size_t foundg = l_sTranGuard.find(p_sArcVariable + wxT('=') + l_sColor);
				if (foundg != std::string::npos)
				{
					l_sColPlaceName += l_sColor + wxT(",");
				}
			}
			l_sColPlaceName = l_sColPlaceName.BeforeLast(',');

		}
		else
			l_sColPlaceName += p_sColorSetName;
	//}
	return l_sColPlaceName;
}

wxString SP_DS_ColPN_Coloring::GuardeffectonpostPlaceName(SP_DS_Node* p_pcPlaceNode)
{
	
	wxString l_sColorSetName = wxT("");

	wxString l_sColPlaceName = wxT("");

	for (auto l_itEdge = p_pcPlaceNode->GetTargetEdges()->begin(); l_itEdge != p_pcPlaceNode->GetTargetEdges()->end(); ++l_itEdge)
   // for (auto l_itEdge : m_pcPreEdgesofplace)
	 {

		wxString l_sArcVariable = GetArcVariable((*l_itEdge));
	
		SP_DS_Node* l_pcPlacesNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
		l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType
		(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute
		(SP_DS_CPN_COLORSETNAME));
		
		l_sColorSetName = l_pcNameAttibute->GetValue();

		vector<wxString> l_ColorVector;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
		l_ColorVector = l_pcColorSet->GetStringValue();
		long l_nColorSetSize = l_ColorVector.size();
		wxString l_sColorSetSize = wxString::Format(wxT("%ld"), l_nColorSetSize);


		l_sColPlaceName += wxT("__");
		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
		
		SP_ListEdge::const_iterator l_itEdge1;
		wxString l_sArcVariable1;
			
			l_sColPlaceName += GuardParsingpostPlace(l_pcTransNode, l_sArcVariable, l_nColorSetSize, l_ColorVector, l_sColorSetName);
			
			
	}
	
	return l_sColPlaceName;
}

wxString SP_DS_ColPN_Coloring::GuardParsingpostPlace(SP_DS_Node* p_pcTransNode, wxString& p_sArcVariable,
	                                        long& p_nColorSetSize, vector<wxString>& p_ncolorSize, wxString& p_sColorSetName)
{

	wxString l_sColorSetName1 = wxT("");
	wxString l_sColPlaceName = wxT("");

	SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(p_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());

	l_sTranGuard.Replace(wxT(" "), wxT(""));

	std::size_t foundAVGN = l_sTranGuard.find(wxT("="));
	
	wxString l_sArcVariable1;

	SP_ListEdge::const_iterator l_itEdge1;
	for (auto l_itEdge1 = p_pcTransNode->GetTargetEdges()->begin(); 
		      l_itEdge1 != p_pcTransNode->GetTargetEdges()->end(); ++l_itEdge1)

	{

		
		l_sArcVariable1 = GetArcVariable((*l_itEdge1));
		SP_DS_Node* l_pcPlaceNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetSource());


		SP_DS_TextAttribute* l_pcNameAttibute1 = dynamic_cast<SP_DS_TextAttribute*>(l_pcPlaceNode1->GetAttribute
		(SP_DS_CPN_COLORSETNAME));

		l_sColorSetName1 = l_pcNameAttibute1->GetValue();

		vector<wxString> l_ColorVector1;
		SP_CPN_ColorSet* l_pcColorSet1 = m_ColorSetClass.LookupColorSet(l_sColorSetName1);
		l_ColorVector1 = l_pcColorSet1->GetStringValue();
		long l_nColorSetSize1 = l_ColorVector1.size();
		wxString l_sColorSetSize1 = wxString::Format(wxT("%ld"), l_nColorSetSize1);

		wxString l_sGuardEx = p_sArcVariable + wxT("=") + l_sArcVariable1;
		wxString l_sGuardEx1 = l_sArcVariable1 + wxT("=") + p_sArcVariable;

		if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 < p_nColorSetSize)
		{
			l_sColPlaceName += l_sColorSetName1;
			return l_sColPlaceName;
		}
		else
			if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 > p_nColorSetSize)
			{
				l_sColPlaceName += p_sColorSetName;
				return l_sColPlaceName;
			}
			else
				if ((l_sTranGuard == l_sGuardEx || l_sTranGuard == l_sGuardEx1) && l_nColorSetSize1 == p_nColorSetSize)
				{
					l_sColPlaceName += p_sColorSetName;
					return l_sColPlaceName;
				}

		std::size_t foundAVG = l_sTranGuard.find(p_sArcVariable);
		if (foundAVG != std::string::npos)
		{
			for (unsigned int l_nPos = 0; l_nPos < p_ncolorSize.size(); l_nPos++)
			{
				wxString l_sColor = p_ncolorSize[l_nPos];


				std::size_t foundg = l_sTranGuard.find(p_sArcVariable + wxT('=') + l_sColor);
				if (foundg != std::string::npos)
				{
					l_sColPlaceName += l_sColor + wxT(",");
				}
			}
			l_sColPlaceName = l_sColPlaceName.BeforeLast(',');

		}
		else
			l_sColPlaceName += p_sColorSetName;

	}
	return l_sColPlaceName;
}
wxString SP_DS_ColPN_Coloring::ColTransitionsRatfn(SP_DS_Node* p_pcTransNode)
{

	SP_DS_ColListAttribute* l_pcColList;
	l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcTransNode->GetAttribute(wxT("FunctionList")));
	wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);

	stringstream testTrans(l_TransRatFunction);
	std::size_t foundMA = l_TransRatFunction.find("MassAction");
         size_t s = l_TransRatFunction.find("(");
	     size_t e = l_TransRatFunction.find(")", s);
	     string Ratevalue = l_TransRatFunction.substr(s + 1, e - s - 1);

	
	if (foundMA != std::string::npos)
	{
		l_TransRatFunction = Ratevalue;
		for (auto l_itEdge = p_pcTransNode->GetTargetEdges()->begin(); l_itEdge != p_pcTransNode->GetTargetEdges()->end(); ++l_itEdge)
		
		{
			
			SP_DS_Edge* p_pcEdge1 = *l_itEdge;
			wxString l_sArweight = GetEdgeWeight(p_pcEdge1);
			SP_DS_Node* l_pcPlacesNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->
				                                    GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			if (l_sArweight == wxT("1"))
				l_TransRatFunction += wxT("*") + l_sColPlaceName;
			else
			l_TransRatFunction += wxT("*") + l_sColPlaceName + wxT("^") + l_sArweight;
		}
		l_TransRatFunction = l_TransRatFunction;
		return l_TransRatFunction;
	}
	else
	return  l_TransRatFunction;	

}

wxString SP_DS_ColPN_Coloring::GetEdgeWeight(SP_DS_Edge* p_pcEdge)
{

	wxString l_sArcMultiplicty;

	SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
	if (!l_pcColList1)

		wxString l_sArcExpression;
	wxString l_sArcExpression = l_pcColList1->GetCell(0, 1);

	stringstream testA(l_sArcExpression);
	string segment;
	string dash("`");
	wxString l_sArcmult;
	std::size_t foundEx = l_sArcExpression.find(dash);
	std::size_t foundEx1 = l_sArcExpression.find("++");
	if (foundEx1 != std::string::npos)
	{
		l_sArcMultiplicty = wxT("2");

	}
	else
		if (foundEx != std::string::npos)
		{

			std::getline(testA, segment, '`');
			l_sArcMultiplicty = segment;

		}
		else
			l_sArcMultiplicty = wxT("1");

	return l_sArcMultiplicty;
}
wxString SP_DS_ColPN_Coloring::GetArcweightwithSize(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTargetNode)
{
	return evaluateGuardontheArc(p_pcSourceNode, p_pcEdge, p_pcTargetNode);
}

wxString SP_DS_ColPN_Coloring::evaluateGuardontheArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTargetNode)
{

	wxString l_sArcWeightwSize;

	m_ValueAssign.InitializeColorset(m_ColorSetClass);


	wxString l_sArcWeight = GetEdgeWeight(p_pcEdge);
	if (l_sArcWeight != wxT("1"))
		l_sArcWeight += wxT("*");
	else
		l_sArcWeight = wxT("");

	

	if (p_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
          return evaluateGuardonthepostArc( p_pcSourceNode,  p_pcEdge,  p_pcTargetNode, l_sArcWeight);
	

	else
	     return evaluateGuardonthepreArc(p_pcSourceNode, p_pcEdge, p_pcTargetNode, l_sArcWeight);	
	
}


wxString SP_DS_ColPN_Coloring::evaluateGuardonthepostArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge,
	                                                     SP_DS_Node* p_pcTargetNode, wxString& p_sArcWeight)
{
	wxString l_sArcWeightwSize;
	vector<SP_DS_Edge*> l_pcEdgeVector;

	SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(p_pcSourceNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());

	l_sTranGuard.Replace(wxT(" "), wxT(""));

	SP_DS_TextAttribute* l_pcCSAttibute = dynamic_cast<SP_DS_TextAttribute*>(p_pcTargetNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	wxString l_sColorSetName = l_pcCSAttibute->GetValue();
	vector<wxString> l_ColorVector;
	SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
	l_ColorVector = l_pcColorSet->GetStringValue();
	long l_nVarSizeonArc = l_ColorVector.size();
	wxString l_sArcVariable = GetArcVariable(p_pcEdge);


	
	SP_ListEdge::const_iterator l_itEdge1;
	for (l_itEdge1 = p_pcSourceNode->GetTargetEdges()->begin(); l_itEdge1 != p_pcSourceNode->GetTargetEdges()->end(); ++l_itEdge1)

	{

		l_pcEdgeVector.push_back(*l_itEdge1);

		wxString l_sArcVariable1 = GetArcVariable(*l_itEdge1);
		SP_DS_Node* l_pcSourceNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetSource());

		SP_DS_TextAttribute* l_pcCSAttibute21 = dynamic_cast<SP_DS_TextAttribute*>(l_pcSourceNode1->GetAttribute(SP_DS_CPN_COLORSETNAME));
		wxString l_sColorSetName21 = l_pcCSAttibute21->GetValue();
		vector<wxString> l_ColorVector21;
		SP_CPN_ColorSet* l_pcColorSet21 = m_ColorSetClass.LookupColorSet(l_sColorSetName21);
		l_ColorVector21 = l_pcColorSet21->GetStringValue();

		long l_nVarSizeonArc1 = l_ColorVector21.size();

		vector<wxString> l_svVarnewSizeonArc1;

		std::size_t foundplusV1 = l_sArcVariable1.find(wxT("++"));
		std::size_t foundplusV2 = l_sArcVariable.find(wxT("++"));
		std::size_t foundplusVSuccor = l_sArcVariable.find(wxT("+"));
		std::size_t foundAVG = l_sTranGuard.find(l_sArcVariable1);
		std::size_t foundAVS = l_sTranGuard.find(l_sArcVariable);

		std::size_t foundAVGN = l_sTranGuard.find(wxT("="));
		if (foundAVGN == std::string::npos && l_sArcVariable != l_sArcVariable1 &&
			foundplusVSuccor == std::string::npos &&
			foundplusV1 == std::string::npos && foundplusV2 == std::string::npos)
		{
			l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_nVarSizeonArc1);
			return l_sArcWeightwSize;
		}


		else
			if (foundAVG != std::string::npos)
			{
				for (unsigned int l_nPos = 0; l_nPos < l_ColorVector21.size(); l_nPos++)
				{
					wxString l_sColor = l_ColorVector21[l_nPos];


					std::size_t foundg = l_sTranGuard.find(l_sArcVariable1 + wxT('=') + l_sColor);
					if (foundg != std::string::npos)
					{
						l_svVarnewSizeonArc1.push_back(l_sColor);
					}

				}
				l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_svVarnewSizeonArc1.size());
				if (l_svVarnewSizeonArc1.size() == 0)
				{

					l_sArcWeightwSize = p_sArcWeight + wxT("1");
				}
				return l_sArcWeightwSize;

			}
			else
				if (foundAVG == std::string::npos && foundAVS != std::string::npos)
				{
					l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_ColorVector21.size());
					return l_sArcWeightwSize;
				}


				else
				{
					l_sArcWeightwSize = GetEdgeWeight(p_pcEdge);
					return l_sArcWeightwSize;
				}

	}
	if (l_pcEdgeVector.size() == 0)
	{

		l_sArcWeightwSize = GetEdgeWeight(p_pcEdge);
		return l_sArcWeightwSize;
	}
	return l_sArcWeightwSize;
}

wxString SP_DS_ColPN_Coloring::evaluateGuardonthepreArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge,
	                                                    SP_DS_Node* p_pcTargetNode, wxString& p_sArcWeight)
{

	wxString l_sArcWeightwSize;
	wxString l_sVarSizeonArc1;
	vector<SP_DS_Edge*> l_pcEdgeVector;

	SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(p_pcTargetNode->GetAttribute(SP_DS_CPN_GUARDLIST));
	wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());
	std::size_t foundAVGN = l_sTranGuard.find(wxT("="));

	SP_DS_TextAttribute* l_pcCSAttibute = dynamic_cast<SP_DS_TextAttribute*>(p_pcSourceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	wxString l_sColorSetName = l_pcCSAttibute->GetValue();
	vector<wxString> l_ColorVector;
	SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
	l_ColorVector = l_pcColorSet->GetStringValue();
	long l_nVarSizeonArc = l_ColorVector.size();
	wxString l_sArcVariable = GetArcVariable(p_pcEdge);


	
	SP_ListEdge::const_iterator l_itEdge1;
	for (l_itEdge1 = p_pcTargetNode->GetSourceEdges()->begin(); l_itEdge1 != p_pcTargetNode->GetSourceEdges()->end(); ++l_itEdge1)

	{
		l_pcEdgeVector.push_back(*l_itEdge1);

		wxString l_sArcVariable1 = GetArcVariable(*l_itEdge1);
		SP_DS_Node* l_pcSourceNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetTarget());

		SP_DS_TextAttribute* l_pcCSAttibute21 = dynamic_cast<SP_DS_TextAttribute*>(l_pcSourceNode1->GetAttribute(SP_DS_CPN_COLORSETNAME));
		wxString l_sColorSetName21 = l_pcCSAttibute21->GetValue();
		vector<wxString> l_ColorVector21;
		SP_CPN_ColorSet* l_pcColorSet21 = m_ColorSetClass.LookupColorSet(l_sColorSetName21);
		l_ColorVector21 = l_pcColorSet21->GetStringValue();
		long l_nVarSizeonArc1 = l_ColorVector21.size();
		l_sVarSizeonArc1 = wxString::Format(wxT("%ld"), l_nVarSizeonArc1);

		vector<wxString> l_svVarnewSizeonArc1;
		std::size_t foundplusV1 = l_sArcVariable1.find(wxT("++"));
		std::size_t foundplusV2 = l_sArcVariable.find(wxT("++"));
		std::size_t foundAVG = l_sTranGuard.find(l_sArcVariable1);
		std::size_t foundAVS = l_sTranGuard.find(l_sArcVariable);

		std::size_t foundAVGN = l_sTranGuard.find(wxT("="));
		if (foundAVGN == std::string::npos && l_sArcVariable1 != l_sArcVariable &&
			(foundplusV1 == std::string::npos && foundplusV2 == std::string::npos))
		{
			l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_nVarSizeonArc1);
			return l_sArcWeightwSize;
		}

		else
			if (foundAVG != std::string::npos)
			{
				for (unsigned int l_nPos = 0; l_nPos < l_ColorVector21.size(); l_nPos++)
				{
					wxString l_sColor = l_ColorVector21[l_nPos];


					std::size_t foundg = l_sTranGuard.find(l_sArcVariable1 + wxT('=') + l_sColor);
					if (foundg != std::string::npos)
					{
						l_svVarnewSizeonArc1.push_back(l_sColor);
					}

				}
				l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_svVarnewSizeonArc1.size());
				if (l_svVarnewSizeonArc1.size() == 0)
				{
					l_sArcWeightwSize = p_sArcWeight + wxT("1");
				}
				return l_sArcWeightwSize;


			}

			else
				if (foundAVG == std::string::npos && foundAVS != std::string::npos)
				{
					l_sArcWeightwSize = p_sArcWeight + wxString::Format(wxT("%ld"), l_ColorVector21.size());
					return l_sArcWeightwSize;
				}
				else
				{
					l_sArcWeightwSize = GetEdgeWeight(p_pcEdge);
					return l_sArcWeightwSize;
				}

	}

	if (l_pcEdgeVector.size() == 0)
	{

		l_sArcWeightwSize = GetEdgeWeight(p_pcEdge);
		return l_sArcWeightwSize;
	}

	return l_sArcWeightwSize;
}


wxString SP_DS_ColPN_Coloring::GetArcVariable(SP_DS_Edge* p_pcEdge)
{

	wxString l_sArcVariable;

	SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));

	wxString l_sArcExpression = l_pcColList1->GetCell(0, 1);

	std::size_t foundEx = l_sArcExpression.find(wxT("dot"));
	std::size_t founddsu = l_sArcExpression.find(wxT("-"));
	std::size_t founddash = l_sArcExpression.find(wxT("`"));
	if (foundEx != std::string::npos)
	{
		l_sArcVariable = wxT("dot");
		return l_sArcVariable;
	}
	else if (founddsu != std::string::npos)
	{
		auto found = l_sArcExpression.find_last_of("-");
		l_sArcVariable = l_sArcExpression.substr(found + 1);
		return l_sArcVariable;
	}
	else if (founddash != std::string::npos)
	{
		auto found = l_sArcExpression.find_last_of("`");
		l_sArcVariable = l_sArcExpression.substr(found + 1);
		return l_sArcVariable;
	}
	else if (founddash == std::string::npos && founddsu == std::string::npos)
	{
		l_sArcVariable = l_sArcExpression;
		return l_sArcVariable;
	}
	else
	{
        auto found = l_sArcExpression.find("++");
        l_sArcVariable = l_sArcExpression.substr(0,found);
	}

	return l_sArcVariable;
}
