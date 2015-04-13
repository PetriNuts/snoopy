//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColExtPT2CPNTool.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

SP_ExportColExtPT2CPNTool::SP_ExportColExtPT2CPNTool()
{		
}

SP_ExportColExtPT2CPNTool::~SP_ExportColExtPT2CPNTool()
{	
}

bool SP_ExportColExtPT2CPNTool::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return ( cName == SP_DS_COLPN_CLASS || cName == SP_DS_COLEPN_CLASS || cName == SP_DS_COLSPN_CLASS);
}

bool SP_ExportColExtPT2CPNTool::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);
	
	SP_CPNToolXmlWriter l_cCPNToolXmlWriter;

	bool l_bSuccess = l_cCPNToolXmlWriter.Write(p_doc->GetGraph(), p_fileName);
	return l_bSuccess; 
	 
}

