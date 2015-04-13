//////////////////////////////////////////////////////////////////////
// $source: alextov $
// $author (modification): akurth$
// $Version: 0.1 $
// $Date: 2006/10/12 $
// Short Description: writes Extended Fault Tree from Fault Tree
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTFTREE2EXTFTREE_H__
#define __SP_EXPORTFTREE2EXTFTREE_H__

#include "export/SP_ExportRoutine.h"
#include "sp_core/SP_XmlWriter.h"

class SP_ExportFTree2ExtFTree:  public SP_ExportRoutine, private SP_XmlWriter
{
 private:
 protected:
  virtual bool WriteNetclass(SP_DS_Netclass* p_pcVal,
			     wxXmlNode* p_pcRoot);
 public:

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Extended FaultTree");}
  inline virtual wxString GetDescr()
    { return wxT("Export to Extended Fault Tree Format");}
  virtual wxString GetExtension()
    { return wxT("speft");}
  virtual wxString GetFileDescr()
    { return wxT("Extended Fault Tree Files");}

  public :

     
     
    

};

#endif // __SP_EXPORTPED2EXTPT_H__

