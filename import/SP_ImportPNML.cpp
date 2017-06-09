//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2017/06/09 $
////////////////////////////////////////////////////////////////////

#include "import/SP_ImportPNML.h"

#include "dssz/andl/andl_reader.h"

SP_ImportPNML::SP_ImportPNML()
{
}

SP_ImportPNML::~SP_ImportPNML()
{
}

bool SP_ImportPNML::ReadFile(const wxString& p_sFile)
{
	ClearAll();

	m_fileName = p_sFile;

	dsszmc::andl::reader p;
	bool l_Return = false;
	try
	{
		l_Return = p(p_sFile.ToStdString());
		if (l_Return)
		{
			const dsszmc::andl::Net& l_Net = *(p.get());
			wxString l_sMsg = wxT("parse successful\n");
			SP_LOGMESSAGE(l_sMsg);
			l_Return = CreateGraph(p_sFile, l_Net);
			if (l_Return)
                doLayout();
		}
	}
	catch(const std::exception& e)
	{
		wxString l_sMsg = wxT("While parsing ") + p_sFile;
		SP_LOGERROR(l_sMsg);
		SP_LOGERROR(wxString(e.what()));
	}

	ClearAll();

	return l_Return;
}
