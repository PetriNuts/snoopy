//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/09/06 11:40:00 $
// Short Description: Edge with arrow head Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_FAULTEDGE_H__
#define __SP_GR_FAULTEDGE_H__

#include "sp_gr/SP_GR_Edge.h"

class SP_GR_BaseEdge;

enum SP_EXTENDED_ARC_TYPE
{
	SP_EXTENDED_TYPE_UNDIRECTED,
	SP_EXTENDED_TYPE_DIRECTED
};
class SP_GR_FaultEdge: public SP_GR_Edge
{
private:
protected:
    SP_GR_BaseEdge* m_pcPrimitive;
public:
    SP_GR_FaultEdge(SP_DS_Edge* p_pcParent,
		    WXTYPE p_type = ARROW_ARROW,
		    int p_end = ARROW_POSITION_END,
		    double p_size = 10);
    virtual ~SP_GR_FaultEdge();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
    virtual bool Update(bool p_bLocalOnly = FALSE);

    void AddAnotherArrow(WXTYPE p_type = ARROW_ARROW,
			 int p_end = ARROW_POSITION_END,
			 double p_size = 10);
};

#endif // __SP_GR_FAULTEDGE_H__
