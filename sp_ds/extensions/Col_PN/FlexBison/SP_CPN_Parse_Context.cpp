//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// $Updated:by george assaf with new operations for colored PN
// Short Description: Binding class
//////////////////////////////////////////////////////////////////////


#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"
#include "sp_core/SP_Core.h"
#include <algorithm>

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
class SP_Core;

void SP_CPN_ParseNode::CollectResult()
{
	m_ParseNode_Info.m_EvalResults.clear();
	SP_CPN_EvaluatedSingleValue l_stEvalRes;
	SP_CPN_DATATYPE l_DataType = m_ParseNode_Info.m_DataType;

	if (m_pLeft)//george 19.12
	{

		if (m_pLeft->GetLeftNode())
		{
			if (m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE|| m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)//george 19.12

			{
				m_ParseNode_Info.m_EvalResults = m_pLeft->GetParseNodeInfo()->m_EvalResults;
				return;
			}
		}
	}
    //george12.2020
	bool l_bIsAllExp = false;
	IsAllNodeExists(m_pLeft, l_bIsAllExp);
	if (l_bIsAllExp)
	{
		for (auto itV = m_pLeft->GetParseNodeInfo()->m_EvalResults.begin(); itV != m_pLeft->GetParseNodeInfo()->m_EvalResults.end(); ++itV)
		{
			 (*itV).m_ColorValue.Replace(wxT("("), wxT(""));
			 (*itV).m_ColorValue.Replace(wxT(")"), wxT(""));
			 wxString l_sTupelColour = wxT("(") + (*itV).m_ColorValue + wxT(")");
			 (*itV).m_ColorValue = l_sTupelColour;
		}
		m_ParseNode_Info.m_EvalResults = m_pLeft->GetParseNodeInfo()->m_EvalResults;
	
		l_stEvalRes.m_Predicate = m_ParseNode_Info.m_Predicate;
		l_stEvalRes.m_bPlaceFlag = m_ParseNode_Info.m_bPlaceFlag;
		return;
	}
	if(l_DataType == CPN_INTEGER)
	{
		wxString l_sTemp;
		l_sTemp << m_ParseNode_Info.m_IntegerValue;
		l_stEvalRes.m_ColorValue = l_sTemp; //wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue);
	}

	 
	else if(l_DataType == CPN_BOOLEAN)
	{	
		if(m_ParseNode_Info.m_BooleanValue == true)
			l_stEvalRes.m_ColorValue = wxT("true");
		else
			l_stEvalRes.m_ColorValue = wxT("false");
	}
	else
	{
		l_stEvalRes.m_ColorValue = *(m_ParseNode_Info.m_StringValue);
	}

	if( m_sPlaceType == SP_DS_CONTINUOUS_PLACE )
		l_stEvalRes.m_DoubleMultiplicity = m_ParseNode_Info.m_DoubleMultiplicity;
	else
		l_stEvalRes.m_Multiplicity = m_ParseNode_Info.m_Multiplicity;
	
	l_stEvalRes.m_Predicate = m_ParseNode_Info.m_Predicate;
	l_stEvalRes.m_bPlaceFlag = m_ParseNode_Info.m_bPlaceFlag;
	m_ParseNode_Info.m_EvalResults.push_back(l_stEvalRes);

}

bool SP_CPN_Parse_Integer_Node::check()
{	
	if (m_ParseNode_Info.m_bSeparaterExpression)
	{
		if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			m_ParseNode_Info.m_DoubleMultiplicity = m_ParseNode_Info.m_IntegerValue;
		else
			m_ParseNode_Info.m_Multiplicity = m_ParseNode_Info.m_IntegerValue;
	}

	if(m_ParseNode_Info.m_CheckColorSet != wxT(""))
	{
		SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
		if( ! l_pcColorSet )
			return false;

		if(l_pcColorSet->GetDataType() == CPN_UNION && l_pcColorSet->IsColor(wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue)))
		{
			return true;	
		}
		else if( l_pcColorSet->GetDataType() == CPN_INTEGER && l_pcColorSet->IsColor(wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue)) )
		{
			return true;
		}
		else
		{
			wxString l_sError;					
			l_sError = wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue) + wxT(" is not a color of ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}
	return true;
}


