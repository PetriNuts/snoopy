// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Take the name prefix into account.
#define yylex   SP_CPNlex

// First part of user declarations.
#line 4 "SP_CPN_Parser.yy" // lalr1.cc:404
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "SP_CPN_Parse_Context.h"


#line 48 "SP_CPN_Parser.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "SP_CPN_Parser.hh"

// User implementation prologue.
#line 109 "SP_CPN_Parser.yy" // lalr1.cc:412


#include "SP_CPN_Driver.h"
#include "SP_CPN_Scanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex


#line 74 "SP_CPN_Parser.cc" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
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


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace SP_CPN {
#line 160 "SP_CPN_Parser.cc" // lalr1.cc:479

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
              // Fall through.
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
  {}

  SP_CPN_Parser::~SP_CPN_Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  SP_CPN_Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  SP_CPN_Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  SP_CPN_Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  SP_CPN_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  SP_CPN_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  SP_CPN_Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  SP_CPN_Parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  SP_CPN_Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  SP_CPN_Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  SP_CPN_Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  SP_CPN_Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  SP_CPN_Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  SP_CPN_Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  SP_CPN_Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  SP_CPN_Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  SP_CPN_Parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  SP_CPN_Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  SP_CPN_Parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  SP_CPN_Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  SP_CPN_Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  SP_CPN_Parser::symbol_number_type
  SP_CPN_Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  SP_CPN_Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  SP_CPN_Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  SP_CPN_Parser::stack_symbol_type&
  SP_CPN_Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  SP_CPN_Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 32: // "identifier"

#line 97 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 413 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 33: // "string"

#line 98 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 420 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 34: // "nb1Dfunc"

#line 99 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 427 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 35: // "nb2Dfunc"

#line 100 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 434 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 36: // "allfunc"

#line 101 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 441 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 37: // "absfunc"

#line 102 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 448 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 38: // "valfunc"

#line 103 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.stringVal); }
#line 455 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 47: // CPN_Constant

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 462 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 48: // CPN_Variable

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 469 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 49: // CPN_Neighbor1D

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 476 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 50: // CPN_Neighbor2D

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 483 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 51: // CPN_AllFunc

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 490 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 52: // CPN_ABSFunc

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 497 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 53: // CPN_VALFunc

#line 104 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 504 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 54: // CPN_Function

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 511 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 55: // CPN_ArgumentList

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 518 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 56: // CPN_Atomexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 525 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 57: // CPN_Postfixexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 532 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 58: // CPN_Unaryexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 539 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 59: // CPN_Mulexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 546 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 60: // CPN_Addexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 553 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 61: // CPN_Relexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 560 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 62: // CPN_Eqexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 567 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 63: // CPN_Andexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 574 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 64: // CPN_Orexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 581 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 65: // CPN_Commaexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 588 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 66: // CPN_Normalexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 595 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 67: // CPN_Seperatorexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 602 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 68: // CPN_Predicateexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 609 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 69: // CPN_Connectorexpr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 616 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;

      case 70: // CPN_expr

#line 105 "SP_CPN_Parser.yy" // lalr1.cc:614
        { delete (yysym.value.calcnode); }
#line 623 "SP_CPN_Parser.cc" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  SP_CPN_Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  SP_CPN_Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  SP_CPN_Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  SP_CPN_Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
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
#endif // YYDEBUG

  inline SP_CPN_Parser::state_type
  SP_CPN_Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

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
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 40 "SP_CPN_Parser.yy" // lalr1.cc:741
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &driver.streamname;
}

#line 761 "SP_CPN_Parser.cc" // lalr1.cc:741

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 127 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = new SP_CPN_Parse_Integer_Node((yystack_[0].value.integerVal));
			}
#line 873 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 3:
#line 131 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = new SP_CPN_Parse_Double_Node((yystack_[0].value.doubleVal));
			}
#line 881 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 4:
#line 135 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = new SP_CPN_Parse_String_Node( *(yystack_[0].value.stringVal) ); 
				delete (yystack_[0].value.stringVal);
			}
#line 890 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 5:
#line 141 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_Variable_Node( *(yystack_[0].value.stringVal) ); 
				delete (yystack_[0].value.stringVal);				
			}
