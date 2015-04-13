//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/23 $
// Short Description: export to prod
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPROD__
#define __SP_EXPORTPROD__

#include "export/SP_ExportPT.h"

class SP_ExportProd: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();
  virtual bool DoWrite();

 public:
  SP_ExportProd();
  virtual ~SP_ExportProd();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to Prod");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in Prod Format");}
  virtual wxString GetExtension()
    { return wxT("net");}
  virtual wxString GetFileDescr()
    { return wxT("Prod Files");}

  public :

    
    
    

};

#endif // __SP_EXPORTPROD__

