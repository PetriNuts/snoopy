//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/07/22 10:20:00 $
// Short Description: Double drawn circle implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/metadata/SP_GR_PlotSymbol.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"


//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_PlotSymbol::SP_GR_PlotSymbol(SP_DS_Metadata* p_pcParent)
:SP_GR_MetaDrawnShape(p_pcParent)
{
  SetWidth(20);
  SetHeight(20);
}

SP_GR_PlotSymbol::~SP_GR_PlotSymbol()
{
}

SP_Graphic*
SP_GR_PlotSymbol::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_METADATA)
        return NULL;

    SP_GR_PlotSymbol* l_pcNew = new SP_GR_PlotSymbol(dynamic_cast<SP_DS_Metadata*>(p_pcParent));

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_PlotSymbol::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();




  int l_nX = (int) (GetWidth() / 2.0);
  int l_nY = (int) (GetHeight() / 2.0);

  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);

  m_pcPrimitive->SetDrawnPen(wxLIGHT_GREY_PEN);
  m_pcPrimitive->DrawRectangle(wxRect(wxPoint(-l_nX, -l_nY),
                                      wxSize(l_nX * 2, l_nY * 2)));

  m_pcPrimitive->SetDrawnPen(GetPen());
  m_pcPrimitive->DrawLine(wxPoint((int)(-l_nX * 0.75), -l_nY),
                          wxPoint((int)(-l_nX * 0.75), l_nY));
  m_pcPrimitive->DrawLine(wxPoint(-l_nX, (int)(l_nY * 0.75)),
                          wxPoint(l_nX, (int)(l_nY * 0.75)));


  m_pcPrimitive->SetDrawnPen(wxRED_PEN);
  if (GetParent()->GetElementType() == SP_ELEMENT_METADATA)
  {
    SP_DS_Metadata* l_pcMetadata = dynamic_cast<SP_DS_Metadata*>(GetParent());
    SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Active"));
    if (l_pcAttr)
    {
      if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL == l_pcAttr->GetAttributeType())
      {
        SP_DS_BoolAttribute* l_pcBoolAttr = dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttr);
        if (l_pcBoolAttr->GetValue())
        {
          m_pcPrimitive->SetDrawnPen(wxGREEN_PEN);
        }
      }
    }
  }
  m_pcPrimitive->DrawLine(wxPoint((int)(-l_nX * 0.75), 0),
                          wxPoint((int)(-l_nX * 0.5), (int)(-l_nY * 0.1)));
  m_pcPrimitive->DrawLine(wxPoint((int)(-l_nX * 0.5), (int)(-l_nY * 0.1)),
                          wxPoint((int)(-l_nX * 0.25), (int)(l_nY * 0.25)));
  m_pcPrimitive->DrawLine(wxPoint((int)(-l_nX * 0.25), (int)(l_nY * 0.25)),
                          wxPoint(0, (int)(-l_nY * 0.25)));
  m_pcPrimitive->DrawLine(wxPoint(0, (int)(-l_nY * 0.25)),
                          wxPoint((int)(l_nX * 0.25), (int)(l_nY * 0.5)));
  m_pcPrimitive->DrawLine(wxPoint((int)(l_nX * 0.25), (int)(l_nY * 0.5)),
                          wxPoint((int)(l_nX * 0.5), 0));
  m_pcPrimitive->DrawLine(wxPoint((int)(l_nX * 0.5), 0),
                          wxPoint((int)(l_nX * 0.75), (int)(-l_nY * 0.5)));
  m_pcPrimitive->DrawLine(wxPoint((int)(l_nX * 0.75), (int)(-l_nY * 0.5)),
                          wxPoint(l_nX, (int)(l_nY * 0.75)));


  m_pcPrimitive->CalculateSize();

  return TRUE;
}



