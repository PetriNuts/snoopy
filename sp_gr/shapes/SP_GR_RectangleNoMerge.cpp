//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Rectangle Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_RectangleNoMerge.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"



//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_RectangleNoMerge::SP_GR_RectangleNoMerge(SP_DS_Node* p_pcParent,
				 double p_nW,
				 double p_nH,
				 double p_nR)
:SP_GR_Rectangle(p_pcParent, p_nW, p_nH, p_nR)
{

}

SP_GR_RectangleNoMerge::~SP_GR_RectangleNoMerge()
{
	//TODO Call destructor of baseclass

}

SP_Graphic*
SP_GR_RectangleNoMerge::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_RectangleNoMerge* l_pcNew = new SP_GR_RectangleNoMerge(dynamic_cast<SP_DS_Node*>(p_pcParent),
        GetWidth(), GetHeight(), GetRadius());

    CloneBase(l_pcNew);

    return l_pcNew;
}


bool
SP_GR_RectangleNoMerge::Merge(SP_Graphic* p_pcTarget)
{

	SP_MESSAGEBOX(wxT("No merging possible for this Nodetype"));
	return FALSE;

}
