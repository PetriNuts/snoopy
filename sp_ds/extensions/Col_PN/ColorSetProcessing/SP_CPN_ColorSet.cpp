//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored colorset class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSet.h"
#include <algorithm>
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"


SP_CPN_ColorSet::SP_CPN_ColorSet()
{					
	m_ColorValues.clear();
	m_Alias.clear();
	m_ComponentMap.clear();	
	m_ComponentName.clear();	
}

SP_CPN_ColorSet::~SP_CPN_ColorSet()
{	
}

wxString SP_CPN_ColorSet::Successor(wxString p_color)
{
	bool l_bFound = false;
	wxString l_color = p_color;
	unsigned l_nFound;

	for(unsigned i = 0; i < m_ColorValues.size(); i++)
	{
		if(l_color == m_ColorValues[i])
		{
			l_nFound = i;
			l_bFound = true;				
			break;
		}
	}
	if(l_bFound)
	{
		if(l_nFound ==  m_ColorValues.size()-1 )
			return m_ColorValues[0];
		else
			return m_ColorValues[l_nFound + 1];
	}
	else
	{
		wxString l_error = l_color + wxT("is not found in the colorset.");
		SP_MESSAGEBOX(l_error, wxT("Expression checking"), wxOK | wxICON_ERROR);
		return wxT("");
	}	
	
}

wxString SP_CPN_ColorSet::Predecessor(wxString p_color)
{
	bool l_bFound = false;
	wxString l_color = p_color;
	unsigned l_nFound;

	for(unsigned i = 0; i < m_ColorValues.size(); i++)
	{
		if(l_color == m_ColorValues[i])
		{
			l_nFound = i;
			l_bFound = true;				
			break;
		}
	}
	if(l_bFound)
	{
		if(l_nFound == 0 )
			return m_ColorValues[m_ColorValues.size()-1];
		else
			return m_ColorValues[l_nFound - 1];
	}
	else
	{
		wxString l_error = l_color + wxT("is not found in the colorset.");
		SP_MESSAGEBOX(l_error, wxT("Expression checking"), wxOK | wxICON_ERROR);
		return wxT("");
	}	
}


vector<wxString> SP_CPN_ColorSet::GetColorsForMarkingPredicat(map<wxString, wxString> p_mColorSetToValue)
{
	vector<wxString> l_vRealColored;

	vector<wxString> l_vColors = GenerateColorsForMarkingPredicat( p_mColorSetToValue );

	for( unsigned i = 0; i < l_vColors.size(); i++ )
	{
		if ( m_DataType == CPN_UNION )
		{
			if( IsColor( l_vColors[i] ) )
			{
				l_vRealColored.push_back( l_vColors[i] );
			}
		}
		else
		{
			if( IsColor( l_vColors[i] ) )
			{
				l_vRealColored.push_back( l_vColors[i] );
			}		
		}
	}
	return l_vRealColored;
}



