//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: abstract export routine,
//                    to be used with ExportManager
//////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORT_ROUTINE__
#define __SP_IMPORT_ROUTINE__

#include <wx/string.h>


class SP_ImportRoutine
{
 public:
  inline virtual ~SP_ImportRoutine() {}
  
  virtual bool ReadFile(const wxString& fileName) = 0;
  
  virtual wxString GetName() = 0;
  virtual wxString GetDescr() = 0;
  virtual wxString GetExtension() = 0;

};
#endif // __SP_IMPORT_ROUTINE__
