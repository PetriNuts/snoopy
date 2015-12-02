//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: Binding class
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_Vector.h"

#include <algorithm>

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"


SP_CPN_ColorProcessing::SP_CPN_ColorProcessing()
{
	m_peekc = ' ';	
	m_iChar = 0;
	m_bColorSetClass = false;
}

bool  SP_CPN_ColorProcessing::GetTokenforInt(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString)
{
	m_sString = p_ParseString;
	p_StoredString.clear();
	int c = m_peekc;
	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}
		//deal with digit
		if( ::isdigit( c ) )
		{
			//double v = 0;
			int v = 0;
			while( true ) 
			{
				if ( ::isdigit( c ) ) 
				{
					v = v* 10 + (c - '0');				
				} 
				else if(::isspace( c ) || c == ',' || c == '-' || m_iChar >= m_sString.size())
				{					
					wxString l_s;
					l_s<<v;
					p_StoredString.push_back(l_s);
					break;
				}
				else
				{
					wxString l_sInvalid = wxString::Format(wxT("%c"), c);
					wxString l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the colorset: ") + p_sColorsetName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;

				}
				c = GetChar();
			}
		}

		//deal with other cases
		else if( c == ',' ) 
		{
			p_StoredString.push_back(wxT(","));
			c = GetChar();
		}
		
		else if( c == '-' ) 
		{
			p_StoredString.push_back(wxT("-"));
			c = GetChar();
		}	
		
		else if(::isalpha( c ) || c == '_' ) 
		{
			wxString l_constant;

			while(true)
			{				
				if(::isalnum(c)|| c == '_')
				{	
					l_constant << wxString::Format(wxT("%c"), c);
				}
				else if(::isspace( c ) || m_iChar >= m_sString.size())
				{				
					// Add code to get the value that corresponds to the constant

					map<wxString, SP_CPN_Variable_Constant> l_mConstant;

					if(m_bColorSetClass)
					{
						l_mConstant = *(m_pcColorSetClass->GetConstantMap());
					}
					else
					{
						GetConstant(l_mConstant);
					}

					if( l_mConstant.find(l_constant) != l_mConstant.end() )
					{
						SP_CPN_Variable_Constant l_cConstant = l_mConstant.find(l_constant)->second;
						if( l_cConstant.m_DataType ==  CPN_INTEGER || l_cConstant.m_ColorSet == p_sColorsetName )
						{
							p_StoredString.push_back(wxString::Format(wxT("%d"),l_cConstant.m_IntegerValue));							
						}
						else
						{
							wxString l_sError = p_ParseString + wxT(": Constant type error: ") + l_constant + wxT("when defining the colorset: ") + p_sColorsetName;
							SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
							SP_LOGERROR(l_sError);
							return false;							
						}
					}
					else
					{
						wxString l_sError = p_ParseString + wxT(": Constant is not defined: ") + l_constant + wxT("when defining the colorset: ") + p_sColorsetName;
						SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
						SP_LOGERROR(l_sError);
						return false;	

					}

					//if(l_constant)
					//	p_StoredString.push_back(wxT("555")); // 555 is supposed to be the value of the constant

					break;
				}
				else
				{
					wxString l_sInvalid = wxString::Format(wxT("%c"), c);
					wxString l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the colorset: ") + p_sColorsetName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;

				}

				c = GetChar();
			}
		}	
		else if(c == 0)
		{
			break;
		}
		else
		{
			wxString l_sInvalid = wxString::Format(wxT("%c"), c);
			wxString l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());
	
	return true;
}



