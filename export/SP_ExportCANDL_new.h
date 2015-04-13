//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCANDL2__
#define __SP_EXPORTCANDL2__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_DS_Graph;

class SP_ExportCANDL_new: public SP_ExportRoutine, public SP_Error
{
 private:
  SP_DS_Graph* m_graph;
  wxString m_fileName;

 public:
  SP_ExportCANDL_new();
  virtual ~SP_ExportCANDL_new();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to CANDL using dssz_util");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in CANDL Format");}
  virtual wxString GetExtension()
    { return wxT("candl");}
  virtual wxString GetFileDescr()
    { return wxT("CANDL Files");}

};

#endif // __SP_EXPORTCANDL2__


