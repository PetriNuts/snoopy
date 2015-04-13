// $Author: md $
// $Version: $
// $Date: 2005/08/01 $
// Short Description: export to framemaker format *.mif
// 
//////////////////////////////////////////////////////////////////////

#ifndef __SP_EXPORT_MIF__
#define __SP_EXPORT_MIF__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_ExportMIF : public SP_ExportRoutine, public SP_Error
{
 public:
  virtual ~SP_ExportMIF();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName() { return wxT("Export to MIF");}
  inline virtual wxString GetDescr() { return wxT("Export document to FrameMaker Interchange Format");}
  virtual wxString GetExtension() { return wxT("mif");}
  virtual wxString GetFileDescr() { return wxT("FrameMaker File ");}

 public :

     
     
    

};
#endif // __SP_EXPORT_MIF__

