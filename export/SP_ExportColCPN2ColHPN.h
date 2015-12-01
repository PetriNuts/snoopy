//////////////////////////////////////////////////////////////////////
// $Author: F. Liu $
// $Version: 0.2 $
// $Date: 2010/10/23 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportColCPN2ColHPN_H__
#define __SP_ExportColCPN2ColHPN_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportColCPN2ColHPN:  public SP_ExportRoutine, private SP_XmlWriter
{

 private:
	 wxString m_edgeClass;

 protected:

  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;

  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);

 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Colored  Hybrid Petri Net"); }

  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Continuous Petri Net into Colored Hybrid Petri Net Format");}

  virtual wxString GetExtension()
    { return wxT("colhpn");}

  virtual wxString GetFileDescr()
    { return wxT("Colored Hybrid Petri Net Files");}

  public :

    
    
    

};

#endif // __SP_ExportColCPN2ColHPN_H__

