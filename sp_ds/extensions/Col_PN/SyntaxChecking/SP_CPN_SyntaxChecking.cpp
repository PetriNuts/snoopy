//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: syntax checking class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_Binding.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessRateFunction.h"
#include <wx/busyinfo.h> 


SP_CPN_SyntaxChecking::SP_CPN_SyntaxChecking()
{
	m_mvCSExpr2PColors.clear();
}

SP_CPN_SyntaxChecking::~SP_CPN_SyntaxChecking()
{
}

bool SP_CPN_SyntaxChecking::Initialize()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!m_pcGraph)
		return false;	

	m_sNetClassName = m_pcGraph->GetNetclass()->GetName();	
	
	if( !m_cValueAssign.InitializeColorset(m_cColorSetClass) )   
		return false;

	return true;
}
	
bool SP_CPN_SyntaxChecking::SyntaxChecking()
{	
	//show state	
	wxBusyInfo wait(wxT("Please wait, syntax checking..."));

	if(!Initialize())
		return false;

	//check places
	if( !CheckPlaceClass(SP_DS_CONTINUOUS_PLACE) )
		return false;
	if( !CheckPlaceClass(SP_DS_DISCRETE_PLACE) )
		return false;
	
	//check transitions
	if( !CheckTransitionClass(SP_DS_CONTINUOUS_TRANS) )
		return false;
	if( !CheckTransitionClass(SP_DS_STOCHASTIC_TRANS) )
		return false;
	if( !CheckTransitionClass(wxT("Immediate Transition")) )
		return false;
	if( !CheckTransitionClass(wxT("Deterministic Transition")) )
		return false;
	if( !CheckTransitionClass(wxT("Scheduled Transition")) )
		return false;		

	return true;
}

bool SP_CPN_SyntaxChecking::CheckPlaceClass(wxString p_sPlaceNCName)
{
	SP_ListNode::const_iterator l_itElem;	
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( p_sPlaceNCName );
	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcPlaceNode = (*l_itElem);	
			map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
			if( ! ComputeInitialMarking(l_pcPlaceNode,l_mColorToMarkingMap) )
				return false;
		}
	}

	return true;
}



bool SP_CPN_SyntaxChecking::CheckTransitionClass(wxString p_sTransNCName)
{
	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( p_sTransNCName );

	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcTransNode = (*l_itElem);
			
			//check arc expressions
			for (l_itEdges = l_pcTransNode->GetTargetEdges()->begin(); l_itEdges != l_pcTransNode->GetTargetEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetSource())
				{					
					SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());					
					SP_DS_Edge* l_pcEdge = *l_itEdges;	
					if( !CheckArcExpression(l_pcPlaceNode,l_pcEdge, l_pcTransNode))
						return false;
				}
			}

			//check arc expressions
			for (l_itEdges = l_pcTransNode->GetSourceEdges()->begin(); l_itEdges != l_pcTransNode->GetSourceEdges()->end(); ++l_itEdges)
			{
				if ((*l_itEdges)->GetTarget())
				{					
					SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());					
					SP_DS_Edge* l_pcEdge = *l_itEdges;	
					if( !CheckArcExpression(l_pcPlaceNode,l_pcEdge, l_pcTransNode))
						return false;
				}
			}

			//check guard
			if( !CheckGuard(l_pcTransNode))
				return false;

			//check rate function
			if( m_sNetClassName ==  SP_DS_COLSPN_CLASS  || m_sNetClassName ==  SP_DS_COLCPN_CLASS || m_sNetClassName ==  SP_DS_COLHPN_CLASS )
			{
				if( !CheckTransRateFunction(l_pcTransNode))
					return false;				
			}
		}
	}

	return true;
}




bool SP_CPN_SyntaxChecking::CheckArcExpression(SP_DS_Node* p_pcPlaceNode,SP_DS_Edge* p_pcEdge,SP_DS_Node* p_pcTransNode)
{
	SP_DS_Node* l_pcPlaceNode = p_pcPlaceNode;

	SP_CPN_ColorSet l_cColorSet;
	if( ! GetPlaceColorSet(p_pcPlaceNode, l_cColorSet) )
		return false;
	wxString l_sColorSetName = l_cColorSet.GetName();
	
	wxString l_sEdgeName = GetEdgeName(p_pcEdge);	

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );	
	if( ! l_pcColList)
		return false;

	wxString l_sExpression = l_pcColList->GetCell(0, 1);

	wxString l_sErrorPosition = wxT("Error Position: ") + l_sExpression + wxT(" | ") + l_sEdgeName;

	l_sExpression.Replace(wxT(" "),wxT(""));
	l_sExpression.Replace(wxT("\t"),wxT(""));
	l_sExpression.Replace(wxT("\n"),wxT(""));

	if( l_sExpression == wxT(""))
	{
		wxString l_sError = wxT("The arc expression is not allowed to be empty. ") + l_sErrorPosition;		
		SP_LOGERROR(l_sError);
		return false;
	}
							
	l_sExpression = l_pcColList->GetCell(0, 1);

	//deal with the diffusion function all() on an arc;
	SP_CPN_Binding l_cBinding;
	if( ! l_cBinding.Diffusion(l_sColorSetName, l_sExpression, &m_cColorSetClass) )
		return false;

	wxString l_sPlaceType = l_pcPlaceNode->GetNodeclass()->GetName();

	if( ! CheckExpression(l_sExpression, l_sErrorPosition,l_sColorSetName, l_sPlaceType, false) )
		return false;

	return true;

}

