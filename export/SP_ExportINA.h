//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to ina
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTINA__
#define __SP_EXPORTINA__

#include "export/SP_ExportPT.h"

class SP_ExportINA: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();	
  bool WritePlPrePost();	
  virtual bool DoWrite();

 public:
  SP_ExportINA();
  virtual ~SP_ExportINA();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

	
  inline virtual wxString GetName()
    { return wxT("Export to INA");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in INA Format");}
  virtual wxString GetExtension()
    { return wxT("pnt");}
  virtual wxString GetFileDescr()
    { return wxT("INA Files");}


  public :

    
    
    

};

#endif // __SP_EXPORTINA__

