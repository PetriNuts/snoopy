// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 07.03.2020
// Short Description:Fuzzy coloured Hybrid Petri Net Class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FUZZY_ColHPN_H__
#define __SP_DS_FUZZY_ColHPN_H__

#include "sp_ds/netclasses/SP_DS_ColHPN.h"


#define SP_DS_CONTINUOUSPED_PLACEMARKINGLIST
#define SP_DS_CONTINUOUSPED_PARAMMARKINGLIST
#define SP_DS_CONTINUOUSPED_EQUATIONPATTERN


class SP_DS_Fuzzy_ColHPN : public SP_DS_ColHPN
{
private:
protected:
public:
	SP_DS_Fuzzy_ColHPN();
	SP_DS_Fuzzy_ColHPN(const wxString& p_pchName);

	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
};




#endif // __SP_DS_FUZZY_ColHPN_H__