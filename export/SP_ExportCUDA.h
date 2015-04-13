//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/09/12 $
// Short Description: export to CUDA
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCUDA__
#define __SP_EXPORTCUDA__

#include "export/SP_ExportPT.h"

class SP_ExportCUDA: public SP_ExportPT
{
 private:

 protected:
  bool WritePlaces();
  bool WriteTransitions();
  bool WriteConstants();
  virtual bool DoWrite();

 public:
  SP_ExportCUDA();
  virtual ~SP_ExportCUDA();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to CUDA ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in CUDA Format");}
  virtual wxString GetExtension()
    { return wxT("cu");}
  virtual wxString GetFileDescr()
    { return wxT("CUDA Files");}

};

#endif // __SP_EXPORTSPN2MATLAB__