bool SP_CPN_Parse_String_Node::check()
{
	if(m_ParseNode_Info.m_CheckColorSet != wxT(""))
	{
		SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
		if( ! l_pcColorSet )
			return false;

		if(l_pcColorSet->GetDataType() == CPN_UNION && l_pcColorSet->IsColor( *(m_ParseNode_Info.m_StringValue)))
		{
			return true;	
		}
		else if( l_pcColorSet->GetDataType() == CPN_INTEGER && l_pcColorSet->IsColor(*(m_ParseNode_Info.m_StringValue)) )
		{
			return true;
		}
		else
		{
			wxString l_sError;					
			l_sError = *(m_ParseNode_Info.m_StringValue) + wxT(" is not a color of ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}

	return true;
}




bool SP_CPN_Parse_Variable_Node::ProcessingVariable()  // add flag to indicate checking or evaluate
{
	////////////////////////////////////
	// deal with functions
	////////////////////////////////////

	if( m_bFunctionFlag )
	{
		if(m_pColorSetClass->GetFunctionMap() && (m_pColorSetClass->GetFunctionMap()->find(m_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end()))
		{
			SP_CPN_Function l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(m_sFunctionName)->second;
			if(l_FunctionStruct.m_ParameterMap.find(m_VariableName) != l_FunctionStruct.m_ParameterMap.end())
			{
			
				SP_CPN_Parameter l_ParameterStruct = l_FunctionStruct.m_ParameterMap.find(m_VariableName)->second;
				m_ParseNode_Info.m_ColorSet = l_ParameterStruct.m_sParaColorSet;
				m_ParseNode_Info.m_DataType = l_ParameterStruct.m_DataType;
				m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;
				//SP_CPN_DATATYPE l_DataType = m_ParseNode_Info.m_DataType;
				return true;
			}
			else if( m_pColorSetClass->GetConstantMap() &&m_pColorSetClass->GetConstantMap()->find(m_VariableName) != m_pColorSetClass->GetConstantMap()->end())
			{
				SP_CPN_DATATYPE l_DataType = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_DataType;
				m_ParseNode_Info.m_DataType = l_DataType;
				m_ParseNode_Info.m_ColorSet = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_ColorSet;
				
				m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;

				if(l_DataType == CPN_INTEGER)
					m_ParseNode_Info.m_IntegerValue = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_IntegerValue;
				
				if(l_DataType == CPN_BOOLEAN)
					m_ParseNode_Info.m_BooleanValue = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_BooeanValue;

				if(l_DataType == CPN_STRING||
					l_DataType == CPN_INDEX||
					l_DataType == CPN_PRODUCT||
					l_DataType == CPN_ENUM)     
				{
					m_String = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_StringValue;
					m_ParseNode_Info.m_StringValue = &m_String ;
				}
				return true;
			}
			else
			{
				//Check enum color set if the variable name is a enum value			
				for(unsigned int k = 0; k < m_pColorSetClass->GetColorSetVector()->size(); k++)
				{
					SP_CPN_ColorSet* l_pcColorSet = (*m_pColorSetClass->GetColorSetVector())[k];
					if(l_pcColorSet->GetDataType()== CPN_ENUM)
					{
						for(unsigned i = 0; i < l_pcColorSet->GetStringValue().size(); i++)
						{
							if(l_pcColorSet->IsColor(m_VariableName))
							{						
								m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;
								m_ParseNode_Info.m_ColorSet = l_pcColorSet->GetName();
								m_ParseNode_Info.m_DataType = CPN_ENUM;
								m_ParseNode_Info.m_StringValue = &m_VariableName;	

								m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
								if( m_ParseNode_Info.m_sColorSetList == wxT("") )
									m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;		
								return true;
							}
							else if(l_pcColorSet->GetName()== m_VariableName)
							{//byy beorge
								m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;//CPN_ELEMENT_OF_NODE
								m_ParseNode_Info.m_ColorSet = l_pcColorSet->GetName();
								m_ParseNode_Info.m_DataType = CPN_ENUM;
								m_ParseNode_Info.m_StringValue = &m_VariableName;
								m_ParseNode_Info.m_CheckedString = m_VariableName;
								m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
								if (m_ParseNode_Info.m_sColorSetList == wxT(""))
									m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
								return true;
							}
						}
					}
					else
					{//by george for elemOf operator
						if (l_pcColorSet->GetName() == m_VariableName)
						{
							if ((l_pcColorSet)->GetName() == m_VariableName && (l_pcColorSet)->GetDataType() == CPN_PRODUCT)
							{
								m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;
								m_ParseNode_Info.m_ColorSet = (l_pcColorSet)->GetName();
								m_ParseNode_Info.m_DataType = (l_pcColorSet)->GetDataType();
								m_ParseNode_Info.m_StringValue = &m_VariableName;
								auto vComp = (l_pcColorSet)->GetComponentName();
								wxString l_sList;
								if (vComp.size()>0)
								{
									for (auto vit = vComp.begin(); vit != vComp.end(); ++vit)
									{
										l_sList << (*vit) << wxT(",");
									}
									l_sList = l_sList.BeforeLast(wxChar(','));
								}
								m_ParseNode_Info.m_sColorSetList = l_sList;// m_ParseNode_Info.m_ColorSet;
								if (m_ParseNode_Info.m_sColorSetList == wxT(""))
									m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
								return true;
							}
							else
							{
								m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;
								m_ParseNode_Info.m_ColorSet = (l_pcColorSet)->GetName();
								m_ParseNode_Info.m_DataType = (l_pcColorSet)->GetDataType();
								m_ParseNode_Info.m_StringValue = &m_VariableName;

								m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
								if (m_ParseNode_Info.m_sColorSetList == wxT(""))
									m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
								return true;
							}
					    }
					}
				}
				
				///////////////////1.9.2021
				//Look up the colored place names
				if (this->m_pcGraph)//for numOf
				{//discrete places
					SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
					SP_ListNode::const_iterator l_itElem;
					if (l_pcNodeclass)
					{
						for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
						{
							SP_DS_Node* l_pcPlaceNode = (*l_itElem);

							if (l_pcNodeclass)
							{
								wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
								if (l_sPlaceName == m_VariableName)
								{
									m_ParseNode_Info.m_NodeType = CPN_PLACE_NODE;
									m_ParseNode_Info.m_sColoredPlaceName = m_VariableName;
									m_ParseNode_Info.m_bPlaceFlag = true;
									m_ParseNode_Info.m_DataType = CPN_INTEGER;
									return true;
								}
							}
						}
					}
				}

				if (this->m_pcGraph)//for numOf
				{//continuous places
					SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
					SP_ListNode::const_iterator l_itElem;
					if (l_pcNodeclass)
					{
						for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
						{
							SP_DS_Node* l_pcPlaceNode = (*l_itElem);

							if (l_pcNodeclass)
							{
								wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
								if (l_sPlaceName == m_VariableName)
								{
									m_ParseNode_Info.m_NodeType = CPN_PLACE_NODE;
									m_ParseNode_Info.m_sColoredPlaceName = m_VariableName;
									m_ParseNode_Info.m_bPlaceFlag = true;
									m_ParseNode_Info.m_DataType = CPN_DOUBLE;
									return true;
								}
							}
						}
					}

				}


				///////////////////

				wxString l_sError;				
				l_sError << wxT("Variable: ") << m_VariableName << wxT(" is not defined. Position: ") << m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
		}
		else
		{
			wxString l_sError;
			l_sError << wxT("Function: ") << m_sFunctionName << wxT(" does not exist.");
			SP_LOGERROR(l_sError);				
			return false;
		}

	}


	else  //not function, ordinary expressions
	{
		// Lookup in the variable table
		if(m_pColorSetClass->GetVariableMap() && m_pColorSetClass->GetVariableMap()->find(m_VariableName) != m_pColorSetClass->GetVariableMap()->end())		
		{
			SP_CPN_DATATYPE l_DataType  = m_pColorSetClass->GetVariableMap()->find(m_VariableName)->second.m_DataType;
			m_ParseNode_Info.m_ColorSet = m_pColorSetClass->GetVariableMap()->find(m_VariableName)->second.m_ColorSet;
			m_ParseNode_Info.m_DataType = l_DataType;		
			m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE; // Labeled as a variable node				

			if(l_DataType == CPN_UNION)
			{
				SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_ColorSet);
				//if the data type of all union components are the same, then set it this type; otherwise set it CPN_NULL
				m_ParseNode_Info.m_DataType = l_pcColorSet->GetUnionDataType();  
				//m_ParseNode_Info.m_NodeType = CPN_UNIONVARIABLE_NODE;				
			}

			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
			if( m_ParseNode_Info.m_sColorSetList == wxT("") )
				m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;	

			return true;
		} 

		//Lookup in the function table
		if(m_pColorSetClass->GetFunctionMap()&&
			m_pColorSetClass->GetFunctionMap()->find(m_VariableName) != m_pColorSetClass->GetFunctionMap()->end())
		{
			m_ParseNode_Info.m_DataType = CPN_STRING; // another special case
			m_ParseNode_Info.m_StringValue = &m_VariableName;
			m_ParseNode_Info.m_NodeType = CPN_FUNCTIONNAME_NODE;
			return true;
		}

		// Lookup in the constant table
		if( m_pColorSetClass->GetConstantMap() &&
				 m_pColorSetClass->GetConstantMap()->find(m_VariableName) != m_pColorSetClass->GetConstantMap()->end())
		{
			SP_CPN_DATATYPE l_DataType = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_DataType;
			m_ParseNode_Info.m_DataType = l_DataType;
			m_ParseNode_Info.m_ColorSet = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_ColorSet;			
			m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;

			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
			if( m_ParseNode_Info.m_sColorSetList == wxT("") )
				m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;	

			if (l_DataType == CPN_INTEGER)
			{
				m_ParseNode_Info.m_IntegerValue = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_IntegerValue;

				if (m_ParseNode_Info.m_bSeparaterExpression)
				{
					if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
						m_ParseNode_Info.m_DoubleMultiplicity = m_ParseNode_Info.m_IntegerValue;
					else
						m_ParseNode_Info.m_Multiplicity = m_ParseNode_Info.m_IntegerValue;
				}
			}
				
		 
			
			if(l_DataType == CPN_BOOLEAN)
				m_ParseNode_Info.m_BooleanValue = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_BooeanValue;

			if(l_DataType == CPN_STRING||
				l_DataType == CPN_INDEX||
				l_DataType == CPN_PRODUCT||
				l_DataType == CPN_ENUM)      
			{
				m_String = m_pColorSetClass->GetConstantMap()->find(m_VariableName)->second.m_StringValue;
				m_ParseNode_Info.m_StringValue = &m_String ;
			}
			if(l_DataType == CPN_UNION)
			{	
				SP_LOGWARNING(wxT("Not support the definition of constants yet!"));
			}
			return true;
		}
		
		//Look up the colored place names
		if (m_psvColored2UnColoredPlaceNames)
		{
			if (m_psvColored2UnColoredPlaceNames->find(m_VariableName) != m_psvColored2UnColoredPlaceNames->end())
			{
				//deal with this place
				m_ParseNode_Info.m_NodeType = CPN_PLACE_NODE;
				m_ParseNode_Info.m_sColoredPlaceName = m_VariableName;
				m_ParseNode_Info.m_bPlaceFlag = true;
				return true;
			}
		}

		// Lookup the black token "dot"
		if(m_VariableName == wxT("dot"))
		{
			m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;
			m_ParseNode_Info.m_DataType = CPN_DOT;
			m_ParseNode_Info.m_ColorSet = wxT("Dot");
			m_String = m_VariableName;
			m_ParseNode_Info.m_StringValue = &m_String ;

			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
			return true;
		}
		
		// Lookup the boolean values: true or false
		if(m_VariableName == wxT("false")||m_VariableName == wxT("true"))  //deal with bool constant
		{
			m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;
			m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

			m_ParseNode_Info.m_sColorSetList = wxT("");		
			m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;	

			if(m_VariableName == wxT("false") )
				m_ParseNode_Info.m_BooleanValue = false;
			else
				m_ParseNode_Info.m_BooleanValue = true;

			return true;
		}	
		
		//Check if it is an index name						
		vector<SP_CPN_ColorSet*>::iterator it;
		if( !m_pColorSetClass->GetColorSetVector())
			return false;
		for(it = m_pColorSetClass->GetColorSetVector()->begin(); it != m_pColorSetClass->GetColorSetVector()->end(); it++)
		{
			if((*it)->GetIndexName() == m_VariableName)
			{				
				m_ParseNode_Info.m_NodeType = CPN_INDEXNAME_NODE;
				m_ParseNode_Info.m_DataType = CPN_STRING; // a special case
				m_ParseNode_Info.m_StringValue = &m_VariableName;
				return true;
			}
		}				
		
		//Check enum color set if the variable name is a enum value				
		for(it = m_pColorSetClass->GetColorSetVector()->begin(); it != m_pColorSetClass->GetColorSetVector()->end(); it++)
			{
				if((*it)->GetDataType()== CPN_ENUM)
				{
					for(unsigned i = 0; i < (*it)->GetStringValue().size(); i++)
					{
						if((*it)->IsColor(m_VariableName))
						{						
							m_ParseNode_Info.m_NodeType = CPN_CONSTANT_NODE;
							m_ParseNode_Info.m_ColorSet = (*it)->GetName();
							m_ParseNode_Info.m_DataType = CPN_ENUM;
							m_ParseNode_Info.m_StringValue = &m_VariableName;	

							m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
							if( m_ParseNode_Info.m_sColorSetList == wxT("") )
								m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;		
							return true;
						}
					}
				}
			}

		////////////////////////////
		//check if the Identifier is a colorset name, required for elemOf operator
		for (it = m_pColorSetClass->GetColorSetVector()->begin(); it != m_pColorSetClass->GetColorSetVector()->end(); it++)
		{
			if ((*it)->GetName() == m_VariableName && (*it)->GetDataType()!=CPN_PRODUCT)
			{
				m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;
				m_ParseNode_Info.m_ColorSet = (*it)->GetName();
				m_ParseNode_Info.m_DataType = (*it)->GetDataType();
				m_ParseNode_Info.m_StringValue = &m_VariableName;

				m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
				if (m_ParseNode_Info.m_sColorSetList == wxT(""))
                   m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
             return true;
			}

			if ((*it)->GetName() == m_VariableName && (*it)->GetDataType() == CPN_PRODUCT)
			{
				m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE;
				m_ParseNode_Info.m_ColorSet = (*it)->GetName();
				m_ParseNode_Info.m_DataType = (*it)->GetDataType();
				m_ParseNode_Info.m_StringValue = &m_VariableName;
				auto vComp=(*it)->GetComponentName();  
				wxString l_sList;
				if (vComp.size()>0)
				{
					for (auto vit = vComp.begin(); vit != vComp.end(); ++vit)
					{
						l_sList << (*vit) << wxT(",");
					}
					l_sList=l_sList.BeforeLast(wxChar(','));
				}
				m_ParseNode_Info.m_sColorSetList = l_sList;// m_ParseNode_Info.m_ColorSet;
				if (m_ParseNode_Info.m_sColorSetList == wxT(""))
					m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
				return true;
			}
		}
		//===========================

		//Look up the colored place names
		if (this->m_pcGraph)//for numOf
		{//discrete places
			SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

			if (l_pcNodeclass)
			{
				SP_ListNode::const_iterator l_itElem;
				for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
				{
					SP_DS_Node* l_pcPlaceNode = (*l_itElem);

					if (l_pcNodeclass)
					{
						wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
						if (l_sPlaceName == m_VariableName)
						{
							m_ParseNode_Info.m_NodeType = CPN_PLACE_NODE;
							m_ParseNode_Info.m_sColoredPlaceName = m_VariableName;
							m_ParseNode_Info.m_bPlaceFlag = true;
							m_ParseNode_Info.m_DataType = CPN_INTEGER;
							return true;
						}
					}
				}


			}
		}

		if (this->m_pcGraph)//for numOf
		{//continuous places

			SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
			SP_ListNode::const_iterator l_itElem;
			if (l_pcNodeclass)
			{
				for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
				{
					SP_DS_Node* l_pcPlaceNode = (*l_itElem);

					if (l_pcNodeclass)
					{
						wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
						if (l_sPlaceName == m_VariableName)
						{
							m_ParseNode_Info.m_DataType = CPN_DOUBLE;
							m_ParseNode_Info.m_NodeType = CPN_PLACE_NODE;
							m_ParseNode_Info.m_sColoredPlaceName = m_VariableName;
							m_ParseNode_Info.m_bPlaceFlag = true;

							return true;
						}
					}
				}

			}
		}
		//============================



		//////////////////////////
		//for other cases, we report errors			
		wxString l_sError;
		l_sError << wxT("Variable, constant or color: ") << m_VariableName << wxT(" is not defined. Position: ") << m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	return false;
}

////only for evaluation
void SP_CPN_Parse_Variable_Node::SetValue()
{
	/******************/
	//for user-defined functions
	if (!(m_ParseNode_Info.m_NodeType == CPN_VARIABLE_NODE || m_ParseNode_Info.m_NodeType == CPN_PLACE_NODE))
		return;

	if (m_ParseNode_Info.m_NodeType == CPN_PLACE_NODE)
	{
		vector<wxString> *l_pvsUncoloredPlaceName = &(m_psvColored2UnColoredPlaceNames->find(m_VariableName)->second);
		wxString l_sValue;
		l_sValue = wxT("");
		for (unsigned i = 0; i < l_pvsUncoloredPlaceName->size(); i++)
		{
			l_sValue = l_sValue + wxT("+") + (*l_pvsUncoloredPlaceName)[i];
		}
		l_sValue = l_sValue.AfterFirst('+');
		m_ParseNode_Info.m_stringMultiplicity = l_sValue;
		return;
	}

	//check if the var name is a color set name, required for elemOf operator
	//by george
	bool l_bIsColorSet = false;
	for (auto itV = m_pColorSetClass->GetColorSetVector()->begin(); itV != m_pColorSetClass->GetColorSetVector()->end(); ++itV)
	{
		if (m_VariableName == (*itV)->GetName())
		{
			l_bIsColorSet = true;
			break;
		}
	}

	if (m_bFunctionFlag)
	{
		if (l_bIsColorSet) return;//by george

		SP_CPN_Function l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(m_sFunctionName)->second;
		SP_CPN_Parameter l_ParameterStruct = l_FunctionStruct.m_ParameterMap.find(m_VariableName)->second;
		if (m_ParseNode_Info.m_DataType == CPN_INTEGER)
			m_ParseNode_Info.m_IntegerValue = l_ParameterStruct.m_IntegerValue;

		if (m_ParseNode_Info.m_DataType == CPN_BOOLEAN)
			m_ParseNode_Info.m_BooleanValue = l_ParameterStruct.m_BooeanValue;

		if (m_ParseNode_Info.m_DataType == CPN_STRING ||
			m_ParseNode_Info.m_DataType == CPN_INDEX ||
			m_ParseNode_Info.m_DataType == CPN_DOT ||
			m_ParseNode_Info.m_DataType == CPN_PRODUCT ||
			m_ParseNode_Info.m_DataType == CPN_ENUM)			// the enum should be carefully considered.
		{
			m_String = l_ParameterStruct.m_StringValue;
			m_ParseNode_Info.m_StringValue = &m_String;
		}
		//for the union type, its data type should be dynamically determined 			
	}
	//for ordinary expressions
	else
	{
		if(l_bIsColorSet)
		  {
			///for elemOf with gecode unfolder
			if (m_String.IsEmpty())//for elemOf, by george
			{
				for (auto l_itFind = m_pColorSetClass->GetVariableMap()->begin(); l_itFind != m_pColorSetClass->GetVariableMap()->end(); ++l_itFind)
				{
					if (l_itFind->first.Contains(wxT("__AUX_CS_")) && l_itFind->first.Contains(m_VariableName))
					{
						m_String = l_itFind->second.m_StringValue;
						m_ParseNode_Info.m_StringValue = &m_String;
					}
				}

				//by george for elemOF
				if (m_String.IsEmpty())
				{
					SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_VariableName);
					wxString l_sVal;
					int l_nelemCount = 0;
					l_sVal << wxT("(");
					std::vector<wxString> l_sCompCSNames;

					if (l_pcColorSet)
					{
						l_sCompCSNames = l_pcColorSet->GetComponentName();
					}
					for (auto v : l_sCompCSNames)
					{
						for (auto l_itFind = m_pColorSetClass->GetVariableMap()->begin(); l_itFind != m_pColorSetClass->GetVariableMap()->end(); ++l_itFind)
						{
							wxString l_sRankAux; l_sRankAux << l_nelemCount;
							if (l_itFind->first.Contains(wxT("__AUX_CS_")) && l_itFind->first.Contains(v) && l_itFind->first.Contains(l_sRankAux))
							{
								//m_String = l_itFind->second.m_StringValue;
								m_String = l_itFind->second.m_ColorSet;
								//m_ParseNode_Info.m_StringValue = &m_String;
								l_sVal << m_String << wxT(",");
								l_nelemCount++;
							}
						}
					}

					if (l_nelemCount == l_sCompCSNames.size())
					{
						l_sVal = l_sVal.BeforeLast(wxChar(','));
						l_sVal << wxT(")");
						m_String = l_sVal;
						m_ParseNode_Info.m_StringValue = &m_String;
					}

				}

			}
			/////for gecode///
			return;
		}

		if (m_ParseNode_Info.m_DataType == CPN_INTEGER)
			m_ParseNode_Info.m_IntegerValue = m_pColorSetClass->GetVariableMap()->find(m_VariableName)->second.m_IntegerValue;

		if (m_ParseNode_Info.m_DataType == CPN_BOOLEAN)
			m_ParseNode_Info.m_BooleanValue = m_pColorSetClass->GetVariableMap()->find(m_VariableName)->second.m_BooeanValue;

		if (m_ParseNode_Info.m_DataType == CPN_STRING ||
			m_ParseNode_Info.m_DataType == CPN_INDEX ||
			m_ParseNode_Info.m_DataType == CPN_DOT ||
			m_ParseNode_Info.m_DataType == CPN_PRODUCT ||
			m_ParseNode_Info.m_DataType == CPN_ENUM)			// the enum should be carefully considered.
		{
			m_String = m_pColorSetClass->GetVariableMap()->find(m_VariableName)->second.m_StringValue;
			m_ParseNode_Info.m_StringValue = &m_String;

		}
	}

	/**
	if (m_String.IsEmpty())//for elemOf, by george
	{
		for (auto l_itFind = m_pColorSetClass->GetVariableMap()->begin(); l_itFind != m_pColorSetClass->GetVariableMap()->end(); ++l_itFind)
		{
			if (l_itFind->first.Contains(wxT("__AUX_CS_")) && l_itFind->first.Contains(m_VariableName))
			{
				m_String = l_itFind->second.m_StringValue;
				m_ParseNode_Info.m_StringValue = &m_String;
            }
		}

		//by george for elemOF
		if (m_String.IsEmpty())
		{
			SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_VariableName);
			wxString l_sVal;
			int l_nelemCount = 0;
			l_sVal << wxT("(");
			std::vector<wxString> l_sCompCSNames;

			if (l_pcColorSet)
			{
				 l_sCompCSNames = l_pcColorSet->GetComponentName();
			}
			for (auto v : l_sCompCSNames)
			{
				for (auto l_itFind = m_pColorSetClass->GetVariableMap()->begin(); l_itFind != m_pColorSetClass->GetVariableMap()->end(); ++l_itFind)
				{
					wxString l_sRankAux; l_sRankAux << l_nelemCount;
					if (l_itFind->first.Contains(wxT("__AUX_CS_")) && l_itFind->first.Contains(v) && l_itFind->first.Contains(l_sRankAux))
					{
						m_String = l_itFind->second.m_StringValue;
						//m_ParseNode_Info.m_StringValue = &m_String;
						l_sVal << m_String << wxT(",");
						l_nelemCount++;
					}
				}
			}

			if (l_nelemCount == l_sCompCSNames.size())
			{
				l_sVal = l_sVal.BeforeLast(wxChar(','));
				l_sVal << wxT(")");
				m_String = l_sVal;
				m_ParseNode_Info.m_StringValue = &m_String;
			}
			 
		}
		
	}
	*/
}


bool SP_CPN_Parse_Variable_Node::GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
{	
	if( ! m_bFunctionFlag )
	{
		if( m_ParseNode_Info.m_NodeType ==  CPN_VARIABLE_NODE && m_ParseNode_Info.m_DataType == CPN_INTEGER )
		{
			m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, p_cCSPSolver.GetGecodeIntVar(m_VariableName) );		
			return true;
		}
		else if( m_ParseNode_Info.m_NodeType ==  CPN_CONSTANT_NODE && m_ParseNode_Info.m_DataType == CPN_INTEGER )
		{
			m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, m_ParseNode_Info.m_IntegerValue );		
			return true;
		}

		else if (m_ParseNode_Info.m_NodeType == CPN_VARIABLE_NODE && m_ParseNode_Info.m_DataType == CPN_PRODUCT)
		{//by george
			bool l_bIsfound = false;
			std::vector<IntVar> vvc;
			for (auto itV = m_pColorSetClass->GetColorSetVector()->begin(); itV != m_pColorSetClass->GetColorSetVector()->end(); ++itV)
			{
				if (m_VariableName == (*itV)->GetName())
				{
					for (auto itVar = m_pColorSetClass->GetVariableMap()->begin(); itVar != m_pColorSetClass->GetVariableMap()->end(); ++itVar)
					{
						if (itVar->first.Contains(wxT("__AUX_CS")))
						{
							l_bIsfound = true;
							wxString l_sVarName = itVar->first;
							 
							vvc.push_back(expr(p_cCSPSolver, p_cCSPSolver.GetGecodeIntVar(l_sVarName)));
						}
						
					}
					
				}
			}
			if (vvc.size() > 0)
			{
				//m_ParseNode_Info.m_vIntconstraintVector = new std::vector<IntVar>(2);
				for (auto elem : vvc)
				{
					m_ParseNode_Info.m_vIntconstraintVector->push_back(elem);
			 

				}
			}
			return l_bIsfound;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(m_pColorSetClass->GetFunctionMap() && (m_pColorSetClass->GetFunctionMap()->find(m_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end()))
		{
			SP_CPN_Function l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(m_sFunctionName)->second;
			if(l_FunctionStruct.m_ParameterMap.find(m_VariableName) != l_FunctionStruct.m_ParameterMap.end())
			{
				SP_CPN_Parameter l_ParameterStruct = l_FunctionStruct.m_ParameterMap.find(m_VariableName)->second;				
				SP_CPN_DATATYPE l_DataType = l_ParameterStruct.m_DataType;
				if(l_DataType == CPN_INTEGER)
				{
					wxString l_sRealVar = l_ParameterStruct.m_sRealVar;
					long l_nRealVar;
					if(l_sRealVar.ToLong(&l_nRealVar))
					{
						m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_nRealVar );
						return true;
					}
					else
					{
						IntVar  l_nIntVar;
						if( !p_cCSPSolver.GetGecodeIntVar(l_sRealVar,l_nIntVar) )
							return false;
						m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_nIntVar );
						return true;
					}
				}
				else
				{
					return false;
				}				
			}
			else if( m_ParseNode_Info.m_NodeType ==  CPN_CONSTANT_NODE && m_ParseNode_Info.m_DataType == CPN_INTEGER )
			{
				m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, m_ParseNode_Info.m_IntegerValue );		
				return true;
			}
			else
			{
				return false;
			}
		}

	}
	return false;
}





void SP_CPN_Parse_Neighbor12D_Node::GetValue()
{
	m_ParseNode_Info.m_IntegerValue = m_pColorSetClass->GetNeighbor12DFunc()->find(m_sFuncName)->second;
}

bool SP_CPN_Parse_Neighbor12D_Node::check()
{
	if(m_pColorSetClass->GetNeighbor12DFunc()->find(m_sFuncName) != m_pColorSetClass->GetNeighbor12DFunc()->end())		
	{
	}
	else
	{
		wxString l_sError;					
		l_sError = m_sFuncName + wxT(" Unknow function. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
			
	return true;
}


bool SP_CPN_Parse_AllFunc_Node::GetValue()
{
	m_ParseNode_Info.m_EvalResults.clear();
	 

	SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ColorSetName);
	if(!l_pcColorSet)
		return false;

	m_ParseNode_Info.m_DataType = l_pcColorSet->GetDataType();
	m_ParseNode_Info.m_ColorSet = m_ColorSetName;

	vector<wxString> l_vColors = l_pcColorSet->GetStringValue();
	for(unsigned i = 0; i< l_vColors.size(); i++)
	{
		SP_CPN_EvaluatedSingleValue l_stEvalRes;
		l_stEvalRes.m_ColorValue = l_vColors[i];

		if( m_sPlaceType == SP_DS_CONTINUOUS_PLACE )
			l_stEvalRes.m_DoubleMultiplicity = m_ParseNode_Info.m_DoubleMultiplicity;
		else
			l_stEvalRes.m_Multiplicity = m_ParseNode_Info.m_Multiplicity;
		
		l_stEvalRes.m_Predicate = m_ParseNode_Info.m_Predicate;
		l_stEvalRes.m_bPlaceFlag = m_ParseNode_Info.m_bPlaceFlag;
		m_ParseNode_Info.m_EvalResults.push_back(l_stEvalRes);		
	}
   return true;
}

 

bool SP_CPN_Parse_AllFunc_Node::check()
{

	SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ColorSetName);
	if(!l_pcColorSet)
		return false;

	return true;
}


bool SP_CPN_Parse_AbsFunc_Node::check()
{
	if(!m_pRight->check())
		return false;
	
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_RightNodeInfo->m_DataType == CPN_INTEGER) )
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the absolute value function error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	m_ParseNode_Info.m_DataType = l_RightNodeInfo->m_DataType;

	
	if(l_RightNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

	if(m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}

	return true;
}


SP_CPN_ParseNode_Info SP_CPN_Parse_AbsFunc_Node::evaluate() 
{		
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_RightNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo.m_ColorSet;

		if(l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_IntegerValue = std::abs(l_RightNodeInfo.m_IntegerValue);
		}

		CollectResult();
		return m_ParseNode_Info;
 }


bool SP_CPN_Parse_ValFunc_Node::check()
{
	if(!m_pRight->check())
		return false;
	
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_RightNodeInfo->m_DataType == CPN_INTEGER) )
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the val() function error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	m_ParseNode_Info.m_DataType = l_RightNodeInfo->m_DataType;

	
	if(l_RightNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

	if(m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}

	return true;
}