bool SP_CPN_ColorProcessing::ProcessingInt(wxString p_sColorsetName, wxString & p_ParseString, vector<unsigned> &p_vResult, bool &p_bSuccessive)
{
	vector<wxString> l_StoredString;
	l_StoredString.clear();
	if(!GetTokenforInt(p_sColorsetName,p_ParseString,l_StoredString))
		return false;
	
	long l_value;	

	if(l_StoredString[0] == wxT(",") || 
		l_StoredString[0] == wxT("-")||
		l_StoredString[l_StoredString.size()-1] == wxT(",") || 
		l_StoredString[l_StoredString.size()-1] == wxT("-"))
	{
		wxString l_sError = p_ParseString + wxT(": Input format error") + wxT("when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	for(unsigned i = 0; i < l_StoredString.size(); i++)
	{
		if(l_StoredString[i] == wxT(",") || l_StoredString[i] == wxT("-") )
		{
			wxString l_sError = p_ParseString + wxT(": Input format error") + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 1; i < l_StoredString.size(); i++)
	{
		if(l_StoredString[i] != wxT(",") && l_StoredString[i] != wxT("-") )
		{
			wxString l_sError = p_ParseString + wxT(": Input format error") + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 0; i < l_StoredString.size(); i++)
	{
		if(l_StoredString[i]!= wxT(",") && l_StoredString[i] != wxT("-") )
		{
			if(l_StoredString[i].ToLong(&l_value))
				p_vResult.push_back(l_value);
		}
		
		if(l_StoredString[i] == wxT("-"))
		{
			unsigned int l_low, l_up;
			unsigned index = p_vResult.size()-1;
			l_StoredString[i+1].ToLong(&l_value);

			if(l_value >= (long)(p_vResult[index]))
			{
				l_low = p_vResult[p_vResult.size()-1];
				l_up = l_value;
			}
			else
			{
				l_up = p_vResult[p_vResult.size()-1];
				l_low = l_value;
			}

			p_vResult.pop_back();
			for(; l_low <= l_up; l_low++)
			{
				
				p_vResult.push_back(l_low);
			}
			i++;
		}
	}	

	//Removes the duplicate consecutive elements

	unsigned l_nCountb = p_vResult.size();
	//first sort it
	//std::sort(p_vResult.begin(),p_vResult.end());
	vector<unsigned>::iterator it;
	it = std::unique(p_vResult.begin(), p_vResult.end());
	p_vResult.resize( it - p_vResult.begin() ); 

	unsigned l_nCounta = p_vResult.size();
	if(l_nCountb > l_nCounta)
	{
		//warning
		wxString l_sWarning = wxT("There are duplicate elements in the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sWarning, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_WARNING );
		SP_LOGWARNING(l_sWarning);
	}

	//sort it
	std::sort(p_vResult.begin(),p_vResult.end());

	//evaluate if it is successive
	if( p_ParseString.Find(wxT(",")) == wxNOT_FOUND )
	{
		p_bSuccessive = true;
	}
	else 
	{
		if(p_vResult.size() <= 1)
		{
			p_bSuccessive = true;
		}
		else
		{
			//for two cases, only "," or both "," and "-"
			unsigned l_nPreOne = p_vResult[0];
			for(unsigned int i = 1; i < p_vResult.size(); i++)
			{
				unsigned l_nCurrVal = p_vResult[i];
				if(l_nPreOne+1 != l_nCurrVal)
				{
					p_bSuccessive = false;
					break;
				}
				l_nPreOne = l_nCurrVal;
			}
		}
	}

	return true;

}





bool  SP_CPN_ColorProcessing::GetTokenforString(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &l_StoredString)
{
	m_sString = p_ParseString;	
	
	l_StoredString.clear();

	wxString l_sResult = wxT("");
	int c = m_peekc;

	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle = wxT("");
					//char a = static_cast<char>(c);
					l_sMiddle = wxString::Format(wxT("%c"), c);
	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == ',' || c == '-' || m_iChar >= m_sString.size())
				{
					l_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sInvalid = wxString::Format(wxT("%c"), c);
					wxString l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the colorset: ") + p_sColorsetName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases
		else if( c == ',' ) 
		{
			l_StoredString.push_back(wxT(","));
			c = GetChar();
		}
		
		else if( c == '-' ) 
		{
			l_StoredString.push_back(wxT("-"));
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}
				
		else
		{
			wxString l_sInvalid = wxString::Format(wxT("%c"), c);
			wxString l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());

	return true;
}



bool SP_CPN_ColorProcessing::ProcessingString(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult)
{
	m_sString = p_ParseString;
	vector<wxString> l_vString;
	vector<wxString> l_vStoredString;
	typedef vector<wxString> StringVector;
	vector<StringVector> l_vvString;
	l_vString.clear();
	l_vStoredString.clear();
	l_vvString.clear();

	// Identify if there is or lack of a '[' or ']'
	int l_nLeftBracket = m_sString.Freq('[');
	int l_nRightBracket = m_sString.Freq(']');
	if(l_nLeftBracket == 0 && l_nRightBracket == 0)
	{
			GetTokenforString(p_sColorsetName,m_sString,l_vStoredString);	
			l_vvString.push_back(l_vStoredString);
	}
	else if(l_nLeftBracket != l_nRightBracket)
	{
		wxString l_sError;
		if(l_nLeftBracket > l_nRightBracket)
			l_sError << wxT("Lack of ']' ")  ;
		else
			l_sError << wxT("Lack of '[' ")  ;

		l_sError = m_sString + wxT(": ") + l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;

	}
	else
	{
		wxString m_sProcessingString = m_sString; 
		wxString  m_middleString;
		
		wxString  m_sThirdString = wxT("");

		while(m_sProcessingString.Freq('[') != 0)   // [1,200,45,550][a,c,b]
		{
			m_middleString = m_sProcessingString;
			m_middleString = m_middleString.BeforeFirst(wxT('[')); // It does not work.
			m_middleString.Replace(wxT(" "), wxT(""));
			m_middleString.Replace(wxT("\n"), wxT(""));
			m_middleString.Replace(wxT("\t"), wxT(""));
			if(m_middleString != wxT(""))
			{
				wxString l_sError;
				l_sError << wxT("Format error before the first '[' ") ;						
				
				l_sError = m_sString + wxT(": ") + l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
				SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
				SP_LOGERROR(l_sError);
				return false;

			}

			m_sProcessingString = m_sProcessingString.AfterFirst('[');
			m_middleString = m_sProcessingString;
			m_middleString = m_middleString.BeforeFirst(']');
			m_sThirdString = m_middleString;
			if(m_sThirdString.Freq('['))
			{
				wxString l_sError;
				l_sError << wxT("Format error before the first ']' ") ;						
				
				l_sError = m_sString + wxT(": ") + l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
				SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
				SP_LOGERROR(l_sError);
				return false;
			}
			m_sThirdString.Replace(wxT(" "), wxT(""));
			m_sThirdString.Replace(wxT("\n"), wxT(""));
			m_sThirdString.Replace(wxT("\t"), wxT(""));
			if(m_sThirdString == wxT(""))
			{
				wxString l_sError;
				l_sError << wxT("It should not be empty between '[' and ']' ");
				l_sError = m_sString + wxT(": ") + l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
				SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
				SP_LOGERROR(l_sError);
				return false;
			}
			l_vString.push_back(m_middleString);
			m_sProcessingString = m_sProcessingString.AfterFirst(']');			
		}
	}
	
	for(unsigned i = 0; i < l_vString.size();i++)
	{
		l_vStoredString.clear();
		if( !GetTokenforString(p_sColorsetName,l_vString[i],l_vStoredString))
			return false;
		l_vvString.push_back(l_vStoredString);
	}


	// Then will generate the needed strings
	if(l_vvString.size() == 1)
		ProcessingStringSub(p_sColorsetName,l_vvString[0], p_vResult);
	if(l_vvString.size() > 1)
	{
		//Add codes to generate strings from regular expressions
		
		vector<StringVector> l_vvStringSub;
		l_vvStringSub.clear();
		for(unsigned i = 0; i < l_vvString.size(); i++)
		{
			ProcessingStringSub(p_sColorsetName, l_vvString[i], p_vResult);
			l_vvStringSub.push_back(p_vResult);
		}		
		
		vector<wxString> l_vMiddle, l_vResult;
		l_vMiddle.clear();
		l_vResult.clear();
		l_vMiddle = l_vvStringSub[0];
		wxString l_sMiddle;

		for(unsigned i = 0; i < l_vvStringSub.size()-1; i++)
		{
			for(unsigned j = 0; j < l_vMiddle.size(); j++)
			{
				for(unsigned k = 0; k < l_vvStringSub[i+1].size(); k++)
				{
					l_sMiddle = l_vMiddle[j] + l_vvStringSub[i+1][k];
					l_vResult.push_back(l_sMiddle);
				}
			}
			l_vMiddle.clear();
			l_vMiddle = l_vResult;
		}
		p_vResult = l_vResult;
	}
	return true;
}

bool SP_CPN_ColorProcessing::ProcessingStringSub(wxString p_sColorsetName, vector<wxString> &p_vInputString, vector<wxString> &p_vString)
{
	p_vString.clear();

	if(p_vInputString[0] == wxT(",") ||
		p_vInputString[0] == wxT("-")||
		p_vInputString[p_vInputString.size()-1] == wxT(",") ||
		p_vInputString[p_vInputString.size()-1] == wxT("-"))
	{
		wxString l_sError;
		l_sError << wxT("Input format error");
		l_sError = l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	for(unsigned i = 0; i < p_vInputString.size(); i++)
	{
		if(p_vInputString[i] == wxT(",") || p_vInputString[i] == wxT("-") )
		{
			wxString l_sError;
			l_sError << wxT("Input format error");
			l_sError = l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 1; i < p_vInputString.size(); i++)
	{
		if(p_vInputString[i] != wxT(",") && p_vInputString[i] != wxT("-") )
		{
			wxString l_sError;
			l_sError << wxT("Input format error");
			l_sError = l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 0; i < p_vInputString.size(); i++)
	{

		if(p_vInputString[i]!= wxT(",") && p_vInputString[i] != wxT("-") )
		{
			p_vString.push_back(p_vInputString[i]);
		}

		if(p_vInputString[i] == wxT("-"))
		{
			
			wxString l_slow, l_sup;
			unsigned index = p_vString.size()-1;
			l_slow = p_vString[index];
			p_vString.pop_back();
			l_sup = p_vInputString[i+1];

			/////////////////////////////////////////////////////////////////////////////
			//deal with the constant
			map<wxString, SP_CPN_Variable_Constant> l_mConstant;
			if(m_bColorSetClass)
			{
				l_mConstant = *(m_pcColorSetClass->GetConstantMap());
			}
			else
			{
				GetConstant(l_mConstant);
			}
			if( l_mConstant.find(l_sup) != l_mConstant.end() )
			{
				SP_CPN_Variable_Constant l_cConstant = l_mConstant.find(l_sup)->second;
				if( l_cConstant.m_DataType ==  CPN_INTEGER )
				{
					l_sup = wxString::Format(wxT("%d"),l_cConstant.m_IntegerValue);							
				}
			}
			/////////////////////////////////////////////////////////////////////////////



			//for digital range
			long l_nLow, l_nUp;
			bool l_bLow = l_slow.ToLong(&l_nLow);
			bool l_bUp = l_sup.ToLong(&l_nUp);

			vector<wxString> StringVector;

			if( l_bLow & l_bUp )
			{
				GenerateDigitalRange(p_sColorsetName,l_nLow,l_nUp,&StringVector); 
			}
			else
			{
				// Add codes to deal with the generation of strings from l_slow to l_sup				
				GenerateString(p_sColorsetName,l_slow,l_sup,&StringVector); 
			}

			for(unsigned k = 0; k < StringVector.size(); k++)
			{
				p_vString.push_back(StringVector[k]);
			}			
			i++;
		}
	}	

	return true;
}

bool SP_CPN_ColorProcessing::GenerateDigitalRange(wxString p_sColorsetName, long p_nLowBound, long p_nUpBound, vector<wxString> * p_vString)
{
	long l_nTemp;
	if( p_nLowBound > p_nUpBound)
	{
		l_nTemp = p_nLowBound;
		p_nLowBound = p_nUpBound;
		p_nUpBound = l_nTemp;
	}
	for(long i = p_nLowBound; i <= p_nUpBound; i++)
	{
		p_vString->push_back( wxString::Format(wxT("%d"),i));
	}
	return true;
}


bool SP_CPN_ColorProcessing::GenerateString(wxString p_sColorsetName,wxString SL, wxString SH, vector<wxString> *StringVector)
{

	wxString SLow;
    wxString SHigh;

	bool l_b = operator <(SL,SH);
	if(l_b)
	{
		SLow = SL;
		SHigh = SH;
	}
	else
	{
		SLow = SH;
		SHigh = SL;
	}

	



	if(SLow.size() != SHigh.size())
	{
		wxString l_sError;
		l_sError << wxT("The size of ") << SLow << wxT("and") << SHigh << wxT("should be same.");
		
		l_sError = l_sError + wxT("when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;

	}
	
	unsigned size = SLow.size();
    
    wxString SA = wxT("");
    wxString SZ = wxT("");
    
    for(unsigned i = 0; i < size; i++)
    {
        SA += wxT("0");
        SZ += wxT("z");        
    }
    vector<wxString> V;
    GenerateAllString(SA, SZ, &V);
	

	for(unsigned k = 0; k < V.size(); k++ )
	{
		if(SLow <= V[k] && V[k] <= SHigh)
			StringVector->push_back(V[k]);			
	}

	return true; 
}



bool SP_CPN_ColorProcessing::GenerateAllString(wxString SA, wxString SZ, vector<wxString> *StringVector)
{
    StringVector->clear();
    
    wxString SC;
    wxString SD;
    wxString RT;
    if(SA.size() != 1)
    {
		SC = SA.AfterFirst(wxT('0')); 
        SD = SZ.AfterFirst(wxT('z')); 
        
        vector<wxString> VV;
        GenerateAllString(SC, SD, &VV);
        
        for(int i = '0'; i <= 'z'; i++)
        {
            if(i == 58)
			{
				i += 7;				
			}
			
			if(i == 91)
			{
				i += 6;				
			}

			for(unsigned j = 0; j < VV.size(); j++)
            {                
				RT = wxString::Format(wxT("%c"), i) + VV[j];
                StringVector->push_back(RT);
            }
        }
    }
    else
    {
        for(int i = '0'; i <= 'z'; i++)
        {
            if(i == 58)
			{
				i += 7;				
			}
			
			if(i == 91)
			{
				i += 6;				
			}	
			
			RT = wxString::Format(wxT("%c"), i);
			StringVector->push_back(RT);
			
        }
    }
	return true;

}


bool SP_CPN_ColorProcessing::ProcessingEnum(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult)
{
	wxString l_ParseString = p_ParseString;
	vector<wxString> l_sStoredString;

	GetTokenforEnum(p_sColorsetName,l_ParseString, l_sStoredString);

	p_vResult.clear();

	if(l_sStoredString[l_sStoredString.size()-1] == wxT(",") )
	{
			
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	for(unsigned i = 0; i < l_sStoredString.size(); i++)
	{
		if(l_sStoredString[i] == wxT(","))
		{
			wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 1; i < l_sStoredString.size(); i++)
	{
		if(l_sStoredString[i] != wxT(","))
		{
			wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
		i++;
	}

	for(unsigned i = 0; i < l_sStoredString.size(); i++)
	{
		if(l_sStoredString[i]!= wxT(","))
		{
			p_vResult.push_back(l_sStoredString[i]);
		}	
	}	
	
	return true;
}


bool SP_CPN_ColorProcessing::GetTokenforEnum(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString)
{
	m_sString  = p_ParseString;		
	p_StoredString.clear();
	wxString l_sResult = wxT("");
	int c = m_peekc;
	m_iChar = 0;


	/// is it null?
	wxString l_sNull = m_sString;
	l_sNull.Replace(wxT(" "), wxT(""));
	if(l_sNull == wxT(""))
	{
	
		wxString l_sError =  wxT("Enum colors should not be null when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle;					
					l_sMiddle = wxString::Format(wxT("%c"), c);	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == ',' || m_iChar >= m_sString.size())
				{
					p_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					
					wxString l_sError = wxString::Format(wxT("%c"), c);	;
					l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sError + wxT(" when defining the colorset: ") +p_sColorsetName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases

		else if( c == ',' ) 
		{
			p_StoredString.push_back(wxT(","));
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}
		else
		{
			wxString l_sError = wxString::Format(wxT("%c"), c);	;
			l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sError + wxT(" when defining the colorset: ") +p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;

		}		
	} while(m_iChar <= m_sString.size());

	return true;
}



bool SP_CPN_ColorProcessing::ProcessingIndex(wxString p_sColorsetName, wxString & p_ParseString, wxString & p_IndexName, vector<unsigned> &p_vResult)
{	
	wxString l_ParseString = p_ParseString;
	vector<wxString> l_sStoredString;
	GetTokenforIndex(p_sColorsetName,l_ParseString, l_sStoredString);
	p_vResult.clear();

	if(l_sStoredString.size() != 6)
	{
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	if(l_sStoredString[0] == wxT("[") || l_sStoredString[0] == wxT("]") || l_sStoredString[0] == wxT("-") )
	{
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}
	
	if(l_sStoredString[1] != wxT("["))
	{
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}
	
	long l_n;
	if(!l_sStoredString[2].ToLong(&l_n))	
	{
		
		wxString l_sError = p_ParseString + wxT(": Index should be digital number when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	if(l_sStoredString[3] != wxT("-"))
	{
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	if(!l_sStoredString[4].ToLong(&l_n))	
	{
		wxString l_sConstant = l_sStoredString[4];
		map<wxString, SP_CPN_Variable_Constant> l_mConstant;

		if(m_bColorSetClass)
		{
			l_mConstant = *(m_pcColorSetClass->GetConstantMap());
		}
		else
		{
			GetConstant(l_mConstant);
		}
		if( l_mConstant.find(l_sConstant) != l_mConstant.end() )
		{
			SP_CPN_Variable_Constant l_cConstant = l_mConstant.find(l_sConstant)->second;
			if( l_cConstant.m_DataType ==  CPN_INTEGER || l_cConstant.m_ColorSet == p_sColorsetName )
			{
				l_sStoredString[4] = wxString::Format(wxT("%d"),l_cConstant.m_IntegerValue);							
			}
			else
			{							
				return false;							
			}
		}
		else
		{						
			wxString l_sError = p_ParseString + wxT(": Index should be digital number when defining the colorset: ") + p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}
	}

	if(l_sStoredString[5] != wxT("]"))
	{
		wxString l_sError = p_ParseString + wxT(": Input format error when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}

	//////////////////////////////////////////////////////////
	p_IndexName = l_sStoredString[0];
	long l_nLow, l_nHigh;
	l_sStoredString[2].ToLong(&l_nLow);
	l_sStoredString[4].ToLong(&l_nHigh);
	for(long i = l_nLow; i <= l_nHigh; i++)
	{
		p_vResult.push_back(i);
	}


	
	return true;
}


bool SP_CPN_ColorProcessing::GetTokenforIndex(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString)
{
	m_sString = p_ParseString;		
	p_StoredString.clear();
	wxString l_sResult = wxT("");
	int c = m_peekc;
	m_iChar = 0;

	/// is it null?
	wxString l_sNull = m_sString;
	l_sNull.Replace(wxT(" "), wxT(""));
	if(l_sNull == wxT(""))
	{
		wxString l_sError =  wxT("Index colors should not be empty when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}


	do
	{		
		//deal with space

		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter

		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle;					
					l_sMiddle = wxString::Format(wxT("%c"), c);	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == '[' || c == ']' || c == '-')
				{
					p_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sError = wxString::Format(wxT("%c"), c);	;
					l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sError + wxT(" when defining the colorset: ") +p_sColorsetName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases

		else if( c == '['  ) 
		{
			p_StoredString.push_back(wxT("["));
			c = GetChar();
		}	
		else if( c == ']' ) 
		{
			p_StoredString.push_back(wxT("]"));
			c = GetChar();
		}	
		else if( c == '-' ) 
		{
			p_StoredString.push_back(wxT("-"));
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}

		else
		{
			wxString l_sError = wxString::Format(wxT("%c"), c);	;
			l_sError = p_ParseString + wxT(": Invalid symbol: ") + l_sError + wxT(" when defining the colorset: ") +p_sColorsetName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}		
	} while(m_iChar <= m_sString.size());

	
	return true;
}


bool SP_CPN_ColorProcessing::ProcessingComponent(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult)
{
	m_sString  = p_ParseString;	
	p_vResult.clear();

	wxString l_sResult = wxT("");
	m_iChar = 0;

	/// is it null?
	wxString l_sNull = m_sString;
	l_sNull.Replace(wxT(" "), wxT(""));
	l_sNull.Replace(wxT("\n"), wxT(""));
	l_sNull.Replace(wxT("\t"), wxT(""));

	if(l_sNull == wxT(""))
	{
		wxString l_sError =  wxT("product colors should not be empty when defining the colorset: ") + p_sColorsetName;
		SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
		SP_LOGERROR(l_sError);
		return false;
	}
		

	m_sString.Replace(wxT(" "),wxT(""));

	while(m_sString.Freq(wxT(',')))
	{
		l_sResult = m_sString.BeforeFirst(wxT(','));
		p_vResult.push_back(l_sResult);
		m_sString = m_sString.AfterFirst(wxT(','));
	}
	p_vResult.push_back(m_sString);

	// The next step is to test if these component colorsets have been defined.

	return true;
}


bool SP_CPN_ColorProcessing::ProcessingParameter(wxString p_sFunctionName, wxString p_sParameter, vector<wxString> &p_vResult)
{
	m_sString = p_sParameter;	
	
	vector<wxString> l_StoredString;
	

	wxString l_sResult = wxT("");
	int c = m_peekc;

	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle = wxT("");
					
					l_sMiddle = wxString::Format(wxT("%c"), c);
	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == ',' || c == ' ' || m_iChar >= m_sString.size())
				{
					l_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sInvalid = wxString::Format(wxT("%c"), c);
					wxString l_sError = p_sParameter + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the function: ") + p_sFunctionName;
					SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sError);
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases
		else if( c == ',' ) 
		{
			
			c = GetChar();
		}
		
		else if( c == ' ' ) 
		{
			
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}
				
		else
		{
			wxString l_sInvalid = wxString::Format(wxT("%c"), c);
			wxString l_sError = p_sParameter + wxT(": Invalid symbol: ") + l_sInvalid + wxT("when defining the function: ") + p_sFunctionName;
			SP_MESSAGEBOX(l_sError, wxT("Syntax checking of Colorset Definition"), wxOK | wxICON_ERROR );
			SP_LOGERROR(l_sError);
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());

	p_vResult = l_StoredString;

	return true;

}



bool SP_CPN_ColorProcessing::ProcessingIdentifier(wxString p_identifier,wxString &p_sError)
{
	m_sString = p_identifier;
	int c = m_peekc;
	m_iChar = 0;
	wxString l_constant;
	l_constant = wxT("");

	wxString l_NullString;
	l_NullString = m_sString;
	l_NullString.Replace(wxT(" "), wxT(""));
	l_NullString.Replace(wxT("\n"), wxT(""));
	l_NullString.Replace(wxT("\t"), wxT(""));
	if(l_NullString ==  wxT(""))
	{	

		p_sError = wxT("Identifier should not be Null");

		//SP_MESSAGEBOX(l_sError, wxT("Syntax checking"), wxOK | wxICON_ERROR);
		//SP_LOGERROR(l_sError);
		return false;
	}

	do
	{
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}
	
		if(::isalpha( c ) || c == '_' ) 
		{
			while(true)
			{			
				if(::isalnum(c)|| c == '_')
				{
					l_constant.FromAscii(c);
				}
				else if(::isspace( c ) || m_iChar > m_sString.size())   // far3_4%
				{				
					break;
				}
				else
				{
					//wxString l_sError;
					p_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in the ") << m_sString ;
					//SP_MESSAGEBOX(l_sError, wxT("Syntax checking"), wxOK | wxICON_ERROR);
					//SP_LOGERROR(l_sError);
					return false;
				}

				if(m_iChar <= m_sString.size())
					c = GetChar();
			}
		}
		else if(c == 0)
		{
			break;
		}
		else
		{
			
			//wxString l_sError;
			p_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in the ") << m_sString ;
			//SP_MESSAGEBOX(l_sError, wxT("Syntax checking"), wxOK | wxICON_ERROR);
			//SP_LOGERROR(l_sError);
			return false;
		}
	} while(m_iChar <= m_sString.size());		


	return true;
}









bool SP_CPN_ColorProcessing::DecomposeIntegerDef(wxString p_sColorString, vector<wxString> &p_vResult)
{

	wxString l_sString = p_sColorString;
	vector<wxString>* l_pvResult = &p_vResult;


	// Identify if there is or lack of a '[' or ']'
	int l_nLeftBracket = l_sString.Freq('[');
	int l_nRightBracket = l_sString.Freq(']');

	//allow no [], for example, 1-7
	if(l_nLeftBracket == 0 && l_nRightBracket == 0)
	{
		l_pvResult->push_back(l_sString);
		return true;
	}
	else if(l_nLeftBracket != l_nRightBracket)
	{
		wxString l_sError;
		if(l_nLeftBracket > l_nRightBracket)
			l_sError << wxT("Lack of ']' ") << wxT(" in integer colorset definition: ") << l_sString;
		else
			l_sError << wxT("Lack of '[' ") << wxT(" in integer colorset definition: ") << l_sString;

		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
		return false;

	}
	else
	{
		wxString l_sProcessingString = l_sString; 
		wxString  l_sMiddleString;
		
		wxString  l_sThirdString = wxT("");

		while(l_sProcessingString.Freq('[') != 0)   // [1,200,45,550][1-5]
		{
			l_sMiddleString = l_sProcessingString;
			l_sMiddleString = l_sMiddleString.BeforeFirst(wxT('[')); // It does not work.
			l_sMiddleString.Replace(wxT(" "), wxT(""));
			l_sMiddleString.Replace(wxT("\n"), wxT(""));
			l_sMiddleString.Replace(wxT("\t"), wxT(""));
			if( wxStrcmp(l_sMiddleString, wxT("")) != 0 )			
			{
				wxString l_sError;
				l_sError << wxT("Format error before the first '[' ") << wxT(" in integer colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}

			l_sProcessingString = l_sProcessingString.AfterFirst('[');
			l_sMiddleString = l_sProcessingString;
			l_sMiddleString = l_sMiddleString.BeforeFirst(']');
			l_sThirdString = l_sMiddleString;
			if(l_sThirdString.Freq('['))
			{
				wxString l_sError;
				l_sError << wxT("Format error before the ']' ") << wxT(" in integer colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}

			l_sThirdString.Replace(wxT(" "), wxT(""));
			l_sThirdString.Replace(wxT("\n"), wxT(""));
			l_sThirdString.Replace(wxT("\t"), wxT(""));

		
			if( wxStrcmp(l_sThirdString, wxT("")) == 0 )
			{
				wxString l_sError;
				l_sError << wxT("It should not be empty between '[' and ']' ") << wxT(" in integer colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}
			l_pvResult->push_back(l_sMiddleString);
			l_sProcessingString = l_sProcessingString.AfterFirst(']');			
		}
	}
	return true;
}


bool SP_CPN_ColorProcessing::ProcessingInteger(wxString p_sColorString, vector<wxString> &p_vResult, SP_MapInt2String & p_mResult, bool &p_bFinite)
{
	wxString l_sString = p_sColorString;
	vector<wxString>* l_vResult = &p_vResult;
	l_vResult->clear();

	vector<wxString> l_vComponents;
	
	if( !DecomposeIntegerDef(l_sString,l_vComponents) )
		return false;

	if(l_vComponents.size() == 1 )
	{
		wxString l_sComponent = l_vComponents[0];
		l_sComponent.Replace(wxT(" "), wxT(""));
		l_sComponent.Replace(wxT("\n"), wxT(""));
		l_sComponent.Replace(wxT("\t"), wxT(""));
		if( wxStrcmp(l_sComponent, wxT("")) == 0 )
		{
			// infinite colorset
			p_bFinite = false;
			return true;
		}

	}
	for(unsigned i = 0; i < l_vComponents.size(); i++)
	{
		if(l_vComponents[i].Freq('-') != 0)
		{
			//deal case '-'
			int l_nMinValue;
			wxString l_sMaxValue;
			if( !GetIntegerToken_Dash( l_vComponents[i], l_nMinValue, l_sMaxValue))
				return false;
			p_mResult[l_nMinValue] = l_sMaxValue;
		}
		else
		{
			//deal case ',' or only one color case
			vector<wxString> l_vTokens;
			if( !GetIntegerToken_Comma(l_vComponents[i],l_vTokens) )
				return false;
			for(unsigned j = 0; j < l_vTokens.size(); j++)
				l_vResult->push_back(l_vTokens[j]);
			
		}
	}


	return true;
}


bool  SP_CPN_ColorProcessing::GetIntegerToken_Dash(wxString p_sParseString, int &p_nMinValue, wxString &p_sMaxValue)
{
	
	m_sString = p_sParseString;		
	vector<wxString> l_sStoredString;
	wxString l_sResult = wxT("");
	int c = m_peekc;
	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle;					
					l_sMiddle = wxString::Format(wxT("%c"), c);	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == '-' || m_iChar >= m_sString.size())
				{
					l_sStoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sError;
					l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in integer colorset definition: ") << m_sString;
					SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases

		else if( c == '-' ) 
		{
			l_sStoredString.push_back(wxT("-"));
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}
		else
		{
			wxString l_sError;
			l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in integer colorset definition:") << m_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
			return false;
		}		
	} while(m_iChar <= m_sString.size());

	//check
	if(l_sStoredString.size() != 3)
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in integer colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
		return false;
	}

	wxString l_sString = l_sStoredString[0];
	long l_nMin;

	if( !l_sString.ToLong(&l_nMin) )
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in integer colorset definition:")<< m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
		return false;
	}
	p_nMinValue = l_nMin;

	if(l_sStoredString[1] != wxT("-"))
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in integer colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
		return false;
	}	

	if(l_sStoredString[2] == wxT("-"))
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in integer colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
		return false;
	}

	//////////////////////////////////////////////////////////
	
	p_sMaxValue = l_sStoredString[2];
	
	return true;
}


bool  SP_CPN_ColorProcessing::GetIntegerToken_Comma(wxString p_sParseString, vector<wxString> & p_vStoredString)
{
	m_sString = p_sParseString;  //m_sString is global class variable
	p_vStoredString.clear();
	int c = m_peekc;
	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}
		//deal with digit
		if( ::isdigit( c ) )
		{
			double v = 0;
			while( true ) 
			{
				if ( ::isdigit( c ) ) 
				{
					v = v* 10 + (c - '0');				
				} 
				else if(::isspace( c ) || c == ',' || m_iChar >= m_sString.size())
				{					
					wxString l_s;
					l_s<<v;
					p_vStoredString.push_back(l_s);
					break;
				}
				else
				{
					wxString l_sError;					
					l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in integer colorset definition: ") << m_sString;
					SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
					return false;
				}
				c = GetChar();
			}
		}

		//deal with other cases
		else if( c == ',' ) 
		{
			//p_StoredString.push_back(wxT(","));
			c = GetChar();
		}		
		
		else if(c == 0)
		{
			break;
		}
		else
		{
			wxString l_sError;
			l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in integer colorset definition: ") << m_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());
	
	return true;
}


bool SP_CPN_ColorProcessing::ProcessingIntegerSubset(wxString p_sColorString, vector<wxString> &p_vResult, wxString &p_sPredicate)
{

	
	wxString l_sString = p_sColorString;
	vector<wxString>* l_vResult = &p_vResult;
	l_vResult->clear();
	
	wxString l_sOtherStr;

	vector<wxString> l_vComponents;

	//Get the predicate
	int l_nLeftBracket = l_sString.Freq('{');
	int l_nRightBracket = l_sString.Freq('}');

	if(l_nLeftBracket == 0 && l_nRightBracket == 0)
	{	
		l_sOtherStr = l_sString;
		
	}
	else if(l_nLeftBracket == 1 && l_nRightBracket == 1)
	{
		if( l_sString.Find(wxT('{')) > l_sString.Find(wxT('}')) )
		{
			wxString l_sError;
			l_sError<<  wxT(" Format error in integer colorset definition: ") << l_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);	
			return false;
		}
		wxString l_sStr = l_sString.AfterFirst(wxT('}'));
		l_sStr.Replace(wxT(" "), wxT(""));
		l_sStr.Replace(wxT("\n"), wxT(""));
		l_sStr.Replace(wxT("\t"), wxT(""));
		if(l_sStr != wxT(""))
		{
			wxString l_sError;
			l_sError <<  wxT(" Format error in integer colorset definition: ") << l_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);	
			return false;
		}
		l_sOtherStr = l_sString.BeforeFirst(wxT('{'));
		wxString l_sPre = l_sString.AfterFirst(wxT('{'));		
		l_sPre = l_sPre.BeforeFirst(wxT('}'));
		p_sPredicate = l_sPre;

	}
	else
	{
		wxString l_sError;
		if(l_nLeftBracket > l_nRightBracket)
			l_sError << wxT("Lack of '}' ") << wxT(" in integer colorset definition: ") << l_sString;
		else
			l_sError << wxT("Lack of '{' ") << wxT(" in integer colorset definition: ") << l_sString;

		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
		return false;
	}

	//
	wxString l_sEmptyCheck = l_sOtherStr;
	l_sEmptyCheck.Replace(wxT(" "), wxT(""));
	l_sEmptyCheck.Replace(wxT("\n"), wxT(""));
	l_sEmptyCheck.Replace(wxT("\t"), wxT(""));
	if(l_sEmptyCheck == wxT(""))
		return true;	
	
	if( !DecomposeIntegerDef(l_sOtherStr,l_vComponents) )
		return false;

	if(l_vComponents.size() == 1 )
	{
		wxString l_sComponent = l_vComponents[0];
		l_sComponent.Replace(wxT(" "), wxT(""));
		l_sComponent.Replace(wxT("\n"), wxT(""));
		l_sComponent.Replace(wxT("\t"), wxT(""));

		if( wxStrcmp(l_sComponent, wxT("")) == 0 )
		{
			return true;
		}

	}
	for(unsigned i = 0; i < l_vComponents.size(); i++)
	{
		if(l_vComponents[i].Freq('-') != 0)
		{
			//deal case '-'
			int l_nMinValue;
			wxString l_sMaxValue;
			if( !GetIntegerToken_Dash( l_vComponents[i], l_nMinValue, l_sMaxValue))
				return false;
			//l_mResult[l_nMinValue] = l_sMaxValue;
			long l_nMax;
			if( !l_sMaxValue.ToLong(&l_nMax))
			{
				wxString l_sError;
				l_sError <<  wxT(" Format error in integer colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);	
				return false;
			}
			if(l_nMinValue > l_nMax )
			{
				wxString l_sError;
				l_sError <<  wxT(" Format error in integer colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);	
				return false;
			}
			for(int i = l_nMinValue; i <= l_nMax; i++ )
			{
				wxString l_sValue = wxString::Format(wxT("%d"),i);
				l_vResult->push_back(l_sValue);
			}

		}
		else
		{
			//deal case ',' or only one color case
			vector<wxString> l_vTokens;
			if( !GetIntegerToken_Comma(l_vComponents[i],l_vTokens) )
				return false;
			for(unsigned j = 0; j < l_vTokens.size(); j++)
				l_vResult->push_back(l_vTokens[j]);			
		}
	}

	return true;
}









///for string

bool SP_CPN_ColorProcessing::DecomposeStringDef(wxString p_sColorString, vector<wxString> &p_vResult)
{

	wxString l_sString = p_sColorString;
	vector<wxString>* l_pvResult = &p_vResult;


	// Identify if there is or lack of a '[' or ']'
	int l_nLeftBracket = l_sString.Freq('[');
	int l_nRightBracket = l_sString.Freq(']');

	//allow no [], for example, 1-7
	if(l_nLeftBracket == 0 && l_nRightBracket == 0)
	{
		l_pvResult->push_back(l_sString);
		return true;
	}
	else if(l_nLeftBracket != l_nRightBracket)
	{
		wxString l_sError;
		if(l_nLeftBracket > l_nRightBracket)
			l_sError << wxT("Lack of ']' ") << wxT(" in string colorset definition: ") << l_sString;
		else
			l_sError << wxT("Lack of '[' ") << wxT(" in string colorset definition: ") << l_sString;

		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
		return false;

	}
	else
	{
		wxString l_sProcessingString = l_sString; 
		wxString  l_sMiddleString;
		
		wxString  l_sThirdString = wxT("");

		while(l_sProcessingString.Freq('[') != 0)   // [1,200,45,550][1-5]
		{
			l_sMiddleString = l_sProcessingString;
			l_sMiddleString = l_sMiddleString.BeforeFirst(wxT('[')); // It does not work.
			l_sMiddleString.Replace(wxT(" "), wxT(""));
			l_sMiddleString.Replace(wxT("\n"), wxT(""));
			l_sMiddleString.Replace(wxT("\t"), wxT(""));
			if( wxStrcmp(l_sMiddleString, wxT("")) != 0 )			
			{
				wxString l_sError;
				l_sError << wxT("Format error before the first '[' ") << wxT(" in string colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}

			l_sProcessingString = l_sProcessingString.AfterFirst('[');
			l_sMiddleString = l_sProcessingString;
			l_sMiddleString = l_sMiddleString.BeforeFirst(']');
			l_sThirdString = l_sMiddleString;
			if(l_sThirdString.Freq('['))
			{
				wxString l_sError;
				l_sError << wxT("Format error before the ']' ") << wxT(" in string colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}

			l_sThirdString.Replace(wxT(" "), wxT(""));
			l_sThirdString.Replace(wxT("\n"), wxT(""));
			l_sThirdString.Replace(wxT("\t"), wxT(""));

		
			if( wxStrcmp(l_sThirdString, wxT("")) == 0 )
			{
				wxString l_sError;
				l_sError << wxT("It should not be empty between '[' and ']' ") << wxT(" in string colorset definition: ") << l_sString;
				SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
				return false;
			}
			l_pvResult->push_back(l_sMiddleString);
			l_sProcessingString = l_sProcessingString.AfterFirst(']');			
		}
	}
	return true;
}


bool  SP_CPN_ColorProcessing::GetTokenforString_Comma(wxString & p_ParseString, vector<wxString> &p_StoredString)
{
	m_sString = p_ParseString;		
	p_StoredString.clear();
	wxString l_sResult = wxT("");
	int c = m_peekc;
	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle = wxT("");					
					l_sMiddle = wxString::Format(wxT("%c"), c);	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == ',' || m_iChar >= m_sString.size())
				{
					p_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sError;
					l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in ") << m_sString;
					SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
					return false;
				}
				c = GetChar();
			}
		}

		//deal with other cases
		else if( c == ',' ) 
		{			
			c = GetChar();
		}		
		else if(c == 0)
		{
			break;
		}
				
		else
		{
			wxString l_sError;
			l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in ") << m_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());

	return true;
}

bool  SP_CPN_ColorProcessing::GetTokenforString_Dash(wxString p_sParseString, wxString &p_sMinValue, wxString &p_sMaxValue)
{
	m_sString = p_sParseString;		
	vector<wxString> l_StoredString;
	wxString l_sResult = wxT("");
	int c = m_peekc;
	m_iChar = 0;

	do
	{		
		//deal with space
		while( ::isspace( c ) ) 
		{
			c = GetChar();
		}

		//deal with digit or letter
		if( ::isalnum( c ) || c == '_' )
		{
			while( true ) 
			{
				if (::isalnum( c ) || c == '_') 
				{
					wxString l_sMiddle = wxT("");
					l_sMiddle = wxString::Format(wxT("%c"), c);	
					l_sResult << l_sMiddle;
				} 
				else if(::isspace( c ) || c == '-' || m_iChar >= m_sString.size())
				{
					l_StoredString.push_back(l_sResult);
					l_sResult = wxT("");
					break;
				}
				else
				{
					wxString l_sError;
					l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in ") << m_sString;
					SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
					return false;
				}

				c = GetChar();
			}
		}

		//deal with other cases		
		else if( c == '-' ) 
		{
			l_StoredString.push_back(wxT("-"));
			c = GetChar();
		}
		else if(c == 0)
		{
			break;
		}
				
		else
		{
			wxString l_sError;
			l_sError << wxT("Invalid symbol: ") << wxString::Format(wxT("%c"), c) << wxT(" in ") << m_sString;
			SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
			return false;
		}		
		
	} while(m_iChar <= m_sString.size());

	
	//check
	if(l_StoredString.size() != 3)
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in string colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);					
		return false;
	}
	if(l_StoredString[1] != wxT("-"))
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in integer colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
		return false;
	}	

	if(l_StoredString[0] == wxT("-") || l_StoredString[2] == wxT("-"))
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in string colorset definition:") << m_sString;
		SP_MESSAGEBOX(l_sError, wxT("Colorset syntax checking"), wxOK | wxICON_ERROR);
		return false;
	}

	//////////////////////////////////////////////////////////	
	p_sMinValue = l_StoredString[0];
	p_sMaxValue = l_StoredString[2];

	return true;
}








	
bool SP_CPN_ColorProcessing::GetConstant(map<wxString, SP_CPN_Variable_Constant> &m_mConstant)
{

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		SP_CPN_Variable_Constant l_cConstant;

		wxString l_sConstantName = l_pcColList->GetCell(i,0).c_str();		
		l_sConstantName.Replace(wxT(" "), wxT(""));
		l_sConstantName.Replace(wxT("\n"), wxT(""));
		l_sConstantName.Replace(wxT("\t"), wxT(""));

		wxString l_sConstantType = l_pcColList->GetCell(i,1).c_str();		
		
		wxString l_sConstantValue = l_pcColList->GetCell(i,2).c_str();

		l_sConstantValue.Replace(wxT(" "), wxT(""));
		l_sConstantValue.Replace(wxT("\n"), wxT(""));
		l_sConstantValue.Replace(wxT("\t"), wxT(""));

		if(l_sConstantType == wxT("int"))
		{
			l_cConstant.m_DataType = CPN_INTEGER;
			long l_nValue;
			if(l_sConstantValue.ToLong(&l_nValue))
			{
				l_cConstant.m_IntegerValue = l_nValue;
				m_mConstant[l_sConstantName] = l_cConstant;
			}			
		}	

	}


	return true;
}

bool SP_CPN_ColorProcessing::ProcessingBool(wxString & p_ParseString, vector<wxString> &p_vResult)
{
	wxString l_sParseString = p_ParseString;
	l_sParseString.Replace(wxT(" "), wxT(""));
	l_sParseString.Replace(wxT("\n"), wxT(""));
	l_sParseString.Replace(wxT("\t"), wxT(""));

	wxString l_sFirst  = l_sParseString.BeforeFirst(wxT(','));
	wxString l_sSecond = l_sParseString.AfterFirst(wxT(','));

	if( ! (l_sFirst == wxT("true") || l_sFirst == wxT("false") ) )
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in Boolean colorset definition:") << p_ParseString;
		SP_LOGERROR(l_sError);
		return false;
	}
	if( ! (l_sSecond == wxT("true") || l_sSecond == wxT("false") ) )
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in Boolean colorset definition:") << p_ParseString;
		SP_LOGERROR(l_sError);
		return false;
	}
	if( l_sFirst == l_sSecond)
	{
		wxString l_sError;
		l_sError << wxT("Format error: ") << wxT(" in Boolean colorset definition:") << p_ParseString;
		SP_LOGERROR(l_sError);
		return false;
	}

	p_vResult.push_back(l_sFirst);
	p_vResult.push_back(l_sSecond);

	return true;
}
