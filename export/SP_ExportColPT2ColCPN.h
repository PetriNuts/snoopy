//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/09/01 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportColPT2ColCPN_H__
#define __SP_ExportColPT2ColCPN_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportColPT2ColCPN:  public SP_ExportRoutine, private SP_XmlWriter
{

 private:

	 wxXmlNode* m_pcTransitionNodeclass;

 protected:

  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;

  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot);

  virtual bool WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);

  bool WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  bool WriteTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  bool WriteParameter( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);


 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Colored Continuous Petri Net"); }

  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Petri Net into Colored Continuous Petri Net Format");}

  virtual wxString GetExtension()
    { return wxT("colcpn");}

  virtual wxString GetFileDescr()
    { return wxT("Colored Continuous Petri Net Files");}

  public :

    
    
    

};

#endif // __SP_ExportColPT2ColCPN_H__

