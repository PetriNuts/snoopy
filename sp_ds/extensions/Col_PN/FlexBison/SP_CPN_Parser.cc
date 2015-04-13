/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   SP_CPNlex

/* First part of user declarations.  */
/* Line 279 of lalr1.cc  */
#line 4 "SP_CPN_Parser.yy"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "SP_CPN_Parse_Context.h"


/* Line 279 of lalr1.cc  */
#line 50 "SP_CPN_Parser.cc"


#include "SP_CPN_Parser.hh"

/* User implementation prologue.  */
/* Line 285 of lalr1.cc  */
#line 105 "SP_CPN_Parser.yy"


#include "SP_CPN_Driver.h"
#include "SP_CPN_Scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex


/* Line 285 of lalr1.cc  */
#line 71 "SP_CPN_Parser.cc"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace SP_CPN {
/* Line 353 of lalr1.cc  */
#line 166 "SP_CPN_Parser.cc"

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  SP_CPN_Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  SP_CPN_Parser::SP_CPN_Parser (class SP_CPN_Driver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg)
  {
  }

  SP_CPN_Parser::~SP_CPN_Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  SP_CPN_Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  SP_CPN_Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  SP_CPN_Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 30: /* "identifier" */
/* Line 455 of lalr1.cc  */
#line 93 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 273 "SP_CPN_Parser.cc"
        break;
      case 31: /* "string" */
/* Line 455 of lalr1.cc  */
#line 94 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 280 "SP_CPN_Parser.cc"
        break;
      case 32: /* "nb1Dfunc" */
/* Line 455 of lalr1.cc  */
#line 95 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 287 "SP_CPN_Parser.cc"
        break;
      case 33: /* "nb2Dfunc" */
/* Line 455 of lalr1.cc  */
#line 96 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 294 "SP_CPN_Parser.cc"
        break;
      case 34: /* "allfunc" */
/* Line 455 of lalr1.cc  */
#line 97 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 301 "SP_CPN_Parser.cc"
        break;
      case 35: /* "absfunc" */
/* Line 455 of lalr1.cc  */
#line 98 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 308 "SP_CPN_Parser.cc"
        break;
      case 36: /* "valfunc" */
/* Line 455 of lalr1.cc  */
#line 99 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).stringVal); };
/* Line 455 of lalr1.cc  */
#line 315 "SP_CPN_Parser.cc"
        break;
      case 43: /* CPN_Constant */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 322 "SP_CPN_Parser.cc"
        break;
      case 44: /* CPN_Variable */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 329 "SP_CPN_Parser.cc"
        break;
      case 45: /* CPN_Neighbor1D */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 336 "SP_CPN_Parser.cc"
        break;
      case 46: /* CPN_Neighbor2D */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 343 "SP_CPN_Parser.cc"
        break;
      case 47: /* CPN_AllFunc */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 350 "SP_CPN_Parser.cc"
        break;
      case 48: /* CPN_ABSFunc */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 357 "SP_CPN_Parser.cc"
        break;
      case 49: /* CPN_VALFunc */
/* Line 455 of lalr1.cc  */
#line 100 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 364 "SP_CPN_Parser.cc"
        break;
      case 50: /* CPN_Function */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 371 "SP_CPN_Parser.cc"
        break;
      case 51: /* CPN_ArgumentList */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 378 "SP_CPN_Parser.cc"
        break;
      case 52: /* CPN_Atomexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 385 "SP_CPN_Parser.cc"
        break;
      case 53: /* CPN_Postfixexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 392 "SP_CPN_Parser.cc"
        break;
      case 54: /* CPN_Unaryexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 399 "SP_CPN_Parser.cc"
        break;
      case 55: /* CPN_Mulexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 406 "SP_CPN_Parser.cc"
        break;
      case 56: /* CPN_Addexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 413 "SP_CPN_Parser.cc"
        break;
      case 57: /* CPN_Relexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 420 "SP_CPN_Parser.cc"
        break;
      case 58: /* CPN_Eqexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 427 "SP_CPN_Parser.cc"
        break;
      case 59: /* CPN_Andexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 434 "SP_CPN_Parser.cc"
        break;
      case 60: /* CPN_Orexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 441 "SP_CPN_Parser.cc"
        break;
      case 61: /* CPN_Commaexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 448 "SP_CPN_Parser.cc"
        break;
      case 62: /* CPN_Normalexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 455 "SP_CPN_Parser.cc"
        break;
      case 63: /* CPN_Seperatorexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 462 "SP_CPN_Parser.cc"
        break;
      case 64: /* CPN_Predicateexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 469 "SP_CPN_Parser.cc"
        break;
      case 65: /* CPN_Connectorexpr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 476 "SP_CPN_Parser.cc"
        break;
      case 66: /* CPN_expr */
