//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/03 11:50:00 $
// Short Description: Condition-Parameter implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_ExtendedParameterCondition.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ExtendedParameterCondition::SP_GR_ExtendedParameterCondition(SP_DS_Node* p_pcParent,int p_nWidth,SP_EXTENDED_TYPE p_nType, size_t p_nThickness,wxColour p_pcColur)
:SP_GR_ExtendedDrawnShape(p_pcParent,p_nType,p_nThickness)
{
  SetWidth(p_nWidth);
  SetPenColour(p_pcColur);
}

SP_GR_ExtendedParameterCondition::~SP_GR_ExtendedParameterCondition()
{
	if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ExtendedParameterCondition::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_ExtendedParameterCondition* l_pcNew = new SP_GR_ExtendedParameterCondition(dynamic_cast<SP_DS_Node*>(p_pcParent));

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------
bool
SP_GR_ExtendedParameterCondition::Draw()
{
  CHECK_POINTER(m_pcPrimitive, return FALSE);

  // Clear Content
  // because of GetMetaFile() returns a reference object,
  // we take the adress to avoid it beeing copied
  wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
  // so we really clear the metafile of our drawn shape
  l_cMeta->Clear();

  int l_nX = wxRound(GetWidth() / 2.0);
  int l_nY = wxRound(GetWidth() / 4.0);


  m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
  /* change ckruege2 19.07.09 */
  SetCanvasModPenStyle(wxColor(128,128,128),3,SP_EXTENDED_TYPE_DEFAULT);  
  m_pcPrimitive->SetPen(GetCanvasModPen());
  //m_pcPrimitive->SetDrawnPen(wxGetApp().GetCanvasThickDarkGreyPen());
  /* change end */
  m_pcPrimitive->DrawEllipse(wxRect(-l_nX, -l_nY, l_nX * 2, l_nY * 2));
	int size = 0;
	if (l_nX >= l_nY)
	{
		size = wxRound(l_nY/1.5);
	}else
	{
		size = l_nX;
	}
	m_pcPrimitive->SetFont(wxTheFontList->FindOrCreateFont(l_nY, wxMODERN, wxNORMAL, wxNORMAL));

	m_pcPrimitive->AddText(wxT(""));
	m_pcPrimitive->CalculateSize();

	return TRUE;
}

bool
SP_GR_ExtendedParameterCondition::Merge(SP_Graphic* p_pcTarget)
{
	SP_DS_Node* targetNode = dynamic_cast<SP_DS_Node*>(p_pcTarget->GetParent());

	if (!targetNode->GetLogical()){
		SP_MESSAGEBOX(wxT("No merging possible for undependent cond-parameters."));
		return FALSE;
	}

	return SP_GR_Node::Merge(p_pcTarget);

}

