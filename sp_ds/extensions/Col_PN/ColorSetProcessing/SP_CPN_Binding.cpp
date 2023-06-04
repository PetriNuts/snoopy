//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: Binding class
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_Binding.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_PlaceMultiSet.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_BindingSelection.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_TransAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStTransAnimator.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include <stack>
#include <cctype>

SP_CPN_Binding::SP_CPN_Binding()
{	
}

SP_CPN_Binding::~SP_CPN_Binding()
{
	m_ParseInputVector.clear();
}

bool SP_CPN_Binding::EnableTest(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick,SP_DS_Animator* p_pcTransAnimator, 
								int p_nBindingChoice, map<wxString, map<SP_DS_Edge*, map<wxString,int> > >& p_mmmBind2Edge2Mult2Color, const std::vector<wxString>& p_vBindingVector)
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	m_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransAnimator->GetAnimObject()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	if( ! m_ValueAssign.InitializeColorset(m_ColorSetClass) )
		return false;

	//Initialize parsing and build parser tree
	if( !BuildParserTree(p_pcExprInfoVector))
		return false;	
	
	//Order the binding patterns so as to get an efficient binding

	Ordering();  // to be replaced

	//begin to binding
	vector<vector<wxString> > l_CompleteBinding;
	SP_MapString2UInt l_IndexMap;

	bool l_bBinding = BindingInference(l_IndexMap,l_CompleteBinding);

	//Disable the transition
	if(!l_bBinding )
		return false;

	//Deal with the case that all arcs are constant colors
	if(l_bBinding && l_IndexMap.size() == 0)   
	{
		if(Parsing())
		{
			if(m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS && m_pcGraph->GetNetclass()->GetName() != SP_DS_FUZZY_ColSPN_CLASS/*added by george */)
			{
				map<SP_DS_Edge*, map<wxString, int> > l_mmEdge2Mult2Color;
				GetBinding(l_mmEdge2Mult2Color);
				wxString l_sTransitionName = m_sTransitionName + wxT("_constant");
				p_mmmBind2Edge2Mult2Color[l_sTransitionName] = l_mmEdge2Mult2Color;
			}
			writecolors(&m_ParseInputVector,p_pcExprInfoVector);
			return true;
		}
		else
			return false;
	}	

	//Deal with the case that has the variables
	//Assign values for variables
	SP_MapString2UInt::iterator itMap;

	vector<vector<SP_CPN_ParseInput> > l_EnabledChoiceList;
	vector<wxString> l_EnabledBindings;	

	map<wxString, int> l_mUnionCheck; // to check the case of union

	for( unsigned i = 0; i < l_CompleteBinding.size(); i++)
	{
		wxString l_sBinding;
		l_sBinding = wxT("");
		//Assign values to variables
		for(itMap = l_IndexMap.begin(); itMap != l_IndexMap.end(); itMap++)
		{
			if( ! m_ColorSetClass.SetVariableValue(itMap->first, l_CompleteBinding[i][itMap->second]) )
				return false;

			l_sBinding = l_sBinding + itMap->first + wxT(" = ") + l_CompleteBinding[i][itMap->second] + wxT(";");

		}
		//Then begin to parse
		if(Parsing())
		{		
			vector<SP_CPN_ParseInput>::iterator itList;
			int l_nCheck = 0;
			int l_nInputArcCount = 0;
			wxString l_sArcEvalSum;  //for testing if there are repeated bindings, for the union case

			for(unsigned i = 0; i < m_ParseInputVector.size(); i++ )
			{
				if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR )
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for(itEvalVector =  m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector !=  m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						l_nCheck = l_nCheck + itEvalVector->m_Multiplicity;
					}
					
					l_nInputArcCount++;
				}		

				
				//deal with the union type, remove redundant transitions 

				if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR || m_ParseInputVector[i].m_eExprType == CPN_OUTPUT_EXPR)
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector; 
					for(itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						if ( itEvalVector->m_Predicate == true ) 
						{
							l_sArcEvalSum += wxString::Format(wxT("%d"),i) + itEvalVector->m_ColorValue + wxString::Format(wxT("%d"),itEvalVector->m_Multiplicity);
						}						
					}					
				}			

				
			}

			//
			if(	l_mUnionCheck.find(l_sArcEvalSum) != l_mUnionCheck.end() )
			{
				continue;   //we do care the same bindings
			}
			else
			{
				l_mUnionCheck[l_sArcEvalSum] = 1; 
			}
			//


			if((l_nInputArcCount == 0) || (l_nInputArcCount != 0 && l_nCheck != 0 ) )
			{
				wxString l_sArc2Color2Num = RemoveRedundantTransInstances();
				if( m_mExistingBindings.find(l_sArc2Color2Num) == m_mExistingBindings.end() )
				{
					m_mExistingBindings[l_sArc2Color2Num] = true;
					l_EnabledChoiceList.push_back(m_ParseInputVector);   //Store all enabled binding choices
					
					if(m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS &&m_pcGraph->GetNetclass()->GetName() != SP_DS_FUZZY_ColSPN_CLASS /*added by george*/ )
					{
						map<SP_DS_Edge*, map<wxString, int> > l_mmEdge2Mult2Color;
						GetBinding(l_mmEdge2Mult2Color);
						p_mmmBind2Edge2Mult2Color[l_sBinding] = l_mmEdge2Mult2Color;
					}

					l_EnabledBindings.push_back(l_sBinding);
				}				
			}
		}		
	}

	//for the variables
	if(l_EnabledChoiceList.size() == 0)
		return false;	

	//decide to show dialog or random, depending on animation mode (single click or run)
	int l_nChose = 0;
	if(p_bSingleClick)
	{
		//call dialog;

		if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS
			|| m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLPN_CLASS)
		{
			SP_DS_CPN_TransAnimator* l_pcCPN_TransAnimator = dynamic_cast<SP_DS_CPN_TransAnimator*>(p_pcTransAnimator);
			l_pcCPN_TransAnimator->ResetSingleClick(false);  //reset singlick flage to false
		}
		else
		if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS /*added by george*/)
		{
			SP_DS_ColStTransAnimator* l_pcCPN_TransAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(p_pcTransAnimator);
			l_pcCPN_TransAnimator->ResetSingleClick(false);  //reset singlick flage to false
		}

		if( p_nBindingChoice == 2 || (p_nBindingChoice == 1 && l_EnabledBindings.size()>1) )
		{
			SP_DLG_BindingSelection* l_pcDlg = new SP_DLG_BindingSelection(l_EnabledBindings, NULL);

			if (l_pcDlg->ShowModal() == wxID_OK)
			{
				l_nChose = (int)(l_pcDlg->GetReturnSelection());
				m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());
				m_OutputVector = l_CompleteBinding[l_nChose];//by george
			}
			else
			{
				l_nChose = 0;
			}
			l_pcDlg->Destroy();
		}
		else if( p_nBindingChoice == 1 && l_EnabledBindings.size()==1 )
		{
			l_nChose = 0;
		}
		else
		{
			//Randomly select one binding to enable
			l_nChose = SP_RandomLong(l_EnabledChoiceList.size());
			m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
			m_OutputVector = l_CompleteBinding[l_nChose];//by george
		}
	}
	else
	{
		if (p_vBindingVector.size() == 0) {
			//Randomly select one binding to enable
			l_nChose = SP_RandomLong(l_EnabledChoiceList.size());
			m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
			m_OutputVector = l_CompleteBinding[l_nChose];//by george
		}
		else {
			//select the index whose binding is given by user
			bool l_bFound = false;
			for (auto l_sSimBinding : p_vBindingVector) {
				for (long i = 0; i < l_CompleteBinding.size(); ++i)
				{
					wxString l_sBindVal;
					for (int j = 0; j < l_CompleteBinding[i].size(); ++j) {
						l_sBindVal << l_CompleteBinding[i][j];
						l_sBindVal << wxT("_");

					}

					if (l_sSimBinding == l_sBindVal.BeforeLast('_')) {
						l_bFound = true;
						l_nChose = i;
						m_OutputVector = p_vBindingVector;
						break;
					}
				}
				if (l_bFound) break;
			}
		}
	}
	if(l_nChose > (int)(l_EnabledChoiceList.size())-1)
	{
		l_nChose = 0;
	}
	if (l_CompleteBinding.size()>0 && m_BindingVector.size()==0 && p_vBindingVector.size()==0)
	{
		m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
		m_OutputVector = l_CompleteBinding[0];//by george
		m_BindingVector.clear();
		for (long i = 0; i < l_CompleteBinding.size(); ++i)
		{
			//l_CompleteBinding
			wxString l_sBindVal;
			for (int j = 0; j < l_CompleteBinding[i].size();++j) {
				l_sBindVal<< l_CompleteBinding[i][j];
				l_sBindVal << wxT("_");
			
			}

			m_BindingVector.push_back(l_sBindVal.BeforeLast('_'));
 		}

		m_OutputVector = m_BindingVector;
 		
	}

	if (p_vBindingVector.size() != 0) {// by george for color simulation, force a transition to fire under a given binding
		m_OutputVector = p_vBindingVector;
		
	}
	writecolors(&(l_EnabledChoiceList[l_nChose]),p_pcExprInfoVector);
	return true;

}


/************************************************************/

