/*
 * ErodeNet.h
 *
 *  Created on: 16 Dec 2017
 *      Author: hadron
 */

#ifndef IMPORT_ERODE_ERODENET_H_
#define IMPORT_ERODE_ERODENET_H_

#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/variant/recursive_variant.hpp>
//#include <boost/foreach.hpp>

#include <string>
#include <vector>

namespace sp {
namespace import {
namespace erode {

namespace num {

enum type {
	NaN = 0,
	INT,
	DOUBLE
};

}

struct Num {
	num::type m_type = num::NaN;
	std::string m_strValue;
};

struct Constant {
	std::string m_strName;
	std::string m_strValue;
	Num m_value;

	Constant() {

	}

	Constant(const std::string &strName, const std::string &strValue) :
		m_strName(strName), m_strValue(strValue) {

	}
	/**
	 *
	 */
	Constant & operator= (const Num &num) {
		m_strValue = "";
		m_value	= num;
		return *this;
	}
	/**
	 *
	 */
	Constant & operator= (const std::string &strValue) {
		m_value.m_type = num::NaN;
		m_strValue = strValue;
		return *this;
	}

};

struct Place {
	std::string m_strName;
	std::string m_strValue;
};

struct SetPlaces {
	std::vector<std::string> m_vSet;

	/**
	 * Multiple last entry in collection
	 */
	SetPlaces & operator*= (const std::string &strPlaceMultiplicity) {
		int iMultiplicity = 0;
		std::string strPlaceName;

		iMultiplicity = std::stoi(strPlaceMultiplicity);
		iMultiplicity -= 1;

		if(iMultiplicity > 1) {
			m_vSet.reserve(m_vSet.size() + iMultiplicity);
		}

		strPlaceName = m_vSet.back();
		for(int iIdx = 0; iIdx < iMultiplicity; iIdx++) {
			m_vSet.push_back(strPlaceName);
		}

		return *this;
	}
	/**
	 *
	 */
	SetPlaces & operator+= (const SetPlaces &setPlaces) {
		m_vSet.reserve(m_vSet.size() + setPlaces.m_vSet.size());
		m_vSet.insert(m_vSet.end(),
						setPlaces.m_vSet.begin(),
						setPlaces.m_vSet.end());

		return *this;
	}
	/**
	 *
	 */
	SetPlaces & operator= (const SetPlaces &setPlaces) {
			m_vSet.clear();
			*this += setPlaces;

			return *this;
	}

//	SetPlaces & operator+= (const std::string &strExpresion) {
//		int iMultiplicity = 0;
//		std::string strPlaceMultiplicity;
//		std::string strPlaceName;
//		size_t EndPart1 = strExpresion.find_first_of("*");
//		if(EndPart1 != std::string::npos) {
//			strPlaceMultiplicity = strExpresion.substr(0, EndPart1 + 1);
//			strPlaceName = strExpresion.substr(EndPart1 + 1);
//			iMultiplicity = std::stoi(strPlaceMultiplicity);
//			for(int iIdx = 0; iIdx < iMultiplicity; iIdx++) {
//				m_vSet.push_back(strPlaceName);
//			}
//		} else {
//			m_vSet.push_back(strExpresion);
//		}
//
//		return *this;
//	}
	SetPlaces & operator+= (const std::string &strPlaceName) {
			m_vSet.push_back(strPlaceName);
			return *this;
	}

	SetPlaces & operator= (const std::string &strPlaceName) {
		m_vSet.clear();
		*this += strPlaceName;
		return *this;
	}
};

struct Function {
	std::string m_strName;
	std::string m_strParam;
};

struct Transition {
	std::string name() {
		std::string strName = "";
		for(auto strReducTransName : m_vReducTransitons) {
			if(strName.length() == 0) {
				strName = strReducTransName;
			} else {
				strName += "_" + strReducTransName;
			}
		}
		return strName;
	}

	Function m_function;
	std::vector<std::string> m_vReducTransitons;
//	std::vector<Place> m_vPrePlaces;
//	std::vector<Place> m_vPostPlaces;

//	std::vector<std::string> m_vPrePlaces;
//	std::vector<std::string> m_vPostPlaces;
	SetPlaces m_setPrePlaces;
	SetPlaces m_setPostPlaces;

};

struct Net {
	std::string m_strName;
	std::vector<Constant> m_vConstants;
	std::vector<Place> m_vPlaces;
	std::vector<Transition> m_vTransitions;
};

} /* namespace erode */
} /* namespace import */
} /* namespace sp */

// We need to tell fusion about our Net struct
// to make it a first-class fusion citizen
//BOOST_FUSION_ADAPT_STRUCT(
//    sp::import::erode::Net,
//    (std::vector<sp::import::erode::Const>, m_vConsts)
//    (std::vector<sp::import::erode::Transition>, m_vTransitions)
//)

BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Num,
	(sp::import::erode::num::type, m_type)
	(std::string, m_strValue)
)
BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Net,
	(std::string, m_strName)
	(std::vector<sp::import::erode::Constant>, m_vConstants)
	(std::vector<sp::import::erode::Place>, m_vPlaces)
	(std::vector<sp::import::erode::Transition>, m_vTransitions)
)
BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Constant,
	(std::string, m_strName)
	(std::string, m_strValue)
	(sp::import::erode::Num, m_value)
)
BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Place,
	(std::string, m_strName)
	(std::string, m_strValue)
)
BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Function,
	(std::string, m_strName)
	(std::string, m_strParam)
)
BOOST_FUSION_ADAPT_STRUCT(
    sp::import::erode::Transition,
//	(std::string, m_strName)
	(std::vector<std::string>, m_vReducTransitons)
	(sp::import::erode::Function, m_function)
//	(std::vector<std::string>, m_vPrePlaces)
//	(std::vector<std::string>, m_vPostPlaces)
	(sp::import::erode::SetPlaces, m_setPrePlaces)
	(sp::import::erode::SetPlaces, m_setPostPlaces)

)
#endif /* IMPORT_ERODE_ERODENET_H_ */
