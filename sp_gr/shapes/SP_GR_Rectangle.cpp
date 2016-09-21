//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Rectangle Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/base/SP_GR_BaseRectangle.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_Rectangle::SP_GR_Rectangle(SP_DS_Node* p_pcParent,
				 double p_nW,
				 double p_nH,
				 double p_nR)
:SP_GR_Node(p_pcParent),
m_nRadius(p_nR)
{
    m_pcPrimitive = new SP_GR_BaseRectangle(p_nW, p_nH);
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive, m_pcPrimitive));
    m_pcPrimitive->SetCornerRadius(p_nR);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_Rectangle::~SP_GR_Rectangle()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }

    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_Rectangle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_Rectangle* l_pcNew = new SP_GR_Rectangle(dynamic_cast<SP_DS_Node*>(p_pcParent),
        GetWidth(), GetHeight(), GetRadius());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxShape*
SP_GR_Rectangle::GetPrimitive()
{
    return m_pcPrimitive;
}

double
SP_GR_Rectangle::GetRadius()
{
  return m_nRadius;
}

bool
SP_GR_Rectangle::SetRadius(double p_radius)
{
  if (! m_pcPrimitive) return false;
  m_nRadius = p_radius;
  m_pcPrimitive->SetCornerRadius(p_radius);
  return true;
}


bool
SP_GR_Rectangle::Merge(SP_Graphic* p_pcTarget)
{

	return SP_GR_Node::Merge(p_pcTarget);

}
