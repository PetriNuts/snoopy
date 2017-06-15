//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Steffen Laarz $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/07/31 12:37:00 $
// Short Description: to handle expression and constants for cpn edges
//////////////////////////////////////////////////////////////////////
#include <dssd/auxi/auxi.h>
#include "sp_ds/attributes/SP_DS_DoubleMarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"


SP_DS_DoubleMarkingDependentMultiplicity::SP_DS_DoubleMarkingDependentMultiplicity(const wxString& p_pchName,
                                        const wxString& p_pchVal)
:SP_DS_ExpressionAttribute(p_pchName, p_pchVal, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY)
{
}

SP_DS_DoubleMarkingDependentMultiplicity::SP_DS_DoubleMarkingDependentMultiplicity(const wxString& p_pchName, double p_nVal)
:SP_DS_ExpressionAttribute(p_pchName, dssd::aux::toString(p_nVal), SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKINGDEPENDETMULTIPLICITY), m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_DoubleMarkingDependentMultiplicity::Clone(bool p_bCloneGr)
{
	SP_DS_DoubleMarkingDependentMultiplicity* l_pcAttr = new SP_DS_DoubleMarkingDependentMultiplicity(GetName(), GetValueString());

	// just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}


/*
 *	set the given string value in numeric value
 */
bool
SP_DS_DoubleMarkingDependentMultiplicity::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	wxString l_sOld = p_pchVal != m_sValue ? m_sValue : m_sValueStandard;
	m_sValue = p_pchVal;

	if(p_bCheckValue)
	{
		if (!CheckIntegrity())
		{
			m_sValue = l_sOld;
		}
		else
		{
			UpdateValue();
		}
	}

	return true;
}

bool
SP_DS_DoubleMarkingDependentMultiplicity::SetValue(double l_nValue)
{
	m_nValue = l_nValue;
	return true;
}

double
SP_DS_DoubleMarkingDependentMultiplicity::GetValue(bool p_bUpdate)
{
    if(!m_Function)
    {
        SetValueString(m_sValue);
    }
    else if(p_bUpdate)
    {
    	UpdateValue();
    }
	return m_nValue;
}


wxString
SP_DS_DoubleMarkingDependentMultiplicity::GetValueString()
{
	return m_sValue;
}

bool
SP_DS_DoubleMarkingDependentMultiplicity::UpdateValue()
{
	double val = 0;
	if(m_Function->isValue())
	{
		val = m_Function->getValue();
	}
	else
	{
		m_sValueStandard.ToDouble(&val);
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
		val = SP_DS_FunctionEvaluatorDouble{l_pcFR, m_Function, val}(l_pcTrans);
	}
	if ( val < 0 )
	{
//		SP_MESSAGEBOX(wxT("negative value not allowed"),	wxT("Error"), wxOK | wxICON_ERROR);
		SP_LOGERROR( wxT("negative value not allowed"));
		m_sValueStandard.ToDouble(&val);
	}
	return SetValue(val);
}

bool
SP_DS_DoubleMarkingDependentMultiplicity::CheckIntegrity()
{
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

