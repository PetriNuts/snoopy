//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/06/05 11:55:00 $
// Short Description: PNML Export
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPNML_H__
#define __SP_EXPORTPNML_H__

#include <wx/xml/xml.h>
#include "sp_core/SP_Writer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_ExportPNML: public SP_ExportRoutine, public SP_Error
{
private:
    SP_DS_Graph* m_pcGraph;
    unsigned long m_ArcId;

    bool WriteText(const wxString& p_sText, wxXmlNode* p_pcRoot);
    bool WriteName(const wxString& p_sName, wxXmlNode* p_pcRoot);

protected:
    virtual void Init();

    virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot);

    virtual bool WriteData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot);
public:
    SP_ExportPNML();
    virtual ~SP_ExportPNML();

    virtual bool Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);

	virtual bool AcceptsDoc(SP_MDI_Doc*);
	virtual bool Write(SP_MDI_Doc*, const wxString&);

	inline virtual wxString GetName()
	{
		return wxT("Export to PNML");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export a Petri Net into PNML Format");
	}
	virtual wxString GetExtension()
	{
		return wxT("pnml");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("PNML Files");
	}

};

#endif // __SP_EXPORTPNML_H__

