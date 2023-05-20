//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: r2 $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/01/22 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MTBDD_H__
#define __SP_DS_MTBDD_H__

#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

class SP_DS_MTBDD: public SP_DS_SimpleGraph
{
private:
protected:
public:
    SP_DS_MTBDD();
    SP_DS_MTBDD(const wxString& p_pchName);

	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
};

#endif // __SP_DS_MTBDD_H__