vector<wxString> SP_CPN_ColorSet::GenerateColorsForMarkingPredicat(map<wxString, wxString> p_mColorSetToValue)
{	
	//new
	for(unsigned int i= 0; i < m_ColorValues.size(); i++)
	{
		wxString l_sColor = m_ColorValues[i];

		if( m_DataType == CPN_PRODUCT )
		{
		}
		else if( m_DataType == CPN_UNION )
		{
		}
		else
		{
			//only allow one variable for simple color sets
		}
	}
	



	if( m_DataType == CPN_PRODUCT )
	{
		vector<wxString> l_vMiddle,l_vResult;
		vector<vector<wxString> > l_ComponentString;
		for(unsigned i = 0; i < m_ComponentName.size(); i++)
		{
			
			if(m_ComponentMap[m_ComponentName[i]]->GetDataType() == CPN_PRODUCT)
			{
				l_vMiddle = m_ComponentMap[m_ComponentName[i]]->GenerateColorsForMarkingPredicat(p_mColorSetToValue);							
			}
			else if(m_ComponentMap[m_ComponentName[i]]->GetDataType() == CPN_UNION)
			{		
				SP_CPN_ColorSet* l_pcColorSet = m_ComponentMap[m_ComponentName[i]];
				map<wxString, SP_CPN_ColorSet*>::iterator itMap;
				for ( itMap = l_pcColorSet->GetComponentMap().begin(); itMap != l_pcColorSet->GetComponentMap().end(); itMap++ )
				{
					vector<wxString> l_vUnion;
					if( itMap->second->GetDataType() == CPN_PRODUCT)
					{
						l_vUnion = itMap->second->GenerateColorsForMarkingPredicat(p_mColorSetToValue);
					}
					else if ( itMap->second->GetDataType() == CPN_UNION)
					{
						l_vUnion = itMap->second->GenerateColorsForMarkingPredicat(p_mColorSetToValue);
					}
					else
					{
						wxString l_sColorSetName = itMap->second->GetName();
						if( p_mColorSetToValue.find(l_sColorSetName) != p_mColorSetToValue.end() )
						{
							l_vUnion.push_back(p_mColorSetToValue.find(l_sColorSetName)->second);
						}
						else
						{
							l_vUnion = m_ComponentMap[m_ComponentName[i]]->GetStringValue();	
						}
					}	
					for(unsigned i = 0; i < l_vUnion.size(); i++ )
					{
						l_vMiddle.push_back( l_vUnion[i] );
					}
				}

			}
			else
			{
				wxString l_sColorSetName = m_ComponentMap[m_ComponentName[i]]->GetName();
				if( p_mColorSetToValue.find(l_sColorSetName) != p_mColorSetToValue.end() )
				{
					l_vMiddle.push_back(p_mColorSetToValue.find(l_sColorSetName)->second);
				}
				else
				{
					l_vMiddle = m_ComponentMap[m_ComponentName[i]]->GetStringValue();	
				}
			}

			l_ComponentString.push_back(l_vMiddle);
			l_vMiddle.clear();
		}


		// Generate Product values
		if(l_ComponentString.size() == 0 )
			return l_vMiddle;   //to be dealt
		if(l_ComponentString.size() == 1 )
			return l_ComponentString[0];

		l_vMiddle.clear();
		l_vMiddle = l_ComponentString[0];
		wxString l_sMiddle;

		for(unsigned i = 0; i < l_ComponentString.size()-1; i++)
		{
			l_vResult.clear();

			for(unsigned j = 0; j < l_vMiddle.size(); j++)
			{
				for(unsigned k = 0; k < l_ComponentString[i+1].size(); k++)
				{
					l_sMiddle = l_vMiddle[j] + wxT(",") + l_ComponentString[i+1][k];
					l_vResult.push_back(l_sMiddle);
				}
			}
			l_vMiddle.clear();
			l_vMiddle = l_vResult;
		}

		for(unsigned m = 0; m < l_vResult.size(); m++)
		{
			wxString l_sString = wxT("(") + l_vResult[m] + wxT(")");	
			//m_ColorValues.push_back(l_sString);
			l_vResult[m] = l_sString;
		}

		return l_vResult;
	}

	else if ( m_DataType == CPN_UNION )
	{
		vector<wxString> l_vMiddle;
		//SP_CPN_ColorSet* l_pcColorSet = m_ComponentMap[m_ComponentName[i]];
		map<wxString, SP_CPN_ColorSet*>::iterator itMap;
		for ( itMap = m_ComponentMap.begin(); itMap != m_ComponentMap.end(); itMap++ )
		{
			vector<wxString> l_vUnion;
			if( itMap->second->GetDataType() == CPN_PRODUCT)
			{
				l_vUnion = itMap->second->GenerateColorsForMarkingPredicat(p_mColorSetToValue);
			}
			else if ( itMap->second->GetDataType() == CPN_UNION)
			{
				l_vUnion = itMap->second->GenerateColorsForMarkingPredicat(p_mColorSetToValue);
			}
			else
			{
				wxString l_sColorSetName = itMap->second->GetName();
				if( p_mColorSetToValue.find(l_sColorSetName) != p_mColorSetToValue.end() )
				{
					l_vUnion.push_back(p_mColorSetToValue.find(l_sColorSetName)->second);
				}
				else
				{
					l_vUnion = itMap->second->GetStringValue();	
				}
			}	
			for(unsigned i = 0; i < l_vUnion.size(); i++ )
			{
				l_vMiddle.push_back( l_vUnion[i] );
			}
		}
		return l_vMiddle;

	}
	else
	{
		vector<wxString> l_vMiddle;
		
		if( p_mColorSetToValue.find(m_sName) != p_mColorSetToValue.end() )
		{
			l_vMiddle.push_back(p_mColorSetToValue.find(m_sName)->second);
		}
		else
		{
			l_vMiddle = m_ColorValues;	
		}
		return l_vMiddle;
	}	
}


