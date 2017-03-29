//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2009/03/25 $
// Short Description: creates a stochastic petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#ifndef __SP_IMPORTSBML2SPN_H__
#define __SP_IMPORTSBML2SPN_H__

#include "import/SP_ImportSBML.h"

class SP_ImportSBML2sPn: public SP_ImportSBML
{

private:

	/**
	\brief  Add model description

	 */
	void getModelDescription();

	/**
	\brief  Add model compartment as a parameter node to the a net

	 */
	void getModelCompartments();

	/**
	\brief  Add model functions to the a net

	 */
	void getFunctionDefinitions();

	/**
	\brief  Add all model parameters as a parameter node to the a net

	 */
	void getModelParameters();

	/**
	\brief  Add all compounds as a place to the a net

	 */
	void getSpecies();

	/**
	\brief  Add all events as a deterministic transition to the a net

	 */
	void getEvents();

	/**
	\brief  Add all reactionss as a transition to the a net

	 */
	void getReactions ();

	/**
	\brief  Add all reaction parameters as a parameter node to the a net

	 */
	void getReactionParameters(Reaction*  l_sbmlReaction, ASTNode* l_sbmlMath);

protected:

public:
	SP_ImportSBML2sPn();
	virtual ~SP_ImportSBML2sPn();


	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import SBML to stochastic Petri Net");}
	inline virtual wxString GetDescr()
		{ return wxT("Choose a SBML File");}
	virtual wxString GetExtension()
		{ return wxT("SBML files (*.xml;*.sbml)|*.xml;*.sbml");}

};

#endif // __SP_IMPORTSBML2SPN_H__