/* Line 455 of lalr1.cc  */
#line 101 "SP_CPN_Parser.yy"
        { delete ((*yyvaluep).calcnode); };
/* Line 455 of lalr1.cc  */
#line 483 "SP_CPN_Parser.cc"
        break;

	default:
	  break;
      }
  }

  void
  SP_CPN_Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SP_CPN_Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SP_CPN_Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SP_CPN_Parser::debug_level_type
  SP_CPN_Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  SP_CPN_Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  SP_CPN_Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  SP_CPN_Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SP_CPN_Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


/* User initialization code.  */
/* Line 545 of lalr1.cc  */
#line 40 "SP_CPN_Parser.yy"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &driver.streamname;
}
/* Line 545 of lalr1.cc  */
#line 584 "SP_CPN_Parser.cc"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 2:
/* Line 670 of lalr1.cc  */
#line 123 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = new SP_CPN_Parse_Integer_Node((yysemantic_stack_[(1) - (1)].integerVal));
			}
    break;

  case 3:
/* Line 670 of lalr1.cc  */
#line 127 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = new SP_CPN_Parse_Double_Node((yysemantic_stack_[(1) - (1)].doubleVal));
			}
    break;

  case 4:
/* Line 670 of lalr1.cc  */
#line 131 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = new SP_CPN_Parse_String_Node( *(yysemantic_stack_[(1) - (1)].stringVal) ); 
				delete (yysemantic_stack_[(1) - (1)].stringVal);
			}
    break;

  case 5:
/* Line 670 of lalr1.cc  */
#line 137 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_Variable_Node( *(yysemantic_stack_[(1) - (1)].stringVal) ); 
				delete (yysemantic_stack_[(1) - (1)].stringVal);				
			}
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 143 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_Neighbor12D_Node( *(yysemantic_stack_[(3) - (1)].stringVal) ); 
				delete (yysemantic_stack_[(3) - (1)].stringVal);				
			}
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 149 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_Neighbor12D_Node( *(yysemantic_stack_[(3) - (1)].stringVal) ); 
				delete (yysemantic_stack_[(3) - (1)].stringVal);				
			}
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 155 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_AllFunc_Node( *(yysemantic_stack_[(3) - (1)].stringVal) ); 
				delete (yysemantic_stack_[(3) - (1)].stringVal);				
			}
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 161 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_AbsFunc_Node( *(yysemantic_stack_[(4) - (1)].stringVal), (yysemantic_stack_[(4) - (3)].calcnode) ); 
				delete (yysemantic_stack_[(4) - (1)].stringVal);				
			}
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 167 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = new SP_CPN_Parse_ValFunc_Node( *(yysemantic_stack_[(4) - (1)].stringVal), (yysemantic_stack_[(4) - (3)].calcnode) ); 
				delete (yysemantic_stack_[(4) - (1)].stringVal);				
			}
    break;

  case 11:
/* Line 670 of lalr1.cc  */
#line 173 "SP_CPN_Parser.yy"
    {
				
				(yyval.calcnode) = new SP_CPN_Parse_Function_Node( (yysemantic_stack_[(4) - (1)].calcnode),(yysemantic_stack_[(4) - (3)].calcnode) ); 				
				
			}
    break;

  case 12:
/* Line 670 of lalr1.cc  */
#line 179 "SP_CPN_Parser.yy"
    {				
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);				
			}
    break;

  case 13:
/* Line 670 of lalr1.cc  */
#line 183 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = new SP_CPN_Parse_Argument_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
			}
    break;

  case 14:
