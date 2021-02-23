//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored colorsetclass class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_core/SP_Core.h"


SP_CPN_ColorSetClass::SP_CPN_ColorSetClass()
{
	m_mNeighbor12DFunc[wxT("Neighbor1D")] = 0;	
	m_mNeighbor12DFunc[wxT("Neighbor2D")] = 0;	
}

SP_CPN_ColorSetClass::~SP_CPN_ColorSetClass()
{
	
}	

SP_CPN_ColorSet* SP_CPN_ColorSetClass::LookupColorSet(wxString p_ColosetName)
{
	vector<SP_CPN_ColorSet*>::iterator it;
	for(it = m_pColorSetVector->begin(); it != m_pColorSetVector->end(); it++)
	{
		if( (*it)->GetName() == p_ColosetName )
		{
			return *it;
		}
	}

	SP_LOGERROR( wxT("\n")+ p_ColosetName + wxT(": Colorset is not found") );
	wxASSERT_MSG(false, p_ColosetName + wxT(": Colorset is not found"));
	return NULL;
}
bool SP_CPN_ColorSetClass::SetVariableValue(wxString p_VariableName, unsigned int p_nVarVal)
{
	map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	itMap = m_pVariableMap->find(p_VariableName);
	if(itMap != m_pVariableMap->end())
	{	
		itMap->second.m_IntegerValue = p_nVarVal;
		itMap->second.m_DataType = CPN_INTEGER;
	}
	else
	{
		wxString l_sError;					
		l_sError << wxT("Variable not found");		
		SP_LOGERROR( wxT("\n Variable not found: Variables binding checking") );	
		return false;
	}
	return true;
}

bool SP_CPN_ColorSetClass::SetVariableValue(wxString p_VariableName, wxString p_VariableValue)
{
	map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	itMap = m_pVariableMap->find(p_VariableName);

	/**********by george for elemOf**********/
	if (itMap == m_pVariableMap->end() && LookupColorSet(p_VariableName) != NULL)
	{
		return true;
	}

	if(itMap != m_pVariableMap->end())
	{	
		SP_CPN_ColorSet* l_pcColorSet = LookupColorSet(itMap->second.m_ColorSet);

		if( ! l_pcColorSet )
			return false;
		
		if(l_pcColorSet->GetDataType() == CPN_UNION)  //Note that it is different from others'
		{
			SP_CPN_DATATYPE l_DataType = l_pcColorSet->GetUnionDataType(p_VariableValue);

			if(l_DataType == CPN_INTEGER)
			{
				long l_nValue;
				if(p_VariableValue.ToLong(&l_nValue))
				{
					itMap->second.m_IntegerValue = l_nValue;
					itMap->second.m_DataType = CPN_INTEGER;
				}
				else
				{
					wxString l_sError;					
					l_sError << wxT("Variable not found");		
					SP_LOGERROR( wxT("\n Variable not found: Variables binding checking") );		
					return false;
				}
			}
			else if(l_DataType == CPN_BOOLEAN)
			{
				if(p_VariableValue == wxT("false"))
					itMap->second.m_BooeanValue = false;
				else
					itMap->second.m_BooeanValue = true;

				itMap->second.m_DataType = CPN_BOOLEAN;
			}
			else
			{
				itMap->second.m_StringValue = p_VariableValue;	
				itMap->second.m_DataType = CPN_STRING;
			}
		}
		

		else if(itMap->second.m_DataType == CPN_INTEGER)
		{
			long l_nValue;
			if(p_VariableValue.ToLong(&l_nValue))
			{
				itMap->second.m_IntegerValue = l_nValue;
			}
			else
			{
				//does not make sense to give here an error message, by george
				//wxString l_sError;
				//l_sError << wxT("Color error");
				//SP_MESSAGEBOX(l_sError, wxT("Variables binding checking"), wxOK | wxICON_ERROR);
				return false;
			}
		}
		
		else if(itMap->second.m_DataType == CPN_BOOLEAN)
		{			
			if(p_VariableValue == wxT("false"))
				itMap->second.m_BooeanValue = false;
			else
				itMap->second.m_BooeanValue = true;
		}
		else
			itMap->second.m_StringValue = p_VariableValue;
	}
	else
	{
		wxString l_sError;					
		l_sError << wxT("Variable not found");
		SP_MESSAGEBOX(l_sError, wxT("Variables binding checking"), wxOK | wxICON_ERROR);					
		return false;
	}
	return true;
}




