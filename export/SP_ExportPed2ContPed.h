//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/05/30 12:20:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPED2CONTPED_H__
#define __SP_EXPORTPED2CONTPED_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportPed2ContPed: public SP_XmlWriter, public SP_ExportRoutine
{
private:
protected:
  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);

public:
  // must be implemented in successors
  virtual bool AcceptsDoc(SP_MDI_Doc*);

  virtual bool Write(SP_MDI_Doc*, const wxString&);
  inline virtual wxString GetName()
    { return wxT("Export to Continuous Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Discrete Petri Net into Continuous Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("cpn");}
  virtual wxString GetFileDescr()
    { return wxT("Continuous Petri Net Files");}

};

#endif // __SP_EXPORTPED2CONTPED_H__