bool SP_CPN_Binding::EnableTestForColorSimulation(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick, wxString m_sTransitionName,
	int p_nBindingChoice, map<wxString, map<SP_DS_Edge*, map<wxString, int> > >& p_mmmBind2Edge2Mult2Color)
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	//m_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcTransAnimator->GetAnimObject()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	if (!m_ValueAssign.InitializeColorset(m_ColorSetClass))
		return false;

	//Initialize parsing and build parser tree
	if (!BuildParserTree(p_pcExprInfoVector))
		return false;

	//Order the binding patterns so as to get an efficient binding

	Ordering();  // to be replaced

				 //begin to binding
	vector<vector<wxString> > l_CompleteBinding;
	SP_MapString2UInt l_IndexMap;

	bool l_bBinding = BindingInference(l_IndexMap, l_CompleteBinding);

	//Disable the transition
	if (!l_bBinding)
		return false;

	if (l_bBinding)
		return true;//end of function

	//Deal with the case that all arcs are constant colors
	if (l_bBinding && l_IndexMap.size() == 0)
	{
		if (Parsing())
		{
			if (m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS && m_pcGraph->GetNetclass()->GetName() != SP_DS_FUZZY_ColSPN_CLASS/*added by george */)
			{
				map<SP_DS_Edge*, map<wxString, int> > l_mmEdge2Mult2Color;
				GetBinding(l_mmEdge2Mult2Color);
				wxString l_sTransitionName = m_sTransitionName + wxT("_constant");
				p_mmmBind2Edge2Mult2Color[l_sTransitionName] = l_mmEdge2Mult2Color;
			}
			writecolors(&m_ParseInputVector, p_pcExprInfoVector);
			return true;
		}
		else
			return false;
	}

	//Deal with the case that has the variables
	//Assign values for variables
	SP_MapString2UInt::iterator itMap;

	vector<vector<SP_CPN_ParseInput> > l_EnabledChoiceList;
	vector<wxString> l_EnabledBindings;

	map<wxString, int> l_mUnionCheck; // to check the case of union

	for (unsigned i = 0; i < l_CompleteBinding.size(); i++)
	{
		wxString l_sBinding;
		l_sBinding = wxT("");
		//Assign values to variables
		for (itMap = l_IndexMap.begin(); itMap != l_IndexMap.end(); itMap++)
		{
			if (!m_ColorSetClass.SetVariableValue(itMap->first, l_CompleteBinding[i][itMap->second]))
				return false;

			l_sBinding = l_sBinding + itMap->first + wxT(" = ") + l_CompleteBinding[i][itMap->second] + wxT(";");

		}
		//Then begin to parse
		if (Parsing())
		{
			vector<SP_CPN_ParseInput>::iterator itList;
			int l_nCheck = 0;
			int l_nInputArcCount = 0;
			wxString l_sArcEvalSum;  //for testing if there are repeated bindings, for the union case

			for (unsigned i = 0; i < m_ParseInputVector.size(); i++)
			{
				if (m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR)
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for (itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						l_nCheck = l_nCheck + itEvalVector->m_Multiplicity;
					}

					l_nInputArcCount++;
				}


				//deal with the union type, remove redundant transitions 

				if (m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR || m_ParseInputVector[i].m_eExprType == CPN_OUTPUT_EXPR)
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for (itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						if (itEvalVector->m_Predicate == true)
						{
							l_sArcEvalSum += wxString::Format(wxT("%d"), i) + itEvalVector->m_ColorValue + wxString::Format(wxT("%d"), itEvalVector->m_Multiplicity);
						}
					}
				}


			}

			//
			if (l_mUnionCheck.find(l_sArcEvalSum) != l_mUnionCheck.end())
			{
				continue;   //we do care the same bindings
			}
			else
			{
				l_mUnionCheck[l_sArcEvalSum] = 1;
			}
			//


			if ((l_nInputArcCount == 0) || (l_nInputArcCount != 0 && l_nCheck != 0))
			{
				wxString l_sArc2Color2Num = RemoveRedundantTransInstances();
				if (m_mExistingBindings.find(l_sArc2Color2Num) == m_mExistingBindings.end())
				{
					m_mExistingBindings[l_sArc2Color2Num] = true;
					l_EnabledChoiceList.push_back(m_ParseInputVector);   //Store all enabled binding choices

					if (m_pcGraph->GetNetclass()->GetName() != SP_DS_COLSPN_CLASS &&m_pcGraph->GetNetclass()->GetName() != SP_DS_FUZZY_ColSPN_CLASS /*added by george*/)
					{
						map<SP_DS_Edge*, map<wxString, int> > l_mmEdge2Mult2Color;
						GetBinding(l_mmEdge2Mult2Color);
						p_mmmBind2Edge2Mult2Color[l_sBinding] = l_mmEdge2Mult2Color;
					}

					l_EnabledBindings.push_back(l_sBinding);
				}
			}
		}
	}

	//for the variables
	if (l_EnabledChoiceList.size() == 0)
		return false;



	return true;
	 
	//decide to show dialog or random, depending on animation mode (single click or run)
	//int l_nChose = 0;
	//if (p_bSingleClick)
//	{
	 
		///	if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS /*added by george*/)
		//	{
		//		SP_DS_ColStTransAnimator* l_pcCPN_TransAnimator = dynamic_cast<SP_DS_ColStTransAnimator*>(p_pcTransAnimator);
		//		l_pcCPN_TransAnimator->ResetSingleClick(false);  //reset singlick flage to false
		//	}

		//if (p_nBindingChoice == 2 || (p_nBindingChoice == 1 && l_EnabledBindings.size()>1))
		//{
			//SP_DLG_BindingSelection* l_pcDlg = new SP_DLG_BindingSelection(l_EnabledBindings, NULL);

			//if (l_pcDlg->ShowModal() == wxID_OK)
			//{
				///l_nChose = (int)(l_pcDlg->GetReturnSelection());
				//m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());
				//m_OutputVector = l_CompleteBinding[l_nChose];//by george
			//}
			//else
			//{
				//l_nChose = 0;
			//}
		///	l_pcDlg->Destroy();
		//}
		//else if (p_nBindingChoice == 1 && l_EnabledBindings.size() == 1)
		//{
		//	l_nChose = 0;
		//}
		//else
	//	{
			//Randomly select one binding to enable
			//l_nChose = SP_RandomLong(l_EnabledChoiceList.size());
			//m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
			//m_OutputVector = l_CompleteBinding[l_nChose];//by george
		//}
	//}
	//else
	//{
		//Randomly select one binding to enable
		//l_nChose = SP_RandomLong(l_EnabledChoiceList.size());
		//m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
		//m_OutputVector = l_CompleteBinding[l_nChose];//by george
	//}
	//if (l_nChose > (int)(l_EnabledChoiceList.size()) - 1)
	//{
		//l_nChose = 0;
	//}
	//if (l_CompleteBinding.size()>0 && m_OutputVector.size() == 0)
	//{
		//m_OutputVector.erase(m_OutputVector.begin(), m_OutputVector.end());//george
		//m_OutputVector = l_CompleteBinding[0];//by george
	//}
	//writecolors(&(l_EnabledChoiceList[l_nChose]), p_pcExprInfoVector);
	//return true;

}




/*********************************************************/

void SP_CPN_Binding::GetBinding(map<SP_DS_Edge*, map<wxString, int> >& p_mmEdge2Mult2Color)
{	

			for(unsigned int i = 0; i < m_ParseInputVector.size(); i++)
			{
				if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR || m_ParseInputVector[i].m_eExprType == CPN_OUTPUT_EXPR)
				{							
					vector<SP_CPN_ParsedColors>::iterator itColorList;
					map<wxString, int> l_mMult2Color;

					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for(itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						SP_CPN_ParsedColors l_ParsedColors;								
						int l_nAppearance = itEvalVector->m_Multiplicity;
						wxString l_sColor = itEvalVector->m_ColorValue;						
						l_mMult2Color[l_sColor] = l_nAppearance;
					}					
					p_mmEdge2Mult2Color[m_ParseInputVector[i].m_pcEdge] = l_mMult2Color;
				}
			}
}