void SP_CPN_ColorSetClass::SetFunctionVariableValue(wxString p_sFunctionName, wxString p_VariableName, wxString p_VariableValue)
{
	
	map<wxString,SP_CPN_Parameter>* l_pParameterMap;

	map<wxString, SP_CPN_Function>::iterator itFunctionMap;
	itFunctionMap = m_pFunctionMap->find(p_sFunctionName);

	if( itFunctionMap != m_pFunctionMap->end())
	{
		l_pParameterMap = &(itFunctionMap->second.m_ParameterMap);
	}

	map<wxString,SP_CPN_Parameter>::iterator itMap;
	itMap = l_pParameterMap->find(p_VariableName);
	if(itMap != l_pParameterMap->end())
	{	
		SP_CPN_ColorSet* l_pcColorSet = LookupColorSet(itMap->second.m_sParaColorSet);

		if(l_pcColorSet->GetDataType() == CPN_UNION)  //Note that it is different from others'
		{
			SP_CPN_DATATYPE l_DataType = l_pcColorSet->GetUnionDataType(p_VariableValue);

			if(l_DataType == CPN_INTEGER)
			{
				long l_nValue;
				if(p_VariableValue.ToLong(&l_nValue))
				{
					itMap->second.m_IntegerValue = l_nValue;
					itMap->second.m_DataType = CPN_INTEGER;
				}
			}
			else if(l_DataType == CPN_BOOLEAN)
			{
				if(p_VariableValue == wxT("false"))
					itMap->second.m_BooeanValue = false;
				else
					itMap->second.m_BooeanValue = true;
				itMap->second.m_DataType = CPN_BOOLEAN;
			}
			else
			{
				itMap->second.m_StringValue = p_VariableValue;	
				itMap->second.m_DataType = CPN_STRING;
			}
		}
		

		else if(itMap->second.m_DataType == CPN_INTEGER)
		{
			long l_nValue;
			if(p_VariableValue.ToLong(&l_nValue))
				itMap->second.m_IntegerValue = l_nValue;
		}
		
		else if(itMap->second.m_DataType == CPN_BOOLEAN)
		{			
			if(p_VariableValue == wxT("false"))
				itMap->second.m_BooeanValue = false;
			else
				itMap->second.m_BooeanValue = true;
		}
		else
			itMap->second.m_StringValue = p_VariableValue;
	}
}


void SP_CPN_ColorSetClass::SetParameterValues(wxString p_sFunctionName,wxString p_sArgumentList)
{

	map<wxString,SP_CPN_Parameter>* l_pParameterMap;

	map<wxString, SP_CPN_Function>::iterator itFunctionMap;
	itFunctionMap = m_pFunctionMap->find(p_sFunctionName);

	if( itFunctionMap != m_pFunctionMap->end())
	{
		l_pParameterMap = &(itFunctionMap->second.m_ParameterMap);
	}

	vector<wxString> l_vArgument;
	wxString l_sArgument;

	while(p_sArgumentList.Freq(wxT(',')))
	{
		l_sArgument = p_sArgumentList.BeforeFirst(wxT(','));
		l_vArgument.push_back(l_sArgument);
		p_sArgumentList = p_sArgumentList.AfterFirst(wxT(','));
	}
	l_vArgument.push_back(p_sArgumentList);


	map<wxString,SP_CPN_Parameter>::iterator itMap;
	
	for(itMap = l_pParameterMap->begin(); itMap != l_pParameterMap->end(); itMap++ )
	{	
		//SP_CPN_ColorSet* l_pcColorSet = LookupColorSet(itMap->second.m_sParaColorSet);

		wxString l_sVariableValue;
		l_sVariableValue = l_vArgument[itMap->second.m_nPosition];
		
		if(itMap->second.m_DataType == CPN_INTEGER)
		{
			long l_nValue;
			if(l_sVariableValue.ToLong(&l_nValue))
				itMap->second.m_IntegerValue = l_nValue;
		}
		
		else if(itMap->second.m_DataType == CPN_BOOLEAN)
		{			
			if(l_sVariableValue == wxT("false"))
				itMap->second.m_BooeanValue = false;
			else
				itMap->second.m_BooeanValue = true;
		}
		else
			itMap->second.m_StringValue = l_sVariableValue;
	}
}

