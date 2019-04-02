//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportANDL.h"
#include "sp_core/tools/SP_SimpleNetBuilder.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/stdstream.h>
#include <dssd/andl/andl_writer.h>

SP_ExportANDL::SP_ExportANDL()
{
}

SP_ExportANDL::~SP_ExportANDL()
{
}

bool SP_ExportANDL::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS ||
			cName == SP_DS_EXTPN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS ||
			cName == SP_DS_SPN_CLASS ||
			cName == SP_DS_CONTINUOUSPED_CLASS ||
		cName==SP_DS_FUZZYSPN_CLASS|| //Added by G.A
			cName == SP_DS_HYBRIDPN_CLASS);
}

bool SP_ExportANDL::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_graph = p_doc->GetGraph();
	m_fileName = p_fileName;

	SP_SimpleNetBuilder builder;
	if(builder(m_graph))
	{
		dssd::andl::writer writer{builder.GetNet()};
		writer(m_fileName);
	}

	return !m_bError;
}

