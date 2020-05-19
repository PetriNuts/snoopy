//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/03 09:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <wx/valgen.h>
#include <wx/regex.h>

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"

#include "SP_DS_StParser.h"

#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimGillespie.h"

//by george for constants harmonizing
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
SP_DS_StParser :: SP_DS_StParser(SP_DS_Graph* p_pcGraph) :
m_pcGraph(p_pcGraph),
m_nPlaceCount( 0 ),
m_bParseErrorParantheses( 0 ),
m_bOnlyCheck( 0 ),
m_bOnlyPrePlaces( 0 ),
m_bFirstOperatorDone( 0 ),
m_nCurrentTransitionNumber( 0 ),
m_pcCurrentTransitionNode(NULL),
m_anCurrentMarking( 0 ),
m_bUseConcentration(false),
m_nSimIntervalStart( 0 ),
m_nSimIntervalEnd( 0 ),
m_mnParameter( 0 ),
m_mnPlaceArrayPos( 0 ),
m_nPlaceArrayPosRefCount(0),
m_mnTransitionArrayPos( 0 ),
m_pcSimTransition(0),
m_nSimTime( 0 ),
m_aanDeltaIncMatrix( 0 ),
m_aanPreIncMatrix( 0 ),
m_pcInfixOperatorList( 0 ),
m_pcPrefixOperatorList( 0 ),
m_pcBinaryOperatorList( 0 ),
m_pcFunction0ParamOperatorList( 0 ),
m_pcFunction2ParamOperatorList( 0 ),
m_pcFunction3ParamOperatorList( 0 ),
m_bColoredPN(false)
{
	if(!m_pcGraph)
	{
		m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	}
	m_mnParameter = new SP_MapString2Double();
	m_mnPlaceArrayPos = new SP_MapString2Long();

	LoadLookupTables();
	LoadParameter();

	LoadPlaces();
	LoadPrePlaces();

	long i = 0;
	m_pcFunction0ParamOperatorList = new Function0ParamOperator[ 1 ];
/*
	m_pcFunction0ParamOperatorList[ i ].string = wxT("ImmediateFiring");
	m_pcFunction0ParamOperatorList[ i ].match_string = wxT("ImmediateFiring(");
	m_pcFunction0ParamOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction0Param;
	m_pcFunction0ParamOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcFunction0ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
*/
	m_pcFunction0ParamOperatorList[ i ].ptr = 0;

	m_pcInfixOperatorList = new InfixOperator[ 11 ];
	i=0;
	m_pcInfixOperatorList[ i ].chr = wxT('+');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::CalcInfixPlus;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('-');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::CalcInfixMinus;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('*');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::CalcInfixTimes;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('/');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::CalcInfixDivide;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('^');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::CalcInfixPower;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('>');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnGt;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('<');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnLt;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('=');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnEq;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('&');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnAnd;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT('|');
	m_pcInfixOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnOr;
	m_pcInfixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN;
	i++;
	m_pcInfixOperatorList[ i ].chr = wxT(' ');
	m_pcInfixOperatorList[ i ].ptr = NULL;

    m_pcPrefixOperatorList = new PrefixOperator[ 27 ];
	i=0;
	m_pcPrefixOperatorList[ i ].string = wxT("MassAction");
	m_pcPrefixOperatorList[ i ].match_string = wxT("MassAction(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcMassAction;
    m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("BioMassAction");
	m_pcPrefixOperatorList[ i ].match_string = wxT("BioMassAction(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcMassAction;
    m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("linlog");
	m_pcPrefixOperatorList[ i ].match_string = wxT("linlog(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("nlinlog");
	m_pcPrefixOperatorList[ i ].match_string = wxT("nlinlog(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("ImmediateFiring");
	m_pcPrefixOperatorList[ i ].match_string = wxT("ImmediateFiring(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("FixedTimedFiring_Single");
	m_pcPrefixOperatorList[ i ].match_string = wxT("FixedTimedFiring_Single(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("TimedFiring");
	m_pcPrefixOperatorList[ i ].match_string = wxT("TimedFiring(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("Summation");
	m_pcPrefixOperatorList[ i ].match_string = wxT("Summation(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("Product");
	m_pcPrefixOperatorList[ i ].match_string = wxT("Product(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("Multiplicity");
	m_pcPrefixOperatorList[ i ].match_string = wxT("Multiplicity(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction1Param;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("acos");
	m_pcPrefixOperatorList[ i ].match_string = wxT("acos(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixAcos;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("asin");
	m_pcPrefixOperatorList[ i ].match_string = wxT("asin(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixAsin;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("atan");
	m_pcPrefixOperatorList[ i ].match_string = wxT("atan(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixAtan;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("ceil");
	m_pcPrefixOperatorList[ i ].match_string = wxT("ceil(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixCeil;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("cos");
	m_pcPrefixOperatorList[ i ].match_string = wxT("cos(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixCos;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("exp");
	m_pcPrefixOperatorList[ i ].match_string = wxT("exp(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixExp;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("floor");
	m_pcPrefixOperatorList[ i ].match_string = wxT("floor(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixFloor;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("log");
	m_pcPrefixOperatorList[ i ].match_string = wxT("log(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixLog;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("log10");
	m_pcPrefixOperatorList[ i ].match_string = wxT("log10(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixLog10;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("sin");
	m_pcPrefixOperatorList[ i ].match_string = wxT("sin(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixSin;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("sqr");
	m_pcPrefixOperatorList[ i ].match_string = wxT("sqr(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixSqr;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("sqrt");
	m_pcPrefixOperatorList[ i ].match_string = wxT("sqrt(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixSqrt;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("tan");
	m_pcPrefixOperatorList[ i ].match_string = wxT("tan(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixTan;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("abs");
	m_pcPrefixOperatorList[ i ].match_string = wxT("abs(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixAbs;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("-");
	m_pcPrefixOperatorList[ i ].match_string = wxT("-(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixMinus;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcPrefixOperatorList[ i ].string = wxT("not");
	m_pcPrefixOperatorList[ i ].match_string = wxT("not(");
	m_pcPrefixOperatorList[ i ].ptr = &SP_DS_StParser::CalcPrefixNot;
	m_pcPrefixOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcPrefixOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcPrefixOperatorList[ i ].string = NULL;
	m_pcPrefixOperatorList[ i ].match_string = NULL;
	m_pcPrefixOperatorList[ i ].ptr = NULL;

    m_pcFunction2ParamOperatorList = new Function2ParamOperator[6];
    i=0;
	m_pcFunction2ParamOperatorList[ i ].string = wxT("LevelInterpretation");
	m_pcFunction2ParamOperatorList[ i ].match_string = wxT("LevelInterpretation(");
	m_pcFunction2ParamOperatorList[ i ].ptr = &SP_DS_StParser::CalcLevelInterpretation;
	m_pcFunction2ParamOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcFunction2ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcFunction2ParamOperatorList[ i ].string = wxT("BioLevelInterpretation");
	m_pcFunction2ParamOperatorList[ i ].match_string = wxT("BioLevelInterpretation(");
	m_pcFunction2ParamOperatorList[ i ].ptr = &SP_DS_StParser::CalcLevelInterpretation;
	m_pcFunction2ParamOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcFunction2ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcFunction2ParamOperatorList[ i ].string = wxT("BinomialCoefficient");
	m_pcFunction2ParamOperatorList[ i ].match_string = wxT("BinomialCoefficient(");
	m_pcFunction2ParamOperatorList[ i ].ptr = &SP_DS_StParser::CalcBinomialCoefficient;
	m_pcFunction2ParamOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcFunction2ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcFunction2ParamOperatorList[ i ].string = wxT("LigandBindingPotential");
	m_pcFunction2ParamOperatorList[ i ].match_string = wxT("LigandBindingPotential(");
	m_pcFunction2ParamOperatorList[ i ].ptr = &SP_DS_StParser::CalcLigandBindingPotential;
	m_pcFunction2ParamOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcFunction2ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
    m_pcFunction2ParamOperatorList[ i ].string = wxT("MichaelisMenten");
	m_pcFunction2ParamOperatorList[ i ].match_string = wxT("MichaelisMenten(");
	m_pcFunction2ParamOperatorList[ i ].ptr = &SP_DS_StParser::CalcMichaelisMenten;
    m_pcFunction2ParamOperatorList[ i ].bOnlyFirstOperator = false;
    m_pcFunction2ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcFunction2ParamOperatorList[ i ].string = NULL;
	m_pcFunction2ParamOperatorList[ i ].match_string = NULL;
	m_pcFunction2ParamOperatorList[ i ].ptr = NULL;

	m_pcFunction3ParamOperatorList = new Function3ParamOperator[ 4 ];
	i=0;
	m_pcFunction3ParamOperatorList[ i ].string = wxT("FixedTimedFiring_Periodic");
	m_pcFunction3ParamOperatorList[ i ].match_string = wxT("FixedTimedFiring_Periodic(");
	m_pcFunction3ParamOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction3Param;
	m_pcFunction3ParamOperatorList[ i ].bOnlyFirstOperator = true;
	m_pcFunction3ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcFunction3ParamOperatorList[ i ].string = wxT("Move2DGrid");
	m_pcFunction3ParamOperatorList[ i ].match_string = wxT("Move2DGrid(");
	m_pcFunction3ParamOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction3Param;
	m_pcFunction3ParamOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcFunction3ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;
	i++;
	m_pcFunction3ParamOperatorList[ i ].string = wxT("CountXY");
	m_pcFunction3ParamOperatorList[ i ].match_string = wxT("CountXY(");
	m_pcFunction3ParamOperatorList[ i ].ptr = &SP_DS_StParser::DummyFunction3Param;
	m_pcFunction3ParamOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcFunction3ParamOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD;

	i++;
	m_pcFunction3ParamOperatorList[ i ].string = NULL;
	m_pcFunction3ParamOperatorList[ i ].match_string = NULL;
	m_pcFunction3ParamOperatorList[ i ].ptr = NULL;



	m_pcBinaryOperatorList = new BinaryOperator[ 12 ];
	i=0;
	m_pcBinaryOperatorList[ i ].string = wxT("pow");
	m_pcBinaryOperatorList[ i ].match_string = wxT("pow(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::CalcBinaryPower;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("min");
	m_pcBinaryOperatorList[ i ].match_string = wxT("min(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::CalcBinaryMin;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("max");
	m_pcBinaryOperatorList[ i ].match_string = wxT("max(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::CalcBinaryMax;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("gt");
	m_pcBinaryOperatorList[ i ].match_string = wxT("gt(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnGt;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("lt");
	m_pcBinaryOperatorList[ i ].match_string = wxT("lt(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnLt;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("geq");
	m_pcBinaryOperatorList[ i ].match_string = wxT("geq(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnGeq;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("leq");
	m_pcBinaryOperatorList[ i ].match_string = wxT("leq(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnLeq;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("eq");
	m_pcBinaryOperatorList[ i ].match_string = wxT("eq(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnEq;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("neq");
	m_pcBinaryOperatorList[ i ].match_string = wxT("neq(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnNeq;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("and");
	m_pcBinaryOperatorList[ i ].match_string = wxT("and(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnAnd;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = wxT("or");
	m_pcBinaryOperatorList[ i ].match_string = wxT("or(");
	m_pcBinaryOperatorList[ i ].ptr = &SP_DS_StParser::EvalOpnOr;
	m_pcBinaryOperatorList[ i ].bOnlyFirstOperator = false;
	m_pcBinaryOperatorList[ i ].visibleGroup = SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE;
	i++;
	m_pcBinaryOperatorList[ i ].string = NULL;
	m_pcBinaryOperatorList[ i ].match_string = NULL;
	m_pcBinaryOperatorList[ i ].ptr = NULL;


}

SP_DS_StParserNode*
SP_DS_StParser :: CreateTree( wxChar* p_sFormula )
{

	wxChar* l_psStripped = new wxChar[ wxStrlen( p_sFormula ) + 1 ];
	wxChar* l_psSp;

	const wxChar* l_psFp;

	l_psSp = l_psStripped;
	l_psFp = p_sFormula;

	while( *l_psFp )
	{
		if( ! wxIsspace( *l_psFp ) )
		{
			*l_psSp = *l_psFp;
			l_psSp++;
		}

		l_psFp++;
	}

	*l_psSp = wxT('\0');

	SP_DS_StParserNode* l_pcRoot;
	l_pcRoot = Parse( l_psStripped );
	wxDELETEA(l_psStripped);

	return( l_pcRoot );

}

SP_DS_StParserNode*
SP_DS_StParser::Parse( wxChar* p_sFormula )
{

	SP_DS_StParserNode* l_pcRoot = NULL;
	wxChar* p = 0;

	InfixOperator* l_pcInfixOperator;
	PrefixOperator* l_pcPrefixOperator;

	BinaryOperator* l_pcBinaryOperator;

	Function0ParamOperator* l_pcFunction0ParamOperator;

	Function2ParamOperator* l_pcFunction2ParamOperator;
	Function3ParamOperator* l_pcFunction3ParamOperator;


	/* first check for infix operators */

	l_pcInfixOperator = m_pcInfixOperatorList;

	while( l_pcInfixOperator->ptr ) {

		bool l_bMatchFirst = false;
		if(l_pcInfixOperator->chr == wxT('^'))
		{
			l_bMatchFirst = true;
		}
		if( ( p = Scan( p_sFormula, l_pcInfixOperator->chr, l_bMatchFirst ) ) ) {

			l_pcRoot = CreateParserNodeForInfixOperator( p_sFormula, l_pcInfixOperator->ptr, p );
			return l_pcRoot;

		}

		l_pcInfixOperator++;

	}


	/* now check for parentheses */

	if ( *p_sFormula == wxT('(') ) {

		long l_nLen = wxStrlen( p_sFormula );

		if( ! ( p_sFormula[ l_nLen - 1 ] == wxT(')') ) ) {

			m_bParseErrorParantheses = true;
			return( NULL );

		}

		p_sFormula[ l_nLen - 1 ] = wxT('\0');
		return ( Parse( ++p_sFormula ) );

	}

	/* now check for prefix operators (which must be unary) */

	l_pcPrefixOperator = m_pcPrefixOperatorList;

	while( l_pcPrefixOperator->ptr ) {

		if( wxStrstr( p_sFormula, l_pcPrefixOperator->match_string ) == p_sFormula ) {

			if( m_bFirstOperatorDone && l_pcPrefixOperator->bOnlyFirstOperator ) {

				m_asMissingVariableList.Add( wxString(p_sFormula, wxConvUTF8) );

			}

			m_bFirstOperatorDone = true;

			if( wxStrcmp( l_pcPrefixOperator->match_string, wxT("MassAction(") ) == 0 ) {

				l_pcRoot = CreateParserNodeForMassActionOperator( p_sFormula, l_pcPrefixOperator->ptr );

			}
			else {

				l_pcRoot = CreateParserNodeForPrefixOperator( p_sFormula, l_pcPrefixOperator->ptr );

			}

			return l_pcRoot;

		}

		l_pcPrefixOperator++;

	}

	/* now check for lookup tables (which must be unary) */

	map< wxString, SP_MapString2Double >::iterator l_cIter =
    m_mmLookupTablesMap.begin();

	for(; l_cIter != m_mmLookupTablesMap.end(); ++l_cIter ) {

		if( wxString(p_sFormula, wxConvUTF8).StartsWith(l_cIter->first) ) {

			l_pcRoot = CreateParserNodeForLookupTable( p_sFormula, l_cIter->first );

			return l_pcRoot;

		}

	}

	l_pcFunction0ParamOperator = m_pcFunction0ParamOperatorList;

	while( l_pcFunction0ParamOperator->ptr ) {

		if( wxStrstr( p_sFormula, l_pcFunction0ParamOperator->match_string ) == p_sFormula ) {

			long l_nLen = wxStrlen( p_sFormula );

			if( ! ( p_sFormula[ l_nLen - 1 ] == wxT(')') ) ) {

				m_asMissingVariableList.Add( wxString(p_sFormula, wxConvUTF8) );
				return ( NULL );

			}

			p_sFormula[ l_nLen - 1 ] = wxT('\0');

			if( m_bFirstOperatorDone && l_pcFunction0ParamOperator->bOnlyFirstOperator ) {

				m_asMissingVariableList.Add( wxString(p_sFormula, wxConvUTF8) );

			}

			m_bFirstOperatorDone = true;

			l_pcRoot = CreateParserNodeForFunction0ParamOperator( p_sFormula,
																 l_pcFunction0ParamOperator->ptr );

			return l_pcRoot;

		}

		l_pcFunction0ParamOperator++;

	}


	l_pcFunction2ParamOperator = m_pcFunction2ParamOperatorList;

	while( l_pcFunction2ParamOperator->ptr ) 
	{

		if( wxStrstr( p_sFormula, l_pcFunction2ParamOperator->match_string ) == p_sFormula ) 
		{

			long l_nLen = wxStrlen( p_sFormula );

			if( ! ( p_sFormula[ l_nLen - 1 ] == wxT(')') ) )
			{

				m_asMissingVariableList.Add( wxString( p_sFormula ) );
				return ( NULL );

			}

			p_sFormula[ l_nLen - 1 ] = wxT('\0');

			if( m_bFirstOperatorDone && l_pcFunction2ParamOperator->bOnlyFirstOperator ) 
			{

				m_asMissingVariableList.Add( wxString( p_sFormula ) );

			}

			m_bFirstOperatorDone = true;

			if( wxStrcmp( l_pcFunction2ParamOperator->match_string, wxT("LevelInterpretation(") ) == 0 ) 
			{

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				if ( ! ( p = Scan( p_sFormula, wxT(',') ) ) )
				{

					m_asMissingVariableList.Add( wxString( p_sFormula ) );

				}

				l_pcRoot = CreateParserNodeForLevelInterpretationOperator( p_sFormula,
																			 l_pcFunction2ParamOperator->ptr );

			}

			else if( wxStrcmp( l_pcFunction2ParamOperator->match_string, wxT("MichaelisMenten(") ) == 0 ) 
			{

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				if ( ! ( p = Scan( p_sFormula, wxT(',') ) ) )
				{

					m_asMissingVariableList.Add( wxString( p_sFormula ) );

				}

				l_pcRoot = CreateParserNodeForMichaelisMentenOperator( p_sFormula,
																			 l_pcFunction2ParamOperator->ptr );

			}

			else 
			{

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				l_pcRoot = CreateParserNodeForFunction2ParamOperator( p_sFormula,
																	 l_pcFunction2ParamOperator->ptr );

			}

			return l_pcRoot;

		}

		l_pcFunction2ParamOperator++;

	}


	l_pcFunction3ParamOperator = m_pcFunction3ParamOperatorList;

	while( l_pcFunction3ParamOperator->ptr ) {

		if( wxStrstr( p_sFormula, l_pcFunction3ParamOperator->match_string ) == p_sFormula ) {

			long l_nLen = wxStrlen( p_sFormula );

			if( ! ( p_sFormula[ l_nLen - 1 ] == wxT(')') ) ) {

				m_asMissingVariableList.Add( wxString( p_sFormula ) );
				return ( NULL );

			}

			p_sFormula[ l_nLen - 1 ] = '\0';

			if( m_bFirstOperatorDone && l_pcFunction3ParamOperator->bOnlyFirstOperator ) {

				m_asMissingVariableList.Add( wxString( p_sFormula ) );

			}

			m_bFirstOperatorDone = true;

			if( wxStrcmp( l_pcFunction3ParamOperator->match_string, wxT("FixedTimedFiring_Periodic(") ) == 0 ) {

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				l_pcRoot = CreateParserNodeForFixedTimedFiring_Periodic_IntervalOperator( p_sFormula,
																						 l_pcFunction3ParamOperator->ptr );

			}
			else if( wxStrcmp( l_pcFunction3ParamOperator->match_string, wxT("Move2DGrid(") ) == 0 ) {

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				l_pcRoot = CreateParserNodeForMove2DGridOperator( p_sFormula,
																 l_pcFunction3ParamOperator->ptr );

			}
			else if( wxStrcmp( l_pcFunction3ParamOperator->match_string, wxT("CountXY(") ) == 0 ) {

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				l_pcRoot = CreateParserNodeForCountXYOperator( p_sFormula,
																 l_pcFunction3ParamOperator->ptr );

			}
			else {

				p_sFormula = wxStrchr( p_sFormula, wxT('(') );
				p_sFormula++;

				l_pcRoot = CreateParserNodeForFunction3ParamOperator( p_sFormula,
																	 l_pcFunction3ParamOperator->ptr );

			}

			return l_pcRoot;

		}

		l_pcFunction3ParamOperator++;

	}

	/* now check for binary prefix operators */

	l_pcBinaryOperator = m_pcBinaryOperatorList;

	while( l_pcBinaryOperator->ptr ) {

		if( wxStrstr( p_sFormula, l_pcBinaryOperator->match_string ) == p_sFormula ) {

			long l_nLen = wxStrlen( p_sFormula );

			if( ! ( p_sFormula[ l_nLen - 1 ] == wxT(')') ) ) {

				m_asMissingVariableList.Add( wxString( p_sFormula ) );
				return ( NULL );

			}

			p_sFormula[ l_nLen - 1 ] = '\0';

			if( m_bFirstOperatorDone && l_pcBinaryOperator->bOnlyFirstOperator ) {

				m_asMissingVariableList.Add( wxString( p_sFormula ) );

			}

			m_bFirstOperatorDone = true;

			p_sFormula = wxStrchr( p_sFormula, wxT('(') );
			p_sFormula++;

			if ( ! ( p = Scan( p_sFormula, wxT(',') ) ) ) {

				m_asMissingVariableList.Add( wxString( p_sFormula ) );

			}

			l_pcRoot = CreateParserNodeForInfixOperator( p_sFormula, l_pcBinaryOperator->ptr, p );
			return l_pcRoot;

		}

		l_pcBinaryOperator++;

	}

	/* now check for unary minus */

	if ( *p_sFormula == wxT('-') ) {

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_PREFIX );

		( l_pcRoot->info ).PrefixOperator = &SP_DS_StParser::CalcPrefixMinus;

		l_pcRoot->left = Parse( ++p_sFormula );

		if( l_pcRoot->left == NULL ) {

			FreeTree(l_pcRoot);
			return NULL;

		}
		else {

			return l_pcRoot;

		}

	}
	else if ( *p_sFormula == wxT('+') ) {   /* now check for unary plus */

		return( Parse( ++p_sFormula ) );

	}
	else if ( ! wxIsdigit( *p_sFormula ) ) {   /* now check for a variable */

		l_pcRoot = CreateParserNodeForVariable( p_sFormula );
		return l_pcRoot;

	}
	else {    /* now must be a constant */

		double l_nDouble = -1;
		wxString l_sFormula( p_sFormula );

		if( l_sFormula.ToDouble( &l_nDouble ) ) {

			l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_CONSTANT );
			( l_pcRoot->info ).constD = l_nDouble;

		}
		else {

			l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_VARIABLE_ERROR );
			m_asMissingVariableList.Add( l_sFormula );

		}

		return l_pcRoot;

	}

}


SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForInfixOperator( wxChar* p_sFormula, double ( SP_DS_StParser::*p_pfOperator )( double, double ), wxChar* p )
{

	SP_DS_StParserNode* l_pcRoot;
	*p = '\0';

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_INFIX );
	( l_pcRoot->info ).InfixOperator = p_pfOperator;

	l_pcRoot->left = Parse( p_sFormula );
	l_pcRoot->right = Parse( ++p );

	if( ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}


SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForFunction2ParamOperator( wxChar* p_sFormula,
															double ( SP_DS_StParser::*p_pfOperator )( double, double ) )

{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_FUNCTION_2_PARAMETER );
	( l_pcRoot->info ).Function2ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->right = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if(
	   ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}


SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForFunction3ParamOperator( wxChar* p_sFormula,
															double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) )

{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER );
	( l_pcRoot->info ).Function3ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->middle = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->right = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if(
	   ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ||
	   ( l_pcRoot->middle == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}


SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForFixedTimedFiring_Periodic_IntervalOperator( wxChar* p_sFormula,
																				double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) )

{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_DET_FIRING_PERIODIC_WITHIN_INTERVAL );
	( l_pcRoot->info ).Function3ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
		l_pcRoot->middle = 0;
		l_pcRoot->right = 0;

	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("FixedTimedFiring_Periodic: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->middle = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("FixedTimedFiring_Periodic: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->right = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("FixedTimedFiring_Periodic: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if(
	   ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ||
	   ( l_pcRoot->middle == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}

SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForMove2DGridOperator( wxChar* p_sFormula,
														double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) )

{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER );
	( l_pcRoot->info ).Function3ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
		l_pcRoot->middle = 0;
		l_pcRoot->right = 0;

	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Move2DGrid: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->middle = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Move2DGrid: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	for(unsigned i = 0; i < 3; ++i)
	{
		if( l_nTokenizer.HasMoreTokens() )
		{
			l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
			l_pcRoot->right = Parse( l_sParam );
			wxDELETEA(l_sParam);
		}
		else
		{
			m_asMissingVariableList.Add( wxString( wxT("Move2DGrid: Missing Parameter") ) );
			FreeTree(l_pcRoot);
			return NULL;
		}
	}

	if(( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ||
	   ( l_pcRoot->middle == NULL ) )
	{
		FreeTree(l_pcRoot);
		return NULL;
	}
	else
	{
		return l_pcRoot;
	}

}

SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForCountXYOperator( wxChar* p_sFormula,
														double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) )

{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER );
	( l_pcRoot->info ).Function3ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
		l_pcRoot->middle = 0;
		l_pcRoot->right = 0;

	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("CountXY: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {

		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->middle = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("CountXY: Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	for(unsigned i = 0; i < 4; ++i)
	{
		if( l_nTokenizer.HasMoreTokens() )
		{
			l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
			l_pcRoot->right = Parse( l_sParam );
			wxDELETEA(l_sParam);
		}
		else
		{
			m_asMissingVariableList.Add( wxString( wxT("CountXY: Missing Parameter") ) );
			FreeTree(l_pcRoot);
			return NULL;
		}
	}

	if(( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ||
	   ( l_pcRoot->middle == NULL ) )
	{
		FreeTree(l_pcRoot);
		return NULL;
	}
	else
	{
		return l_pcRoot;
	}

}

SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForPrefixOperator( wxChar* p_sFormula, double ( SP_DS_StParser::*op )( double ) )
{

	SP_DS_StParserNode* l_pcRoot;
	wxChar* p = wxStrchr( p_sFormula, wxT('(') );

	if( wxStrstr( p_sFormula, wxT("Summation") ) == p_sFormula ||
		wxStrstr( p_sFormula, wxT("Product") ) == p_sFormula)
	{
		wxChar l_chSign = (wxStrstr( p_sFormula, wxT("Product") ) == p_sFormula) ? wxT('*') : wxT('+');

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_PREFIX );
		(l_pcRoot->info).PrefixOperator = op;

		wxString l_sFormula(p);
		if(l_sFormula.Find(wxT("_PrePlaces")) != wxNOT_FOUND)
		{
			SP_MapString2Long* l_pmPrePlaceArrayPos = GetPrePlaceMapOfNode(m_nCurrentTransitionNumber);
			SP_MapString2Long::iterator l_iter = l_pmPrePlaceArrayPos->begin();
			if(l_iter != l_pmPrePlaceArrayPos->end())
			{
				wxString l_sNewFormula = l_sFormula;
				l_sNewFormula.Replace(wxT("_PrePlaces"), l_iter->first);
				l_iter++;
				for(;	l_iter != l_pmPrePlaceArrayPos->end(); l_iter++ )
				{
					l_sNewFormula << l_chSign << l_sFormula;
					l_sNewFormula.Replace(wxT("_PrePlaces"), l_iter->first);
				}
				l_sFormula = l_sNewFormula;
			}
			else
			{
				l_sFormula.Replace(wxT("_PrePlaces"), wxT("1"));
			}
		}
		wxChar* l_sParam = ConvertToCharArray(l_sFormula);
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
		if( l_pcRoot->left == NULL )
		{
			FreeTree(l_pcRoot);
			return NULL;
		}
		else
		{
			return l_pcRoot;
		}
	}

	if(wxStrstr( p_sFormula, wxT("Multiplicity") ) == p_sFormula)
	{
		l_pcRoot = Parse(p);
		if(l_pcRoot && l_pcRoot->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE)
		{
			wxString l_sPlace(++p);
			l_pcRoot->type = SP_ST_PARSER_NODE_TYPE_CONSTANT;
			if( ! m_bOnlyCheck )
			{
				l_pcRoot->info.constD = m_aanPreIncMatrix->GetValue(m_nCurrentTransitionNumber, ( *m_mnPlaceArrayPos )[ l_sPlace ]).first;
			}
		}
		else
		{
			FreeTree(l_pcRoot);
		}
		return l_pcRoot;
	}

	if( wxStrstr( p_sFormula, wxT("FixedTimedFiring_Single") ) == p_sFormula )
	{
		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_DET_FIRING_SINGLE );
	}
	else if( wxStrstr( p_sFormula, wxT("ImmediateFiring") ) == p_sFormula )
	{
		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_IMMEDIATE_TRANSITION );
	}
	else if( wxStrstr( p_sFormula, wxT("TimedFiring") ) == p_sFormula )
	{
		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_TIMED_TRANSITION );
	}
	else
	{
		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_PREFIX );
	}

	( l_pcRoot->info ).PrefixOperator = op;
	l_pcRoot->left = Parse( p );

	if( l_pcRoot->left == NULL )
	{
		FreeTree(l_pcRoot);
		return NULL;
	}
	else
	{
		return l_pcRoot;
	}
}

SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForMassActionOperator( wxChar* p_sFormula, double ( SP_DS_StParser::*op )( double ) )
{

	SP_DS_StParserNode* l_pcRoot;
	SP_DS_StParserNode* l_pcChild;

	wxChar* p;


	p = wxStrchr( p_sFormula, wxT('(') );

	l_pcChild = Parse( p );

	if( m_lnBasedPlaces.size() == 0 )  {

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_CONSTANT_PARAMETER );

	}
	else {

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_DYNAMIC_PARAMETER );

	}


	( l_pcRoot->info ).PrefixOperator = op;
	l_pcRoot->left = l_pcChild;

	SP_MapString2Long :: iterator l_iter;

	for( l_iter = GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->begin();
		l_iter != GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->end(); l_iter++ ) 
	{

		m_lnBasedPlaces.insert( l_iter->second );


	}

	if( l_pcRoot->left == NULL ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}

SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForLevelInterpretationOperator( wxChar* p_sFormula, double ( SP_DS_StParser::*p_pfOperator )( double, double ) )
{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_BIO_LEVEL_INTERPRETATION );
	( l_pcRoot->info ).Function2ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->right = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}


	SP_MapString2Long :: iterator l_iter;

	for( l_iter = GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->begin();
		l_iter != GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->end(); l_iter++ ) {

		m_lnBasedPlaces.insert( l_iter->second );

	}


	if(
	   ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}

SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForMichaelisMentenOperator( wxChar* p_sFormula, double ( SP_DS_StParser::*p_pfOperator )( double, double ) )
{

	SP_DS_StParserNode* l_pcRoot;

	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_BIO_MICHAELIS_MENTEN );
	( l_pcRoot->info ).Function2ParamOperator = p_pfOperator;

	wxChar* l_sParam;

	wxStringTokenizer l_nTokenizer( p_sFormula, wxT(",") );

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->left = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}

	if( l_nTokenizer.HasMoreTokens() ) {
		l_sParam = ConvertToCharArray( l_nTokenizer.GetNextToken() );
		l_pcRoot->right = Parse( l_sParam );
		wxDELETEA(l_sParam);
	}
	else {

		m_asMissingVariableList.Add( wxString( wxT("Missing Parameter") ) );
		FreeTree(l_pcRoot);
		return NULL;

	}


	SP_MapString2Long :: iterator l_iter;

	for( l_iter = GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->begin();
		l_iter != GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->end(); l_iter++ ) {

		m_lnBasedPlaces.insert( l_iter->second );

	}


	if(
	   ( l_pcRoot->left == NULL ) ||
	   ( l_pcRoot->right == NULL ) ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}

SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForLookupTable( wxChar* p_sFormula, const wxString& p_sTableName )
{

	SP_DS_StParserNode* l_pcRoot;
	wxChar* p;

	p = wxStrchr( p_sFormula, wxT('(') );
	l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_LOOKUP_TABLE );

	( l_pcRoot->info ).table_key = p_sTableName.c_str();
	l_pcRoot->left = Parse( p );

	if( l_pcRoot->left == NULL ) {

		FreeTree(l_pcRoot);
		return NULL;

	}
	else {

		return l_pcRoot;

	}

}


SP_DS_StParserNode*
SP_DS_StParser::CreateParserNodeForVariable( wxChar* p_sFormula )
{

	SP_DS_StParserNode* l_pcNode;
	l_pcNode = CreateParserNode( SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE );

	AssignNodeToVariable( p_sFormula, l_pcNode );

	if( l_pcNode->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_ERROR ) {

		FreeTree(l_pcNode);
		return NULL;

	}
	else {

		return l_pcNode;

	}

}


SP_DS_StParserNode*
SP_DS_StParser::CreateParserNode( SP_StParserNodeType p_nType )
{

	SP_DS_StParserNode* l_pcNode = new SP_DS_StParserNode();
	l_pcNode->type = p_nType;

	l_pcNode->left = 0;
	l_pcNode->middle = 0;
	l_pcNode->right = 0;

	return l_pcNode;

}


wxChar*
SP_DS_StParser::Scan( wxChar* p_sFormula, wxChar op, bool p_bMatchFirst )
{
	long l_nLevel = 0;
	wxChar* p = p_sFormula;
	wxChar* ret = NULL;

	while( *p )
	{
		if( *p == wxT('(') )
		{
			l_nLevel++;
		}
		else if ( *p == wxT(')') )
		{
			l_nLevel--;
		}
		else if( ( *p == op ) &&
				( l_nLevel == 0 ) &&
				( p != p_sFormula ) )
		{
			if(ret == NULL || !p_bMatchFirst)
			{
				ret = p;
			}
		}
		p++;
	}

	return ret;

}


void
SP_DS_StParser::FreeTree( SP_DS_StParserNode* p_pcTree )
{
	if(p_pcTree)
	{
		if(p_pcTree->left)
			FreeTree(p_pcTree->left);

		if(p_pcTree->middle)
			FreeTree(p_pcTree->middle);

		if(p_pcTree->right)
			FreeTree(p_pcTree->right);

		wxDELETE(p_pcTree);
	}
}



double
SP_DS_StParser::Eval( SP_DS_StParserNode* p_pcTree )
{

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_INFIX)
			return ( this->*( p_pcTree->info ).InfixOperator )( Eval( p_pcTree->left ),
																Eval( p_pcTree->right ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_PREFIX)
			return ( this->*( p_pcTree->info ).PrefixOperator )( Eval( p_pcTree->left ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_FUNCTION_0_PARAMETER)
			return ( this->*( p_pcTree->info ).Function0ParamOperator )();
	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_FUNCTION_2_PARAMETER)
			return ( this->*( p_pcTree->info ).Function2ParamOperator )(Eval( p_pcTree->left ),
																		Eval( p_pcTree->right ) );
	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER)
			return ( this->*( p_pcTree->info ).Function3ParamOperator )(Eval( p_pcTree->left ),
																		Eval( p_pcTree->middle ),
																		Eval( p_pcTree->right ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_CONSTANT_PARAMETER ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_DYNAMIC_PARAMETER )
			return CalcMassAction( Eval( p_pcTree->left ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_LEVEL_INTERPRETATION)
			return CalcLevelInterpretation( Eval( p_pcTree->left ), Eval( p_pcTree->right ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_MICHAELIS_MENTEN)
			return CalcMichaelisMenten( Eval( p_pcTree->left ), Eval( p_pcTree->right ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_LOOKUP_TABLE)
			return EvalLookupTable( ( p_pcTree->info ).table_key, Eval( p_pcTree->left ) );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_CONSTANT)
			return ( p_pcTree->info ).constD;

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE)
		return FindCurrentMarking(( p_pcTree->info ).constL);

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_START ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_END ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_TIME)
			return *( ( p_pcTree->info ).varD );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_DET_FIRING_SINGLE )
			return Eval( p_pcTree->left );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_IMMEDIATE_TRANSITION )
			return Eval( p_pcTree->left );

	return 0;
}


bool
SP_DS_StParser :: PreProcess( SP_DS_StParserNode* p_pcTree )
{

	double l_nConstantValue = 0;

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_INFIX )
			return PreProcess( p_pcTree->left ) && PreProcess( p_pcTree->right );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_PREFIX ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_LOOKUP_TABLE )
			return PreProcess( p_pcTree->left );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER )
			return PreProcess( p_pcTree->left ) && PreProcess( p_pcTree->middle ) && PreProcess( p_pcTree->right );

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_START ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_END ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_TIME ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_DYNAMIC_PARAMETER ||
		p_pcTree->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_CONSTANT_PARAMETER )
	{
		if( p_pcTree->left && ! PreProcess( p_pcTree->left ) )
		{

			l_nConstantValue = Eval( p_pcTree->left );
			FreeTree( p_pcTree->left );

			p_pcTree->left = CreateParserNode( SP_ST_PARSER_NODE_TYPE_CONSTANT );
			p_pcTree->left->info.constD = l_nConstantValue;
		}
		return true;
	}
	return false;
}


double
SP_DS_StParser::EvalLookupTable( const wxChar* p_sTableName, double p_nX )
{

	wxString l_sTableName( p_sTableName );

	if( m_mmLookupTablesMap[ l_sTableName ].find( wxString::Format( wxT("%f"), p_nX ) ) !=
	    m_mmLookupTablesMap[ l_sTableName ].end() ) {

		return m_mmLookupTablesMap[ l_sTableName ][ wxString::Format( wxT("%f"), p_nX ) ];

	}
	else {

		return m_mnLookupTablesDefaultValueMap[ l_sTableName ];

	}

	return 0;

}


double
SP_DS_StParser::CalcInfixPlus( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 + p_nArg2 );

}


double
SP_DS_StParser::CalcInfixMinus( double p_nArg1, double p_nArg2 )
{
	return ( p_nArg1 - p_nArg2 );
}


double
SP_DS_StParser::CalcInfixTimes( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 * p_nArg2 );

}


double
SP_DS_StParser::CalcInfixDivide( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 / p_nArg2 );

}


double
SP_DS_StParser::CalcInfixPower( double p_nArg1, double p_nArg2 )
{

	return pow( p_nArg1, p_nArg2 );

}


double
SP_DS_StParser::EvalOpnGt( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 > p_nArg2 );

}


double
SP_DS_StParser::EvalOpnLt( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 < p_nArg2 );

}


double
SP_DS_StParser::EvalOpnGeq( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 >= p_nArg2 );

}


double
SP_DS_StParser::EvalOpnLeq( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 <= p_nArg2 );

}


double
SP_DS_StParser::EvalOpnEq( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 == p_nArg2 );

}


double
SP_DS_StParser::EvalOpnNeq( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 != p_nArg2 );

}


double
SP_DS_StParser::EvalOpnAnd( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 && p_nArg2 );

}



double
SP_DS_StParser::EvalOpnOr( double p_nArg1, double p_nArg2 )
{

	return ( p_nArg1 || p_nArg2 );

}


double
SP_DS_StParser::CalcBinaryPower( double p_nArg1, double p_nArg2 )
{

	return pow( p_nArg1, p_nArg2 );

}

double
SP_DS_StParser::CalcBinaryMin( double p_nArg1, double p_nArg2 )
{

	return min( p_nArg1, p_nArg2 );

}

double
SP_DS_StParser::CalcBinaryMax( double p_nArg1, double p_nArg2 )
{
	return max( p_nArg1, p_nArg2 );

}

double
SP_DS_StParser::CalcPrefixMinus( double p_nArg )
{

	return( -p_nArg );

}


double
SP_DS_StParser::CalcPrefixAbs( double p_nArg ) {

	if( p_nArg < 0 ) {

		return -p_nArg;

	}
	else {

		return p_nArg;

	}

}

double
SP_DS_StParser::CalcPrefixAcos( double p_nArg )
{

	return acos( p_nArg );

}


double
SP_DS_StParser::CalcPrefixAsin( double p_nArg )
{

	return asin( p_nArg );

}


double
SP_DS_StParser::CalcPrefixAtan( double p_nArg )
{

	return atan( p_nArg );

}


double
SP_DS_StParser::CalcPrefixCeil( double p_nArg )
{

	return ceil( p_nArg );

}


double
SP_DS_StParser::CalcPrefixCos( double p_nArg )
{

	return cos( p_nArg );

}


double
SP_DS_StParser::CalcPrefixExp( double p_nArg )
{

	return exp( p_nArg );

}


double
SP_DS_StParser::CalcPrefixFloor( double p_nArg )
{

	return floor( p_nArg );

}


double
SP_DS_StParser::CalcPrefixLog( double p_nArg )
{

	return log( p_nArg );

}


double
SP_DS_StParser::CalcPrefixLog10( double p_nArg )
{

	return log10( p_nArg );

}


double
SP_DS_StParser::CalcPrefixSin( double p_nArg )
{

	return sin( p_nArg );

}


double
SP_DS_StParser::CalcPrefixSqr( double p_nArg ) {

	return ( p_nArg * p_nArg );

}


double
SP_DS_StParser::CalcPrefixSqrt( double p_nArg )
{

	return sqrt( p_nArg );

}


double
SP_DS_StParser::CalcPrefixTan( double p_nArg )
{

	return tan( p_nArg );

}


double
SP_DS_StParser::CalcPrefixNot( double p_nArg )
{

	return ( p_nArg == 0 ? 1 : 0 );

}

double
SP_DS_StParser :: CalcLigandBindingPotential( double p_nArg0, double p_nArg1)
{
	double l_nResult = 0;
	for(int i = 0; i <= p_nArg0; i++)
	{
		l_nResult += CalcBinomialCoefficient(p_nArg0, i) * pow(p_nArg1, i);
	}
	return l_nResult;
}

double
SP_DS_StParser :: CalcMassAction( double p_nArg )
{
	double l_nResult = p_nArg;
	for(unsigned long i = 0; i < m_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlaceArrayPos = m_pcSimTransition->basedPlacePos[i];
		l_nResult *= CalcBinomialCoefficient(FindCurrentMarking(l_nPlaceArrayPos),
											m_pcSimTransition->intervalLowerBound[i]);
	}
	return l_nResult;

}

double
SP_DS_StParser :: CalcLevelInterpretation( double p_nArg0, double p_nArg1 )
{
	double l_nResult = p_nArg0 * p_nArg1;
	for(unsigned long i = 0; i < m_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlaceArrayPos = m_pcSimTransition->basedPlacePos[i];
		l_nResult *= (FindCurrentMarking(l_nPlaceArrayPos) / p_nArg1);
	}
	return l_nResult;

}

double SP_DS_StParser ::CalcMichaelisMenten( double p_nArg0, double p_nArg1 )
{
	double l_nResult = 1;
	for(unsigned long i = 0; i < m_pcSimTransition->basedPlacePos.size(); i++)
	{
		long l_nPlaceArrayPos = m_pcSimTransition->basedPlacePos[i];
		double l_nPlaceMarking = FindCurrentMarking(l_nPlaceArrayPos);
		double l_nMultiplicity = m_pcSimTransition->intervalLowerBound[i];

		l_nResult *= pow( l_nPlaceMarking, l_nMultiplicity );		
	}

	l_nResult = p_nArg0 * l_nResult / (p_nArg1 + l_nResult);
	
	return l_nResult;
}


double
SP_DS_StParser::CalcPrefixDTT( double p_nArg )
{

	return log10( p_nArg );

}



void
SP_DS_StParser :: AssignNodeToVariable( wxChar* p_sVariable, SP_DS_StParserNode* p_pcNode )
{
	if( m_mnParameter->find( p_sVariable ) != m_mnParameter->end() )
	{
		p_pcNode->type = SP_ST_PARSER_NODE_TYPE_CONSTANT;
		( p_pcNode->info ).constD = ( *m_mnParameter )[ p_sVariable ];
		return;
	}

	if( wxStrcmp( p_sVariable, wxT("_SimTime") ) == 0 )
	{
		p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_TIME;
		( p_pcNode->info ).varD = m_nSimTime;
		return;
	}


	if( wxStrcmp( p_sVariable, wxT("_SimStart") ) == 0 )
	{
		p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_START;
		( p_pcNode->info ).varD = &m_nSimIntervalStart;
		return;
	}

	if( wxStrcmp( p_sVariable, wxT("_SimEnd") ) == 0 )
	{
		p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_END;
		( p_pcNode->info ).varD = &m_nSimIntervalEnd;
		return;
	}

	if( m_bOnlyPrePlaces )
	{
		//Deal with colors
		wxString l_sNetClassName = m_pcGraph->GetNetclass()->GetName();
		//now we only use it for syntax checking
		if( (!m_bColoredPN) && (l_sNetClassName == SP_DS_COLSPN_CLASS ))
		{
			//for the special case of rate function definition like P1[c1]*p1[c2]
			if(m_mnPlaceArrayPos->find(p_sVariable) != m_mnPlaceArrayPos->end())
			{
				p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE;
				m_lnBasedPlaces.insert( ( *m_mnPlaceArrayPos )[ p_sVariable ] );

				if( ! m_bOnlyCheck )
				{
					( p_pcNode->info ).constL = ( *m_mnPlaceArrayPos )[ p_sVariable ];
				}
				return;				
			}
		}

		else
		{

			if( GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->find( p_sVariable ) !=
			   GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->end() )
			{
				p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE;
				m_lnBasedPlaces.insert( ( *m_mnPlaceArrayPos )[ p_sVariable ] );

				m_vsPrePlaceInFunction.push_back(p_sVariable);

				if( ! m_bOnlyCheck )
				{
					( p_pcNode->info ).constL = ( *m_mnPlaceArrayPos )[ p_sVariable ];
				}
				return;
			}
		}
	}
	else if( m_mnPlaceArrayPos->find( p_sVariable ) != m_mnPlaceArrayPos->end() )
	{
		p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE;
		m_lnBasedPlaces.insert( ( *m_mnPlaceArrayPos )[ p_sVariable ] );

		if( ! m_bOnlyCheck )
		{
			( p_pcNode->info ).constL = ( *m_mnPlaceArrayPos )[ p_sVariable ];
		}
		return;
	}

	m_asMissingVariableList.Add( wxString( p_sVariable ) );
	p_pcNode->type = SP_ST_PARSER_NODE_TYPE_VARIABLE_ERROR;
	return;
}

void
SP_DS_StParser::LoadLookupTables() {

	m_mmLookupTablesMap.clear();
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass( wxT("LookupTable") );
    if(!l_pcNodeclass)
    	return;   //this class does not support look up Table
	SP_ListNode::const_iterator l_itElem;
	wxString l_sLookupTableName;

	for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{

		l_sLookupTableName = wxString(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());

		l_sLookupTableName += wxT("(");

		m_mmLookupTablesMap[ l_sLookupTableName ] = ConvertToLookupTable(
															dynamic_cast< SP_DS_ColListAttribute* > ((*l_itElem)->GetAttribute( wxT("MapList") ) ) );

		m_mnLookupTablesDefaultValueMap[ l_sLookupTableName ] = dynamic_cast< SP_DS_DoubleAttribute* > (
																	(*l_itElem)->GetAttribute( wxT("DefaultValue") ) )->GetValue();
	}
}


SP_MapString2Double
SP_DS_StParser::ConvertToLookupTable( SP_DS_ColListAttribute* p_pcColList ) {

	SP_MapString2Double l_pmMap;

	double l_nX;
	double l_nFx;

	for(unsigned long i = 0; i < p_pcColList->GetRowCount(); i++ )
	{
		wxString l_sCell0 = wxString(p_pcColList->GetCell(i, 0).c_str());
		l_sCell0.ToDouble( &l_nX );
		wxString l_sCell1 = wxString(p_pcColList->GetCell(i, 1).c_str());
		l_sCell1.ToDouble( &l_nFx );

		l_pmMap[ wxString::Format( wxT("%f"), l_nX ) ] = l_nFx;
	}

	return l_pmMap;
}


SP_VectorLong
SP_DS_StParser :: GetBasedPlaces()
{

	for(SP_MapString2Long::iterator l_iter = GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->begin();
		l_iter != GetPrePlaceMapOfNode(m_nCurrentTransitionNumber)->end(); l_iter++ )
	{
		m_lnBasedPlaces.insert(l_iter->second);
	}

	SP_VectorLong l_anPlaces;

	for(SP_SetLong::iterator l_iter = m_lnBasedPlaces.begin();
		l_iter != m_lnBasedPlaces.end(); l_iter++ )
	{
		l_anPlaces.push_back( *l_iter );
	}

	return l_anPlaces;

}


bool
SP_DS_StParser :: CheckFormulaFunction( const wxString& p_sFormula, SP_DS_Node* p_pcTransitionNode, bool p_bColoredPN  )
{

	wxString l_sConsistenceCheck = p_sFormula;
	l_sConsistenceCheck.Replace(wxT(" "), wxT(""));
	l_sConsistenceCheck.Replace(wxT("\n"), wxT(""));
	l_sConsistenceCheck.Replace(wxT("\t"), wxT(""));

	SP_DS_FunctionRegistry* l_pcFR = p_pcTransitionNode->GetNodeclass()->GetParentGraph()->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction = l_pcFR->parseFunctionString(l_sConsistenceCheck);
    if(l_pcFunction) {
        SP_FunctionPtr l_pcExpanded = l_pcFR->substituteFunctions(l_pcFunction);
        l_sConsistenceCheck = wxString(l_pcExpanded->toString());
    }
	m_bOnlyCheck = true;

	m_bColoredPN = p_bColoredPN;

	m_bParseErrorParantheses = false;
	m_bFirstOperatorDone = false;

	m_asMissingVariableList.clear();
	m_lnBasedPlaces.clear();

	m_vsPrePlaceInFunction.clear();

	m_eParserType = SP_ST_PARSER_TYPE_CHECK_FUNCTION;
	m_pcCurrentTransitionNode = p_pcTransitionNode;

	LoadPrePlaces();

	m_bOnlyPrePlaces = true;

	wxChar* l_pchFormula = ConvertToCharArray( l_sConsistenceCheck );
	SP_DS_StParserNode* l_pcRoot = CreateTree( l_pchFormula );

	m_bModifierPlaceAsFactor = ModifierPlaceAsFactor(l_sConsistenceCheck);

	
	wxDELETEA(l_pchFormula);
	FreeTree(l_pcRoot);
	bool l_bISExp = false;//by george for vlaues having -e 
	if (m_asMissingVariableList.size() > 0)
	{
		if (m_asMissingVariableList[0] == wxT("1e"))
		{
			l_bISExp = true;
		}
	}
	if ( ( m_bParseErrorParantheses ) || ( m_asMissingVariableList.size() > 0 ) && !l_bISExp)
	{
		return false;
	}
	else
	{
		CheckConsistence(p_pcTransitionNode,l_sConsistenceCheck);
		return true;
	}
}


bool SP_DS_StParser::CheckConsistence(SP_DS_Node* p_pcTransitionNode, wxString p_sFormula)
{
	//The principles are: 
	//1) If a rate function is constant, then we only check unused places connected by modifier arcs, 
	//2) If a rate function contains places, then we check all unused places.

	//SP_LOGMESSAGE(wxT("Consistence Checking:"));

	wxString l_sTransName = wxString(dynamic_cast<SP_DS_NameAttribute*>(p_pcTransitionNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());
	
	m_asUnusedModifierPlaceList.clear();
	m_asUnusedPrePlaceList.clear();

	vector<wxString> l_vsModifierPlaces;
	vector<wxString> l_vsIllegalPrePlaces;
	vector<wxString> l_vsOtherPlaces;
	SP_ListEdge::const_iterator l_itEdges;
	for (l_itEdges = p_pcTransitionNode->GetTargetEdges()->begin(); l_itEdges != p_pcTransitionNode->GetTargetEdges()->end(); ++l_itEdges)
	{
		SP_DS_Edge* l_pcEdge = (*l_itEdges);
		if ((*l_itEdges)->GetSource())
		{
			SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*> ((*l_itEdges)->GetSource());

			wxString l_sPlaceName = wxString(dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());
			wxString l_sEdgeClassName = wxString(l_pcEdge->GetClassName().c_str());
			if( m_vsPrePlaceInFunction.size() == 0 )
			{
				if( l_sEdgeClassName ==  wxT("Modifier Edge") )
				{
					l_vsModifierPlaces.push_back(l_sPlaceName);
					m_asUnusedModifierPlaceList.Add(l_sPlaceName);
				}
			}
			else
			{
				if( l_sEdgeClassName ==  wxT("Modifier Edge") )
				{
					bool l_bFound = false;
					for(unsigned i = 0; i < m_vsPrePlaceInFunction.size(); i++ )
					{
						if( l_sPlaceName == m_vsPrePlaceInFunction[i])
							l_bFound = true;
					}
					if(!l_bFound)
					{
						l_vsModifierPlaces.push_back(l_sPlaceName);
						m_asUnusedModifierPlaceList.Add(l_sPlaceName);
					}
				}
				else if( p_sFormula.Find(wxT("MassAction(")) != wxNOT_FOUND 
					|| p_sFormula.Find(wxT("LevelInterpretation(")) != wxNOT_FOUND
					|| p_sFormula.Find(wxT("MichaelisMenten(")) != wxNOT_FOUND)
				{
					//do nothing
					bool l_bFound = false;
					for(unsigned i = 0; i < m_vsPrePlaceInFunction.size(); i++ )
					{
						if( l_sPlaceName == m_vsPrePlaceInFunction[i])
							l_bFound = true;
					}
					if(l_bFound)
					{
						l_vsIllegalPrePlaces.push_back(l_sPlaceName);						
					}
				}
				else
				{
					bool l_bFound = false;
					for(unsigned i = 0; i < m_vsPrePlaceInFunction.size(); i++ )
					{
						if( l_sPlaceName == m_vsPrePlaceInFunction[i])
							l_bFound = true;
					}
					if(!l_bFound)
					{
						l_vsOtherPlaces.push_back(l_sPlaceName);
						m_asUnusedPrePlaceList.Add(l_sPlaceName);
					}
				}
			}
		}
	}

	wxString l_sError = wxT("");
	if( l_vsModifierPlaces.size() > 0 )
	{
		for(unsigned i = 0; i < l_vsModifierPlaces.size(); i++ )
		{
			l_sError = l_sError + wxT(" ") + l_vsModifierPlaces[i];
		}
		l_sError = wxT("The rate function for ") + l_sTransName + wxT(" has unused modifier pre-places: ") + l_sError;
		SP_LOGWARNING(l_sError);					
	}

	l_sError = wxT("");
	if( l_vsOtherPlaces.size() > 0 )
	{
		for(unsigned i = 0; i < l_vsOtherPlaces.size(); i++ )
		{
			l_sError = l_sError + wxT(" ") + l_vsOtherPlaces[i];
		}
		l_sError = wxT("The rate function for ") + l_sTransName + wxT(" has unused pre-places: ") + l_sError;
		SP_LOGWARNING(l_sError);					
	}

	l_sError = wxT("");
	if( l_vsIllegalPrePlaces.size() > 0 )
	{
		for(unsigned i = 0; i < l_vsIllegalPrePlaces.size(); i++ )
		{
			l_sError = l_sError + wxT(" ") + l_vsIllegalPrePlaces[i];
		}
		l_sError = wxT("The rate function for ") + l_sTransName + wxT(" has illegal preplaces as parameters of MassAction(): ") + l_sError;
		SP_LOGWARNING(l_sError);					
	}
	

	return true;
}

bool SP_DS_StParser::ModifierPlaceAsFactor( wxString p_sFormula )
{	
	//warning if a modifier arc can be replaced by a read arc
	
	return true;
}

bool
SP_DS_StParser :: ParseFormulaFunction( wxString p_sFormula, long p_nTransitionNumber, SP_DS_StParserNode*& p_pcRoot,
									   SP_DS_StSimHazardFunctionType* p_peHazardFunctionType )
{

	m_bOnlyCheck = false;

	m_bParseErrorParantheses = false;
	m_bFirstOperatorDone = false;

	m_asMissingVariableList.clear();
	m_lnBasedPlaces.clear();

	m_eParserType = SP_ST_PARSER_TYPE_PARSE_FUNCTION;
	m_nCurrentTransitionNumber = p_nTransitionNumber;

	m_bOnlyPrePlaces = true;

	wxChar* l_pchFormula = ConvertToCharArray( p_sFormula );
	p_pcRoot = CreateTree( l_pchFormula );
	wxDELETEA(l_pchFormula);

	if ( ( m_bParseErrorParantheses ) || ( m_asMissingVariableList.size() > 0 ) )
	{
		FreeTree(p_pcRoot);
		return false;
	}
	else
	{
		SetHazardFunctionType( p_pcRoot, p_peHazardFunctionType );
		return true;
	}

}

wxChar*
SP_DS_StParser :: ConvertToCharArray( const wxString& p_sFormula )
{
	wxChar* l_sNew = new wxChar[ p_sFormula.Length() + 1 ];

	wxStrcpy(l_sNew, p_sFormula.c_str());

	return l_sNew;

}


void
SP_DS_StParser :: LoadParameter()
{
	m_mnParameter->clear();
	
	if(m_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		/**
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( SP_DS_PARAM );
		SP_ListNode::const_iterator l_itElem;

		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem )
		{
			wxString l_sParameterName = wxString(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( (*l_itElem)->GetAttribute( wxT("ParameterList") ) );
			double l_nDouble = l_pcColList->GetActiveCellValueDouble(1);

			( *m_mnParameter )[ l_sParameterName ] = l_nDouble;
		}
		**/
		///
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_ListMetadata::const_iterator it;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcConstant = *it;
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

			SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();


			SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
			if (l_FE.IsOk())
			{
				SP_FunctionPtr l_Function = l_FE.getFunction();

				double l_nValue = 0.0;


				if (l_Function->isValue())
				{
					if (l_sType == wxT("int"))
					{
						l_nValue = (int)l_Function->getValue();
						 
					}
					else if (l_sType == wxT("double"))
					{
						l_nValue = l_Function->getValue();
						 
					}
					//wxString l_sConstVal;
					//l_sConstVal << l_nValue;
					 
				}
				else
				{
					//evaluate string
					wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
					if (l_sType == wxT("int"))
					{
						l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();
						//wxString l_sConstVal;
						//l_sConstVal << l_nValue;


					}
					else if (l_sType == wxT("double"))
					{
						l_nValue = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
					}
					//l_pcFR->registerFunction(l_sName, to_string(l_nValue));
					
				}
				(*m_mnParameter)[l_sName] = (double)l_nValue;
			}
		}
		////
	}
	else
	{
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		SP_ListMetadata::const_iterator it;
		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it) {

				SP_DS_Metadata* l_pcMetadata = *it;
				wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
				double l_nDouble = l_pcColList->GetActiveCellValueDouble(1);

				( *m_mnParameter )[ l_sMetadataName ] = l_nDouble;
		}
	}
}


void
SP_DS_StParser :: LoadPlaces()
{
	//Load Continuous Places
	LoadPlaceOfType(SP_DS_CONTINUOUS_PLACE);

	//Load Discrete Places
	LoadPlaceOfType(SP_DS_DISCRETE_PLACE);
}

void SP_DS_StParser::LoadPlaceOfType(const wxString & p_sPlaceType)
{

	 SP_DS_Nodeclass* l_pcNodeclass;
	 l_pcNodeclass= m_pcGraph->GetNodeclass( p_sPlaceType);

	 if(l_pcNodeclass!=NULL) // Is there places of such type?
	 {
		 //To remember where did we stop
		 long l_nPos = m_nPlaceCount;

		 m_nPlaceCount = l_pcNodeclass->GetElements()->size();
		 SP_ListNode::const_iterator l_itElem;
		 wxString l_sPlaceName;

		 //Get the elements of that type
		 for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		 {
			l_sPlaceName = wxString(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());

			( *m_mnPlaceArrayPos )[ l_sPlaceName ] = l_nPos;

			l_nPos++;
		 }
	 }
}

void
SP_DS_StParser :: LoadPrePlaces()
{
	long l_nPos = 0;
	m_mmPrePlaceArrayPosOfTransitionNode.clear();

	LoadPrePlacesOfTransitionOfType(SP_DS_STOCHASTIC_TRANS,l_nPos);
	LoadPrePlacesOfTransitionOfType(SP_DS_CONTINUOUS_TRANS,l_nPos);
	LoadPrePlacesOfTransitionOfType(SP_DS_IMMEDIATE_TRANS,l_nPos);
	LoadPrePlacesOfTransitionOfType(SP_DS_DETERMINISTIC_TRANS,l_nPos);
	LoadPrePlacesOfTransitionOfType(SP_DS_SCHEDULED_TRANS,l_nPos);

}


void SP_DS_StParser::LoadPrePlacesOfTransitionOfType(const wxString & p_sTransType,long& p_nPos)
{
	SP_ListNode::const_iterator l_itElem;
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(p_sTransType);

	//Is there a transition of such type?
	if(l_pcNodeclass!=NULL)
	{
		for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
				++l_itElem )
		{
			SP_DS_Node* l_pcTransitionNode = (*l_itElem);
			if(l_pcTransitionNode == m_pcCurrentTransitionNode)
			{
				m_nCurrentTransitionNumber = p_nPos;
			}
			SP_MapString2Long* l_pmNew = GetPrePlaceMapOfNode(p_nPos);
			SP_ListEdge::const_iterator l_itEdge;
			for ( l_itEdge = l_pcTransitionNode->GetTargetEdges()->begin();
				 l_itEdge != l_pcTransitionNode->GetTargetEdges()->end(); ++l_itEdge )
			{
				SP_DS_Node* l_pcSourceNode	= dynamic_cast< SP_DS_Node* >( ( *l_itEdge )->GetSource() );

				if( l_pcSourceNode == NULL )
				{
					break;
				}

				wxString l_sPlaceName = wxString(dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue().c_str());

				long l_nPlaceArrayPos = (*m_mnPlaceArrayPos )[l_sPlaceName];
				(*l_pmNew)[l_sPlaceName] = l_nPlaceArrayPos;
			}
			p_nPos++;
		}
	}

}
wxString
SP_DS_StParser :: GetFormulaString( SP_DS_StParserNode* p_pcTree )
{

	if( p_pcTree == NULL )
	{
		return wxT("");
	}

	PrefixOperator* l_pcPrefixOperator;
	InfixOperator* l_pcInfixOperator;

	BinaryOperator* l_pcBinaryOperator;
	wxString l_sFormula = wxT("");

	if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_INFIX)
	{
		l_pcInfixOperator = m_pcInfixOperatorList;

		while( l_pcInfixOperator->ptr )
		{
			if( ( l_pcInfixOperator->ptr ) == ( p_pcTree->info ).InfixOperator )
			{
				l_sFormula += wxString :: Format( wxT("(%c "), l_pcInfixOperator->chr );
				l_sFormula += GetFormulaString( p_pcTree->left );

				l_sFormula += wxT(" ");

				l_sFormula += GetFormulaString( p_pcTree->right );
				l_sFormula += wxT(")\n");

				return l_sFormula;
			}

			l_pcInfixOperator++;
		}

		l_pcBinaryOperator = m_pcBinaryOperatorList;

		while( l_pcBinaryOperator->ptr )
		{
			if( ( l_pcBinaryOperator->ptr ) == ( p_pcTree->info ).InfixOperator )
			{
				l_sFormula += wxString :: Format( wxT("(%s "), l_pcBinaryOperator->string );

				l_sFormula += GetFormulaString( p_pcTree->left );
				l_sFormula += wxT(" ");

				l_sFormula += GetFormulaString( p_pcTree->right );
				l_sFormula += wxT(")\n");

				return l_sFormula;
			}

			l_pcBinaryOperator++;
		}
	}
	else if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_PREFIX)
	{
			l_pcPrefixOperator=m_pcPrefixOperatorList;

			while( l_pcPrefixOperator->ptr )
			{
				if( ( l_pcPrefixOperator->ptr ) == ( p_pcTree->info ).PrefixOperator )
				{
					l_sFormula += wxString :: Format( wxT("(%s "),l_pcPrefixOperator->string );

					l_sFormula += GetFormulaString( p_pcTree->left );
					l_sFormula += wxT(") ");

					return l_sFormula;
				}

				l_pcPrefixOperator++;
			}
	}
	else if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_CONSTANT)
	{
		l_sFormula  << ( p_pcTree->info ).constD;
		return l_sFormula;
	}
	else if( p_pcTree->type == SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE)
	{
			l_sFormula << (*m_anCurrentMarking)[ ( p_pcTree->info ).constL ];
			return l_sFormula;
	}
	else
	{
			l_sFormula << wxT("Unknown l_pcNode p_nType: ") << p_pcTree->type << wxT("\n");
	}

	return l_sFormula;

}

double
SP_DS_StParser :: CalcBinomialCoefficient( double p_nN, double p_nK )
{
	if(p_nN < 0 || p_nK < 0 || p_nN < p_nK)
	{
		return 0;
	}
	if(p_nK == 0 || p_nN == p_nK)
	{
		return 1;
	}
	if(p_nK == 1)
	{
		return p_nN;
	}
	if(p_nK*2 > p_nN)
	{
		p_nK = p_nN - p_nK;
	}
	double l_nReturn = p_nN;
	for(unsigned long i = 2; i <= p_nK; i++)
	{
		l_nReturn = l_nReturn * (p_nN + 1 - i);
		l_nReturn = l_nReturn / i;
	}
	return l_nReturn;
}

double
SP_DS_StParser :: Evaluate(SP_DS_StParserNode* p_pcTree, SP_DS_StSimTransition* p_pcSimTransition)
{
	m_pcSimTransition = p_pcSimTransition;
	m_nCurrentTransitionNumber = m_pcSimTransition->arrayIndex;

	return Eval( p_pcTree );
}

double
SP_DS_StParser :: DummyFunction3Param( double p_nArg0, double p_nArg1, double p_nArg2 )
{
	return 0;
}

double
SP_DS_StParser :: DummyFunction1Param( double p_nArg0 )
{
	return p_nArg0;
}

double
SP_DS_StParser :: DummyFunction0Param()
{
	return 0;
}

void
SP_DS_StParser::SetPlaceArrayPos( SP_MapString2Long* p_mnPlaceArrayPos )
{
	if(!m_nPlaceArrayPosRefCount)
	{
		m_nPlaceArrayPosRefCount++;
		wxDELETE(m_mnPlaceArrayPos);
	}
	m_mnPlaceArrayPos = p_mnPlaceArrayPos;
}

void
SP_DS_StParser::SetTransitionArrayPos( SP_MapString2Long* p_mnTransitionArrayPos )
{
	m_mnTransitionArrayPos = p_mnTransitionArrayPos;
}

void
SP_DS_StParser :: SetHazardFunctionType( SP_DS_StParserNode* p_pcRoot,
										SP_DS_StSimHazardFunctionType* p_peHazardFunctionType )
{

	if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_CONSTANT_PARAMETER )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_DYNAMIC_PARAMETER )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_BIO_LEVEL_INTERPRETATION )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_BIO_MICHAELIS_MENTEN )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_DET_FIRING_SINGLE )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_DET_FIRING_PERIODIC_WITHIN_INTERVAL )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_IMMEDIATE_TRANSITION )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION;

	else if( p_pcRoot->type == SP_ST_PARSER_NODE_TYPE_TIMED_TRANSITION )

		*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION;

	else
	{
		if( m_lnBasedPlaces.size() == 0 )
		{
			*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_CONSTANT;
		}
		else
		{
			*p_peHazardFunctionType = SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC;
		}
	}

}