bool SP_CPN_ColorSetClass::CheckParaTypeMatch(wxString p_sFunctionName,wxString p_sArgumentList, wxString p_sCSList)
{
	map<wxString,SP_CPN_Parameter>* l_pParameterMap;

	map<wxString, SP_CPN_Function>::iterator itFunctionMap;
	itFunctionMap = m_pFunctionMap->find(p_sFunctionName);
	if( itFunctionMap == m_pFunctionMap->end())
		return false;	
	l_pParameterMap = &(itFunctionMap->second.m_ParameterMap);

	//variable name
	vector<wxString> l_vArgument;
	wxString l_sArgument;
	while(p_sArgumentList.Freq(wxT(',')))
	{
		l_sArgument = p_sArgumentList.BeforeFirst(wxT(','));
		l_vArgument.push_back(l_sArgument);
		p_sArgumentList = p_sArgumentList.AfterFirst(wxT(','));
	}
	l_vArgument.push_back(p_sArgumentList);


	//color set
	vector<wxString> l_vCSList;
	wxString l_sCheckCS;
	while(p_sCSList.Freq(wxT(',')))
	{
		l_sCheckCS = p_sCSList.BeforeFirst(wxT(','));
		l_vCSList.push_back(l_sCheckCS);
		p_sCSList = p_sCSList.AfterFirst(wxT(','));
	}
	l_vCSList.push_back(p_sCSList);


	map<wxString,SP_CPN_Parameter>::iterator itMap;	
	for(itMap = l_pParameterMap->begin(); itMap != l_pParameterMap->end(); itMap++ )
	{	
		wxString l_sRealCS  = itMap->second.m_sParaColorSet;
		SP_CPN_DATATYPE l_RealType = itMap->second.m_DataType;

		wxString l_sCheckCS = l_vCSList[itMap->second.m_nPosition];

		wxString l_sVariable = l_vArgument[itMap->second.m_nPosition];
		itMap->second.m_sRealVar = l_sVariable;   //map parameter to its real parameter (variable)

		if( l_sCheckCS.Find(wxT("#")) != wxNOT_FOUND )
		{
			l_sCheckCS = l_sCheckCS.AfterFirst(wxT('#'));
			if( l_sCheckCS != l_sRealCS)
			{
				SP_LOGERROR(wxT("Parameters of user defined function error. Error position: ")+ p_sFunctionName);
				return false;
			}
		}
		else
		{			
			wxString l_sRealDataType;
			l_sRealDataType << l_RealType; 
			if( l_sRealDataType != l_sCheckCS)
			{
				SP_CPN_ColorSet* l_pcColorSet = LookupColorSet(l_sCheckCS);
				if(l_pcColorSet && l_pcColorSet->GetDataType() == l_RealType ) 
				{}
				else
				{
					SP_LOGERROR(wxT("Parameters of user defined function error. Error position: ")+ p_sFunctionName);
					return false;
				}
			}
		}
	}

	return true;
}



bool SP_CPN_ColorSetClass::SetNeighbor12DFuncValue(int p_nFuncValue)
{	
	map<wxString, int>::iterator itMap;

	for(itMap = m_mNeighbor12DFunc.begin(); itMap != m_mNeighbor12DFunc.end(); itMap++)
	{
		itMap->second = p_nFuncValue;		
	}

/*	
	itMap = m_mNeighbor12DFunc.find(p_sFuncName);
	if(itMap != m_mNeighbor12DFunc.end())
	{	
		itMap->second = p_nFuncValue;		
	}
	else
	{
		return false;
	}
*/
	return true;
}
