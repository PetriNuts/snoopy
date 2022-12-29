//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 26.01.2019
// Short Description:Fuzzy Cont. Petri Net Class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FUZZYCPN_H__
#define __SP_DS_FUZZYCPN_H__
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/netclasses/SP_DS_BipartGraph.h"

//#define SP_DS_CONTINUOUSPED_PLACEMARKINGLIST
//#define SP_DS_CONTINUOUSPED_PARAMMARKINGLIST
//#define SP_DS_CONTINUOUSPED_EQUATIONPATTERN


class SP_DS_FuzzyCPN : public SP_DS_ContinuousPed
{
private:
protected:
public:
	SP_DS_FuzzyCPN();
	SP_DS_FuzzyCPN(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
};

#endif // __SP_DS_FUZZYCPN_H__
