/*
 * ErodeParser.h
 *
 *  Created on: 30 Nov 2017
 *      Author: hadron
 */

#ifndef IMPORT_ERODE_ERODEPARSER_H_
#define IMPORT_ERODE_ERODEPARSER_H_

//#include <boost/config/warning_disable.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/spirit/include/phoenix_fusion.hpp>
//#include <boost/spirit/include/phoenix_stl.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/variant/recursive_variant.hpp>
//#include <boost/foreach.hpp>
//
//#include <iostream>
//#include <fstream>
//#include <string>
#include <vector>

namespace sp {
namespace import {
namespace erode {

struct Net;

class ErodeParser {
	private:
		Net *m_pNet = NULL;

	public:
		ErodeParser();
		virtual ~ErodeParser();

		void parse(const std::string &strFileName) throw();
		void parse(std::istream &inStream) throw();
		Net& getNet();
};

} /* namespace erode */
} /* namespace import */
} /* namespace sp */

#endif /* IMPORT_ERODE_ERODEPARSER_H_ */