SP_CPN_ParseNode_Info SP_CPN_Parse_ValFunc_Node::evaluate() 
{		
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_RightNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo.m_ColorSet;

		if(l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_IntegerValue = l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();
		return m_ParseNode_Info;
 }



bool SP_CPN_Parse_Variable_Node::check()
{
	if( ! ProcessingVariable() ) 
	{
		return false;
	}

	if(m_ParseNode_Info.m_CheckColorSet != wxT(""))
	{
		SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
		if(!l_pcColorSet)
			return false;

		if( l_pcColorSet->GetDataType() == CPN_UNION && l_pcColorSet->IsUnionComponent(m_ParseNode_Info.m_ColorSet))
		{
			//it is the union variable of itselt or of one of its components
			return true;			
		}
		else if(l_pcColorSet->GetDataType() != CPN_UNION && m_ParseNode_Info.m_DataType == l_pcColorSet->GetDataType())
		{
			return true;
		}
		else
		{
			wxString l_sError;					
			l_sError = m_VariableName + wxT(" mismatches the color set ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}		
	}	

	return true;
}


bool SP_CPN_Parse_Predecessor_Node::GetPredecessor()
{
	SP_CPN_ParseNode_Info l_NodeInfo = m_pLeft->evaluate();
	SP_CPN_ColorSet* l_ColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_ColorSet);	
	if(!l_ColorSet)
		return false;	
	
	if(l_NodeInfo.m_DataType == CPN_INTEGER)
	{
		long l_nColor;
		wxString l_sColor = l_ColorSet->Predecessor(wxString::Format(wxT("%d"),l_NodeInfo.m_IntegerValue));
		if( ! l_sColor.ToLong(&l_nColor) )
			return false;
		m_ParseNode_Info.m_IntegerValue = (int)l_nColor;		
	}

	if(l_NodeInfo.m_DataType == CPN_BOOLEAN)
	{
		if(l_NodeInfo.m_BooleanValue == false)
			m_ParseNode_Info.m_BooleanValue = true;
		else
			m_ParseNode_Info.m_BooleanValue = false;	
	}

	if(l_NodeInfo.m_DataType == CPN_STRING ||
		l_NodeInfo.m_DataType == CPN_INDEX ||
		l_NodeInfo.m_DataType == CPN_ENUM ||   //Special
		l_NodeInfo.m_DataType == CPN_PRODUCT) 
	{
		m_String = *(l_NodeInfo.m_StringValue);
		m_String = l_ColorSet->Predecessor(m_String);
		m_ParseNode_Info.m_StringValue = &m_String;			
	}
	
	return true;
}


bool SP_CPN_Parse_Predecessor_Node::check()
{
	if(!m_pLeft->check())
		return false;

	SP_CPN_ParseNode_Info* l_NodeInfo = m_pLeft->GetParseNodeInfo();	
	m_ParseNode_Info.m_ColorSet = l_NodeInfo->m_ColorSet;
	m_ParseNode_Info.m_DataType = l_NodeInfo->m_DataType;
	
	if(m_ParseNode_Info.m_CheckColorSet != wxT("") && m_ParseNode_Info.m_ColorSet != wxT("") )
	{		
		if(m_ParseNode_Info.m_ColorSet != m_ParseNode_Info.m_CheckColorSet)
		{
			wxString l_sError;					
			l_sError = wxT("Predecessor mismatches the color set ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}	
	}
	if(m_ParseNode_Info.m_CheckColorSet != wxT("") && m_ParseNode_Info.m_ColorSet == wxT("") )
	{
		m_ParseNode_Info.m_ColorSet = m_ParseNode_Info.m_CheckColorSet;
	}
	
	if(m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}
	return true;
}


bool SP_CPN_Parse_Successor_Node::GetSuccessor()
{
	SP_CPN_ParseNode_Info  l_NodeInfo = m_pLeft->evaluate();
	SP_CPN_ColorSet* l_ColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_ColorSet);

	if(!l_ColorSet)
		return false;
	
	if(l_NodeInfo.m_DataType == CPN_INTEGER)
		{
			long l_nColor;
			wxString l_sColor = l_ColorSet->Successor(wxString::Format(wxT("%d"),l_NodeInfo.m_IntegerValue));
			l_sColor.ToLong(&l_nColor);
			m_ParseNode_Info.m_IntegerValue = (int)l_nColor;		
		}
		
	if(l_NodeInfo.m_DataType == CPN_BOOLEAN)
		{
			if(l_NodeInfo.m_BooleanValue == false)
				m_ParseNode_Info.m_BooleanValue = true;
			else
				m_ParseNode_Info.m_BooleanValue = false;	
		}

	if(l_NodeInfo.m_DataType == CPN_STRING ||
			l_NodeInfo.m_DataType == CPN_INDEX ||
			l_NodeInfo.m_DataType == CPN_ENUM ||
			l_NodeInfo.m_DataType == CPN_PRODUCT) 
		{
			m_String = *(l_NodeInfo.m_StringValue);
			m_String = l_ColorSet->Successor(m_String);
			m_ParseNode_Info.m_StringValue = &m_String;			
		}
	
	return true;
}


bool SP_CPN_Parse_Successor_Node::check()
{
	if(!m_pLeft->check())
		return false;

	SP_CPN_ParseNode_Info* l_NodeInfo = m_pLeft->GetParseNodeInfo();	
	m_ParseNode_Info.m_ColorSet = l_NodeInfo->m_ColorSet;
	m_ParseNode_Info.m_DataType = l_NodeInfo->m_DataType;
	m_ParseNode_Info.m_CheckedString = wxT("+") + l_NodeInfo->m_CheckedString;
	
	if(m_ParseNode_Info.m_CheckColorSet != wxT("") && m_ParseNode_Info.m_ColorSet != wxT("") )
	{		
		if(m_ParseNode_Info.m_ColorSet != m_ParseNode_Info.m_CheckColorSet)
		{
			wxString l_sError;					
			l_sError = wxT("Successor mismatches the color set ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}	
	}
	if(m_ParseNode_Info.m_CheckColorSet != wxT("") && m_ParseNode_Info.m_ColorSet == wxT("") )
	{
		m_ParseNode_Info.m_ColorSet = m_ParseNode_Info.m_CheckColorSet;
	}

	if(m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}

	return true;
}


SP_CPN_ParseNode_Info SP_CPN_Parse_Function_Node::evaluate()
{
	SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
	SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

	wxString l_sFunctionName   = *(l_LeftNodeInfo.m_StringValue);
	SP_CPN_DATATYPE l_DataType = l_RightNodeInfo.m_DataType;
	wxString l_sArgumentList;
	if(l_DataType == CPN_INTEGER)
	{
		l_sArgumentList = wxString::Format(wxT("%d"),l_RightNodeInfo.m_IntegerValue);
	}
	else
	{
		l_sArgumentList   = *(l_RightNodeInfo.m_StringValue);
	}
	

	m_pColorSetClass->SetParameterValues(l_sFunctionName,l_sArgumentList);

	if(m_pColorSetClass->GetFunctionMap()&&
		m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end())
	{
		SP_CPN_Function l_FunctionStruct;
		l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName)->second;
		//l_FunctionStruct.m_pParseContext->SetFunctionFlag(true);
		l_FunctionStruct.m_pParseContext->SetFunctionName(l_FunctionStruct.m_sFunctionName);

		//SP_CPN_ParseNode* l_pcParseNode = *(l_FunctionStruct.m_pParseContext->GetExpressionsVector()->begin());
		//l_FunctionStruct.m_pParseContext->ConfigurationColorSets(l_pcParseNode,m_pColorSetClass);		

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;
		l_FunctionStruct.m_pParseContext->EvaluateExpression( l_EvaluateResultArray);
			

		vector<SP_CPN_EvaluatedSingleValue>	m_ParsedInfo = l_EvaluateResultArray ; //l_FunctionStruct.m_pParseContext->EvaluateExpressionOnly();

		//l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);   //Reset it to false as it is a global variable

		l_DataType = m_ParseNode_Info.m_DataType;		

		// Return single value		
		if(m_ParsedInfo.size() == 1)
		{
			wxString l_sColorValue = m_ParsedInfo[0].m_ColorValue;

			if(l_DataType == CPN_INTEGER)
			{
				long l_nValue;
				if(l_sColorValue.ToLong(&l_nValue))
					m_ParseNode_Info.m_IntegerValue = l_nValue;
			}
		
			else if(l_DataType == CPN_BOOLEAN)
			{			
				if(l_sColorValue == wxT("false"))
					m_ParseNode_Info.m_BooleanValue = false;
				else
					m_ParseNode_Info.m_BooleanValue = true;
			}
			else
			{
				m_String = l_sColorValue;
				m_ParseNode_Info.m_StringValue = &m_String;
			}

			m_ParseNode_Info.m_Multiplicity = m_ParsedInfo[0].m_Multiplicity;
		}
		else if(m_ParsedInfo.size() == 0)
		{		
			m_ParseNode_Info.m_EvalResults.clear();
			return m_ParseNode_Info;		
		}
		else
		{
			SP_MESSAGEBOX(wxT("The user-defined function only allows to return a single value."), wxT("Expression checking"), wxOK | wxICON_ERROR);
		}
	}

	CollectResult();
	
	return m_ParseNode_Info;
}

bool SP_CPN_Parse_Function_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

 
	wxString l_sFunctionName = *(l_LeftNodeInfo->m_StringValue);
	wxString l_sAuguments = l_RightNodeInfo->m_CheckedString;
	wxString l_sCheckCS = l_RightNodeInfo->m_ColorSet;

	//checking parameters
	if( ! m_pColorSetClass->CheckParaTypeMatch(l_sFunctionName,l_sAuguments,l_sCheckCS) )
		return false;

	if(m_pColorSetClass->GetFunctionMap()&&
		m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end())
	{
		SP_CPN_Function l_FunctionStruct;
		l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName)->second;
		m_ParseNode_Info.m_DataType = l_FunctionStruct.m_ReturnDataType;
		m_ParseNode_Info.m_ColorSet = l_FunctionStruct.m_sReturnColorSet;
	}

	//m_ParseNode_Info.m_CheckedString = l_LeftNodeInfo->m_CheckedString + wxT("(") + l_RightNodeInfo->m_CheckedString + wxT(")");

	if(	l_LeftNodeInfo->m_NodeType != CPN_FUNCTIONNAME_NODE)   
	{
		wxString l_sError;					
		l_sError = wxT("Function type error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}

bool SP_CPN_Parse_Function_Node::GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
{		
/*	
	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	wxString l_sFunctionName   = *(l_LeftNodeInfo.m_StringValue);
	
	
	if(m_pColorSetClass->GetFunctionMap()&&
		m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end())
	{
		SP_CPN_Function l_FunctionStruct;
		l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName)->second;
		SP_CPN_DATATYPE l_DataType = l_FunctionStruct.m_ReturnDataType;
		l_FunctionStruct.m_pParseContext->SetFunctionFlag(true);
		l_FunctionStruct.m_pParseContext->SetFunctionName(l_FunctionStruct.m_sFunctionName);

		l_FunctionStruct.m_pParseContext->GetConstraints();
		SP_CPN_ParseNode_Info* l_NodeInfo = l_FunctionStruct.m_pParseContext->GetConstraints(

		if(l_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_IntConstraintExpr = ;
			l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);
		}
		else if(l_DataType == CPN_BOOLEAN)
		{
			m_ParseNode_Info.m_BoolConstraintExpr = l_FunctionStruct.m_pParseContext->GetConstraints();
			l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);
		}
		else
		{
			l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);
			return false;
		}		   
	}	
*/
	return false;
}


bool SP_CPN_Parse_Argument_Node::check()
{
		m_pLeft->check();
		SP_CPN_ParseNode_Info* l_pLeftNodeInfo = m_pLeft->GetParseNodeInfo();
		m_pRight->check();
		SP_CPN_ParseNode_Info* l_pRightNodeInfo = m_pRight->GetParseNodeInfo();

		wxString l_sLeftString, l_sRightString;
		wxString l_sLeftCS,   l_sRightCS;

		if(l_pLeftNodeInfo->m_NodeType == CPN_VARIABLE_NODE)
		{
			l_sLeftString = l_pLeftNodeInfo->m_CheckedString;
			l_sLeftCS     = wxT("#") + l_pLeftNodeInfo->m_ColorSet;
		}
		else if(l_pLeftNodeInfo->m_NodeType == CPN_ARGUMENT_NODE)
		{
			l_sLeftString = l_pLeftNodeInfo->m_CheckedString;
			l_sLeftCS     = l_pLeftNodeInfo->m_ColorSet;
		}
		else
		{
			if(l_pLeftNodeInfo->m_DataType == CPN_INTEGER)
			{
				l_sLeftString = wxString::Format(wxT("%d"),l_pLeftNodeInfo->m_IntegerValue);
			}
			else if(l_pLeftNodeInfo->m_DataType == CPN_BOOLEAN)
			{
				if(l_pLeftNodeInfo->m_BooleanValue == true)
					l_sLeftString = wxT("true");
				else
					l_sLeftString = wxT("false");
			}
			else			
			{
				l_sLeftString = *(l_pLeftNodeInfo->m_StringValue);
			}
			l_sLeftCS    << l_pLeftNodeInfo->m_DataType;
		}

		if(l_pRightNodeInfo->m_NodeType == CPN_VARIABLE_NODE)
		{
			l_sRightString = l_pRightNodeInfo->m_CheckedString;
			l_sRightCS     = wxT("#") + l_pRightNodeInfo->m_ColorSet;
		}
		else if(l_pRightNodeInfo->m_NodeType == CPN_ARGUMENT_NODE)
		{
			l_sRightString = l_pRightNodeInfo->m_CheckedString;
			l_sRightCS     = l_pRightNodeInfo->m_ColorSet;
		}
		else
		{
			if(l_pRightNodeInfo->m_DataType == CPN_INTEGER)
			{
				l_sRightString = wxString::Format(wxT("%d"),l_pRightNodeInfo->m_IntegerValue);
			}
			else if(l_pRightNodeInfo->m_DataType == CPN_BOOLEAN)
			{
				if(l_pRightNodeInfo->m_BooleanValue == true)
					l_sRightString = wxT("true");
				else
					l_sRightString = wxT("false");
			}
			else			
			{
				l_sRightString = *(l_pRightNodeInfo->m_StringValue);
			}		
			l_sRightCS    <<  l_pRightNodeInfo->m_DataType;
		}		
		 
		m_ParseNode_Info.m_CheckedString = l_sLeftString + wxT(",") + l_sRightString ;
		m_ParseNode_Info.m_ColorSet = l_sLeftCS + wxT(",") + l_sRightCS ;

		return true;
}




bool SP_CPN_Parse_IndexAbstract_Node::GetIndex()
{
	SP_CPN_ParseNode_Info l_NodeInfo = m_pLeft->evaluate();	

	wxString l_sIndexValue = *(l_NodeInfo.m_StringValue) ;
	l_sIndexValue = l_sIndexValue.AfterFirst(wxT('['));
	l_sIndexValue = l_sIndexValue.BeforeFirst(wxT(']'));
	long l_nIndex;
	
	if(l_sIndexValue.ToLong(&l_nIndex))
		m_ParseNode_Info.m_IntegerValue = l_nIndex;
	else
	{
		wxString l_sError;					
		l_sError = wxT("index value error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}


bool SP_CPN_Parse_Rand_Node::check()
{

	m_pLeft->check();
	SP_CPN_ParseNode_Info* l_pLeftNodeInfo = m_pLeft->GetParseNodeInfo();
	m_pRight->check();
	SP_CPN_ParseNode_Info* l_pRightNodeInfo = m_pRight->GetParseNodeInfo();

	wxString l_sLeftString, l_sRightString;
	wxString l_sLeftCS, l_sRightCS;

	if (l_pLeftNodeInfo->m_NodeType == CPN_VARIABLE_NODE && l_pLeftNodeInfo->m_NodeType == CPN_INTEGER)
	{
		l_sLeftString = l_pLeftNodeInfo->m_CheckedString;
		l_sLeftCS = l_pLeftNodeInfo->m_ColorSet;
		l_sRightString = wxString::Format(wxT("%d"), l_pRightNodeInfo->m_IntegerValue);
	}
	else if (l_pLeftNodeInfo->m_NodeType == CPN_ARGUMENT_NODE)
	{
		l_sLeftString = l_pLeftNodeInfo->m_CheckedString;
		l_sLeftCS = l_pLeftNodeInfo->m_ColorSet;
	}
	else
	{
		/**
		if (l_pLeftNodeInfo->m_DataType == CPN_INTEGER)
		{
			l_sLeftString = wxString::Format(wxT("%d"), l_pLeftNodeInfo->m_IntegerValue);
		}
		else if (l_pLeftNodeInfo->m_DataType == CPN_BOOLEAN)
		{
			if (l_pLeftNodeInfo->m_BooleanValue == true)
				l_sLeftString = wxT("true");
			else
				l_sLeftString = wxT("false");
		}
		else
		{
			l_sLeftString = *(l_pLeftNodeInfo->m_StringValue);
		}
		l_sLeftCS << l_pLeftNodeInfo->m_DataType;
		*/
	}

	//if (l_pRightNodeInfo->m_NodeType == CPN_VARIABLE_NODE)
////	{
	//	l_sRightString = l_pRightNodeInfo->m_CheckedString;
	//	l_sRightCS =  l_pRightNodeInfo->m_ColorSet;
	//}
	//else if (l_pRightNodeInfo->m_NodeType == CPN_ARGUMENT_NODE)
	//{
	//	l_sRightString = l_pRightNodeInfo->m_CheckedString;
	//	l_sRightCS = l_pRightNodeInfo->m_ColorSet;
	//}

		if (l_pRightNodeInfo->m_DataType == CPN_INTEGER)
		{
			l_sRightString = wxString::Format(wxT("%d"), l_pRightNodeInfo->m_IntegerValue);
		}

		else
		{
			l_sRightString = *(l_pRightNodeInfo->m_StringValue);
		}
		l_sRightCS << l_pRightNodeInfo->m_DataType;


	m_ParseNode_Info.m_CheckedString = l_sLeftString + wxT(",") + l_sRightString;
	m_ParseNode_Info.m_ColorSet = l_sLeftCS ;


	return true;
}


  SP_CPN_ParseNode_Info SP_CPN_Parse_Rand_Node::evaluate()
{
	//GetIndex();

	  SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pRight->evaluate();
	  SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();//->evaluate();

	  m_ParseNode_Info.m_DataType = l_RightNodeInfo->m_DataType;
	  m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

	  //TODO
	CollectResult();

	return m_ParseNode_Info;
}

bool SP_CPN_Parse_IndexAbstract_Node::check()
{
	if(!m_pLeft->check())
		return false;

	SP_CPN_ParseNode_Info* l_NodeInfo = m_pLeft->GetParseNodeInfo();
	//m_ParseNode_Info.m_CheckedString = wxT("@") + l_NodeInfo->m_CheckedString;

	if( ! (l_NodeInfo->m_NodeType == CPN_INDEX_NODE || l_NodeInfo->m_DataType == CPN_INDEX ) )
	{		
		wxString l_sError;					
		l_sError = wxT("Operands of index error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;		
	}
	
	
	if(m_ParseNode_Info.m_CheckColorSet != wxT(""))
	{		
		if(m_ParseNode_Info.m_ColorSet != m_ParseNode_Info.m_CheckColorSet)
		{
			wxString l_sError;					
			l_sError = wxT("Index mismatches the color set ** ") + m_ParseNode_Info.m_CheckColorSet+ wxT(". Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}	
	}	

	return true;
}

SP_CPN_ParseNode_Info SP_CPN_Parse_Dot_Node::evaluate() 
{	
	SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();	
	//SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

	SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(l_LeftNodeInfo.m_ColorSet);
	//(wxString l_sColor = l_pcColorSet->GetProductElement(*(l_LeftNodeInfo.m_StringValue),l_RightNodeInfo.m_IntegerValue);
	wxString l_sColor = l_pcColorSet->GetProductElement(*(l_LeftNodeInfo.m_StringValue),m_nPosition);

	if(m_ParseNode_Info.m_DataType == CPN_INTEGER)
	{
		long l_nColor;
		l_sColor.ToLong(&l_nColor);
		m_ParseNode_Info.m_IntegerValue = l_nColor;
	}
	else if(m_ParseNode_Info.m_DataType == CPN_BOOLEAN)
	{
		if(l_sColor == wxT("false"))
			m_ParseNode_Info.m_BooleanValue = false;
		else
			m_ParseNode_Info.m_BooleanValue = true;
	}
	else if(m_ParseNode_Info.m_DataType == CPN_UNION)
	{
		l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_ColorSet);
		if(l_pcColorSet->GetUnionDataType(l_sColor) == CPN_INTEGER)
		{
			long l_nColor;
			l_sColor.ToLong(&l_nColor);
			m_ParseNode_Info.m_IntegerValue = l_nColor;
		}
		else if(l_pcColorSet->GetUnionDataType(l_sColor) == CPN_BOOLEAN)
		{
			if(l_sColor == wxT("false"))
				m_ParseNode_Info.m_BooleanValue = false;
			else
				m_ParseNode_Info.m_BooleanValue = true;
		}
		else
		{
			m_String = l_sColor;
			m_ParseNode_Info.m_StringValue = &m_String;
		}
	}
	else
	{
		m_String = l_sColor;
		m_ParseNode_Info.m_StringValue = &m_String;
	}

	CollectResult();

	return m_ParseNode_Info;
}

bool SP_CPN_Parse_Dot_Node::check()
{
	if(!m_pLeft->check())
		return false;
	//if(!m_pRight->check())
	//	return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	//SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();
	

	//if( !(l_LeftNodeInfo->m_DataType == CPN_PRODUCT && l_RightNodeInfo->m_NodeType == CPN_INTEGERCONSTANT_NODE && l_RightNodeInfo->m_IntegerValue >0))
	if( !(l_LeftNodeInfo->m_DataType == CPN_PRODUCT))
	{
		wxString l_sError;					
		l_sError = wxT("Operands of dot error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(l_LeftNodeInfo->m_ColorSet);
	vector<wxString> l_vComponent = l_pcColorSet->GetComponentName();
	if(m_nPosition > (int)(l_vComponent.size()))
	{
		SP_MESSAGEBOX(wxT("The index of element extracting is not right"), wxT("Expression checking"), wxOK | wxICON_ERROR);
	}
	SP_CPN_ColorSet* l_pcColorSet2 = l_pcColorSet->GetComponentMap().find(l_vComponent[m_nPosition -1])->second;
	m_ParseNode_Info.m_ColorSet = l_pcColorSet2->GetName();
	m_ParseNode_Info.m_DataType = l_pcColorSet2->GetDataType();
	
	m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;

	return true;
}


bool SP_CPN_Parse_Index_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if( l_RightNodeInfo->m_DataType == CPN_INTEGER && l_LeftNodeInfo->m_NodeType == CPN_INDEXNAME_NODE)   // Only for index type
	{
		return true;
	}
	
	if (l_LeftNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
		return true;
	}

	wxString l_sError;
	l_sError = wxT("Operands of index error. Position: ") + m_sErrorPosition;
	SP_LOGERROR(l_sError);
	return false;
}



bool SP_CPN_Parse_Add_Node::check()
{
	if (m_ParseNode_Info.m_bSeparaterExpression)
	{
		m_pLeft->GetParseNodeInfo()->m_bSeparaterExpression = true;
		m_pRight->GetParseNodeInfo()->m_bSeparaterExpression = true;
	}
	
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;	

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;		
	}

	if (! m_ParseNode_Info.m_bSeparaterExpression)
	{
		if (!((l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER) ||
			  (l_LeftNodeInfo->m_DataType == CPN_STRING  && l_RightNodeInfo->m_DataType == CPN_STRING)))
		{
			wxString l_sError;
			l_sError = wxT("Operands of the Add operator error. Position: ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;

		if (l_LeftNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

		if (l_RightNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

		if (m_ParseNode_Info.m_ColorSet != wxT(""))
			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
		else
		{
			m_ParseNode_Info.m_sColorSetList = wxT("");
			m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
		}
	}
	else
	{
		//if the add expression is a part of a separator expression
		//will add the check codes
	}
	
	return true;
}

void SP_CPN_Parse_NumOf_Node::CollectResult()
{
	m_ParseNode_Info.m_EvalResults.clear();
	SP_CPN_EvaluatedSingleValue l_stEvalRes;
	SP_CPN_DATATYPE l_DataType = m_ParseNode_Info. m_DataType;


	if (l_DataType == CPN_INTEGER)
	{
		wxString l_sTemp;
		//l_sTemp << m_ParseNode_Info.m_Multiplicity;
		l_sTemp << m_ParseNode_Info.m_IntegerValue;
		l_stEvalRes.m_ColorValue = l_sTemp; //wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue);
		//m_ParseNode_Info.m_IntegerValue= m_ParseNode_Info.m_Multiplicity;
	}


	else if (l_DataType == CPN_BOOLEAN)
	{
		if (m_ParseNode_Info.m_BooleanValue == true)
			l_stEvalRes.m_ColorValue = wxT("true");
		else
			l_stEvalRes.m_ColorValue = wxT("false");
	}

	if ( l_DataType == CPN_DOUBLE)
	l_stEvalRes.m_DoubleMultiplicity = m_ParseNode_Info.m_DoubleMultiplicity;
	else
		l_stEvalRes.m_Multiplicity = m_ParseNode_Info.m_Multiplicity;

	l_stEvalRes.m_Predicate = m_ParseNode_Info.m_Predicate;
	l_stEvalRes.m_bPlaceFlag = m_ParseNode_Info.m_bPlaceFlag;
	m_ParseNode_Info.m_EvalResults.push_back(l_stEvalRes);

}



 wxString SP_CPN_Parse_NumOf_Node::eval(wxString p_scolour, wxString p_sTokenNUM, SP_DS_Node* p_pcNode)
{
	 SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast<SP_DS_TextAttribute*>(p_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	 if (l_pcNameAttibute)
	 {
		 wxString l_sColorSetName = l_pcNameAttibute->GetValue();
		 SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(l_sColorSetName);



		 if (l_pcColorSet)
		 {
			 std::vector<wxString> l_vColors = l_pcColorSet->GetStringValue();
			 for (auto itV = l_vColors.begin(); itV != l_vColors.end(); ++itV)
			 {
				 if (*itV == p_scolour)
				 {
						 return p_sTokenNUM;

				 }
			 }
		 }
	 }

	return wxT("0");
}

bool SP_CPN_Parse_NumOf_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	this->m_ParseNode_Info.m_DataType = l_RightNodeInfo->m_DataType;

	if (l_LeftNodeInfo->m_bPlaceFlag == false && l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;

	}

	if (!m_ParseNode_Info.m_bSeparaterExpression)
		{
			if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && (l_RightNodeInfo->m_DataType == CPN_INTEGER || l_RightNodeInfo->m_DataType == CPN_DOUBLE)))
			{
				wxString l_sError;
				l_sError = wxT("Operands of the numOf operator error. Position: ") + m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
	           m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;
			if (l_LeftNodeInfo->m_ColorSet != wxT(""))
				m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

			if (l_RightNodeInfo->m_ColorSet != wxT(""))
				m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

			if (m_ParseNode_Info.m_ColorSet != wxT(""))
				m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
			else
			{
				m_ParseNode_Info.m_sColorSetList = wxT("");
				m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
			}
		}



	return true;
}

bool SP_CPN_Parse_Substract_Node::check()
{

	if (m_ParseNode_Info.m_bSeparaterExpression)
	{
		m_pLeft->GetParseNodeInfo()->m_bSeparaterExpression = true;
		m_pRight->GetParseNodeInfo()->m_bSeparaterExpression = true;
	}

	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
	}

	if (!m_ParseNode_Info.m_bSeparaterExpression)
	{
		if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER))
		{
			wxString l_sError;
			l_sError = wxT("Operands of the Substract operator error. Position: ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;
		if (l_LeftNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

		if (l_RightNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

		if (m_ParseNode_Info.m_ColorSet != wxT(""))
			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
		else
		{
			m_ParseNode_Info.m_sColorSetList = wxT("");
			m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
		}
	}
	else
	{
		//if the substract expression is a part of a separator expression
		//will add the check codes
	}

	return true;
}

bool SP_CPN_Parse_Multiply_Node::check()
{
	if (m_ParseNode_Info.m_bSeparaterExpression)
	{
		m_pLeft->GetParseNodeInfo()->m_bSeparaterExpression = true;
		m_pRight->GetParseNodeInfo()->m_bSeparaterExpression = true;
	}

	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
	}

	if (!m_ParseNode_Info.m_bSeparaterExpression)
	{
		if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER))
		{
			wxString l_sError;
			l_sError = wxT("Operands of the Multiply operator error. Position: ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;
		if (l_LeftNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;
		if (l_RightNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

		if (m_ParseNode_Info.m_ColorSet != wxT(""))
			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
		else
		{
			m_ParseNode_Info.m_sColorSetList = wxT("");
			m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
		}
	}
	else
	{
		//if the add expression is a part of a separator expression
		//will add the check codes
	}

	return true;
}


bool SP_CPN_Parse_Divide_Node::check()
{
	if (m_ParseNode_Info.m_bSeparaterExpression)
	{
		m_pLeft->GetParseNodeInfo()->m_bSeparaterExpression = true;
		m_pRight->GetParseNodeInfo()->m_bSeparaterExpression = true;
	}

	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
	}

	if (!m_ParseNode_Info.m_bSeparaterExpression)
	{
		if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER))
		{
			wxString l_sError;
			l_sError = wxT("Operands of the Divide operator error. Position: ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;

		if (l_LeftNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

		if (l_RightNodeInfo->m_ColorSet != wxT(""))
			m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

		if (m_ParseNode_Info.m_ColorSet != wxT(""))
			m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
		else
		{
			m_ParseNode_Info.m_sColorSetList = wxT("");
			m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
		}
	}
	else
	{
		//if the add expression is a part of a separator expression
		//will add the check codes
	}

	return true;
}

bool SP_CPN_Parse_Modulo_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER) )
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the Modulo operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;

	if(l_LeftNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

	if(l_RightNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

	if(m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;	
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}

	return true;
}


bool SP_CPN_Parse_Power_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER) )
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the power operator error. Position: ") +m_sErrorPosition;
SP_LOGERROR(l_sError);
return false;
	}

	m_ParseNode_Info.m_DataType = l_LeftNodeInfo->m_DataType;

	if (l_LeftNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo->m_ColorSet;

	if (l_RightNodeInfo->m_ColorSet != wxT(""))
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo->m_ColorSet;

	if (m_ParseNode_Info.m_ColorSet != wxT(""))
		m_ParseNode_Info.m_sColorSetList = m_ParseNode_Info.m_ColorSet;
	else
	{
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
	}

	return true;
}


bool SP_CPN_Parse_Equal_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	///if (l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE)
	//{//george for numOf
	//	return true;
	//}

	//CPN_NUMOF_NODE, it is possible to compare both discrete and cont token num in places in hybrid nets
	if (l_LeftNodeInfo->m_DataType != l_RightNodeInfo->m_DataType &&  !(l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE))
	{
		wxString l_sError;
		l_sError = wxT("Operands of the logic Equal operator error. Position: ") + m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}


bool SP_CPN_Parse_NotEqual_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	//CPN_NUMOF_NODE, it is possible to compare both discrete and cont token num in places in hybrid nets
	if (l_LeftNodeInfo->m_DataType != l_RightNodeInfo->m_DataType &&  !(l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE))
	{
		wxString l_sError;
		l_sError = wxT("Operands of the logic NotEqual operator error. Position: ") + m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}


bool SP_CPN_Parse_GreaterThan_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER)&& !(l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE))
	{
		wxString l_sError;
		l_sError = wxT("Operands of the logic Greatthan operator error. Position: ") + m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}

bool SP_CPN_Parse_GreaterEqual_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER)&& !(l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE))

	{
		
		wxString l_sError;					
		l_sError = wxT("Operands of the logic Greaterthan&Equal operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}
////////////////////////////////////////

bool SP_CPN_Parse_Element_Of_Node::DoEval(wxString p_sColorSet, std::vector<wxString>& p_vColors)
{
	SP_CPN_ColorSet* l_pcColorSet2 = m_pColorSetClass->LookupColorSet(p_sColorSet);// (*m_pColorSetClass->GetColorSetVector())[k];

	if (!l_pcColorSet2)
		return false;

	std::vector<int> llV=l_pcColorSet2->GetIntColorValues();

	p_vColors = l_pcColorSet2->GetRealColorStringValue();

	return true;
	/**
	wxString l_sCsName = l_pcColorSet2->GetName();
	if (l_sCsName == p_sColorSet)
	{
		  
		if (l_pcColorSet2->IsColor(p_sColor))
		{
			m_ParseNode_Info.m_BooleanValue = TRUE;
			return true;
			
		}
		else
		{
			m_ParseNode_Info.m_BooleanValue = FALSE;
			return false;
		}
		
	}
	*/
	return false;
}

bool SP_CPN_Parse_Element_Of_Node::check()
{
	if (!m_pLeft->check())
		return false;
	if (!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (!((l_LeftNodeInfo->m_NodeType==CPN_VARIABLE_NODE || l_LeftNodeInfo->m_DataType == CPN_INTEGER || l_LeftNodeInfo->m_DataType == CPN_PRODUCT)&& (  m_pRight->GetParseNodeInfo()->m_ColorSet !=wxT(""))))

	{
		
		wxString l_sError;					
		l_sError = wxT("Operands of the elemOf operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	 
	
	//here we must add aux variable
	if (l_RightNodeInfo->m_NodeType == CPN_VARIABLE_NODE && l_RightNodeInfo->m_DataType != CPN_PRODUCT)
	{

		if (l_RightNodeInfo->m_DataType == CPN_ENUM || l_RightNodeInfo->m_DataType == CPN_INTEGER)
		{
			wxString l_sColourSets = l_RightNodeInfo->m_ColorSet;
			wxString l_sTemp = wxT("__AUX_CS_");
			wxString l_sCs = l_RightNodeInfo->m_CheckedString;
			l_sTemp << l_sCs << wxT("_0");

			SP_CPN_Variable_Constant l_variableStruct;
			l_variableStruct.m_ColorSet = l_sCs;
			l_variableStruct.m_DataType = l_RightNodeInfo->m_DataType;
			std::pair<wxString, SP_CPN_Variable_Constant> l_pair;
			l_pair.first = l_sTemp;
	    	l_pair.second = l_variableStruct;
			m_pColorSetClass->GetVariableMap()->insert(l_pair);
			 
	 	}
	
	}
	else if (l_RightNodeInfo->m_NodeType == CPN_VARIABLE_NODE &&  l_RightNodeInfo->m_DataType ==CPN_PRODUCT)
	{
		wxArrayString l_arrCS;
		wxString l_sColourSets = l_RightNodeInfo->m_sColorSetList;
		if (l_sColourSets.Contains(","))
		{
			l_arrCS.Add(l_sColourSets.BeforeFirst(','));
			l_arrCS.Add(l_sColourSets.AfterFirst(','));

		}
		//auxuliary varibles naming convention for Product: __AUX_CS_PRODCS_SIMPLECS_i
		wxString l_sTemp = wxT("__AUX_CS_");
		l_sTemp << l_RightNodeInfo->m_ColorSet << wxT("_");
		int j = 0;
		for (auto cs : l_arrCS)
		{
			wxString l_var;
			l_var << l_sTemp << cs << wxT("_") << j;

			SP_CPN_Variable_Constant l_variableStruct;

			l_variableStruct.m_ColorSet = cs;

		SP_CPN_ColorSet* l_pcColorSet=m_pColorSetClass->LookupColorSet(cs);

			if (l_pcColorSet)
			{
				l_variableStruct.m_DataType = l_pcColorSet->GetDataType();
			}

			std::pair<wxString, SP_CPN_Variable_Constant> l_pair;

			l_pair.first = l_var;
			l_pair.second = l_variableStruct;
			m_pColorSetClass->GetVariableMap()->insert(l_pair);
			j++;
		}

		
	}


	return true;
}
  bool SP_CPN_Parse_Element_Of_Node::GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
	{
	  m_pLeft->GetParseNodeInfo()->m_bIsElemOf = true;

      if (!m_pRight->GetConstraints(p_cCSPSolver))
          return false;

       if (!m_pLeft->GetConstraints(p_cCSPSolver))
           return false;

       SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();


        SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		std::vector<IntVar>* l_vRight = l_pcRightParseNode_Info->m_vIntconstraintVector;

		std::vector<IntVar>* l_vLeft = l_pcLeftParseNode_Info->m_vIntconstraintVector;
        
		if (l_vRight->size() == 2 && l_vLeft->size() == 2)
		{
			m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, m_pLeft->GetParseNodeInfo()->m_vIntconstraintVector->at(0)== m_pRight->GetParseNodeInfo()->m_vIntconstraintVector->at(0)&& m_pLeft->GetParseNodeInfo()->m_vIntconstraintVector->at(1) == m_pRight->GetParseNodeInfo()->m_vIntconstraintVector->at(1));
			return true;
		}

		return false;
  }
 



////////////////////////////////////////
bool SP_CPN_Parse_LessThan_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER))
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the logic Lessthan operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}

bool SP_CPN_Parse_LessEqual_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(!(l_LeftNodeInfo->m_DataType == CPN_INTEGER && l_RightNodeInfo->m_DataType == CPN_INTEGER)&& !(l_RightNodeInfo->m_NodeType == CPN_NUMOF_NODE || l_LeftNodeInfo->m_NodeType == CPN_NUMOF_NODE))
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the logic Lessthan&Equal operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}
	return true;
}


bool SP_CPN_Parse_Not_Node::check()
{
	if(!m_pLeft->check())
		return false;

	SP_CPN_ParseNode_Info* l_NodeInfo = m_pLeft->GetParseNodeInfo();

	if(l_NodeInfo->m_DataType != CPN_BOOLEAN )
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the logic Not operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}


bool SP_CPN_Parse_And_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	//SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if(l_LeftNodeInfo->m_DataType != CPN_BOOLEAN  || l_LeftNodeInfo->m_DataType != CPN_BOOLEAN)
	{
		wxString l_sError;					
		l_sError = wxT("Operands of the logic And operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}

bool SP_CPN_Parse_Or_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	//SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();
	
	if(l_LeftNodeInfo->m_DataType != CPN_BOOLEAN  || l_LeftNodeInfo->m_DataType != CPN_BOOLEAN)
	{		
		wxString l_sError;					
		l_sError = wxT("Operands of the logic Or operator error. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}

bool SP_CPN_Parse_Comma_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	wxString l_sLCS = l_LeftNodeInfo->m_sColorSetList;
	wxString l_sRCS = l_RightNodeInfo->m_sColorSetList;

	if (l_sRCS.IsEmpty())//george 11.2.21 which allows a function to be used in the tupel e.g., (x,fun(x,y))
	{
		if (l_RightNodeInfo->m_NodeType == CPN_FUNCTION_NODE)
		{
			l_sRCS = l_RightNodeInfo->m_ColorSet;
		}
	}

	if (l_sLCS.IsEmpty())//george 11.2.21 which allows a function to be used in the tupel e.g., (fun(x,y),z)
	{
		if (l_LeftNodeInfo->m_NodeType == CPN_FUNCTION_NODE)
		{
			l_sLCS = l_LeftNodeInfo->m_ColorSet;
		}
	}
	
	m_ParseNode_Info.m_sColorSetList = l_sLCS + wxT(",") + l_sRCS;

	return true;
}

void  SP_CPN_Parse_Bracket_Node::IsAllExistsInBrackets(SP_CPN_ParseNode* p_nodeRoot, bool & P_isFound)
{
	if (!p_nodeRoot)
		return;
	 
	// if node is leaf node, print its data    
	if (!p_nodeRoot->GetLeftNode() && !p_nodeRoot->GetRightNode())
	{
		if (p_nodeRoot->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
		{
			P_isFound = true;
			return ;
			
		}
	}

	if (p_nodeRoot->GetLeftNode())
		IsAllExistsInBrackets(p_nodeRoot->GetLeftNode(),P_isFound);

	// if right child exists, check for leaf 
	// recursively
	if (p_nodeRoot->GetRightNode())
		IsAllExistsInBrackets(p_nodeRoot->GetRightNode(), P_isFound);
}

void  SP_CPN_Parse_Bracket_Node::TraversAll(SP_CPN_ParseNode* p_nodeRoot, SP_VectorString l_vComponents, std::vector<int>& p_vIndex)
{
	if (!p_nodeRoot)
		return ;
	// if node is leaf node, print its data    
	if (!p_nodeRoot->GetLeftNode() && !p_nodeRoot->GetRightNode())
	{
		//cout << root->data << " ";
		if (p_nodeRoot->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
		{
			if(p_vIndex.size()==0)
			p_nodeRoot->SetColorSetName(l_vComponents[0]);
			else
			p_nodeRoot->SetColorSetName(l_vComponents[p_vIndex.size()]);
		}
		p_vIndex.push_back(p_vIndex.size() + 1);
		return ;
	}
	if (p_nodeRoot->GetLeftNode())
		 TraversAll(p_nodeRoot->GetLeftNode(), l_vComponents,p_vIndex);

	// if right child exists, check for leaf 
	// recursively
	if (p_nodeRoot->GetRightNode())
		 TraversAll(p_nodeRoot->GetRightNode(), l_vComponents, p_vIndex);
}

bool SP_CPN_Parse_Bracket_Node::IsAllExist(SP_CPN_ParseNode* p_node, wxString p_cs)
{
	if (p_node == NULL) return false;

	if (p_node->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
	{
		p_node->SetColorSetName(p_cs);

		 
		return true;
	}

	return IsAllExist(p_node->GetLeftNode(), p_cs);
}
/**
bool SP_CPN_Parse_Bracket_Node::IsAllExist(SP_CPN_ParseNode* p_node, SP_VectorString p_vCompCS,int pos,unsigned int& cummltivePos)
{
 
	if (p_node == NULL) return false;
	
	if (p_node)
	{
		if (p_node->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
		{
			int l_npos;
			if (cummltivePos > 0)
			{
				l_npos = cummltivePos;
			}
			else
			{
				l_npos = pos;
			}
			p_node->SetColorSetName(p_vCompCS[l_npos]);

			p_node->GetParseNodeInfo()->l_nAllPos = l_npos;
			cummltivePos++;
			 
			l_npos = cummltivePos;
			if (p_node->GetLeftNode())
				return IsAllExist(p_node->GetLeftNode(), p_vCompCS,pos, cummltivePos);

			if (p_node->GetRightNode())
				return IsAllExist(p_node->GetRightNode(), p_vCompCS, pos, cummltivePos);

			 
		}
		else
		{
			if (p_node->GetLeftNode())
				return IsAllExist(p_node->GetLeftNode(), p_vCompCS, pos , cummltivePos);

			if (p_node->GetRightNode())
				return IsAllExist(p_node->GetRightNode(), p_vCompCS, pos, cummltivePos);
		}
	}
	 
 
}
*/
bool SP_CPN_Parse_Bracket_Node::check()
{
	if(!m_pLeft->check())
		return false;	

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();	

    bool l_bHasNumOf= false;

    l_bHasNumOf= m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType==CPN_NUMOF_NODE || m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType==CPN_NUMOF_NODE;

	if (m_pLeft->GetLeftNode() && m_pLeft->GetRightNode() && l_LeftNodeInfo->m_NodeType == CPN_COMMA_NODE)//george 19.12
	{
		SP_CPN_ParseNode_Info* l_leftNodeInfo = m_pLeft->GetLeftNode()->GetParseNodeInfo();
		SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pLeft->GetRightNode()->GetParseNodeInfo();
		wxString ss;ss<< m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType;
		
		ss << wxT("|") << m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType;
		//SP_MESSAGEBOX(ss);
		bool l_bISExist;
		IsAllExistsInBrackets(m_pLeft, l_bISExist);
		if ((m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE || l_bISExist) && (m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE|| m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_BOOLEAN|| m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType ==CPN_CONSTANT_NODE))
		{
			if (m_ParseNode_Info.m_CheckColorSet == wxT("")) return true;
			SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
			if (!l_pcColorSet)
				return false;
			SP_VectorString l_vComponent= l_pcColorSet->GetComponentName();
			 
			if (l_vComponent.size() == 0) return false;
			unsigned int pos;
			//return IsAllExist(m_pLeft, l_vComponent[0]);
			std::vector<int> l_indexing;
			TraversAll(m_pLeft, l_vComponent, l_indexing);
			if (l_indexing.size() > 0)
				return true;
			else
				return false;
			 
		}
	}
	 
	if (m_pLeft->GetLeftNode() && m_pLeft->GetRightNode() )//george 19.12
	{//(all,VAR_CONST_NODE)
		if (m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC|| /*later*/m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
		{
			SP_CPN_ParseNode_Info* l_leftNodeInfo = m_pLeft->GetLeftNode()->GetParseNodeInfo();
			SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pLeft->GetRightNode()->GetParseNodeInfo();
			wxString ss; ss << m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType;

			ss << wxT("|") << m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType;
			SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
			if (!l_pcColorSet)
				return false;
			SP_VectorString l_vComponent = l_pcColorSet->GetComponentName();

			if (l_vComponent.size() == 0) return false;
		 
			 
			// return IsAllExist(m_pLeft, l_vComponent[0]);

			 std::vector<int> l_indexing;
			 TraversAll(m_pLeft, l_vComponent, l_indexing);
			 if (l_indexing.size() > 0)
				 return true;
			 else
				 return false;
			 
			 
		}
	}

	if(	l_LeftNodeInfo->m_NodeType != CPN_COMMA_NODE)
	{		
		wxString l_sError;					
		l_sError = wxT("Syntax error in the tuple expression. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	if (m_pLeft->GetLeftNode() && m_pLeft->GetRightNode() && l_LeftNodeInfo->m_NodeType == CPN_COMMA_NODE)
	{//(all(),var)19.12.2020
		if (m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC &&
		( m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE|| m_pLeft->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE))
		{
			return true;
		}
	}

	m_ParseNode_Info.m_sColorSetList = wxT("(") + l_LeftNodeInfo->m_sColorSetList + wxT(")");

	//Now begin to check the syntax
	if( m_ParseNode_Info.m_CheckColorSet != wxT(""))
	{		
		SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(m_ParseNode_Info.m_CheckColorSet);
		if(!l_pcColorSet)
			return false;

		//SP_MESSAGEBOX(l_pcColorSet->GetName());
		if( l_pcColorSet->GetDataType() == CPN_PRODUCT && ! l_pcColorSet->CheckProductSyntax( m_ParseNode_Info.m_sColorSetList,m_sErrorPosition ) && !l_bHasNumOf )
		{// if the tuple has numOF operator, the number of token may not be bounded to the color set of the of the variable, to do: deal with this case
			wxString l_sError;					
			l_sError = wxT(" product type mismatch. Position: ") + m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}

		if( l_pcColorSet->GetDataType() == CPN_UNION )
		{
			bool l_bSuccess = false;
			map<wxString, SP_CPN_ColorSet*>::iterator itMap;
			map<wxString, SP_CPN_ColorSet*> L_CSComponentMap = l_pcColorSet->GetComponentMap();
			for(itMap = L_CSComponentMap.begin(); itMap != L_CSComponentMap.end(); itMap++)
			{
				if(itMap->second->GetDataType() == CPN_PRODUCT && itMap->second->CheckProductSyntax( m_ParseNode_Info.m_sColorSetList,m_sErrorPosition ) )
				{
					l_bSuccess = true;
					break;
				}
			}
			if( ! l_bSuccess)
			{
				wxString l_sError;					
				l_sError = wxT(" product type mismatch. Position: ") + m_sErrorPosition;
				SP_LOGERROR(l_sError);
				return false;
			}
		}
	}

	return true;
}



SP_CPN_ParseNode_Info SP_CPN_Parse_Seperator_Node::evaluate() 
{

		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();
		
		m_ParseNode_Info.m_DataType = l_RightNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_RightNodeInfo.m_ColorSet;

		if (m_ParseNode_Info.m_bPlaceFlag == false)
		{
			int l_nMultiplicity;
			double l_dMultiplicity;
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (l_LeftNodeInfo.m_NodeType == CPN_INTEGERCONSTANT_NODE)
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_IntegerValue;
				else
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_DoubleValue;
				l_dMultiplicity = m_ParseNode_Info.m_DoubleMultiplicity;
			}
			else
			{
				m_ParseNode_Info.m_Multiplicity = l_LeftNodeInfo.m_IntegerValue;
				l_nMultiplicity = m_ParseNode_Info.m_Multiplicity;
			}			

			//for the all() function
			m_ParseNode_Info.m_EvalResults.clear();
			for (unsigned i = 0; i < l_RightNodeInfo.m_EvalResults.size(); i++)
			{
				SP_CPN_EvaluatedSingleValue l_stEvalRes;
				l_stEvalRes.m_bPlaceFlag = false;
				l_stEvalRes.m_ColorValue = l_RightNodeInfo.m_EvalResults[i].m_ColorValue;
				l_stEvalRes.m_Predicate = l_RightNodeInfo.m_EvalResults[i].m_Predicate;
				if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
				{
					l_stEvalRes.m_DoubleMultiplicity = l_dMultiplicity * l_RightNodeInfo.m_EvalResults[i].m_DoubleMultiplicity;
				}
				else
				{
					l_stEvalRes.m_Multiplicity = l_nMultiplicity * l_RightNodeInfo.m_EvalResults[i].m_Multiplicity;
				}

				m_ParseNode_Info.m_EvalResults.push_back(l_stEvalRes);
			}
		}
		else
		{
			//if there are places in the arc expressions: marking-dependent arcs
			wxString l_stringMultiplicity = l_LeftNodeInfo.m_stringMultiplicity;			

			//for the all() function
			m_ParseNode_Info.m_EvalResults.clear();
			for (unsigned i = 0; i < l_RightNodeInfo.m_EvalResults.size(); i++)
			{
				SP_CPN_EvaluatedSingleValue l_stEvalRes;
				l_stEvalRes.m_bPlaceFlag = true;
				l_stEvalRes.m_ColorValue = l_RightNodeInfo.m_EvalResults[i].m_ColorValue;
				l_stEvalRes.m_Predicate = l_RightNodeInfo.m_EvalResults[i].m_Predicate;
				if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
				{
					double l_dMul = l_RightNodeInfo.m_EvalResults[i].m_DoubleMultiplicity;
					if (l_dMul - 1 < 0.000000001)  //evaluate if two float numbers are equal or not
					{
						l_stEvalRes.m_stringMultiplicity = l_stringMultiplicity;
					}
					else
					{
						wxString l_String = wxString::Format(wxT("%f"), l_dMul);
						l_stringMultiplicity = wxT("(") + l_stringMultiplicity + wxT(")");
						l_stEvalRes.m_stringMultiplicity = l_stringMultiplicity + wxT("*") + l_String;
					}					
				}
				else
				{
					int l_nMul = l_RightNodeInfo.m_EvalResults[i].m_Multiplicity;
					if (l_nMul == 1)
					{
						l_stEvalRes.m_stringMultiplicity = l_stringMultiplicity;
					}
					else
					{
						wxString l_String = wxString::Format(wxT("%d"), l_nMul);
						l_stringMultiplicity = wxT("(") + l_stringMultiplicity + wxT(")");
						l_stEvalRes.m_stringMultiplicity = l_stringMultiplicity + wxT("*") + l_String;
					}					
				}
				m_ParseNode_Info.m_EvalResults.push_back(l_stEvalRes);
			}
		}

		return m_ParseNode_Info;
    }


bool SP_CPN_Parse_Seperator_Node::check()
{
	m_pLeft->GetParseNodeInfo()->m_bSeparaterExpression = true;

	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();	
	

	if (l_LeftNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
	}
	

	if( m_sPlaceType != SP_DS_CONTINUOUS_PLACE)
	{
		if (l_LeftNodeInfo->m_DataType != CPN_INTEGER && !m_ParseNode_Info.m_bPlaceFlag)
		{
			wxString l_sError;					
			l_sError = wxT("The multiplicity should be an integer type. Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}

	if( m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
	{
		if (!(l_LeftNodeInfo->m_DataType == CPN_INTEGER  || l_LeftNodeInfo->m_NodeType == CPN_DOUBLECONSTANT_NODE) && !m_ParseNode_Info.m_bPlaceFlag)
		{
			wxString l_sError;					
			l_sError = wxT("The multiplicity should be an integer or double type. Position: ") +m_sErrorPosition;
			SP_LOGERROR(l_sError);
			return false;
		}
	}


	if( l_RightNodeInfo->m_NodeType == CPN_CONNECTOR_NODE ||
		l_RightNodeInfo->m_NodeType == CPN_PREDICATE_NODE ||
		l_RightNodeInfo->m_NodeType == CPN_SEPERATOR_NODE )
	{
		wxString l_sError;					
		l_sError = wxT("Synatx error in the separator operator. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}	

	return true;
}


bool SP_CPN_Parse_Predicate_Node::check()
{
	if(!m_pLeft->check())
		return false;
	if(!m_pRight->check())
		return false;

	SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
	SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

	if (l_RightNodeInfo->m_bPlaceFlag == true)
	{
		m_ParseNode_Info.m_bPlaceFlag = true;
	}

	if( l_LeftNodeInfo->m_DataType != CPN_BOOLEAN )
	{
		wxString l_sError;					
		l_sError = wxT("Predicate should be boolean. Position: ") +m_sErrorPosition;
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}



SP_CPN_ParseNode_Info SP_CPN_Parse_Predicate_Node::evaluate() 
{	
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		bool l_bPredicate = l_LeftNodeInfo.m_BooleanValue;

		if(l_bPredicate)
		{
			m_ParseNode_Info.m_EvalResults = l_RightNodeInfo.m_EvalResults;
		}
		else
		{
			m_ParseNode_Info.m_EvalResults.clear();
		}

		return m_ParseNode_Info;
}


void SP_CPN_Parse_CNN_Node::SetEvalResult(SP_CPN_EvaluatedSingleValue p_SingleEvalResult)
{
		wxString l_sNewColor = p_SingleEvalResult.m_ColorValue;
		bool l_bFound = false;
		for(unsigned i = 0; i < m_ParseNode_Info.m_EvalResults.size(); i++)
		{
			wxString l_sExistColor = m_ParseNode_Info.m_EvalResults[i].m_ColorValue;
			if(l_sNewColor == l_sExistColor)
			{
				if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
				{
					if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("+") + p_SingleEvalResult.m_stringMultiplicity;
					else if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && !p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("+") + wxString::Format(wxT("%f"), p_SingleEvalResult.m_DoubleMultiplicity);
					else if (!m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = wxString::Format(wxT("%f"), m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity) + wxT("+") + p_SingleEvalResult.m_stringMultiplicity;
					else
						m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity + p_SingleEvalResult.m_DoubleMultiplicity;
				}					
				else
				{
					if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("+") + p_SingleEvalResult.m_stringMultiplicity;
					else if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && !p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("+") + wxString::Format(wxT("%d"), p_SingleEvalResult.m_Multiplicity);
					else if (!m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
						m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = wxString::Format(wxT("%d"), m_ParseNode_Info.m_EvalResults[i].m_Multiplicity) + wxT("+") + p_SingleEvalResult.m_stringMultiplicity;
					else
						m_ParseNode_Info.m_EvalResults[i].m_Multiplicity = m_ParseNode_Info.m_EvalResults[i].m_Multiplicity + p_SingleEvalResult.m_Multiplicity;
				}
				
				l_bFound = true;
				break;
			}			
		}

		if(!l_bFound)
		{
			m_ParseNode_Info.m_EvalResults.push_back( p_SingleEvalResult );
		}
}



void SP_CPN_Parse_CDD_Node::SetEvalResult(SP_CPN_EvaluatedSingleValue p_SingleEvalResult,const bool& p_bIsRight)
{
	wxString l_sNewColor = p_SingleEvalResult.m_ColorValue;
	bool l_bFound = false;
	for (unsigned i = 0; i < m_ParseNode_Info.m_EvalResults.size(); i++)
	{
		wxString l_sExistColor = m_ParseNode_Info.m_EvalResults[i].m_ColorValue;
		if (l_sNewColor == l_sExistColor)
		{
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("-") + p_SingleEvalResult.m_stringMultiplicity;
				else if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && !p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("-") + wxString::Format(wxT("%f"), p_SingleEvalResult.m_DoubleMultiplicity);
				else if (!m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = wxString::Format(wxT("%f"), m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity) + wxT("-") + p_SingleEvalResult.m_stringMultiplicity;
				else
					m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_DoubleMultiplicity + p_SingleEvalResult.m_DoubleMultiplicity;
			}
			else
			{
				if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("-") + p_SingleEvalResult.m_stringMultiplicity;
				else if (m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && !p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity + wxT("-") + wxString::Format(wxT("%d"), p_SingleEvalResult.m_Multiplicity);
				else if (!m_ParseNode_Info.m_EvalResults[i].m_bPlaceFlag && p_SingleEvalResult.m_bPlaceFlag)
					m_ParseNode_Info.m_EvalResults[i].m_stringMultiplicity = wxString::Format(wxT("%d"), m_ParseNode_Info.m_EvalResults[i].m_Multiplicity) + wxT("-") + p_SingleEvalResult.m_stringMultiplicity;
				else
					m_ParseNode_Info.m_EvalResults[i].m_Multiplicity = m_ParseNode_Info.m_EvalResults[i].m_Multiplicity - p_SingleEvalResult.m_Multiplicity;
			}

			l_bFound = true;
			break;
		}
	}

	if (!l_bFound && !p_bIsRight)
	{
		m_ParseNode_Info.m_EvalResults.push_back(p_SingleEvalResult);
	}

	if (p_bIsRight)
	{
		for (auto itV = m_ParseNode_Info.m_EvalResults.begin(); itV != m_ParseNode_Info.m_EvalResults.end(); ++itV)
		{
			if (itV->m_ColorValue == l_sNewColor)
			{
			//	itV->m_Multiplicity= itV->m_Multiplicity-
			}
		}
	}
}




//////////////////////////////////////////////////////////////////////////////
// SP_CPN_Parse_Context
//////////////////////////////////////////////////////////////////////////////
SP_CPN_Parse_Context::SP_CPN_Parse_Context()
{
	clearExpressions();
	m_bFunctionFlag = false;
	m_sErrorPosition = wxT("");
	m_psvColored2UnColoredPlaceNames = NULL;
	/*SP_DS_ColPN_Unfolding l_unfolding;
	l_unfolding.UnfoldPlacesForMarkingDependent();
	m_psvColored2UnColoredPlaceNames = l_unfolding.GetsvColored2UnColoredPlaceNames();
	*/

}

SP_CPN_Parse_Context::SP_CPN_Parse_Context(SP_CPN_ColorSetClass* p_pColorSetClass, wxString p_ColorSetName)
{
	m_pColorSetClass = 	p_pColorSetClass;   //defined in the global variables
	m_ColorSetName = p_ColorSetName;
}


bool SP_CPN_Parse_Context::AssignDataType(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName)
{
	SP_CPN_ParseNode* l_CurrParseNode = p_pcNode;
	wxString l_ColorSetName = p_sColorSetName;

	if(l_ColorSetName == wxT(""))    // for the case of guard now
		return true;

	if( !l_CurrParseNode )
		return false;

	if(l_CurrParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE)
	{
		AssignDataType( l_CurrParseNode->GetLeftNode(),  l_ColorSetName);
		AssignDataType( l_CurrParseNode->GetRightNode(), l_ColorSetName);
	}
	else if(l_CurrParseNode->GetParseNodeInfo()->m_NodeType == CPN_PREDICATE_NODE)
	{		
		AssignDataType( l_CurrParseNode->GetRightNode(), l_ColorSetName);
	}
	else if(l_CurrParseNode->GetParseNodeInfo()->m_NodeType == CPN_SEPERATOR_NODE)
	{		
		AssignDataType( l_CurrParseNode->GetRightNode(), l_ColorSetName);
	}
	else
	{
		l_CurrParseNode->GetParseNodeInfo()->m_CheckColorSet = l_ColorSetName;
	}	
	return true;
}


/*
bool SP_CPN_Parse_Context::DealUnionWithProduct(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName)
{
	if(p_sColorSetName == wxT(""))    // for the case of guard now
		return true;

	SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(p_sColorSetName);

	if( ! l_pcColorSet )
		return false;

	if( l_pcColorSet->GetDataType() != CPN_UNION )
	{
		AssignType( p_pcNode, p_sColorSetName);
		return true;
	}
	
	bool l_bFoundProduct = false;
	vector<wxString> l_ComponentNameVector;
	l_ComponentNameVector = m_pColorSetClass->LookupColorSet(p_sColorSetName)->GetComponentName();
	for(unsigned i = 0; i < l_ComponentNameVector.size(); i++)
	{
		SP_CPN_ColorSet* l_pcColorSet1 = m_pColorSetClass->LookupColorSet(l_ComponentNameVector[i]);

		if( ! l_pcColorSet1 )
		return false;

		if(l_pcColorSet1 ->GetDataType() == CPN_PRODUCT)
		{
			l_bFoundProduct = true;
			break;
		}		
	}

	if( ! l_bFoundProduct )
	{
		AssignType( p_pcNode, p_sColorSetName);
		return true;
	}

	//then deal with the union type with a product type

	bool l_bCheck = false;

	

	CheckUnionWithProduct( p_pcNode, l_ComponentNameVector, l_bCheck);

	if( ! l_bCheck )
		return false;

	return true;

}
*/
/*
bool SP_CPN_Parse_Context::CheckUnionWithProduct(SP_CPN_ParseNode* p_pcNode, vector<wxString> p_ComponentNameVector,bool &p_bCheck)
{
	if(p_pcNode->GetParseNodeInfo()->m_NodeType != CPN_CONNECTOR_NODE)
	{
		//
		vector<wxString > l_sTempErrorMesVector = m_sErrorMesVector;
		//

		bool l_bPassCheck = false;
		for(unsigned i = 0; i < p_ComponentNameVector.size(); i++)
		{
			//SP_CPN_ColorSet* l_pcColorSet = m_pColorSetClass->LookupColorSet(p_ComponentNameVector[i]);
			if( AssignType( p_pcNode, p_ComponentNameVector[i]) )
			{
				if( p_pcNode->check() )
				{
					l_bPassCheck = true;
					break;
				}			
			}			
		}
		if( l_bPassCheck )
		{
			p_bCheck = true;

			m_sErrorMesVector.clear();
			m_sErrorMesVector = l_sTempErrorMesVector;
		}
		else
			p_bCheck = false;
	
	}
	else
	{
		if(p_pcNode->GetLeftNode())
			CheckUnionWithProduct( p_pcNode->GetLeftNode(),p_ComponentNameVector,p_bCheck );
		if(p_pcNode->GetRightNode())
			CheckUnionWithProduct( p_pcNode->GetRightNode(),p_ComponentNameVector,p_bCheck );
	}

	return true;
}
*/
/*
bool SP_CPN_Parse_Context::AssignType(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName)
{

	SP_CPN_ParseNode* l_ParseNode = p_pcNode;
	wxString l_ColorSetName = p_sColorSetName;

	if(l_ColorSetName == wxT(""))    // for the case of guard now
		return true;

	// The following codes will get expressions to be assigned types
	// For example, for 1`(x,y), it will get (x,y).

	if( !l_ParseNode )
		return false;

	if(l_ParseNode->GetParseNodeInfo()->m_NodeType != CPN_CONNECTOR_NODE)
	{
		if(l_ParseNode->GetParseNodeInfo()->m_NodeType != CPN_PREDICATE_NODE)
		{
			if(l_ParseNode->GetParseNodeInfo()->m_NodeType != CPN_SEPERATOR_NODE)
			{
				l_ParseNode->GetParseNodeInfo()->m_CheckColorSet = l_ColorSetName; // Get from parameter				
				traverse(l_ParseNode);
			}
			else
			{
				if( !l_ParseNode->GetRightNode())
					return false;

				l_ParseNode->GetRightNode()->GetParseNodeInfo()->m_CheckColorSet = l_ColorSetName; // Get from parameter
				traverse(l_ParseNode->GetRightNode());
			}
		}
		else
		{
			if( !l_ParseNode->GetRightNode())
					return false;

			if(l_ParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType != CPN_SEPERATOR_NODE )
			{
				l_ParseNode->GetRightNode()->GetParseNodeInfo()->m_CheckColorSet = l_ColorSetName; // Get from parameter
				traverse(l_ParseNode->GetRightNode());
			}
			else
			{
				if( !l_ParseNode->GetRightNode()->GetRightNode())
					return false;

				l_ParseNode->GetRightNode()->GetRightNode()->GetParseNodeInfo()->m_CheckColorSet = l_ColorSetName; // Get from parameter
				traverse(l_ParseNode->GetRightNode()->GetRightNode());
			}
		}
		return true;
	}


	if ( !l_ParseNode->GetLeftNode() )
		return false;

	if ( !l_ParseNode->GetRightNode() )
		return false;

	AssignType( l_ParseNode->GetLeftNode(),  l_ColorSetName);
	AssignType( l_ParseNode->GetRightNode(), l_ColorSetName);

	return true;
}
*/




/*
bool SP_CPN_Parse_Context::traverse(SP_CPN_ParseNode* p_pcNode)
{
	//deal with each comma node

	if(!p_pcNode)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	// The end condition of recursive traverse
	vector<wxString> l_ComponentName;

	if(l_pParseNode->GetParseNodeInfo()->m_NodeType != CPN_BRACKET_NODE)   // CPN_BRACKET_NODE
		return true;

		// dealing with comma node
	if(m_pColorSetClass->LookupColorSet(l_pParseNode->GetParseNodeInfo()->m_CheckColorSet))
		l_ComponentName = m_pColorSetClass->LookupColorSet(l_pParseNode->GetParseNodeInfo()->m_CheckColorSet)->GetComponentName();
	else
		return false;

	if( l_ComponentName.size() == 0)
		return false;
	
	if( !l_pParseNode->GetLeftNode())
		return false;

	SP_CPN_ParseNode* p_TempNode = 	l_pParseNode->GetLeftNode();

	for(unsigned i = l_ComponentName.size()-1; i >=0; i--)
	{
		if( i < 0 || i >= l_ComponentName.size()) 
			return true;

		if(i == l_ComponentName.size()-1)
		{
			if(p_TempNode->GetRightNode())	
			{
				if( p_TempNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE)
				{
					//if it is comma node, then the expression is not right
					m_sErrorMesVector.push_back( wxT("Product type mismatch") );
					return false;
				}
				p_TempNode->GetRightNode()->GetParseNodeInfo()->m_CheckColorSet = l_ComponentName[i];
				
				//p_TempNode->GetRightNode()->GetParseNodeInfo()->m_Bracket  = wxT("(");  //not used
				
				traverse(p_TempNode->GetRightNode());
			}
			else
			{
				//wxString l_sError;					
				//l_sError << wxT("product types mismatch");
				//SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);	
				m_sErrorMesVector.push_back( wxT("Product type mismatch") );
				return false;
			}
				
		}
		else if(i == 0)
		{
			if(p_TempNode)
			{
				if( p_TempNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE)
				{
					//if it is comma node, then the expression is not right
					m_sErrorMesVector.push_back( wxT("Product type mismatch") );
					return false;
				}

				p_TempNode->GetLeftNode()->GetParseNodeInfo()->m_CheckColorSet = l_ComponentName[i];
				//p_TempNode->GetLeftNode()->GetParseNodeInfo()->m_Bracket  = wxT(")");

				traverse(p_TempNode);
			}
			else
			{
				//wxString l_sError;					
				//l_sError << wxT("product types mismatch");
				//SP_MESSAGEBOX(l_sError, wxT("Expression checking"), wxOK | wxICON_ERROR);	
				m_sErrorMesVector.push_back( wxT("Product type mismatch") );
				return false;
			}
		}
		else
		{
			if(p_TempNode && p_TempNode->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE)
			{
				if(p_TempNode->GetRightNode())
				{

					if( p_TempNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE)
					{
						//if it is comma node, then the expression is not right
						m_sErrorMesVector.push_back( wxT("Product type mismatch") );
						return false;
					}

					p_TempNode->GetRightNode()->GetParseNodeInfo()->m_CheckColorSet = l_ComponentName[i];

					traverse(p_TempNode->GetRightNode());
				}
				else
				{				
					m_sErrorMesVector.push_back( wxT("Product type mismatch") );
					return false;
				}
			}
			else
			{
				

				m_sErrorMesVector.push_back( wxT("Product type mismatch") );
				return false;
			}
		}

		if( i > 1 )
		{
			p_TempNode = p_TempNode->GetLeftNode();
		}
	}

	return true;
}
*/

bool SP_CPN_Parse_Context::CheckExpression()
{
	
	if(m_ExpressionsVector.size() > 0)
	{		
		Configuration(m_ExpressionsVector[0]);

		//if( ! DecideVariableNodeType(m_ExpressionsVector[0]) )
		//	return false;

		if( ! AssignDataType( m_ExpressionsVector[0], m_ColorSetName) )
			return false;

		if( !m_ExpressionsVector[0]->check())
		{			
			return false;
		}		
	}
	return true;
}





bool SP_CPN_Parse_Context::EvaluateExpression( vector<SP_CPN_EvaluatedSingleValue> &p_EvaluateResultArray)
{
	if(m_ExpressionsVector.size() > 0)
	{
		SP_CPN_ParseNode_Info l_NodeInfo = m_ExpressionsVector[0]->evaluate();

		for(unsigned i = 0; i < l_NodeInfo.m_EvalResults.size(); i++)
		{
			p_EvaluateResultArray.push_back( l_NodeInfo.m_EvalResults[i] );
		}
	}

	return true;
}


bool SP_CPN_Parse_Context::TraversePattern(SP_CPN_ParseNode* p_Node)
{
		//deal with each comma node

	if(!p_Node)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_Node;

	// The end condition of recursive traverse

	if( !(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE || l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE))
	{
		if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
			m_VariableNode[l_pParseNode->GetParseNodeInfo()->m_CheckedString] = l_pParseNode->GetParseNodeInfo()->m_ColorSet;			
		else if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE  ||
				l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
			m_ConstantNode.push_back(wxT("Constant Node"));
		else
			m_OtherNode.push_back(wxT("NonPattern Node"));
		
		return true;
	}
	// dealing with comma node

	if(l_pParseNode->GetLeftNode())
		TraversePattern(l_pParseNode->GetLeftNode());

	if(l_pParseNode->GetRightNode())
		TraversePattern(l_pParseNode->GetRightNode());

	return true;
}


bool SP_CPN_Parse_Context::ExtractPatternNew(vector<SP_CPN_ExpressionPatternInfo> &p_ExprPatterns)
{
	//vector<SP_CPN_ExpressionPatternInfo> l_vExprPatterns;  //will be returned.

	SP_CPN_ParseNode* l_pcParseNode = m_ExpressionsVector[0];

	if(m_ColorSetName == wxT("") )
	{
		if (! TraverseGuardPatterns1( l_pcParseNode,p_ExprPatterns ) )
			return true;
		else
			return false;
	}

	

	if(l_pcParseNode->GetParseNodeInfo()->m_NodeType != CPN_CONNECTOR_NODE)
	{
		//deal
		if( ! ProcessforPattern(l_pcParseNode,p_ExprPatterns) )
			return false;
	}
	else
	{
		while(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE)
		{
			//deal
			if( ! ProcessforPattern(l_pcParseNode->GetRightNode(),p_ExprPatterns) )
				return false;

			l_pcParseNode = l_pcParseNode->GetLeftNode();
		}

		if( ! ProcessforPattern(l_pcParseNode,p_ExprPatterns) )
			return false;
	}

	return true;	
}



bool SP_CPN_Parse_Context::ProcessforPattern(SP_CPN_ParseNode* p_pcParseNode,vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns)
{
	SP_MapString2String l_mVariableNode;				// for pattern finding
	vector<wxString>	l_vConstantNode;				// for pattern finding
	vector<wxString>	l_vOtherNode;					// for pattern finding

	SP_CPN_ExpressionPatternInfo l_ExprPatternInfo;

	if( p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_SEPERATOR_NODE )
	{
		if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_IntegerValue >= 1)
		{
			l_ExprPatternInfo.m_Multiplicity = p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_IntegerValue;			
		}
		else
		{
			return true;
		}

		if( p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE ) 
		{
			l_ExprPatternInfo.m_Multiplicity = 1;		

			if( ! TraverseTuplePattern(p_pcParseNode->GetRightNode(),l_mVariableNode,l_vConstantNode,l_vOtherNode) )
				return false;

			if( l_vOtherNode.size() != 0 )
			{
					return true;
			}
			else if( l_vConstantNode.size() != 0 && l_mVariableNode.size() != 0)
			{
				l_ExprPatternInfo.m_PatternType = CPN_PATTERN_KEYTUPLE;			
				l_ExprPatternInfo.m_VariableName = l_mVariableNode;
			}
			else if( l_vConstantNode.size() != 0 && l_mVariableNode.size() == 0)
			{
				l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;
					
			}
			else
			{
				l_ExprPatternInfo.m_PatternType = CPN_PATTERN_TUPLE;
				l_ExprPatternInfo.m_VariableName = l_mVariableNode;
			}

		}
		else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_VARIABLE;
			l_ExprPatternInfo.m_VariableName[p_pcParseNode->GetParseNodeInfo()->m_CheckedString] = p_pcParseNode->GetParseNodeInfo()->m_ColorSet;
			l_ExprPatternInfo.m_Multiplicity = 1;		
		}
		else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;			
			l_ExprPatternInfo.m_Multiplicity = 1;		
		}
		else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;		
			l_ExprPatternInfo.m_Multiplicity = 1;		
		}
		else
		{
			return true;
		}
		l_ExprPatternInfo.m_Expression = p_pcParseNode->GetRightNode()->GenerateExpression();
	}
	else if( p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE ) 
	{
		l_ExprPatternInfo.m_Multiplicity = 1;		

		if( !TraverseTuplePattern(p_pcParseNode,l_mVariableNode,l_vConstantNode,l_vOtherNode) )
			return false;

		if( l_vOtherNode.size() != 0 )
		{
				return true;
		}
		else if( l_vConstantNode.size() != 0 && l_mVariableNode.size() != 0)
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_KEYTUPLE;			
			l_ExprPatternInfo.m_VariableName = l_mVariableNode;
		}
		else if( l_vConstantNode.size() != 0 && l_mVariableNode.size() == 0)
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;
				
		}
		else
		{
			l_ExprPatternInfo.m_PatternType = CPN_PATTERN_TUPLE;
			l_ExprPatternInfo.m_VariableName = l_mVariableNode;
		}
		l_ExprPatternInfo.m_Expression = p_pcParseNode->GenerateExpression();

	}
	else if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
	{
		l_ExprPatternInfo.m_PatternType = CPN_PATTERN_VARIABLE;
		l_ExprPatternInfo.m_VariableName[p_pcParseNode->GetParseNodeInfo()->m_CheckedString] = p_pcParseNode->GetParseNodeInfo()->m_ColorSet;
		l_ExprPatternInfo.m_Multiplicity = 1;	
		l_ExprPatternInfo.m_Expression = p_pcParseNode->GenerateExpression();
	}
	else if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
	{
		l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;			
		l_ExprPatternInfo.m_Multiplicity = 1;		
		l_ExprPatternInfo.m_Expression = p_pcParseNode->GenerateExpression();
	}
	else if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
	{
		l_ExprPatternInfo.m_PatternType = CPN_PATTERN_CONSTANT;		
		l_ExprPatternInfo.m_Multiplicity = 1;	
		l_ExprPatternInfo.m_Expression = p_pcParseNode->GenerateExpression();
	}
	else
	{
		return true;
	}

	p_ExprPatterns.push_back(l_ExprPatternInfo);

	return true;
}


bool SP_CPN_Parse_Context::TraverseTuplePattern(SP_CPN_ParseNode* p_Node, SP_MapString2String &p_mVariableNode,vector<wxString> &p_vConstantNode,vector<wxString> &p_vOtherNode)
{
	//deal with each comma node

	if(!p_Node)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_Node;

	// The end condition of recursive traverse

	if( !(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_COMMA_NODE || l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE))
	{
		if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
			p_mVariableNode[l_pParseNode->GetParseNodeInfo()->m_CheckedString] = l_pParseNode->GetParseNodeInfo()->m_ColorSet;			
		else if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE  ||
				l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
			p_vConstantNode.push_back(wxT("Constant Node"));
		else
			p_vOtherNode.push_back(wxT("NonPattern Node"));
		
		return true;
	}
	// dealing with comma node

	if(l_pParseNode->GetLeftNode())
		TraverseTuplePattern(l_pParseNode->GetLeftNode(),p_mVariableNode,p_vConstantNode,p_vOtherNode);

	if(l_pParseNode->GetRightNode())
		TraverseTuplePattern(l_pParseNode->GetRightNode(),p_mVariableNode,p_vConstantNode,p_vOtherNode);

	
	return true;
}



SP_CPN_ExpressionPatternInfo SP_CPN_Parse_Context::ExtractPattern()
{
	m_VariableNode.clear();
	m_ConstantNode.clear();
	m_OtherNode.clear();

	SP_CPN_ExpressionPatternInfo m_ExpressionInfo;


		if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_SEPERATOR_NODE)
		{
			if(m_ExpressionsVector[0]->GetLeftNode()->GetParseNodeInfo()->m_IntegerValue > 1)
			{
				m_ExpressionInfo.m_Multiplicity = m_ExpressionsVector[0]->GetLeftNode()->GetParseNodeInfo()->m_IntegerValue;
				m_ExpressionInfo.m_MultiSet = true;
			}
			else
			{
				m_ExpressionInfo.m_MultiSet = false;
			}
			
			if(m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE)  // CPN_BRACKET_NODE
			{
				TraversePattern(m_ExpressionsVector[0]->GetRightNode());
				if( m_OtherNode.size() != 0 )
				{
					m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
				}
				else if( m_ConstantNode.size() != 0 && m_VariableNode.size() != 0)
				{
					m_ExpressionInfo.m_PatternType = CPN_PATTERN_KEYTUPLE;
					//for(unsigned i = 0; i < m_VariableNode.size(); i++)
						m_ExpressionInfo.m_VariableName = m_VariableNode;
				}
				else if( m_ConstantNode.size() != 0 && m_VariableNode.size() == 0)
				{
					m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;
					//m_ExpressionInfo.m_VariableName.push_back(m_VariableNode[i]);
				}
				else
				{
					m_ExpressionInfo.m_PatternType = CPN_PATTERN_TUPLE;
					//for(unsigned i = 0; i < m_VariableNode.size(); i++)
					m_ExpressionInfo.m_VariableName = m_VariableNode;
				}
			
			}

			else if(m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_VARIABLE;
				m_ExpressionInfo.m_VariableName[m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_CheckedString] = m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_ColorSet;
			}			
			else if(m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;				
			}
			else if(m_ExpressionsVector[0]->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;				
			}
			else
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
			}
		}

		else if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE)
		{
			m_ExpressionInfo.m_Multiplicity = 1;
			m_ExpressionInfo.m_MultiSet = false;

			TraversePattern(m_ExpressionsVector[0]);
			if( m_OtherNode.size() != 0 )
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
			}
			else if( m_ConstantNode.size() != 0 && m_VariableNode.size() != 0)
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_KEYTUPLE;
				for(unsigned i = 0; i < m_VariableNode.size(); i++)
					m_ExpressionInfo.m_VariableName = m_VariableNode;
			}
			else if( m_ConstantNode.size() != 0 && m_VariableNode.size() == 0)
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;
				
			}
			else
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_TUPLE;
				for(unsigned i = 0; i < m_VariableNode.size(); i++)
					m_ExpressionInfo.m_VariableName = m_VariableNode;
			}
			
		}
		else if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
		{
			m_ExpressionInfo.m_PatternType = CPN_PATTERN_VARIABLE;
			m_ExpressionInfo.m_VariableName[m_ExpressionsVector[0]->GetParseNodeInfo()->m_CheckedString] = m_ExpressionsVector[0]->GetParseNodeInfo()->m_ColorSet;
			m_ExpressionInfo.m_Multiplicity = 1;
			m_ExpressionInfo.m_MultiSet = false;
		}

		else if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
		{
			m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;			
			m_ExpressionInfo.m_Multiplicity = 1;
			m_ExpressionInfo.m_MultiSet = false;
		}
		else if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
		{
			m_ExpressionInfo.m_PatternType = CPN_PATTERN_CONSTANT;		
			m_ExpressionInfo.m_Multiplicity = 1;
			m_ExpressionInfo.m_MultiSet = false;
		}
		else if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE)
		{
			vector<SP_CPN_ParseNode*> l_vTupleParseNode;
			if( ! TraverseMultiSetTuplePatterns(m_ExpressionsVector[0], l_vTupleParseNode) )
			{
				m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
			}
			else
			{

				bool l_bMultiSetTuplePatterns = true;
				for(unsigned i= 0; i < l_vTupleParseNode.size(); i++)
				{
					m_VariableNode.clear();
					m_ConstantNode.clear();
					m_OtherNode.clear();

					TraversePattern(l_vTupleParseNode[i]);
					if( m_OtherNode.size() > 0 || m_ConstantNode.size() > 0 )
					{
						l_bMultiSetTuplePatterns = false;
						m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
						break;
					}				
				}	
				if(l_bMultiSetTuplePatterns)
				{
					m_ExpressionInfo.m_PatternType = CPN_PATTERN_MULTISETTUPLE;
				}
			}
		}
		else
		{
			m_ExpressionInfo.m_PatternType = CPN_PATTERN_NULL;
		}

		///
		m_ExpressionInfo.m_Expression = m_ExpressionsVector[0]->GetParseNodeInfo()->m_CheckedString;

		if(m_ExpressionInfo.m_PatternType == CPN_PATTERN_NULL)
			TraverseVariables(m_ExpressionsVector[0], m_ExpressionInfo);

	
	return m_ExpressionInfo;
}


bool SP_CPN_Parse_Context::TraverseMultiSetTuplePatterns(SP_CPN_ParseNode* p_pcNode, vector<SP_CPN_ParseNode*> &p_vTupleParseNode)
{
	if(!p_pcNode)
		return false;	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	if( l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE )
	{		
		p_vTupleParseNode.push_back(l_pParseNode);
		return true;
	}
	else if( l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE )
	{
		if(l_pParseNode->GetLeftNode())
		{
			if( ! TraverseMultiSetTuplePatterns(l_pParseNode->GetLeftNode(),p_vTupleParseNode) ) 
				return false;
		}

		if(l_pParseNode->GetRightNode())
		{
			if( !TraverseMultiSetTuplePatterns(l_pParseNode->GetRightNode(),p_vTupleParseNode) )
				return false;
		}
	}
	else
	{
		return false;
	}
	
	return true;
}




void SP_CPN_Parse_Context::SetColorSetName(wxString p_ColorSetName)
{
	m_ColorSetName = p_ColorSetName;
}

void SP_CPN_Parse_Context::SetFunctionFlag(bool p_bFunctionFlag)
{
	m_bFunctionFlag = p_bFunctionFlag;						

}
void SP_CPN_Parse_Context::SetFunctionName(wxString p_sFunctionName)
{
	m_sFunctionName = p_sFunctionName;
}

void SP_CPN_Parse_Context::SetNetClassName(wxString p_sNetClassName)
{
	m_sNetClassName = p_sNetClassName;
}

void SP_CPN_Parse_Context::SetPlaceType(wxString p_sPlaceType)
{
	m_sPlaceType = p_sPlaceType;
}


void SP_CPN_Parse_Context::SetColorSetClass( SP_CPN_ColorSetClass* p_pColorSetClass )
{
	m_pColorSetClass = p_pColorSetClass;
}

void SP_CPN_Parse_Context::SetErrorPosition(wxString p_sErrorPosition) 
{ 
	m_sErrorPosition = p_sErrorPosition;
}




SP_CPN_DATATYPE SP_CPN_Parse_Context::GetHighLevelNodeDataType()
{
	if(m_ExpressionsVector.size() > 0)
	{		
		return m_ExpressionsVector[0]->GetParseNodeInfo()->m_DataType;
	}		
	
	return CPN_NULL;
}

wxString SP_CPN_Parse_Context::GetHighLevelNodeColorSet()
{
    if(m_ExpressionsVector.size() > 0)
	{		
		return m_ExpressionsVector[0]->GetParseNodeInfo()->m_ColorSet;
	}		
	
	return wxT("");
}


bool SP_CPN_Parse_Context::TraverseVariables(SP_CPN_ParseNode* p_Node, SP_CPN_ExpressionPatternInfo & p_PatternInfo)
{	

	if(!p_Node)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_Node;

	// The end condition of recursive traverse

	if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
	{
		p_PatternInfo.m_VariableName[l_pParseNode->GetParseNodeInfo()->m_CheckedString] = l_pParseNode->GetParseNodeInfo()->m_ColorSet;		
		return true;
	}
	// dealing with comma node
	if(l_pParseNode)
	{
		TraverseVariables(l_pParseNode->GetLeftNode(),p_PatternInfo);
		TraverseVariables(l_pParseNode->GetRightNode(),p_PatternInfo);
	}
		
	return true;
}


bool SP_CPN_Parse_Context::TraverseVariablesforUnfolding(SP_CPN_ParseNode* p_Node,vector<SP_CPN_Var_Color> &p_vVariable2Color)
{	

	if(!p_Node)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_Node;

	// The end condition of recursive traverse
	if(l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
	{
		wxString l_sNewVar = l_pParseNode->GetParseNodeInfo()->m_CheckedString;
		SP_CPN_Var_Color l_str_Var_Col;
		l_str_Var_Col.m_sVariable = l_sNewVar;
		l_str_Var_Col.m_sColor = l_pParseNode->GetParseNodeInfo()->m_ColorSet;
//TODO  variable "all" is not allowed ???
		if(l_sNewVar == wxT("all"))
		{
			return true;
		}
		bool l_bFound = false;
		for(unsigned i = 0; i < p_vVariable2Color.size(); i++)
		{
			wxString l_sExistVar = p_vVariable2Color[i].m_sVariable;
			if(l_sNewVar == l_sExistVar)
			{
				l_bFound = true;
			}
		}
		if(!l_bFound)
		{
			bool l_bIsColorSet = false;
			for (auto itV = m_pColorSetClass->GetColorSetVector()->begin(); itV != m_pColorSetClass->GetColorSetVector()->end(); ++itV)
			{
				if ((*itV)->GetName() == l_str_Var_Col.m_sVariable)
				{
					l_bIsColorSet = true;
					break;
				}
			}
			 
			// step for elemOf
			bool l_bIsCS = false;
			for (auto itV = m_pColorSetClass->GetColorSetVector()->begin(); itV != m_pColorSetClass->GetColorSetVector()->end(); ++itV)
			{
				if((*itV)->GetName()== l_str_Var_Col.m_sVariable)
				{
					l_bIsCS = true;
					break;
				}
			}
			if (l_bIsCS)
			{
				SP_CPN_ColorSet* l_pcCS = m_pColorSetClass->LookupColorSet(l_str_Var_Col.m_sVariable);
				if (l_pcCS)
				{
					if (l_pcCS->GetDataType() == CPN_PRODUCT)
					{
						std::vector<wxString> l_vCsNAmes = l_pcCS->GetComponentName();
						if (l_vCsNAmes.size() > 0)
						{
							for (auto v : l_vCsNAmes)
							{
								for (auto itMap = m_pColorSetClass->GetVariableMap()->begin(); itMap != m_pColorSetClass->GetVariableMap()->end(); ++itMap)
								{
									if (itMap->first.Contains(wxT("__AUX_CS")) && itMap->first.Contains(v))
									{
										SP_CPN_Var_Color l_str_Var_Col1;
										l_str_Var_Col1.m_sVariable = itMap->first;
										l_str_Var_Col1.m_sColor = v;

										bool l_bfound1 = false;
										for (unsigned i = 0; i < p_vVariable2Color.size(); i++)
										{
											wxString l_sExistVar = p_vVariable2Color[i].m_sVariable;
											if (itMap->first == l_sExistVar)
											{
												l_bfound1 = true;
											}
										}
										if (!l_bfound1)
										{
											p_vVariable2Color.push_back(l_str_Var_Col1);
										}
									}
								}
							}
							return true;
						}
					}
					else
					{//simple color set
						for (auto itMap = m_pColorSetClass->GetVariableMap()->begin(); itMap != m_pColorSetClass->GetVariableMap()->end(); ++itMap)
						{
							if (itMap->first.Contains(wxT("__AUX_CS")) && itMap->first.Contains(l_pcCS->GetName()))
							{
								SP_CPN_Var_Color l_str_Var_Col1;
								l_str_Var_Col1.m_sVariable = itMap->first;
								l_str_Var_Col1.m_sColor = l_pcCS->GetName();

								bool l_bfound2= false;
								for (unsigned i = 0; i < p_vVariable2Color.size(); i++)
								{
									wxString l_sExistVar = p_vVariable2Color[i].m_sVariable;
									if (itMap->first == l_sExistVar)
									{
										l_bfound2 = true;
									}
								}
								if (!l_bfound2)
								{
									p_vVariable2Color.push_back(l_str_Var_Col1);
								}
							}
						}
						return true;
					}
				}
			}
			 
			if(!l_bIsColorSet)
			p_vVariable2Color.push_back( l_str_Var_Col );
		}
		
		return true;
	}

	// dealing with comma node
	if(l_pParseNode->GetLeftNode())
	{
		TraverseVariablesforUnfolding(l_pParseNode->GetLeftNode(),p_vVariable2Color);		
	}
	if(l_pParseNode->GetRightNode())
	{
		TraverseVariablesforUnfolding(l_pParseNode->GetRightNode(),p_vVariable2Color);
	}
		
	return true;
}


vector<SP_CPN_Var_Color> SP_CPN_Parse_Context::GetVariables()
{	
	vector<SP_CPN_Var_Color> l_vVariabale2Color;
	TraverseVariablesforUnfolding(m_ExpressionsVector[0], l_vVariabale2Color);

	return l_vVariabale2Color;

}

bool SP_CPN_Parse_Context::TraverseGuardPatterns1(SP_CPN_ParseNode* p_pcParseNode, vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns)
{
	SP_CPN_ParseNode* l_pcParseNode = p_pcParseNode;

	if( ! ( l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_AND_NODE || l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_OR_NODE) )
	{
		TraverseGuardPatterns2(l_pcParseNode,p_ExprPatterns);
	}
	else
	{
		while(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_AND_NODE)
		{
			TraverseGuardPatterns2(l_pcParseNode->GetRightNode(),p_ExprPatterns);
			l_pcParseNode = l_pcParseNode->GetLeftNode();
		}

		TraverseGuardPatterns2(l_pcParseNode,p_ExprPatterns);
	}

	return true;
}


bool SP_CPN_Parse_Context::TraverseGuardPatterns2(SP_CPN_ParseNode* p_pcParseNode, vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns)
{
	if( p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_EQUAL_NODE ||
		p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_GREATTHAN_NODE ||
		p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_GREATTHANEQUAL_NODE ||
		p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_LESSTHAN_NODE ||
		p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_LESSTHANEQUAL_NODE	)
	{
		if( (p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE ||
			p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE ||
			p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE) &&
			(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE||
			p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE||
			p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE))
		{

			SP_CPN_ExpressionPatternInfo l_ExprPattern;

			if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_EQUAL_NODE )
			{
				l_ExprPattern.m_sOpetator = wxT("=");
				l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TYPE1;
			}
			if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_GREATTHAN_NODE )
			{
				l_ExprPattern.m_sOpetator = wxT(">");
				l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TEST;
			}
			if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_GREATTHANEQUAL_NODE )
			{
				l_ExprPattern.m_sOpetator = wxT(">=");
				l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TEST;
			}
			if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_LESSTHAN_NODE )
			{
				l_ExprPattern.m_sOpetator = wxT("<");
				l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TEST;
			}
			if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_LESSTHANEQUAL_NODE )
			{
				l_ExprPattern.m_sOpetator = wxT("<=");
				l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TEST;
			}
			
			wxString l_sValue;

			if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
			{
				l_ExprPattern.m_lsExpression = p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_CheckedString;
				l_ExprPattern.m_VariableName[l_ExprPattern.m_lsExpression] = p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_ColorSet;

				if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE || 
					p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
				{
					if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_INTEGER)
						l_sValue = wxString::Format(wxT("%d"),p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_IntegerValue );
					else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_BOOLEAN)
					{
						if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_BooleanValue == false)
							l_sValue = wxT("false");
						else
							l_sValue = wxT("true");
					}
					else
					{
						l_sValue = *(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_StringValue);
					}
					l_ExprPattern.m_rsExpression = l_sValue;
					p_ExprPatterns.push_back( l_ExprPattern );
				}
				else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
				{
					l_ExprPattern.m_rsExpression = p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_CheckedString;
					l_ExprPattern.m_VariableName[l_ExprPattern.m_lsExpression] = p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_ColorSet;
					l_ExprPattern.m_PatternType = CPN_PATTERN_GUARD_TYPE2;
					p_ExprPatterns.push_back( l_ExprPattern );
				}
				else
				{				
				}
			}

			if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE||
				p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_INTEGERCONSTANT_NODE)
			{
				if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_DataType == CPN_INTEGER)
					l_sValue = wxString::Format(wxT("%d"),p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_IntegerValue );
				else if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_DataType == CPN_BOOLEAN)
				{
					if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_BooleanValue == false)
						l_sValue = wxT("false");
					else
						l_sValue = wxT("true");
					}
				else
				{
					l_sValue = *(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_StringValue);
				}
				l_ExprPattern.m_rsExpression = l_sValue;


				if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
				{
					if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_INTEGER)
						l_sValue = wxString::Format(wxT("%d"),p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_IntegerValue );
					else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_BOOLEAN)
					{
						if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_BooleanValue == false)
							l_sValue = wxT("false");
						else
							l_sValue = wxT("true");
					}
					else
					{
						l_sValue = *(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_StringValue);
					}
					l_ExprPattern.m_lsExpression = l_sValue;
					p_ExprPatterns.push_back( l_ExprPattern );
				}
				else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE)
				{
					l_ExprPattern.m_lsExpression = p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_CheckedString;
					l_ExprPattern.m_VariableName[l_ExprPattern.m_lsExpression] = p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_ColorSet;
					p_ExprPatterns.push_back( l_ExprPattern );
				}
				else
				{				
				}
			}
			
		}
		
	}

	return true;
}











