//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/03 09:35:00 $
// Short Description:
//////////////////////////////////////


#ifndef __SP_DS_STPARSER_H__
#define __SP_DS_STPARSER_H__

#include "sp_core/SP_List.h"

#include <wx/string.h>
#include <wx/arrstr.h>

#include <wx/tokenzr.h>

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_StParser;
class SP_DS_StSimTransition;

typedef enum {

  SP_ST_SIM_HAZARD_FUNCTION_DYNAMIC,
  SP_ST_SIM_HAZARD_FUNCTION_CONSTANT,

  SP_ST_SIM_HAZARD_FUNCTION_BMA_CONSTANT_PARAMETER,
  SP_ST_SIM_HAZARD_FUNCTION_BMA_DYNAMIC_PARAMETER,

  SP_ST_SIM_HAZARD_FUNCTION_BIO_LEVEL_INTERPRETATION,

  SP_ST_SIM_HAZARD_FUNCTION_BIO_MICHAELIS_MENTEN,

  SP_ST_SIM_HAZARD_FUNCTION_TIMED_TRANSITION,

  SP_ST_SIM_HAZARD_FUNCTION_DTF_SINGLE,
  SP_ST_SIM_HAZARD_FUNCTION_DTF_PERIODIC_WITHIN_INTERVAL,

  SP_ST_SIM_HAZARD_FUNCTION_IMMEDIATE_TRANSITION

} SP_DS_StSimHazardFunctionType;

typedef enum
{

  SP_ST_PARSER_TYPE_CHECK_FUNCTION,

  SP_ST_PARSER_TYPE_PARSE_FUNCTION,

} SP_StParserType;

typedef enum
{

  SP_ST_PARSER_NODE_TYPE_INFIX,
  SP_ST_PARSER_NODE_TYPE_PREFIX,

  SP_ST_PARSER_NODE_TYPE_FUNCTION_0_PARAMETER,
  SP_ST_PARSER_NODE_TYPE_FUNCTION_2_PARAMETER,

  SP_ST_PARSER_NODE_TYPE_FUNCTION_3_PARAMETER,

  SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_CONSTANT_PARAMETER,
  SP_ST_PARSER_NODE_TYPE_BIO_MASS_ACTION_DYNAMIC_PARAMETER,

  SP_ST_PARSER_NODE_TYPE_BIO_LEVEL_INTERPRETATION,

  SP_ST_PARSER_NODE_TYPE_BIO_MICHAELIS_MENTEN,

  SP_ST_PARSER_NODE_TYPE_DET_FIRING_SINGLE,

  SP_ST_PARSER_NODE_TYPE_DET_FIRING_PERIODIC_WITHIN_INTERVAL,

  SP_ST_PARSER_NODE_TYPE_IMMEDIATE_TRANSITION,

  SP_ST_PARSER_NODE_TYPE_TIMED_TRANSITION,

  SP_ST_PARSER_NODE_TYPE_LOOKUP_TABLE,
  SP_ST_PARSER_NODE_TYPE_CONSTANT,

  SP_ST_PARSER_NODE_TYPE_VARIABLE_PLACE,

  SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_TIME,
  SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_START,
  SP_ST_PARSER_NODE_TYPE_VARIABLE_SIM_END,

  SP_ST_PARSER_NODE_TYPE_VARIABLE_ERROR

} SP_StParserNodeType;

typedef enum
{

  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_UNVISIBLE,

  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_ARITHMETIC,
  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SIMPLE_ARITHMETIC,

  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_BOOLEAN,

  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_MATH_FUNC,

  SP_ST_PARSER_FUNCTION_VISIBLE_GROUP_SPECIAL_HAZARD

} SP_StParserFunctionVisibleGroup;

typedef union
{

  double ( SP_DS_StParser :: *Function3ParamOperator )( double, double, double );
  double ( SP_DS_StParser :: *Function2ParamOperator )( double, double );
  double ( SP_DS_StParser :: *InfixOperator )( double, double );
  double ( SP_DS_StParser :: *PrefixOperator )( double );
  double ( SP_DS_StParser :: *Function0ParamOperator )();

  const wxChar* table_key;

  double constD;
  double* varD;
  long constL;

} NodeInfo;

typedef struct _SP_DS_StParserNode
{

  SP_StParserNodeType type;
  NodeInfo info;

  struct _SP_DS_StParserNode* left;
  struct _SP_DS_StParserNode* right;
  struct _SP_DS_StParserNode* middle;

} SP_DS_StParserNode;

