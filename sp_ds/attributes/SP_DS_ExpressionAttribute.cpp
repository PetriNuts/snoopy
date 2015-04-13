//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: sl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2011/12/08 12:37:00 $
// Short Description: to handle expression and constants for places
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_ExpressionAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/SP_DS_Function.h"

SP_DS_ExpressionAttribute::SP_DS_ExpressionAttribute(const wxString& p_pchName, const wxString& p_pchVal, SP_ATTRIBUTE_TYPE p_eType)
:SP_DS_Attribute(p_pchName, p_eType),m_Function(NULL), m_sValue(p_pchVal), m_sValueStandard(p_pchVal)
{
}

bool SP_DS_ExpressionAttribute::parseString(const wxString& p_sVal, SP_DS_Node* p_pcTrans)
{
	SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();
	SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sVal));
	if(!l_pcFunction)
	{
		return false;
	}

	SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));

	std::set<std::string> l_Vars;
	l_pcExpanded->getVariables(l_Vars);

	if(!l_Vars.empty())
    {
		std::set<std::string> l_Places;
	    if(p_pcTrans)
    	{
			const SP_ListEdge* l_plEdges = p_pcTrans->GetTargetEdges();
			SP_ListEdge::const_iterator it = l_plEdges->begin();
			SP_ListEdge::const_iterator end = l_plEdges->end();
			for(; it != end; ++it)
			{
				SP_DS_Edge* l_pcEdge = *it;
				SP_DS_Node* l_pcPlace = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
				wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_Places.insert(std::string(l_sPlaceName.mb_str()));
			}
    	}
		std::set<std::string> l_Diff;
		std::set_difference(l_Vars.begin(), l_Vars.end(), l_Places.begin(), l_Places.end(),
				std::inserter(l_Diff, l_Diff.begin()));
		if(!l_Diff.empty())
		{
			wxString l_sFunction(l_pcFunction->toString().c_str(), wxConvUTF8);
			wxString l_sExpanded(l_pcExpanded->toString().c_str(), wxConvUTF8);
			wxString l_sMsg = wxT("function: ") + p_sVal;
#ifdef __WXDEBUG__
			l_sMsg << wxT("\nparsed function: ") << l_sFunction;
			l_sMsg << wxT("\nexpanded function: ") << l_sExpanded;
#endif
			l_sMsg << wxT("\nThe following identifiers are not valid: ");
			for(std::set<std::string>::const_iterator it = l_Diff.begin(); it != l_Diff.end(); ++it)
			{
				if(it != l_Diff.begin())
				{
					l_sMsg << wxT(", ");
				}
				l_sMsg << wxString((*it).c_str(), wxConvUTF8);
			}
			SP_LOGERROR( l_sMsg);
			return false;
		}
    }

	m_Function = l_pcFunction;
	return true;
}


