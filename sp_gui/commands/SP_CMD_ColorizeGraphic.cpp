// SP_CMD_ColorizeGraphic.cpp: Implementierung der Klasse SP_CMD_ColorizeGraphic.
//
//////////////////////////////////////////////////////////////////////

#include "SP_CMD_ColorizeGraphic.h"
#include "sp_ds/SP_DS_Node.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_CMD_ColorizeGraphic::SP_CMD_ColorizeGraphic(SP_Graphic *p_pcGraphic, wxColour p_cBrushColor, wxColour p_cPenColor)
: wxCommand(true, wxT("Colorize Graphic")),
	m_pcGraphic(p_pcGraphic),
	m_cBrushColor(p_cBrushColor),
	m_cPenColor(p_cPenColor)
{
	if (m_pcGraphic) {
		m_cSavedBrushColor = m_pcGraphic->GetBrush()->GetColour();
		m_cSavedPenColor = m_pcGraphic->GetPen()->GetColour();
		if (m_cBrushColor == wxNullColour) m_cBrushColor = m_cSavedBrushColor;
		if (m_cPenColor == wxNullColour) m_cPenColor = m_cSavedPenColor;
	}
}

SP_CMD_ColorizeGraphic::~SP_CMD_ColorizeGraphic()
{ }

bool
SP_CMD_ColorizeGraphic::Do() {

	return Colorize(m_cBrushColor, m_cPenColor); 
}

bool
SP_CMD_ColorizeGraphic::Undo() {

	return Colorize(m_cSavedBrushColor, m_cSavedPenColor);
}

bool
SP_CMD_ColorizeGraphic::Colorize(wxColour p_cBrushColor, wxColour p_cPenColor) {

	if (m_cBrushColor == m_cSavedBrushColor && m_cPenColor == m_cSavedPenColor) return false;
	
	if (!m_pcGraphic) return false;
	SP_GUI_Canvas *l_pcCanvas = m_pcGraphic->GetCanvas(); 
	if (!l_pcCanvas) return false;

	m_pcGraphic->SetBrush(wxTheBrushList->FindOrCreateBrush(p_cBrushColor, m_pcGraphic->GetBrush()->GetStyle()));
	m_pcGraphic->SetPen(wxThePenList->FindOrCreatePen(p_cPenColor, m_pcGraphic->GetPen()->GetWidth(), m_pcGraphic->GetPen()->GetStyle()));
	m_pcGraphic->Update(true);

	l_pcCanvas->Refresh();

	return true;
}
