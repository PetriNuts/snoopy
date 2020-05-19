//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCOLPN2ANDL2__
#define __SP_EXPORTCOLPN2ANDL2__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

class SP_DS_Graph;

class SP_ExportColPN2ANDL_new: public SP_ExportRoutine, public SP_Error
{
 private:
  SP_DS_Graph* m_graph;
  wxString m_fileName;

 public:
  SP_ExportColPN2ANDL_new();
  virtual ~SP_ExportColPN2ANDL_new();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to ANDL using dssd_util");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in ANDL Format");}
  virtual wxString GetExtension()
    { return wxT("andl");}
  virtual wxString GetFileDescr()
    { return wxT("ANDL Files");}

};

#endif // __SP_EXPORTCOLPN2ANDL2__


