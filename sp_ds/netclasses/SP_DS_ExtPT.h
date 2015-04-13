//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/22 $
// Short Description: PT nets with extensions
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EXTPT_H__
#define __SP_DS_EXTPT_H__

#include "sp_ds/netclasses/SP_DS_SimplePed.h"

class SP_DS_ExtPT: public SP_DS_SimplePed
{
private:
protected:
public:
    SP_DS_ExtPT();
    SP_DS_ExtPT(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);

    virtual bool EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass,
			 SP_Data* p_pcNode1, SP_Data* p_pcNode2);

};

#endif // __SP_DS_EXTPT_H__
