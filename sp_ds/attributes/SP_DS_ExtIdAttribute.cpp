//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/03/24 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_ExtIdAttribute.h"
#include "sp_core/base/SP_ElementClass.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_core/SP_Core.h"


SP_DS_ExtIdAttribute::SP_DS_ExtIdAttribute(const wxString& p_pchName)
:SP_DS_NumberAttribute(p_pchName, NOT_SET_YET)
{
    SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
}

SP_DS_Attribute*
SP_DS_ExtIdAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_ExtIdAttribute* l_pcAttr = new SP_DS_ExtIdAttribute(GetName());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

bool
SP_DS_ExtIdAttribute::SetValue(long p_nVal)
{


	if (GetParent() && GetParent()->GetClassObject())
            GetParent()->GetClassObject()->TestIdCount(p_nVal);


    m_nValue = p_nVal;
    return TRUE;
}

long
SP_DS_ExtIdAttribute::GetValue()
{

	if (m_nValue == NOT_SET_YET)
    {
        if (GetParent() && GetParent()->GetClassObject()){
            m_nValue = GetParent()->GetClassObject()->GetNewIdCount();
			if (m_nValue > 0) {
				if (ExistID(m_nValue)){
					m_nValue += GetParent()->GetClassObject()->GetNewIdCount();
				}
			}
		}
    }
    return m_nValue;
}

bool
SP_DS_ExtIdAttribute::Squeeze()
{
    if (GetParent() && GetParent()->GetClassObject())
    {
		m_nValue = GetParent()->GetClassObject()->GetNewIdCount();
        return TRUE;
    }
    return FALSE;
}

bool
SP_DS_ExtIdAttribute::ExistID(long p_nVal)
{

	SP_ListNodeclass::const_iterator nIt;
	const SP_ListNodeclass* ncList = NULL;

	if (GetParent()->GetClassName()== wxT("Basic Event") ||
		GetParent()->GetClassName()== wxT("Undeveloped Event")) {

		SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());
		ncList = l_pcParent->GetNodeclass()->GetParentGraph()->GetNodeclasses();

		CHECK_POINTER(ncList, return FALSE);
	for (nIt = ncList->begin(); nIt != ncList->end(); ++nIt){
		if ((*nIt)->GetName()== wxT("Basic Event")
			|| (*nIt)->GetName()== wxT("Undeveloped Event")){
			const SP_ListNode* n_pcAllNodes = (*nIt)->GetElements();
			SP_ListNode::const_iterator pcIt;
			for (pcIt = n_pcAllNodes->begin(); pcIt != n_pcAllNodes->end(); ++pcIt){
				wxString sID = (*pcIt)->GetAttribute(wxT("ID"))->GetValueString();
				long id;
				sID.ToLong(&id);
				if (id == p_nVal)
					return TRUE;
			}
		}
	}
	}

	return FALSE;
}
