//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to ina
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTIM__
#define __SP_EXPORTIM__

#include "export/SP_ExportPT.h"

class SP_ExportTIM: public SP_ExportPT
{
 protected:
  bool WriteTransitions();	
  virtual bool DoWrite();

 public:
  SP_ExportTIM();
  virtual ~SP_ExportTIM();

  bool AcceptsDoc(SP_MDI_Doc* p_doc);

  inline virtual wxString GetName()
    { return wxT("Export to INA - time intervalls");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in INA Format TIM");}
  virtual wxString GetExtension()
    { return wxT("tim");}
  virtual wxString GetFileDescr()
    { return wxT("INA Files TIM");}

  public :

     
     
    

};

#endif // __SP_EXPORTIM__

