//////////////////////////////////////////////////////////////////////
// $Author: dbayer $
// $Version: 0.1 $
// $Date: 2008/09/08 $
// Short Description: writes SimplePed from MusicPed
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTMUSICPED2PED_H__
#define __SP_EXPORTMUSICPED2PED_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportMusicPed2Ped:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);
  virtual bool WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

  inline virtual wxString GetName()
    { return wxT("Export to PT");}
  inline virtual wxString GetDescr()
    { return wxT("Export to PT Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("Music Petri Net Files");}

  public :

};

#endif // __SP_EXPORTMUSICPED2PED_H__
