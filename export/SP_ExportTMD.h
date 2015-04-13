//////////////////////////////////////////////////////////////////////
// $Author: R2 $
// $Version: 0.1 $
// $Date: 2007/02/06 $
// Short Description: export to ina tmd
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTMD__
#define __SP_EXPORTMD__

#include "export/SP_ExportPT.h"

class SP_ExportTMD: public SP_ExportPT
{
 protected:
  bool WriteTransitions();	
  virtual bool DoWrite();

 public:
  SP_ExportTMD();
  virtual ~SP_ExportTMD();

  bool AcceptsDoc(SP_MDI_Doc* p_doc);

  inline virtual wxString GetName()
    { return wxT("Export to INA - time durations");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in INA Format TMD");}
  virtual wxString GetExtension()
    { return wxT("tmd");}
  virtual wxString GetFileDescr()
    { return wxT("INA Files TMD");}

  public :

     
     
    

};

#endif // __SP_EXPORTMD__

