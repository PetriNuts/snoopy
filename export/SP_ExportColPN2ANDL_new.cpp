//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPN2ANDL_new.h"
#include "sp_core/tools/SP_NetUnfolder.h"
#include "sp_utilities.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "dssz/andl/andl_writer.h"

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/stdstream.h>

SP_ExportColPN2ANDL_new::SP_ExportColPN2ANDL_new()
{
}

SP_ExportColPN2ANDL_new::~SP_ExportColPN2ANDL_new()
{
}

bool SP_ExportColPN2ANDL_new::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS ||
			cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS ||
			cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportColPN2ANDL_new::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_graph = p_doc->GetGraph();
	m_fileName = p_fileName;

	SP_IddNetUnfolder unfolder;
	if(unfolder(m_graph))
	{
		dsszmc::andl::writer w(unfolder.GetUnfoldedNet());
		w(m_fileName);
	}
	else
	{
		SP_LOGERROR(wxT(" while exporting colored net!"));
	}
	return !m_bError;
}