bool SP_CPN_SyntaxChecking::CheckExpression(wxString p_sExpression, wxString p_sErrorPosition, wxString p_sColorSetName,wxString p_sPlaceType, bool p_bFunctionFlag, bool p_bGuardFlag)
{
	//Prepare to parse and check arc expression
	SP_CPN_Parse_Context_ptr l_pcParseContext;
	SP_CPN_Driver_ptr  l_pcDriver;

	l_pcParseContext = make_CPN_Parse_Context();
	l_pcDriver = make_CPN_Driver((*l_pcParseContext));

	//Set the ColorSetClass and colorset name for the parsing context	
	l_pcParseContext->SetErrorPosition(p_sErrorPosition);
	l_pcParseContext->SetColorSetClass(&m_cColorSetClass);	
	l_pcParseContext->SetColorSetName(p_sColorSetName);	
	l_pcParseContext->SetFunctionFlag(p_bFunctionFlag);	
	l_pcParseContext->SetPlaceType(p_sPlaceType);
	

	std::string l_ParseString;
	l_ParseString = string( p_sExpression.mb_str());
	bool l_bSuccess = l_pcDriver->parse_string(l_ParseString, "input");		

	if(!l_bSuccess)
	{
		wxString l_sParseError = l_pcDriver->GetError();
		wxString l_sError = l_sParseError +  wxT(" | ") + p_sErrorPosition;		
		SP_LOGERROR(l_sError);
		return false;
	}

	// call the check function
	if( !l_pcParseContext->CheckExpression())
		return false;

	if(p_bGuardFlag)
	{
		if( l_pcParseContext->GetHighLevelNodeDataType() != CPN_BOOLEAN )
		{
			wxString l_sError = wxT("Guard should be a boolean expression. ") + p_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}

	return true;
}


bool SP_CPN_SyntaxChecking::CheckGuard(SP_DS_Node* p_pcTransNode)
{
	SP_DS_Node* l_pcTransNode = p_pcTransNode;	
	// To get the name of the transition
	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();	

	wxString l_sGuard;
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
	if(!l_pcColList)
		return false;
	
	l_sGuard = l_pcColList->GetCell(0, 1);	

	//Empty checking
	l_sGuard.Replace(wxT(" "),wxT(""));
	l_sGuard.Replace(wxT("\t"),wxT(""));
	l_sGuard.Replace(wxT("\n"),wxT(""));
	if(l_sGuard == wxT(""))
		return true;

	l_sGuard = l_pcColList->GetCell(0, 1);

	wxString l_sErrorPosition = wxT("Error Position: ") + l_sGuard + wxT(" | ") + l_sTransName;

	if( ! CheckExpression(l_sGuard, l_sErrorPosition,wxT(""), wxT("Place"), false, true) )
		return false;

	return true;
}



bool SP_CPN_SyntaxChecking::Binding(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_AllBindingVector,SP_CPN_ColorSetClass* p_pcColorSetClass)
{

	SP_MapString2UInt* l_pIndexMap; //Store the index of ordered variables
	l_pIndexMap = &p_IndexMap;
	SP_MapString2UInt l_IndexMap;
	SP_MapString2String l_VarColorMap;
	unsigned l_nVariableNum ;
	vector<wxString> l_OneBinding;

	vector<vector<wxString> > l_PartialBindingOld;  // that is already bound
	vector<vector<wxString> > l_PartialBindingNew;  // new binding
	vector<vector<wxString> >* l_pCompleteBinding;  // store the final binding results
	l_pCompleteBinding = &p_AllBindingVector;			 // as the final output if the binding is successful


	vector<SP_CPN_PredicateExpression>::iterator itVector;

	for(itVector = m_ExpressionVector.begin(); itVector != m_ExpressionVector.end(); itVector++)
	{
		//SP_MapString2String::iterator itMap;
		//for(itMap = itVector->m_VariableMap.begin(); itMap != itVector->m_VariableMap.end(); itMap++)
		for(unsigned i =0; i <itVector->m_vVariable2Color.size(); i++ )
		{
			l_IndexMap[itVector->m_vVariable2Color[i].m_sVariable] = 0;			//Initialized as 0
			l_VarColorMap[itVector->m_vVariable2Color[i].m_sVariable] = itVector->m_vVariable2Color[i].m_sColor;
		}
	}

	SP_MapString2UInt::iterator itMap;
	unsigned l_ni = 0;
	//Assign the index for each variable for 0...l_nVariableNum
	for(itMap = l_IndexMap.begin(); itMap != l_IndexMap.end(); itMap++)
	{
		itMap->second = l_ni;
		l_ni++;
	}

	//Get the number of variables
	l_nVariableNum = l_IndexMap.size();
	//Initialized as empty to store each binding
	l_OneBinding.assign(l_nVariableNum, wxT(""));  // Assign the vector with the number of variables

	*l_pIndexMap = l_IndexMap; //as the final output if the binding is successful


	//In the followings, the variable will be bound to color in the colorset

	vector<vector<wxString> > l_InternalBindingNew;
	vector<vector<wxString> > l_InternalBindingOld;

	SP_MapString2String::iterator itSSMap;
	for( itSSMap = l_VarColorMap.begin(); itSSMap != l_VarColorMap.end(); itSSMap++)
	{
		vector<wxString> l_ColorVector;
		wxString l_sColorSet = itSSMap->second;
		SP_CPN_ColorSet* l_pcColorSet = p_pcColorSetClass->LookupColorSet(l_sColorSet);
		if( ! l_pcColorSet )
			return false;
		l_ColorVector = l_pcColorSet->GetStringValue();

		vector<wxString>::iterator itVector;
		for(itVector = l_ColorVector.begin(); itVector != l_ColorVector.end(); itVector++)
		{
			l_OneBinding[l_IndexMap.find(itSSMap->first)->second] = *itVector;
			l_InternalBindingNew.push_back(l_OneBinding);			
			l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it to "" for each position of the vector 
		}

		////
		if(l_InternalBindingOld.size() == 0 )
			l_PartialBindingNew = l_InternalBindingNew;
		for(unsigned i = 0; i < l_InternalBindingOld.size(); i++)
		{
			for(unsigned j = 0; j < l_InternalBindingNew.size(); j++)
			{
				for(unsigned k = 0; k < l_VarColorMap.size() ; k++)
				{
					wxString l_sOld = l_InternalBindingOld[i][k];
					wxString l_sNew = l_InternalBindingNew[j][k];						
					l_OneBinding[k] = l_sOld + l_sNew;
				}
				l_PartialBindingNew.push_back(l_OneBinding);
				l_OneBinding.assign(l_nVariableNum, wxT(""));
			}
		}

		l_InternalBindingOld.clear();
		l_InternalBindingOld = l_PartialBindingNew;
		l_InternalBindingNew.clear();
		l_PartialBindingNew.clear();
	}

	(*l_pCompleteBinding) = l_InternalBindingOld;

	return true;
}



// to be removed
void SP_CPN_SyntaxChecking::SeperateColors( wxString p_sVariableString, vector<wxString> &p_sColorVector )
{
	//deal with the case with a single color like 'b'
	if( p_sVariableString.Find(wxT(","))	== wxNOT_FOUND &&
		p_sVariableString.Find(wxT("("))	== wxNOT_FOUND &&
		p_sVariableString.Find(wxT(")"))	== wxNOT_FOUND)
	{
		p_sColorVector.push_back(p_sVariableString);
		return;
	}

	//deal with other cases
    unsigned l_iChar = 0;
    int l_nLevel =0;    

    wxChar l_Char = p_sVariableString[l_iChar];

    int l_MarkPos = l_iChar;

    while( l_iChar < p_sVariableString.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
        else if(l_Char == ',' && l_nLevel == 0  )
        {
            wxString l_sSubString = p_sVariableString.substr(l_MarkPos,l_iChar - l_MarkPos);    

			p_sColorVector.push_back(l_sSubString);

            l_MarkPos = l_iChar+1;      
        }

        l_Char = p_sVariableString[++l_iChar];

        if(l_iChar == p_sVariableString.size() -1 )
        {
            wxString l_sSubString = p_sVariableString.substr(l_MarkPos,p_sVariableString.size() - l_MarkPos);
			
			p_sColorVector.push_back(l_sSubString);            
        }
    }
}

bool SP_CPN_SyntaxChecking::GenerateSubSetColorSet(wxString p_sSubSetName, wxString p_sColors, wxString p_sFatherColorSetName, SP_CPN_ColorSetClass* p_pcColorSetClass,vector<wxString> &p_vColors)
{
	m_cColorSetClass = *p_pcColorSetClass;

	m_sErrorPosition = p_sColors + wxT(" | ") + p_sSubSetName;
	SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet( p_sFatherColorSetName );
	if( !l_pcColorSet )
	{
		wxString l_sError = wxT(": parent colorset is not defined. ") + m_sErrorPosition;		
		SP_LOGERROR(l_sError);		
		return false;
	}
  
   m_bMarkingCheck = false;

	//////////////////////////////////////////////////////
	if(		p_sColors.Find(wxT(">"))	!= wxNOT_FOUND  ||
			p_sColors.Find(wxT(">="))	!= wxNOT_FOUND ||
			p_sColors.Find(wxT("="))	!= wxNOT_FOUND  ||
			p_sColors.Find(wxT("<="))	!= wxNOT_FOUND ||
			p_sColors.Find(wxT("<"))	!= wxNOT_FOUND ||
			p_sColors.Find(wxT("true"))	!= wxNOT_FOUND ||
			p_sColors.Find(wxT("false"))!= wxNOT_FOUND )
	{
		if ( !ComputeSinglePredicate(p_sColors,l_pcColorSet,p_vColors) )
			return false;
	}
	else
	{
		if( !ComputeOtherCase(p_sColors,p_sSubSetName,l_pcColorSet,p_vColors) )
			return false;
	}

	return true;
}




void SP_CPN_SyntaxChecking::GetPlacetoColorSet(SP_DS_Node* p_pcTransNode,map<wxString, wxString> *p_pmPlaceToColorSet)
{
	SP_ListEdge::const_iterator l_itEdges;		
	SP_DS_TextAttribute* l_pcNameAttibute;

	for (l_itEdges = p_pcTransNode->GetTargetEdges()->begin(); l_itEdges != p_pcTransNode->GetTargetEdges()->end(); ++l_itEdges)
	{
		if ((*l_itEdges)->GetSource())
		{			
			SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());

			l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
			if(!l_pcNameAttibute)
				return;

			wxString l_sColorSetName = l_pcNameAttibute->GetValue();	

			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			(*p_pmPlaceToColorSet)[l_sPlaceName] = l_sColorSetName;
			//end
		}
	}

}


