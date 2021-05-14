//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCANDL_new.h"
#include "sp_core/tools/SP_SimpleNetBuilder.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "dssd/candl/candl_writer.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/stdstream.h>

SP_ExportCANDL_new::SP_ExportCANDL_new()
{
}

SP_ExportCANDL_new::~SP_ExportCANDL_new()
{
}

bool SP_ExportCANDL_new::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS ||
			cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS ||
			cName == SP_DS_COLHPN_CLASS ||
		    cName == SP_DS_FUZZY_ColSPN_CLASS ||
		    cName == SP_DS_FUZZY_ColCPN_CLASS ||
		    cName == SP_DS_FUZZY_ColHPN_CLASS
		     );

}

bool SP_ExportCANDL_new::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_graph = p_doc->GetGraph();
	m_fileName = p_fileName;

	SP_ColoredNetBuilder builder;
	if(builder(m_graph))
	{
		dssd::candl::writer{ builder.GetNet() }(m_fileName.ToStdString());
	}
	return !m_bError;
}


