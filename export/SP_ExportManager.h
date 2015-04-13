//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: manager class for export routines
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_MANAGER__
#define __SP_EXPORT_MANAGER__

#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_List.h"
#include "export/SP_ExportRoutine.h"

class SP_MDI_Doc;

typedef list<SP_ExportRoutine*> SP_ListExportRoutine;

class SP_ExportManager: public SP_Error
{
 private:
  SP_ListExportRoutine m_lRoutines;
 public:

  SP_ExportManager();
  ~SP_ExportManager();

  SP_ExportRoutine* GetExportRoutine(const wxString& p_sName, SP_MDI_Doc* p_pcDoc = NULL);

  bool AddExportRoutine(SP_ExportRoutine*);
  void RemoveExportRoutine(SP_ExportRoutine*);

  // shows export possibilities and calles the selected export routine
  bool DoExport(SP_MDI_Doc*);
};

#endif // __SP_EXPORT_MANAGER__
