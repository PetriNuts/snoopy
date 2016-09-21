//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: drawn shape implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/metadata/SP_GR_MetaDrawnShape.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_MetaDrawnShape::SP_GR_MetaDrawnShape(SP_DS_Metadata* p_pcParent)
:SP_GR_Metadata(p_pcParent)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseDrawn();
    m_pcPrimitive->SetEventHandler(new SP_GRM_ShapeHandler(m_pcPrimitive, m_pcPrimitive));

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);

    Draw();
}

SP_GR_MetaDrawnShape::~SP_GR_MetaDrawnShape()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_MetaDrawnShape::Update(bool p_bLocalOnly)
{
    return Draw() && SP_GR_Metadata::Update(p_bLocalOnly);
}

wxShape*
SP_GR_MetaDrawnShape::GetPrimitive()
{
    return m_pcPrimitive;
}

//////////////////////////////////////////////////////////////////////
bool
SP_GR_MetaDrawnShape::Draw()
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();

    int l_nX = 10;
    int l_nY = 10;

    m_pcPrimitive->SetDrawnPen(GetPen(), TRUE);
    m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
    m_pcPrimitive->DrawRectangle(wxRect(wxPoint(-l_nX, -l_nY), wxSize(l_nX * 2, l_nY * 2)));
    m_pcPrimitive->DrawLine(wxPoint(-l_nX + 1, -l_nY + 1), wxPoint( l_nX - 1, l_nY - 1));
    m_pcPrimitive->DrawLine(wxPoint( l_nX - 1, -l_nY + 1), wxPoint(-l_nX + 1, l_nY - 1));

    m_pcPrimitive->CalculateSize();

    return TRUE;
}



