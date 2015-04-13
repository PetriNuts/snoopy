//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/21 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_NodeVector.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"
#include "sp_core/SP_Core.h"

/**	\brief	Constructor

	\param	p_pcGraph	The graph, this class is part of
*/
SP_DS_NodeVector::SP_DS_NodeVector(
SP_DS_Graph* p_pcGraph,
SP_DS_NodeVectorSystem* p_pcNvs)
:
SP_Name(wxT("NodeVector")),
// @todo you should define a new constant
SP_Data(SP_ELEMENT_NODE),
SP_Graphic(SP_GRAPHIC_NODEVECTOR),
m_pcNvs(p_pcNvs)
{
	this->AddGraphic(this);
	this->SetDataParent(this);
}

SP_DS_NodeVector::~SP_DS_NodeVector()
{
}

//
SP_DS_Attribute*
SP_DS_NodeVector::AddAttribute(SP_DS_Attribute* p_pcAttr)
{
	CHECK_POINTER(p_pcAttr, return NULL);
	// alert, parameter already is associated to another data-element
	CHECK_BOOL((p_pcAttr->GetParent() == NULL), return NULL);

	p_pcAttr->SetDataParent(this);
	m_lAttributes.push_back(p_pcAttr);
	return p_pcAttr;
}

SP_DS_Attribute*
SP_DS_NodeVector::GetAttribute(const wxString& p_pchName){
	if(p_pchName.IsEmpty()) return NULL;

	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (p_pchName.Cmp(l_pcAttr->GetName()) == 0)
			return l_pcAttr;
	}

	return NULL;
}

bool
SP_DS_NodeVector::HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return TRUE;
	}
	return FALSE;
}

SP_DS_Attribute*
SP_DS_NodeVector::GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal)
{
	for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
	{
		if (l_pcAttr->GetAttributeType() == p_eVal)
			return l_pcAttr;
	}
	return NULL;
}

SP_DS_Attribute*
SP_DS_NodeVector::RemoveAttribute(SP_DS_Attribute* p_pcVal)
{
	if (!p_pcVal || (SP_Find(m_lAttributes, p_pcVal) == m_lAttributes.end()))
		return NULL;

	p_pcVal->SetDataParent(NULL);
	m_lAttributes.remove(p_pcVal);

	return p_pcVal;
}

bool
SP_DS_NodeVector::AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
	CHECK_POINTER(p_ptmAttributes, return FALSE);
	//CHECK_POINTER(m_lAttributes, return FALSE);

    // iterate over the list of attributes and insert them into the value of
    // the map, i.e. the list of attributes under the key, combined of
    // wxT("<name of the class>|<name of the attribute>|<type of the attribute>")
    for (SP_DS_Attribute* l_pcAttr : m_lAttributes)
    {
    	wxString l_sKey = SP_Core::Instance()->GetDialogWidgetKey(l_pcAttr);
        (*p_ptmAttributes)[l_sKey].push_back(l_pcAttr);
    }

    return TRUE;
}

// apply visitor
bool
SP_DS_NodeVector::Accept(SP_DS_Visitor* p_pcVisitor){
  CHECK_POINTER(p_pcVisitor, return false);
  return p_pcVisitor->Visit(this);
}

// Set the node list that all node vectors belong to
//bool setNodeList(SP_ListNode*);

//removes himself from parent nvs
bool
SP_DS_NodeVector::Remove()
{
    bool l_bReturn = TRUE;
		CHECK_POINTER(m_pcNvs, return false);
    m_pcNvs->RemoveElement(this);

    return l_bReturn;
}

const wxString
SP_DS_NodeVector::GetClassName()
{
    if (m_pcNvs)
        return (*m_pcNvs->GetNodes().begin())->GetClassName();
    return wxT("");
}

SP_ElementClass*
SP_DS_NodeVector::GetClassObject()
{
	if (m_pcNvs)
	        return (*m_pcNvs->GetNodes().begin())->GetClassObject();
	return NULL;
}

bool
SP_DS_NodeVector::EditProperties()
{
    // map of list of attributes
    SP_MapString2ListAttribute l_tmAttributes;

    // get the attributes to show in the dialog
    // implemented in base class 'SP_Graphic'
    CollectDialogAttributes(&l_tmAttributes);

    // construct the dialog
    SP_DLG_ShapeProperties l_cDialog(NULL, wxT("Edit Properties"));
    // add this to the list of affected graphics
    l_cDialog.AddDialogGraphic(this);

    if (l_cDialog.ShowDialogAttributes(&l_tmAttributes))
    {
        // show and forget about the dlg
        // the dialog manages the events (OK, CANCEL) by itself
        l_cDialog.ShowModal();

    }
    else
    {
        SP_LOGDEBUG(wxT("No attributes on this Node Vector"));
    }

    if (GetParent())
        return GetParent()->Update(TRUE);

    return SP_Data::Update(TRUE);
}

// to implement SP_Graphic
wxShape*
SP_DS_NodeVector::GetPrimitive(){
  // @todo implement if needed
  return NULL;
}

SP_Graphic*
SP_DS_NodeVector::Clone(SP_Data* p_pcParent){
  // @todo implement if needed
  return NULL;
}

/*bool
SP_DS_NodeVector::operator<(SP_DS_NodeVector* p_pcLeft, SP_DS_NodeVector* p_pcRight){

    SP_DS_Attribute* l_pcLAttr;
	  SP_DS_Attribute* l_pcRAttr;

    l_pcLAttr = (*p_pcLeft)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
  	l_pcRAttr = (*p_pcRight)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);

    if (l_pcLAttr && l_pcRAttr && (p_nNumber == dynamic_cast<SP_DS_IdAttribute*>(l_pcAttr)->GetValue()))
    {
			return dynamic_cast<SP_DS_NodeVector*>(*l_itNodeVector);
    }
}*/




