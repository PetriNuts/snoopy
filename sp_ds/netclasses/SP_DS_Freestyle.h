//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Ckruege2 $
// $Version: 0.0 $
// $Date: 2009/06/17
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_FREESTYLE_H__
#define __SP_DS_FREESTYLE_H__

#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

class SP_DS_Freestyle: public SP_DS_SimpleGraph
{
private:
		
protected:

public:
    SP_DS_Freestyle();
    SP_DS_Freestyle(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);

};

#endif  //__SP_DS_FREESTYLE_H__
