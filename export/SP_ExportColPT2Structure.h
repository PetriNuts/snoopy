//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPT2Structure_H__
#define __SP_ExportColPT2Structure_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

#include "sp_core/SP_Vector.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


class SP_ExportColPT2Structure: public SP_XmlWriter, public SP_ExportRoutine
{  

private:

	wxString m_sNetClassName;
 
  protected:

    virtual bool WriteNetclass( SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteNodeclass( SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot );
    virtual bool WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot);//by george
    virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);

    bool WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot );
    bool WriteTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

  public:
    SP_ExportColPT2Structure();
	~SP_ExportColPT2Structure();
    virtual bool AcceptsDoc( SP_MDI_Doc* );


    virtual bool Write( SP_MDI_Doc*, const wxString& );

    virtual wxString GetName();
    virtual wxString GetDescr();
    virtual wxString GetExtension();
    virtual wxString GetFileDescr();      

};

#endif // __SP_ExportColPT2Structure_H__


