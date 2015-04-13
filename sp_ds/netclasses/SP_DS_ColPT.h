//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: F. Liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/10/20 11:55:00 $
// Short Description: colored Petri nets
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_ColPT_H__
#define __SP_DS_ColPT_H__

#include "sp_ds/netclasses/SP_DS_SimplePed.h"

class SP_MDI_Doc;

class SP_DS_ColPT : public SP_DS_SimplePed
{

  private:

  protected:

  public:

    SP_DS_ColPT();
    SP_DS_ColPT( const wxString& p_pchName );

    virtual SP_DS_Graph* CreateGraph( SP_DS_Graph* p_pcGraph );
    virtual bool CreateInitialElements(SP_DS_Graph* p_pcGraph);
    virtual bool NodeRequirement( SP_DS_Node* p_pcNode );
    virtual bool EdgeRequirement( SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcNode1, SP_Data* p_pcNode2 );
    virtual bool MetadataRequirement(SP_DS_Metadata* p_pcMetadata);

};

#endif // __SP_DS_ColPT_H__