#line 899 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 6:
#line 147 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_Neighbor12D_Node( *(yystack_[2].value.stringVal) ); 
				delete (yystack_[2].value.stringVal);				
			}
#line 908 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 7:
#line 153 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_Neighbor12D_Node( *(yystack_[2].value.stringVal) ); 
				delete (yystack_[2].value.stringVal);				
			}
#line 917 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 8:
#line 159 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_AllFunc_Node( *(yystack_[2].value.stringVal) ); 
				delete (yystack_[2].value.stringVal);				
			}
#line 926 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 9:
#line 165 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_AbsFunc_Node( *(yystack_[3].value.stringVal), (yystack_[1].value.calcnode) ); 
				delete (yystack_[3].value.stringVal);				
			}
#line 935 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 10:
#line 171 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = new SP_CPN_Parse_ValFunc_Node( *(yystack_[3].value.stringVal), (yystack_[1].value.calcnode) ); 
				delete (yystack_[3].value.stringVal);				
			}
#line 944 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 11:
#line 177 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				
				(yylhs.value.calcnode) = new SP_CPN_Parse_Function_Node( (yystack_[3].value.calcnode),(yystack_[1].value.calcnode) ); 				
				
			}
#line 954 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 12:
#line 183 "SP_CPN_Parser.yy" // lalr1.cc:859
    {				
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);				
			}
#line 962 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 13:
#line 187 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = new SP_CPN_Parse_Argument_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
			}
#line 970 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 14:
#line 192 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 978 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 15:
#line 196 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 986 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 16:
#line 201 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 994 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 17:
#line 206 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 1002 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 18:
#line 210 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 1010 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 19:
#line 214 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 1018 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 20:
#line 218 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 1026 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 21:
#line 223 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
			}
#line 1034 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 22:
#line 228 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
				(yylhs.value.calcnode) = (yystack_[1].value.calcnode);
			}
#line 1042 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 23:
#line 234 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
			(yylhs.value.calcnode) = (yystack_[1].value.calcnode);
		 }
#line 1050 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 24:
#line 240 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
			(yylhs.value.calcnode) = (yystack_[1].value.calcnode);
		 }
#line 1058 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 25:
#line 248 "SP_CPN_Parser.yy" // lalr1.cc:859
    {		
		(yylhs.value.calcnode) = new SP_CPN_Parse_Bracket_Node((yystack_[1].value.calcnode));
	 }
#line 1066 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 26:
#line 255 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_Dot_Node((yystack_[2].value.calcnode), (yystack_[0].value.integerVal));
	      }
#line 1074 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 27:
#line 259 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
			(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
		  }
#line 1082 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 28:
#line 263 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_Index_Node((yystack_[3].value.calcnode), (yystack_[1].value.calcnode));
	      }
#line 1090 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 29:
#line 270 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	      }
#line 1098 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 30:
#line 274 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_Successor_Node((yystack_[0].value.calcnode));
	      }
#line 1106 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 31:
#line 278 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_Predecessor_Node((yystack_[0].value.calcnode));
	      }
#line 1114 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 32:
#line 282 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_IndexAbstract_Node((yystack_[0].value.calcnode));
	      }
#line 1122 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 33:
#line 286 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
			(yylhs.value.calcnode) = new SP_CPN_Parse_Not_Node((yystack_[0].value.calcnode));
	      }