bool SP_CPN_ColorSet::GenerateProduct()
{
	vector<wxString> l_vMiddle,l_vResult;
	vector<vector<wxString> > l_ComponentString;
	
	// take out all components values
	for(unsigned i = 0; i < m_ComponentName.size(); i++)
	{
		
		if(m_ComponentMap[m_ComponentName[i]]->GetDataType() == CPN_UNION)
		{
			l_vMiddle = m_ComponentMap[m_ComponentName[i]]->GetStringValue();			
		}
		else
		{
			l_vMiddle = m_ComponentMap[m_ComponentName[i]]->GetStringValue();			
		}

		l_ComponentString.push_back(l_vMiddle);
		l_vMiddle.clear();
	}

	// Generate Product values
	if(l_ComponentString.size() < 2)
	{
		wxString l_sError = wxT("The components of ") + GetName() + wxT(" should be greater than 2");
		SP_MESSAGEBOX(l_sError, wxT("Colorset definition checking"), wxOK | wxICON_ERROR);
		SP_LOGERROR(l_sError);

		return false;
	}

	l_vMiddle.clear();
	l_vMiddle = l_ComponentString[0];
	wxString l_sMiddle;

	for(unsigned i = 0; i < l_ComponentString.size()-1; i++)
	{
		l_vResult.clear();

		for(unsigned j = 0; j < l_vMiddle.size(); j++)
		{
			for(unsigned k = 0; k < l_ComponentString[i+1].size(); k++)
			{
				l_sMiddle = l_vMiddle[j] + wxT(",") + l_ComponentString[i+1][k];
				l_vResult.push_back(l_sMiddle);
			}
		}
		l_vMiddle.clear();
		l_vMiddle = l_vResult;
	}

	for(unsigned m = 0; m < l_vResult.size(); m++)
	{
		wxString l_sString = wxT("(") + l_vResult[m] + wxT(")");	
		m_ColorValues.push_back(l_sString);
	}

	return true;
}



bool SP_CPN_ColorSet::IsColor(wxString p_color)
{

	if(SP_Find(m_ColorValues,p_color) == m_ColorValues.end())
	{
		return false;
	}
/*
	wxString l_color = p_color;	
	bool l_bFound = false;	
 
	for(unsigned i = 0; i < m_ColorValues.size(); i++)
	{
		if( m_DataType == CPN_BOOLEAN)
		{
			if( l_color == m_ColorValues[i] )
			{
				l_bFound = true;
				break;
			}
		}
		else
		{
			if(l_color == m_ColorValues[i])
			{
				l_bFound = true;
				break;
			}
		}
	}

	if(!l_bFound)
	{
		return false;
	} 
*/
	return true;
}

//store all colors of a union color set one times
void SP_CPN_ColorSet::GenerateUnionValues()
{	
	vector<SP_CPN_ColorSet*> m_pColorsetVector;
	TraverseUnionComponents(&m_ComponentMap,&m_pColorsetVector);

	for(unsigned i = 0; i < m_pColorsetVector.size(); i++)
	{
		vector<wxString> l_StringVector = m_pColorsetVector[i]->GetStringValue();
		for(unsigned i = 0; i < l_StringVector.size(); i++)
		{
			m_ColorValues.push_back(l_StringVector[i]);
		}
	}

}


bool SP_CPN_ColorSet::IsUnionComponent(wxString p_sName)
{
	wxString l_sColorsetName = p_sName;
	vector<SP_CPN_ColorSet*> m_pColorsetVector;

	if(m_sName == l_sColorsetName )   // if it is itself
		return true;

	TraverseUnionComponents(&m_ComponentMap,&m_pColorsetVector,true);

	for(unsigned i = 0; i < m_pColorsetVector.size(); i++)
	{
		if(m_pColorsetVector[i]->GetName() == l_sColorsetName)
			return true;
	}
	return false;
}

SP_CPN_DATATYPE SP_CPN_ColorSet::GetUnionDataType()
{
	SP_CPN_DATATYPE l_DataType;
	bool l_bIdentical = true;
	vector<SP_CPN_ColorSet*> m_pColorsetVector;
	TraverseUnionComponents(&m_ComponentMap,&m_pColorsetVector,true);
	for(unsigned i = 0; i < m_pColorsetVector.size(); i++)
	{
		if(i==0)
		{
			l_DataType = m_pColorsetVector[i]->GetDataType();
		}
		if(m_pColorsetVector[i]->GetDataType() != l_DataType)
		{
			l_bIdentical = false;
			break;
		}
	}

	if(l_bIdentical)
	{
		return l_DataType;
	}
	else
	{
		return CPN_NULL;
	}
}



