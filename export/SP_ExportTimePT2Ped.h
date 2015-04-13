//////////////////////////////////////////////////////////////////////
// $Author: R2 $
// $Version: 0.1 $
// $Date: 2007/02/06 $
// Short Description: writes  SimplePed from Time PT net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTTIMEPT2PED_H__
#define __SP_EXPORTTIMEPT2PED_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"


class SP_ExportTimePT2Ped:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to time-free Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Time PT Net into time-free Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("time-free Petri Net Files");}

  public :

     
     
    

};

#endif // __SP_EXPORTTIMEPT2PED_H__


