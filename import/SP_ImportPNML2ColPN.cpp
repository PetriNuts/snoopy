//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2017/06/09 $
////////////////////////////////////////////////////////////////////

#include "import/SP_ImportPNML2ColPN.h"

#include "dssd/misc/net_reader.h"

SP_ImportPNML2ColPN::SP_ImportPNML2ColPN()
{
}

SP_ImportPNML2ColPN::~SP_ImportPNML2ColPN()
{
}

bool SP_ImportPNML2ColPN::ReadFile(const wxString& p_sFile,SP_ImportRoutine* p_sIR)
{
	ClearAll();

	m_fileName = p_sFile;

	dssd::net::reader p;
	bool l_Return = false;
	try
	{
		l_Return = p(p_sFile.ToStdString());
		if (l_Return)
		{
			auto l_Net = p.get();
            wxString l_sMsg = wxT("parse successful\n");
            SP_LOGMESSAGE(l_sMsg);
            l_Return = CreateGraph(p_sFile, *l_Net);
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