/* Line 670 of lalr1.cc  */
#line 188 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 192 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 197 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 202 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 206 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 210 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 214 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 219 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
			}
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 223 "SP_CPN_Parser.yy"
    {
				(yyval.calcnode) = (yysemantic_stack_[(3) - (2)].calcnode);
			}
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 227 "SP_CPN_Parser.yy"
    {	
			(yyval.calcnode) = (yysemantic_stack_[(3) - (2)].calcnode);
		 }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 231 "SP_CPN_Parser.yy"
    {	
			(yyval.calcnode) = (yysemantic_stack_[(3) - (2)].calcnode);
		 }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 237 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_Dot_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].integerVal));
	      }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 241 "SP_CPN_Parser.yy"
    {	
			(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
		  }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 245 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_Index_Node((yysemantic_stack_[(4) - (1)].calcnode), (yysemantic_stack_[(4) - (3)].calcnode));
	      }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 252 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	      }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 256 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_Successor_Node((yysemantic_stack_[(2) - (2)].calcnode));
	      }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 260 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_Predecessor_Node((yysemantic_stack_[(2) - (2)].calcnode));
	      }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 264 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_IndexAbstract_Node((yysemantic_stack_[(2) - (2)].calcnode));
	      }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 268 "SP_CPN_Parser.yy"
    {
			(yyval.calcnode) = new SP_CPN_Parse_Not_Node((yysemantic_stack_[(2) - (2)].calcnode));
	      }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 273 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 277 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Multiply_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 281 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Divide_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 285 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Modulo_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 289 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Power_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 294 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 298 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Add_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 302 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = new SP_CPN_Parse_Substract_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 309 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 313 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_LessThan_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 317 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_GreaterThan_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 321 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_LessEqual_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 325 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_GreaterEqual_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 332 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 336 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_Equal_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 340 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_NotEqual_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 347 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 351 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_And_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 358 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 362 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = new SP_CPN_Parse_Or_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	  }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 370 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	 }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 374 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = new SP_CPN_Parse_Comma_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	}
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 381 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	}
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 385 "SP_CPN_Parser.yy"
    {	
		(yyval.calcnode) = (yysemantic_stack_[(3) - (2)].calcnode);
	 }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 389 "SP_CPN_Parser.yy"
    {		
		(yyval.calcnode) = new SP_CPN_Parse_Bracket_Node((yysemantic_stack_[(3) - (2)].calcnode));
	 }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 396 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	}
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 400 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = new SP_CPN_Parse_Seperator_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	}
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 406 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	}
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 410 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = new SP_CPN_Parse_Predicate_Node((yysemantic_stack_[(4) - (2)].calcnode), (yysemantic_stack_[(4) - (4)].calcnode));
	}
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 414 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = new SP_CPN_Parse_Predicate_Node((yysemantic_stack_[(6) - (2)].calcnode), (yysemantic_stack_[(6) - (5)].calcnode));
	}
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 421 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	}
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 425 "SP_CPN_Parser.yy"
    {
		(yyval.calcnode) = new SP_CPN_Parse_CNN_Node((yysemantic_stack_[(3) - (1)].calcnode), (yysemantic_stack_[(3) - (3)].calcnode));
	}
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 433 "SP_CPN_Parser.yy"
    {
	      (yyval.calcnode) = (yysemantic_stack_[(1) - (1)].calcnode);
	  }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 442 "SP_CPN_Parser.yy"
    {
	      driver.calc.m_ExpressionsVector.push_back((yysemantic_stack_[(3) - (2)].calcnode));
	  }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 446 "SP_CPN_Parser.yy"
    {
	      driver.calc.m_ExpressionsVector.push_back((yysemantic_stack_[(3) - (2)].calcnode));
	  }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 450 "SP_CPN_Parser.yy"
    {
	      driver.calc.m_ExpressionsVector.push_back((yysemantic_stack_[(3) - (2)].calcnode));
	  }
    break;


