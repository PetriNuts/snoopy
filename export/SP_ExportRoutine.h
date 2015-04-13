//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: abstract export routine,
//                    to be used with ExportManager
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_ROUTINE__
#define __SP_EXPORT_ROUTINE__

#include <wx/string.h>
#include <wx/sizer.h>
#include "sp_utilities.h"

class SP_MDI_Doc;
class SP_DLG_ExportProperties;

class SP_ExportRoutine
{

 public:

  SP_ExportRoutine() {}
  inline virtual ~SP_ExportRoutine() {}

  virtual bool AcceptsDoc(SP_MDI_Doc*) = 0;

  virtual bool Write(SP_MDI_Doc*, const wxString&) = 0;

  virtual wxString GetName() = 0;
  virtual wxString GetDescr() = 0;
  virtual wxString GetExtension() = 0;
  virtual wxString GetFileDescr() = 0;

  virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc) { return true; }
  virtual bool OnDialogOk() { return true; }

 protected :

 private:

};
#endif // __SP_EXPORT_ROUTINE__
