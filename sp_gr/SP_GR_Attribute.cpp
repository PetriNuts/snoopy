//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Attribute-Base Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/SP_GR_Attribute.h"

SP_GR_Attribute::SP_GR_Attribute(SP_DS_Attribute* p_pcDataParent)
:SP_Graphic(SP_GRAPHIC_ATTRIBUTE),
m_nOffsetX(0),
m_nOffsetY(0)
{
    SetDataParent(p_pcDataParent);
}

SP_GR_Attribute::~SP_GR_Attribute()
{
}

bool
SP_GR_Attribute::CloneBase(SP_Graphic* p_pcCopy)
{
    if (!p_pcCopy)
        return FALSE;

    p_pcCopy->SetOffsetX(GetOffsetX());
    p_pcCopy->SetOffsetY(GetOffsetY());

    return SP_Graphic::CloneBase(p_pcCopy);
}

bool
SP_GR_Attribute::OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics)
{
    if (GetGraphicParent() && GetGraphicParent()->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
        return TRUE;

    return SP_Graphic::OnFlatten(p_nToNet, p_nFromNet, p_plGraphics);
}

bool
SP_GR_Attribute::OnDoubleClick(int p_nKeys)
{
  if (GetGraphicParent()) {
    GetGraphicParent()->Select(true);
  }
  return true;
}

bool
SP_GR_Attribute::SetWidth(double p_nVal)
{
    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(p_nVal, GetHeight());
    return TRUE;
}

double
SP_GR_Attribute::GetWidth()
{
    double l_nWidth = 0, l_nHeight = 0;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nWidth;
}

bool
SP_GR_Attribute::SetHeight(double p_nVal)
{
    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(GetWidth(), p_nVal);
    return TRUE;
}

double
SP_GR_Attribute::GetHeight()
{
    double l_nWidth = 0, l_nHeight = 0;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nHeight;
}