bool
SP_CPN_Binding::BuildParserTree(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector)
{
	//m_pcExprInfoVector  = p_pcExprInfoVector;
	SP_CPN_ParseInput l_ParseInputItem;
	vector<SP_CPN_ExpressionInfo>::iterator itExprVector;

	m_ParseInputVector.clear();

	//get the information needed by the binding
	for(itExprVector = p_pcExprInfoVector->begin(); itExprVector != p_pcExprInfoVector->end(); itExprVector++)
	{
		l_ParseInputItem.m_eExprType = itExprVector->m_eExprType;
		l_ParseInputItem.m_pcEdge = itExprVector->m_pcEdge;
		l_ParseInputItem.m_pcPlAnimator = itExprVector->m_pcPlAnimator;
		l_ParseInputItem.m_sExpression = itExprVector->m_sExpression;	
		
		if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS
			|| m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLPN_CLASS)
		{			
			SP_DS_CPN_PlaceAnimator* l_pcCPN_PlaceAnimator = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itExprVector->m_pcPlAnimator);
			if( ! l_pcCPN_PlaceAnimator )
				return false;
			if( ! l_pcCPN_PlaceAnimator->GetPlaceMultiSet())
				return false;
			l_ParseInputItem.m_sColorSetName = l_pcCPN_PlaceAnimator->GetPlaceMultiSet()->GetColorSetName();
		}

		if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS  || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS/*Added by george*/)
		{
			SP_DS_ColStPlaceAnimator* l_pcColStPN_PlaceAnimator = dynamic_cast<SP_DS_ColStPlaceAnimator*>(itExprVector->m_pcPlAnimator);
			if( ! l_pcColStPN_PlaceAnimator )
				return false;
			if( ! l_pcColStPN_PlaceAnimator->GetPlaceMultiSet())
				return false;
			l_ParseInputItem.m_sColorSetName = l_pcColStPN_PlaceAnimator->GetPlaceMultiSet()->GetColorSetName();
		}

		m_ParseInputVector.push_back(l_ParseInputItem);
	}	

	//parsing arc and guard expressions
	vector<SP_CPN_ParseInput>::iterator itList;
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{
		//deal with diffusion function first
		wxString l_sArcExpr= itList->m_sExpression;
		wxString l_sColorSet = itList->m_sColorSetName;

		if( ! Diffusion(l_sColorSet, l_sArcExpr, &m_ColorSetClass) )
			return false;
		itList->m_sExpression = l_sArcExpr;
		//

		//Prepare parsing
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);

		//Set the ColorSetClass and colorset name for the parsing context
		wxString l_sEdgeName = GetEdgeName(itList->m_pcEdge);	
		m_pParseContext->SetErrorPosition(l_sEdgeName);


		m_pParseContext->SetColorSetClass(&m_ColorSetClass);
		m_pParseContext->SetFunctionFlag(false);
		//m_pParseContext->SetNetClassName( m_pcGraph->GetNetclass()->GetName() );
		m_pParseContext->SetPlaceType(wxT("Place"));

		if(itList->m_eExprType != CPN_GUARD_EXPR)		
			m_pParseContext->SetColorSetName(itList->m_sColorSetName);	
		else		
			m_pParseContext->SetColorSetName(wxT(""));
	

		std::string m_ParseString;
		m_ParseString = string( itList->m_sExpression.mb_str());
		bool l_bEval = m_pDriver->parse_string(m_ParseString, "input");	

		if(!l_bEval)
		{
			wxString l_sParseError = m_pDriver->GetError();	
			wxString l_sError = wxT(" in the expression: ") + itList->m_sExpression  ;
			l_sError = l_sParseError + l_sError;

			//SP_MESSAGEBOX(l_sError, wxT("Expression Check"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);

			return false;
		}

		// call the check function
		if( !m_pParseContext->CheckExpression())
			return false;

		//Extract binding pattern from parse tree
		SP_CPN_ExpressionPatternInfo l_PatternInfo;

		//if(itList->m_eExprType != CPN_OUTPUT_EXPR)
		l_PatternInfo = m_pParseContext->ExtractPattern();	

		//Keep parsed information about binding patterns
		//Keep the parse context for later evaluation
		itList->m_pParseContext = m_pParseContext;
		itList->m_pDriver = m_pDriver;

		itList->m_PatternType = l_PatternInfo.m_PatternType;

		//if the free variables lie in the output arcs, then search color sets
		if(itList->m_eExprType == CPN_OUTPUT_EXPR)
			itList->m_PatternType = CPN_PATTERN_NULL;  // for the output arcs

		itList->m_Multiplicity = l_PatternInfo.m_Multiplicity;
		itList->m_MultiSet = l_PatternInfo.m_MultiSet;
		itList->m_VariableName = l_PatternInfo.m_VariableName;


		if(itList->m_PatternType == CPN_PATTERN_MULTISETTUPLE)
		{
			ParseMultiSetTuple(itList->m_sExpression);
		}


		//2010.04.14
		//Get all patterns for the new binding algorithm
/*
		vector<SP_CPN_ExpressionPatternInfo> l_vExprPatterns;

		if(itList->m_eExprType != CPN_OUTPUT_EXPR)
			m_pParseContext->ExtractPatternNew( l_vExprPatterns );

		for(unsigned k = 0; k < l_vExprPatterns.size(); k++)
		{
			if(itList->m_eExprType == CPN_INPUT_EXPR)
			{
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS )
				{
					l_vExprPatterns[k].m_mIniMarking = *(itList->m_pcPlaceAnimator->GetPlaceMultiSet()->GetMultiSetMap());
					l_vExprPatterns[k].m_sColorSet   = itList->m_pcPlaceAnimator->GetPlaceMultiSet()->GetColorSetName();
				}
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS )
				{
					l_vExprPatterns[k].m_mIniMarking = *(itList->m_pcColStPlaceAnimator->GetPlaceMultiSet()->GetMultiSetMap());
					l_vExprPatterns[k].m_sColorSet   = itList->m_pcColStPlaceAnimator->GetPlaceMultiSet()->GetColorSetName();
				}
			}
			m_vExprPatterns.push_back( l_vExprPatterns[k] );
		}

		//to get the all variables of the transition
		SP_MapString2String l_VariableMap;
		l_VariableMap = m_pParseContext->GetVariables();
		SP_MapString2String::iterator itVarMap;
		for( itVarMap = l_VariableMap.begin(); itVarMap != l_VariableMap.end(); itVarMap++)
		{
			m_mTransitionVariables[itVarMap->first] = itVarMap->second;
		}

*/
		//end
		
	}



	return true;
}




//for QPNc
bool SP_CPN_Binding::writecolors(vector<SP_CPN_ParseInput> *p_ParseInputVector ,vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector)
{
	//return the colors to the transition animator	
		p_pcExprInfoVector->clear();
		vector<SP_CPN_ParseInput>::iterator itList;
		for(itList = p_ParseInputVector->begin(); itList != p_ParseInputVector->end(); itList++)
		{	
			if( itList->m_eExprType != CPN_GUARD_EXPR )
			{
				SP_CPN_ExpressionInfo l_ExprInfo;
				l_ExprInfo.m_eExprType = itList->m_eExprType;
				l_ExprInfo.m_pcEdge = itList->m_pcEdge;
				l_ExprInfo.m_pcPlAnimator = itList->m_pcPlAnimator;
				
				vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;				

				for(itEvalVector = itList->m_ParsedInfo.begin(); itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
				{
					SP_CPN_ParsedColors l_ParsedColors;								
					l_ParsedColors.m_nMultiplicity = itEvalVector->m_Multiplicity;
					l_ParsedColors.m_sColorValue = itEvalVector->m_ColorValue;
					l_ExprInfo.m_ParsedColorsList.push_back(l_ParsedColors);
				}

				p_pcExprInfoVector->push_back(l_ExprInfo);
			}
		}

	return true;
}

bool SP_CPN_Binding::Parsing()
{
	vector<SP_CPN_ParseInput>::iterator itList;
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{		
		itList->m_ParsedInfo.clear();

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;

		if( ! itList->m_pParseContext->EvaluateExpression( l_EvaluateResultArray) )
			return false;

		itList->m_ParsedInfo = l_EvaluateResultArray;  //itList->m_pParseContext->EvaluateExpressionOnly();	

		if( itList->m_eExprType == CPN_GUARD_EXPR )
		{
			vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
			itEvalVector = itList->m_ParsedInfo.begin();   //only one guard value
			if(itEvalVector->m_ColorValue == wxT("false"))
				return false;   //disable it if the guard is false
		}
		else
		{
			vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;

			for(itEvalVector = itList->m_ParsedInfo.begin(); itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
			{
				if( itEvalVector->m_Predicate == false )						//modify on 15 Oct. 2009
				{
					itEvalVector->m_Multiplicity = 0;    // let it to be 0 if the predicate is false for the arc expression
				}
			}
		}	
	}

	
	///test if the colors are contained in the markings
	// bool IsMarking( map<wxString, int> p_mSubset )
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{
		if(itList->m_eExprType == CPN_INPUT_EXPR) // for input arc, test if it is contained in the initial markings
		{
			SP_MapString2Int l_mSubset;
			l_mSubset.clear();
			vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
			for(itEvalVector = itList->m_ParsedInfo.begin(); itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
			{
				if( l_mSubset.find(itEvalVector->m_ColorValue) != l_mSubset.end() )
					l_mSubset[itEvalVector->m_ColorValue] = l_mSubset[itEvalVector->m_ColorValue] + itEvalVector->m_Multiplicity;
				else
					l_mSubset[itEvalVector->m_ColorValue] = itEvalVector->m_Multiplicity;
			}
			if(itList->m_pcEdge->GetClassName() == wxT("Edge") ||
				itList->m_pcEdge->GetClassName() == wxT("Read Edge"))
			{
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS
					|| m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLPN_CLASS)	
				{
					if(!(dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->IsMarking(l_mSubset)))
						return false;
				}
				else
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS  || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS)
				{
					if(!(dynamic_cast<SP_DS_ColStPlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->IsMarking(l_mSubset)))
						return false;
				}

				
			}
			else
			if(itList->m_pcEdge->GetClassName() == wxT("Reset Edge"))
			{
				continue;
			}
			else
			if(itList->m_pcEdge->GetClassName() == wxT("Equal Edge"))
			{
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS )	
				{
					if(!(dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->EqualArcTest(l_mSubset)))
						return false;
				}
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS)
				{
					if(!(dynamic_cast<SP_DS_ColStPlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->EqualArcTest(l_mSubset)))
						return false;
				}
				
			}
			else
			if(itList->m_pcEdge->GetClassName() == wxT("Inhibitor Edge"))
			{
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS )	
				{
					if(!(dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->InhibitorArcTest(l_mSubset)))
						return false;
				}
				if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS)
				{
					if(!(dynamic_cast<SP_DS_ColStPlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->InhibitorArcTest(l_mSubset)))
						return false;
				}
				
			} 
			
		}
		if(itList->m_eExprType == CPN_OUTPUT_EXPR)
		{
			// for output arc, test if it is contained in the color set
			wxString l_sColorSetName = itList->m_sColorSetName;
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);

			if( ! l_pcColorSet)
				return false;
			else
			{
				vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
				for(itEvalVector = itList->m_ParsedInfo.begin(); itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
				{
					wxString l_sColor = itEvalVector->m_ColorValue;

					if(itEvalVector->m_Predicate == false)
						continue;					
					
					if(l_pcColorSet->GetDataType()== CPN_UNION )
					{
						if( !l_pcColorSet->IsColor(l_sColor) )
							return false;												
					}
					else
					{
						if( !l_pcColorSet->IsColor(l_sColor) )
							return false;
					}
				}
			}
		}
		
	}

	return true;
}


