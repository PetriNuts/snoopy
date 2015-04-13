//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/09/2 11:40:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColCPN2ColSPN_H__
#define __SP_ExportColCPN2ColSPN_H__

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

class SP_ExportColCPN2ColSPN: public SP_XmlWriter, public SP_ExportRoutine
{
private:
protected:
  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);
  virtual bool WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot);

  bool WriteStochPlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
  bool WriteStochTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

public:

  // must be implemented in successors
  virtual bool AcceptsDoc(SP_MDI_Doc*);

  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Colored Stochastic Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Continuous Petri Net into Colored Stochastic Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("colspn");}
  virtual wxString GetFileDescr()
    { return wxT("Colored Stochastic Petri Net Files");}

};

#endif // __SP_ExportColCPN2ColSPN_H__

