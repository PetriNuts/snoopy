//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/12/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_XMLWRITER_H__
#define __SP_XMLWRITER_H__

#include <wx/xml/xml.h>
#include "sp_core/SP_Writer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_XmlWriter: public SP_Writer
{
private:
    SP_DS_Graph* m_pcGraph;
    bool WriteRedColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);
protected:
    virtual void Init();

    virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot);
/* inserted by dscheibl 22.06.2005 */
    virtual bool WriteOppositeEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteRedEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
/* insertion end */
    virtual bool WriteAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot);

/* inserted by slehrack 06.06.2007 */
    virtual bool WriteColListAttribute( SP_DS_ColListAttribute* p_pcColList, wxXmlNode* p_pcRoot );
/* insertion end */

    virtual bool WriteData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);
/* inserted by dscheibl 22.06.2005 */
    virtual bool WriteOppositeData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteOppositeGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteOppositeEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteRedData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteRedGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteRedEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);
/* insertion end */
    virtual bool WriteColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);
public:
    SP_XmlWriter();
    virtual ~SP_XmlWriter();

    virtual bool Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);
    virtual bool Write(SP_DS_Graph* p_pcGraph, wxOutputStream& p_OutputStream);
};

#endif // __SP_XMLWRITER_H__

