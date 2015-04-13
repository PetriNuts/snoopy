//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/24 $
// Short Description: export to apnn
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTAPNN__
#define __SP_EXPORTAPNN__

#include "export/SP_ExportPT.h"

class SP_ExportAPNN: public SP_ExportPT
{
 protected:
  bool WriteConstants(); //by sl
  bool WritePlaces();
  bool WriteTransitions();	
  bool WriteParameters();
  bool WriteArcs();	
  virtual bool DoWrite();


 public:
  SP_ExportAPNN();
  virtual ~SP_ExportAPNN();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to APNN ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in APNN Format");}
  virtual wxString GetExtension()
    { return wxT("apnn");}
  virtual wxString GetFileDescr()
    { return wxT("APNN Files");}

  public :

    
    
    

};

#endif // __SP_EXPORTAPNN__

