/*
 * ErodeParser.cpp
 *
 *  Created on: 30 Nov 2017
 *      Author: JCH
 */
#include "import/erode/ErodeNet.h"
#include "import/erode/ErodeTokens.h"
#include "import/erode/ErodeGrama.h"
#include "import/erode/ErodeParser.h"

#include "sp_utilities.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>
#include <iostream>
//#include <istream>
#include <fstream>


namespace sp {
namespace import {
namespace erode {
/**
 *
 */
ErodeParser::ErodeParser() {
	// TODO Auto-generated constructor stub

}
/**
 *
 */
ErodeParser::~ErodeParser() {
	// TODO Auto-generated destructor stub
	delete m_pNet;
}
/**
 *
 */
void ErodeParser::parse(const std::string &strFileName) throw() {

	std::ifstream inStream(strFileName, std::ios_base::in);

	if (!inStream) {
		SP_LOGERROR("Error: Could not open input file: " + strFileName);
		throw;
	}
	parse(inStream);

}
/**
 *
 */
void ErodeParser::parse(std::istream &inStream) throw() {

	std::string storage; // We will read the contents here.
	inStream.unsetf(std::ios::skipws); // No white space skipping!
	std::copy(
		std::istream_iterator<char>(inStream),
		std::istream_iterator<char>(),
		std::back_inserter(storage));


	typedef lex::lexertl::token< char const*, boost::mpl::vector<std::string> > token_type;
	typedef lex::lexertl::lexer<token_type> lexer_type;
	typedef ErodeTokens<lexer_type>::iterator_type iterator_type;

	ErodeTokens<lexer_type> erodeTokens;// Our lexer
	Skipper<iterator_type> skipper(erodeTokens);// Our skipper
//	Net net; // Our tree
//	m_pNet = getNet();
	ErodeGrama<iterator_type, Skipper<iterator_type>> erodeGrama(erodeTokens, getNet());// Our parser

	char const* first = storage.c_str();
	char const* last = &first[storage.size()];

	using boost::spirit::ascii::space;
	bool bSuccess = lex::tokenize_and_phrase_parse(first, last, erodeTokens, erodeGrama, skipper, getNet());

	if (!bSuccess) {
		std::string rest(first, last);
		SP_LOGERROR("Parsing failed");
		SP_LOGERROR("stopped at: " + rest);
	}


}
/**
 *
 */
Net& ErodeParser::getNet() {
	if(!m_pNet) {
		m_pNet = new Net();
	}
	return *m_pNet;
}

} /* namespace erode */
} /* namespace import */
} /* namespace sp */
