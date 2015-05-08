//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Steffen Laarz $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/07/31 12:37:00 $
// Short Description: to handle expression and constants for edges
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/SP_DS_Graph.h"

SP_DS_MarkingDependentMultiplicity::SP_DS_MarkingDependentMultiplicity(const wxString& p_pchName, const wxString& p_pchVal)
:SP_DS_MultiplicityAttribute(p_pchName, p_pchVal)
{
	SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
}

SP_DS_MarkingDependentMultiplicity::SP_DS_MarkingDependentMultiplicity(const wxString& p_pchName, long p_nVal)
:SP_DS_MultiplicityAttribute(p_pchName, p_nVal)
{
	SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY);
}


SP_DS_Attribute*
SP_DS_MarkingDependentMultiplicity::Clone(bool p_bCloneGr)
{
	SP_DS_MarkingDependentMultiplicity* l_pcAttr = new SP_DS_MarkingDependentMultiplicity(GetName(), GetValueString());

	// just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_MarkingDependentMultiplicity::UpdateValue()
{
	long val = 0;
	if(m_Function->isValue())
	{
		val = m_Function->getValue();
	}
	else
	{
		m_sValueStandard.ToLong(&val);
		SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();
		SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
		SP_DS_Node* l_pcTrans = NULL;
		SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(GetParent());
		if(l_pcEdge)
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
			if(l_pcTrans && !l_pcTrans->GetClassName().Contains(SP_DS_DISCRETE_TRANS))
			{
				l_pcTrans = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
			}
		}
		val = SP_DS_FunctionEvaluatorLong{l_pcFR, m_Function, val}(l_pcTrans);
	}
	if ( val < 0 )
	{
//		SP_MESSAGEBOX(wxT("negative value not allowed"),	wxT("Error"), wxOK | wxICON_ERROR);
		SP_LOGERROR( wxT("negative value not allowed"));
		m_sValueStandard.ToLong(&val);
	}
	return SetValue(val);
}

bool
SP_DS_MarkingDependentMultiplicity::CheckIntegrity()
{
	long l_Int = 0;
	double l_Double = 0;
	if (!m_sValue.ToLong(&l_Int) && m_sValue.ToDouble(&l_Double))
	{
		SP_LOGERROR( wxT("\"") + m_sValue + wxT("\" double value not allowed"));
		return false;
	}

	SP_DS_Node* l_pcTrans = NULL;
	SP_DS_Edge* l_pcEdge = dynamic_cast<SP_DS_Edge*>(GetParent());
	if(l_pcEdge)
	{
		l_pcTrans = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
		if(l_pcTrans && !l_pcTrans->GetClassName().Contains(SP_DS_DISCRETE_TRANS))
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
		}
	}
	if (!parseString(m_sValue, l_pcTrans))
	{
		SP_LOGERROR( wxT("\"") + m_sValue + wxT("\" value not allowed"));
		return false;
	}
	return SP_DS_ExpressionAttribute::CheckIntegrity();
}
