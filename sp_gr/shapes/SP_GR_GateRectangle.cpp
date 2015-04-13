//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/01/05 11:55:00 $
// Short Description: Gate implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_GateRectangle.h"
#include "sp_gr/shapes/SP_GR_NamedRectangle.h"


#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_GateRectangle::SP_GR_GateRectangle(SP_DS_Node* p_pcParent, const wxString& newName, double p_nWidth, double p_nHeight)
:SP_GR_NamedRectangle(p_pcParent, newName, p_nWidth, p_nHeight)
{

}

SP_GR_GateRectangle::~SP_GR_GateRectangle()
{
}

SP_Graphic*
SP_GR_GateRectangle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_GateRectangle* l_pcNew = new SP_GR_GateRectangle(dynamic_cast<SP_DS_Node*>(p_pcParent),
        m_shapeName, GetWidth(), GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}



bool
SP_GR_GateRectangle::Merge(SP_Graphic* p_pcTarget)
{

	if (m_shapeName == wxT("1") ) {
		SP_MESSAGEBOX(wxT("Merging of NEG-Gatter not possible"));
		return FALSE;
	}

	return SP_GR_Node::Merge(p_pcTarget);

}
