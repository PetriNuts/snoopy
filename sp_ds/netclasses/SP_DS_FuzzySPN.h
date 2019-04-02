/*
* SP_DS_FuzzySPN.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 15.12.2018
* Short Description:Fuzzy Stochastic Petri Net Class
*/
//======================================================================================

#ifndef SP_DS_FUZZYSPN_H_
#define SP_DS_FUZZYSPN_H_

#include "sp_ds/netclasses/SP_DS_EventSPN.h"

class SP_DS_FuzzySPN : public SP_DS_EventSPN
{
private:
protected:
public:
	//default constructor
	SP_DS_FuzzySPN();

	SP_DS_FuzzySPN(const wxString& p_pchName);

	//Hybrid PN specification
	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);

	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);

	//Connectivity specification
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);

};


#endif /* SP_DS_FUZZYSPN_H_*/
