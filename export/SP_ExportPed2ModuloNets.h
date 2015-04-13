//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/09/01 $
// Short Description: writes Extended PT net from SimplePed
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportPed2ModuloNets_H__
#define __SP_ExportPed2ModuloNets_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportPed2ModuloNets:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Modulo Nets");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Modulo Nets Format");}
  virtual wxString GetExtension()
    { return wxT("mpn");}
  virtual wxString GetFileDescr()
    { return wxT("Modulo Nets Files");}

};

#endif // __SP_ExportPed2ModuloNets_H__

