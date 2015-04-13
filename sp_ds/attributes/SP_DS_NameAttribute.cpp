//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//by sl
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/regex.h>

SP_DS_NameAttribute::SP_DS_NameAttribute(const wxString& p_pchName,
                                         const wxString& p_pchVal)
:SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME),
m_sValue(p_pchVal)
{
}

SP_DS_Attribute*
SP_DS_NameAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_NameAttribute* l_pcAttr = new SP_DS_NameAttribute(GetName(), m_sValue);

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_NameAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	bool l_bReturn = true;

	if(p_bCheckValue)
	{
		l_bReturn = SetValue(p_pchVal);
	}
	else
	{
		m_sValue = p_pchVal;
	}
	return l_bReturn;
}

bool
SP_DS_NameAttribute::SetValue(const wxString& p_pchVal)
{

    wxString l_sBackup = m_sValue;
    wxString l_sTemp(p_pchVal);

    // special, if the param contains '*' as wildcard, we replace every occurance
    // of '*' with the content of the old member value
    l_sTemp.Replace(wxT("*"), m_sValue);
    m_sValue = l_sTemp;
    //TODO
    //check if name is accepted by ^[a-zA-Z][0-9a-zA-Z_]*$
    if(m_pcParent->GetElementType() == SP_ELEMENT_NODE
    	|| m_pcParent->GetElementType() == SP_ELEMENT_EDGE
    	|| m_pcParent->GetElementType() == SP_ELEMENT_METADATA)
    {
		while(!IsOk())
		{
			wxString l_sMsg = wxT("The name does not fit the requirements.\nThe name has to begin with a letter and \nonly letters, numbers and underscore are allowed: ");
			wxTextEntryDialog* l_pcDlg = new wxTextEntryDialog(NULL, l_sMsg, wxT("Change name."), m_sValue);
			if(l_pcDlg->ShowModal() == wxID_OK)
			{
				m_sValue = l_pcDlg->GetValue();
			}
			else
			{
				m_sValue = l_sBackup;
				break;
			}
			l_pcDlg->Destroy();
		}
    }

	if(l_sBackup != m_sValue)
	{
		SetValueInAttribute(l_sBackup);
	}

    return TRUE;
}

bool
SP_DS_NameAttribute::IsOk()
{
	bool l_bReturn = false;
	wxString l_sNameRegEx = wxT("^[a-zA-Z_][0-9a-zA-Z_]*$|^$");
	wxRegEx l_cRegEx;
	if(l_cRegEx.Compile(l_sNameRegEx))
	{
		l_bReturn = l_cRegEx.Matches(m_sValue);
	}
#if 0
	/*
	 * check if the name is a constant
	 */
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(l_pcGraph)
	{

		SP_DS_Metadataclass* mc = l_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		if(mc)
		{
			SP_ListMetadata::const_iterator it;
			for (SP_DS_Metadata* meta : *(mc->GetElements()))
			{
				wxString l_sMetadataName =
						dynamic_cast<SP_DS_NameAttribute*>(meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				if(meta != GetParent())
					if (m_sValue.IsSameAs(l_sMetadataName))
					{
						l_bReturn = false;
					}
			}
		}
	}
#endif
	return l_bReturn;
}

void
SP_DS_NameAttribute::ReplaceProhibitedChars()
{
	wxString l_sNameRegEx = wxT("[^0-9a-zA-Z_]");
	wxRegEx l_cRegEx;
	if(l_cRegEx.Compile(l_sNameRegEx))
	{
		l_cRegEx.Replace(&m_sValue, wxT("_"));
	}
	if(!IsOk())
	{
		m_sValue.Prepend(wxT("_"));
	}
}

wxString
SP_DS_NameAttribute::GetValue()
{
//TODO: check for better solution
	if(m_sValue.IsEmpty())// && !GetParent()->GetCoarse())
	{
		return MakeParentName();
	}
	else
	{
		return m_sValue;
	}
}

bool
SP_DS_NameAttribute::SetValueInAttribute(const wxString& p_pchVal)
{
	if(p_pchVal.IsEmpty())
	{
		return true;
	}

	wxString l_sOldName(p_pchVal);
	wxString l_sNewName = GetValue();

	if(GetParent()->GetCoarse())
	{
		SP_DS_Graph* l_pcCoarseGraph = GetParent()->GetCoarse()->GetGraph();
		for(SP_ListNodeclass::const_iterator l_itNC = l_pcCoarseGraph->GetNodeclasses()->begin();
			l_itNC != l_pcCoarseGraph->GetNodeclasses()->end();
			l_itNC++)
		{
			for(SP_ListNode::const_iterator l_it = (*l_itNC)->GetElements()->begin();
				l_it != (*l_itNC)->GetElements()->end();
				l_it++)
			{
				SP_DS_Node* l_pcNode = *l_it;
				if(l_pcNode->GetNetnumber() == GetParent()->GetCoarse()->GetNetnumber())
				{
					SP_DS_Attribute* l_pcAttr = l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME);
					if(l_pcAttr)
					{
						wxString l_sValueString = l_pcAttr->GetValueString();
						if(l_sValueString.EndsWith(wxT("::") + l_sOldName))
						{
							l_sValueString.Replace(wxT("::") + l_sOldName, wxT("::") + l_sNewName);
						}
						else if(l_sValueString.EndsWith(wxT("::")))
						{
							l_sValueString.Append(l_sNewName);
						}
						l_pcAttr->SetValueString(l_sValueString);
					}
				}
			}
		}
	}

	if(GetParent()->GetClassObject())
	{
		if(GetParent()->GetClassName() == SP_DS_DISCRETE_PLACE
			|| GetParent()->GetClassName() == SP_DS_CONTINUOUS_PLACE)
		{
			if(l_sNewName.IsEmpty())
			{
				l_sNewName = GetParentName();
			}
			SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(GetParent());
			if(l_pcNode)
			{
				for(SP_DS_Edge* l_Edge : (*l_pcNode->GetSourceEdges()))
				{
					SP_DS_Node* l_pcTrg = dynamic_cast<SP_DS_Node*>(l_Edge->GetTarget());
					SP_DS_Attribute* l_pcAttr = l_pcTrg->GetAttribute(wxT("FunctionList"));
					if(l_pcAttr)
					{
						ReplaceValueInColListAttribute(l_pcAttr, l_sOldName, l_sNewName);
					}
				}
			}
		}
		else if(GetParent()->GetClassName() == SP_DS_PARAM
				|| GetParent()->GetClassName() == SP_DS_META_CONSTANT
				|| GetParent()->GetClassName() == SP_DS_META_FUNCTION)
		{
			if(l_sNewName.IsEmpty())
			{
				l_sNewName = GetParentName();
			}
			ReplaceNameInNode(l_sOldName, l_sNewName);
		}
	}

    return TRUE;
}

