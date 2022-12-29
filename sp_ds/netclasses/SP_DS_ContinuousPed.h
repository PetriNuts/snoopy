//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/04/05 13:26:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_CONTINUOUSPED_H__
#define __SP_DS_CONTINUOUSPED_H__

#include "sp_ds/netclasses/SP_DS_BipartGraph.h"

#define SP_DS_CONTINUOUSPED_PLACEMARKINGLIST
#define SP_DS_CONTINUOUSPED_PARAMMARKINGLIST
#define SP_DS_CONTINUOUSPED_EQUATIONPATTERN


class SP_DS_ContinuousPed: public SP_DS_BipartGraph
{
private:
protected:
public:
    SP_DS_ContinuousPed();
    SP_DS_ContinuousPed(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
};

#endif // __SP_DS_CONTINUOUSPED_H__
