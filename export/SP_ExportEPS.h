//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/11 $
// Short Description: export to eps
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_EPS__
#define __SP_EXPORT_EPS__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_ExportEPS : public SP_ExportRoutine, public SP_Error
{
 public:
  virtual ~SP_ExportEPS();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to EPS");}
  inline virtual wxString GetDescr()
    { return wxT("Export document in Encapsulated Postscript Format");}
  virtual wxString GetExtension()
    { return wxT("eps");}
  virtual wxString GetFileDescr()
    { return wxT("Encapsulated Postscript Files");}

  public :

     
     
    

};
#endif // __SP_EXPORT_EPS__