#line 1130 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 34:
#line 291 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1138 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 35:
#line 295 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Multiply_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1146 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 36:
#line 299 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Divide_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1154 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 37:
#line 303 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Modulo_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1162 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 38:
#line 307 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Power_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1170 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 39:
#line 312 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1178 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 40:
#line 316 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Add_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1186 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 41:
#line 320 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = new SP_CPN_Parse_Substract_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1194 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 42:
#line 324 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) =new SP_CPN_Parse_NumOf_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1202 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 43:
#line 332 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1210 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 44:
#line 337 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_LessThan_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1218 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 45:
#line 341 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_GreaterThan_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1226 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 46:
#line 345 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_LessEqual_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1234 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 47:
#line 349 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_GreaterEqual_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1242 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 48:
#line 355 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	 (yylhs.value.calcnode) =new SP_CPN_Parse_Element_Of_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	 }
#line 1250 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 49:
#line 362 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1258 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 50:
#line 366 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_Equal_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1266 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 51:
#line 370 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_NotEqual_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1274 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 52:
#line 377 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1282 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 53:
#line 382 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_And_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1290 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 54:
#line 389 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1298 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 55:
#line 394 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = new SP_CPN_Parse_Or_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	  }
#line 1306 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 56:
#line 402 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	 }
#line 1314 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 57:
#line 406 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_Comma_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1322 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 58:
#line 410 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_Comma_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1330 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 59:
#line 417 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	}
#line 1338 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 60:
#line 421 "SP_CPN_Parser.yy" // lalr1.cc:859
    {	
		(yylhs.value.calcnode) = (yystack_[1].value.calcnode);
	 }
#line 1346 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 61:
#line 425 "SP_CPN_Parser.yy" // lalr1.cc:859
    {		
		(yylhs.value.calcnode) = new SP_CPN_Parse_Bracket_Node((yystack_[1].value.calcnode));
	 }
#line 1354 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 62:
#line 432 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	}
#line 1362 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 63:
#line 436 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_Seperator_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1370 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 64:
#line 445 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	}
#line 1378 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 65:
#line 449 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_Predicate_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1386 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 66:
#line 453 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_Predicate_Node((yystack_[4].value.calcnode), (yystack_[1].value.calcnode));
	}
#line 1394 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 67:
#line 460 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	}
#line 1402 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 68:
#line 464 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_CNN_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1410 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 69:
#line 468 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
		(yylhs.value.calcnode) = new SP_CPN_Parse_CDD_Node((yystack_[2].value.calcnode), (yystack_[0].value.calcnode));
	}
#line 1418 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 70:
#line 476 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      (yylhs.value.calcnode) = (yystack_[0].value.calcnode);
	  }
#line 1426 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 74:
#line 485 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      driver.calc.m_ExpressionsVector.push_back((yystack_[1].value.calcnode));
	  }
#line 1434 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 75:
#line 489 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      driver.calc.m_ExpressionsVector.push_back((yystack_[1].value.calcnode));
	  }
#line 1442 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;

  case 76:
#line 493 "SP_CPN_Parser.yy" // lalr1.cc:859
    {
	      driver.calc.m_ExpressionsVector.push_back((yystack_[1].value.calcnode));
	  }
#line 1450 "SP_CPN_Parser.cc" // lalr1.cc:859
    break;