wxString SP_CPN_SyntaxChecking::GetEdgeName(SP_DS_Edge* p_pcEdge)
{
	wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();	
	wxString l_sEdgeName = l_sSourceNodeName + wxT(" -> ") + l_sTargetNodeName;
	return l_sEdgeName;
}


bool SP_CPN_SyntaxChecking::CheckFormula(SP_DS_Node* p_pcTransNode, wxString p_sRateFunExpr, SP_DS_StParser* p_pcParser, bool p_bSingle)
{
	//sum function
	wxString l_sRatefunction = p_sRateFunExpr;
	SP_DS_ColPN_Unfolding l_cUnfolding;
	map<wxString, wxString> l_mPlaceToColorSet;
	GetPlacetoColorSet(p_pcTransNode, &l_mPlaceToColorSet);
	if( ! l_cUnfolding.SumPlaceInstances(&l_mPlaceToColorSet,l_sRatefunction,&m_cColorSetClass,true) )
		return false;

	//check
	wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();	

	map<wxString, SP_ColPN_RateFunction> l_mColPlace2Color;	
	SP_ColPN_ProcessRateFunction l_cProcessRateFunc;
	if( ! l_cProcessRateFunc.CheckColoredRateFunction(l_sRatefunction,p_pcTransNode) )
		return false;
	if( ! l_cProcessRateFunc.ProceedRateFunction(l_sRatefunction,p_pcTransNode,&l_mColPlace2Color) )
		return false;

	map<wxString, SP_ColPN_RateFunction>::iterator itMap;
	for(itMap = l_mColPlace2Color.begin(); itMap != l_mColPlace2Color.end(); itMap++)
	{
		wxString l_sExpression = itMap->second.m_sFormula;
		wxString l_sPlaceName = itMap->second.m_sColPlace;
		wxString l_sColorSetName = l_mPlaceToColorSet[l_sPlaceName];
		if( l_sExpression.Find(wxT("val(")) == wxNOT_FOUND && l_sColorSetName == wxT(""))
			return false;
		if( ! CheckExpression(l_sExpression, l_sTransitionName + wxT(": Rate function"), l_sColorSetName, wxT("Place"), false) )
			return false;
	}

	return true;
}


bool SP_CPN_SyntaxChecking::IsDefFunction(wxString p_sColor)
{
	map<wxString,SP_CPN_Function>::iterator itMap;
	for( itMap = m_cColorSetClass.GetFunctionMap()->begin(); itMap != m_cColorSetClass.GetFunctionMap()->end(); itMap++)
	{
		wxString l_sFuncName = itMap->first;
		if(p_sColor.Find(l_sFuncName) != wxNOT_FOUND)
		{			
			return true;
		}
	}
	
	return false;
}


void SP_CPN_SyntaxChecking::UpdateNetMarking()
{
	UpdateMarkingPlaceClass(SP_DS_CONTINUOUS_PLACE);
	UpdateMarkingPlaceClass(SP_DS_DISCRETE_PLACE);
}

void SP_CPN_SyntaxChecking::UpdateMarkingPlaceClass(wxString p_sPlaceNCName)
{
	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( p_sPlaceNCName );

	if( l_pcNodeclass )
	{
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcPlaceNode = (*l_itElem);

			///////////////////////////////////////////////
			wxString l_sMainMarking = wxT("");
			map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
			
			if( ! ComputeInitialMarking(l_pcPlaceNode,l_mColorToMarkingMap,false) )
				return;
			map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;	
			
			if( p_sPlaceNCName == SP_DS_CONTINUOUS_PLACE)
			{
				double l_dMarking = 0;
				for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
				{		
					l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;			
				}
				l_sMainMarking <<l_dMarking;
			}
			if( p_sPlaceNCName == SP_DS_DISCRETE_PLACE)
			{
				long l_nMarking = 0;
				for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
				{			
					l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;			
				}
				l_sMainMarking <<l_nMarking;
			}
		//////////////////////////////////////////////////

			if( l_pcPlaceNode->GetAttribute( wxT("Marking") ) )
				l_pcPlaceNode->GetAttribute( wxT("Marking") )->SetValueString(l_sMainMarking.c_str());

			l_pcPlaceNode->Update();
		}
	}
}









//new functions

