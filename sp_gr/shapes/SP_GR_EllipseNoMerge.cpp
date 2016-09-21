//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Ellipse implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_EllipseNoMerge.h"
#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_EllipseNoMerge::SP_GR_EllipseNoMerge(SP_DS_Node* p_pcParent, double p_nW, double p_nH)
:SP_GR_Ellipse(p_pcParent, p_nW, p_nH)
{

}

SP_GR_EllipseNoMerge::~SP_GR_EllipseNoMerge()
{

}

SP_Graphic*
SP_GR_EllipseNoMerge::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_EllipseNoMerge* l_pcNew = new SP_GR_EllipseNoMerge(dynamic_cast<SP_DS_Node*>(p_pcParent),
        this->GetWidth(), this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

bool
SP_GR_EllipseNoMerge::Merge(SP_Graphic* p_pcTarget)
{
	SP_DS_Node* targetNode = dynamic_cast<SP_DS_Node*>(p_pcTarget->GetParent());

	if (!targetNode->GetLogical()){
		SP_MESSAGEBOX(wxT("No merging possible for undependent basic events."));
		return FALSE;
	}

	return SP_GR_Node::Merge(p_pcTarget);

}
