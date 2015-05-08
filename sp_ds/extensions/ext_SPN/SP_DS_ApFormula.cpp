/*
 * SP_DS_ApFormula.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */

#include "SP_DS_ApFormula.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

SP_DS_ApFormula::SP_DS_ApFormula(SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree):
SP_DS_Formula(p_pcGraph, p_ExpressionTree),
m_CurrentMarking(NULL)
{
	Initialise();
}

SP_DS_ApFormula::SP_DS_ApFormula(SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree):
SP_DS_Formula(p_pcGraph, p_ExpressionTree),
m_CurrentMarking(NULL)
{
	m_pcUnfoldedNet = p_pcUnfoldedNet;
	Initialise();
}




SP_DS_ApFormula::~SP_DS_ApFormula()
{

}

bool
SP_DS_ApFormula::Initialise()
{
	
	if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		LoadUnfoldedPlaces();
	}
	else
	{
		LoadPlaces();
	}
	
	LoadParameters();

	return true;
}

void
SP_DS_ApFormula::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Place") );

	m_Id2Placename.resize(l_pcNodeclass->GetElements()->size());
	m_Placename2Id.clear();

	unsigned l_nPos = 0;

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Placename2Id[l_sPlaceName] = l_nPos;
		m_Id2Placename[l_nPos] = l_sPlaceName;

		++l_nPos;
	}
}

void
SP_DS_ApFormula::LoadParameters()
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_ListMetadata::const_iterator it;


	m_Parametername2Id.clear();
	m_Id2Parametername.resize(mc->GetElements()->size());
	m_Parameter.resize(mc->GetElements()->size());
	unsigned l_nPos = 0;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if(l_FE.IsOk())
		{
			double l_nValue = 0;
			SP_FunctionPtr l_Function = l_FE.getFunction();
			if (l_Function->isValue())
			{
				l_nValue = l_Function->getValue();
			}
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if(l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{l_pcFR, l_Function}();
				}
				else if(l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{l_pcFR, l_Function}();
				}
			}
			//****************************************************************************************
			m_Parametername2Id[l_sName] = l_nPos;
			m_Id2Parametername[l_nPos] = l_sName;

			m_Parameter[l_nPos] = l_nValue;
			++l_nPos;
		}

	}





	/*
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Parameter") );

	m_Parametername2Id.clear();
	m_Id2Parametername.resize(l_pcNodeclass->GetElements()->size());
	m_Parameter.resize(l_pcNodeclass->GetElements()->size());
	unsigned l_nPos = 0;

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		m_Parametername2Id[l_sParameterName] = l_nPos;
		m_Id2Parametername[l_nPos] = l_sParameterName;

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( (*l_itElem)->GetAttribute( wxT("ParameterList") ) );
		double l_nDouble = 0;
		l_pcColList->GetActiveCellValue( 1).ToDouble( &l_nDouble );

		m_Parameter[l_nPos] = l_nDouble;

		++l_nPos;
	}
	*/
}

double
SP_DS_ApFormula::Evaluate(const SP_VectorLong* p_Marking)
{
	m_bPostEval = false;
	if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		m_NewMarking = *p_Marking;
		GetNewMarking();
	}
	else
	{
		m_CurrentMarking = p_Marking;
	}	

	return Eval(m_Expression);
}