/*
void SP_CPN_Parse_Context::TraversePredicateValue(SP_CPN_ParseNode* p_pcParseNode, SP_MapString2String &p_mPredicateValue)
{
	if(p_pcParseNode && p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_EQUAL_NODE)
	{
		if(p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE &&
			p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE)
		{
			wxString l_sValue;
			if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_INTEGER)
				l_sValue = wxString::Format(wxT("%d"),p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_IntegerValue );
			else if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_DataType == CPN_BOOLEAN)
			{
				if(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_BooleanValue == false)
					l_sValue = wxT("false");
				else
					l_sValue = wxT("true");
			}
			else
			{
				l_sValue = *(p_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_StringValue);
			}
			p_mPredicateValue[p_pcParseNode->GetLeftNode()->GetParseNodeInfo()->m_CheckedString] = l_sValue;
		}
		return;
	}
	else
	{
		if(p_pcParseNode && p_pcParseNode->GetLeftNode())
			TraversePredicateValue( p_pcParseNode->GetLeftNode(),p_mPredicateValue);
		if(p_pcParseNode && p_pcParseNode->GetRightNode())
			TraversePredicateValue( p_pcParseNode->GetRightNode(),p_mPredicateValue);
	}
}
*/
/*
void SP_CPN_Parse_Context::TraverseSortedVariables( SP_CPN_ParseNode* p_pcParseNode, map<wxString,map<wxString, SP_CPN_BindingVariables> > &p_mmVariables)
{
	SP_MapString2String l_VariableMap;
	SP_MapString2String l_mPredicateValue;

	bool l_bProduct = false;
	wxString l_sProduct;

	if(p_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_PREDICATE_NODE)
	{
		//find the logic = for the form "variable = value"
		TraversePredicateValue(p_pcParseNode->GetLeftNode(), l_mPredicateValue);

		SP_CPN_ParseNode* l_pcParseNode = p_pcParseNode->GetRightNode();
		if(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_SEPERATOR_NODE)
		{
			if(l_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE )
			{
				l_bProduct = true;
				l_sProduct = l_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_ColorSet;
			}
		}
		else
		{
			if(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE )
			{
				l_bProduct = true;
				l_sProduct = l_pcParseNode->GetParseNodeInfo()->m_CheckColorSet;
			}
		}
		
		TraverseVariablesforUnfolding( p_pcParseNode, l_VariableMap );  // Predicate expression may have extra variables
	}
	else
	{
		SP_CPN_ParseNode* l_pcParseNode = p_pcParseNode;
		if(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_SEPERATOR_NODE)
		{
			if(l_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE )
			{
				l_bProduct = true;
				l_sProduct = l_pcParseNode->GetRightNode()->GetParseNodeInfo()->m_ColorSet;
			}
		}
		else
		{
			if(l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_BRACKET_NODE )
			{
				l_bProduct = true;
				l_sProduct = l_pcParseNode->GetParseNodeInfo()->m_ColorSet;
			}
		}
		
		TraverseVariablesforUnfolding( l_pcParseNode, l_VariableMap );
	}	

	//SP_CPN_BindingVariables
	if(l_VariableMap.size() == 0)
		return;

	wxString l_sExpColorSet;
	if(l_bProduct)
		l_sExpColorSet = l_sProduct;
	else
		l_sExpColorSet = l_VariableMap.begin()->second;

	//

	if( p_mmVariables.find(l_sExpColorSet) != p_mmVariables.end())
	{
		map<wxString, SP_CPN_BindingVariables> l_sBindingVariables;
		l_sBindingVariables = p_mmVariables.find(l_sExpColorSet)->second;
		SP_MapString2String::iterator itMap;
		for( itMap = l_VariableMap.begin(); itMap != l_VariableMap.end(); itMap++)
		{
			(p_mmVariables.find(l_sExpColorSet)->second)[itMap->first].m_sVarColorSet = itMap->second;
		}
	}
	else
	{
		map<wxString, SP_CPN_BindingVariables> l_sBindingVariables;
		SP_MapString2String::iterator itMap;
		for( itMap = l_VariableMap.begin(); itMap != l_VariableMap.end(); itMap++)
		{
			l_sBindingVariables[itMap->first].m_sVarColorSet = itMap->second;
		}

		p_mmVariables[l_sExpColorSet] = l_sBindingVariables;
	}

	SP_MapString2String::iterator itMap;
	for( itMap = l_mPredicateValue.begin(); itMap != l_mPredicateValue.end(); itMap++ )
	{
		if( p_mmVariables[l_sExpColorSet].find( itMap->first ) != p_mmVariables[l_sExpColorSet].end() )
		{
			p_mmVariables[l_sExpColorSet].find( itMap->first )->second.m_sPredicateValue = itMap->second;
		}
	}	

}
*/

