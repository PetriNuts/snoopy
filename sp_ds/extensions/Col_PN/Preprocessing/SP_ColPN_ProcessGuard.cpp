//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessGuard.h"



SP_ColPN_ProcessGuard::SP_ColPN_ProcessGuard()
{

}

SP_ColPN_ProcessGuard::~SP_ColPN_ProcessGuard()
{	
}

void SP_ColPN_ProcessGuard::ParseGuard(wxString p_sGuardExpr)
{
	unsigned l_iChar = 0;
    int l_nLevel =0;     

    wxChar l_Char = p_sGuardExpr[l_iChar];
    int l_MarkPos = l_iChar;

    while( l_iChar < p_sGuardExpr.size() )
    {
        if( l_Char == '(')
            l_nLevel++;
        else if( l_Char == ')')
            l_nLevel--;
        else if(l_Char == '&' && l_nLevel == 0 )
        {
            wxString l_SubString = p_sGuardExpr.substr(l_MarkPos,l_iChar - l_MarkPos);           
            m_vConjunctions.push_back(l_SubString);
            l_MarkPos = l_iChar+1;                 
        }
		else if(l_Char == '|' && l_nLevel == 0 )
		{
			m_vConjunctions.clear();
			return;
		}

        l_Char = p_sGuardExpr[++l_iChar];

        if(l_iChar == p_sGuardExpr.size() -1 )
        {
            wxString l_SubString = p_sGuardExpr.substr(l_MarkPos,p_sGuardExpr.size() - l_MarkPos); 
            m_vConjunctions.push_back(l_SubString);     
        }
    }	
}


void SP_ColPN_ProcessGuard::InitializeParsing()
{
	m_ExpressionVector.clear();

	//if a guard can not divided into at least two components, we do not process it in the following way.
	if(m_vConjunctions.size() < 2)
	{		
		return;
	}

	//initialize	
	for(unsigned int i= 0; i < m_vConjunctions.size(); i++)
	{
		SP_CPN_Expr_Guard_Conjunct l_Expression;
		//l_Expression.m_eExprType = CPN_GUARD_EXPR;
		l_Expression.m_sExpression = m_vConjunctions[i];
		m_ExpressionVector.push_back(l_Expression);
	}

	SP_CPN_ValueAssign l_ValueAssign;
	SP_CPN_ColorSetClass l_ColorSetClass;
	if( !l_ValueAssign.InitializeColorset(l_ColorSetClass) )
		return;
	
	for(unsigned int i= 0; i < m_ExpressionVector.size(); i++)
	{
		wxString l_sExpr= m_ExpressionVector[i].m_sExpression;
	
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);
		m_pParseContext->SetColorSetClass(&l_ColorSetClass);
		m_pParseContext->SetFunctionFlag(false); 
		//m_pParseContext->SetNetClassName(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName()); 
		m_pParseContext->SetPlaceType(wxT("Place"));

		m_pParseContext->SetColorSetName(wxT(""));
		m_pParseContext->SetErrorPosition(wxT(""));
		

		std::string l_ParseString;
		l_ParseString = string( m_ExpressionVector[i].m_sExpression.mb_str());
		bool l_bEval = m_pDriver->parse_string(l_ParseString, "input");	

		if(!l_bEval)
		{
			wxString l_sParseError  = m_pDriver->GetError();	
			wxString l_sError = wxT(" in the expression: ") + m_ExpressionVector[i].m_sExpression  ;
			l_sError = l_sParseError + l_sError;			
			SP_LOGERROR(l_sError);	
			m_ExpressionVector.clear();
			return;
		}

		// call the check function
		if( !m_pParseContext->CheckExpression())
		{
			m_ExpressionVector.clear();
			return;
		}		


		vector<SP_CPN_Var_Color>   l_vVariable2Color;
		l_vVariable2Color = m_pParseContext->GetVariables();
		
		m_ExpressionVector[i].m_pParseContext = m_pParseContext;
		m_ExpressionVector[i].m_pDriver = m_pDriver;
		m_ExpressionVector[i].m_vVariable2Color = l_vVariable2Color;

	}

}

void SP_ColPN_ProcessGuard::EvaluateGuard(map<wxString,unsigned> p_mBoundVariables,SP_MapString2UInt p_IndexMap,vector<vector<wxString> > &p_CompleteBinding,SP_CPN_ColorSetClass *p_pcColorSetClass)
{
	if(m_ExpressionVector.size() == 0)
		return;


	vector<int> l_vDeleteIndex;
	vector<int> l_DeleteExpr;
	for(unsigned int i= 0; i < m_ExpressionVector.size(); i++)
	{
		vector<SP_CPN_Var_Color> l_vVar_Col = m_ExpressionVector[i].m_vVariable2Color;
		//SP_MapString2String l_VariableMap = m_ExpressionVector[i].m_VariableMap;
		bool l_bBound = true;
		//SP_MapString2String::iterator itMap;
		//for(itMap = l_VariableMap.begin(); itMap != l_VariableMap.end(); itMap++)
		for(unsigned j = 0; j < l_vVar_Col.size(); j++)
		{
			if(p_mBoundVariables.find(l_vVar_Col[j].m_sVariable) == p_mBoundVariables.end())
			{
				l_bBound = false;
				break;
			}
		}
		if(l_bBound)
		{
			l_DeleteExpr.push_back(i);
			for( unsigned j = 0; j < p_CompleteBinding.size(); j++)
			{
				SP_MapString2UInt::iterator itSUMap;
				for(itSUMap = p_mBoundVariables.begin(); itSUMap != p_mBoundVariables.end(); itSUMap++)
				{
					if( ! p_pcColorSetClass->SetVariableValue(itSUMap->first, p_CompleteBinding[j][itSUMap->second]) )	
						return;
				}
			
				vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;

				if( ! m_ExpressionVector[i].m_pParseContext->EvaluateExpression( l_EvaluateResultArray) )
					return;
				
				if( l_EvaluateResultArray.begin()->m_ColorValue != wxT("true") )
				{
					l_vDeleteIndex.push_back(j);
				}
			}
		}
	}

	//deleter
	std::sort(l_DeleteExpr.begin(),l_DeleteExpr.end());
	for(unsigned k = l_DeleteExpr.size()-1; k < l_DeleteExpr.size(); k--)
	{
		m_ExpressionVector.erase(m_ExpressionVector.begin()+l_DeleteExpr[k]);
	}


	if(l_vDeleteIndex.size() == 0)
		return;

	std::sort(l_vDeleteIndex.begin(),l_vDeleteIndex.end());

	vector<int>::iterator it;
	it = std::unique(l_vDeleteIndex.begin(), l_vDeleteIndex.end());
	l_vDeleteIndex.resize( it - l_vDeleteIndex.begin() ); 

	for(unsigned k = l_vDeleteIndex.size()-1; k < l_vDeleteIndex.size(); k--)
	{
		p_CompleteBinding.erase(p_CompleteBinding.begin()+l_vDeleteIndex[k]);
	}
	
	
}

