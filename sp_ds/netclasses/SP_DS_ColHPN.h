//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: F. Liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/11/1 11:55:00 $
// Short Description: colored Stochastic Petri nets
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_ColHPN_H__
#define __SP_DS_ColHPN_H__

#include "sp_ds/netclasses/SP_DS_HybridPN.h"

class SP_MDI_Doc;

class SP_DS_ColHPN : public SP_DS_HybridPN
{

  private:

  protected:

  public:

    SP_DS_ColHPN();
    SP_DS_ColHPN( const wxString& p_pchName );

    virtual SP_DS_Graph* CreateGraph( SP_DS_Graph* p_pcGraph );
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
    virtual bool NodeRequirement( SP_DS_Node* p_pcNode );
    virtual bool EdgeRequirement( SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2 );
    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);

};

#endif // __SP_DS_ColHPN_H__
