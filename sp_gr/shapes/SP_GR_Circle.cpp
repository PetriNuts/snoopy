//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Circle implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Circle.h"
#include "sp_gr/base/SP_GR_BaseCircle.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Circle::SP_GR_Circle(SP_DS_Node* p_pcParent, double p_nR)
:SP_GR_Node(p_pcParent)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseCircle(p_nR);
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive, m_pcPrimitive));

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_Circle::~SP_GR_Circle()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_Circle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Circle* l_pcNew = new SP_GR_Circle(dynamic_cast<SP_DS_Node*>(p_pcParent),
        this->GetRadius());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_Circle::SetRadius(double p_nVal)
{
    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(p_nVal, p_nVal);
    return TRUE;
}

double
SP_GR_Circle::GetRadius()
{
    double l_nWidth = 0, l_nHeight = 0;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return wxMax(l_nWidth, l_nHeight);
}

wxShape*
SP_GR_Circle::GetPrimitive()
{
    return m_pcPrimitive;
}