#line 1454 "SP_CPN_Parser.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
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
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
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

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
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
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  SP_CPN_Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  SP_CPN_Parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
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
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
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
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
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

    char const* yyformat = YY_NULLPTR;
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

    std::string yyres;
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


  const signed char SP_CPN_Parser::yypact_ninf_ = -39;

  const signed char SP_CPN_Parser::yytable_ninf_ = -57;

  const short int
  SP_CPN_Parser::yypact_[] =
  {
     -39,    14,   -39,   158,   158,   158,   158,   -39,   -39,   -39,
     -39,   -39,   -38,   -31,   -14,    -5,    22,    56,    96,   -39,
     -39,    38,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -18,
     -39,    46,    83,     2,     5,    10,    69,   -39,    74,   -39,
       7,     8,    56,   -18,   -18,   -18,   -18,    61,    76,    79,
      96,    96,    20,    -3,    16,     3,    82,    17,    96,    81,
     158,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,   117,    56,    56,
     -39,   -39,   -39,    42,    18,   -39,   -39,   -39,    58,    77,
     -39,   117,   -39,   -39,   117,   -39,   138,    23,    69,   -39,
      72,   -39,   -39,   -39,   -39,    46,    46,    46,    83,    83,
      83,    83,    83,     2,     2,     5,    10,   -39,   -39,   -39,
     -39,   -39,   -39,   -39,   -39,   -39,    56,    74,    96,   -39,
     -39,    12,    69,   -39
  };

  const unsigned char
  SP_CPN_Parser::yydefact_[] =
  {
      71,     0,     1,     0,     0,     0,     0,    73,     2,     3,
       5,     4,     0,     0,     0,     0,     0,     0,     0,    72,
      14,    15,    16,    17,    18,    19,    20,    21,    27,    29,
      34,    39,    43,    49,    52,    54,    59,    62,    64,    67,
      70,     0,     0,    30,    31,    32,    33,     0,     0,     0,
       0,     0,    59,     0,    62,    70,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      76,    75,    74,    59,     0,     6,     7,     8,     0,     0,
      23,     0,    25,    24,     0,    22,     0,     0,    12,    26,
       0,    35,    36,    37,    38,    40,    41,    42,    46,    47,
      45,    44,    48,    51,    50,    53,    55,    63,    68,    69,
      23,    25,     9,    10,    57,    58,     0,    65,     0,    11,
      28,     0,    13,    66
  };

  const short int
  SP_CPN_Parser::yypgoto_[] =
  {
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
      86,    67,    43,    73,   115,    35,    84,    85,   -16,   100,
     -13,    64,    66,    -1,   162,   -39
  };

  const signed char
  SP_CPN_Parser::yydefgoto_[] =
  {
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    97,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    53,
      37,    38,    39,    55,    56,     1
  };

  const short int
  SP_CPN_Parser::yytable_[] =
  {
      40,    52,    57,    47,    54,    68,    69,    59,    80,    91,
      48,    73,    78,    79,     2,    94,    78,    79,    70,    71,
      74,    78,    79,    75,    94,    60,    83,    49,   -56,    54,
      91,    76,     3,     4,    76,   128,    50,    81,     5,    92,
       6,    72,    98,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    82,   133,    17,    76,    18,    93,    19,
     121,    96,    90,    51,   117,   129,    61,    62,    63,    64,
      43,    44,    45,    46,     3,     4,    65,    66,   124,    58,
       5,   125,     6,    76,   120,    77,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    65,    66,    17,    67,    18,
     122,    65,    66,    85,   101,   102,   103,   104,   113,   114,
      52,    99,   132,    54,     3,     4,   130,    67,    86,   123,
       5,    87,     6,    67,    95,   131,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     3,     4,    42,   105,   106,
     107,     5,    84,     6,   118,   119,   100,     8,     9,    10,
      11,    12,    13,    14,    15,    16,     3,     4,    17,   115,
     127,   116,     5,    41,     6,    88,    89,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,   126,
       0,     0,     0,   108,   109,   110,   111,   112,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,    42
  };

  const short int
  SP_CPN_Parser::yycheck_[] =
  {
       1,    17,    18,    41,    17,     3,     4,    25,     0,    12,
      41,     6,     9,    10,     0,    12,     9,    10,    16,    17,
      15,     9,    10,    13,    12,    43,    42,    41,    12,    42,
      12,    14,    18,    19,    14,    12,    41,    29,    24,    42,
      26,    39,    58,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    45,    42,    41,    14,    43,    42,    45,
      42,    44,    42,    41,    77,    42,    20,    21,    22,    23,
       3,     4,     5,     6,    18,    19,    18,    19,    91,    41,
      24,    94,    26,    14,    42,    11,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    18,    19,    41,    40,    43,
      42,    18,    19,    42,    61,    62,    63,    64,    73,    74,
     126,    30,   128,   126,    18,    19,    44,    40,    42,    42,
      24,    42,    26,    40,    42,   126,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    18,    19,    41,    65,    66,
      67,    24,    42,    26,    78,    79,    60,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    18,    19,    41,    75,
      96,    76,    24,     1,    26,    50,    51,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    41,
      -1,    -1,    -1,    68,    69,    70,    71,    72,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    41
  };

  const unsigned char
  SP_CPN_Parser::yystos_[] =
  {
       0,    71,     0,    18,    19,    24,    26,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    41,    43,    45,
      47,    48,    49,    50,    51,    52,    53,    54,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    66,    67,    68,
      69,    70,    41,    57,    57,    57,    57,    41,    41,    41,
      41,    41,    64,    65,    66,    69,    70,    64,    41,    25,
      43,    20,    21,    22,    23,    18,    19,    40,     3,     4,
      16,    17,    39,     6,    15,    13,    14,    11,     9,    10,
       0,    29,    45,    64,    65,    42,    42,    42,    60,    60,
      42,    12,    42,    42,    12,    42,    44,    55,    64,    30,
      56,    58,    58,    58,    58,    59,    59,    59,    60,    60,
      60,    60,    60,    61,    61,    62,    63,    66,    68,    68,
      42,    42,    42,    42,    66,    66,    41,    67,    12,    42,
      44,    69,    64,    42
  };

  const unsigned char
  SP_CPN_Parser::yyr1_[] =
  {
       0,    46,    47,    47,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    55,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    57,    57,    57,    58,
      58,    58,    58,    58,    59,    59,    59,    59,    59,    60,
      60,    60,    60,    61,    61,    61,    61,    61,    61,    62,
      62,    62,    63,    63,    64,    64,    65,    65,    65,    66,
      66,    66,    67,    67,    68,    68,    68,    69,    69,    69,
      70,    71,    71,    71,    71,    71,    71
  };

  const unsigned char
  SP_CPN_Parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     3,     3,     3,     4,
       4,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     1,     4,     1,
       2,     2,     2,     2,     1,     3,     3,     3,     3,     1,
       3,     3,     3,     1,     3,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     1,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     1,     4,     6,     1,     3,     3,
       1,     0,     2,     2,     3,     3,     3
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SP_CPN_Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "LE_OP", "GE_OP", "ELEM_OP",
  "NE_OP", "IF", "INDEX", "CNN", "CDD", "'`'", "','", "'&'", "'|'", "'='",
  "'>'", "'<'", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "'@'", "':'",
  "'!'", "INC_OP", "DEC_OP", "\"end of line\"", "\"integer\"",
  "\"double\"", "\"identifier\"", "\"string\"", "\"nb1Dfunc\"",
  "\"nb2Dfunc\"", "\"allfunc\"", "\"absfunc\"", "\"valfunc\"",
  "\"elemOfOP\"", "\"numOfOP\"", "'('", "')'", "'['", "']'", "';'",
  "$accept", "CPN_Constant", "CPN_Variable", "CPN_Neighbor1D",
  "CPN_Neighbor2D", "CPN_AllFunc", "CPN_ABSFunc", "CPN_VALFunc",
  "CPN_Function", "CPN_ArgumentList", "CPN_Atomexpr", "CPN_Postfixexpr",
  "CPN_Unaryexpr", "CPN_Mulexpr", "CPN_Addexpr", "CPN_Relexpr",
  "CPN_Eqexpr", "CPN_Andexpr", "CPN_Orexpr", "CPN_Commaexpr",
  "CPN_Normalexpr", "CPN_Seperatorexpr", "CPN_Predicateexpr",
  "CPN_Connectorexpr", "CPN_expr", "start", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  SP_CPN_Parser::yyrline_[] =
  {
       0,   126,   126,   130,   134,   140,   146,   152,   158,   164,
     170,   176,   182,   186,   191,   195,   200,   205,   209,   213,
     217,   222,   227,   233,   239,   247,   254,   258,   262,   269,
     273,   277,   281,   285,   290,   294,   298,   302,   306,   311,
     315,   319,   323,   331,   336,   340,   344,   348,   354,   361,
     365,   369,   376,   381,   388,   393,   401,   405,   409,   416,
     420,   424,   431,   435,   444,   448,   452,   459,   463,   467,
     475,   481,   482,   483,   484,   488,   492
  };

  // Print the state stack on the debug stream.
  void
  SP_CPN_Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SP_CPN_Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
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
       2,     2,     2,    26,     2,     2,     2,    22,    13,     2,
      41,    42,    20,    18,    12,    19,     2,    21,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    25,    45,
      17,    15,    16,     2,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    43,     2,    44,    23,     2,    11,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    14,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40
    };
    const unsigned int user_token_number_max_ = 279;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // SP_CPN
#line 1979 "SP_CPN_Parser.cc" // lalr1.cc:1167
#line 499 "SP_CPN_Parser.yy" // lalr1.cc:1168
 /*** Additional Code ***/

void SP_CPN::SP_CPN_Parser::error(const SP_CPN_Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}
