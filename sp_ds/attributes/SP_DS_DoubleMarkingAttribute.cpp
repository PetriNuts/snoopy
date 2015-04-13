/*
 * SP_DS_DoubleMarkingAttribute.cpp
 *
 *  Created on: 14.01.2013
 *      Author: dell-notebook
 */
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"

SP_DS_DoubleMarkingAttribute::SP_DS_DoubleMarkingAttribute(const wxString& p_pchName,
                                         const wxString& p_pchVal)
:SP_DS_ExpressionAttribute(p_pchName, p_pchVal, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING)
{

}


SP_DS_DoubleMarkingAttribute::SP_DS_DoubleMarkingAttribute(const wxString& p_pchName, double p_nVal)
:SP_DS_ExpressionAttribute(p_pchName, wxString::Format(wxT("%g"), p_nVal), SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING), m_nValue(p_nVal)
{
}

SP_DS_Attribute*
SP_DS_DoubleMarkingAttribute::Clone(bool p_bCloneGr)
{
	SP_DS_DoubleMarkingAttribute* l_pcAttr = new SP_DS_DoubleMarkingAttribute(GetName(), GetValueString());

	// just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_DoubleMarkingAttribute::SetValue(double l_nValue)
{
	m_nValue = l_nValue;
	return true;
}

bool
SP_DS_DoubleMarkingAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
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

/*
 * $return: numeric value of attribute
 */
double
SP_DS_DoubleMarkingAttribute::GetValue(bool p_bUpdate)
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
SP_DS_DoubleMarkingAttribute::GetValueString()
{
	return m_sValue;
}

bool
SP_DS_DoubleMarkingAttribute::UpdateValue()
{
	double val = 0;
	if(m_Function->isValue())
	{
		val = m_Function->getValue();
	}
	else
	{
		try
		{
			SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();
			SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
			SP_DS_FunctionEvaluatorDouble eval(l_pcFR, m_Function);
			val = eval();
		}
		catch(dsszmc::functions::Exception& e)
		{
			SP_LOGERROR( wxString(e.getExcName().c_str(), wxConvUTF8) + wxT(": ") + wxString(e.getMsg().c_str(), wxConvUTF8));
			m_sValueStandard.ToDouble(&val);
		}
	}
	if ( val < 0 )
	{
//		SP_MESSAGEBOX(wxT("negative value not allowed"), wxT("Error"), wxOK | wxICON_ERROR);
		SP_LOGERROR( wxT("negative value not allowed"));
		m_sValueStandard.ToDouble(&val);
	}
	return SetValue(val);
}

bool SP_DS_DoubleMarkingAttribute::CheckIntegrity()
{
	if (!parseString(m_sValue, NULL))
	{
		SP_LOGERROR( wxT("\"") + m_sValue + wxT("\" value not allowed"));
		return false;
	}
	return SP_DS_ExpressionAttribute::CheckIntegrity();
}

