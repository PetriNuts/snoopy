/*
 * SP_Export2HPN.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 26.10.2010
 * Short Description:
 */
 //======================================================================================

#ifndef SP_EXPORT2HPN_H_
#define SP_EXPORT2HPN_H_

#include "sp_core/SP_XmlWriter.h"
#include "export/SP_ExportRoutine.h"

/*
 * Export QPN,SPN, and CPN into HPN.
 */
class SP_Export2HPN: public SP_XmlWriter, public SP_ExportRoutine
{
private:
protected:
  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot);

public:
  // must be implemented in successors
  virtual bool AcceptsDoc(SP_MDI_Doc*);

  virtual bool Write(SP_MDI_Doc*, const wxString&);
  inline virtual wxString GetName()
    { return wxT("Export to Hybrid Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Hybrid Petri Nets format");}
  virtual wxString GetExtension()
    { return wxT("hpn");}
  virtual wxString GetFileDescr()
    { return wxT("Hybrid Petri Net Files");}

};


#endif /* SP_EXPORT2HPN_H_ */
