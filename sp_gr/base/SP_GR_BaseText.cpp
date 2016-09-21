//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Text Primitive Implementation
//////////////////////////////////////////////////////////////////////
#include <wx/tokenzr.h>

#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_gr/eventhandler/SP_GRM_AttributeHandler.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_core/SP_Core.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "sp_utilities.h"


#include "snoopy.h"

SP_GR_BaseText::SP_GR_BaseText(const wxString& p_sText, double p_nWidth, double p_nHeight)
:wxTextShape(p_nWidth, p_nHeight),
m_bHideDimensions(FALSE)
{
	SP_MDI_Doc *l_cDoc = SP_Core::Instance()->GetRootDocument();
	wxString l_sNC = wxT("");

	if (l_cDoc) {
		l_sNC = l_cDoc->GetNetclassName();
	}
	SetNormalFont(wxGetApp().GetFontPrefs()->GetNormalFont(l_sNC, wxT("Default")));
    SetSelectFont(wxGetApp().GetFontPrefs()->GetSelectFont(l_sNC, wxT("Default")));
	SetFont(GetNormalFont());
    SetFormatMode(FORMAT_SIZE_TO_CONTENTS);
    SetEventHandler(new SP_GRM_AttributeHandler(this, this));
}

SP_GR_BaseText::~SP_GR_BaseText()
{

}

void
SP_GR_BaseText::Select(bool p_bSelect, wxDC* p_pcDC)
{
    wxTextShape::Select(p_bSelect, p_pcDC);

	if (p_bSelect){
			wxFont* l_pcFont = GetSelectFont();
			SetFont(l_pcFont);

	} else{
			wxFont* l_pcFont = GetNormalFont();
			SetFont(l_pcFont);
	} //endif
}

void
SP_GR_BaseText::MakeControlPoints()
{
    if (m_bHideDimensions)
        return;
    if (GetSelectFont())
        SetFont(GetSelectFont());
}

void
SP_GR_BaseText::DeleteControlPoints(wxDC* p_pcDC)
{
    if (m_bHideDimensions)
        return;
    if (GetNormalFont())
        SetFont(GetNormalFont());
}

void
SP_GR_BaseText::ResetControlPoints()
{
    if (m_bHideDimensions)
        return;
    wxTextShape::ResetControlPoints();
}

void
SP_GR_BaseText::Move(wxDC& p_cDC, double p_nX, double p_nY, bool p_bDisplay)
{
    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic &&
        l_pcGraphic->GetGraphicParent())
    {
        double l_nX = p_nX - l_pcGraphic->GetGraphicParent()->GetPosX();
        double l_nY = p_nY - l_pcGraphic->GetGraphicParent()->GetPosY();
        l_pcGraphic->SetOffsetX(l_nX);
        l_pcGraphic->SetOffsetY(l_nY);
    }

    wxTextShape::Move(p_cDC, p_nX, p_nY, p_bDisplay);
}

void
SP_GR_BaseText::OnDrawOutline(wxDC& p_cDC, double p_nX, double p_nY, double p_nW, double p_nH)
{
    if (m_bHideDimensions)
        return;

    SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(this);
    if (l_pcGraphic &&
        !l_pcGraphic->GetShow())
    {
        return;
    }
    wxShape::OnDrawOutline(p_cDC, p_nX, p_nY, p_nW, p_nH);
}

bool
SP_GR_BaseText::CalculateDimensions(const wxString& p_sVal)
{
    if (!GetCanvas())
        return FALSE;

    wxClientDC l_cDC(GetCanvas());
    GetCanvas()->DoPrepareDC(l_cDC);

    l_cDC.SetFont(*(GetFont()));
    FormatText(l_cDC, p_sVal);

    return TRUE;
}

bool
SP_GR_BaseText::HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance)
{
    if (!m_bHideDimensions)
        return wxTextShape::HitTest(p_nX, p_nY, p_pnAttach, p_pnDistance);

    return FALSE;
}

wxFont* const
SP_GR_BaseText::GetSelectFont()
{
	return m_selectFont;
}

void
SP_GR_BaseText::SetSelectFont(wxFont* p_font)
{
  m_selectFont = p_font;
  SetFont(p_font);

}


wxFont* const
SP_GR_BaseText::GetNormalFont()
{
	return m_normalFont;
}

void
SP_GR_BaseText::SetNormalFont(wxFont* p_font)
{
  m_normalFont = p_font;
	SetFont(p_font);
}
