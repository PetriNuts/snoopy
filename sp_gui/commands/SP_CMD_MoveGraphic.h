//////////////////////////////////////////////////////////////////////
// $Source: SP_CMD_MoveGraphic.h $
// $Author: CR $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2016/10/09 $
// Short Description: this class (derived from wxCommand) moves graphics
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_CMD_MOVEGRAPHIC_H__)
#define __SP_CMD_MOVEGRAPHIC_H__

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "sp_core/base/SP_Graphic.h"


class SP_CMD_MoveGraphic : public wxCommand  {

private:
	SP_GUI_Canvas *m_pcCanvas;
	std::list<wxShape*> m_lShapes;
	std::list<wxLineShape*> m_lLineShapes;
	std::list<wxRealPoint*> m_lLinePoints;
	double m_nX;
	double m_nY;

	//this function actually does the moving
	bool Move(double p_nX, double p_nY);

public:
	SP_CMD_MoveGraphic(SP_GUI_Canvas *p_pcCanvas, double p_nX, double p_nY);
	virtual ~SP_CMD_MoveGraphic();

	//implementing the wxCommand interface
	bool Do();
	bool Undo();

};

#endif // !defined(__SP_CMD_MOVEGRAPHIC_H__)
