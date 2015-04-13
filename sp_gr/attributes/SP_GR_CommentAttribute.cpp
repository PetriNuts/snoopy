//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Comment Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "snoopy.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_CommentAttribute::SP_GR_CommentAttribute(SP_DS_Attribute* p_pcParent)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseText(FormatText());

    wxShapeEvtHandler* l_pcEventHandler = m_pcPrimitive->GetEventHandler();
    wxDELETE(l_pcEventHandler);

    m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));

	SP_MDI_Doc *l_cDoc = SP_Core::Instance()->GetRootDocument();
	wxString l_sNC = wxT("");
	if (l_cDoc)
	{
		l_sNC = l_cDoc->GetNetclassName();
	}

	wxFont *tfnt = wxGetApp().GetFontPrefs()->GetSelectFont(l_sNC, wxT("Comments"));
    m_pcPrimitive->SetSelectFont(tfnt);

	tfnt = wxGetApp().GetFontPrefs()->GetNormalFont(l_sNC, wxT("Comments"));
	m_pcPrimitive->SetNormalFont(tfnt);

	m_pcPrimitive->SetFont(tfnt);


    m_pcPrimitive->SetHideDimensions(FALSE);
    m_pcPrimitive->SetSensitivityFilter(OP_ALL &~ OP_DRAG_LEFT);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_CommentAttribute::~SP_GR_CommentAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_CommentAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_CommentAttribute* l_pcNew = new SP_GR_CommentAttribute(l_pcParent);

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_CommentAttribute::FormatText()
{
    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");

    wxString l_pchVal = dynamic_cast<SP_DS_Attribute*>(GetParent())->GetValueString();

    if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (l_pchVal.Len() != 0));

    wxString l_sVal(l_pchVal);

    return l_sVal;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_CommentAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    wxString l_sVal = FormatText();

    m_pcPrimitive->AddText(l_sVal);
    m_pcPrimitive->CalculateDimensions(l_sVal);

    if (GetGraphicParent())
	{
		GetGraphicParent()->SetWidth(GetWidth());
		GetGraphicParent()->SetHeight(GetHeight());
	}

    return TRUE;
}

wxShape*
SP_GR_CommentAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

