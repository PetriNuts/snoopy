//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_DS_LogicAttribute::SP_DS_LogicAttribute(const wxString& p_pchName,
                                           const wxString& p_pchRef,
                                           bool p_bVal)
:SP_DS_BoolAttribute(p_pchName, p_bVal),
m_sReferenceAttribute(p_pchRef),
m_bOneShot(FALSE)
{
    SetAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LOGIC);

}

SP_DS_Attribute*
SP_DS_LogicAttribute::Clone(bool p_bCloneGr)
{
    SP_DS_LogicAttribute* l_pcAttr = new SP_DS_LogicAttribute(GetName(), GetReferenceAttribute(), GetValue());

    // just clone the graphic stuff, too
    return CloneBase(l_pcAttr, p_bCloneGr);
}

// reimplement
bool
SP_DS_LogicAttribute::SetValue(bool p_bVal)
{
#if 0
    if (m_bValue == p_bVal)
    {
        m_bOneShot = FALSE;
        return TRUE;
    }
#endif

    m_bValue = p_bVal;

    return OnChangeState();
}

bool
SP_DS_LogicAttribute::OnSplit(SP_DS_Attribute* p_pcAttribute)
{
    if (!p_pcAttribute)
        return TRUE;

    if (p_pcAttribute->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LOGIC)
    {
        SP_DS_LogicAttribute* l_pcAttr = dynamic_cast<SP_DS_LogicAttribute*>(p_pcAttribute);
        l_pcAttr->SetOneShot();
        l_pcAttr->SetValue(FALSE);
        m_bValue = TRUE;

        return TRUE;
    }

    return FALSE;
}

//////////////////////////////////////////////////////////////////////

bool
SP_DS_LogicAttribute::OnChangeState()
{
    if (m_bOneShot)
    {
        m_bOneShot = FALSE;
        return TRUE;
    }
    // if the m_bValue is now TRUE, we need to join
    // else we have to split elements
    return (m_bValue)?JoinElements():SplitElements();
}

bool
SP_DS_LogicAttribute::JoinElements()
{
    CHECK_POINTER(GetParent(), return FALSE);

    if(GetParent()->GetElementType() == SP_ELEMENT_NODE)
    {
        SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());

        CHECK_POINTER(l_pcParent->GetNodeclass(), return FALSE);
        CHECK_POINTER(l_pcParent->GetNodeclass()->GetParentGraph(), return FALSE);

        // join elements of the graph, that have in the attribute of name
        // 'GetReferenceAttribute()' the same value as the one in our node
        return l_pcParent->GetNodeclass()->GetParentGraph()->JoinElements(
            l_pcParent,
            GetReferenceAttribute());

    }
    else if(GetParent()->GetElementType() == SP_ELEMENT_METADATA)
    {
        SP_DS_Metadata* l_pcParent = dynamic_cast<SP_DS_Metadata*>(GetParent());

        CHECK_POINTER(l_pcParent->GetMetadataclass(), return FALSE);
        CHECK_POINTER(l_pcParent->GetMetadataclass()->GetParentGraph(), return FALSE);

        // join elements of the graph, that have in the attribute of name
        // 'GetReferenceAttribute()' the same value as the one in our node
        return l_pcParent->GetMetadataclass()->GetParentGraph()->JoinElements(
            l_pcParent,
            GetReferenceAttribute());

    }

    return true;
}

bool
SP_DS_LogicAttribute::SplitElements()
{
    CHECK_POINTER(GetParent(), return FALSE);

    if(GetParent()->GetElementType() == SP_ELEMENT_NODE)
    {

		SP_DS_Node* l_pcSplit = dynamic_cast<SP_DS_Node*>(GetParent());

		// split the list of graphics from the node into separate ones
		return l_pcSplit->Split(SP_Core::Instance()->GetDialogGraphicList());
    }
    else if(GetParent()->GetElementType() == SP_ELEMENT_METADATA)
    {

		SP_DS_Metadata* l_pcSplit = dynamic_cast<SP_DS_Metadata*>(GetParent());

		// split the list of graphics from the node into separate ones
		return l_pcSplit->Split(SP_Core::Instance()->GetDialogGraphicList());
    }

    return true;
}