SP_DS_StParserNode*
SP_DS_StParser :: CreateParserNodeForFunction0ParamOperator( wxChar* p_sFormula,
															double ( SP_DS_StParser::*p_pfOperator )() )

{

	SP_DS_StParserNode* l_pcRoot;

	if( wxStrstr( p_sFormula, wxT("ImmediateFiring") ) == p_sFormula ) {

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_IMMEDIATE_TRANSITION );
		( l_pcRoot->info ).Function0ParamOperator = p_pfOperator;

		return l_pcRoot;

	}
	else {

		l_pcRoot = CreateParserNode( SP_ST_PARSER_NODE_TYPE_FUNCTION_0_PARAMETER );
		( l_pcRoot->info ).Function0ParamOperator = p_pfOperator;

	}

	return l_pcRoot;

}

void SP_DS_StParser::SetCurrentConcentration(const SP_VectorDouble* p_anCurrentConcentration)
{
	m_anCurrentConcentration=p_anCurrentConcentration;
	m_bUseConcentration=true;
}

SP_DS_StParser :: ~SP_DS_StParser()
{

	wxDELETE(m_mnParameter);
	if(!m_nPlaceArrayPosRefCount)
	{
		wxDELETE(m_mnPlaceArrayPos);
	}

	wxDELETEA(m_pcFunction0ParamOperatorList);
	wxDELETEA(m_pcInfixOperatorList);
	wxDELETEA(m_pcPrefixOperatorList);
	wxDELETEA(m_pcFunction2ParamOperatorList);
	wxDELETEA(m_pcFunction3ParamOperatorList);
	wxDELETEA(m_pcBinaryOperatorList);

}

