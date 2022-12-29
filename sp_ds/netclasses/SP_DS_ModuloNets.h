//////////////////////////////////////////////////////////////////////
// $Author: rr $
// $Version: 0.1 $
// $Date: 2008/01/07 $
// Short Description:  Modulo Nets
///////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MODNET_H__
#define __SP_DS_MODNET_H__

#include "sp_ds/netclasses/SP_DS_SimplePed.h"

class SP_DS_ModuloNets: public SP_DS_SimplePed
{
private:
protected:
public:
    SP_DS_ModuloNets();
    SP_DS_ModuloNets(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass,
						 SP_Data* p_pcNode1,
						 SP_Data* p_pcNode2);
};

#endif // __SP_DS_MODNET_H__
