//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/01/05 11:55:00 $
// Short Description: Gate implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/shapes/SP_GR_NamedRectangle.h"
#include "sp_gr/eventhandler/SP_GRM_ShapeHandler.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_NamedRectangle::SP_GR_NamedRectangle(SP_DS_Node* p_pcParent, const wxString& newName, double p_nWidth, double p_nHeight)
:SP_GR_DrawnShape(p_pcParent)
{
  SetWidth(p_nWidth);
  SetHeight(p_nHeight);
  m_shapeName = newName;
}

SP_GR_NamedRectangle::~SP_GR_NamedRectangle()
{
}

SP_Graphic*
SP_GR_NamedRectangle::Clone(SP_Data* p_pcParent)
{
    if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
        return NULL;

    SP_GR_NamedRectangle* l_pcNew = new SP_GR_NamedRectangle(dynamic_cast<SP_DS_Node*>(p_pcParent),
        m_shapeName, GetWidth(), GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Draw method
//
//---------------------------------------------------------------

wxShape*
SP_GR_NamedRectangle::GetPrimitive()
{
    return m_pcPrimitive;
}


bool
SP_GR_NamedRectangle::Draw()
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
	wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
	l_cMeta->Clear();


	SP_DS_Attribute* l_pcNameAttr = GetParent()->GetAttribute(wxT("Value"));
	if ( l_pcNameAttr){
		m_shapeName = l_pcNameAttr->GetValueString();
	}



	int l_nX = (int)(GetWidth() / 2.0);
    int l_nY = (int)(GetHeight() );

    m_pcPrimitive->SetDrawnPen(m_pcPrimitive->GetPen());
	m_pcPrimitive->SetDrawnBrush(GetBrush(), TRUE);
	m_pcPrimitive->DrawRectangle(wxRect(-l_nX, -l_nY, l_nX * 2, l_nY * 2));



	int size = 0;
	if (l_nX >= l_nY) {
			size = l_nY/2.0;
		}else{
			size = l_nX;
	}

	double l_iFontSize = size / ( 1.0 + (0.35* (m_shapeName.Len())));
	m_pcPrimitive->SetFont(wxTheFontList->FindOrCreateFont(l_iFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


	m_pcPrimitive->AddText(m_shapeName);


    m_pcPrimitive->CalculateSize();
	m_pcPrimitive->SetSize(l_nX*2 ,  (int)(l_nY));

    return TRUE;
}




bool
SP_GR_NamedRectangle::Update(bool p_bLocalOnly)
{
    return Draw() && SP_GR_Node::Update(p_bLocalOnly);
}

