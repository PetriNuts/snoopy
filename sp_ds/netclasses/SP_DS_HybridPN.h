/*
 * SP_DS_HybridPN.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 5.7.2010
 * Short Description:
 */
 //======================================================================================

#ifndef SP_DS_HYBRIDPN_H_
#define SP_DS_HYBRIDPN_H_

#include "sp_ds/netclasses/SP_DS_ExtPT.h"

class SP_DS_HybridPN : public SP_DS_ExtPT
{
   private:
   protected:
   public:
	    //default constructor
	    SP_DS_HybridPN();

	    SP_DS_HybridPN(const wxString& p_pchName);

	    //Hybrid PN specification
	    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());

	    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	    virtual bool NodeRequirement(SP_DS_Node* p_pcNode);

	    //Connectivity specification
	    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);

};


#endif /* SP_DS_HYBRIDPN_H_ */