/*
map<wxString,map<wxString, SP_CPN_BindingVariables> > SP_CPN_Parse_Context::GetSortVariables()
{
	map<wxString,map<wxString, SP_CPN_BindingVariables> > l_mmCStoVariables;

	if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE)
	{
		SP_CPN_ParseNode* l_pcParseNode = m_ExpressionsVector[0];
		while( l_pcParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE )
		{
			
			TraverseSortedVariables( l_pcParseNode->GetRightNode(),l_mmCStoVariables );
			l_pcParseNode = l_pcParseNode->GetLeftNode();
		}

		TraverseSortedVariables( l_pcParseNode,l_mmCStoVariables );

	}
	else
	{
		TraverseSortedVariables( m_ExpressionsVector[0],l_mmCStoVariables );
	}


	return l_mmCStoVariables;
}
*/


SP_CPN_Parse_Context::~SP_CPN_Parse_Context()
{
	clearExpressions();
}

    /// free all saved expression trees
void SP_CPN_Parse_Context::clearExpressions()
{

	for(unsigned int i = 0; i < m_ExpressionsVector.size(); ++i)
	{
		if(m_ExpressionsVector[i])
			delete m_ExpressionsVector[i];
	}

	m_ExpressionsVector.clear();

 }



bool SP_CPN_Parse_Context::CSPIntTypeTest()
{
	bool l_bIntType = true;

	CSPIntTypeTraverse( m_ExpressionsVector[0],l_bIntType);

	if( ! l_bIntType )
		return false;

	return true;
}


