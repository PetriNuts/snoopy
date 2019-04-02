/*
 * DocBuilder.h
 *
 *  Created on: 5 Jan 2018
 *      Author: JCH
 */

#ifndef IMPORT_ERODE_DOCBUILDER_H_
#define IMPORT_ERODE_DOCBUILDER_H_

#include "import/erode/IDocBuilder.h"

namespace sp {
namespace import {
namespace erode {


//class Constant;
//class Place;

class DocBuilder : sp::import::IDocBuilder<SP_MDI_Doc,
											std::vector<Constant>,
											std::vector<Function>,
											std::vector<Place>,
											std::vector<Transition>> {
public:
	DocBuilder();
	virtual ~DocBuilder();

	virtual DocBuilder& appendConstants(const std::vector<Constant>& vConstants);
	virtual DocBuilder& appendFunctions(const std::vector<Function>& vFunctions);
	virtual DocBuilder& appendPlaces(const std::vector<Place>& places);
	virtual DocBuilder& appendTransitions(const std::vector<Transition>& vTransitions);
	virtual SP_MDI_Doc& build();

	virtual SP_MDI_Doc& getDoc();

private:
	SP_MDI_Doc* m_pMdiDoc = NULL;
	// For edge Place <-> Transition
	map<std::string, SP_DS_Node*> m_mapPlaceNode;
	map<std::string, SP_DS_Node*> m_mapTransitionNode;
	std::vector<Transition> m_vTransition;

//	void connect(const std::vector<Transition>& vTransitions)
	void connect();
};

} /* namespace erode */
} /* namespace import */
} /* namespace sp */

#endif /* IMPORT_ERODE_DOCBUILDER_H_ */
