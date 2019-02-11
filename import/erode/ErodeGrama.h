/*
 * ErodeGrama.h
 *
 *  Created on: 14 Dec 2017
 *      Author: hadron
 */

#ifndef IMPORT_ERODE_ERODEGRAMA_H_
#define IMPORT_ERODE_ERODEGRAMA_H_

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/phoenix/object/construct.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include "import/erode/ErodeNet.h"

namespace sp {
namespace import {
namespace erode {

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator, typename Skipper>
struct ErodeGrama: qi::grammar<Iterator, Net(), Skipper> {
	template<typename TokenDef>
	ErodeGrama(TokenDef const& tok, const Net &net) :
	ErodeGrama::base_type(doc), c(0), w(0), l(0), i(0) {

		using boost::phoenix::ref;
		using boost::phoenix::size;
		using qi::lit;
		using qi::lexeme;
		using qi::double_;
		using qi::int_;

//		using qi::_val;
//		using qi::_1;
//		using namespace qi::labels;

		using boost::phoenix::at_c;

//		startModel =  *(   tok.word          [++ref(w), ref(c) += size(_1)]
//		                  |   lit('\n')         [++ref(c), ++ref(l)]
//		                  |   qi::token(100)  [++ref(c)]
//		                  )
//		              ;
		// Num
		num =
				tok.DOUBLESCI [at_c<0>(_val) = num::DOUBLE, at_c<1>(_val) = _1]
				| tok.DOUBLE [at_c<0>(_val) = num::DOUBLE, at_c<1>(_val) = _1]
				| tok.INT [at_c<0>(_val) = num::INT, at_c<1>(_val) = _1]
				;
		// Expresion
		expresion =
				(
				tok.BO
				>>
				expresion
				>>
				tok.BC
				>> (tok.ADD | tok.SUB | tok.MUL | tok.DIV)
				>> (expresion)
				) [_val = _1 + _2 + _3 + _4 + _5]
				|
				(
				tok.BO
				>>
				expresion
				>>
				tok.BC
				) [_val = _1 + _2 + _3]
				|
				(
				(buildFunc | tok.IDENTIFIER | tok.INT | tok.DOUBLE)
				>> (tok.ADD | tok.SUB | tok.MUL | tok.DIV)
				>> (expresion)
				) [_val = _1 + _2 + _3]
				|
				(buildFunc | tok.IDENTIFIER | tok.INT | tok.DOUBLE) [_val = _1]
				;
		// Build in functions
		buildFunc =
				(
				tok.ABS
				>>tok.BO
				>> (buildFunc | expresion)
				>> tok.BC
				) [_val = _1 + _2 + _3 + _4]
				;
		// Begin model
		beginModel =
				tok.BEGIN
				>> tok.MODEL
				>> tok.IDENTIFIER [_val = _1]
				;

		endModel =
				tok.END
				>> tok.MODEL
				;

		// const
		param =
//				(
//				tok.IDENTIFIER
//				>> tok.EQ
////				>> (tok.INT | tok.DOUBLE)
//				>> num
////			    )
//				/*
//				 * Constructing new object, what we already have,
//				 * so for simplify BOOST_FUSION_ADAPT_STRUCT
//				 * is in use (see ErodeNet.h)
//				 */
////				) [_val = phoenix::construct<Constant>(_1, _3)]
//				) [at_c<0>(_val) = _1, at_c<1>(_val) = _3]
//				;
				// New
//				(
//				tok.IDENTIFIER
//				>> tok.EQ
//				>> num
//				) [at_c<0>(_val) = _1, at_c<2>(_val) = _3]
//				|
//				(
//				tok.IDENTIFIER
//				>> tok.EQ
//				>> (buildFunc | expresion)
//				) [at_c<0>(_val) = _1, at_c<1>(_val) = _3]
				(
				tok.IDENTIFIER
				>> tok.EQ
				>>
				(
//				num [at_c<2>(_val) = _1]
				(
				buildFunc | expresion
//				) [at_c<1>(_val) = _1]
				) [_val = _1]
				|
				num [_val = _1]
				)
				) [at_c<0>(_val) = _1]

				;


		parameters =
				tok.BEGIN
				>> tok.PARAMETERS
//						>> *(tok.IDENTIFIER >> tok.EQ >> int_ | double_)
//						>> *(tok.IDENTIFIER >> tok.EQ >> int_ | double_) % lexeme[+(tok.SPACE)]
//						>> param >> *('|' >> param)
				>> *param [phoenix::push_back(_val, _1)]
//		        >> *param [phoenix::bind(net.m_vConstants.push_back, NULL)]
				>> tok.END
				>> tok.PARAMETERS
				;
		// place
		place =
				(
				(
				tok.IDENTIFIER
				>> tok.EQ
//				>> (tok.IDENTIFIER | tok.INT | tok.DOUBLE)
				>> (buildFunc | expresion)
				) [at_c<0>(_val) = _1, at_c<1>(_val) = _3]
				| tok.IDENTIFIER [at_c<0>(_val) = _1, at_c<1>(_val) = "0"]
				)
				;
		// init places
		init =
				tok.BEGIN
				>> tok.INIT
				/*
				 * Correct
				 * From complex to simpler
				 */
				>> *place [phoenix::push_back(_val, _1)]
				/*
				 *  Not Correct
				 *  From simpler to complex
				 */
//				>> *(tok.IDENTIFIER
//					| (tok.IDENTIFIER >> tok.EQ >> (tok.IDENTIFIER | tok.INT | tok.DOUBLE)))
				>> tok.END
				>> tok.INIT
				;
		//TODO Grama for Partition
		// Transition
//		react =
//				(
//				(
//				tok.IDENTIFIER
//				>>
//				tok.ADD
//				>>
//				tok.IDENTIFIER
////				) [phoenix::push_back(_val, _1), phoenix::push_back(_val, _3)]
//				) [_val = _1, _val = _3]
//				|
//				(
//				tok.INT
//				>>
//				tok.MUL
//				>>
//				tok.IDENTIFIER
////				) [phoenix::push_back(_val, _3)]
//				) [_val = _1 + _2 + _3]
//				| tok.IDENTIFIER [_val = _1]//[phoenix::push_back(_val, _1)]
//				)
//				;
		react =
				(
				tok.IDENTIFIER
				>> tok.ADD
				>> react
				) [_val += _1, _val += _3]
				|
				(
				tok.INT
				>> tok.MUL
				>> react
				) [_val += _3, _val *= _1]
				|
				(tok.IDENTIFIER) [_val = _1]
				;

		reactFunc =
				(
//				(
//				tok.IDENTIFIER
//				>> tok.MUL
//				>> tok.IDENTIFIER
//				) [at_c<0>(_val) = "MassAction", at_c<1>(_val) = _1 + _2 + _3]
//				| tok.IDENTIFIER [at_c<0>(_val) = "MassAction", at_c<1>(_val) = _1]
				(
				expresion | tok.IDENTIFIER
				) [at_c<0>(_val) = "MassAction", at_c<1>(_val) = _1]
				)
				;

		reactName =
				tok.BSO
				// TODO: change to vector ???
//				>> +tok.IDENTIFIER [_val += _1 + "_"]
				>> +tok.IDENTIFIER [phoenix::push_back(_val, _1)]
				>> tok.BSC
				;

		reaction =
				(
				react
				>> tok.TRANSITION
				>> react
				>> tok.COMA
				>> reactFunc
				>> reactName
				) [at_c<0>(_val) = _6, at_c<1>(_val) = _5, at_c<2>(_val) = _1, at_c<3>(_val) = _3]
				;

		reactions =
				tok.BEGIN
				>> tok.REACTIONS
//				>> +(((tok.IDENTIFIER >> tok.ADD >> tok.IDENTIFIER) | tok.IDENTIFIER) >> tok.TRANSITION
//						>> ((tok.IDENTIFIER >> tok.ADD >> tok.IDENTIFIER) | tok.IDENTIFIER))
				>> +reaction [phoenix::push_back(_val, _1)]
				>> tok.END
				>> tok.REACTIONS
				;

		doc =
				beginModel [at_c<0>(_val) = _1]
				>> parameters [at_c<1>(_val) = _1]
				>> init [at_c<2>(_val) = _1]
				>> reactions [at_c<3>(_val) = _1]
				>> endModel
				;

	}
	std::size_t c, w, l, i;
	std::string strModel;
	/*
	 * ex. Net() - create new object and return as _val, so
	 * we can use _val as a object but we must cast using something
	 * from phoenix
	 */
	qi::rule<Iterator, Net(), Skipper> doc;
	qi::rule<Iterator, std::string(), Skipper> beginModel;
	qi::rule<Iterator, Skipper> endModel;
	qi::rule<Iterator, std::vector<Constant>(), Skipper> parameters;
	qi::rule<Iterator, Constant(), Skipper> param;
	qi::rule<Iterator, std::vector<Place>(), Skipper> init;
	qi::rule<Iterator, std::string(), Skipper> expresion;
	qi::rule<Iterator, std::string(), Skipper> buildFunc;
	qi::rule<Iterator, Place(), Skipper> place;
	/*
	 * TODO: Add to AST rest of below
	 * It will be necessary change
	 * std::vector<Place> m_vPrePlaces; =>
	 * std::vector<String> m_vPrePlaces;
	 * and etc.
	 * in ErodeNet.h
	 */
	qi::rule<Iterator, std::vector<Transition>(), Skipper> reactions;
	qi::rule<Iterator, Transition(), Skipper> reaction;
//	qi::rule<Iterator, std::vector<std::string>(), Skipper> react;
	qi::rule<Iterator, SetPlaces(), Skipper> react;
	qi::rule<Iterator, Function(), Skipper> reactFunc;
	qi::rule<Iterator, std::vector<std::string>(), Skipper> reactName;
	qi::rule<Iterator, Num(), Skipper> num;
};

template<typename Iterator>
struct Skipper: qi::grammar<Iterator> {
	template<typename TokenDef>
	Skipper(TokenDef const& tok) :
	Skipper::base_type(skip) {

		skip = tok.SPACE | tok.COMMENT;

	}
	qi::rule<Iterator> skip;
};

} /* namespace erode */
} /* namespace import */
} /* namespace sp */

#endif /* IMPORT_ERODE_ERODEGRAMA_H_ */