typedef struct
{

  wxChar chr;
  double ( SP_DS_StParser :: *ptr )( double, double );

  SP_StParserFunctionVisibleGroup visibleGroup;

} InfixOperator;


typedef struct
{

  const wxChar* string;
  const wxChar* match_string;
  double ( SP_DS_StParser::*ptr )( double );

  bool bOnlyFirstOperator;

  SP_StParserFunctionVisibleGroup visibleGroup;

} PrefixOperator;


typedef struct
{

  const wxChar* string;
  const wxChar* match_string;
  double ( SP_DS_StParser :: *ptr )( double, double, double );

  bool bOnlyFirstOperator;

  SP_StParserFunctionVisibleGroup visibleGroup;

} Function3ParamOperator;


typedef struct
{

  const wxChar* string;
  const wxChar* match_string;
  double ( SP_DS_StParser :: *ptr )( double, double );

  bool bOnlyFirstOperator;

  SP_StParserFunctionVisibleGroup visibleGroup;

} Function2ParamOperator;


typedef struct
{

  const wxChar* string;
  const wxChar* match_string;
  double ( SP_DS_StParser :: *ptr )();

  bool bOnlyFirstOperator;

  SP_StParserFunctionVisibleGroup visibleGroup;

} Function0ParamOperator;

/* binary prefix operators */
typedef struct
{

  const wxChar* string;
  const wxChar* match_string;
  double( SP_DS_StParser::*ptr )( double, double );

  bool bOnlyFirstOperator;

  SP_StParserFunctionVisibleGroup visibleGroup;

} BinaryOperator;


class SP_DS_StParser
{

  // data

  private :
	SP_DS_Graph* m_pcGraph;

    long m_nPlaceCount;

    SP_StParserType m_eParserType;

    bool m_bParseErrorParantheses;
    bool m_bOnlyCheck;

    bool m_bOnlyPrePlaces;
    bool m_bFirstOperatorDone;

    long m_nCurrentTransitionNumber;
    SP_DS_Node* m_pcCurrentTransitionNode;

    const SP_VectorLong* m_anCurrentMarking;

    //For hybrid Petri Net
    const SP_VectorDouble* m_anCurrentConcentration;
    bool m_bUseConcentration;

    double m_nSimIntervalStart;
    double m_nSimIntervalEnd;


    SP_MapString2Double* m_mnParameter;

    SP_MapString2Long* m_mnPlaceArrayPos;
    long m_nPlaceArrayPosRefCount;
    SP_MapString2Long* m_mnTransitionArrayPos;

    map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;

    map< wxString, SP_MapString2Double > m_mmLookupTablesMap;

    SP_MapString2Double m_mnLookupTablesDefaultValueMap;

    wxArrayString m_asMissingVariableList;

	wxArrayString m_asUnusedModifierPlaceList;
	wxArrayString m_asUnusedPrePlaceList;


    SP_SetLong m_lnBasedPlaces;

    SP_DS_StSimTransition* m_pcSimTransition;

    double* m_nSimTime;

    SP_MatrixDouble* m_aanDeltaIncMatrix;
    SP_MatrixPairDouble* m_aanPreIncMatrix;

    InfixOperator* m_pcInfixOperatorList;
    PrefixOperator* m_pcPrefixOperatorList;

    BinaryOperator* m_pcBinaryOperatorList;

    Function0ParamOperator* m_pcFunction0ParamOperatorList;
    Function2ParamOperator* m_pcFunction2ParamOperatorList;
    Function3ParamOperator* m_pcFunction3ParamOperatorList;

	//only for colored stochastic petri net 
	bool m_bColoredPN;
	//for consistence checking
	vector<wxString> m_vsPrePlaceInFunction;
	bool m_bModifierPlaceAsFactor;

///////////////////////////////////////////////////////////////////////////////////

  // methods


  public :

    SP_DS_StParser(SP_DS_Graph* p_pcGraph = NULL);
    virtual ~SP_DS_StParser();

    bool CheckFormulaFunction( const wxString& p_sFormula, SP_DS_Node* p_pcTransitionNode, bool p_bColoredPN = false );

    bool ParseFormulaFunction( wxString p_sFormula, long p_nTransitionNumber, SP_DS_StParserNode*& p_pcRoot,
      SP_DS_StSimHazardFunctionType* l_peHazardFunctionType );

