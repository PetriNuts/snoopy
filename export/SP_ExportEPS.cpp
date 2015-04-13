//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/11 $
// Short Description: export to eps
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportEPS.h"
#include "export/SP_EPSDC.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/cmndata.h>

SP_ExportEPS::~SP_ExportEPS()
{
}

bool
SP_ExportEPS::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return FALSE);
  return TRUE;
}

bool
SP_ExportEPS::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return FALSE);
  CHECK_BOOL((!p_fileName.IsEmpty()), return FALSE);

  wxPrintData *pd = new wxPrintData();
  pd->SetFilename(p_fileName);
  pd->SetPaperId(wxPAPER_NONE);
  pd->SetPrintMode(wxPRINT_MODE_FILE);
  SP_EPSDC epsDC (*pd);
#if wxABI_VERSION < 30000
  epsDC.SetResolution(72);
#endif
  epsDC.StartDoc(p_fileName);

  wxNode *node = p_doc->GetDiagram()->GetShapeList()->GetFirst();
  while (node) {
    wxShape *shape = (wxShape*)node->GetData();
    shape->Select(FALSE);
    shape->Draw(epsDC);
    node = node->GetNext();
  }

  epsDC.EndDoc();
  wxDELETE(pd);

  return  !m_bError;
}
