//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: syntax checking class
// @Changes by George 25.01.2020: Using dssd_util for syntax checking and computing the initial marking of places
//////////////////////////////////////////////////////////////////////


#ifndef __SP_CPN_SyntaxChecking__
#define __SP_CPN_SyntaxChecking__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"
#include <dssd/auxi/exceptions.h>
#include <dssd/colexpr/colexpr_parser.h>



#include <dssd/andl/andl_builder.h>
#include <dssd/auxi/logger.h>
#include <dssd/colexpr/builder.h>
#include <dssd/colexpr/colexpr_parser.h>
#include <dssd/colexpr/environment.h>
#include <dssd/colexpr/eval.h>
#include <dssd/colexpr/modify.h>
#include <dssd/extern/alphanum.hpp>
#include <dssd/misc/net_evaluation.h>
#include "dssd/unfolding/net_unfolding.icc"
#include "dssd/unfolding/gecode_representation.h"
#include "dssd/unfolding/idd_representation.h"

struct SP_CPN_PredicateExpression
{
	wxString						m_sExpression;
	SP_CPN_Parse_Context_ptr 			m_pParseContext;
	SP_CPN_Driver_ptr 			m_pDriver;

	vector<SP_CPN_Var_Color>        m_vVariable2Color;

	vector<int>						m_nMaringArray;
	vector<double>					m_doubleMarkingArray;
	vector<wxString>				m_sMaringArray;

	vector<SP_CPN_EvaluatedSingleValue>			m_ParsedInfo;
};

struct SP_CPN_ColorToMarking
{
	wxString m_name;

	vector<wxString>	m_sMaringArray;
	vector<int>			m_nMaringArray;
	vector<double>		m_doubleMaringArray;

};

union SP_CPN_TokenNum
{
	int							m_intMultiplicity;
	double						m_DoubleMultiplicity;
};


class SP_CPN_SyntaxChecking
{

private:
	vector<SP_CPN_PredicateExpression> m_ExpressionVector;
	SP_CPN_PredicateExpression m_PredicateExpression;
	SP_CPN_Parse_Context_ptr m_pParseContext;
	SP_CPN_Driver_ptr  m_pDriver;

	wxString m_sNetClassName;

	SP_CPN_ColorSetClass m_cColorSetClass;
	SP_CPN_ValueAssign m_cValueAssign;

	dssd::colexpr::environment colDefinitions_;
	dssd::colexpr::parser* l_pcdssdParser;//experemental snoopy syntax checking of colPN using dssd util grammer
	dssd::colexpr::builder* l_pcBuilder;
	 
public:
	SP_CPN_SyntaxChecking();
	~SP_CPN_SyntaxChecking();
	bool Initialize();
	//bool IsDefFunction(wxString p_sColor);					//used	
	std::map<wxString, SP_CPN_Variable_Constant>* GetVariableMap();

private:
	bool CheckPlaceClass(wxString p_sPlaceNCName);			//used
	bool CheckTransitionClass(wxString p_sTransNCName);		//used

	bool Binding(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_AllBindingVector, SP_CPN_ColorSetClass* p_pcColorSetClass);		//used
	wxString GetEdgeName(SP_DS_Edge* p_pcEdge);				//used
	void GetPlacetoColorSet(SP_DS_Node* p_pcTransNode, map<wxString, wxString> *p_pmPlaceToColorSet);											//used

	bool CheckExpression(wxString p_sExpression, wxString p_sErrorPosition, wxString p_sColorSetName, wxString p_sPlaceType, bool p_bFunctionFlag = false, bool p_bGuardFlag = false);   //used	

	bool IsDefFunction(wxString p_sColor);					//used	

	wxString SubstituteConstants(wxString p_sColorExpr);    //used
	 
	void PrepareExpressionString(wxString& l_sRawExp,wxString& p_sPredicate,wxString& l_sResultExp);//george

	bool CheckBracketStructure(wxString p_sExpression, wxString p_csName);

	wxString m_sPlaceName;//george 
	wxString m_sPlaceExp;
	bool m_bIsAll;

