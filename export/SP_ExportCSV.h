//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2009/07/03 $
// Short Description: export to csv
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCSV__
#define __SP_EXPORTCSV__

#include "export/SP_ExportPT.h"

class SP_ExportCSV: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();	
  bool WriteParameters();	
  virtual bool DoWrite();

 public:
  SP_ExportCSV();
  virtual ~SP_ExportCSV();

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

#endif // __SP_EXPORTCSV__

