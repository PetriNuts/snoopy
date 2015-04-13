//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/05/01 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportFAU_H__
#define __SP_ExportFAU_H__

#include "export/SP_ExportPT.h"

class SP_ExportFAU : public SP_ExportPT
{

private:

protected:

	wxString m_sContent;

	virtual bool WriteGraph();
	virtual bool WritePlaces();
	virtual bool WriteTransitions();
	virtual bool WriteParameters();

	virtual bool DoWrite();

public:

	virtual bool AcceptsDoc(SP_MDI_Doc*);

	inline virtual wxString GetName()
	{
		return wxT("Export to FAU");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export a Petri Net into FAU Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("xml");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("FAU Files");
	}


};

#endif // __SP_ExportFAU_H__
