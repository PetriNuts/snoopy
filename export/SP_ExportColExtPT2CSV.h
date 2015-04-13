//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/05/26 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColExtPT2CSV__
#define __SP_ExportColExtPT2CSV__

#include "export/SP_ExportPT.h"

class SP_ExportColExtPT2CSV: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();	
  
  virtual bool DoWrite();

 public:
  SP_ExportColExtPT2CSV();
  virtual ~SP_ExportColExtPT2CSV();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to CSV");}
  inline virtual wxString GetDescr()
    { return wxT("Export Nodevalues in CSV Format");}
  virtual wxString GetExtension()
    { return wxT("csv");}
  virtual wxString GetFileDescr()
    { return wxT("CSV Files");}

  public :

    
    
    

};

#endif // __SP_ExportColExtPT2CSV__

