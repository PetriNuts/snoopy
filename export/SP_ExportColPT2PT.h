//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPT2PT_H__
#define __SP_ExportColPT2PT_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

#include "sp_core/SP_Vector.h"

class SP_ExportColPT2PT: public SP_XmlWriter, public SP_ExportRoutine
{  

private:
	SP_DS_Graph* m_pcExportGraph;

	map<wxString, SP_DS_Node*> m_mNewPlaceNodes;
	map<wxString, SP_DS_Node*> m_mNewTransitionNodes;
	SP_ListEdge m_lNewEdges;
	 
  protected:
    virtual bool WriteNetclass( SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteNodeclass( SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot );
    virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
	virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
    virtual bool WriteMetadataclass( SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot );

    bool WriteUnfoldedPlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot );
    bool WriteUnfoldedTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

  public:
    SP_ExportColPT2PT();
	~SP_ExportColPT2PT();

    virtual bool AcceptsDoc( SP_MDI_Doc* );
    virtual bool Write( SP_MDI_Doc*, const wxString& );

    inline virtual wxString GetName()
      { return wxT("Export to Petri Net"); }

    inline virtual wxString GetDescr()
      { return wxT("Export a Colored Petri Net into Petri Net Format"); }

    virtual wxString GetExtension()
      { return wxT("pn"); }

    virtual wxString GetFileDescr()
      { return wxT("PT Net Files"); }

};

#endif // __SP_ExportColPT2PT_H__