bool SP_CPN_Binding::Ordering()
{
	//Order should be refined
	//New binding pattern order
	vector<SP_CPN_ParseInput>::iterator itList;

	unsigned l_nIndicator = 0;
	itList = m_ParseInputVector.begin();
	
	while( itList != m_ParseInputVector.end() )
	{
		if(itList->m_eExprType == CPN_INPUT_EXPR)
		{
			if( !(itList->m_PatternType == CPN_PATTERN_VARIABLE||
				itList->m_PatternType == CPN_PATTERN_TUPLE||
				itList->m_PatternType == CPN_PATTERN_KEYTUPLE||
				itList->m_PatternType == CPN_PATTERN_MULTISETTUPLE||
				itList->m_PatternType == CPN_PATTERN_CONSTANT))
			{
				SP_CPN_ParseInput l_cParseInput= *itList;				
				m_ParseInputVector.erase(itList);
				m_ParseInputVector.push_back(l_cParseInput);
			}
		}
			
		itList = m_ParseInputVector.begin();
		for(unsigned i = 0; i < l_nIndicator; i++ )
			itList++;

		l_nIndicator++;
	}


	vector<SP_CPN_ParseInput>::iterator itList2;
	itList2 = m_ParseInputVector.begin();	
	l_nIndicator = 0;
	SP_CPN_ParseInput l_cTempParseInput;
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{
		while( itList2 != m_ParseInputVector.end())
		{
			if((itList2->m_PatternType == CPN_PATTERN_CONSTANT ||itList2->m_PatternType == CPN_PATTERN_TUPLE || itList2->m_PatternType == CPN_PATTERN_KEYTUPLE||itList->m_PatternType == CPN_PATTERN_MULTISETTUPLE) &&
			   !(itList->m_PatternType == CPN_PATTERN_CONSTANT ||itList->m_PatternType == CPN_PATTERN_TUPLE || itList->m_PatternType == CPN_PATTERN_KEYTUPLE ||itList->m_PatternType == CPN_PATTERN_MULTISETTUPLE) )
			{
				l_cTempParseInput = *itList;
				*itList = *itList2;
				*itList2 = l_cTempParseInput;
			}

			itList2++;			
		}

		l_nIndicator++;
		itList2 = m_ParseInputVector.begin();
		for(unsigned i = 0; i < l_nIndicator; i++ )
			itList2++;
	}

	return true;
}

bool SP_CPN_Binding::BindingInference(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_CompleteBinding)
{

	SP_MapString2UInt* l_pIndexMap; //Store the index of ordered variables
	l_pIndexMap = &p_IndexMap;
	SP_MapString2UInt l_IndexMap;
	SP_MapString2Bool l_IndicatorMap;  //indicate which variable has been bound
	unsigned l_nVariableNum ;
	vector<wxString> l_OneBinding;	

	vector<vector<wxString> > l_PartialBindingOld;  // that is already bound
	vector<vector<wxString> > l_PartialBindingNew;  // new binding
	vector<vector<wxString> >* l_pCompleteBinding;  // store the final binding results
	l_pCompleteBinding = &p_CompleteBinding;			 // as the final output if the binding is successful

	///Initialize
	vector<SP_CPN_ParseInput>::iterator itList;
	//Get all the free variabls and order them according to the alphabet
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{	
		SP_MapString2String::iterator itMap;
		for(itMap = itList->m_VariableName.begin(); itMap != itList->m_VariableName.end(); itMap++)
		{
			l_IndexMap[itMap->first] = 0;			//Initialized as 0
			l_IndicatorMap[itMap->first] = false;   //Initialized as false: at the same time
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

	if(l_IndexMap.size() == 0)    //if all arcs are constants then return true and deal with it in other places
		return true;

	///Begin binding inference	
	for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++)
	{
		l_PartialBindingNew.clear();
		//get the initial marking
		SP_MapString2Int* l_MultiSetMap;
		if(itList->m_eExprType == CPN_INPUT_EXPR &&
			(itList->m_pcEdge->GetClassName() == wxT("Edge")|| itList->m_pcEdge->GetClassName() == wxT("Read Edge") || itList->m_pcEdge->GetClassName() == wxT("Equal Edge")))
		{
			if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLEPN_CLASS || m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLPN_CLASS)	
			{
				l_MultiSetMap = dynamic_cast<SP_DS_CPN_PlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->GetMultiSetMap();
			}
			if( m_pcGraph->GetNetclass()->GetName() ==  SP_DS_COLSPN_CLASS  || m_pcGraph->GetNetclass()->GetName() == SP_DS_FUZZY_ColSPN_CLASS)
			{
				l_MultiSetMap = dynamic_cast<SP_DS_ColStPlaceAnimator*>(itList->m_pcPlAnimator)->GetPlaceMultiSet()->GetMultiSetMap();
			}

			for (SP_MapString2Int::iterator itMap1 = l_MultiSetMap->begin(); itMap1 != l_MultiSetMap->end(); ++itMap1)
			{//idd bug fix, by george
				if (itMap1->first.Contains("_") && !itMap1->first.Contains(wxT("(")))
					{
					    wxString l_sColor = wxT("(") + itMap1->first + wxT(")");
						l_sColor.Replace(wxT("_"), wxT(","));
						int l_sSecond = itMap1->second;
						std::pair<wxString, int> l_pair;
						l_pair.first = l_sColor;
						l_pair.second = l_sSecond;
						l_MultiSetMap->insert( l_pair);
						l_MultiSetMap->erase(itMap1);
						itMap1= l_MultiSetMap->begin();

						}
				}

			if(l_MultiSetMap->size() == 0)
			{
				return false;	// tell that this transition cannot be enabled if the initial marking is empty	
			}
		}

		//if an expression is constant,
		if( itList->m_PatternType == CPN_PATTERN_CONSTANT && itList->m_pcEdge->GetClassName() == wxT("Edge"))
		{			
			continue;
		} 
		/*
		//check if a variable has been bound
		if( CheckVariableBinding(itList->m_VariableName,l_IndicatorMap))
		{
			continue;  //that is, it has been bound
		}
		*/

		//if an expression is a variable pattern, 
		if( itList->m_PatternType == CPN_PATTERN_VARIABLE && 
			(itList->m_pcEdge->GetClassName() == wxT("Edge")|| itList->m_pcEdge->GetClassName() == wxT("Read Edge") || itList->m_pcEdge->GetClassName() == wxT("Equal Edge")) )
		{			
			SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(itList->m_sColorSetName);
			wxString l_sVariableName = itList->m_VariableName.begin()->first ;
			SP_CPN_Variable_Constant l_VariableInfo = m_ColorSetClass.GetVariableMap()->find(l_sVariableName)->second;
			
			if(l_pcColorSet && l_pcColorSet->GetDataType() == CPN_UNION)
			{
				SP_CPN_ColorSet* l_pcColorSet1 = m_ColorSetClass.LookupColorSet(l_VariableInfo.m_ColorSet);

				SP_MapString2Int::iterator itMap;
				for( itMap = l_MultiSetMap->begin(); itMap != l_MultiSetMap->end(); itMap++ )
				{
					if( l_pcColorSet1->IsColor(itMap->first) )
					{
						l_OneBinding[l_IndexMap.find(itList->m_VariableName.begin()->first)->second] = itMap->first;
						l_PartialBindingNew.push_back(l_OneBinding);
						l_IndicatorMap[l_IndexMap.find(itList->m_VariableName.begin()->first)->first] = true;
						l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
					}					
				}				
			}		
			else   //deal with non-union variables
			{
				//if the initial marking is not empty, then begin to bind the marking to the variable	
				SP_MapString2Int::iterator itMap;
				for( itMap = l_MultiSetMap->begin(); itMap != l_MultiSetMap->end(); itMap++ )
				{
					l_OneBinding[l_IndexMap.find(itList->m_VariableName.begin()->first)->second] = itMap->first;
					l_PartialBindingNew.push_back(l_OneBinding);
					l_IndicatorMap[l_IndexMap.find(itList->m_VariableName.begin()->first)->first] = true;
					l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
				}	
			}

			CombineBindings(l_PartialBindingNew,p_CompleteBinding);
		}
		//if an expression is a multiset tuple pattern, 
		else if(itList->m_PatternType == CPN_PATTERN_MULTISETTUPLE && 
			(itList->m_pcEdge->GetClassName() == wxT("Edge")|| itList->m_pcEdge->GetClassName() == wxT("Read Edge") ||itList->m_pcEdge->GetClassName() == wxT("Equal Edge")) )
		{
			for(unsigned int i= 0; i < m_vMultiTupleExpression.size(); i++)
			{
				l_PartialBindingNew.clear();
				SP_MapString2Int::iterator itMap;
				for( itMap = l_MultiSetMap->begin(); itMap != l_MultiSetMap->end(); itMap++ )
				{				
					wxString l_VariableString = m_vMultiTupleExpression[i];
					wxString l_ValueString = itMap->first;
					vector<wxString> StringVariable;
					vector<wxString> StringValue;  
					//Match the variable tuple to the corresponding value tuple
					Match(l_VariableString,l_ValueString,StringVariable,StringValue);

					/////////////////////////////////////////////////////////////////////////////
					//Check if it is a right union value
					bool l_bColor = true;
					for(unsigned i= 0; i < StringVariable.size(); i++)
					{
						//check the variable and color		
						if( m_ColorSetClass.GetVariableMap()->find(StringVariable[i]) == m_ColorSetClass.GetVariableMap()->end() )
							return false;
						SP_CPN_Variable_Constant l_VariableInfo = m_ColorSetClass.GetVariableMap()->find(StringVariable[i])->second;
						SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_VariableInfo.m_ColorSet);
						if( ! l_pcColorSet)
							return false;
						if( ! l_pcColorSet->IsColor(StringValue[i]))
						{
							l_bColor = false;
							break;
						}					
					}
					if(! l_bColor)
						continue; // do not push back the stack if it is not a color
					////////////////////////////////////////////////////////////////////////////////
					for(unsigned i= 0; i < StringVariable.size(); i++)
					{
						l_OneBinding[l_IndexMap.find(StringVariable[i])->second] = StringValue[i];
						l_IndicatorMap[l_IndexMap.find(StringVariable[i])->first] = true;
					}
					l_PartialBindingNew.push_back(l_OneBinding);
					l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
				}	
				CombineBindings(l_PartialBindingNew,p_CompleteBinding);
			}
		
		}
		//if an expression is a tuple pattern, 
		else if( itList->m_PatternType == CPN_PATTERN_TUPLE && 
			(itList->m_pcEdge->GetClassName() == wxT("Edge")|| itList->m_pcEdge->GetClassName() == wxT("Read Edge")|| itList->m_pcEdge->GetClassName() == wxT("Equal Edge")) )
		{
			SP_MapString2Int::iterator itMap;
			for( itMap = l_MultiSetMap->begin(); itMap != l_MultiSetMap->end(); itMap++ )
			{				
				wxString l_VariableString = itList->m_sExpression;
				wxString l_ValueString = itMap->first;
				vector<wxString> StringVariable;
				vector<wxString> StringValue;  
				//Match the variable tuple to the corresponding value tuple
				Match(l_VariableString,l_ValueString,StringVariable,StringValue);

				/////////////////////////////////////////////////////////////////////////////
				//Check if it is a right union value
				bool l_bColor = true;
				for(unsigned i= 0; i < StringVariable.size(); i++)
				{
					if( m_ColorSetClass.GetVariableMap()->find(StringVariable[i]) == m_ColorSetClass.GetVariableMap()->end() )
							return false;				
					SP_CPN_Variable_Constant l_VariableInfo = m_ColorSetClass.GetVariableMap()->find(StringVariable[i])->second;
					SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_VariableInfo.m_ColorSet);

					if( ! l_pcColorSet)
						return false;

					if( ! l_pcColorSet->IsColor(StringValue[i]))
					{
						l_bColor = false;
						break;
					}					
				}
				if(! l_bColor)
					continue; // do not push back the stack
				////////////////////////////////////////////////////////////////////////////////
				for(unsigned i= 0; i < StringVariable.size(); i++)
				{
					l_OneBinding[l_IndexMap.find(StringVariable[i])->second] = StringValue[i];
					l_IndicatorMap[l_IndexMap.find(StringVariable[i])->first] = true;
				}
				l_PartialBindingNew.push_back(l_OneBinding);
				l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
			}	
			CombineBindings(l_PartialBindingNew,p_CompleteBinding);
		} 
		//if an expression is a key tuple pattern, 
		else if(itList->m_PatternType == CPN_PATTERN_KEYTUPLE && 
			(itList->m_pcEdge->GetClassName() == wxT("Edge")|| itList->m_pcEdge->GetClassName() == wxT("Read Edge")||itList->m_pcEdge->GetClassName() == wxT("Equal Edge")))
		{
			SP_MapString2Int::iterator itMap;
			for( itMap = l_MultiSetMap->begin(); itMap != l_MultiSetMap->end(); itMap++ )
			{
				wxString l_VariableString = itList->m_sExpression;
				wxString l_ValueString = itMap->first;
				vector<wxString> StringVariable;
				vector<wxString> StringValue;    
				Match(l_VariableString,l_ValueString,StringVariable,StringValue);

				bool l_bMatch = true;

				for(unsigned i= 0; i < StringVariable.size(); i++)
				{
					SP_MapString2UInt::iterator itUnsignedMap;
					itUnsignedMap = l_IndexMap.find(StringVariable[i]);
					if(itUnsignedMap == l_IndexMap.end() && StringVariable[i] != StringValue[i])
					{
						l_bMatch = false;
						break;
					}
				}

				if(l_bMatch)
				{
					for(unsigned i= 0; i < StringVariable.size(); i++)
					{
						SP_MapString2UInt::iterator itUnsignedMap;
						itUnsignedMap = l_IndexMap.find(StringVariable[i]);
						if(itUnsignedMap != l_IndexMap.end())
						{
							l_OneBinding[itUnsignedMap->second] = StringValue[i];
							l_IndicatorMap[l_IndexMap.find(StringVariable[i])->first] = true;
						}
					}
					l_PartialBindingNew.push_back(l_OneBinding);

					l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
				}
			}
			CombineBindings(l_PartialBindingNew,p_CompleteBinding);
		}

		////if an expression is a non-pattern one, 
		else 
		{
			//binding a variable to its color set
			SP_MapString2String::iterator itMap;
			for(itMap =itList->m_VariableName.begin(); itMap != itList->m_VariableName.end(); itMap++)
			{
				l_PartialBindingNew.clear();
				if(l_IndicatorMap.find(itMap->first) != l_IndicatorMap.end() && l_IndicatorMap.find(itMap->first)->second == false)
				{
					vector<wxString> l_ColorVector;
					wxString l_sColorSet = itMap->second;
					SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSet);		
					if( ! l_pcColorSet )
						return false;
					
					l_ColorVector = l_pcColorSet->GetStringValue(); 
					vector<wxString>::iterator itVector;
					for(itVector = l_ColorVector.begin(); itVector != l_ColorVector.end(); itVector++)
					{
						l_OneBinding[l_IndexMap.find(itMap->first)->second] = *itVector;
						l_PartialBindingNew.push_back(l_OneBinding);						
						l_IndicatorMap[itMap->first] = true;
						l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it to "" for each position of the vector 
					}
					CombineBindings(l_PartialBindingNew,p_CompleteBinding);
				}			
			}
		}

		/////////////////
		//Judge if all variables have been bound, if not, continue.
		bool l_bNotFinished = true;
		SP_MapString2Bool::iterator itboolMap;
		for(itboolMap = l_IndicatorMap.begin(); itboolMap != l_IndicatorMap.end(); itboolMap++)
		{
			if(itboolMap->second == false)
			{
				l_bNotFinished = false;
				break;
			}
		}
		if(l_bNotFinished)
		{
			return true;
		}		
	}


	return true;
}