bool SP_CPN_Parse_Context::CSPIntTypeTraverse(SP_CPN_ParseNode* p_pcNode, bool &p_bIntType)
{
	if(!p_pcNode)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	// The end condition of recursive traverse
	if( l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_VARIABLE_NODE ||
	    l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_CONSTANT_NODE ||
		l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_FUNCTION_NODE )
	{	
		if(l_pParseNode->GetParseNodeInfo()->m_DataType != CPN_INTEGER)
		{
			p_bIntType = false;
		}
		return true;
	}

	//
	if(l_pParseNode->GetLeftNode())
	{
		CSPIntTypeTraverse(l_pParseNode->GetLeftNode(),p_bIntType);		
	}
	if(l_pParseNode->GetRightNode())
	{
		CSPIntTypeTraverse(l_pParseNode->GetRightNode(),p_bIntType);
	}
		
	return true;
}

bool SP_CPN_Parse_Context::GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
{
	if(m_ExpressionsVector[0]->GetParseNodeInfo()->m_NodeType != CPN_FUNCTION_NODE)
	{
		if( ! m_ExpressionsVector[0]->GetConstraints(p_cCSPSolver) )
			return false;
		if( ! CSPConstraintTraverse(m_ExpressionsVector[0],p_cCSPSolver) )
		return false;
	}
	else
	{
		SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_ExpressionsVector[0]->GetLeftNode()->GetParseNodeInfo();
		wxString l_sFunctionName   = *(l_LeftNodeInfo->m_StringValue);	
	
		if(m_pColorSetClass->GetFunctionMap()&&
			m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName) != m_pColorSetClass->GetFunctionMap()->end())
		{
			SP_CPN_Function l_FunctionStruct;
			l_FunctionStruct = m_pColorSetClass->GetFunctionMap()->find(l_sFunctionName)->second;
			//SP_CPN_DATATYPE l_DataType = l_FunctionStruct.m_ReturnDataType;
			//l_FunctionStruct.m_pParseContext->SetFunctionFlag(true);
			l_FunctionStruct.m_pParseContext->SetFunctionName(l_FunctionStruct.m_sFunctionName);

			SP_CPN_ParseNode* l_pcParseNode = *(l_FunctionStruct.m_pParseContext->GetExpressionsVector()->begin());

			//ConfigurationColorSets(l_pcParseNode,m_pColorSetClass);

			if( ! l_pcParseNode->GetConstraints(p_cCSPSolver) )
			{
				//l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);
				return false;
			}

			//l_FunctionStruct.m_pParseContext->SetFunctionFlag(false);

			if( ! CSPConstraintTraverse(l_pcParseNode,p_cCSPSolver) )
				return false;
		}		

	}	

	return true;
}

