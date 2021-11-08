/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "SP_CPN_Scanner.h"
#include "SP_CPN_Parser.hh"

/* import the parser's token type into a local typedef */
typedef SP_CPN::SP_CPN_Parser::token token;
typedef SP_CPN::SP_CPN_Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="Example"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release versions. */
/* %option debug  */

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

 /*** BEGIN EXAMPLE - Change the example lexer rules below ***/
 
[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

[0-9]+"."[0-9]* {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

"Neighbor1D"  { 
	yylval->stringVal = new std::string(yytext, yyleng);
    return token::NEIGHBOR1D; 
}	

"Neighbor2D"  { 
	yylval->stringVal = new std::string(yytext, yyleng);
    return token::NEIGHBOR2D; 
}	

"all"  { 
	yylval->stringVal = new std::string(yytext, yyleng);
    return token::ALLFUNC; 
}	

"abs"  { 
	yylval->stringVal = new std::string(yytext, yyleng);
    return token::ABSFUNC; 
}		

"val"  { 
	yylval->stringVal = new std::string(yytext, yyleng);
    return token::VALFUNC; 
}		
"elemOf"    { yylval->stringVal = new std::string(yytext, yyleng); return(token::ELEM); }

[A-Za-z_][A-Za-z0-9_]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::IDENTIFIER;
}
\"[A-Za-z0-9_]*\" {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}


"++"		{ return(token::CNN); }				
"<="		{ return(token::LE_OP); }
">="		{ return(token::GE_OP); }
"<>"		{ return(token::NE_OP); }
"!="		{ return(token::NE_OP); }
"&"			{ return static_cast<token_type>(*yytext); }
"|"			{ return static_cast<token_type>(*yytext); }
","			{ return static_cast<token_type>(*yytext); }
"="			{ return static_cast<token_type>(*yytext); }
"("			{ return static_cast<token_type>(*yytext); }
")"			{ return static_cast<token_type>(*yytext); }
("[")		{ return static_cast<token_type>(*yytext);}
("]")		{ return static_cast<token_type>(*yytext); }
":"			{ return static_cast<token_type>(*yytext); }
"!"			{ return static_cast<token_type>(*yytext); }
"-"			{ return static_cast<token_type>(*yytext); }
"+"			{ return static_cast<token_type>(*yytext); }
"*"			{ return static_cast<token_type>(*yytext); }
"/"			{ return static_cast<token_type>(*yytext); }
"%"			{ return static_cast<token_type>(*yytext); }
"^"			{ return static_cast<token_type>(*yytext); }
"<"			{ return static_cast<token_type>(*yytext); }
">"			{ return static_cast<token_type>(*yytext); }
"@"			{ return static_cast<token_type>(*yytext); }
"`"			{ return static_cast<token_type>(*yytext); }
		
				
				
				
				
 /* gobble up white-spaces */
[ \t\r\n]+ {
    yylloc->step();
}


 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace SP_CPN {

SP_CPN_Scanner::SP_CPN_Scanner(std::istream* in,
		 std::ostream* out)
    : ExampleFlexLexer(in, out)
{
}

SP_CPN_Scanner::~SP_CPN_Scanner()
{
}

void SP_CPN_Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int ExampleFlexLexer::yylex()
{
    std::cerr << "in ExampleFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int ExampleFlexLexer::yywrap()
{
    return 1;
}
