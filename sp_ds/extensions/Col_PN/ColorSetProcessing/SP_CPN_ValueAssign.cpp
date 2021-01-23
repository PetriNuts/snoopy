//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored valueassign class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include <algorithm>
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
SP_CPN_ValueAssign::SP_CPN_ValueAssign()
{
	m_ColorSetVector.clear();
	m_VariableMap.clear();
	m_ConstantMap.clear();

}
SP_CPN_ValueAssign::~SP_CPN_ValueAssign()
{
	// Delete all Vector or map
	for(unsigned i=0; i < m_ColorSetVector.size(); i++)
	{		
		wxDELETE( m_ColorSetVector[i] );
	}
}	


bool SP_CPN_ValueAssign::InitializeColorset(SP_CPN_ColorSetClass &p_pColorSetClass,bool p_bDeclarations)
{
	if(p_bDeclarations)
	{
		if( ! CollectAllDeclarations() )
			return false;
	}

	m_pcColorSetClass = &p_pColorSetClass;
	p_pColorSetClass.SetColorSetVector(&m_ColorSetVector);
	p_pColorSetClass.SetVariableMap(&m_VariableMap);
	p_pColorSetClass.SetConstantMap(&m_ConstantMap);
	p_pColorSetClass.SetFunctionMap(&m_FunctionMap);

	if( !InitializeBasicTypeConstant() )
		return false;

	if( !InitializeBasicColorset() )
		return false;

	if( !InitializeStructuredColorset() )
		return false;

	m_VariableMap.clear();
	if( !InitializeVariable() )
		return false;

//	m_ConstantMap.clear();
	if( !InitializeConstant() )
		return false;

	if( !InitializeFunction(p_pColorSetClass))
		return false;

	if( !InitializeFunctionParseTree(p_pColorSetClass))
		return false;

	return true;
}