void SP_CPN_Binding::CombineBindings(vector<vector<wxString> > l_vNewBindings, vector<vector<wxString> > &l_vCompleteBindings)
{	
	//for the first time
	if(l_vCompleteBindings.size() == 0)
	{
		l_vCompleteBindings = l_vNewBindings;
		return;
	}

	//from the second time
	bool l_bMatch = true;
	vector<vector<wxString> > l_vOldBindings;
	l_vOldBindings = l_vCompleteBindings;
	l_vCompleteBindings.clear();
	vector<wxString> l_OneBinding;	

	for(unsigned i = 0; i < l_vOldBindings.size(); i++)
	{
		unsigned int l_nVariableNum = l_vOldBindings[0].size();		
		for(unsigned j = 0; j < l_vNewBindings.size(); j++)
		{
			l_OneBinding.assign(l_nVariableNum, wxT(""));
			l_bMatch = true;
			for(unsigned k = 0; k < l_nVariableNum ; k++)
			{
				wxString l_sOld = l_vOldBindings[i][k];
				wxString l_sNew = l_vNewBindings[j][k];
				if(l_sOld == wxT("") || l_sNew == wxT(""))
				{
					l_OneBinding[k] = l_sOld + l_sNew;
				}
				else if(l_sOld == l_sNew)
				{
					l_OneBinding[k] = l_sOld ;
				}
				else
				{
					l_bMatch = false;
					break;
				}
			}
			if(l_bMatch)
			{
				l_vCompleteBindings.push_back(l_OneBinding);
				//l_OneBinding.assign(l_nVariableNum, wxT("")); // reset it  to "" for each position 
			}
		}
	}
}




bool SP_CPN_Binding::Match(wxString p_VariableString, wxString p_ValueString, vector<wxString> &StringVariable, vector<wxString> &StringValue)
{
    
    unsigned l_iChar = 0;
    int l_nLevel =0;    

	/*by george*/
	if (p_VariableString.Contains("`"))
	{
		wxString l_BeforeSep = p_VariableString.BeforeFirst('`');
		if(l_BeforeSep.Contains(wxT("(")))
		{
			wxString l_sAfterSep = p_VariableString.AfterFirst('`');
			l_BeforeSep.Replace(wxT("("), wxT(""));
			l_BeforeSep.Replace(wxT(")"), wxT(""));
			p_VariableString = l_BeforeSep + wxT("`") + l_sAfterSep;
		}

	}

	if (p_VariableString.Freq('(') == p_VariableString.Freq(')') && p_VariableString.Freq(')') > 2)
	{
		while (p_VariableString.Freq('(') != 2)
		{
			p_VariableString.Replace(wxT("("), wxT(""),false);
			p_VariableString.Replace(wxT(")"), wxT(""),false);
		}
	}

    //remove the the first '(' and the last ')'
	p_VariableString  = p_VariableString.AfterFirst(wxT('('));
	p_VariableString  = p_VariableString.BeforeLast(wxT(')'));

    wxChar l_Char = p_VariableString[l_iChar];

    int l_MarkPos = l_iChar;

    while( l_iChar < p_VariableString.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
        else if(l_Char == ',' && l_nLevel == 0  )
        {
            wxString l_SubString = p_VariableString.substr(l_MarkPos,l_iChar - l_MarkPos);           
            if (l_SubString.Find('(') != -1)
            {
                StringVariable.push_back(l_SubString);
                l_MarkPos = l_iChar+1;
            }
            else
            {
                StringVariable.push_back(l_SubString);                
                l_MarkPos = l_iChar+1;
            }            
        }

        l_Char = p_VariableString[++l_iChar];

        if(l_iChar == p_VariableString.size() -1 )
        {
            wxString l_SubString = p_VariableString.substr(l_MarkPos,p_VariableString.size() - l_MarkPos);
            
             if (l_SubString.Find('(') != -1)
            {
                StringVariable.push_back(l_SubString);
            }
            else
            {
                StringVariable.push_back(l_SubString);         
                
            }        
        }
    }


    /////////////////////////////////////////////
    unsigned   l_iChar2 = 0;
    int l_nLevel2 =0;    

 	p_ValueString  = p_ValueString.AfterFirst(wxT('('));
	p_ValueString  = p_ValueString.BeforeLast(wxT(')'));


    wxChar l_Char2 = p_ValueString[l_iChar2];

    int l_MarkPos2 = l_iChar2;

    while( l_iChar2 < p_ValueString.size() )
    {
        if( l_Char2 == '(')
            l_nLevel2 ++;
        else if( l_Char2 == ')')
            l_nLevel2 --;
        else if(l_Char2 == ',' && l_nLevel2 == 0  )
        {
            wxString l_SubString = p_ValueString.substr(l_MarkPos2,l_iChar2 - l_MarkPos2);
			if (l_SubString.Find('(') != -1)
            {
                StringValue.push_back(l_SubString);
                l_MarkPos2 = l_iChar2+1;
            }
            else
            {
                StringValue.push_back(l_SubString);
                l_MarkPos2 = l_iChar2+1;
            }            
        }

        l_Char2 = p_ValueString[++l_iChar2];

        if(l_iChar2 == p_ValueString.size() -1 )
        {
            wxString l_SubString = p_ValueString.substr(l_MarkPos2,p_ValueString.size() - l_MarkPos2);
            if (l_SubString.Find('(') != -1)
            {
                StringValue.push_back(l_SubString);
            }
            else
            {
                StringValue.push_back(l_SubString);                
                
            }        
        }
    }


    //////Prepare recursion
    //
    for(unsigned i = 0; i < StringVariable.size(); i++)
    {
        if( StringVariable[i].find('(') != string::npos )
        {
            wxString S1 = StringVariable[i];
            wxString S2 = StringValue[i];

            vector<wxString>::iterator itReC;
            itReC = StringVariable.begin() + i;

           vector<wxString>::iterator itVal;
            itVal = StringValue.begin() + i;

            StringVariable.erase(itReC);
            StringValue.erase(itVal);

            Match(S1, S2, StringVariable, StringValue);
        }
    }



    return true;
}

