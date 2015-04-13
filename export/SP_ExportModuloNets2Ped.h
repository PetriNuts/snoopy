//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/09/03 $
// Short Description: writes SimplePed from Modulo Nets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportModuloNets2Ped_H__
#define __SP_ExportModuloNets2Ped_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportModuloNets2Ped:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
  bool m_changeColour;
  wxString m_edgeClass;
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);

  virtual bool WriteClasses(SP_DS_Graph* p_pcVal,
				 wxXmlNode* p_pcRoot);

  bool
  WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
  				wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Discrete Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Discrete Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("Discrete Petri Net Format");}

};

#endif // __SP_ExportModuloNets2Ped_H__

