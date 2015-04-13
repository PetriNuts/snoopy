//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_SIMPLEGRAPH_H__
#define __SP_DS_SIMPLEGRAPH_H__

#include "sp_ds/SP_DS_Netclass.h"

class SP_DS_SimpleGraph: public SP_DS_Netclass
{
private:
protected:
public:
    SP_DS_SimpleGraph();
    SP_DS_SimpleGraph(const wxString& p_pchName);

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);
};

#endif // __SP_DS_SIMPLEGRAPH_H__
