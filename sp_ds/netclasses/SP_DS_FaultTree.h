//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2004/11/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FAULTTREE_H__
#define __SP_DS_FAULTTREE_H__

#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

class SP_DS_FaultTree: public SP_DS_SimpleGraph
{
private:
protected:
public:
    SP_DS_FaultTree();
    SP_DS_FaultTree(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);

};

#endif // __SP_DS_FAULTTREE_H__
