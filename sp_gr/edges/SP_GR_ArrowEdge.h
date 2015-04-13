//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Edge with arrow head Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ARROWEDGE_H__
#define __SP_GR_ARROWEDGE_H__

#include "sp_gr/SP_GR_Edge.h"

class SP_GR_BaseEdge;

class SP_GR_ArrowEdge: public SP_GR_Edge
{
private:
	WXTYPE m_nType;
protected:
    SP_GR_BaseEdge* m_pcPrimitive;
public:
    SP_GR_ArrowEdge(SP_DS_Edge* p_pcParent,
		    WXTYPE p_type = ARROW_ARROW,
		    int p_end = ARROW_POSITION_END,
		    double p_size = 10);
    virtual ~SP_GR_ArrowEdge();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();

    void AddAnotherArrow(WXTYPE p_type = ARROW_ARROW,
			 int p_end = ARROW_POSITION_END,
			 double p_size = 10);

	bool SetDesignType(long p_nType);
    long GetDesignType();
};

#endif // __SP_GR_ARROWEDGE_H__