bool SP_CPN_Parse_Context::CSPConstraintTraverse(SP_CPN_ParseNode* p_pcNode, SP_DS_ColCSPSovler& p_cCSPSolver)
{
	if(!p_pcNode)
		return false;
	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	// The end condition of recursive traverse
	if( l_pParseNode->GetParseNodeInfo()->m_NodeType == CPN_AND_NODE )
	{	
		if(l_pParseNode->GetLeftNode())
		{
			p_cCSPSolver.AddConstraint(l_pParseNode->GetLeftNode()->GetParseNodeInfo()->m_BoolConstraintExpr);
		}
		if(l_pParseNode->GetRightNode())
		{
			CSPConstraintTraverse(l_pParseNode->GetRightNode(),p_cCSPSolver);
		}
		return true;
	}
	else
	{
		p_cCSPSolver.AddConstraint(l_pParseNode->GetParseNodeInfo()->m_BoolConstraintExpr);
		return true;
	}	
		
	return true;
}


void SP_CPN_Parse_Context::Configuration(SP_CPN_ParseNode* p_pcNode)
{
	if(!p_pcNode)
		return;	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	l_pParseNode->SetColorSetClass(m_pColorSetClass);
	l_pParseNode->SetColorSetName(m_ColorSetName);
	l_pParseNode->SetErrorPosition(m_sErrorPosition);
	l_pParseNode->SetFunctionFlag(m_bFunctionFlag);
	l_pParseNode->SetFunctionName(m_sFunctionName);
	l_pParseNode->SetColored2UnColoredPlaceNames(m_psvColored2UnColoredPlaceNames);
	//l_pParseNode->SetNetClassName(m_sNetClassName);
	l_pParseNode->SetPlaceType(m_sPlaceType);
	l_pParseNode->SetGraph();//by george 1.9.21
//	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(l_pParseNode->GetLeftNode())
	{
		Configuration(l_pParseNode->GetLeftNode()) ;
		
	}
	if(l_pParseNode->GetRightNode())
	{
		Configuration(l_pParseNode->GetRightNode()) ;			
	}	
}
void SP_CPN_Parse_Context::ConfigurateParser()
{

	if(m_ExpressionsVector.size() > 0)
	{
		Configuration(m_ExpressionsVector[0]);
	}
}