bool SP_CPN_Binding::CheckVariableBinding(SP_MapString2String p_mCheckVariables, map<wxString,bool> p_mVariables)
{
	SP_MapString2String::iterator itMap;
	for(itMap = p_mCheckVariables.begin(); itMap != p_mCheckVariables.end(); itMap++)
	{
		if(p_mVariables.find(itMap->first) != p_mVariables.end() && p_mVariables.find(itMap->first)->second == false)
			return false;
	}
	return true;

}

void SP_CPN_Binding::ParseMultiSetTuple(wxString p_sExpr)
{
	m_vMultiTupleExpression.clear();
	p_sExpr.Replace(wxT(" "),wxT(""));
	p_sExpr.Replace(wxT("\n"),wxT(""));
	p_sExpr.Replace(wxT("\t"),wxT(""));
	p_sExpr.Replace(wxT("++"),wxT("#"));

	unsigned l_iChar = 0;
    int l_nLevel =0;     

    wxChar l_Char = p_sExpr[l_iChar];
    int l_MarkPos = l_iChar;

    while( l_iChar < p_sExpr.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
		else if(l_Char == '#' && l_nLevel == 0 )
        {
            wxString l_SubString = p_sExpr.substr(l_MarkPos,l_iChar - l_MarkPos);           
            m_vMultiTupleExpression.push_back(l_SubString);
            l_MarkPos = l_iChar+1;                 
        }		

        l_Char = p_sExpr[++l_iChar];

        if(l_iChar == p_sExpr.size() -1 )
        {
            wxString l_SubString = p_sExpr.substr(l_MarkPos,p_sExpr.size() - l_MarkPos); 
            m_vMultiTupleExpression.push_back(l_SubString);     
        }
    }	
}


























//////////////////////////////////////////////////////////////////
//to be removed
//only for colored stochastic Petri Nets
/////////////////////////////////////////////////////////////////

bool SP_CPN_Binding::EnableTest(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick, SP_DS_ColStTransAnimator* p_pcTransAnimator, SP_DS_Node* p_pcParent, map<wxString,wxString>& p_msBindingsToTransition)
{
	m_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcParent->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	//Initialize parsing and build parser tree

	if( !BuildParserTree(p_pcExprInfoVector))
		return false;	
	
	//Order the binding patterns so as to get an efficient binding
	Ordering();

	//begin to binding
	map<wxString,wxString> l_msBindingsToTransition;

	vector<vector<wxString> > l_CompleteBinding;
	SP_MapString2UInt l_IndexMap;

	bool l_bBinding = BindingInference(l_IndexMap,l_CompleteBinding);
	
	m_vvCompleteBinding = l_CompleteBinding;
	if(!l_bBinding )
		return false;

	vector<SP_CPN_ParseInput>::iterator itList;

	if(l_bBinding && l_IndexMap.size() == 0)   //Deal with the case that all arcs are constant colors to be done
	{
		if(Parsing())
		{
			//To get the constant value
			wxString l_sConstantBinding = wxT("");
			for(itList = m_ParseInputVector.begin(); itList != m_ParseInputVector.end(); itList++ )
			{
				if( itList->m_eExprType == CPN_INPUT_EXPR )
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for(itEvalVector = itList->m_ParsedInfo.begin();  itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
					{
						if(itEvalVector == itList->m_ParsedInfo.begin())
							l_sConstantBinding += itEvalVector->m_ColorValue;
						else
							l_sConstantBinding = l_sConstantBinding +wxT(",")+ itEvalVector->m_ColorValue;
					}
				}				
			}
			//end

			wxString l_sTransitionName = m_sTransitionName + wxT("_constant");
			l_msBindingsToTransition[l_sConstantBinding] = l_sTransitionName;
			p_msBindingsToTransition = l_msBindingsToTransition;
			
			writeConstantcolors(&m_ParseInputVector,p_pcExprInfoVector);

			return true;
		}
		else
			return false;
	}

	//Assign values for variables
	SP_MapString2UInt::iterator itMap;
	vector<vector<SP_CPN_ParseInput> > l_EnabledChoiceList;
	vector<wxString> l_EnabledBindings;
	wxString l_sBinding;

	map<wxString, int> l_mUnionCheck;

	for( unsigned i = 0; i < l_CompleteBinding.size(); i++)
	{
		l_sBinding = wxT("");
		wxString l_sTransInstance = wxT("");
		for(itMap = l_IndexMap.begin(); itMap != l_IndexMap.end(); itMap++)
		{
			if( ! m_ColorSetClass.SetVariableValue(itMap->first, l_CompleteBinding[i][itMap->second]) )
				return false;

			l_sBinding = l_sBinding + itMap->first + wxT(" = ") + l_CompleteBinding[i][itMap->second] + wxT(";");

			if( itMap == l_IndexMap.begin() )
				l_sTransInstance = l_CompleteBinding[i][itMap->second];
			else
			{
				l_sTransInstance = l_sTransInstance + wxT("_") + l_CompleteBinding[i][itMap->second] ;
			}
		}
		//Then begin to parse
		if(Parsing())
		{
		
			//vector<SP_ColStParseInput>::iterator itList;
			int l_nCheck = 0;
			int l_nInputArcCount = 0;
			wxString l_sArcEvalSum;

			for(unsigned i = 0; i < m_ParseInputVector.size(); i++ )
			{
				if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR )
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;
					for(itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						//check if all the input ars are zero?
						l_nCheck = l_nCheck + itEvalVector->m_Multiplicity;
					}
					l_nInputArcCount++;					
				}			
				//deal with the union type, remove redundant transitions 

				if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR || m_ParseInputVector[i].m_eExprType == CPN_OUTPUT_EXPR)
				{
					vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector; 
					for(itEvalVector = m_ParseInputVector[i].m_ParsedInfo.begin(); itEvalVector != m_ParseInputVector[i].m_ParsedInfo.end(); itEvalVector++)
					{
						if ( itEvalVector->m_Predicate == true ) 
						{
							l_sArcEvalSum += wxString::Format(wxT("%d"),i) + itEvalVector->m_ColorValue + wxString::Format(wxT("%d"),itEvalVector->m_Multiplicity);
						}						
					}					
				}		
			}

			//
			if(	l_mUnionCheck.find(l_sArcEvalSum) != l_mUnionCheck.end() )
			{
				continue;   //we do care the same bindings
			}
			else
			{
				l_mUnionCheck[l_sArcEvalSum] = 1; 
			}
			//
			
			if((l_nInputArcCount == 0) || (l_nInputArcCount != 0 && l_nCheck != 0 ) )
			{
				l_EnabledChoiceList.push_back(m_ParseInputVector);   //Store all enabled binding choices		
				l_EnabledBindings.push_back(l_sBinding);
				m_TransitionInstancesVector.push_back(l_sTransInstance);
			}
		}		
	}

	//for the variables
	if(l_EnabledChoiceList.size() == 0)
		return false;
	

	//begin to write the information back to the transition animator

	
	bool l_bFirst = true;
	
	unsigned l_nExprCount = p_pcExprInfoVector->size();
	p_pcExprInfoVector->clear();
	
	for(unsigned i = 0; i < l_nExprCount; i++)
	{
		SP_CPN_ExpressionInfo l_StExprInfo;

		for(unsigned j = 0; j < l_EnabledChoiceList.size(); j++)
		{
			

			
			vector<SP_CPN_EvaluatedSingleValue> l_ParsedInfo = l_EnabledChoiceList[j][i].m_ParsedInfo;

			l_StExprInfo.m_eExprType = l_EnabledChoiceList[j][i].m_eExprType;
			l_StExprInfo.m_pcEdge = l_EnabledChoiceList[j][i].m_pcEdge;
			l_StExprInfo.m_pcPlAnimator = l_EnabledChoiceList[j][i].m_pcPlAnimator;
			l_StExprInfo.m_sExpression = l_EnabledChoiceList[j][i].m_sExpression;
			

			vector<SP_CPN_ParsedColors> l_ParsedColorsVector;
			for(unsigned k = 0; k < l_ParsedInfo.size(); k++)
			{
				SP_CPN_ParsedColors l_ParsedColors;	
				l_ParsedColors.m_nMultiplicity = l_ParsedInfo[k].m_Multiplicity;
				l_ParsedColors.m_sColorValue   = l_ParsedInfo[k].m_ColorValue;
				l_ParsedColorsVector.push_back(l_ParsedColors);
			}

			wxString l_sTransitionName = m_sTransitionName + wxT("_") + m_TransitionInstancesVector[j];
			if(l_bFirst)
				l_msBindingsToTransition[l_EnabledBindings[j]] = l_sTransitionName;

			l_StExprInfo.m_mvParsedColors[l_sTransitionName] = l_ParsedColorsVector;
			
		}
		p_pcExprInfoVector->push_back(l_StExprInfo);

		if(l_bFirst)
			p_msBindingsToTransition = l_msBindingsToTransition;
		l_bFirst = false;
	}





	return true;
}
//to be removed
bool SP_CPN_Binding::writeConstantcolors(vector<SP_CPN_ParseInput> *p_ParseInputList ,vector<SP_CPN_ExpressionInfo>* p_pcStExprInfoVector)
{
	//return the colors to the transition animator
	
		p_pcStExprInfoVector->clear();
		vector<SP_CPN_ParseInput>::iterator itList;
		for(itList = p_ParseInputList->begin(); itList != p_ParseInputList->end(); itList++)
		{	
			if( itList->m_eExprType != CPN_GUARD_EXPR )
			{
				SP_CPN_ExpressionInfo l_ExprInfo;
				l_ExprInfo.m_eExprType = itList->m_eExprType;
				l_ExprInfo.m_pcEdge = itList->m_pcEdge;
				//l_ExprInfo.m_pcPlaceAnimator = itList->m_pcPlaceAnimator;
				l_ExprInfo.m_pcPlAnimator = itList->m_pcPlAnimator;
				
				vector<SP_CPN_EvaluatedSingleValue>::iterator itEvalVector;				

				vector<SP_CPN_ParsedColors> l_ParsedColorsVector;
				for(itEvalVector = itList->m_ParsedInfo.begin(); itEvalVector != itList->m_ParsedInfo.end(); itEvalVector++)
				{
					SP_CPN_ParsedColors l_ParsedColors;								
					l_ParsedColors.m_nMultiplicity = itEvalVector->m_Multiplicity;
					l_ParsedColors.m_sColorValue = itEvalVector->m_ColorValue;					
					l_ParsedColorsVector.push_back(l_ParsedColors);									
				}
				wxString l_sTransitionName = m_sTransitionName + wxT("_constant");	
				l_ExprInfo.m_mvParsedColors[l_sTransitionName] = l_ParsedColorsVector;

				p_pcStExprInfoVector->push_back(l_ExprInfo);
			}
		}

	return true;
}













