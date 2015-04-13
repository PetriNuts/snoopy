//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/07/01
// Short Description: Edge with multi linestyles and arrowstyles
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDEDGE_H__
#define __SP_GR_EXTENDEDEDGE_H__

#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/base/SP_GR_BaseEdge.h"

enum SP_EXTENDED_TYPE_EDGE
{
	SP_EXTENDED_TYPE_DEFAULT_EDGE,
	SP_EXTENDED_TYPE_DOTTED_EDGE,
	SP_EXTENDED_TYPE_DOTTED_DASHED_EDGE,
	SP_EXTENDED_TYPE_DASHED_EDGE
};

enum SP_EXTENDED_ARROW_TYPE_EDGE
{
	SP_EXTENDED_ARROW_TYPE_NONE_EDGE,
	SP_EXTENDED_ARROW_TYPE_HOLLOW_CIRCLE_EDGE,
	SP_EXTENDED_ARROW_TYPE_FILLED_CIRCLE_EDGE,
	SP_EXTENDED_ARROW_TYPE_ARROW_EDGE,
	SP_EXTENDED_ARROW_TYPE_DOUBLE_ARROW_EDGE,
	SP_EXTENDED_ARROW_TYPE_DOUBLE_CIRCLE_EDGE,
	SP_EXTENDED_ARROW_TYPE_CIRCLE_ARROW_EDGE
};


class SP_GR_BaseEdge;

class SP_GR_ExtendedEdge: public SP_GR_Edge
{
private:
	


protected:
    SP_GR_BaseEdge* m_pcPrimitive;
    SP_EXTENDED_TYPE_EDGE m_nType;
	size_t m_nThickness;
	SP_EXTENDED_ARROW_TYPE_EDGE m_nArrowTypeRight;
	size_t m_nArrowLeftSize;
	SP_EXTENDED_ARROW_TYPE_EDGE m_nArrowTypeLeft;
	size_t m_nArrowRightSize;
	wxPen* m_pcCanvasModPen;
	wxPen* m_pcCanvasModCoarsePen;

public:
    SP_GR_ExtendedEdge(SP_DS_Edge* p_pcParent,
		SP_EXTENDED_TYPE_EDGE p_nType = SP_EXTENDED_TYPE_DEFAULT_EDGE,size_t p_nThickness=1,
		SP_EXTENDED_ARROW_TYPE_EDGE p_nATypeLeft = SP_EXTENDED_ARROW_TYPE_NONE_EDGE,double p_sizeLeft = 10,
		SP_EXTENDED_ARROW_TYPE_EDGE p_nATypeRight = SP_EXTENDED_ARROW_TYPE_NONE_EDGE,double p_sizeRight = 10,wxColour p_pcColur= wxColour(0, 0, 0));
    virtual ~SP_GR_ExtendedEdge();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();

    void AddAnotherArrow(WXTYPE p_type = ARROW_ARROW,int p_end = ARROW_POSITION_END,double p_size = 10);
    virtual bool Update(bool p_bLocalOnly = FALSE);

	//-----------------------------------------------------
    // Accessors
    bool SetType(SP_EXTENDED_TYPE_EDGE p_nType);
    SP_EXTENDED_TYPE_EDGE GetType();

	bool SetThickness(size_t p_nThickness);
    size_t GetThickness();
    
	bool SetArrowTypeRight(SP_EXTENDED_ARROW_TYPE_EDGE m_nArrowTypeRight);
    SP_EXTENDED_ARROW_TYPE_EDGE GetArrowTypeRight();

	bool SetArrowTypeLeft(SP_EXTENDED_ARROW_TYPE_EDGE m_nArrowTypeLeft);
    SP_EXTENDED_ARROW_TYPE_EDGE GetArrowTypeLeft();

	bool SetDesignType(long p_nType);
    long GetDesignType();

    bool SetDesignThickness(long p_nThickness);
    long GetDesignThickness();

	bool SetDesignArrowTypeRight(long m_nArrowTypeRight);
    long GetDesignArrowTypeRight();

	bool SetDesignArrowTypeLeft(long m_nArrowTypeLeft);
    long GetDesignArrowTypeLeft();

	bool SetArrowLeftThickness(size_t l_nArrowLeftSize);
	bool SetArrowRightThickness(size_t l_nArrowLeftSize);
	size_t GetArrowLeftThickness();
	size_t GetArrowRightThickness();

	/*
	 * Set and getter for a modifical pen
	 */
	bool SetCanvasModPenStyle(wxColour colour,int thick,int style);
	inline wxPen* GetCanvasModPen() const { return m_pcCanvasModPen; }
    inline wxPen* GetCanvasModCoarsePen() const { return m_pcCanvasModCoarsePen; }
};

#endif // __SP_GR_EXTENDEDEDGE_H__
