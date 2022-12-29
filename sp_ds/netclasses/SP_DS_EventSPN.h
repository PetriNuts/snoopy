//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_EVENTSPN_H__
#define __SP_DS_EVENTSPN_H__

#include "sp_ds/netclasses/SP_DS_ExtPT.h"

class SP_MDI_Doc;

class SP_DS_EventSPN : public SP_DS_ExtPT
{

  private:

  protected:

  public:

    SP_DS_EventSPN();
    SP_DS_EventSPN( const wxString& p_pchName );

    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mapAttributeValue = SP_MapString2Int());
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
    virtual bool NodeRequirement( SP_DS_Node* p_pcNode );
    virtual bool EdgeRequirement( SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2 );
    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);

};

#endif // __SP_DS_EVENTSPN_H__
