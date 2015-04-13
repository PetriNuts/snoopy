//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WRITER_H__
#define __SP_WRITER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

class SP_Writer: public SP_Error
{
private:
    SP_DS_Graph* m_pcGraph;
    bool WriteColourInformation(SP_Graphic* p_pcVal);
protected:
    virtual void Init();

    virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal);
    virtual bool WriteClasses(SP_DS_Graph* p_pcVal);

    virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal);
    virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal);
    virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal);

    virtual bool WriteNode(SP_DS_Node* p_pcVal);
    virtual bool WriteEdge(SP_DS_Edge* p_pcVal);
    virtual bool WriteMetadata(SP_DS_Metadata* p_pcVal);
    virtual bool WriteAttribute(SP_DS_Attribute* p_pcVal);

    virtual bool WriteData(SP_Data* p_pcVal);
    virtual bool WriteGraphic(SP_Graphic* p_pcVal);
    virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal);
public:
    SP_Writer();
    virtual ~SP_Writer();

    virtual bool Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);
};

#endif // __SP_WRITER_H__

