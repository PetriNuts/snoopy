/*
* SP_DS_HybridPN.h
* $Author: MGeorge Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 27.1.2019
* Short Description: Fuzzy Hybrid Petri Net
*/
//======================================================================================

#ifndef SP_DS_FUZZYHYBRIDPN_H_
#define SP_DS_FUZZYHYBRIDPN_H_

#include "sp_ds/netclasses/SP_DS_ExtPT.h"
#include "sp_ds/netclasses/SP_DS_HybridPN.h"

class SP_DS_FuzzyHybridPN : public SP_DS_HybridPN
{
private:
protected:
public:
	//default constructor
	SP_DS_FuzzyHybridPN();

	SP_DS_FuzzyHybridPN(const wxString& p_pchName);

	//Hybrid PN specification
	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap = SP_MapString2Int());

	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);

	//Connectivity specification
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);

};


#endif /* SP_DS_FUZZYHYBRIDPN_H_ */
