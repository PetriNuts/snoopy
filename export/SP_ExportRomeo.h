//////////////////////////////////////////////////////////////////////
// $Author: ms $
// $Version: 0.1 $
// $Date: 2007/06/7 $
// Short Description: export to romeo
//////////////////////////////////////////////////////////////////////

#ifndef __SP_EXPORTROMEO__
#define __SP_EXPORTROMEO__

#include "export/SP_ExportPT.h"

class SP_ExportRomeo: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();
  bool WriteArcs();
  virtual bool DoWrite();

 public:
  SP_ExportRomeo();
  virtual ~SP_ExportRomeo();

  bool AcceptsDoc(SP_MDI_Doc* p_doc);

  inline virtual wxString GetName()
    { return wxT("Export to Romeo");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Time Petri Net in Romeo");}
  virtual wxString GetExtension()
    { return wxT("xml");}
  virtual wxString GetFileDescr()
    { return wxT("Romeo");}

  private :

};

#endif // __SP_EXPORTIM__

