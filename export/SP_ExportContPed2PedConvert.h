//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/22 12:05:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCONTPED2PEDCONVERT_H__
#define __SP_EXPORTCONTPED2PEDCONVERT_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportContPed2PedConvert: public SP_XmlWriter, public SP_ExportRoutine
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

  virtual bool WriteAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot);

  bool WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
public:
  // must be implemented in successors
  virtual bool AcceptsDoc(SP_MDI_Doc*);

  virtual bool Write(SP_MDI_Doc*, const wxString&);
  inline virtual wxString GetName()
    { return wxT("Export to Discrete Petri Net - convert test edges and remove inhibitor edges");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Continuous Petri Net into Discrete Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("Discrete Petri Net Files");}

  public :

    
    
    

};

#endif // __SP_EXPORTCONTPED2PEDCONVERT_H__

