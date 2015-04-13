//////////////////////////////////////////////////////////////////////
// $Author: R2 $
// $Version: 0.1 $
// $Date: 2007/02/06 $
// Short Description: writes Time PT net from SimplePed
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPED2TIMEPT_H__
#define __SP_EXPORTPED2TIMEPT_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportPed2TimePT:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Time PT Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Time PT Net Format");}
  virtual wxString GetExtension()
    { return wxT("tpn");}
  virtual wxString GetFileDescr()
    { return wxT("Time PT Net Files");}

  public :

     
     
    

};

#endif // __SP_EXPORTPED2TIMEPT_H__

