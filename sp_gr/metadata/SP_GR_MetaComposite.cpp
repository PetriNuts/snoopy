//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2015/01/12 11:55:00 $
// Short Description: Composite Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/metadata/SP_GR_MetaComposite.h"
#include "sp_gr/base/SP_GR_BaseRectangle.h"
#include "sp_gr/SP_GR_Attribute.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MetaComposite::SP_GR_MetaComposite(SP_DS_Metadata* p_pcParent,
		double p_nW,
		double p_nH,
		double p_nR)
:SP_GR_Metadata(p_pcParent),
m_nRadius(p_nR)
{
	//TODO check for make_unique
    m_pcPrimitive = SP_GR_BaseRectangle_Ptr(new SP_GR_BaseRectangle(p_nW, p_nH));
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive.get(), m_pcPrimitive.get()));
    m_pcPrimitive->SetCornerRadius(p_nR);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive.get(), this);

	for(SP_DS_Attribute* l_pcAttr : *m_pcParent->GetAttributes())
	{
		SP_Graphic* l_pcGr = l_pcAttr->GetGraphicInSubnet(GetNetnumber());
		if(l_pcGr)
		{
			m_pcGraphics.push_back(l_pcGr);
			auto l_pcEventHandler = l_pcGr->GetPrimitive()->GetEventHandler();
			wxDELETE(l_pcEventHandler);
			l_pcGr->GetPrimitive()->SetEventHandler(new SP_GRM_UpwardHandler(l_pcGr->GetPrimitive(), l_pcGr->GetPrimitive()));
			l_pcGr->GetPrimitive()->SetSensitivityFilter(OP_ALL &~ OP_DRAG_LEFT);
		}
	}
	if(GetPosX() <= 0)
		SetPosX(50);
	if(GetPosY() <= 0)
		SetPosY(50);
}

SP_GR_MetaComposite::~SP_GR_MetaComposite()
{
    SP_Core::Instance()->RemoveExtern(m_pcPrimitive.get());

    m_pcGraphics.clear();
}

SP_Graphic*
SP_GR_MetaComposite::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_METADATA)
        return NULL;

    SP_GR_MetaComposite* l_pcNew = new SP_GR_MetaComposite(dynamic_cast<SP_DS_Metadata*>(p_pcParent), GetWidth(), GetHeight(), GetRadius());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxShape*
SP_GR_MetaComposite::GetPrimitive()
{
    return m_pcPrimitive.get();
}

bool
SP_GR_MetaComposite::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    double l_nX = GetPosX();
	double l_nY = GetPosY();

   	SetWidth(0);
   	SetHeight(0);

	double l_nWidth = 0;
	double l_nHeight = 0;

	//compute size
    for(SP_Graphic* l_pcGr : m_pcGraphics)
    {
    	if(l_pcGr->GetShow())
    	{
			l_nWidth += l_pcGr->GetWidth();
			l_nHeight = std::max(l_nHeight, l_pcGr->GetHeight());
    	}
    }
   	SetWidth(l_nWidth);
   	SetHeight(l_nHeight);

    //set positions
    l_nX -= l_nWidth/2;
    if(l_nX <= 0)
    {
    	l_nX = 0;
    	SetPosX(l_nWidth/2);
    }
    for(SP_Graphic* l_pcGr : m_pcGraphics)
    {
    	if(l_pcGr->GetShow())
    	{
			l_nWidth = l_pcGr->GetWidth();
			if(l_nWidth > 0)
			{
				l_pcGr->SetPosXY(l_nX + l_nWidth/2, l_nY);
				l_nX += l_nWidth;
			}
    	}
    }

    return SP_GR_Metadata::Update(p_bLocalOnly);
}

double
SP_GR_MetaComposite::GetRadius() const
{
  return m_nRadius;
}

bool
SP_GR_MetaComposite::SetRadius(double p_radius)
{
  if (! m_pcPrimitive) return false;
  m_nRadius = p_radius;
  m_pcPrimitive->SetCornerRadius(p_radius);
  return true;
}