void SP_CPN_Parse_Context::ConfigurationColorSets(SP_CPN_ParseNode* p_pcNode,SP_CPN_ColorSetClass *p_pColorSetClass)
{
	if(!p_pcNode)
		return;	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	l_pParseNode->SetColorSetClass(p_pColorSetClass);
	

	if(l_pParseNode->GetLeftNode())
	{
		ConfigurationColorSets(l_pParseNode->GetLeftNode(),p_pColorSetClass) ;
		
	}
	if(l_pParseNode->GetRightNode())
	{
		ConfigurationColorSets(l_pParseNode->GetRightNode(),p_pColorSetClass) ;			
	}	
}

/*
bool SP_CPN_Parse_Context::DecideVariableNodeType(SP_CPN_ParseNode* p_pcNode)
{
	if(!p_pcNode)
		return false;	
	SP_CPN_ParseNode* l_pParseNode = p_pcNode;

	if(l_pParseNode->GetParseNodeInfo()->m_DataType == CPN_VARIABLE_NODE)
	{
		if( ! dynamic_cast<SP_CPN_Parse_Variable_Node*>(l_pParseNode)->ProcessingVariable() )
			return false;		
	}

	if(l_pParseNode->GetLeftNode())
	{
		if( ! DecideVariableNodeType(l_pParseNode->GetLeftNode()) )
			return false;
		
	}
	if(l_pParseNode->GetRightNode())
	{
		if( ! DecideVariableNodeType(l_pParseNode->GetRightNode()) )
			return false;
	}	

	return true;
}
*/

SP_CPN_Parse_Context_ptr
make_CPN_Parse_Context()
{
	return SP_CPN_Parse_Context_ptr(new SP_CPN_Parse_Context());
}