//new algorithm

bool SP_CPN_Binding::TestMultiplicity()
{

	//vector<SP_CPN_ExpressionPatternInfo> m_vExprPatterns;
	
	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		SP_CPN_ExpressionPatternInfo l_ExprPattern = m_vExprPatterns[i];

		map<wxString,int> l_InitialMarking = l_ExprPattern.m_mIniMarking;

		if( l_ExprPattern.m_PatternType == CPN_PATTERN_CONSTANT) 
		{
			wxString l_sColor = l_ExprPattern.m_Expression;

			if( l_InitialMarking.find(l_sColor) != l_InitialMarking.end() )
			{
				if(l_InitialMarking.find(l_sColor)->second < l_ExprPattern.m_Multiplicity)
					return false;   //the transition is not enabled
			}
			else
			{
				return false;   //the transition is not enabled
			}
			
		}
		if( l_ExprPattern.m_PatternType == CPN_PATTERN_VARIABLE||
					l_ExprPattern.m_PatternType == CPN_PATTERN_TUPLE||
					l_ExprPattern.m_PatternType == CPN_PATTERN_KEYTUPLE	)
		{
			map<wxString,int>::iterator itMap;
			vector<wxString> l_vRemovedTokens;
			for( itMap = l_InitialMarking.begin(); itMap != l_InitialMarking.end(); itMap++)
			{
				if( itMap->second < l_ExprPattern.m_Multiplicity )
				{
					//l_InitialMarking.erase
					l_vRemovedTokens.push_back(itMap->first);
				}
			}
			//delete elements
			for(unsigned j =0; j < l_vRemovedTokens.size(); j++ )
			{
				l_InitialMarking.erase(l_vRemovedTokens[j]);   
			}
			//if all the initial tokens can not satisfy the enabled conditions, then disable the transition
			if( l_InitialMarking.size() ==0 )
			{
				return false;   //the transition is not enabled
			}
			else
			{
				m_vExprPatterns[i].m_mIniMarking = l_InitialMarking;
			}
		}
	}

	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_CONSTANT) 
		{
			m_vExprPatterns.erase (m_vExprPatterns.begin()+i);

			i=0;
		}
	}

	return true;
}

bool SP_CPN_Binding::MergePatterns()
{
	vector<SP_CPN_ExpressionPatternInfo> l_vExprPatternsNew;
	vector<SP_CPN_ExpressionPatternInfo> l_vExprPatternsNoGuard;

	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_VARIABLE||
			m_vExprPatterns[i].m_PatternType == CPN_PATTERN_TUPLE||
			m_vExprPatterns[i].m_PatternType == CPN_PATTERN_KEYTUPLE	)
		{
			l_vExprPatternsNoGuard.push_back(m_vExprPatterns[i]);
		}
		else
		{
			l_vExprPatternsNew.push_back(m_vExprPatterns[i]);
		}
	}

	map<wxString,int> l_mVisitedPatterns;
	for(unsigned i = 0; i < l_vExprPatternsNoGuard.size(); i++)
	{
		if( l_mVisitedPatterns.find( l_vExprPatternsNoGuard[i].m_Expression ) != l_mVisitedPatterns.end() )
			continue;

		l_mVisitedPatterns[l_vExprPatternsNoGuard[i].m_Expression ] = 1;

		bool l_bFound = false;
		SP_CPN_ExpressionPatternInfo l_ExprPattern;
		map<wxString,int>::iterator itMap_i;
		vector<wxString> l_vColors;
		for(itMap_i = l_vExprPatternsNoGuard[i].m_mIniMarking.begin(); itMap_i != l_vExprPatternsNoGuard[i].m_mIniMarking.end(); itMap_i++)
		{
			l_vColors.push_back(itMap_i->first);
		}

		for(unsigned j = 0; j < l_vExprPatternsNoGuard.size(); j++)
		{
			if( (i != j) && (l_vExprPatternsNoGuard[i].m_Expression == l_vExprPatternsNoGuard[j].m_Expression))
			{
				//				
				map<wxString,int>::iterator itMap_j;
				vector<wxString> l_vColors_j;
				for(itMap_j = l_vExprPatternsNoGuard[j].m_mIniMarking.begin(); itMap_j != l_vExprPatternsNoGuard[j].m_mIniMarking.end(); itMap_j++)
				{
					l_vColors_j.push_back(itMap_j->first);
				}
				
				vector<wxString> l_vMergedColors;
				for( unsigned k1 = 0; k1 < l_vColors.size(); k1 ++ )
				{
					for( unsigned k2 = 0; k2 < l_vColors_j.size(); k2 ++ )
					{
						if(l_vColors[k1] == l_vColors_j[k2])
						{
							l_vMergedColors.push_back(l_vColors[k1]);
						}
					}
				}
				if(l_vMergedColors.size() == 0)
				{
					return false; //the transition is not enabled
				}
				else
				{
					l_vColors.clear();
					l_vColors = l_vMergedColors;
				}
				l_bFound = true;
				l_ExprPattern = l_vExprPatternsNoGuard[i];
			}			 
		}

		if( l_bFound )
		{
			l_ExprPattern.m_Multiplicity = 1;
			l_ExprPattern.m_mIniMarking.clear();
			for(unsigned k = 0; k < l_vColors.size(); k++)
				l_ExprPattern.m_mIniMarking[ l_vColors[k] ] = 1;
			l_vExprPatternsNew.push_back(l_ExprPattern);
		}
		else
		{
			l_vExprPatternsNew.push_back(l_vExprPatternsNoGuard[i]);
		}

		l_bFound = false;
	}

	m_vExprPatterns.clear();
	m_vExprPatterns = l_vExprPatternsNew;

	return true;
}


bool SP_CPN_Binding::SortByTokenNumber()
{
	//bubble sort
	bool l_bExchange;

	for( unsigned i = 0; i < m_vExprPatterns.size(); i++ )
	{
		l_bExchange = false;
		for(unsigned j = i+1; j <m_vExprPatterns.size(); j++ )
		{
			if( m_vExprPatterns[j].m_mIniMarking.size() < m_vExprPatterns[i].m_mIniMarking.size())
			{
				l_bExchange = true;

				SP_CPN_ExpressionPatternInfo l_ExprPattern;
				l_ExprPattern = m_vExprPatterns[j];
				m_vExprPatterns[j] = m_vExprPatterns[i];
				m_vExprPatterns[i] = l_ExprPattern;
			}
		}
		if(l_bExchange == false)
			break;
	}

	return true;
}

