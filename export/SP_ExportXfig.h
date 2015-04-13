//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/11 $
// Short Description: export to xfig
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_XFIG__
#define __SP_EXPORT_XFIG__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_ExportXfig : public SP_ExportRoutine, public SP_Error
{
 public:
  virtual ~SP_ExportXfig();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to Xfig");}
  inline virtual wxString GetDescr()
    { return wxT("Export document in Xfig Format");}
  virtual wxString GetExtension()
    { return wxT("fig");}
  virtual wxString GetFileDescr()
    { return wxT("Xfig Files");}

  public :

     
     
    


};
#endif // __SP_EXPORT_XFIG__
