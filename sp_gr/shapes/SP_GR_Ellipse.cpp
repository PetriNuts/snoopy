//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Ellipse implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/base/SP_GR_BaseEllipse.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Ellipse::SP_GR_Ellipse(SP_DS_Node* p_pcParent, double p_nW, double p_nH)
:SP_GR_Node(p_pcParent)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseEllipse(p_nW, p_nH);
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive, m_pcPrimitive));

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_Ellipse::~SP_GR_Ellipse()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_Ellipse::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Ellipse* l_pcNew = new SP_GR_Ellipse(dynamic_cast<SP_DS_Node*>(p_pcParent),
        this->GetWidth(), this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
wxShape*
SP_GR_Ellipse::GetPrimitive()
{
    return m_pcPrimitive;
}

