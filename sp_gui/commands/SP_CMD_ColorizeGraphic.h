//////////////////////////////////////////////////////////////////////
// $Source: SP_CMD_ColorizeGraphic.h $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/29 $
// Short Description: this class (derived from wxCommand) colors graphics
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_CMD_COLORIZEGRAPHIC_H__)
#define __SP_CMD_COLORIZEGRAPHIC_H__

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "sp_core/base/SP_Graphic.h"


class SP_CMD_ColorizeGraphic : public wxCommand  {

private:
	SP_Graphic *m_pcGraphic;
	wxColour m_cBrushColor, m_cSavedBrushColor;
	wxColour m_cPenColor, m_cSavedPenColor;

	//this function actually does the coloring
	bool Colorize(wxColour p_cBrushColor, wxColour p_cPenColor);

public:
	SP_CMD_ColorizeGraphic(SP_Graphic *p_pcGraphic, wxColour p_cBrushColor, wxColour p_cPenColor = *wxBLACK);
	virtual ~SP_CMD_ColorizeGraphic();

	//implementing the wxCommand interface
	bool Do();
	bool Undo();

};

#endif // !defined(__SP_CMD_COLORIZEGRAPHIC_H__)
