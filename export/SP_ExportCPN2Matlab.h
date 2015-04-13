//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/04/30 $
// Short Description: export to Matlab
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCPN2MATLAB__
#define __SP_EXPORTCPN2MATLAB__

#include "export/SP_ExportPT.h"

class SP_ExportCPN2Matlab: public SP_ExportPT
{
protected:
	virtual bool DoWrite();

public:
	SP_ExportCPN2Matlab();
	virtual ~SP_ExportCPN2Matlab();

	virtual bool AcceptsDoc(SP_MDI_Doc*);

	inline virtual wxString GetName()
	{
		return wxT("Export ODEs to Matlab ");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export ODEs to Matlab Format");
	}
	virtual wxString GetExtension()
	{
		return wxT("m");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("Matlab Files");
	}

};

#endif // __SP_EXPORTCPN2MATLAB__