    wxArrayString* GetMissingVariableList() { return &m_asMissingVariableList; }

	wxArrayString* GetUnusedModifierPlaceList() { return &m_asUnusedModifierPlaceList; }
	wxArrayString* GetUnusedPrePlaceList() { return &m_asUnusedPrePlaceList; }

    void SetCurrentMarking( const SP_VectorLong* p_anCurrentMarking ) { m_anCurrentMarking = p_anCurrentMarking;}
    void SetCurrentConcentration(const SP_VectorDouble* p_anCurrentConcentration);
    double FindCurrentMarking(const long &p_nIndex){return  m_bUseConcentration? (*m_anCurrentConcentration)[p_nIndex]:
																			( double )(*m_anCurrentMarking)[ p_nIndex];}
    void SetPlaceArrayPos( SP_MapString2Long* p_mnPlaceArrayPos );
    void SetTransitionArrayPos( SP_MapString2Long* p_mnTransitionArrayPos );

    SP_VectorLong GetBasedPlaces();

    SP_MapString2Double ConvertToLookupTable( SP_DS_ColListAttribute* p_pcColList );

    wxString GetFormulaString( SP_DS_StParserNode *p_pcTree );

    void SetDeltaIncMatrix( SP_MatrixDouble* p_aanDeltaIncMatrix ) { m_aanDeltaIncMatrix = p_aanDeltaIncMatrix; }
    void SetPreIncMatrix( SP_MatrixPairDouble* p_aanPreIncMatrix ) { m_aanPreIncMatrix = p_aanPreIncMatrix; }

    void LoadParameter();
    void LoadPrePlaces();

    double Evaluate( SP_DS_StParserNode* p_pcTree, SP_DS_StSimTransition* p_pcSimTransition);

    void FreeTree( SP_DS_StParserNode* p_pcTree );

    InfixOperator* GetInfixOperator() { return m_pcInfixOperatorList; }
    PrefixOperator* GetPrefixOperator() { return m_pcPrefixOperatorList; }
    BinaryOperator* GetBinaryOperator() { return m_pcBinaryOperatorList; }
    Function3ParamOperator* GetFunction3ParamOperator() { return m_pcFunction3ParamOperatorList; }
    Function2ParamOperator* GetFunction2ParamOperator() { return m_pcFunction2ParamOperatorList; }
    Function0ParamOperator* GetFunction0ParamOperator() { return m_pcFunction0ParamOperatorList; }

    void SetSimIntervalEnd( double l_nValue ) { m_nSimIntervalEnd = l_nValue; }
    void SetSimIntervalStart( double l_nValue ) { m_nSimIntervalStart = l_nValue; }

    void SetSimTime( double* l_pnTime ) { m_nSimTime = l_pnTime; }


	void SetPrePlaceArrayPosOfTransitionNode(const map< long, SP_MapString2Long >& p_mmPrePlaceArrayPosOfTransitionNode )
	{
		m_mmPrePlaceArrayPosOfTransitionNode = p_mmPrePlaceArrayPosOfTransitionNode;
	}

	vector<wxString>* GetPrePlacesForColoredPN(){return &m_vsPrePlaceInFunction;}

	bool ModifierPlaceAsFactor( wxString p_sFormula );

  private :

    void LoadLookupTables();
    void LoadPlaces();

    void LoadPlaceOfType(const wxString & p_sPlaceType);
    void LoadPrePlacesOfTransitionOfType(const wxString & p_sTransType,long& p_nPos);

    inline SP_MapString2Long* GetPrePlaceMapOfNode( long p_nTransitionPos )
    {
    	return &(m_mmPrePlaceArrayPosOfTransitionNode[p_nTransitionPos]);
    }

    SP_DS_StParserNode* Parse( wxChar* p_sFormula );

    SP_DS_StParserNode* CreateParserNode( SP_StParserNodeType p_nType );

    wxChar* Scan( wxChar* p_sFormula, wxChar p_cOperator, bool p_bMatchFirst = true );