bool SP_CPN_ValueAssign::InitializeBasicColorset()
{
	vector<unsigned> l_vInt;
	vector<bool> l_vBool;
	vector<wxString> l_vString;
	wxString l_sDataType;	


	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("ColorsetList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{
		l_vInt.clear();
		l_vString.clear();
		l_vBool.clear();
		
		m_pColorSet = new SP_CPN_ColorSet; 
		wxString l_sColorsetName = (*l_pvDeclarations)[i].m_sName;
		
		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_sColorsetName,l_sError) )
		{
			l_sError += wxT("in the colorset definition");
			SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_sColorsetName.Replace(wxT(" "), wxT(""));
		l_sColorsetName.Replace(wxT("\n"), wxT(""));
		l_sColorsetName.Replace(wxT("\t"), wxT(""));

		m_pColorSet->SetName(l_sColorsetName);

		l_sDataType = (*l_pvDeclarations)[i].m_sType;

		wxString l_sParseString;
		l_sParseString = (*l_pvDeclarations)[i].m_sColors;

		if(l_sDataType == wxT("dot"))
		{
			if( l_sColorsetName != wxT("Dot") )
			{
				wxString l_sError = wxT("The colorset name for type dot should be Dot in the colorset definition.");
				SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
				SP_LOGERROR(l_sError);
				return false;
			}

			m_pColorSet->SetDataType( CPN_DOT );
			
			l_sParseString.Replace(wxT(" "), wxT(""));
			l_sParseString.Replace(wxT("\n"), wxT(""));
			l_sParseString.Replace(wxT("\t"), wxT(""));

			if( l_sParseString != wxT("dot") )
			{
				wxString l_sError = wxT("The color name for type dot should be dot in the colorset definition.");
				SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
				SP_LOGERROR(l_sError);
				return false;
			}

			l_vString.push_back(wxT("dot"));
			m_pColorSet->SetStringValue( l_vString );
		}

		else if(l_sDataType == wxT("int"))
		{
			if( l_sParseString == wxT("") )
			{
				SP_LOGERROR( wxT("The colors should not be empty."));
				return false;
			}
			m_pColorSet->SetDataType( CPN_INTEGER );

			bool l_bIntSuccessive;
			m_cColorProcessing.SetColorSetClassFlag(true);
			m_cColorProcessing.SetColorSetClass(m_pcColorSetClass);

			if( ! m_cColorProcessing.ProcessingInt(l_sColorsetName,l_sParseString,l_vInt,l_bIntSuccessive) )
			{
				return false;
			}
			
			for(unsigned j = 0; j < l_vInt.size(); j++)
			{
				wxString l_sColor = wxString::Format(wxT("%u"),l_vInt[j]);
				l_vString.push_back(l_sColor);
			}
			m_pColorSet->SetStringValue( l_vString );	

			m_pColorSet->SetIntSuccessiveFlag(l_bIntSuccessive);
		}
		
		else if(l_sDataType == wxT("string"))
		{
			if( l_sParseString == wxT("") )
			{
				SP_LOGERROR( wxT("The colors should not be empty."));
				return false;
			}

			m_pColorSet->SetDataType( CPN_STRING );

			if( ! m_cColorProcessing.ProcessingString(l_sColorsetName,l_sParseString,l_vString) )	
			{
				return false;
			}

			// Add double quotes
			for(unsigned j = 0; j < l_vString.size(); j++ )
			{
				l_vString[j] = wxT("\"") + l_vString[j] + wxT("\"");
			}
			m_pColorSet->SetStringValue( l_vString );			
		}

		else if(l_sDataType == wxT("enum"))
		{
			if( l_sParseString == wxT("") )
			{
				SP_LOGERROR( wxT("The colors should not be empty."));
				return false;
			}

			m_pColorSet->SetDataType( CPN_ENUM );
					
			if( ! m_cColorProcessing.ProcessingString(l_sColorsetName,l_sParseString,l_vString) )		
			{
				return false;
			}
			for(unsigned j = 0; j < l_vString.size(); j++ )
			{
				wxChar l_cFirst = l_vString[j][0];
				if( ! (::isalpha(l_cFirst)|| l_cFirst == '_') )
				{
					wxString l_sError = wxT("The color with the enum type should be an identifier.");
					SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
					SP_LOGERROR(l_sError);
					return false;
				}
			}

			m_pColorSet->SetStringValue( l_vString );			
		}

		else if(l_sDataType == wxT("bool"))
		{
			m_pColorSet->SetDataType( CPN_BOOLEAN );
			if( ! m_cColorProcessing.ProcessingBool(l_sParseString,l_vString) )	
				return false;
			m_pColorSet->SetStringValue( l_vString );
		}
		
		else if(l_sDataType == wxT("index"))
		{
			if( l_sParseString == wxT("") )
			{
				SP_LOGERROR( wxT("The colors should not be empty."));
				return false;
			}

			m_pColorSet->SetDataType( CPN_INDEX );
			wxString l_sName;
			if( ! m_cColorProcessing.ProcessingIndex(l_sColorsetName,l_sParseString,l_sName, l_vInt) )
			{
				return false;
			}

			m_pColorSet->SetIndexName( l_sName );			
			for(unsigned i = 0; i < l_vInt.size(); i++)
			{
				wxString l_sString = l_sName +  wxT("[") + wxString::Format(wxT("%u"),l_vInt[i]) +  wxT("]");
				l_vString.push_back(l_sString); 				
			}	
			m_pColorSet->SetStringValue( l_vString );			
		}

		else if(l_sDataType == wxT("----Basic types----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
			return false;
		}

		else if(l_sDataType == wxT("----User-defined colorsets----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else
		{
			//Deal with subset
			vector<wxString> l_vColors;
			vector<wxString> l_vRealColor;
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			if( ! l_cSyntaxChecking.GenerateSubSetColorSet(l_sColorsetName,l_sParseString,l_sDataType,m_pcColorSetClass,l_vColors))
				return false;

			if (!l_cSyntaxChecking.GenerateSubSetColorSet(l_sColorsetName, l_sParseString, l_sDataType, m_pcColorSetClass, l_vRealColor,true))//by george
				return false;

			SP_CPN_ColorSet* l_pcColorSet = m_pcColorSetClass->LookupColorSet( l_sDataType );
			if( ! l_pcColorSet )
						return false;

			SP_CPN_DATATYPE l_eDataType = l_pcColorSet->GetDataType();
			m_pColorSet->SetDataType( l_eDataType );
			m_pColorSet->SetStringValue( l_vColors );
			m_pColorSet->SetRealColor(l_vRealColor);//by george,for elemOf operator

			m_pColorSet->SetFatherSet( l_sDataType );
			l_pcColorSet->SetSubSet( l_sColorsetName );
		}		

		m_ColorSetVector.push_back(m_pColorSet); // Add the colorset	

		//deal with variable
		if( ! Incremental_Init_Variable(m_pColorSet) )
			return false;
	}
	
	return true;
}

bool SP_CPN_ValueAssign::InitializeStructuredColorset()
{
	vector<int> l_vInt;
	vector<wxString> l_vString;


	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("StructuredColorsetList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{		
		m_pColorSet = new SP_CPN_ColorSet; 

		map<wxString, SP_CPN_ColorSet*> l_ComponentMap;
		vector<wxString> l_ComponentName;

		wxString l_sError;
		wxString l_sColorsetName = (*l_pvDeclarations)[i].m_sName;

		if ( ! m_cColorProcessing.ProcessingIdentifier(l_sColorsetName,l_sError) )
		{
			l_sError += wxT(" in the colorset definition");
			SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_sColorsetName.Replace(wxT(" "), wxT(""));
		l_sColorsetName.Replace(wxT("\n"), wxT(""));
		l_sColorsetName.Replace(wxT("\t"), wxT(""));

		m_pColorSet->SetName(l_sColorsetName);		
		
		wxString l_sDataType = (*l_pvDeclarations)[i].m_sType;

		wxString l_sParseString;
		l_sParseString = (*l_pvDeclarations)[i].m_sCompound_Colorsets;

		m_cColorProcessing.ProcessingComponent(l_sColorsetName,l_sParseString,l_vString);	

		if(l_sDataType == wxT("product") || l_sDataType == wxT("union") )
		{
			for(unsigned i = 0; i < l_vString.size(); i++)
			{
				//
				bool l_bFindFlag = false;
				for(unsigned k = 0; k < m_ColorSetVector.size(); k++ )
				{
					if(m_ColorSetVector[k]->GetName() == l_vString[i])
					{
						l_ComponentMap[l_vString[i]] = m_ColorSetVector[k];
						l_bFindFlag = true;
						break;				
					}				
				}

				if( ! l_bFindFlag)
				{
					
					wxString l_sError =  l_vString[i] + wxT(" is not found in defined colorset.");
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;
				}
				l_ComponentName.push_back(l_vString[i]);
			}
		
			m_pColorSet->SetComponentMap(l_ComponentMap);
			m_pColorSet->SetComponentName(l_ComponentName);  	
			
			// Generate Product values
			if(l_sDataType == wxT("product"))
			{
				m_pColorSet->SetDataType( CPN_PRODUCT );
				if( ! m_pColorSet->GenerateProduct() ) 
					return false;
			}		

			if(l_sDataType == wxT("union"))
			{
				m_pColorSet->SetDataType( CPN_UNION );
				m_pColorSet->GenerateUnionValues();
			}
		}
		
		else if(l_sDataType == wxT("----Structured types----") )
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if(l_sDataType == wxT("----User-defined colorsets----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Color set definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else
		{
			//Deal with subset
			vector<wxString> l_vColors;
			vector<wxString> l_vRealColor;
			SP_CPN_SyntaxChecking l_cSyntaxChecking;
			
			if( ! l_cSyntaxChecking.GenerateSubSetColorSet(l_sColorsetName,l_sParseString,l_sDataType,m_pcColorSetClass,l_vColors))
				return false;

			if( ! l_cSyntaxChecking.GenerateSubSetColorSet(l_sColorsetName,l_sParseString,l_sDataType,m_pcColorSetClass,l_vRealColor,true))
							return false;

			wxString l_serror = l_sColorsetName;
			SP_CPN_ColorSet* l_pcColorSet = m_pcColorSetClass->LookupColorSet( l_sDataType );	

			if(!l_pcColorSet)
			{				
				return false;
			}

			
			SP_CPN_DATATYPE l_eDataType = l_pcColorSet->GetDataType();		
			m_pColorSet->SetDataType( l_eDataType );		
			m_pColorSet->SetStringValue( l_vColors );
			m_pColorSet->SetRealColor(l_vRealColor);//by george

			m_pColorSet->SetFatherSet( l_sDataType );		
			l_pcColorSet->SetSubSet( l_sColorsetName );		
			m_pColorSet->SetComponentName( l_pcColorSet->GetComponentName());		
			m_pColorSet->SetComponentMap( l_pcColorSet->GetComponentMap() );		

		}	

		m_ColorSetVector.push_back(m_pColorSet);

		if( ! Incremental_Init_Variable(m_pColorSet) )
		{			
			return false;	
		}
		
	}

	
	return true;
}


bool SP_CPN_ValueAssign::Incremental_Init_Variable(SP_CPN_ColorSet* p_pcColorSet)
{


	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("VariableList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{		
		wxString l_sColorsetName= (*l_pvDeclarations)[i].m_sType;

		if( l_sColorsetName != p_pcColorSet->GetName() )
		{
			continue; //only look up the variable defined on the current color set
		}

		SP_CPN_Variable_Constant l_variableStruct;
		l_variableStruct.m_DataType = p_pcColorSet->GetDataType();			

		wxString l_variableName = (*l_pvDeclarations)[i].m_sName;
		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_variableName,l_sError) )
		{
			l_sError += wxT("in the variable definition");
			SP_MESSAGEBOX(l_sError, wxT("Variable definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_variableName.Replace(wxT(" "), wxT(""));
		l_variableName.Replace(wxT("\n"), wxT(""));
		l_variableName.Replace(wxT("\t"), wxT(""));

		l_variableStruct.m_ColorSet = l_sColorsetName;			
		m_VariableMap[l_variableName] = l_variableStruct;
	}

	return true;
}

bool SP_CPN_ValueAssign::Incremental_Init_Constant(SP_CPN_ColorSet* p_pcColorSet)
{
	vector<int> l_vInt;
	vector<wxString> l_vString;


	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("ConstantList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{		
		wxString l_sColorsetName= (*l_pvDeclarations)[i].m_sType;
		if( l_sColorsetName != p_pcColorSet->GetName() )
		{
			continue; //only look up the constant defined on the current color set
		}
		
		SP_CPN_Variable_Constant l_ConstantStruct;
		l_ConstantStruct.m_ColorSet = l_sColorsetName;

		long l_nValue;
		wxString l_ConstantName = (*l_pvDeclarations)[i].m_sName;

		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_ConstantName,l_sError) )
		{
			l_sError += wxT("in the constant definition");
			SP_MESSAGEBOX(l_sError, wxT("Constant definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_ConstantName.Replace(wxT(" "), wxT(""));
		l_ConstantName.Replace(wxT("\n"), wxT(""));
		l_ConstantName.Replace(wxT("\t"), wxT(""));

		wxString l_sConstantValue = (*l_pvDeclarations)[i].m_sConstantvalue;
		if(l_ConstantStruct.m_DataType == CPN_INTEGER)
		{
			if(l_sConstantValue.ToLong(&l_nValue))
				l_ConstantStruct.m_IntegerValue = l_nValue;
			else
			{
	
				wxString l_sError;
				l_sError << l_sConstantValue <<  wxT(" is not integer in the constant definition.") ;
				SP_MESSAGEBOX(l_sError, wxT("Constant definition checking"), wxOK | wxICON_ERROR);	
			
				SP_LOGERROR(l_sError);
				return false;
			}
		}

		if(l_ConstantStruct.m_DataType == CPN_BOOLEAN)
		{
			if(l_sConstantValue == wxT("false"))
				l_ConstantStruct.m_BooeanValue = false;
			else
				l_ConstantStruct.m_BooeanValue = true;
		}

		if(l_ConstantStruct.m_DataType == CPN_STRING||
			l_ConstantStruct.m_DataType == CPN_ENUM||
			l_ConstantStruct.m_DataType == CPN_INDEX||
			l_ConstantStruct.m_DataType == CPN_PRODUCT)
		{
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		
		m_ConstantMap[l_ConstantName] = l_ConstantStruct;

	}
	
	return true;

}

bool SP_CPN_ValueAssign::InitializeVariable()
{
	vector<int> l_vInt;
	vector<wxString> l_vString;
	l_vInt.clear();
	l_vString.clear();

	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("VariableList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{		
		
		SP_CPN_Variable_Constant l_variableStruct;

		wxString l_variableName = (*l_pvDeclarations)[i].m_sName;
		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_variableName,l_sError) )
		{
			l_sError += wxT("in the variable definition");
			SP_MESSAGEBOX(l_sError, wxT("Variable definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_variableName.Replace(wxT(" "), wxT(""));
		l_variableName.Replace(wxT("\n"), wxT(""));
		l_variableName.Replace(wxT("\t"), wxT(""));


		wxString l_sColorsetName= (*l_pvDeclarations)[i].m_sType;

		if(l_sColorsetName == wxT("int"))
			l_variableStruct.m_DataType = CPN_INTEGER;
		else if(l_sColorsetName == wxT("string"))
			l_variableStruct.m_DataType = CPN_STRING;
		else if(l_sColorsetName == wxT("bool"))
			l_variableStruct.m_DataType = CPN_BOOLEAN;
		else if(l_sColorsetName == wxT("enum"))
			l_variableStruct.m_DataType = CPN_ENUM;
		else if(l_sColorsetName == wxT("index"))
			l_variableStruct.m_DataType = CPN_INDEX;
		else if(l_sColorsetName == wxT("product"))
			l_variableStruct.m_DataType = CPN_PRODUCT;

		else if(l_sColorsetName == wxT("----Basic types----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Variable definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if(l_sColorsetName == wxT("----Structured types----") )
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Variable definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if(l_sColorsetName == wxT("----User-defined colorsets----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Variable definition checking"), wxOK | wxICON_ERROR);
			return false;
		}

		else
		{
			bool l_bFound = false;
			for(unsigned j = 0; j < m_ColorSetVector.size(); j++)
			{			
				if(m_ColorSetVector[j]->GetName() == l_sColorsetName )			  
				{
					l_variableStruct.m_DataType = m_ColorSetVector[j]->GetDataType();
					l_bFound = true;
					break;
				}	
				else
				{	
					
					for(unsigned k = 0; k < m_ColorSetVector[j]->GetAlias().size(); k++)
					{
						if(m_ColorSetVector[j]->GetAlias()[k] == l_sColorsetName )
						{
							l_variableStruct.m_DataType = m_ColorSetVector[j]->GetDataType();
							l_bFound = true;
							break;
						}
					}
				}
			}

			if(!l_bFound)
			{
				wxString l_sError;
				l_sError << wxT("Colorset: ") << l_sColorsetName << wxT(" is not defined in the variable definition.") ;
				SP_MESSAGEBOX(l_sError, wxT("Variable definition checking"), wxOK | wxICON_ERROR);	

			
				SP_LOGERROR(l_sError);
				return false;
			}

		}
		l_variableStruct.m_ColorSet = l_sColorsetName;		
		
		m_VariableMap[l_variableName] = l_variableStruct;

	}

	return true;

}


bool SP_CPN_ValueAssign::InitializeConstant()
{
	vector<int> l_vInt;
	vector<wxString> l_vString;
	l_vInt.clear();
	l_vString.clear();


	
	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("ConstantList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{		
		
		SP_CPN_Variable_Constant l_ConstantStruct;
		wxString l_ConstantName = (*l_pvDeclarations)[i].m_sName;

		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_ConstantName,l_sError) )
		{
			l_sError += wxT("in the constant definition");
			SP_MESSAGEBOX(l_sError, wxT("Constant definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}

		l_ConstantName.Replace(wxT(" "), wxT(""));
		l_ConstantName.Replace(wxT("\n"), wxT(""));
		l_ConstantName.Replace(wxT("\t"), wxT(""));

		wxString l_ConstantType = (*l_pvDeclarations)[i].m_sType;
		
		if(l_ConstantType == wxT("int"))
			l_ConstantStruct.m_DataType = CPN_INTEGER;
		else if(l_ConstantType == wxT("string"))
			l_ConstantStruct.m_DataType = CPN_STRING;
		else if(l_ConstantType == wxT("bool"))
			l_ConstantStruct.m_DataType = CPN_BOOLEAN;
		else if(l_ConstantType == wxT("enum"))
			l_ConstantStruct.m_DataType = CPN_ENUM;
		else if(l_ConstantType == wxT("index"))
			l_ConstantStruct.m_DataType = CPN_INDEX;
		else if(l_ConstantType == wxT("product"))
			l_ConstantStruct.m_DataType = CPN_PRODUCT;
		else if(l_ConstantType == wxT("double"))
			l_ConstantStruct.m_DataType = CPN_DOUBLE;//by george for cont. pns

		else if(l_ConstantType == wxT("----Basic types----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Constant definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if(l_ConstantType == wxT("----Structured types----") )
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Constant definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if(l_ConstantType == wxT("----User-defined colorsets----"))
		{
			SP_MESSAGEBOX(wxT("Please select data type"), wxT("Constant definition checking"), wxOK | wxICON_ERROR);
			return false;
		}
		else if (l_ConstantType == wxT("double") || l_ConstantType == wxT("TFN"))
		{
			continue;
		}
		else
		{
			
			bool l_bFound = false;
			for(unsigned j = 0; j < m_ColorSetVector.size(); j++)
			{
				if(m_ColorSetVector[j]->GetName() == l_ConstantType )			  
				{
					l_ConstantStruct.m_DataType = m_ColorSetVector[j]->GetDataType();
					l_bFound = true;
					break;
				}
				else
				{					
					for(unsigned k = 0; k < m_ColorSetVector[j]->GetAlias().size(); k++)
					{
						if(m_ColorSetVector[j]->GetAlias()[k] == l_ConstantType )
						{
							l_ConstantStruct.m_DataType = m_ColorSetVector[j]->GetDataType();
							l_bFound = true;
							break;
						}
					}
				}

			}

			if(!l_bFound)
			{
				wxString l_sError;
				l_sError << wxT("Colorset: ") << l_ConstantType << wxT(" is not defined in the constant definition.") ;
				SP_MESSAGEBOX(l_sError, wxT("Constant definition checking"), wxOK | wxICON_ERROR);	
			
				SP_LOGERROR(l_sError);
				return false;
			}

			l_ConstantStruct.m_ColorSet = l_ConstantType;
		}		

		wxString l_sConstantValue = (*l_pvDeclarations)[i].m_sConstantvalue;
		if(l_ConstantStruct.m_DataType == CPN_INTEGER)
		{
/*			if(l_sConstantValue.ToLong(&l_nValue))
				l_ConstantStruct.m_IntegerValue = l_nValue;
			else
			{
	
				wxString l_sError;
				l_sError << l_sConstantValue <<  wxT(" is not integer in the constant definition.") ;
				SP_MESSAGEBOX(l_sError, wxT("Constant definition checking"), wxOK | wxICON_ERROR);	
			
				SP_LOGERROR(l_sError);
				return false;
			}
*/		}

		if(l_ConstantStruct.m_DataType == CPN_BOOLEAN)
		{
			if(l_sConstantValue == wxT("false"))
				l_ConstantStruct.m_BooeanValue = false;
			else
				l_ConstantStruct.m_BooeanValue = true;
		}

		if(l_ConstantStruct.m_DataType == CPN_STRING||
			l_ConstantStruct.m_DataType == CPN_ENUM||
			l_ConstantStruct.m_DataType == CPN_INDEX||
			l_ConstantStruct.m_DataType == CPN_PRODUCT)
		{
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		
		//m_ConstantMap[l_ConstantName] = l_ConstantStruct;

	}
	
	return true;

}


bool SP_CPN_ValueAssign::InitializeBasicTypeConstant()
{	

	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("ConstantList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) 
	{		
		SP_CPN_Variable_Constant l_ConstantStruct;
		wxString l_sError;
		wxString l_ConstantName = (*l_pvDeclarations)[i].m_sName;		
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_ConstantName,l_sError) )
		{			
			l_sError += wxT("in the constant definition");			
			SP_LOGERROR(l_sError);
			return false;
		}

		l_ConstantName.Replace(wxT(" "), wxT(""));
		l_ConstantName.Replace(wxT("\n"), wxT(""));
		l_ConstantName.Replace(wxT("\t"), wxT(""));

		wxString l_ConstantType = (*l_pvDeclarations)[i].m_sType;
		wxString l_sConstantValue = (*l_pvDeclarations)[i].m_sConstantvalue;
		
		if(l_ConstantType == wxT("int"))
		{
			int l_nValue;
			l_ConstantStruct.m_DataType = CPN_INTEGER;
			if( ! GetIntConstantValue(l_sConstantValue,l_nValue) )
				return false;
			l_ConstantStruct.m_IntegerValue = l_nValue;			
		}		
		else if(l_ConstantType == wxT("bool"))
		{
			l_ConstantStruct.m_DataType = CPN_BOOLEAN;
			if(l_sConstantValue == wxT("false"))
				l_ConstantStruct.m_BooeanValue = false;
			else
				l_ConstantStruct.m_BooeanValue = true;
		}
		else if(l_ConstantType == wxT("string"))
		{
			l_ConstantStruct.m_DataType = CPN_STRING;
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		else if(l_ConstantType == wxT("enum"))
		{
			l_ConstantStruct.m_DataType = CPN_ENUM;
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		else if(l_ConstantType == wxT("index"))
		{
			l_ConstantStruct.m_DataType = CPN_INDEX;
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		else if(l_ConstantType == wxT("product"))
		{
			l_ConstantStruct.m_DataType = CPN_PRODUCT;
			l_ConstantStruct.m_StringValue = l_sConstantValue;
		}
		else if (l_ConstantType == wxT("double") || l_ConstantType == wxT("TFN"))
		{//add by George Assaf (constant harmonizing)
			//double constant to be used as multiplicity or to init. marking in colCpn nets
			if(l_ConstantType == wxT("double"))//by george
			 {
				double l_dValue;
				l_ConstantStruct.m_DataType = CPN_DOUBLE;

				if (l_sConstantValue.ToDouble(&l_dValue))
				{
				   l_ConstantStruct.m_DoubleValue = l_dValue;
				}
				else
				{
				 return false;
				}

			}
			 
		}
		else 
		{
			wxString l_sError;
			l_sError << l_ConstantType <<  wxT(" is not supported in the constant definition.") ;				
			SP_LOGERROR(l_sError);
			return false;
		}			

			
		m_ConstantMap[l_ConstantName] = l_ConstantStruct;

	}
	
	return true;

}

bool SP_CPN_ValueAssign::GetIntConstantValue(wxString p_sConstExpr, int& p_nValue)
{
	wxString l_sConstExpr = p_sConstExpr;
	long l_nValue;
	if(l_sConstExpr.ToLong(&l_nValue))
	{
		p_nValue = l_nValue;
	}
	else
	{
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);
		
		m_pParseContext->SetColorSetClass(m_pcColorSetClass);	
		m_pParseContext->SetColorSetName(wxT(""));
		m_pParseContext->SetFunctionFlag(false);		
		//m_pParseContext->SetNetClassName( m_sNetClassName );	
		m_pParseContext->SetPlaceType(wxT("Place"));
		std::string m_ParseString;
		m_ParseString = string( l_sConstExpr.mb_str() );

		bool l_bEval = m_pDriver->parse_string(m_ParseString, "input");	
		if(!l_bEval)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.") ;			
			SP_LOGERROR(l_sError);
			return false;
		}

		bool l_bCheck = m_pParseContext->CheckExpression();
		if(!l_bCheck)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.") ;			
			SP_LOGERROR(l_sError);
			return false;
		}

		vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;
		if( ! m_pParseContext->EvaluateExpression( l_EvaluateResultArray) || l_EvaluateResultArray.size() == 0)
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.") ;			
			SP_LOGERROR(l_sError);
			return false;
		}
		
		wxString l_sValue = l_EvaluateResultArray.begin()->m_ColorValue;

		if(l_sValue.ToLong(&l_nValue))
		{
			p_nValue = l_nValue;
		}
		else
		{
			wxString l_sError;
			l_sError << wxT(" error in the constant definition.") ;			
			SP_LOGERROR(l_sError);
			return false;
		}

	}

	return true;
}




bool SP_CPN_ValueAssign::InitializeSubsetColorset()
{	
	return true;
}

bool SP_CPN_ValueAssign::InitializeAliasColorset()
{	
	return true;
}


bool SP_CPN_ValueAssign:: InitializeFunction(SP_CPN_ColorSetClass &p_ColorSetClass)
{
	vector<int> l_vInt;
	vector<wxString> l_vString;
	std::map<wxString,SP_CPN_Parameter> l_mParamName2Info;//by george

	
	vector<SP_CPN_Collist_Declarations>* l_pvDeclarations = &(m_svDeclarations[wxT("FunctionList")]);
	
	for (unsigned int i = 0; i < l_pvDeclarations->size(); i++) //(*l_pvDeclarations)[i]
	{	
		//function name
		SP_CPN_Function l_FunctionStruct;
		l_FunctionStruct.m_pDriver = SP_CPN_Driver_ptr(NULL);
		l_FunctionStruct.m_pParseContext = SP_CPN_Parse_Context_ptr(NULL);
		wxString l_sFunctionName = (*l_pvDeclarations)[i].m_sFunctionname;
		wxString l_sError;
		if ( ! m_cColorProcessing.ProcessingIdentifier(l_sFunctionName,l_sError) )
		{
			l_sError = l_sFunctionName + wxT(" is illegal in the function definition");
			//SP_MESSAGEBOX(l_sError, wxT("Function definition checking"), wxOK | wxICON_ERROR);
			SP_LOGERROR(l_sError);
			return false;
		}
		l_sFunctionName.Replace(wxT(" "), wxT(""));
		l_sFunctionName.Replace(wxT("\n"), wxT(""));
		l_sFunctionName.Replace(wxT("\t"), wxT(""));		
		l_FunctionStruct.m_sFunctionName = l_sFunctionName;

		//function color set or data type
		wxString l_sColorsetName= (*l_pvDeclarations)[i].m_sFunctionReturntype;
		if(l_sColorsetName == wxT("int"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_INTEGER;
			l_sColorsetName = wxT("");
		}
		else if(l_sColorsetName == wxT("string"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_STRING;
			l_sColorsetName = wxT("");
		}
		else if(l_sColorsetName == wxT("bool"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_BOOLEAN;
			l_sColorsetName = wxT("");
		}
		else if(l_sColorsetName == wxT("enum"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_ENUM;
			l_sColorsetName = wxT("");
		}
		else if(l_sColorsetName == wxT("index"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_INDEX;
			l_sColorsetName = wxT("");
		}
		else if(l_sColorsetName == wxT("product"))
		{
			l_FunctionStruct.m_ReturnDataType = CPN_PRODUCT;
			l_sColorsetName = wxT("");
		}
		else
		{
			bool l_bFound = false;
			for(unsigned j = 0; j < m_ColorSetVector.size(); j++)
			{			
				if(m_ColorSetVector[j]->GetName() == l_sColorsetName )			  
				{
					l_FunctionStruct.m_ReturnDataType = m_ColorSetVector[j]->GetDataType();
					l_bFound = true;
					break;
				}	
				else
				{	
					
					for(unsigned k = 0; k < m_ColorSetVector[j]->GetAlias().size(); k++)
					{
						if(m_ColorSetVector[j]->GetAlias()[k] == l_sColorsetName )
						{
							l_FunctionStruct.m_ReturnDataType = m_ColorSetVector[j]->GetDataType();
							l_bFound = true;
							break;
						}
					}
				}
			}

			if(!l_bFound)
			{
				wxString l_sError;
				l_sError << wxT("Colorset: ") << l_sColorsetName << wxT(" is not defined in the function definition.") ;
				//SP_MESSAGEBOX(l_sError, wxT("Function definition checking"), wxOK | wxICON_ERROR);				
				SP_LOGERROR(l_sError);
				return false;
			}

		}
		l_FunctionStruct.m_sReturnColorSet = l_sColorsetName;	

		SP_CPN_Parameter l_ParameterStruct;

		wxString l_sParameter= (*l_pvDeclarations)[i].m_sFunctionParameterlist;

		//l_FunctionStruct.m_sParameterString = l_sParameter;

		vector<wxString> l_vResult;
		if( ! m_cColorProcessing.ProcessingParameter(l_sFunctionName,l_sParameter, l_vResult) )
			return false;

		for(unsigned k = 0; k < l_vResult.size(); k++)
		{
			bool l_bFound = false;
			for(unsigned j = 0; j < m_ColorSetVector.size(); j++)
			{
				if(l_vResult[k] == wxT("int"))
				{
					l_bFound = true;
					l_ParameterStruct.m_DataType = CPN_INTEGER;
					l_ParameterStruct.m_sParaColorSet = wxT("");
				}
				if(l_vResult[k] == wxT("bool"))
				{
					l_bFound = true;
					l_ParameterStruct.m_DataType = CPN_BOOLEAN;
					l_ParameterStruct.m_sParaColorSet = wxT("");
				}
				if(m_ColorSetVector[j]->GetName() == l_vResult[k] )			  
				{
					l_ParameterStruct.m_DataType = m_ColorSetVector[j]->GetDataType();
					l_ParameterStruct.m_sParaColorSet = l_vResult[k];
					l_bFound = true;
					break;
				}					
			}

			if(!l_bFound)
			{
				wxString l_sError;
				l_sError << wxT("Colorset: ") <<  l_vResult[k] << wxT(" is not defined in the function definition.") ;					
				SP_LOGERROR(l_sError);
				return false;
			}
			
			l_ParameterStruct.m_nPosition = k/2;
			l_FunctionStruct.m_ParameterMap[l_vResult[k+1]] = l_ParameterStruct;

			l_mParamName2Info = l_FunctionStruct.m_ParameterMap;//by george
			k++;
		}
		wxString l_sConstitutedFun;
		bool l_bIsSubstituted=CheckAndSubstituteForwardDependency(m_FunctionMap, (*l_pvDeclarations)[i].m_sFunctionBody, l_mParamName2Info, l_sConstitutedFun);
		l_mParamName2Info.clear();

		wxString l_sFunctionBody;
		if(!l_bIsSubstituted)
			l_sFunctionBody = (*l_pvDeclarations)[i].m_sFunctionBody;
		else
		{
			l_sFunctionBody = l_sConstitutedFun;
		}
		l_FunctionStruct.m_sFunctionBody = l_sFunctionBody;
		m_FunctionMap[l_sFunctionName] = l_FunctionStruct;    // first store the declaration information
				
	}

	return true;
}

bool SP_CPN_ValueAssign::CheckAndSubstituteForwardDependency(map<wxString, SP_CPN_Function>& p_functionMap, const wxString& p_sOldFunBody, std::map<wxString, SP_CPN_Parameter> p_mParam2Inf, wxString& p_sNewFunction)
{

	wxString l_sCleanedBody = p_sOldFunBody;

	l_sCleanedBody.Replace(wxT(" "), wxT(""));
	l_sCleanedBody.Replace(wxT("\t"), wxT(""));
	l_sCleanedBody.Replace(wxT("\n"), wxT(""));

	std::string l_sOldBody = l_sCleanedBody.ToStdString();
	bool l_bIsSubstitued = false;

	p_sNewFunction = l_sCleanedBody;
	wxString l_sNewBody;

	wxString l_sTobeTokenized = p_sOldFunBody;
	 
	wxStringTokenizer tokenizer(l_sCleanedBody, wxT("()+*/^=<>!%&|,- "));

	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();

		for (auto itMap = p_functionMap.begin(); itMap != p_functionMap.end(); ++itMap)
		{
			if (itMap->first == token)
			{
				size_t l_nStartPos = l_sOldBody.find(token);
				size_t l_nLength = token.length();
				wxString l_sFunParam;


				size_t i = l_nLength;
				unsigned  l_nlastPos= l_nLength;

				vector<wxString> l_vParams;

				if (l_sOldBody.find('(') == std::string::npos || l_sOldBody.find(')') == std::string::npos)
				{
					return false;
				}

				while (l_sOldBody[i] != wxChar('('))
				{
					i++;
				}

				unsigned j = 0;
			    while (l_sOldBody[i] != wxChar(')'))
				{
					j++;
				  if (l_sOldBody[i] == wxChar(','))
				  {
					  l_vParams.push_back(l_sFunParam);
					  l_sFunParam.Empty();
					  i++;
					  continue;
				  }

				  if(l_sOldBody[i]!=wxChar('('))
				  l_sFunParam << l_sOldBody[i];

				  i++;
				}


				l_nlastPos += j;
				l_vParams.push_back(l_sFunParam);

				//check compatibility with the function signature
				unsigned index = 0;
				bool l_bFound = false;
				for (auto itM = p_mParam2Inf.begin(); itM != p_mParam2Inf.end();)
				{
					l_bFound = false;
					if (l_vParams[index] == itM->first && itM->second.m_nPosition == index)
					{
						//check color set type
						auto itFindCs = itMap->second.m_ParameterMap.find(l_vParams[index]);
						if (itFindCs != itMap->second.m_ParameterMap.end())
						{
							if (itFindCs->second.m_sParaColorSet != itM->second.m_sParaColorSet)
							{
								return false;
							}
						}
						
						index++;
						itM = p_mParam2Inf.begin();
						l_bFound = true;
					}
					if(!l_bFound)
					++itM;
				}

				if (index != l_vParams.size() )
				{
					return false;
				}
				else
				{
					//substitute
					wxString l_sNewBody = itMap->second.m_sFunctionBody;
					wxString l_sTobeReplaced = p_sNewFunction.substr(l_nStartPos, l_nlastPos+1);
					p_sNewFunction.Replace(l_sTobeReplaced, l_sNewBody);
					l_bIsSubstitued = true;
				}
			}
		}
		
	} 
	if (l_bIsSubstitued)
		return true;
	else
		return false;
}

bool SP_CPN_ValueAssign::InitializeFunctionParseTree(SP_CPN_ColorSetClass &p_ColorSetClass)
{
	map<wxString, SP_CPN_Function>::iterator itMap;

	for(itMap = m_FunctionMap.begin(); itMap != m_FunctionMap.end(); itMap++)
	{	

		//if(itMap->second.m_sFunctionBody.Contains(wxT("elemOf")))
		//{
		//	continue;
		//}
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);
		
		m_pParseContext->SetColorSetClass(&p_ColorSetClass);	
		m_pParseContext->SetColorSetName(wxT(""));

		m_pParseContext->SetFunctionFlag(true);
		m_pParseContext->SetFunctionName(itMap->second.m_sFunctionName);
		//m_pParseContext->SetNetClassName( m_sNetClassName );
		m_pParseContext->SetPlaceType(wxT("Place"));

		wxString l_sFunctionBody = itMap->second.m_sFunctionBody;
		std::string m_ParseString;
		m_ParseString = string( l_sFunctionBody.mb_str() );
		bool l_bEval = m_pDriver->parse_string(m_ParseString, "input");	

		if(!l_bEval)
		{
			wxString l_sError;
			l_sError << itMap->second.m_sFunctionName << wxT(" parser error in the function definition.") ;
			SP_MESSAGEBOX(l_sError, wxT("Function definition checking"), wxOK | wxICON_ERROR);	
			
			SP_LOGERROR(l_sError);
			return false;
		}

		bool l_bCheck = m_pParseContext->CheckExpression();
		if(!l_bCheck)
		{
			return false;
		}

		itMap->second.m_pParseContext = m_pParseContext;
		itMap->second.m_pDriver = m_pDriver;

	}

	return true;
}


bool SP_CPN_ValueAssign::CollectAllDeclarations()
{
	m_svDeclarations.clear();

	SP_DS_Graph* l_pcGraph;
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute *l_pcColList;

	l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!l_pcGraph)
		return false;

	m_sNetClassName = wxString(l_pcGraph->GetNetclass()->GetName().c_str());

	vector<SP_CPN_Collist_Declarations> l_vDeclarations;

	//simple color sets
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;	
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{	
		SP_CPN_Collist_Declarations l_scDeclaration;
		l_scDeclaration.m_sName = wxString(l_pcColList->GetCell(i,0).c_str());
		l_scDeclaration.m_sType = wxString(l_pcColList->GetCell(i,1).c_str());
		l_scDeclaration.m_sColors = wxString(l_pcColList->GetCell(i,2).c_str());

		l_vDeclarations.push_back(l_scDeclaration);
	}
	m_svDeclarations[wxT("ColorsetList")] = l_vDeclarations;


	l_vDeclarations.clear();
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{		
		SP_CPN_Collist_Declarations l_scDeclaration;
		l_scDeclaration.m_sName = wxString(l_pcColList->GetCell(i,0).c_str());
		l_scDeclaration.m_sType = wxString(l_pcColList->GetCell(i,1).c_str());
		l_scDeclaration.m_sCompound_Colorsets = wxString(l_pcColList->GetCell(i,2).c_str());

		l_vDeclarations.push_back(l_scDeclaration);
	}
	m_svDeclarations[wxT("StructuredColorsetList")] = l_vDeclarations;


	l_vDeclarations.clear();
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		SP_CPN_Collist_Declarations l_scDeclaration;
		l_scDeclaration.m_sName = wxString(l_pcColList->GetCell(i,0).c_str());
		l_scDeclaration.m_sType = wxString(l_pcColList->GetCell(i,1).c_str());		

		l_vDeclarations.push_back(l_scDeclaration);
	}
	m_svDeclarations[wxT("VariableList")] = l_vDeclarations;


	l_vDeclarations.clear();
	if (!l_pcGraph->GetNetclass()->GetName().Contains(wxT("Fuzzy"))) {
		l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
		if (!l_pcMetadataclass)
			return false;
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if (!l_pcNewMetadata)
			return false;
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
		if (!l_pcColList)
			return false;
	}
	/**************************/
	wxString l_sMetaClass;
	if (l_pcGraph->GetNetclass()->GetName().Contains(wxT("Fuzzy")))
	{
		l_sMetaClass = SP_DS_META_CONSTANT;
	}
	else 
	{
		l_sMetaClass = SP_DS_CPN_CONSTANT_HARMONIZING;
	}
	SP_DS_Metadataclass* mc = l_pcGraph->GetMetadataclass(l_sMetaClass);
	SP_ListMetadata::const_iterator it;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

		SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();


		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if (l_FE.IsOk())
		{
			SP_FunctionPtr l_Function = l_FE.getFunction();
			double l_dVal = 0.0;
			long l_nValue = 0;
			if (l_Function->isValue())
			{
				l_nValue = l_Function->getValue();

				if (l_sType == wxT("double"))
				{
				  l_dVal= l_Function->getValue();
				}
				 
				wxString l_sConstVal;
				l_sConstVal << l_nValue;

				if (l_sType == wxT("double"))
				{
					l_sConstVal.Clear();
					l_sConstVal <<  l_dVal;
				}
				SP_CPN_Collist_Declarations l_scDeclaration;
	  		    l_scDeclaration.m_sName = l_sName;
	      	    l_scDeclaration.m_sType = l_sType;
				l_scDeclaration.m_sConstantvalue = l_sConstVal;
	  		    l_vDeclarations.push_back(l_scDeclaration);

	  		  if (l_sType == wxT("bool"))
	  		  	{
	  		  		if(l_nValue==1)
	  		  			l_scDeclaration.m_sConstantvalue = wxT("true");
	  		  		else
	  		  		  l_scDeclaration.m_sConstantvalue = wxT("false");
	  		  	}
	  		  	  l_vDeclarations.push_back(l_scDeclaration);
	      }
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if (l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();
					wxString l_sConstVal; 
					l_sConstVal<< l_nValue;
					SP_CPN_Collist_Declarations l_scDeclaration;
					 l_scDeclaration.m_sName = l_sName;
					 l_scDeclaration.m_sType = l_sType;
					 l_scDeclaration.m_sConstantvalue = l_sConstVal;

					 	l_vDeclarations.push_back(l_scDeclaration);

				}
				else if (l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
					wxString l_sConstVal;
					l_sConstVal << l_nValue;
					SP_CPN_Collist_Declarations l_scDeclaration;
					l_scDeclaration.m_sName = l_sName;
					l_scDeclaration.m_sType = l_sType;
					l_scDeclaration.m_sConstantvalue = l_sConstVal;
					l_vDeclarations.push_back(l_scDeclaration);
				}
				l_pcFR->registerFunction(l_sName, to_string(l_nValue));
			}
		}
		else
		{//fuzzy nets which have been exported from plain col pns
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			wxString l_sMainVAl = l_pcColList->GetCell(0, 1);
			
			if (l_sType == wxT("int"))
			{
				l_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sMainVAl);
				SP_CPN_Collist_Declarations l_scDeclaration;
				l_scDeclaration.m_sName = l_sName;
				l_scDeclaration.m_sType = l_sType;
				l_scDeclaration.m_sConstantvalue = l_sMainVAl;
				l_vDeclarations.push_back(l_scDeclaration);
			}
			else if(l_sType == wxT("double")) {
				l_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sMainVAl);
			}
		}
	}


	/**************************/
	//for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	//{
	//	SP_CPN_Collist_Declarations l_scDeclaration;
		//l_scDeclaration.m_sName = wxString(l_pcColList->GetCell(i,0).c_str());
		//l_scDeclaration.m_sType = wxString(l_pcColList->GetCell(i,1).c_str());		
		//l_scDeclaration.m_sConstantvalue = wxString(l_pcColList->GetCell(i,2).c_str());		

		//l_vDeclarations.push_back(l_scDeclaration);
	//}
	m_svDeclarations[wxT("ConstantList")] = l_vDeclarations;


	l_vDeclarations.clear();
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		SP_CPN_Collist_Declarations l_scDeclaration;
		l_scDeclaration.m_sFunctionReturntype = wxString(l_pcColList->GetCell(i,0).c_str());
		l_scDeclaration.m_sFunctionname = wxString(l_pcColList->GetCell(i,1).c_str());		
		l_scDeclaration.m_sFunctionParameterlist = wxString(l_pcColList->GetCell(i,2).c_str());		
		l_scDeclaration.m_sFunctionBody = wxString(l_pcColList->GetCell(i,3).c_str());		

		l_vDeclarations.push_back(l_scDeclaration);
	}
	m_svDeclarations[wxT("FunctionList")] = l_vDeclarations;

	return true;
}