double
SP_DS_ApFormula::Eval(const rfdag::RFDAG& p_Expression)
{
	double l_nReturn;
	switch(p_Expression.token)
	{
	case rfdag::DOUBLE:
		l_nReturn = p_Expression.value;
		break;
	case rfdag::IDENT:
	case rfdag::PLACE:
		if(m_bPostEval)
			l_nReturn = (*m_PostCurrentMarking)[p_Expression.placeid];
		else
			l_nReturn = (*m_CurrentMarking)[p_Expression.placeid];
		break;
	case rfdag::PARAMETER:
		l_nReturn = m_Parameter[p_Expression.placeid];
		break;
	case rfdag::ADD:
		l_nReturn = Eval(p_Expression.children[0]) + Eval(p_Expression.children[1]);
		break;
	case rfdag::MUL:
		l_nReturn = Eval(p_Expression.children[0]) * Eval(p_Expression.children[1]);
		break;
	case rfdag::DIV:
		l_nReturn = Eval(p_Expression.children[0]) / Eval(p_Expression.children[1]);
		break;
	case rfdag::MINUS:
		l_nReturn = Eval(p_Expression.children[0]) - Eval(p_Expression.children[1]);
		break;
	case rfdag::POW:
		l_nReturn = pow(Eval(p_Expression.children[0]), Eval(p_Expression.children[1]));
		break;
	case rfdag::SQR:
		l_nReturn = pow(Eval(p_Expression.children[0]), 2.0);
		break;
	case rfdag::SQRT:
		l_nReturn = sqrt(Eval(p_Expression.children[0]));
		break;
	case rfdag::SIN:
		l_nReturn = sin(Eval(p_Expression.children[0]));
		break;
	case rfdag::ASIN:
		l_nReturn = asin(Eval(p_Expression.children[0]));
		break;
	case rfdag::ABS:
		l_nReturn = fabs(Eval(p_Expression.children[0]));
		break;
	case rfdag::CEIL:
		l_nReturn = ceil(Eval(p_Expression.children[0]));
		break;
	case rfdag::FLOOR:
		l_nReturn = floor(Eval(p_Expression.children[0]));
		break;
	case rfdag::COS:
		l_nReturn = cos(Eval(p_Expression.children[0]));
		break;
	case rfdag::ACOS:
		l_nReturn = acos(Eval(p_Expression.children[0]));
		break;
	case rfdag::TAN:
		l_nReturn = tan(Eval(p_Expression.children[0]));
		break;
	case rfdag::ATAN:
		l_nReturn = atan(Eval(p_Expression.children[0]));
		break;
	case rfdag::LOG:
		l_nReturn = log(Eval(p_Expression.children[0]));
		break;
	case rfdag::LOG10:
		l_nReturn = log10(Eval(p_Expression.children[0]));
		break;
	case rfdag::EQ:
		l_nReturn = Eval(p_Expression.children[0]) == Eval(p_Expression.children[1]);
		break;
	case rfdag::NEQ:
		l_nReturn = Eval(p_Expression.children[0]) != Eval(p_Expression.children[1]);
		break;
	case rfdag::LEQ:
		l_nReturn = Eval(p_Expression.children[0]) <= Eval(p_Expression.children[1]);
		break;
	case rfdag::GEQ:
		l_nReturn = Eval(p_Expression.children[0]) >= Eval(p_Expression.children[1]);
		break;
	case rfdag::LESS:
		l_nReturn = Eval(p_Expression.children[0]) < Eval(p_Expression.children[1]);
		break;
	case rfdag::GREAT:
		l_nReturn = Eval(p_Expression.children[0]) > Eval(p_Expression.children[1]);
		break;
	case rfdag::NOT:
		l_nReturn = !Eval(p_Expression.children[0]);
		break;
	case rfdag::AND:
		l_nReturn = Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]);
		break;
	case rfdag::OR:
		l_nReturn = Eval(p_Expression.children[0]) || Eval(p_Expression.children[1]);
		break;
	case rfdag::XOR:
		l_nReturn = (Eval(p_Expression.children[0]) && !Eval(p_Expression.children[1]))
				|| (!Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]));
		break;
	case rfdag::IF:
		l_nReturn = !Eval(p_Expression.children[0]) || Eval(p_Expression.children[1]);
		break;
	case rfdag::IFB:
		l_nReturn = Eval(p_Expression.children[0]) || !Eval(p_Expression.children[1]);
		break;
	case rfdag::IFF:
		l_nReturn = (Eval(p_Expression.children[0]) && Eval(p_Expression.children[1]))
				|| (!Eval(p_Expression.children[0]) && !Eval(p_Expression.children[1]));
		break;
	default:
		SP_LOGERROR( wxT("Error while evaluating ") + wxString(p_Expression.toString().c_str(), wxConvUTF8));
		l_nReturn = 0;
		break;
	}
	return l_nReturn;
}

void SP_DS_ApFormula::LoadUnfoldedPlaces()
{
	m_Placename2Id = *( m_pcUnfoldedNet->GetPlaceArrayPos() );
	m_Id2Placename = *( m_pcUnfoldedNet->GetPlaceNames() );

	//////////////////////
	
	m_msColoredPlaceNames =  *(m_pcUnfoldedNet->GetColoredPlaceNames());	
	m_pnsPlaceCountById = *(m_pcUnfoldedNet->GetPlaceCountById());

	
	for(unsigned i=0; i < m_msColoredPlaceNames.size(); i++)
	{
		m_Id2Placename.push_back( m_msColoredPlaceNames[i] );
		m_Placename2Id[m_msColoredPlaceNames[i]] = m_Id2Placename.size()-1;
	}
	


}

void SP_DS_ApFormula::GetNewMarking()
{	
	
	SP_VectorLong m_TempMarking = m_NewMarking;

	for( unsigned l_nPos = 0; l_nPos < m_msColoredPlaceNames.size(); l_nPos++ )
	{				
		long l_nNewToken = 0;
		for(unsigned int l_nCol = m_pnsPlaceCountById[l_nPos].m_nLow;  l_nCol <= m_pnsPlaceCountById[l_nPos].m_nUp; l_nCol++)
		{
			l_nNewToken = l_nNewToken +  m_TempMarking[l_nCol];					
		}

		m_NewMarking.push_back( l_nNewToken );					
	}

	m_CurrentMarking = &m_NewMarking;

}



double
SP_DS_ApFormula::PostEvaluate(const SP_VectorDouble* p_Marking)
{
	m_bPostEval = true;
	if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS )
	{
		m_PostNewMarking = *p_Marking;
		GetPostNewMarking();
	}
	else
	{
		m_PostCurrentMarking = p_Marking;
	}	

	return Eval(m_Expression);
	m_bPostEval = false;
}


void SP_DS_ApFormula::GetPostNewMarking()
{	
	
	SP_VectorDouble m_TempMarking = m_PostNewMarking;

	for( unsigned l_nPos = 0; l_nPos < m_msColoredPlaceNames.size(); l_nPos++ )
	{				
		double l_nNewToken = 0;
		for(unsigned int l_nCol = m_pnsPlaceCountById[l_nPos].m_nLow;  l_nCol <= m_pnsPlaceCountById[l_nPos].m_nUp; l_nCol++)
		{
			l_nNewToken = l_nNewToken +  m_TempMarking[l_nCol];					
		}

		m_PostNewMarking.push_back( l_nNewToken );					
	}

	m_PostCurrentMarking = &m_PostNewMarking;

}