	bool CheckAllExprSpecialCase(wxString p_sColExpr,SP_CPN_ColorSet& p_colorSet,wxString& p_sNewExp);
	wxString ObtainPlaceUnfoldedColor(wxString p_sPlaceName);
private:
	bool IntCSSuccessiveTest(vector<SP_CPN_Var_Color>& p_vVariable2Color);
	bool CSPSolve(SP_CPN_Parse_Context_ptr p_pcParseContext, SP_CPN_ColorSet* p_pcColorSet, vector<SP_CPN_Var_Color>& p_vVariable2Color, vector<wxString> &p_vParsedColors);
	bool OrdinarySolve(SP_CPN_Parse_Context_ptr p_pcParseContext, SP_CPN_ColorSet* p_pcColorSet, vector<SP_CPN_Var_Color>& p_vVariable2Color, vector<wxString> &p_vParsedColors);


public:
	bool SyntaxChecking();
	bool CheckArcExpression(SP_DS_Node* p_pcPlaceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTransNode);							//used
	bool CheckGuard(SP_DS_Node* p_pcTransNode);																					//used
																																//used for checking rate functions of stochastic transitions
	bool CheckFormula(SP_DS_Node* p_pcTransNode, wxString p_sRateFunExpr, SP_DS_StParser* p_pcParser, bool p_bSingle = true);	//used
																																//used in the color set definition
	bool GenerateSubSetColorSet(wxString p_sSubSetName, wxString p_sColors, wxString p_sFatherColorSetName, SP_CPN_ColorSetClass* p_pcColorSetClass, vector<wxString> &l_vColors);  //used
																																													//used to update marking when a color set or constant changes
	void UpdateNetMarking();
	void UpdateMarkingPlaceClass(wxString p_sPlaceNCName); //used


														   ///////////////////////////new marking functions
private:
	SP_DS_Graph* m_pcGraph;
	//for marking checking or computing
	bool m_bMarkingCheck;
	wxString m_sErrorPosition;
	//record the marking compution history during marking checking and computation
	map<wxString, vector<wxString> > m_mvCSExpr2PColors;

	vector< vector<unsigned int> > m_vvCSPAssigns;

public:
	//marking check
	bool ComputeInitialMarking(SP_DS_Node* p_pcPlaceNode, map<wxString, vector<SP_CPN_TokenNum> >& p_mColorToMarkingMap, bool p_bMarkingCheck = true);   //used

	bool CheckPredicateInRateFunction(wxString p_sPredicate, wxString p_sErrorPosition);																//used

	bool ComputeRandomMarking(wxString p_sColorSetName, vector< vector<wxString> > p_vvColorExprs, map<wxString, vector<SP_CPN_TokenNum> >& p_mColorToMarkingMap);  //used

	bool ComputeMarkingUsingDssdUtil(wxString& p_sPlaceExp, wxString& p_sPlaceName, SP_CPN_ColorSet& p_cColorSet,std::vector<wxString>& l_vColorVector);//george

	void ComputeResultMarkingColours(dssd::unfolding::placeLookUpTable p_placeLookupTable, SP_CPN_ColorSet& p_cColorSet, wxString& p_sTokenNum, wxString& p_sPlaceName, vector<wxString>& p_vColVector);
	 

private:
	//marking check
	bool GetTokenNumber(wxString p_sTokenNumber, SP_CPN_TokenNum &p_uNumber, wxString p_sErrorPosition, wxString p_sPlaceType);						//used
	bool GetPlaceColorSet(SP_DS_Node* p_pcPlaceNode, SP_CPN_ColorSet &p_cColorSet);																	//used
	bool ComputeInitialMarkingStep2(wxString p_sColorExpr, vector<wxString> p_vColorVector, SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors); //used
	bool ComputeSinglePredicate(wxString p_sColorExpr, SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors);							//used
	bool ComputeAllFunction(wxString p_sColorExpr, SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors);							//used
	bool ComputeOtherCase(wxString p_sColorExpr, wxString p_sSubSetName, SP_CPN_ColorSet* p_pcColorSet, vector<wxString> &p_vParsedColors);	//used

																																			//rate function check	
	bool CheckTransRateFunction(SP_DS_Node* p_pcTransNode);																							//used

	bool CheckIDSRateFunction(wxString p_sRate, SP_DS_Node* p_pcTransNode);																			//used

	void SeperateColors(wxString p_sVariableString, vector<wxString> &p_sColorVector);






};

#endif // __SP_CPN_SyntaxChecking__