/* Line 670 of lalr1.cc  */
#line 1249 "SP_CPN_Parser.cc"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  SP_CPN_Parser::yysyntax_error_ (int yystate, int yytoken)
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yychar.
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            /* Stay within bounds of both yycheck and yytname.  */
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULL;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char SP_CPN_Parser::yypact_ninf_ = -25;
  const short int
  SP_CPN_Parser::yypact_[] =
  {
       -25,     2,   -25,   143,   143,   143,   143,   -25,   -25,   -25,
     -25,   -25,   -24,   -21,    18,    22,    30,    52,    76,   -25,
     -25,    53,   -25,   -25,   -25,   -25,   -25,   -25,   -25,   -16,
     -25,   -10,    41,   155,    -1,    36,    40,   -25,    94,   -25,
     113,    15,    52,   -16,   -16,   -16,   -16,    85,    86,    87,
      76,    76,    34,     7,    10,   100,     9,    76,   111,   143,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    98,    52,   -25,   -25,   -25,    39,
     102,   -25,   -25,   -25,   -11,    37,   -25,    98,   -25,   -25,
     -25,   120,    12,    40,   -25,   101,   -25,   -25,   -25,   -25,
     -10,   -10,    41,    41,    41,    41,   155,   155,    -1,    36,
     -25,   -25,   -25,   -25,   -25,   -25,    52,    94,    76,   -25,
     -25,     6,    40,   -25
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  SP_CPN_Parser::yydefact_[] =
  {
        66,     0,     1,     0,     0,     0,     0,    68,     2,     3,
       5,     4,     0,     0,     0,     0,     0,     0,     0,    67,
      14,    15,    16,    17,    18,    19,    20,    21,    26,    28,
      33,    38,    41,    46,    49,    51,    55,    58,    60,    63,
      65,     0,     0,    29,    30,    31,    32,     0,     0,     0,
       0,     0,    55,     0,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    71,    70,    69,    55,
      58,     6,     7,     8,     0,     0,    23,     0,    57,    24,
      22,     0,     0,    12,    25,     0,    34,    35,    36,    37,
      39,    40,    44,    45,    43,    42,    48,    47,    50,    52,
      59,    64,    23,     9,    10,    54,     0,    61,     0,    11,
      27,     0,    13,    62
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  SP_CPN_Parser::yypgoto_[] =
  {
       -25,   -25,   -25,   -25,   -25,   -25,   -25,   -25,   -25,   -25,
      84,    58,    56,     1,    28,     0,    73,    74,   -17,   -25,
     -14,    55,    88,    44,   160,   -25
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  SP_CPN_Parser::yydefgoto_[] =
  {
        -1,    20,    21,    22,    23,    24,    25,    26,    27,    92,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    53,
      37,    38,    39,    40,    55,     1
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char SP_CPN_Parser::yytable_ninf_ = -54;
  const signed char
  SP_CPN_Parser::yytable_[] =
  {
        52,    56,     2,    54,    70,    64,    65,    58,    60,    61,
      62,    63,    71,    47,    75,    76,    48,    87,     3,     4,
     -53,    73,   118,    59,     5,    79,     6,   113,    80,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      93,    18,    77,    19,   123,    88,    73,    72,    89,    91,
     119,    73,    73,    64,    65,    49,    78,    64,    65,    50,
     110,    43,    44,    45,    46,   100,   101,    51,     3,     4,
     106,   107,    86,   115,     5,   114,     6,   112,    84,    85,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      57,    18,     3,     4,   102,   103,   104,   105,     5,    52,
       6,   122,    54,    74,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    42,     3,     4,    96,    97,    98,    99,
       5,    75,     6,    81,    82,    83,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,     3,     4,    90,    94,
      89,   120,     5,    95,     6,   108,   117,   109,     8,     9,
      10,    11,    12,    13,    14,    15,    16,   116,    66,    67,
     121,    41,     0,   111,     0,     0,     0,     0,     0,    68,
      69,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      42
  };

  /* YYCHECK.  */
  const signed char
  SP_CPN_Parser::yycheck_[] =
  {
        17,    18,     0,    17,     5,    16,    17,    23,    18,    19,
      20,    21,    13,    37,     8,     0,    37,    10,    16,    17,
      10,    12,    10,    39,    22,    42,    24,    38,    42,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      57,    39,    27,    41,    38,    38,    12,    11,    38,    40,
      38,    12,    12,    16,    17,    37,    41,    16,    17,    37,
      74,     3,     4,     5,     6,    64,    65,    37,    16,    17,
      70,    71,    38,    87,    22,    38,    24,    38,    50,    51,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      37,    39,    16,    17,    66,    67,    68,    69,    22,   116,
      24,   118,   116,     9,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    16,    17,    60,    61,    62,    63,
      22,     8,    24,    38,    38,    38,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    16,    17,    38,    28,
      38,    40,    22,    59,    24,    72,    91,    73,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     3,     4,
     116,     1,    -1,    75,    -1,    -1,    -1,    -1,    -1,    14,
      15,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  SP_CPN_Parser::yystos_[] =
  {
         0,    67,     0,    16,    17,    22,    24,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    39,    41,
      43,    44,    45,    46,    47,    48,    49,    50,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    62,    63,    64,
      65,    66,    37,    53,    53,    53,    53,    37,    37,    37,
      37,    37,    60,    61,    62,    66,    60,    37,    23,    39,
      18,    19,    20,    21,    16,    17,     3,     4,    14,    15,
       5,    13,    11,    12,     9,     8,     0,    27,    41,    60,
      62,    38,    38,    38,    56,    56,    38,    10,    38,    38,
      38,    40,    51,    60,    28,    52,    54,    54,    54,    54,
      55,    55,    56,    56,    56,    56,    57,    57,    58,    59,
      62,    64,    38,    38,    38,    62,    37,    63,    10,    38,
      40,    65,    60,    38
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  SP_CPN_Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,    96,
      44,    38,   124,    61,    62,    60,    43,    45,    42,    47,
      37,    94,    64,    58,    33,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,    40,    41,    91,
      93,    59
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  SP_CPN_Parser::yyr1_[] =
  {
         0,    42,    43,    43,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    51,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    53,    53,    53,    54,    54,
      54,    54,    54,    55,    55,    55,    55,    55,    56,    56,
      56,    57,    57,    57,    57,    57,    58,    58,    58,    59,
      59,    60,    60,    61,    61,    62,    62,    62,    63,    63,
      64,    64,    64,    65,    65,    66,    67,    67,    67,    67,
      67,    67
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  SP_CPN_Parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     3,     3,     3,     4,
       4,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     1,     4,     1,     2,
       2,     2,     2,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       1,     4,     6,     1,     3,     1,     0,     2,     2,     3,
       3,     3
  };


  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const SP_CPN_Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "LE_OP", "GE_OP", "NE_OP",
  "IF", "INDEX", "CNN", "'`'", "','", "'&'", "'|'", "'='", "'>'", "'<'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "'@'", "':'", "'!'", "DEC_OP",
  "INC_OP", "\"end of line\"", "\"integer\"", "\"double\"",
  "\"identifier\"", "\"string\"", "\"nb1Dfunc\"", "\"nb2Dfunc\"",
  "\"allfunc\"", "\"absfunc\"", "\"valfunc\"", "'('", "')'", "'['", "']'",
  "';'", "$accept", "CPN_Constant", "CPN_Variable", "CPN_Neighbor1D",
  "CPN_Neighbor2D", "CPN_AllFunc", "CPN_ABSFunc", "CPN_VALFunc",
  "CPN_Function", "CPN_ArgumentList", "CPN_Atomexpr", "CPN_Postfixexpr",
  "CPN_Unaryexpr", "CPN_Mulexpr", "CPN_Addexpr", "CPN_Relexpr",
  "CPN_Eqexpr", "CPN_Andexpr", "CPN_Orexpr", "CPN_Commaexpr",
  "CPN_Normalexpr", "CPN_Seperatorexpr", "CPN_Predicateexpr",
  "CPN_Connectorexpr", "CPN_expr", "start", YY_NULL
  };

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const SP_CPN_Parser::rhs_number_type
  SP_CPN_Parser::yyrhs_[] =
  {
        67,     0,    -1,    28,    -1,    29,    -1,    31,    -1,    30,
      -1,    32,    37,    38,    -1,    33,    37,    38,    -1,    34,
      37,    38,    -1,    35,    37,    56,    38,    -1,    36,    37,
      56,    38,    -1,    44,    37,    51,    38,    -1,    60,    -1,
      51,    10,    60,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      37,    66,    38,    -1,    37,    60,    38,    -1,    37,    62,
      38,    -1,    53,    23,    28,    -1,    52,    -1,    53,    39,
      52,    40,    -1,    53,    -1,    16,    53,    -1,    17,    53,
      -1,    22,    53,    -1,    24,    53,    -1,    54,    -1,    55,
      18,    54,    -1,    55,    19,    54,    -1,    55,    20,    54,
      -1,    55,    21,    54,    -1,    55,    -1,    56,    16,    55,
      -1,    56,    17,    55,    -1,    56,    -1,    57,    15,    56,
      -1,    57,    14,    56,    -1,    57,     3,    56,    -1,    57,
       4,    56,    -1,    57,    -1,    58,    13,    57,    -1,    58,
       5,    57,    -1,    58,    -1,    59,    11,    58,    -1,    59,
      -1,    60,    12,    59,    -1,    62,    -1,    61,    10,    62,
      -1,    60,    -1,    37,    60,    38,    -1,    37,    61,    38,
      -1,    62,    -1,    63,     9,    62,    -1,    63,    -1,    39,
      60,    40,    63,    -1,    39,    60,    40,    37,    65,    38,
      -1,    64,    -1,    65,     8,    64,    -1,    65,    -1,    -1,
      67,    41,    -1,    67,    27,    -1,    67,    66,    41,    -1,
      67,    66,    27,    -1,    67,    66,     0,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  SP_CPN_Parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    15,    19,    23,
      28,    33,    38,    40,    44,    46,    48,    50,    52,    54,
      56,    58,    60,    64,    68,    72,    76,    78,    83,    85,
      88,    91,    94,    97,    99,   103,   107,   111,   115,   117,
     121,   125,   127,   131,   135,   139,   143,   145,   149,   153,
     155,   159,   161,   165,   167,   171,   173,   177,   181,   183,
     187,   189,   194,   201,   203,   207,   209,   210,   213,   216,
     220,   224
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  SP_CPN_Parser::yyrline_[] =
  {
         0,   122,   122,   126,   130,   136,   142,   148,   154,   160,
     166,   172,   178,   182,   187,   191,   196,   201,   205,   209,
     213,   218,   222,   226,   230,   236,   240,   244,   251,   255,
     259,   263,   267,   272,   276,   280,   284,   288,   293,   297,
     301,   308,   312,   316,   320,   324,   331,   335,   339,   346,
     350,   357,   361,   369,   373,   380,   384,   388,   395,   399,
     405,   409,   413,   420,   424,   432,   438,   439,   440,   441,
     445,   449
  };

  // Print the state stack on the debug stream.
  void
  SP_CPN_Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SP_CPN_Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  SP_CPN_Parser::token_number_type
  SP_CPN_Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,     2,     2,     2,    20,    11,     2,
      37,    38,    18,    16,    10,    17,     2,    19,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    23,    41,
      15,    13,    14,     2,    22,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    39,     2,    40,    21,     2,     9,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    12,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int SP_CPN_Parser::yyeof_ = 0;
  const int SP_CPN_Parser::yylast_ = 180;
  const int SP_CPN_Parser::yynnts_ = 26;
  const int SP_CPN_Parser::yyempty_ = -2;
  const int SP_CPN_Parser::yyfinal_ = 2;
  const int SP_CPN_Parser::yyterror_ = 1;
  const int SP_CPN_Parser::yyerrcode_ = 256;
  const int SP_CPN_Parser::yyntokens_ = 42;

  const unsigned int SP_CPN_Parser::yyuser_token_number_max_ = 275;
  const SP_CPN_Parser::token_number_type SP_CPN_Parser::yyundef_token_ = 2;


} // SP_CPN
/* Line 1141 of lalr1.cc  */
#line 1889 "SP_CPN_Parser.cc"
/* Line 1142 of lalr1.cc  */
#line 456 "SP_CPN_Parser.yy"
 /*** Additional Code ***/

void SP_CPN::SP_CPN_Parser::error(const SP_CPN_Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
