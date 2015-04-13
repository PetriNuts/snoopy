//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/09/02 $
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColCPN2CSV__
#define __SP_ExportColCPN2CSV__

#include "export/SP_ExportPT.h"

class SP_ExportColCPN2CSV: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();	
  bool WriteParameters();	
  virtual bool DoWrite();

 public:
  SP_ExportColCPN2CSV();
  virtual ~SP_ExportColCPN2CSV();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to CSV");}
  inline virtual wxString GetDescr()
    { return wxT("Export Nodevalues in CSV Format");}
  virtual wxString GetExtension()
    { return wxT("csv");}
  virtual wxString GetFileDescr()
    { return wxT("CSV Files");}    
    

};

#endif // __SP_ExportColCPN2CSV__

