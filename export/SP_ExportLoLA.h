//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to LoLA
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTLOLA__
#define __SP_EXPORTLOLA__

#include "export/SP_ExportPT.h"

class SP_ExportLoLA: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteMarking();
  bool WriteTransitions();
  virtual bool DoWrite();

  wxString GetName(SP_Place* p);
  wxString GetName(SP_Transition* t);

 public:
  SP_ExportLoLA();
  virtual ~SP_ExportLoLA();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to LoLA");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in LoLA Format");}
  virtual wxString GetExtension()
    { return wxT("net");}
  virtual wxString GetFileDescr()
    { return wxT("LoLA Files");}

  public :

    
    
    

};

#endif // __SP_EXPORTLOLA__

