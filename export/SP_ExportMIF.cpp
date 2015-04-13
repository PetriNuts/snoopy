// $Author: md $
// $Version: $
// $Date: 2005/08/01 $
// Short Description: export to framemaker format *.mif
//
//////////////////////////////////////////////////////////////////////


#include "export/SP_ExportMIF.h"
#include "export/SP_EXP_MIFDC.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/cmndata.h>

SP_ExportMIF::~SP_ExportMIF() { }


bool SP_ExportMIF::AcceptsDoc(SP_MDI_Doc* p_doc) {
  CHECK_POINTER(p_doc, return FALSE);
  return TRUE;
}


bool SP_ExportMIF::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName) {
  CHECK_POINTER(p_doc, return FALSE);
  CHECK_BOOL((!p_fileName.IsEmpty()), return FALSE);

  wxPrintData *pd = new wxPrintData();
  pd->SetFilename(p_fileName);
  SP_EXP_MIFDC mifDC (*pd);

  mifDC.StartDoc(p_fileName);

  wxNode *node = p_doc->GetDiagram()->GetShapeList()->GetFirst();
  while (node) {
    wxShape *shape = (wxShape*)node->GetData();
		shape->Draw(mifDC);
		node = node->GetNext();
  }

  mifDC.EndDoc();
  wxDELETE(pd);

  return  !m_bError;
}
