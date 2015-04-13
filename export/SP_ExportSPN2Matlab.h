//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/04/30 $
// Short Description: export to Matlab
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTSPN2MATLAB__
#define __SP_EXPORTSPN2MATLAB__

#include "export/SP_ExportPT.h"

class SP_ExportSPN2Matlab: public SP_ExportPT
{
 private:
	wxString SubstituteNames(const wxString& p_Func);

 protected:
  bool WritePlaces();
  bool WriteTransitions();
  bool WriteConstants();
  virtual bool DoWrite();

 public:
  SP_ExportSPN2Matlab();
  virtual ~SP_ExportSPN2Matlab();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to Matlab ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in Matlab Format");}
  virtual wxString GetExtension()
    { return wxT("m");}
  virtual wxString GetFileDescr()
    { return wxT("Matlab Files");}

};

#endif // __SP_EXPORTSPN2MATLAB__