bool SP_CPN_Binding::SortVariables()
{
	vector<SP_CPN_ExpressionPatternInfo> l_vExprPatternsNew;
	vector<SP_CPN_ExpressionPatternInfo> l_vExprPatternsGuardWithTwoVariables;

	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_GUARD_TYPE2	)
		{
			l_vExprPatternsGuardWithTwoVariables.push_back(m_vExprPatterns[i]);
		}		
	}

	map<wxString, int> l_mVariables;
	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		
		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_GUARD_TYPE1	)
		{
			l_vExprPatternsNew.push_back( m_vExprPatterns[i] );
			wxString l_sVariable = m_vExprPatterns[i] .m_lsExpression;
			l_mVariables[l_sVariable] = 1;
		}

		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_VARIABLE||
					m_vExprPatterns[i].m_PatternType == CPN_PATTERN_TUPLE||
					m_vExprPatterns[i].m_PatternType == CPN_PATTERN_KEYTUPLE) 
		{
			map<wxString, wxString>::iterator itMap; 
			bool l_bNotFound = false;
			for( itMap = m_vExprPatterns[i].m_VariableName.begin(); itMap != m_vExprPatterns[i].m_VariableName.end(); itMap++)
			{
				if( l_mVariables.find(itMap->first) == l_mVariables.end() )
				{
					l_bNotFound = true;
					l_mVariables[itMap->first] = 1;
				}
			}
			if(l_bNotFound)
				l_vExprPatternsNew.push_back( m_vExprPatterns[i] );
		}

		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_GUARD_TYPE2	)
		{
			wxString l_slVariable = m_vExprPatterns[i] .m_lsExpression;
			wxString l_srVariable = m_vExprPatterns[i] .m_rsExpression;

			if(l_mVariables.find(l_slVariable) != l_mVariables.end() || l_mVariables.find(l_srVariable) == l_mVariables.end())
			{
				m_vExprPatterns[i].m_lsExpression = l_srVariable;
				m_vExprPatterns[i].m_rsExpression = l_slVariable;
				l_mVariables[l_srVariable] = 1;
				l_vExprPatternsNew.push_back( m_vExprPatterns[i] );
			}
			if(l_mVariables.find(l_slVariable) == l_mVariables.end() || l_mVariables.find(l_srVariable) != l_mVariables.end())
			{
				l_mVariables[l_slVariable] = 1;
				l_vExprPatternsNew.push_back( m_vExprPatterns[i] );
			}
		}

	}

	m_vExprPatterns.clear();
	m_vExprPatterns = l_vExprPatternsNew;

	return true;
}


bool SP_CPN_Binding::PreProcession()
{
	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		if( m_vExprPatterns[i].m_PatternType == CPN_PATTERN_GUARD_TEST	)
		{
			m_vGuardTestPatterns.push_back( m_vExprPatterns[i] );
			m_vExprPatterns.erase (m_vExprPatterns.begin()+i);
			i=0;
		}
	}

	if( ! TestMultiplicity() )
		return false;

	if( ! MergePatterns() ) 
		return false;

	SortByTokenNumber();
	SortVariables();


	map<wxString,wxString> l_mPatternVariables;
	for(unsigned i = 0; i < m_vExprPatterns.size(); i++)
	{
		map<wxString,wxString>::iterator itMap;
		for(itMap = m_vExprPatterns[i].m_VariableName.begin(); itMap != m_vExprPatterns[i].m_VariableName.end(); itMap++)
		{
			l_mPatternVariables[itMap->first] = itMap->second;
		}
	}

	map<wxString,wxString>::iterator itMap;
	
	for( itMap = m_mTransitionVariables.begin();itMap != m_mTransitionVariables.end();itMap++)
	{
		if( l_mPatternVariables.find(itMap->first) == l_mPatternVariables.end() )
			m_mVariablesBoundbyColorSet[itMap->first] = itMap->second;
	}

	return true;
}



//General problems for both binding and unfolding

bool SP_CPN_Binding::Diffusion(wxString p_sColorSet, wxString &p_sArcExpr, SP_CPN_ColorSetClass* p_pcColorSetClass)
{
	wxString l_sExpression = p_sArcExpr;
	l_sExpression.Replace(wxT(" "), wxT(""));//george

	if (l_sExpression.Contains(wxT("all()")) && l_sExpression.Contains(wxChar(',')))//george temp
		return true;//this case will be checked by the parser

	if (!IsbalanceParentheses(l_sExpression))
	{
	//	SP_MESSAGEBOX("Arc expresion is not correct, check bracket structue please");
		wxString l_sError = wxT("The arc expression: all() function error, bracket structure is not correct ()");
		SP_MESSAGEBOX(l_sError, wxT("Arc Expression checking"), wxOK | wxICON_ERROR);
		SP_LOGERROR(l_sError);
		return false;
	}

	if( l_sExpression.Find( wxT("all"))!= wxNOT_FOUND )
	{
		int l_nEndIndex = 3 + l_sExpression.Find( wxT("all"));
		int l_nLength = l_sExpression.Len();

		if(l_nLength == l_nEndIndex +1 )
		{
			wxString l_sError = wxT("The arc expression: all() function error, lacking ()");			
			SP_MESSAGEBOX(l_sError, wxT("Arc Expression checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}
		wxString l_sSubExpr = l_sExpression.Mid(l_nEndIndex+1);
		wxString l_sBracket = l_sSubExpr.AfterFirst(wxT('('));
		l_sBracket = l_sBracket.BeforeFirst(wxT(')'));

		l_sBracket.Replace(wxT(" "),wxT(""));
		l_sBracket.Replace(wxT("\t"),wxT(""));
		l_sBracket.Replace(wxT("\n"),wxT(""));
		if( l_sBracket != wxT(""))
		{
			wxString l_sError = wxT("The arc expression: all() function error.");			
			SP_MESSAGEBOX(l_sError, wxT("Arc Expression checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		wxString l_sBeforeFunction = l_sExpression.Mid(0,l_sExpression.Find( wxT("all")));
		wxString l_sAfterFunction = l_sSubExpr.AfterFirst(wxT(')'));

		wxString l_sDealString = l_sBeforeFunction;
		l_sDealString.Replace(wxT(" "),wxT(""));
		l_sDealString.Replace(wxT("\t"),wxT(""));
		l_sDealString.Replace(wxT("\n"),wxT(""));
		while( l_sDealString.Find( wxT("++"))!= wxNOT_FOUND )
		{
			l_sDealString = l_sDealString.Mid( l_sDealString.Find( wxT("++")) +2);
		}
		l_sDealString = l_sDealString.AfterLast(wxT(']'));		

		if( ! p_pcColorSetClass->LookupColorSet(p_sColorSet) )
			return false;

		wxString l_sDiffusionRep=wxT("");
		vector<wxString> l_vColors =  p_pcColorSetClass->LookupColorSet(p_sColorSet)->GetStringValue();
		for( unsigned i = 0; i < l_vColors.size(); i++ )
		{
			if(i==0)
				l_sDiffusionRep = l_vColors[i];
			else
				l_sDiffusionRep = l_sDiffusionRep + wxT("++")+l_sDealString + l_vColors[i];
		}

		p_sArcExpr.clear();
		p_sArcExpr = l_sBeforeFunction + l_sDiffusionRep + l_sAfterFunction;

	}
	return true;
}



wxString SP_CPN_Binding::GetEdgeName(SP_DS_Edge* p_pcEdge)
{
	wxString l_sSourceNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	wxString l_sTargetNodeName = dynamic_cast<SP_DS_NameAttribute*>(p_pcEdge->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();			
	p_pcEdge->Update( true);

	wxString l_sEdgeName = l_sSourceNodeName + wxT("->") + l_sTargetNodeName;
	
	return l_sEdgeName;
}


wxString SP_CPN_Binding::RemoveRedundantTransInstances()
{
	map<wxString,wxString > l_mArc2Color2Number;	
	for(unsigned int i = 0; i < m_ParseInputVector.size(); i++)
	{
		map<wxString, int> l_mColor2Number;
		if( m_ParseInputVector[i].m_eExprType == CPN_INPUT_EXPR || m_ParseInputVector[i].m_eExprType == CPN_OUTPUT_EXPR  )
		{
			for(unsigned j = 0; j < m_ParseInputVector[i].m_ParsedInfo.size(); j++)
			{
				l_mColor2Number[m_ParseInputVector[i].m_ParsedInfo[j].m_ColorValue] = m_ParseInputVector[i].m_ParsedInfo[j].m_Multiplicity;
			}
		}
		wxString l_sColor2Num;
		map<wxString, int>::iterator itMap;
		for(itMap = l_mColor2Number.begin(); itMap != l_mColor2Number.end(); itMap++)
		{
			l_sColor2Num = l_sColor2Num + itMap->first + wxString::Format(wxT("%d"),itMap->second);
		}
		l_mArc2Color2Number[m_ParseInputVector[i].m_sExpression] = l_sColor2Num;
	}

	wxString l_sArc2Color2Num;
	map<wxString,wxString >::iterator itMap;
	for(itMap = l_mArc2Color2Number.begin(); itMap != l_mArc2Color2Number.end(); itMap++)
	{
		l_sArc2Color2Num = l_sArc2Color2Num + itMap->first + itMap->second;
	}	
	return 	l_sArc2Color2Num;
}


bool SP_CPN_Binding::IsbalanceParentheses(const wxString& p_sColExp)
{
	stack<char> a;

	std::string exp = p_sColExp.ToStdString();

	for (int i = 0; i < exp.length(); i++)
	{
		if (!(exp[i] == '(' || exp[i] == ')' || exp[i] == '[' || exp[i] == ']'))
		{
			exp[i] = ' ';
		}
	}
	exp.erase(remove_if(exp.begin(), exp.end(), ::isspace), exp.end());

	std::stack<char> stk;
	char x;
	for (int i = 0; i<exp.length(); i++) {
		if (exp[i] == '(' || exp[i] == '[' || exp[i] == '{') {
			stk.push(exp[i]);
			continue;
		}
		if (stk.empty())
			return false;
		switch (exp[i]) {
		case ')':
			x = stk.top();
			stk.pop();
			if (x == '{' || x == '[')
				return false;
			break;
		case '}':
			x = stk.top();
			stk.pop();
			if (x == '(' || x == '[')
				return false;
			break;
		case ']':
			x = stk.top();
			stk.pop();
			if (x == '(' || x == '{')
				return false;
			break;
		}
	}
	return (stk.empty());
}

