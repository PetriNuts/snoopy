//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/12/17 11:55:00 $
// Short Description: XML Reader, reads graph structures
//////////////////////////////////////////////////////////////////////
#ifndef __SP_XMLREADER_H__
#define __SP_XMLREADER_H__

#include <wx/xml/xml.h>

#include "sp_core/SP_Reader.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Metadata.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_XmlReader: public SP_Reader
{
private:
    map<unsigned long, SP_Data*> m_mId2Data;
    map<unsigned long, SP_Graphic*> m_mId2Graphic;
    bool FillInMap(SP_Data* p_pcData);
    bool FillInMap(SP_Graphic* p_pcGraphic);

    SP_Data* GetDataById(wxXmlNode* p_pcRoot, const wxString& p_pchAttr);
    SP_Graphic* GetGraphicById(wxXmlNode* p_pcRoot, const wxString& p_pchAttr);

    bool ReadPenColour(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);
    bool ReadBrushColour(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);
    //Old CPN Transformer
    void TransformOldCPN(SP_DS_Attribute* p_pcVal,wxString p_sVal);

    void RemoveInitialElements(SP_DS_Graph* p_pcGraph);

protected:
    virtual void Init();

    virtual bool CheckRoot(wxXmlNode* p_pcVal);

    virtual bool ReadNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool ReadClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool ReadNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool ReadNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot,bool p_sVal=false);

/* inserted by slehrack 09.06.2007 */
    virtual bool ReadColListAttribute( SP_DS_ColListAttribute* p_pcVal, wxXmlNode* p_pcRoot );
/* insertion end */

    virtual bool ReadData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadGraphics(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadEdgeGraphics(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadEdgeData(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);

    //by sl
    //for load old parameter and markinglist
    virtual bool ReadOldNodeClass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadOldNode(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadOldAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool ReadNewConstant(SP_DS_Node* p_pcVal, const wxString& p_pchAttr, wxXmlNode* p_pcRoot);
    virtual bool ReadValueList( SP_DS_ColListAttribute* p_pcVal, wxXmlNode* p_pcRoot);


public:
    SP_XmlReader();
    virtual ~SP_XmlReader();

    virtual bool Read(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);
    virtual bool Read(SP_DS_Graph* p_pcGraph, wxInputStream& p_InputStream);
};

#endif // __SP_XMLREADER_H__

