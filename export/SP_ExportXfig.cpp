//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/11 $
// Short Description: export to xfig
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportXfig.h"
#include "export/SP_XfigDC.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/cmndata.h>

SP_ExportXfig::~SP_ExportXfig()
{
}


bool
SP_ExportXfig::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return FALSE);
  return TRUE;
}


bool
SP_ExportXfig::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return FALSE);
  CHECK_BOOL((!p_fileName.IsEmpty()), return FALSE);

  wxPrintData *pd = new wxPrintData();
  pd->SetFilename(p_fileName);
  pd->SetPaperId(wxPAPER_A4);
  pd->SetOrientation(wxLANDSCAPE);
  pd->SetPrintMode(wxPRINT_MODE_FILE);
  SP_XfigDC xfigDC (*pd);
#if wxABI_VERSION < 30000
  xfigDC.SetResolution(1200);
#endif
  xfigDC.StartDoc(p_fileName);

  wxNode *node = p_doc->GetDiagram()->GetShapeList()->GetFirst();
  while (node) {
    wxShape *shape = (wxShape*)node->GetData();
    //shape->Select(FALSE);
    shape->Draw(xfigDC);
    node = node->GetNext();
  }

  xfigDC.EndDoc();
  wxDELETE(pd);

  return  !m_bError;
}


