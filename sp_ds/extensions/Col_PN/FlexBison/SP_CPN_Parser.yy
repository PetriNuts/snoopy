/* $Id: SP_CPN_Parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file SP_CPN_Parser.yy Contains the SP_CPN Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "SP_CPN_Parse_Context.h"

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release versions. */
/* %debug */ 

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="SP_CPN"

/* set the parser's class identifier */
%define "parser_class_name" "SP_CPN_Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class SP_CPN_Driver& driver }

/* verbose error messages */
%error-verbose

 /*** BEGIN SP_CPN - Change the SP_CPN grammar's tokens below ***/

%union {
    int 						integerVal;
    double						doubleVal;
    std::string*				stringVal;
    class SP_CPN_ParseNode*		calcnode;
}

%token LE_OP GE_OP ELEM_OP NE_OP IF INDEX CNN CDD

%left CNN 
%left CDD
%left '`'
%left ','
%left '&' '|' 
%left '=' NE_OP
%left '>' '<' GE_OP LE_OP 
%left '+' '-'
%left '*' '/' '%' '^'
%left '@' ':' '!'
%nonassoc INC_OP DEC_OP IF INDEX



%token					END	     0	"end of file"
%token					EOL			"end of line" 
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 		DOUBLE		"double"
%token <stringVal> 		IDENTIFIER	"identifier"
%token <stringVal> 		STRING		"string"
%token <stringVal> 		NEIGHBOR1D	"nb1Dfunc"
%token <stringVal> 		NEIGHBOR2D	"nb2Dfunc"
%token <stringVal> 		ALLFUNC		"allfunc"
%token <stringVal> 		ABSFUNC		"absfunc"
%token <stringVal> 		VALFUNC		"valfunc"
%token <stringVal> 		ELEM		"elemOfOP"
%token <stringVal> 		NUMOF		"numOfOP"



%type <calcnode>	CPN_Constant CPN_Variable CPN_Neighbor1D CPN_Neighbor2D CPN_AllFunc CPN_ABSFunc CPN_VALFunc
%type <calcnode>	CPN_Atomexpr CPN_Function CPN_ArgumentList CPN_Commaexpr CPN_Normalexpr CPN_Postfixexpr CPN_Unaryexpr CPN_Mulexpr CPN_Addexpr CPN_Relexpr CPN_Eqexpr CPN_Andexpr CPN_Orexpr CPN_Seperatorexpr CPN_Predicateexpr CPN_Connectorexpr CPN_expr

%destructor { delete $$; } IDENTIFIER
%destructor { delete $$; } STRING
%destructor { delete $$; } NEIGHBOR1D
%destructor { delete $$; } NEIGHBOR2D
%destructor { delete $$; } ALLFUNC
%destructor { delete $$; } ABSFUNC
%destructor { delete $$; } VALFUNC
%destructor { delete $$; } CPN_Constant CPN_Variable CPN_Neighbor1D CPN_Neighbor2D CPN_AllFunc CPN_ABSFunc CPN_VALFunc
%destructor { delete $$; } CPN_Atomexpr CPN_Function CPN_ArgumentList CPN_Commaexpr CPN_Normalexpr CPN_Postfixexpr CPN_Unaryexpr CPN_Mulexpr CPN_Addexpr CPN_Relexpr CPN_Eqexpr CPN_Andexpr CPN_Orexpr CPN_Seperatorexpr CPN_Predicateexpr CPN_Connectorexpr CPN_expr

 /*** END SP_CPN - Change the SP_CPN grammar's tokens above ***/

