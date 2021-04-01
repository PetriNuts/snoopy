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
#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_Graph_Declarations.h"

class SP_MDI_Doc;
class SP_DLG_ImportProperties;//by george

class SP_ImportRoutine
{
 public:
  inline virtual ~SP_ImportRoutine() {}
  
  //virtual bool ReadFile(const wxString& fileName) = 0;
  virtual bool ReadFile(const wxString& fileName, SP_ImportRoutine* p_sIR=NULL) = 0;//by george
  virtual SP_DS_Graph* ImportToDoc(const wxString& fileName)=0;//by george, for command line usage
  virtual wxString GetName() = 0;
  virtual wxString GetDescr() = 0;
  virtual wxString GetExtension() = 0;

  virtual bool AddToDialog(SP_DLG_ImportProperties* p_pcDlg, dssd::andl::Net_ptr p_pcDoc) { return true; }//by george
 private:

};
#endif // __SP_IMPORT_ROUTINE__
