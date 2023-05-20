
#ifndef __SP_DS_FUZZY_ColSPN_H__
#define  __SP_DS_FUZZY_ColSPN_H__
#include "sp_ds/netclasses/SP_DS_ColSPN.h"
class SP_MDI_Doc;
class SP_DS_Fuzzy_ColSPN :public SP_DS_ColSPN {
private:

protected:

public:

	SP_DS_Fuzzy_ColSPN();
	SP_DS_Fuzzy_ColSPN(const wxString& p_pchName);

	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
};

#endif //  __SP_DS_FUZZY_ColSPN_H__