//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.2 $
// $Date: 2007/03/27 $
// Short Description: creates a extended (discrete) petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#ifndef __SP_IMPORTSBML2EXTPN_H__
#define __SP_IMPORTSBML2EXTPN_H__

#include "import/SP_ImportSBML.h"

class SP_ImportSBML2extPN: public SP_ImportSBML

{

private:

	bool m_HighlightReversibleReactions;
	bool m_CreateReverseReactions;

	/**
	\brief  Add model compartment as a parameter node to the a net

	 */
	void getModelCompartments();

	/**
	\brief  Add all compounds as a place to the a net

	 */
	void getSpecies();

	/**
	\brief  Add all reactions as transitions to the net

	 */
	void getReactions ();

protected:

public:
	SP_ImportSBML2extPN();
	virtual ~SP_ImportSBML2extPN();

	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import SBML to discrete Petri Net");}
	inline virtual wxString GetDescr()
		{ return wxT("Choose a SBML File");}
	virtual wxString GetExtension()
		{ return wxT("SBML files (*.xml;*.sbml)|*.xml;*.sbml");}

};

#endif // __SP_IMPORTSBML2EXTPN_H__
