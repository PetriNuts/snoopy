// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 02.03.2020
// Short Description:Fuzzy coloured Cont. Petri Net Class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FUZZY_ColCPN_H__
#define __SP_DS_FUZZY_ColCPN_H__

#include "sp_ds/netclasses/SP_DS_ColCPN.h"


#define SP_DS_CONTINUOUSPED_PLACEMARKINGLIST
#define SP_DS_CONTINUOUSPED_PARAMMARKINGLIST
#define SP_DS_CONTINUOUSPED_EQUATIONPATTERN


class SP_DS_Fuzzy_ColCPN : public SP_DS_ColCPN
{
private:
protected:
public:
	SP_DS_Fuzzy_ColCPN();
	SP_DS_Fuzzy_ColCPN(const wxString& p_pchName);

	virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);

	virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
	virtual bool NodeRequirement(SP_DS_Node* p_pcNode);
	virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2);
	virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
};

 


#endif // __SP_DS_FUZZY_ColCPN_H__