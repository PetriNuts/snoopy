//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: F. Liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/08/05 13:28:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColCPN_H__
#define __SP_DS_ColCPN_H__

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"


#define SP_DS_CONTINUOUSPED_PLACEMARKINGLIST
#define SP_DS_CONTINUOUSPED_PARAMMARKINGLIST
#define SP_DS_CONTINUOUSPED_EQUATIONPATTERN


class SP_DS_ColCPN: public SP_DS_ContinuousPed
{
private:
protected:
public:
    SP_DS_ColCPN();
    SP_DS_ColCPN(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
};

#endif // __SP_DS_ColCPN_H__
