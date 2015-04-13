//////////////////////////////////////////////////////////////////////
// $source: alextov $
// $author (modification): akurth$
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: manager class for import routines
//////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORT_MANAGER__
#define __SP_IMPORT_MANAGER__

#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_List.h"
#include "import/SP_ImportRoutine.h"

class SP_ImportManager: public SP_Error
{
 private:
  list<SP_ImportRoutine*> m_lRoutines;
 public:

  SP_ImportManager();
  ~SP_ImportManager();

  SP_ImportRoutine* GetImportRoutine(const wxString&);

  bool AddImportRoutine(SP_ImportRoutine*);
  void RemoveImportRoutine(SP_ImportRoutine*);

  // shows export possibilities and calles the selected export routine
  bool DoImport();
};

#endif // __SP_IMPORT_MANAGER__
