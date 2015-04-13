//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/11/15 $
// Short Description: export of P/T nets to TINA
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTTINA__
#define __SP_EXPORTTINA__

#include "export/SP_ExportPT.h"

class SP_ExportTINA: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();
  virtual bool DoWrite();

  public:
  
    SP_ExportTINA();

  virtual ~SP_ExportTINA();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to TINA");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in TINA Format");}
  virtual wxString GetExtension()
    { return wxT("net");}
  virtual wxString GetFileDescr()
    { return wxT("TINA Files");}


  public :

    
    
    

};

#endif // __SP_EXPORTTINA__

