/*
 * SP_DS_MarkingAttribute.cpp
 *
 *  Created on: 05.12.2012
 *      Author: Steffen Laarz
 */

#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

SP_DS_MarkingAttribute::SP_DS_MarkingAttribute(const wxString& p_pchName,
                                         const wxString& p_pchVal)
:SP_DS_ExpressionAttribute(p_pchName, p_pchVal, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)
{
}


SP_DS_MarkingAttribute::SP_DS_MarkingAttribute(const wxString& p_pchName, long p_nVal)
:SP_DS_ExpressionAttribute(p_pchName, wxString::Format(wxT("%li"), p_nVal), SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)
,m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_MarkingAttribute::Clone(bool p_bCloneGr)
{
	SP_DS_MarkingAttribute* l_pcAttr = new SP_DS_MarkingAttribute(GetName(), GetValueString());

	// just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_MarkingAttribute::SetValue(long l_nValue)
{
	m_nValue = l_nValue;
	return true;
}

bool
SP_DS_MarkingAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	wxString l_sOld = p_pchVal != m_sValue ? m_sValue : m_sValueStandard;
	m_sValue = p_pchVal;

	if(p_bCheckValue)
	{
		if (CheckIntegrity())
		{
			UpdateValue();
		}
		else
		{
			m_sValue = l_sOld;
		}
	}

	return true;
}

/*
 * $return: numeric value of attribute
 */
long
SP_DS_MarkingAttribute::GetValue(bool p_bUpdate)
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

/*
 * $return: wxString value of attribute
 */
wxString
SP_DS_MarkingAttribute::GetValueString()
{
	return m_sValue;
}

bool
SP_DS_MarkingAttribute::UpdateValue()
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
		val = SP_DS_FunctionEvaluatorLong{l_pcFR, m_Function, val}();
	}
	if ( val < 0 )
	{
//		SP_MESSAGEBOX(wxT("negative value not allowed"),	wxT("Error"), wxOK | wxICON_ERROR);
		SP_LOGERROR( wxT("negative value not allowed"));
		m_sValueStandard.ToLong(&val);
	}
	return SetValue(val);
}

bool SP_DS_MarkingAttribute::CheckIntegrity()
{
	long l_Int = 0;
	double l_Double = 0;
	if (!m_sValue.ToLong(&l_Int) && m_sValue.ToDouble(&l_Double) &&  !m_sValue.Contains(wxT("e")))
	{
		SP_LOGERROR( wxT("\"") + m_sValue + wxT("\" double value not allowed"));
		return false;
	}

	if (!parseString(m_sValue, NULL))
	{
		SP_LOGERROR( wxT("\"") + m_sValue + wxT("\" value not allowed"));
		return false;
	}
	return SP_DS_ExpressionAttribute::CheckIntegrity();
}

