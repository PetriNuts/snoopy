//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ms $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/06/04 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MTIDD_H__
#define __SP_DS_MTIDD_H__

#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

class SP_DS_MTIDD: public SP_DS_SimpleGraph
{
private:
protected:
public:
    SP_DS_MTIDD();
    SP_DS_MTIDD(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);

    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
};

#endif // __SP_DS_MTIDD_H__