bool SP_CPN_ColorSet::TraverseUnionComponents(map<wxString, SP_CPN_ColorSet*>* p_pComponentMap, vector<SP_CPN_ColorSet*>* p_pColorsetVector, bool p_bAll)
{
	map<wxString, SP_CPN_ColorSet*>::iterator itMap;
	for(itMap = p_pComponentMap->begin(); itMap != p_pComponentMap->end(); itMap++)
	{
		if( itMap->second->GetDataType() != CPN_UNION )
		{
			p_pColorsetVector->push_back(itMap->second);
		}
		else
		{
			if(p_bAll)
			{
				p_pColorsetVector->push_back(itMap->second);
			}
			TraverseUnionComponents(&(itMap->second->GetComponentMap()),p_pColorsetVector,p_bAll);
		}
	}
	return true;
}


SP_CPN_DATATYPE SP_CPN_ColorSet::GetUnionDataType(wxString p_sColor)   // only applies to union color set
{	
	wxString l_sColor = p_sColor;

    //
	vector<SP_CPN_ColorSet*> m_pColorsetVector;
	TraverseUnionComponents(&m_ComponentMap,&m_pColorsetVector);

	for(unsigned i = 0; i < m_pColorsetVector.size(); i++)
	{
		if(m_pColorsetVector[i]->IsColor(l_sColor))
			return m_pColorsetVector[i]->GetDataType();
	}

	return CPN_UNION;
}




wxString SP_CPN_ColorSet::GetProductElement(wxString p_sColor, int p_nPos)
{    
	wxString l_sColor = p_sColor;
	int l_nPos = p_nPos;

	vector<wxString> l_vString;

    unsigned    l_iChar = 0;
    int l_nLevel =0;    

    //remove the the first '(' and the last ')'
	l_sColor  = l_sColor.AfterFirst(wxT('('));
	l_sColor  = l_sColor.BeforeLast(wxT(')'));

    wxChar l_Char = l_sColor[l_iChar];

    int l_MarkPos = l_iChar;

    while( l_iChar < l_sColor.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
        else if(l_Char == ',' && l_nLevel == 0  )
        {
            wxString l_SubString = l_sColor.substr(l_MarkPos,l_iChar - l_MarkPos);           
            l_vString.push_back(l_SubString);
            l_MarkPos = l_iChar+1;                 
        }

        l_Char = l_sColor[++l_iChar];

        if(l_iChar == l_sColor.size() -1 )
        {
            wxString l_SubString = l_sColor.substr(l_MarkPos,l_sColor.size() - l_MarkPos); 
            l_vString.push_back(l_SubString);     
        }
    }
	if((int)(l_vString.size()) >= l_nPos)
		return l_vString[l_nPos-1];

	return wxT("");
}



bool SP_CPN_ColorSet::IsVariableofColorSet(wxString p_sColorSetName)
{
	if( p_sColorSetName == m_sName)
	{
		return true;
	}

	vector<SP_CPN_ColorSet*> l_pColorsetVector;
	TraverseComponents(&m_ComponentMap,&l_pColorsetVector);


	for(unsigned i = 0; i < l_pColorsetVector.size(); i++)
	{
		if(l_pColorsetVector[i]->GetName() == p_sColorSetName)
			return true;
	}

	return false;
}


bool SP_CPN_ColorSet::TraverseComponents(map<wxString, SP_CPN_ColorSet*>* p_pComponentMap, vector<SP_CPN_ColorSet*>* p_pColorsetVector)
{
	map<wxString, SP_CPN_ColorSet*>::iterator itMap;
	for(itMap = p_pComponentMap->begin(); itMap != p_pComponentMap->end(); itMap++)
	{
		if( !(itMap->second->GetDataType() == CPN_UNION || itMap->second->GetDataType() == CPN_PRODUCT))
			p_pColorsetVector->push_back(itMap->second);
		else
		{
			p_pColorsetVector->push_back(itMap->second);
			TraverseComponents(&(itMap->second->GetComponentMap()),p_pColorsetVector);
		}
	}
	return true;
}


bool SP_CPN_ColorSet::CheckVariableInSubset(wxString p_sColorSet)
{
	if( p_sColorSet == m_sName)
	{
		return true;
	}
	else
	{
		for(unsigned i=0; i < m_vSubSet.size(); i++)
		{
			if( p_sColorSet == m_vSubSet[i])
			{
				return true;
			}
		}	
	}

	return false;
}



