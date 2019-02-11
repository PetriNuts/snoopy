/*
 * IDocBuilder.h
 *
 *  Created on: 5 Jan 2018
 *      Author: JCH
 */

#ifndef IMPORT_ERODE_IDOCBUILDER_H_
#define IMPORT_ERODE_IDOCBUILDER_H_

namespace sp {
namespace import {

template <class Doc, class Constants, class Functions, class Places, class Transitions>
struct IDocBuilder {
	virtual IDocBuilder& appendConstants(const Constants& constants) = 0;
	virtual IDocBuilder& appendFunctions(const Functions& functions) = 0;
	virtual IDocBuilder& appendPlaces(const Places& places) = 0;
	virtual IDocBuilder& appendTransitions(const Transitions& transitions) = 0;
	virtual Doc& build() = 0;

	virtual Doc& getDoc() = 0;
};

} /* namespace import */
} /* namespace sp */

#endif /* IMPORT_ERODE_IDOCBUILDER_H_ */
