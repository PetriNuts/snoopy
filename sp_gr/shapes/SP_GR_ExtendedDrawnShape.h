//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/28 13:10:00 $
// Short Description: Drawn Shape Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDDRAWNSHAPE_H__
#define __SP_GR_EXTENDEDDRAWNSHAPE_H__

#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/shapes/SP_GR_DrawnShape.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"

enum SP_EXTENDED_TYPE
{
	SP_EXTENDED_TYPE_DEFAULT,
	SP_EXTENDED_TYPE_TWO,
	SP_EXTENDED_TYPE_DOTTED,
	SP_EXTENDED_TYPE_DOTTED_DASHED,
	SP_EXTENDED_TYPE_DASHED
};


class SP_GR_ExtendedDrawnShape: public SP_GR_DrawnShape
{
private:
protected:
    SP_EXTENDED_TYPE m_nType;
	/* changed by ckruege2 17.06.2009 */
	wxPen* m_pcCanvasModPen;
	wxPen* m_pcCanvasModCoarsePen;
	/* changing end */
public:
    SP_GR_ExtendedDrawnShape(SP_DS_Node* p_pcParent, SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT, int p_nThickness=1);
    virtual ~SP_GR_ExtendedDrawnShape();

    bool SetType(SP_EXTENDED_TYPE p_nType);
    SP_EXTENDED_TYPE GetType();

	bool SetDesignType(long p_nType);
    long GetDesignType();

    SP_EXTENDED_TYPE ReadDefaultDesignType();

	/*
	 * Set and getter for a modifical pen
	 */
	bool SetCanvasModPenStyle(wxColour colour,int thick,int style);
	inline wxPen* GetCanvasModPen() const { return m_pcCanvasModPen; }
    inline wxPen* GetCanvasModCoarsePen() const { return m_pcCanvasModCoarsePen; }
};

#endif // __SP_GR_EXTENDEDDRAWNSHAPE_H__