bool SP_CPN_SyntaxChecking::ComputeInitialMarking(SP_DS_Node* p_pcPlaceNode, map<wxString,vector<SP_CPN_TokenNum> >& p_mColorToMarkingMap, bool p_bMarkingCheck)
{
	m_bMarkingCheck = p_bMarkingCheck;

	wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(p_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sPlaceType = p_pcPlaceNode->GetNodeclass()->GetName();
	
	//check the color set
	SP_CPN_ColorSet l_cColorSet;
	if( !GetPlaceColorSet(p_pcPlaceNode, l_cColorSet) )
		return false;
	vector<wxString> l_ColorVector;

	//only for computing marking
	map<wxString,SP_CPN_TokenNum> l_mColor2TokenNumOrg;
	if(!m_bMarkingCheck)
	{
		l_ColorVector = l_cColorSet.GetStringValue();		
		for(unsigned i= 0; i < l_ColorVector.size(); i++)
		{
			l_mColor2TokenNumOrg[ l_ColorVector[i] ].m_intMultiplicity = 0;  //initialized as 0
		}
	}

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcPlaceNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
	if( !l_pcColList )
		return false;

	//if there is no marking defined
	if( l_pcColList->GetRowCount() == 0 )
	{		
		int l_nCount = l_pcColList->GetColCount()/2;
		l_ColorVector = l_cColorSet.GetStringValue();
		SP_CPN_TokenNum l_nNum;
		l_nNum.m_intMultiplicity = 0;
		vector<SP_CPN_TokenNum> l_nColTokens;
		l_nColTokens.assign(l_nCount,l_nNum);
		for(unsigned i= 0; i < l_ColorVector.size(); i++)
		{
			p_mColorToMarkingMap[ l_ColorVector[i] ] = l_nColTokens; 
		}
		return true;
	}

	//if there is initial marking
    for(unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
	{
		map<wxString,SP_CPN_TokenNum> l_mColor2TokenNum;
		l_mColor2TokenNum = l_mColor2TokenNumOrg;

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sColorExpr   = l_pcColList->GetCell(i, j);
			wxString l_sTokenNum = l_pcColList->GetCell(i, j+1);

			m_sErrorPosition = wxT("Error Position: ") + l_sColorExpr + wxT(" | ") + l_sPlaceName;

			SP_CPN_TokenNum l_uTokenNumber;
			if( ! GetTokenNumber(l_sTokenNum,l_uTokenNumber,m_sErrorPosition,l_sPlaceType) )
				return false;

			vector<wxString> l_vParsedColors;
			if( ! ComputeInitialMarkingStep2(l_sColorExpr, l_ColorVector, &l_cColorSet, l_vParsedColors) )
				return false;

			//only for computing marking
			if(!m_bMarkingCheck)
			{
				for(unsigned k = 0; k < l_vParsedColors.size(); k++)
				{
					wxString l_sColor = l_vParsedColors[k];
					 SP_CPN_TokenNum l_uNewTokenNum;
					if( l_sPlaceType == SP_DS_CONTINUOUS_PLACE)
					{
						l_uNewTokenNum.m_DoubleMultiplicity = l_mColor2TokenNum[l_sColor].m_DoubleMultiplicity + l_uTokenNumber.m_DoubleMultiplicity;
						l_mColor2TokenNum[l_sColor] = l_uNewTokenNum;
					}
					else
					{
						l_uNewTokenNum.m_intMultiplicity = l_mColor2TokenNum[l_sColor].m_intMultiplicity + l_uTokenNumber.m_intMultiplicity;		
						l_mColor2TokenNum[l_sColor] = l_uNewTokenNum;
					}
				}		
			}
		}

		//only for computing marking
		if(!m_bMarkingCheck)
		{
			//collect tokens for one column
			map<wxString,SP_CPN_TokenNum>::iterator itMap;
			for( itMap = l_mColor2TokenNum.begin(); itMap != l_mColor2TokenNum.end(); itMap++)
			{
				p_mColorToMarkingMap[itMap->first].push_back(itMap->second);
			}
		}

		j++;
	}

	return true;
}

bool SP_CPN_SyntaxChecking::ComputeInitialMarkingStep2(wxString p_sColorExpr, vector<wxString> p_vColorVector, SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors)
{
	wxString l_sCSName = p_pcColorSet->GetName();
	wxString l_sCSExpr = l_sCSName + wxT(":") + p_sColorExpr;

	//If the expression to be checked is the same as the one that has been checked, then we will directly return the values of that one.
	if( m_mvCSExpr2PColors.find(l_sCSExpr) != m_mvCSExpr2PColors.end() )
	{
		p_vParsedColors = m_mvCSExpr2PColors.find(l_sCSExpr)->second;
		return true;
	}
	//substitute constants
	wxString l_sColorExpr = SubstituteConstants(p_sColorExpr);

	if(     l_sColorExpr.Find(wxT(">"))		!= wxNOT_FOUND ||
			l_sColorExpr.Find(wxT(">="))	!= wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("="))		!= wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("<="))	!= wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("<"))	    != wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("&"))	    != wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("|"))	    != wxNOT_FOUND ||
			l_sColorExpr.Find(wxT("!"))		!= wxNOT_FOUND ||
			IsDefFunction(l_sColorExpr)  )
	{
		//handle predicate
		if( !ComputeSinglePredicate(l_sColorExpr,p_pcColorSet,p_vParsedColors))
			return false;
	}
	else if(l_sColorExpr.Find( wxT("all") ) != wxNOT_FOUND)
	{
		if( !ComputeAllFunction(l_sColorExpr,p_pcColorSet,p_vParsedColors))
			return false;
	}
	else
	{
		if( !ComputeOtherCase(l_sColorExpr,wxT(""),p_pcColorSet,p_vParsedColors))
			return false;
	}

	//add the new expression
	m_mvCSExpr2PColors[l_sCSExpr] = p_vParsedColors;

	return true;
}

wxString
SP_CPN_SyntaxChecking::SubstituteConstants(wxString p_sColorExpr)
{
	wxString l_sColorExpr;
	if(m_cColorSetClass.GetConstantMap())
	{
		wxStringTokenizer l_StringTok(p_sColorExpr, wxT("()+*/^=<>!%&|,-"),  wxTOKEN_RET_EMPTY_ALL );
		while(l_StringTok.HasMoreTokens())
		{
			wxString l_sToken = l_StringTok.GetNextToken();
			if(!l_sToken.IsEmpty())
			{
				std::map<wxString, SP_CPN_Variable_Constant>::const_iterator it = m_cColorSetClass.GetConstantMap()->find(l_sToken);
				if(it != m_cColorSetClass.GetConstantMap()->end())
				{
					SP_CPN_DATATYPE l_DataType  = it->second.m_DataType;
					//now not support real constant
					if (l_DataType != CPN_INTEGER)
					{
						wxString l_sError = wxT(": Constant type error: ") + l_sToken + wxT(" | ") + m_sErrorPosition;
						SP_LOGERROR(l_sError);
						return p_sColorExpr;
					}
					else
					{
						int l_nMarking = it->second.m_IntegerValue;
						l_sColorExpr << l_nMarking;
					}
				}
				else
				{
					l_sColorExpr << l_sToken;
				}
			}
			wxChar l_chDelim = l_StringTok.GetLastDelimiter();
			if(l_chDelim != '\0')
			{
				l_sColorExpr << l_chDelim;
			}
		}
	}
	else
	{
		l_sColorExpr = p_sColorExpr;
	}
	return l_sColorExpr;
}

