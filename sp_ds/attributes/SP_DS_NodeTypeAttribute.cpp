//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: george assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/04/17 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_NodeTypeAttribute.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//by sl
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/regex.h>

SP_DS_NodeTypeAttribute::SP_DS_NodeTypeAttribute(const wxString& p_pchName)
	:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NODETYPE),
	m_sValue("")
{
	//IsOk();
}

SP_DS_Attribute*
SP_DS_NodeTypeAttribute::Clone(bool p_bCloneGr)
{
	SP_DS_NodeTypeAttribute* l_pcAttr = new SP_DS_NodeTypeAttribute(GetName());

	// just clone the graphic stuff, too
	return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_NodeTypeAttribute::IsOk()
{
	if (m_pcParent->GetElementType() == SP_ELEMENT_NODE
		|| m_pcParent->GetElementType() == SP_ELEMENT_EDGE
		)
	{
		if (GetParent()->GetClassObject())
		{
			if (GetParent()->GetClassName() == SP_DS_DISCRETE_PLACE)
			{
				m_sValue = wxT("Discrete Place");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_CONTINUOUS_PLACE)
			{
				m_sValue = wxT("Continuous Place");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_CONTINUOUS_TRANS)
			{
				m_sValue = wxT("Continuous Transition");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_SCHEDULED_TRANS)
			{
				m_sValue = wxT("Scheduled Transition");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_STOCHASTIC_TRANS || GetParent()->GetClassName() == SP_DS_DISCRETE_TRANS)
			{
				m_sValue = wxT("Stochastic Transition");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_DETERMINISTIC_TRANS)
			{
				m_sValue = wxT("Deterministic Transition");
				return TRUE;
			}
			else if (GetParent()->GetClassName() == SP_DS_IMMEDIATE_TRANS)
			{
				m_sValue = wxT("Immediate Transition");
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool
SP_DS_NodeTypeAttribute::SetValue(const wxString& p_pchVal)
{
	/*we do not expect any string value*/
    
	return IsOk();
}

bool SP_DS_NodeTypeAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue )
{
	return SetValue(wxT(""));
}
