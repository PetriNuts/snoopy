//////////////////////////////////////////////////////////////////////
// $Author: dbayer $
// $Version: 0.1 $
// $Date: 2008/09/08 $
// Short Description: writes Music Petri Net net from SimplePed
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPED2MUSICPED_H__
#define __SP_EXPORTPED2MUSICPED_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportPed2MusicPed:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Music Petri Net");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Music Petri Net Format");}
  virtual wxString GetExtension()
    { return wxT("spm");}
  virtual wxString GetFileDescr()
    { return wxT("Music Petri Net Files");}

  public :

     
     
    

};

#endif // __SP_EXPORTPED2MUSICPED_H__

