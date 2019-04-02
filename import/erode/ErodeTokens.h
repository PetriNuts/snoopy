/*
 * ErodeTokens.h
 *
 *  Created on: 14 Dec 2017
 *      Author: hadron
 */

#ifndef IMPORT_ERODE_ERODETOKENS_H_
#define IMPORT_ERODE_ERODETOKENS_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

namespace sp {
namespace import {
namespace erode {

using namespace boost::spirit;
using namespace boost::spirit::ascii;

template<typename Lexer>
struct ErodeTokens: boost::spirit::lex::lexer<Lexer> {
	ErodeTokens() {
		// define patterns (lexer macros) to be used during token definition
		// below
		this->self.add_pattern
		("BEGIN", "begin")
		("END", "end")
		("MODEL", "model")
		("PARAMETERS", "parameters")
		("INIT", "init")
		("REACTIONS", "reactions")
		("ABS", "abs")
		("INT", "0|[1-9][0-9]*")
		("DOUBLE", "(0|[1-9][0-9]*)[.][0-9]+")
		("DOUBLESCI", "(0|[1-9][0-9]*)[.][0-9]+[eE][+-][0-9]+")
		("TRANSITION", "->")
		("ADD", "[+]")// + is special character
		("SUB", "[-]")
		("MUL", "[*]")
		("DIV", "[/]")
		("EQ", "=")
		("IDENTIFIER", "[A-Za-z][A-Za-z_0-9]*")
		//
		("BO", "[\\(]")
		("BC", "[\\)]")
		("BSO", "[\\[]")
		("BSC", "[\\]]")
		//
		("COMA", ",")
		//
		("SPACE", "[ \\t\\n]")
		("COMMENT", "[/][/][^\n]*[\n]")
//		("SPACE", qi::space)
		;
		// define tokens and associate them with the lexer
		BEGIN = "{BEGIN}";
		END = "{END}";
		MODEL = "{MODEL}";
		PARAMETERS = "{PARAMETERS}";
		INIT = "{INIT}";
		REACTIONS = "{REACTIONS}";
		ABS = "{ABS}";
		INT = "{INT}";
		DOUBLE = "{DOUBLE}";
		DOUBLESCI = "{DOUBLESCI}";
		TRANSITION = "{TRANSITION}";
		ADD = "{ADD}";
		SUB = "{SUB}";
		MUL = "{MUL}";
		DIV = "{DIV}";
		EQ = "{EQ}";
		IDENTIFIER = "{IDENTIFIER}";
		//
		BO = "{BO}";
		BC = "{BC}";
		BSO = "{BSO}";
		BSC = "{BSC}";
		//
		COMA = "{COMA}";
		//
		SPACE = "{SPACE}";
		COMMENT = "{COMMENT}";
		// Key words first
		this->self.add
		(BEGIN)
		(END)
		(MODEL)
		(PARAMETERS)
		(INIT)
		(REACTIONS)
		(ABS)
		(INT)
		(DOUBLE)
		(DOUBLESCI)
		(TRANSITION)
		(ADD)
		(SUB)
		(MUL)
		(DIV)
		(EQ)
		(IDENTIFIER)
		//
		(BO)
		(BC)
		(BSO)
		(BSC)
		//
		(COMA)
		//
		(SPACE)
		(COMMENT)
		// no token id is needed here
//		('\n')
		// characters are usable as tokens as well
		(".", 100)
		// string literals will not be esacped by the library
		;
	}
	// the token 'word' exposes the matched string as its parser attribute
	// keywords
	lex::token_def<std::string> BEGIN, END, MODEL, PARAMETERS, INIT, REACTIONS;
	/*
	 * Problem with variant when taking value
	 * from token in grama.
	 * TOOD: resolve this issue in the near feature
	 */
//	lex::token_def<int> INT;
//	lex::token_def<double> DOUBLE;
	lex::token_def<std::string> INT, DOUBLE, DOUBLESCI;
	lex::token_def<std::string> EQ, ADD, SUB, MUL, DIV, TRANSITION;
	lex::token_def<std::string> BO, BC, BSO, BSC;
	lex::token_def<std::string> IDENTIFIER;
	lex::token_def<std::string> ABS;

	lex::token_def<> COMA;
	//
	lex::token_def<lex::omit> SPACE;
	lex::token_def<lex::omit> COMMENT;

};

} /* namespace erode */
} /* namespace import */
} /* namespace sp */


#endif /* IMPORT_ERODE_ERODETOKENS_H_ */