bool SP_CPN_ColorSet::CheckProductSyntax( wxString p_sCompSets, wxString p_sErrorPosition)
{
	
	if( ! Check(p_sCompSets, this,p_sErrorPosition) )
		return false;

	return true;
}

bool SP_CPN_ColorSet::Check(wxString p_sCompSets,SP_CPN_ColorSet* p_pcColorSet, wxString p_sErrorPosition)
{    
    unsigned l_iChar = 0;
    int l_nLevel =0;    

    //remove the the first '(' and the last ')'
	p_sCompSets  = p_sCompSets.AfterFirst(wxT('('));
	p_sCompSets  = p_sCompSets.BeforeLast(wxT(')'));

    wxChar l_Char = p_sCompSets[l_iChar];
    int l_MarkPos = l_iChar;
	unsigned int l_nCompPos = 0;

    while( l_iChar < p_sCompSets.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
        else if(l_Char == ',' && l_nLevel == 0  )
        {
            wxString l_SubString = p_sCompSets.substr(l_MarkPos,l_iChar - l_MarkPos);   

			wxString l_sCompName;
			if(l_nCompPos < p_pcColorSet->GetComponentName().size() )
			{
				l_sCompName = p_pcColorSet->GetComponentName()[l_nCompPos];
			}
			else
			{
				wxString l_sError;					
				l_sError = wxT(" product type mismatch. Position: ") + p_sErrorPosition;
				//SP_LOGERROR(l_sError);
				return false;
			}
			SP_CPN_ColorSet* l_pcCompColorSet = p_pcColorSet->GetComponentMap().find(l_sCompName)->second;

            if (l_SubString.Find('(') != -1)
            {
                //StringVariable.push_back(l_SubString);						
				if( ! Check(l_SubString,l_pcCompColorSet,p_sErrorPosition) )
				{
					wxString l_sError;					
					l_sError = wxT(" product type mismatch. Position: ") + p_sErrorPosition;
					//SP_LOGERROR(l_sError);
					return false;
				}
                l_MarkPos = l_iChar+1;
				l_nCompPos++;
            }
            else
            {
                //StringVariable.push_back(l_SubString); 				
				wxString l_sDataType = wxT("");
				l_sDataType << l_pcCompColorSet->GetDataType();
				//not considering union so far
				if( !(l_SubString == l_sCompName || l_SubString == l_sDataType) ) 
				{
					wxString l_sError;					
					l_sError = wxT(" product type mismatch. Position: ") + p_sErrorPosition;
					//SP_LOGERROR(l_sError);
					return false;
				}
                l_MarkPos = l_iChar+1;
				l_nCompPos++;
            }            
        }

		if (l_iChar < p_sCompSets.size() - 1)
			l_Char = p_sCompSets[++l_iChar];
		else
			++l_iChar;

        if(l_iChar == p_sCompSets.size() -1 )
        {
            wxString l_SubString = p_sCompSets.substr(l_MarkPos,p_sCompSets.size() - l_MarkPos);

			wxString l_sCompName;
			if(l_nCompPos < p_pcColorSet->GetComponentName().size() )
			{
				l_sCompName = p_pcColorSet->GetComponentName()[l_nCompPos];
			}
			else
			{
				wxString l_sError;					
				l_sError = wxT(" product type mismatch. Position: ") + p_sErrorPosition;
				//SP_LOGERROR(l_sError);
				return false;
			}
			SP_CPN_ColorSet* l_pcCompColorSet = p_pcColorSet->GetComponentMap().find(l_sCompName)->second;
            
            if (l_SubString.Find('(') != -1)
            {
               	if( ! Check(l_SubString,l_pcCompColorSet,p_sErrorPosition) )
				{
					wxString l_sError;					
					l_sError = wxT(" product type mismatch. Position: ") +p_sErrorPosition;
					//SP_LOGERROR(l_sError);
					return false;
				}
            }
            else
            {               
				wxString l_sDataType = wxT("");
				l_sDataType << l_pcCompColorSet->GetDataType();
				//not considering union so far
				if( !(l_SubString == l_sCompName || l_SubString == l_sDataType) ) 
				{
					wxString l_sError;					
					l_sError = wxT(" product type mismatch. Position: ") + p_sErrorPosition;
					//SP_LOGERROR(l_sError);
					return false;
				}                
            }        
        }
    }

	return true;
}
