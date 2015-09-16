//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/12/22 $
// Short Description: export to latex
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportLatex.h"
#include "export/SP_LatexDC.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/cmndata.h>

SP_ExportLatex::~SP_ExportLatex()
{
}


bool
SP_ExportLatex::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return FALSE);
  return TRUE;
}


bool
SP_ExportLatex::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return FALSE);
  CHECK_BOOL((!p_fileName.IsEmpty()), return FALSE);

  wxPrintData *pd = new wxPrintData();
  pd->SetFilename(p_fileName);
  pd->SetPaperId(wxPAPER_A2);
  pd->SetOrientation(wxPORTRAIT);
  pd->SetPrintMode(wxPRINT_MODE_FILE);
  SP_LatexDC latexDC (*pd);
#if wxABI_VERSION < 30000
  latexDC.SetResolution(72);
#endif
  latexDC.StartDoc(p_fileName);

  wxNode *node = p_doc->GetDiagram()->GetShapeList()->GetFirst();
  while (node) {
    wxShape *shape = (wxShape*)node->GetData();
    //shape->Select(FALSE);
    shape->Draw(latexDC);
    node = node->GetNext();
  }

  latexDC.EndDoc();
  wxDELETE(pd);

  return  !m_bError;
}


