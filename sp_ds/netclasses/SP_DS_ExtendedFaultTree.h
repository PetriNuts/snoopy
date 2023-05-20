//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2004/11/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EXTENDEDFAULTTREE_H__
#define __SP_DS_EXTENDEDFAULTTREE_H__

#include "sp_ds/netclasses/SP_DS_FaultTree.h"

class SP_DS_ExtendedFaultTree: public SP_DS_FaultTree
{
private:
protected:
public:
    SP_DS_ExtendedFaultTree();
    SP_DS_ExtendedFaultTree(const wxString& p_pchName);

	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
};

#endif // __SP_DS_EXTENDEDFAULTTREE_H__
