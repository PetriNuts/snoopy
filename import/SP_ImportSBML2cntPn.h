//////////////////////////////////////////////////////////////////////
// $Author: schroed $
// $Version: 0.1 $
// $Date: 2007/02/03 $
// Short Description: creates a continious petrinet on a SBML file
//////////////////////////////////////////////////////////////////////

#ifndef __SP_IMPORTSBML2CNTPN_H__
#define __SP_IMPORTSBML2CNTPN_H__

#include "import/SP_ImportSBML.h"

class SP_ImportSBML2cntPn: public SP_ImportSBML
{

private:

	SP_ListMetadata g_ParameterList;

	/**
	\brief  Add model compartment as a parameter node to the a net

	 */
	void getModelCompartments();

	/**
	\brief  Add all model parameters as a parameter node to the a net

	 */
	void getModelParameters();

	/**
	\brief  Add all compounds as a place to the a net

	 */
	void getSpecies();

	/**
	\brief  Add all reactionss as a transition to the a net

	 */
	void getReactions ();

	/**
	\brief  Add all reaction parameters as a parameter node to the a net

	 */
	void getReactionParameters(Reaction*  l_sbmlReaction);

	/**
	\brief  Check for existing parameter node

	\param		l_parameterName		the name of the parameter to check
	*/
	bool existParameterNode(const wxString& l_parameterName);



protected:

public:
	SP_ImportSBML2cntPn();
	virtual ~SP_ImportSBML2cntPn();


	virtual bool ReadFile(const wxString& fileName);

	inline virtual wxString GetName()
		{ return wxT("Import SBML to continuous Petri Net");}
	inline virtual wxString GetDescr()
		{ return wxT("Choose a SBML File");}
	virtual wxString GetExtension()
		{ return wxT("SBML files (*.xml;*.sbml)|*.xml;*.sbml");}

};

#endif // __SP_IMPORTSBML2CNTPN_H__
