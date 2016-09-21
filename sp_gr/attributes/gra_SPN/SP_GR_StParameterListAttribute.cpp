//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/10/01 14:08:00 $
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_StParameterListAttribute::SP_GR_StParameterListAttribute(SP_DS_Attribute* p_pcParent,
        double p_nWidth,
        double p_nHeight)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseDrawn();
    m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));
    m_pcPrimitive->SetHideDimensions(TRUE);
    m_pcPrimitive->SetSensitivityFilter(OP_ALL &~ OP_DRAG_LEFT);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_StParameterListAttribute::~SP_GR_StParameterListAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_StParameterListAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);
    SP_GR_StParameterListAttribute* l_pcNew = new SP_GR_StParameterListAttribute(l_pcParent,
          this->GetWidth(),
          this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_StParameterListAttribute::Update( bool p_bLocalOnly )
{

  CHECK_POINTER( m_pcPrimitive, return FALSE );
  CHECK_POINTER( GetParent(), return FALSE );
  CHECK_BOOL( ( GetParent()->GetElementType() == SP_ELEMENT_ATTRIBUTE ), return FALSE );
	
  double l_nDouble = 0;

  SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( GetParent() );

  l_pcColList->GetActiveCellValue( 1 ).ToDouble( &l_nDouble );

  return DrawValue( l_nDouble );

}

wxShape*
SP_GR_StParameterListAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

//////////////////////////////////////////////////////////////////////
bool
SP_GR_StParameterListAttribute::DrawValue(double p_nValue)
{
    CHECK_POINTER(m_pcPrimitive, return FALSE);

    // Clear Content
    // because of GetMetaFile() returns a reference object,
    // we take the adress to avoid it beeing copied
    wxPseudoMetaFile* l_cMeta = &(m_pcPrimitive->GetMetaFile());
    // so we really clear the metafile of our drawn shape
    l_cMeta->Clear();

    m_pcPrimitive->Show(m_bShow );
    wxString l_sValue;
    l_sValue << p_nValue;
    if(m_pcPrimitive->CalculateDimensions(l_sValue))
    {
		double l_nX = m_pcPrimitive->GetWidth();
		double l_nY = m_pcPrimitive->GetHeight();
	    m_pcPrimitive->DrawText(l_sValue, wxPoint(-(l_nX/2), -(l_nY/2)));

	    m_pcPrimitive->CalculateSize();
    }

    return TRUE;
}



