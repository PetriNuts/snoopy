//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/23 $
// Short Description: export to maria
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTMARIA__
#define __SP_EXPORTMARIA__

#include "export/SP_ExportPT.h"

class SP_ExportMaria: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();
  virtual bool DoWrite();

 public:
  SP_ExportMaria();
  virtual ~SP_ExportMaria();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to Maria");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in Maria Format");}
  virtual wxString GetExtension()
    { return wxT("pn");}
  virtual wxString GetFileDescr()
    { return wxT("Maria Files");}

  public :

    
    
    

};

#endif // __SP_EXPORTMARIA__

