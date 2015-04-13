//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/23 $
// Short Description: writes Extended PT net from SimplePed
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPED2EXTPT_H__
#define __SP_EXPORTPED2EXTPT_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportPed2ExtPT:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Extended PT Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Extended PT Net Format");}
  virtual wxString GetExtension()
    { return wxT("xpn");}
  virtual wxString GetFileDescr()
    { return wxT("Extended PT Net Files");}

};

#endif // __SP_EXPORTPED2EXTPT_H__