    SP_DS_StParserNode* CreateParserNodeForInfixOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double ),
      wxChar* p );

    SP_DS_StParserNode* CreateParserNodeForPrefixOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double ) );

    SP_DS_StParserNode* CreateParserNodeForMassActionOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double ) );

    SP_DS_StParserNode* CreateParserNodeForFunction2ParamOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double ) );

    SP_DS_StParserNode* CreateParserNodeForLevelInterpretationOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double ) );

	 SP_DS_StParserNode* CreateParserNodeForMichaelisMentenOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double ) );

    SP_DS_StParserNode* CreateParserNodeForFunction3ParamOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) );

    SP_DS_StParserNode* CreateParserNodeForFunction0ParamOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )() );

    SP_DS_StParserNode* CreateParserNodeForFixedTimedFiring_Periodic_IntervalOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) );

    SP_DS_StParserNode* CreateParserNodeForMove2DGridOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) );

    SP_DS_StParserNode* CreateParserNodeForCountXYOperator( wxChar* p_sFormula,
      double ( SP_DS_StParser::*p_pfOperator )( double, double, double ) );

    SP_DS_StParserNode* CreateParserNodeForLookupTable( wxChar* p_sFormula, const wxString& p_sTableName );

    SP_DS_StParserNode* CreateParserNodeForVariable( wxChar* p_sFormula );

    bool PreProcess( SP_DS_StParserNode* p_pcTree );


    double CalcInfixPlus( double p_nArg1, double p_nArg2 );
    double CalcInfixMinus( double p_nArg1, double p_nArg2 );
    double CalcInfixTimes( double p_nArg1, double p_nArg2 );
    double CalcInfixDivide( double p_nArg1, double p_nArg2 );
    double CalcInfixPower( double p_nArg1, double p_nArg2 );

    double EvalOpnGt( double p_nArg1, double p_nArg2 );
    double EvalOpnLt( double p_nArg1, double p_nArg2 );
    double EvalOpnGeq( double p_nArg1, double p_nArg2 );
    double EvalOpnLeq( double p_nArg1, double p_nArg2 );
    double EvalOpnEq( double p_nArg1, double p_nArg2 );
    double EvalOpnNeq( double p_nArg1, double p_nArg2 );
    double EvalOpnAnd( double p_nArg1, double p_nArg2 );
    double EvalOpnOr( double p_nArg1, double p_nArg2 );

    double EvalLookupTable( const wxChar* p_sTableName, double p_nX );

    double CalcBinaryPower( double p_nArg1, double p_nArg2 );
    double CalcBinaryMin( double p_nArg1, double p_nArg2 );
    double CalcBinaryMax( double p_nArg1, double p_nArg2 );

    double CalcPrefixAbs( double p_nArg );
    double CalcPrefixAcos( double p_nArg );
    double CalcPrefixAsin( double p_nArg );
    double CalcPrefixAtan( double p_nArg );
    double CalcPrefixCeil( double p_nArg );
    double CalcPrefixCos( double p_nArg );
    double CalcPrefixExp( double p_nArg );
    double CalcPrefixFloor( double p_nArg );
    double CalcPrefixLog( double p_nArg );
    double CalcPrefixLog10( double p_nArg );
    double CalcPrefixSin( double p_nArg );
    double CalcPrefixSqr( double p_nArg );
    double CalcPrefixSqrt( double p_nArg );
    double CalcPrefixTan( double p_nArg );
    double CalcPrefixMinus( double p_nArg );
    double CalcPrefixNot( double p_nArg );

    double CalcBinomialCoefficient( double p_nN, double p_nK );
	double CalcLigandBindingPotential( double p_nArg0, double p_nArg1);
    double CalcMassAction( double p_nArg );
    double CalcLevelInterpretation( double p_nArg0, double p_nArg1 );

	double CalcMichaelisMenten( double p_nArg0, double p_nArg1 );

    double CalcPrefixDTT( double p_nArg );

    double DummyFunction0Param();
    double DummyFunction1Param( double p_nArg0 );
    double DummyFunction3Param( double p_nArg0, double p_nArg1, double p_nArg2 );

    void AssignNodeToVariable( wxChar* p_sVariable, SP_DS_StParserNode* p_pcNode );


    SP_DS_StParserNode* CreateTree( wxChar* p_sFormula );

    double Eval( SP_DS_StParserNode* p_pcTree );

    wxChar* ConvertToCharArray( const wxString& p_sFormula );

    void SetHazardFunctionType( SP_DS_StParserNode* p_pcRoot, SP_DS_StSimHazardFunctionType* p_peHazardFunctionType );

	bool CheckConsistence(SP_DS_Node* p_pcTransitionNode, wxString p_sFormula);


};

#endif