%{

#include "SP_CPN_Driver.h"
#include "SP_CPN_Scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN SP_CPN - Change the SP_CPN grammar rules below ***/

CPN_Constant : INTEGER
           {
				$$ = new SP_CPN_Parse_Integer_Node($1);
			}
		 | DOUBLE
			{
				$$ = new SP_CPN_Parse_Double_Node($1);
			}
         | STRING
			{
				$$ = new SP_CPN_Parse_String_Node( *$1 ); 
				delete $1;
			}
 
CPN_Variable : IDENTIFIER
           {				
				$$ = new SP_CPN_Parse_Variable_Node( *$1 ); 
				delete $1;				
			}

CPN_Neighbor1D : NEIGHBOR1D '(' ')'
           {				
				$$ = new SP_CPN_Parse_Neighbor12D_Node( *$1 ); 
				delete $1;				
			}

CPN_Neighbor2D : NEIGHBOR2D '(' ')'
           {				
				$$ = new SP_CPN_Parse_Neighbor12D_Node( *$1 ); 
				delete $1;				
			}
			
CPN_AllFunc : ALLFUNC '(' ')'
           {				
				$$ = new SP_CPN_Parse_AllFunc_Node( *$1 ); 
				delete $1;				
			}
			
CPN_ABSFunc : ABSFUNC '(' CPN_Addexpr ')'
           {				
				$$ = new SP_CPN_Parse_AbsFunc_Node( *$1, $3 ); 
				delete $1;				
			}

CPN_VALFunc : VALFUNC '(' CPN_Addexpr ')'
           {				
				$$ = new SP_CPN_Parse_ValFunc_Node( *$1, $3 ); 
				delete $1;				
			}
			
CPN_Function : CPN_Variable '(' CPN_ArgumentList ')'
           {
				
				$$ = new SP_CPN_Parse_Function_Node( $1,$3 ); 				
				
			}
CPN_ArgumentList : CPN_Orexpr
           {				
				$$ = $1;				
			}	
		  | CPN_ArgumentList  ',' CPN_Orexpr
           {
				$$ = new SP_CPN_Parse_Argument_Node($1, $3);
			}	

CPN_Atomexpr : CPN_Constant
           {
				$$ = $1;
			}
         | CPN_Variable
           {
				$$ = $1;
			}
			
		| CPN_Neighbor1D
           {
				$$ = $1;
			}
			
		| CPN_Neighbor2D
           {
				$$ = $1;
			}
		| CPN_AllFunc
           {
				$$ = $1;
			}
		| CPN_ABSFunc
           {
				$$ = $1;
			}
		| CPN_VALFunc
           {
				$$ = $1;
			}
			
		 | CPN_Function
           {
				$$ = $1;
			}
		
         | '(' CPN_expr ')'
           {
				$$ = $2;
			}
			
	  
		| '(' CPN_Orexpr ')'
		 {	
			$$ = $2;
		 }
  
		 
		 | '(' CPN_Normalexpr ')'
		 {	
			$$ = $2;
		 }
		  
		  
	 
 
		| '(' CPN_Commaexpr ')'
	{		
		$$ = new SP_CPN_Parse_Bracket_Node($2);
	 }
	 

CPN_Postfixexpr
		: CPN_Postfixexpr ':' INTEGER  %prec INDEX
          {
			$$ = new SP_CPN_Parse_Dot_Node($1, $3);
	      }
	    | CPN_Atomexpr
		  {	
			$$ = $1;
		  }
        | CPN_Postfixexpr '[' CPN_Atomexpr ']' %prec INDEX
          {
			$$ = new SP_CPN_Parse_Index_Node($1, $3);
	      }

	
CPN_Unaryexpr 
		: CPN_Postfixexpr
          {
			$$ = $1;
	      }
        | '+' CPN_Postfixexpr %prec INC_OP
          {
			$$ = new SP_CPN_Parse_Successor_Node($2);
	      }
        | '-' CPN_Postfixexpr
          {
			$$ = new SP_CPN_Parse_Predecessor_Node($2);
	      }
		| '@' CPN_Postfixexpr
          {
			$$ = new SP_CPN_Parse_IndexAbstract_Node($2);
	      }
		| '!' CPN_Postfixexpr
          {
			$$ = new SP_CPN_Parse_Not_Node($2);
	      }

CPN_Mulexpr : CPN_Unaryexpr
          {
	      $$ = $1;
	  }
        | CPN_Mulexpr '*' CPN_Unaryexpr
          {
	      $$ = new SP_CPN_Parse_Multiply_Node($1, $3);
	  }
        | CPN_Mulexpr '/' CPN_Unaryexpr
          {
	      $$ = new SP_CPN_Parse_Divide_Node($1, $3);
	  }
        | CPN_Mulexpr '%' CPN_Unaryexpr
          {
	      $$ = new SP_CPN_Parse_Modulo_Node($1, $3);
	  }
		| CPN_Mulexpr '^' CPN_Unaryexpr
          {
	      $$ = new SP_CPN_Parse_Power_Node($1, $3);
	  }

CPN_Addexpr : CPN_Mulexpr
          {
	      $$ = $1;
	  }
        | CPN_Addexpr '+' CPN_Mulexpr
          {
	      $$ = new SP_CPN_Parse_Add_Node($1, $3);
	  }
        | CPN_Addexpr '-' CPN_Mulexpr
          {
	      $$ = new SP_CPN_Parse_Substract_Node($1, $3);
	  }
	| CPN_Addexpr NUMOF CPN_Mulexpr
          {
	      $$ =new SP_CPN_Parse_NumOf_Node($1, $3);
	  }
	   


CPN_Relexpr
	: CPN_Addexpr
	  {	
		$$ = $1;
	  }
 
	| CPN_Relexpr '<' CPN_Addexpr
	  {	
		$$ = new SP_CPN_Parse_LessThan_Node($1, $3);
	  }
	| CPN_Relexpr '>' CPN_Addexpr
	  {	
		$$ = new SP_CPN_Parse_GreaterThan_Node($1, $3);
	  }
	| CPN_Relexpr LE_OP CPN_Addexpr
	  {	
		$$ = new SP_CPN_Parse_LessEqual_Node($1, $3);
	  }
	| CPN_Relexpr GE_OP CPN_Addexpr
	  {	
		$$ = new SP_CPN_Parse_GreaterEqual_Node($1, $3);
	  }
	 
	|  
	  CPN_Relexpr ELEM CPN_Addexpr
	 {
	 $$ =new SP_CPN_Parse_Element_Of_Node($1, $3);
	 }
	   
     
CPN_Eqexpr
	: CPN_Relexpr
	  {	
		$$ = $1;
	  }
	| CPN_Eqexpr '=' CPN_Relexpr
	  {	
		$$ = new SP_CPN_Parse_Equal_Node($1, $3);
	  }
	| CPN_Eqexpr NE_OP CPN_Relexpr
	  {	
		$$ = new SP_CPN_Parse_NotEqual_Node($1, $3);
	  }

		
CPN_Andexpr
	: CPN_Eqexpr
	  {	
		$$ = $1;
	  }
	 
	| CPN_Andexpr '&' CPN_Eqexpr
	  {	
		$$ = new SP_CPN_Parse_And_Node($1, $3);
	  }


CPN_Orexpr
	: CPN_Andexpr
	  {	
		$$ = $1;
	  }
 
	| CPN_Orexpr '|' CPN_Andexpr	
	  {	
		$$ = new SP_CPN_Parse_Or_Node($1, $3);
	  }
	  


CPN_Commaexpr 
	: CPN_Normalexpr	
	 {	
		$$ = $1;
	 }	 
	| CPN_Commaexpr ',' CPN_Normalexpr
	{
		$$ = new SP_CPN_Parse_Comma_Node($1, $3);
	}
	| CPN_Connectorexpr ',' CPN_Normalexpr /*19.12.2020 */
	{
		$$ = new SP_CPN_Parse_Comma_Node($1, $3);
	}


CPN_Normalexpr 
	: CPN_Orexpr
	{	
		$$ = $1;
	}
	| '(' CPN_Orexpr ')'
	{	
		$$ = $2;
	 }
	| '(' CPN_Commaexpr ')'
	{		
		$$ = new SP_CPN_Parse_Bracket_Node($2);
	 }


CPN_Seperatorexpr	
	: CPN_Normalexpr
	{
		$$ = $1;
	}
	| CPN_Seperatorexpr '`' CPN_Normalexpr
	{
		$$ = new SP_CPN_Parse_Seperator_Node($1, $3);
	}
	
	 
	
	
CPN_Predicateexpr	
	: CPN_Seperatorexpr
	{
		$$ = $1;
	}
	| '[' CPN_Orexpr ']' CPN_Seperatorexpr
	{
		$$ = new SP_CPN_Parse_Predicate_Node($2, $4);
	}
	| '[' CPN_Orexpr ']' '(' CPN_Connectorexpr ')'
	{
		$$ = new SP_CPN_Parse_Predicate_Node($2, $5);
	}
	
	
CPN_Connectorexpr	
	: CPN_Predicateexpr
	{
		$$ = $1;
	}
	| CPN_Connectorexpr CNN CPN_Predicateexpr
	{
		$$ = new SP_CPN_Parse_CNN_Node($1, $3);
	}
    | CPN_Connectorexpr CDD CPN_Predicateexpr
	{
		$$ = new SP_CPN_Parse_CDD_Node($1, $3);
	}
	 
	
	  
CPN_expr
	: CPN_Connectorexpr
      {
	      $$ = $1;
	  }


start	: /* empty */
        | start ';'
        | start EOL
        | start CPN_expr ';'
          {
	      driver.calc.m_ExpressionsVector.push_back($2);
	  }
        | start CPN_expr EOL
          {
	      driver.calc.m_ExpressionsVector.push_back($2);
	  }
        | start CPN_expr END
          {
	      driver.calc.m_ExpressionsVector.push_back($2);
	  }

 /*** END SP_CPN - Change the SP_CPN grammar rules above ***/

%% /*** Additional Code ***/

void SP_CPN::SP_CPN_Parser::error(const SP_CPN_Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
