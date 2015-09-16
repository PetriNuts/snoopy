//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/12/22 $
// Short Description: export to latex
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_LATEX__
#define __SP_EXPORT_LATEX__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_ExportLatex : public SP_ExportRoutine, public SP_Error
{
 public:
  virtual ~SP_ExportLatex();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to LaTex");}
  inline virtual wxString GetDescr()
    { return wxT("Export document in LaTex Format");}
  virtual wxString GetExtension()
    { return wxT("tex");}
  virtual wxString GetFileDescr()
    { return wxT("LaTex Files");}

};
#endif // __SP_EXPORT_LATEX__