bool SP_CPN_SyntaxChecking::ComputeSinglePredicate(wxString p_sColorExpr,SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors)
{
		SP_CPN_ColorSet* l_pcColorSet = p_pcColorSet;

		SP_CPN_Parse_Context_ptr l_pcParseContext = make_CPN_Parse_Context();
		SP_CPN_Driver_ptr  l_pcDriver = make_CPN_Driver(*l_pcParseContext);
		
		l_pcParseContext->SetErrorPosition(p_sColorExpr);
		l_pcParseContext->SetColorSetClass(&m_cColorSetClass);
		l_pcParseContext->SetFunctionFlag(false);
		l_pcParseContext->SetColorSetName(wxT(""));		
		l_pcParseContext->SetPlaceType(wxT("Place")); //means also integer, not real

		std::string l_ParseString;
		l_ParseString = string( p_sColorExpr.mb_str());
		bool l_bEval = l_pcDriver->parse_string(l_ParseString, "input");
		if( ! l_bEval)
		{
			wxString l_sParseError = l_pcDriver->GetError();	
			wxString l_sError = l_sParseError + wxT(" : ") + m_sErrorPosition;			
			SP_LOGERROR(l_sError);
			return false;
		}

		if( !l_pcParseContext->CheckExpression())
			return false;

 		if( l_pcParseContext->GetHighLevelNodeDataType() != CPN_BOOLEAN )
		{
			wxString l_sError = wxT("Predicate should be a boolean expression.") wxT(" : ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		vector<SP_CPN_Var_Color>   l_vVariable2Color;
		l_vVariable2Color = l_pcParseContext->GetVariables();		
		for(unsigned i=0; i < l_vVariable2Color.size(); i++)
		{
			if( ! l_pcColorSet->IsVariableofColorSet( l_vVariable2Color[i].m_sColor )  ) 
			{
				wxString l_sError;					
				l_sError = l_vVariable2Color[i].m_sVariable + wxT(" is not defined on the colorset: ") + m_sErrorPosition ;					
				SP_LOGERROR(l_sError);
				return false;
			}
		}

		//only for computing marking
		if(!m_bMarkingCheck)
		{
			//first csp solver			
			bool l_bCSPSucceed = false;
			if( l_vVariable2Color.size() > 0)
			{
				l_bCSPSucceed = CSPSolve(l_pcParseContext,l_pcColorSet,l_vVariable2Color,p_vParsedColors);
			}

			if(!l_bCSPSucceed )
			{			
				if( ! OrdinarySolve(l_pcParseContext,l_pcColorSet, l_vVariable2Color,p_vParsedColors) )
					return false;
			}		
		}

	return true;
}

//for csp solver
bool SP_CPN_SyntaxChecking::IntCSSuccessiveTest(vector<SP_CPN_Var_Color>& p_vVariable2Color)
{	
	//test integer cs and successive
	for(unsigned i = 0; i < p_vVariable2Color.size(); i++)
	{
		wxString l_sVarName = p_vVariable2Color[i].m_sVariable;
		wxString l_sVarCS   = p_vVariable2Color[i].m_sColor;

		SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(l_sVarCS);

		if(l_pcColorSet->GetDataType() != CPN_INTEGER)
			return false;

		if( ! l_pcColorSet->GetIntSuccessiveFlag() )
			return false;
	}

	return true;
}

bool SP_CPN_SyntaxChecking::CSPSolve(SP_CPN_Parse_Context_ptr p_pcParseContext, SP_CPN_ColorSet* p_pcColorSet, vector<SP_CPN_Var_Color>& p_vVariable2Color,vector<wxString> &p_vParsedColors)
{
	if(!IntCSSuccessiveTest(p_vVariable2Color))
		return false;

	SP_CPN_Parse_Context_ptr  l_pcGDParsContext = p_pcParseContext;
	SP_CPN_ColorSet* l_pcColorSet = p_pcColorSet;

	SP_DS_ColCSPSovler *l_pcCSPSolver = new SP_DS_ColCSPSovler();

	//initialize variables
	l_pcCSPSolver->InitializeVarArrary( p_vVariable2Color.size() );

	for(unsigned i = 0; i < p_vVariable2Color.size(); i++)
	{
		wxString l_sVarName = p_vVariable2Color[i].m_sVariable;
		wxString l_sVarCS = p_vVariable2Color[i].m_sColor;
		SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet( l_sVarCS );
		wxString l_sLow = l_pcColorSet->GetStringValue()[0];
		wxString l_sHigh = l_pcColorSet->GetStringValue()[l_pcColorSet->GetStringValue().size()-1];
		long l_nLow, l_nHigh;
		l_sLow.ToLong(&l_nLow);
		l_sHigh.ToLong(&l_nHigh);

		l_pcCSPSolver->InitializeVariable(l_sVarName, i, l_nLow, l_nHigh);
	}

	//get constraints
	vector< vector<wxString> > l_vvsConstraints;
	if( ! l_pcGDParsContext->GetConstraints(*l_pcCSPSolver ) )
		return false;

	l_pcCSPSolver->SetBranch();

	//solve it	
	Gecode::DFS<SP_DS_ColCSPSovler> xy(l_pcCSPSolver);
	wxDELETE( l_pcCSPSolver );
	
	m_vvCSPAssigns.clear();
	while (SP_DS_ColCSPSovler *ss = xy.next())
	{
		//ss->print();
	  vector<unsigned int> l_vOneRowVals;
	  IntVarArray* l_ArrayVariables = ss->GetArrayVariables();
	  for(int k = 0; k < l_ArrayVariables->size(); k++ )
	  {
		  for(Gecode::IntVarValues i((*l_ArrayVariables)[k]); i(); ++i)
		  {
			  l_vOneRowVals.push_back( i.val() );
			  break;
		  }
	  }
	  m_vvCSPAssigns.push_back(l_vOneRowVals);

		delete ss;
	}



	for(unsigned i1 = 0; i1< m_vvCSPAssigns.size(); i1++)
	{		
		map<wxString, wxString> l_mColorSetToValue;
		for(unsigned k1 = 0; k1 < p_vVariable2Color.size(); k1++)
		{
			if( ! m_cColorSetClass.SetVariableValue( p_vVariable2Color[k1].m_sVariable, m_vvCSPAssigns[i1][k1]) )	
				return false;	
			if ( m_cColorSetClass.GetVariableMap()->find(p_vVariable2Color[k1].m_sVariable) != m_cColorSetClass.GetVariableMap()->end() )
			{
				l_mColorSetToValue[ m_cColorSetClass.GetVariableMap()->find(p_vVariable2Color[k1].m_sVariable)->second.m_ColorSet ] = wxString::Format(wxT("%d"),m_vvCSPAssigns[i1][k1]);
			}
		}		

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;		
		if( ! l_pcGDParsContext->EvaluateExpression( l_EvaluateResultArray) )
			return false;

		if(l_EvaluateResultArray.size() > 0 )
		{
			if(l_EvaluateResultArray[0].m_ColorValue  == wxT("true") )
			{						
				l_pcColorSet->SetColorSetClass( &m_cColorSetClass );
				vector<wxString>  l_vColors = l_pcColorSet->GetColorsForMarkingPredicat( l_mColorSetToValue );
				for( unsigned i = 0; i < l_vColors.size(); i++ )
				{
					p_vParsedColors.push_back( l_vColors[i] );
				}
			}
		}		
	}



	return true;
}


bool SP_CPN_SyntaxChecking::OrdinarySolve(SP_CPN_Parse_Context_ptr p_pcParseContext, SP_CPN_ColorSet* p_pcColorSet, vector<SP_CPN_Var_Color>& p_vVariable2Color,vector<wxString> &p_vParsedColors)
{
	SP_CPN_Parse_Context_ptr l_pcParseContext = p_pcParseContext;
	SP_CPN_ColorSet* l_pcColorSet = p_pcColorSet;
			vector<vector<wxString> > l_CompleteBinding;
			SP_MapString2UInt l_IndexMap;

			SP_CPN_PredicateExpression l_PredicateExpression;
			l_PredicateExpression.m_vVariable2Color = p_vVariable2Color;

			m_ExpressionVector.clear();
			m_ExpressionVector.push_back(l_PredicateExpression);

			if( ! Binding(l_IndexMap,l_CompleteBinding,&m_cColorSetClass) )	
				return false;

			//constant
			if(l_IndexMap.size() == 0 )	
			{
				//the constant case get all colors in this color set
				vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;		
				if( ! l_pcParseContext->EvaluateExpression( l_EvaluateResultArray) )
					return false;

				if(l_EvaluateResultArray.size() > 0 )
				{
					if(l_EvaluateResultArray[0].m_ColorValue  == wxT("true") )
					{	
						p_vParsedColors.clear();
						p_vParsedColors = l_pcColorSet->GetStringValue();						
					}
				}
				return true;
			}

			//variables
			SP_MapString2UInt::iterator itSUMap;
			for( unsigned i = 0; i < l_CompleteBinding.size(); i++)
			{
				map<wxString, wxString> l_mColorSetToValue;
				//Assign values for variables

				for(itSUMap = l_IndexMap.begin(); itSUMap != l_IndexMap.end(); itSUMap++)
				{
					if( ! m_cColorSetClass.SetVariableValue(itSUMap->first, l_CompleteBinding[i][itSUMap->second]) )
						return false;

					//
					if ( m_cColorSetClass.GetVariableMap()->find(itSUMap->first) != m_cColorSetClass.GetVariableMap()->end() )
					{
						l_mColorSetToValue[ m_cColorSetClass.GetVariableMap()->find(itSUMap->first)->second.m_ColorSet ] = l_CompleteBinding[i][itSUMap->second];
					}
				}

				//begin of parsing
				vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;		
				if( ! l_pcParseContext->EvaluateExpression( l_EvaluateResultArray) )
					return false;

				if(l_EvaluateResultArray.size() > 0 )
				{
					if(l_EvaluateResultArray[0].m_ColorValue  == wxT("true") )
					{						
						l_pcColorSet->SetColorSetClass( &m_cColorSetClass );
						vector<wxString>  l_vColors = l_pcColorSet->GetColorsForMarkingPredicat( l_mColorSetToValue );
						for( unsigned i = 0; i < l_vColors.size(); i++ )
						{
							p_vParsedColors.push_back( l_vColors[i] );
						}
					}
				}
			}
	return true;
}










bool SP_CPN_SyntaxChecking::ComputeAllFunction(wxString p_sColorExpr,SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors)
{
	wxString l_sColor = p_sColorExpr;
	l_sColor.Replace(wxT(" "),wxT(""));
	l_sColor.Replace(wxT("\t"),wxT(""));
	l_sColor.Replace(wxT("\n"),wxT(""));

	if(l_sColor == wxT("all()"))
	{		
		p_vParsedColors = p_pcColorSet->GetStringValue();		
	}
	else if(l_sColor.Find( wxT("all(") ) != wxNOT_FOUND && l_sColor != wxT("all()"))
	{
		wxString l_sPara = l_sColor.AfterFirst(wxT('('));
		l_sPara = l_sPara.BeforeLast(wxT(')'));

		vector<wxString> l_vComponentName = p_pcColorSet->GetComponentName();
		bool l_bFoundComponent = false;
		for(unsigned k = 0; k < l_vComponentName.size(); k++)
		{
			if( l_sPara == l_vComponentName[k])
			{
				l_bFoundComponent = true;
				break;
			}				
		}
		if(!l_bFoundComponent)
		{
			wxString l_sError = wxT("The usage of all(para) function is wrong. ") + m_sErrorPosition;			
			SP_LOGERROR(l_sError);
			return false;
		}

		SP_CPN_ColorSet* l_pcComponentColorSet = (p_pcColorSet->GetComponentMap())[l_sPara];			
		p_vParsedColors = l_pcComponentColorSet->GetStringValue();		
			
	}
	else
	{
		wxString l_sError = wxT("The usage of all(para) function is wrong. ")  + m_sErrorPosition;			
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}

bool SP_CPN_SyntaxChecking::ComputeOtherCase(wxString p_sColorExpr,wxString p_sSubSetName,SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors)
{
	wxString l_sColor = p_sColorExpr;
	l_sColor.Replace(wxT(" "),wxT(""));
	l_sColor.Replace(wxT("\t"),wxT(""));
	l_sColor.Replace(wxT("\n"),wxT(""));

	if(l_sColor == wxT(""))   
	{
		return true;
	}

	SP_CPN_ColorSet* l_pcColorSet = p_pcColorSet;
	SP_CPN_ColorProcessing l_cColorProcessing;
	SP_CPN_DATATYPE l_sDataType = l_pcColorSet->GetDataType();

	if(l_sDataType == CPN_DOT && p_pcColorSet->IsColor(l_sColor) )
	{
		p_vParsedColors.push_back(l_sColor);
	}
	else if(l_sDataType == CPN_INTEGER )
	{
		vector<unsigned> l_vInt;
		bool l_nUnused;
		l_cColorProcessing.SetColorSetClassFlag(true);
		l_cColorProcessing.SetColorSetClass(&m_cColorSetClass);
		if( ! l_cColorProcessing.ProcessingInt(p_sSubSetName,l_sColor,l_vInt,l_nUnused) )
		{
			return false;
		}
		for(unsigned j = 0; j < l_vInt.size(); j++)
		{
			wxString l_sColor = wxString::Format(wxT("%u"),l_vInt[j]);
			p_vParsedColors.push_back(l_sColor);
		}
	}
	else if(l_sDataType == CPN_STRING )
	{
		vector<wxString> l_vString;
		if( ! l_cColorProcessing.ProcessingString(p_sSubSetName,l_sColor,l_vString) )
		{
			return false;
		}

		// Add double quotes
		for(unsigned j = 0; j < l_vString.size(); j++ )
		{
			wxString l_sColor = wxT("\"") + l_vString[j] + wxT("\"");
			p_vParsedColors.push_back( l_sColor );
		}
	}
	else if( l_sDataType == CPN_ENUM )
	{
		vector<wxString> l_vString;
		if( ! l_cColorProcessing.ProcessingString(p_sSubSetName,l_sColor,l_vString) )
		{
			return false;
		}
		for(unsigned j = 0; j < l_vString.size(); j++ )
		{
			wxChar l_cFirst = l_vString[j][0];
			if( ! (::isalnum(l_cFirst)|| l_cFirst == '_') )
			{
				wxString l_sError = wxT("The color for the enum type should be an identifier.") + m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
			p_vParsedColors.push_back( l_vString[j] );
		}
	}
	else if(l_sDataType == CPN_BOOLEAN && p_pcColorSet->IsColor(l_sColor) )
	{
		p_vParsedColors.push_back( l_sColor );
	}
	else if(l_sDataType == CPN_INDEX )
	{
		vector<unsigned> l_vInt;
		wxString l_sName;

		if(l_sColor.Find(wxT("-")) != wxNOT_FOUND)
		{
			if( ! l_cColorProcessing.ProcessingIndex(p_sSubSetName,l_sColor,l_sName, l_vInt) )
			{
				return false;
			}
			for(unsigned i = 0; i < l_vInt.size(); i++)
			{
				wxString l_sString = l_sName +  wxT("[") + wxString::Format(wxT("%u"),l_vInt[i]) +  wxT("]");
				p_vParsedColors.push_back(l_sString);
			}
		}
		else if(l_sColor.Find(wxT(",")) != wxNOT_FOUND)
		{
			wxString l_sCurrentcolor = l_sColor;
			while(l_sCurrentcolor.Find(wxT(",")) != wxNOT_FOUND)
			{
				wxString l_sOnecolor = l_sCurrentcolor.BeforeFirst(wxT(','));
				p_vParsedColors.push_back(l_sOnecolor);
				l_sCurrentcolor =  l_sCurrentcolor.AfterFirst(wxT(','));
			}
			p_vParsedColors.push_back(l_sCurrentcolor);

		}
		else
		{
			p_vParsedColors.push_back(l_sColor);
		}
	}
	else if(l_sDataType == CPN_UNION || l_sDataType == CPN_PRODUCT)
	{
		vector<wxString> l_sColorVector;
		if(l_sColor.Find(wxT("-")) != wxNOT_FOUND)
		{
			wxString l_sLowColor = l_sColor.BeforeFirst(wxT('-'));
			if( ! p_pcColorSet->IsColor(l_sLowColor) )
			{
				wxString l_sError = wxT("marking definition error ") + m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
			wxString l_sUpColor = l_sColor.AfterFirst(wxT('-'));
			if( ! p_pcColorSet->IsColor(l_sUpColor) )
			{
				wxString l_sError = wxT("marking definition error ") + m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
			unsigned l_nLowBound, l_nUpBound,l_nExchange;
			for(unsigned int l_nIndex = 0; l_nIndex <p_pcColorSet->GetStringValue().size(); l_nIndex++)
			{
				if(l_sLowColor == p_pcColorSet->GetStringValue()[l_nIndex])
					l_nLowBound = l_nIndex;
				if(l_sUpColor == p_pcColorSet->GetStringValue()[l_nIndex])
					l_nUpBound = l_nIndex;
			}
			if(l_nUpBound < l_nLowBound)
			{
				l_nExchange = l_nUpBound;
				l_nUpBound = l_nLowBound;
				l_nLowBound = l_nExchange;
			}
			for(unsigned int l_nIndex = l_nLowBound; l_nIndex <= l_nUpBound; l_nIndex++)
			{
				p_vParsedColors.push_back( p_pcColorSet->GetStringValue()[l_nIndex] );
			}

		}
		else
		{
			SeperateColors( l_sColor, l_sColorVector );
			for( unsigned j = 0; j < l_sColorVector.size(); j++)
			{
				wxString l_sProcessColor = l_sColorVector[j];
				if(l_sProcessColor == wxT(""))
				{
					continue;
				}

				p_vParsedColors.push_back( l_sProcessColor );
			}
		}
	}
	else
	{
		wxString l_sError = wxT("marking definition error ") + m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	for(unsigned i= 0; i < p_vParsedColors.size(); i++)
	{
		if( ! p_pcColorSet->IsColor(p_vParsedColors[i]) )
		{
			wxString l_sError = wxT("marking definition error ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}
		

	return true;
}

bool SP_CPN_SyntaxChecking::GetTokenNumber(wxString p_sTokenNumber,SP_CPN_TokenNum &p_uNumber, wxString p_sErrorPosition, wxString p_sPlaceType)
{
	wxString l_sTokenNumber = p_sTokenNumber;
	double l_doubleMarking;  //colored continuous Petri nets
	long    l_longMarking;     //other colored netclasses

	if( p_sPlaceType == SP_DS_CONTINUOUS_PLACE && l_sTokenNumber.ToDouble(&l_doubleMarking))
	{								

	}
	//for other colored PN
	else if( p_sPlaceType == SP_DS_DISCRETE_PLACE && l_sTokenNumber.ToLong(&l_longMarking) )
	{

	}
	else
	{
		wxString l_sConstant = l_sTokenNumber;
		l_sConstant.Replace(wxT(" "),wxT(""));
		l_sConstant.Replace(wxT("\t"),wxT(""));
		l_sConstant.Replace(wxT("\n"),wxT(""));

		if(l_sConstant == wxT(""))
		{
			l_doubleMarking = 0;
			l_longMarking   = 0;
		}
		else if(m_cColorSetClass.GetConstantMap() && m_cColorSetClass.GetConstantMap()->find(l_sConstant) != m_cColorSetClass.GetConstantMap()->end())		
		{
			SP_CPN_DATATYPE l_DataType  = m_cColorSetClass.GetConstantMap()->find(l_sConstant)->second.m_DataType;
			//now not support real constant
			if (l_DataType != CPN_INTEGER)
			{
				wxString l_sError = wxT(": Constant type error: ") + l_sConstant + wxT(" | ") + p_sErrorPosition;					
				SP_LOGERROR(l_sError);
				return false;
			}
			else
			{
				int l_nMarking = m_cColorSetClass.GetConstantMap()->find(l_sConstant)->second.m_IntegerValue;
				if( p_sPlaceType == SP_DS_CONTINUOUS_PLACE)	
					l_doubleMarking = (double)l_nMarking;
				else
					l_longMarking  = l_nMarking;
			}
		}
		else
		{
			wxString l_sError = wxT(": Constant undefined: ") + l_sConstant + wxT(" | ") + p_sErrorPosition;			
			SP_LOGERROR(l_sError);
			return false;
		}
	}

	if( p_sPlaceType == SP_DS_CONTINUOUS_PLACE)	
		p_uNumber.m_DoubleMultiplicity = l_doubleMarking;
	else
		p_uNumber.m_intMultiplicity = (int)l_longMarking;

	return true;
}



bool SP_CPN_SyntaxChecking::CheckTransRateFunction(SP_DS_Node* p_pcTransNode)
{
	SP_DS_Node* l_pcTransNode = p_pcTransNode;	

	// get the collist
	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	SP_DS_ColListAttribute* l_pcColList = nullptr;
	wxString l_sNodeClassName = p_pcTransNode->GetNodeclass()->GetName();

	if( l_sNodeClassName == SP_DS_STOCHASTIC_TRANS  || l_sNodeClassName == SP_DS_CONTINUOUS_TRANS)
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(wxT("FunctionList")) );
	}
	else if ( l_sNodeClassName == wxT("Immediate Transition") )
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(wxT("FunctionList")) );
	}
	else if( l_sNodeClassName == wxT("Deterministic Transition") )
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(wxT("DelayList")) );
	}
	else if( l_sNodeClassName == wxT("Scheduled Transition") )
	{
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(wxT("PeriodicList")) );
	}
	if(!l_pcColList)
		return false;


	//begin to check
	bool l_nTrueCheck = false;
	int  l_nPredNumber = 0;
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		//deal with predicates
		wxString l_sPredicate = l_pcColList->GetCell(i, 0);
		wxString l_sErrorPosition = wxT("Error Position: ") + l_sPredicate + wxT(" | ") + l_sTransName;

		//if the predicate in a row is empty, this row will be omited.
		wxString l_sEmptyCheck = l_sPredicate;
		l_sEmptyCheck.Replace(wxT(" "),wxT(""));
		l_sEmptyCheck.Replace(wxT("\t"),wxT(""));
		l_sEmptyCheck.Replace(wxT("\n"),wxT(""));

		if(l_sEmptyCheck==wxT(""))
			continue;
		if(l_sEmptyCheck==wxT("true"))
			l_nTrueCheck = true;

		l_nPredNumber++;

		if( !CheckPredicateInRateFunction(l_sPredicate,l_sErrorPosition) )
			return false;		

		//deal with rate functions.
		for(unsigned j = 1; j < l_pcColList->GetColCount(); j++)
		{
			if( l_sNodeClassName == SP_DS_STOCHASTIC_TRANS || l_sNodeClassName == SP_DS_CONTINUOUS_TRANS )
			{
				wxString l_sRatefunction = l_pcColList->GetCell(i, j);
				SP_DS_StParser l_cParser;
				if( ! CheckFormula(p_pcTransNode,l_sRatefunction,&l_cParser) )
					return false;
			}

			if( l_sNodeClassName == wxT("Immediate Transition") )
			{
				wxString l_sWeight = wxT("ImmediateFiring(") + l_pcColList->GetCell(i, j) + wxT(")");
				if( !CheckIDSRateFunction(l_sWeight,l_pcTransNode) )
					return false;
			}

			if( l_sNodeClassName == wxT("Deterministic Transition") )
			{
				wxString l_sDelay = wxT("TimedFiring(")+ l_pcColList->GetCell(i, j) +wxT(")");
				if( !CheckIDSRateFunction(l_sDelay,l_pcTransNode) )
					return false;
			}

			if( l_sNodeClassName == wxT("Scheduled Transition") )
			{
				wxString l_Begin=l_pcColList->GetCell(i, j);
				wxString l_Repetition=l_pcColList->GetCell(i, ++j);  
				wxString l_End=l_pcColList->GetCell(i, ++j);
				wxString l_sDelay = wxT("FixedTimedFiring_Periodic(")+ l_Begin +wxT(",")+ l_Repetition+wxT(",")+l_End+wxT(")");
				if( !CheckIDSRateFunction(l_sDelay,l_pcTransNode) )
					return false;
			}
		}
	}	

	if(l_nTrueCheck && l_nPredNumber > 1 )
	{
		wxString l_sError = wxT("Predicate true should be unique in the rate function. Error position: ") + l_sTransName;		
		SP_LOGERROR(l_sError);
		return false;
	}

	if(l_nPredNumber < 1 )
	{
		wxString l_sError = wxT("Predicates should be greater than or equal to one in the rate function. Error position: ") + l_sTransName;		
		SP_LOGERROR(l_sError);
		return false;
	}	

	return true;
}


bool SP_CPN_SyntaxChecking::CheckPredicateInRateFunction(wxString p_sPredicate, wxString p_sErrorPosition)
{
	wxString l_sPredicate = p_sPredicate;	

	//begin to deal with predicates
	if( ! CheckExpression(l_sPredicate, p_sErrorPosition,wxT(""), wxT("Place"), false, true) )
		return false;

	return true;
}

bool SP_CPN_SyntaxChecking::CheckIDSRateFunction(wxString p_sRate, SP_DS_Node* p_pcTransNode)
{
	wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sErrorPosition = wxT(" Error position: ") + p_sRate + wxT(" | ") + l_sTransName;
	
	SP_DS_StParser l_cParser;

	if (!(l_cParser.CheckFormulaFunction(p_sRate, p_pcTransNode,true)))
	{
		wxString l_sVariables = wxT("The formula \"");		
		l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

		wxArrayString* l_plMissingVariableList = 0;
		l_plMissingVariableList = l_cParser.GetMissingVariableList();
		for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
		{
			l_sVariables += ( *l_plMissingVariableList )[ i ];
			l_sVariables += wxT("\n");
		}

		wxString l_sError = l_sVariables + l_sErrorPosition; 					
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}

bool SP_CPN_SyntaxChecking::GetPlaceColorSet(SP_DS_Node* p_pcPlaceNode,SP_CPN_ColorSet  &p_cColorSet)
{
	if( ! p_pcPlaceNode ) 
		return false;	
	wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(p_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	if(!l_pcNameAttibute)
		return false;
	wxString l_sColorSetName = l_pcNameAttibute->GetValue();

	//Checking color set for a place
	l_sColorSetName.Replace(wxT(" "),wxT(""));
	l_sColorSetName.Replace(wxT("\t"),wxT(""));
	l_sColorSetName.Replace(wxT("\n"),wxT(""));

	if(l_sColorSetName == wxT(""))
	{
		wxString l_sError =  wxT("Colorset is not allowed empty. Error occurs in Place: ") + l_sPlaceName;			
		SP_LOGERROR(l_sError);
		return false;
	}
	
	SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(l_sColorSetName);	
	if(!l_pcColorSet)
	{
		wxString l_sError = wxT("Place: ") + l_sPlaceName +wxT(": ")+ l_sColorSetName + wxT(" is not right.");	
		SP_LOGERROR(l_sError);		
		return false;
	}	

	p_cColorSet = *l_pcColorSet;

	return true;
}


bool SP_CPN_SyntaxChecking::ComputeRandomMarking(wxString p_sColorSetName, vector< vector<wxString> > p_vvColorExprs, map<wxString,vector<SP_CPN_TokenNum> >& p_mColorToMarkingMap)
{
	m_bMarkingCheck = false;	

	vector<wxString> l_ColorVector;		
	SP_CPN_ColorSet* l_pcColorSet = m_cColorSetClass.LookupColorSet(p_sColorSetName);
	if( ! l_pcColorSet )
		return false;

	//only for computing marking
	map<wxString,SP_CPN_TokenNum> l_mColor2TokenNumOrg;
	
	l_ColorVector = l_pcColorSet->GetStringValue();		
	for(unsigned i= 0; i < l_ColorVector.size(); i++)
	{
		l_mColor2TokenNumOrg[ l_ColorVector[i] ].m_intMultiplicity = 0;  //initialized as 0
	}
	
	if(p_vvColorExprs.size() == 0)
		return true;


	//if there is initial marking
	for(unsigned int j = 0; j < p_vvColorExprs[0].size(); j++)
	{
		map<wxString,SP_CPN_TokenNum> l_mColor2TokenNum;
		l_mColor2TokenNum = l_mColor2TokenNumOrg;

		for (unsigned int i = 0; i < p_vvColorExprs.size(); i++)
		{
			wxString l_sColorExpr   = p_vvColorExprs[i][j];
			wxString l_sTokenNum    = p_vvColorExprs[i][j+1];

			m_sErrorPosition = wxT("Error Position: ") + l_sColorExpr + wxT(" | Random marking.") ;

			SP_CPN_TokenNum l_uTokenNumber;
			if( ! GetTokenNumber(l_sTokenNum,l_uTokenNumber,m_sErrorPosition, wxT("Place")) ) // only allowed for stochastic net
				return false;

			vector<wxString> l_vParsedColors;
			if( ! ComputeInitialMarkingStep2(l_sColorExpr, l_ColorVector, l_pcColorSet, l_vParsedColors) )
				return false;

			//only for computing marking
			if(!m_bMarkingCheck)
			{
				for(unsigned k = 0; k < l_vParsedColors.size(); k++)
				{
					wxString l_sColor = l_vParsedColors[k];
					 SP_CPN_TokenNum l_uNewTokenNum;
					if( m_sNetClassName == SP_DS_COLCPN_CLASS)   //need to change
					{
						l_uNewTokenNum.m_DoubleMultiplicity = l_mColor2TokenNum[l_sColor].m_DoubleMultiplicity + l_uTokenNumber.m_DoubleMultiplicity;
						l_mColor2TokenNum[l_sColor] = l_uNewTokenNum;
					}
					else
					{
						l_uNewTokenNum.m_intMultiplicity = l_mColor2TokenNum[l_sColor].m_intMultiplicity + l_uTokenNumber.m_intMultiplicity;		
						l_mColor2TokenNum[l_sColor] = l_uNewTokenNum;
					}
				}		
			}
		}

		//only for computing marking
		if(!m_bMarkingCheck)
		{
			//collect tokens for one column
			map<wxString,SP_CPN_TokenNum>::iterator itMap;
			for( itMap = l_mColor2TokenNum.begin(); itMap != l_mColor2TokenNum.end(); itMap++)
			{
				p_mColorToMarkingMap[itMap->first].push_back(itMap->second);
			}
		}

		j++;
	}

	return true;
}