bool
SP_DS_NameAttribute::ReplaceNameInNode(const wxString& p_sOld, const wxString& p_sNew)
{
	SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();
	for(SP_DS_Nodeclass* l_NC : *(l_pcGraph->GetNodeclasses()))
	{
		if(l_NC->GetName().Contains(wxT("Transition"))
				&& l_NC->GetPrototype()->GetAttribute(wxT("FunctionList")))
		{
			for(SP_DS_Node* l_Node : *(l_NC->GetElements()))
			{
				SP_DS_Attribute* l_pcAttr = l_Node->GetAttribute(wxT("FunctionList"));
				if(l_pcAttr)
				{
					ReplaceValueInColListAttribute(l_pcAttr, p_sOld, p_sNew);
				}
			}
		}
	}
	return true;
}

bool
SP_DS_NameAttribute::ReplaceValueInColListAttribute(SP_DS_Attribute *p_pcAttr, const wxString &p_sOldValue, const wxString &p_sNewValue)
{
	wxString l_sSeperators = wxT("([^0-9a-zA-Z_])");
	SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(p_pcAttr);
	if(l_pcColAttr)
	{
		for(unsigned long i = 1; i < l_pcColAttr->GetColCount(); ++i)
		{
			for(unsigned long j = 0; j < l_pcColAttr->GetRowCount(); ++j)
			{
				wxString l_sFunction = wxT(" ") + l_pcColAttr->GetCell(j,i) + wxT(" ");
				wxString l_sOldRegEx = l_sSeperators + wxT("(") + p_sOldValue + wxT(")") + l_sSeperators;
				wxString l_sNewRegEx = wxT("\\1") + p_sNewValue + wxT("\\3");
				wxRegEx l_cRegEx(l_sOldRegEx);
				l_cRegEx.Replace(&l_sFunction, l_sNewRegEx);
				l_sFunction.Trim();
				l_sFunction.Trim(false);
				l_pcColAttr->SetCell(j,i,l_sFunction);
			}
		}
	}
	return true;
}

/*
bool
SP_DS_NameAttribute::ReSetName()
{
	if(!m_sBackup.IsEmpty() && m_sBackup != m_sValue)
	{
		SP_DS_Attribute* l_pcAttr = m_pcParent->GetAttribute(wxT("Comment"));
		if(l_pcAttr)
		{
			wxString l_sMsg = l_pcAttr->GetValueString();
			if(!l_sMsg.IsEmpty())
				l_sMsg << wxT("\n");
			l_sMsg << wxT("//old name:") << m_sBackup;
			l_pcAttr->SetValueString(l_sMsg);
		}
		wxString l_sTemp(m_sValue);
		m_sValue = m_sBackup;
		m_sBackup = wxT("");
		SetValueInAttribute(l_sTemp);
		m_sValue = l_sTemp;
		return true;
	}
	return false;
}
*/
