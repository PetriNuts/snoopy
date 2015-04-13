/*
 * SP_ExportContODE2Octave.h
 * $Author: cr
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 29.10.2013
 * Short Description:
 */
//==============================================================================
#ifndef SP_EXPORTCONTODE2OCTAVE_H_
#define SP_EXPORTCONTODE2OCTAVE_H_

#include "export/SP_ExportPT.h"

/*
 * Export the continuous Petri Net's ODE to a text file
 */
class SP_ExportContODE2Octave: public SP_ExportPT
{
public:
	bool DoWrite();

	bool AcceptsDoc(SP_MDI_Doc* p_doc);

	inline virtual wxString GetName()
	{
		return wxT("Export ODEs to Octave");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export ODEs to Octave Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("m");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("Octave Files");
	}

	SP_ExportContODE2Octave();
	virtual ~SP_ExportContODE2Octave();

};

#endif /* SP_EXPORTCONTODE2OCTAVE_H_ */
