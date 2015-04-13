//////////////////////////////////////////////////////////////////////
// $Author: dscheibl $
// $Version: 0.1 $
// $Date: 2005/08/17 $
// Short Description: export to sbml
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCONTPED2SBML__
#define __SP_EXPORTCONTPED2SBML__
#include <sbml/SBMLTypes.h>

#include "export/SP_ExportPT.h"

class SP_ExportContPed2SBML: public SP_ExportPT
{
private:
	SBMLDocument* m_pcSbmlDoc;
	Model* m_pcSbmlModel;

protected:
	bool WriteGraph();
	bool WriteCompartments();
	bool WritePlaces();
	bool WriteTransitions();
	bool WriteParameters();
	virtual bool DoWrite();


public:
	SP_ExportContPed2SBML();
	virtual ~SP_ExportContPed2SBML();

	virtual bool AcceptsDoc(SP_MDI_Doc*);

	inline virtual wxString GetName()
	{
		return wxT("Export to SBML");
	}
	inline virtual wxString GetDescr()
	{
		return wxT("Export a Continuous Petri Net in SBML Format");
	}
	virtual wxString GetExtension()
	{
		return wxT("xml");
	}
	virtual wxString GetFileDescr()
	{
		return wxT("SBML Files");
	}

	bool ValidateSBML();

};

#endif // __SP_EXPORTCONTPED2SBML__
