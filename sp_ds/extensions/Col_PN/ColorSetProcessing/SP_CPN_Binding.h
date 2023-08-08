//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: Binding class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_Binding__
#define __SP_CPN_Binding__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include "sp_ds/animators/ani_CPN/SP_DS_CPN_PlaceAnimator.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStPlaceAnimator.h"


class SP_DS_CPN_TransAnimator;
class SP_DS_ColStTransAnimator;

enum SP_CPN_ExprType
{
	CPN_INPUT_EXPR,
	CPN_OUTPUT_EXPR,
	CPN_GUARD_EXPR,
	CPN_PREDICATE_EXPR,
	CPN_RATEFUNCTION_EXPR
};

struct SP_CPN_ParsedColors
{
	int m_nMultiplicity;
	wxString m_sColorValue;
};

struct SP_CPN_ExpressionInfo
{
	SP_CPN_ExprType					m_eExprType;
	wxString						m_sExpression;
	SP_DS_Edge*						m_pcEdge;

	SP_DS_Animator*					m_pcPlAnimator;	

	vector<SP_CPN_ParsedColors>		m_ParsedColorsList;				//For QPNc, in fact, they are bound colors	
	map< wxString,vector<SP_CPN_ParsedColors> > m_mvParsedColors;	//For SPNc
};

struct SP_CPN_ParseInput
{

	SP_CPN_ExprType						m_eExprType;
	wxString							m_sColorSetName;
	wxString							m_sExpression;

	SP_DS_Animator*					    m_pcPlAnimator;
	SP_DS_Edge*							m_pcEdge;

	SP_CPN_PATTERNTYPE					m_PatternType;
	SP_MapString2String					m_VariableName;

	int									m_Multiplicity;
	bool								m_MultiSet;

	SP_CPN_Parse_Context_ptr 				m_pParseContext;
	SP_CPN_Driver_ptr 				m_pDriver;

	vector<SP_CPN_EvaluatedSingleValue>	m_ParsedInfo;
};



class SP_CPN_Binding
{

private:
	SP_CPN_Parse_Context_ptr m_pParseContext;
	SP_CPN_Driver_ptr  m_pDriver;

	SP_CPN_ValueAssign m_ValueAssign;	
	SP_CPN_ColorSetClass m_ColorSetClass;
	
	vector<SP_CPN_ParseInput> m_ParseInputVector;	
	SP_DS_Graph* m_pcGraph;

	vector<wxString> m_vMultiTupleExpression;
	SP_VectorString m_OutputVector;//by george
	SP_VectorString m_BindingVector;//by george
	SP_MapString2String m_mPreplace2Color;//by george
	SP_MapString2String m_mColor2Preplace;//by george
	SP_MapString2String m_mbinding2ResolvedRateFunction;
	SP_MapString2String m_mRateFunctionMap;
	vector<vector<wxString> > m_vvCompleteBinding;//george we need this for checking the possible bindings of the variables when we do import trace fro the col animators
	std::string ConvertToNewFormat(const std::string& inputString);
	SP_MapString2String FormatRateFunction(const wxString& inpuRateFunction);

public:
	SP_CPN_Binding();	
	~SP_CPN_Binding();
	SP_MapString2String GetResolvedbindedRateFunctions() {
		return m_mbinding2ResolvedRateFunction
			;
	}
	void SetRateFunctionMap(const SP_MapString2String& p_mRateFunMap) {
		m_mRateFunctionMap = p_mRateFunMap
			;
	}
	std::string SubstituteSubstring(const std::string& input, const std::string& target, const std::string& replacement);

	bool EnableTestForColorSimulation(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick, wxString m_sTransitionName,
		int p_nBindingChoice, map<wxString, map<SP_DS_Edge*, map<wxString, int> > >& p_mmmBind2Edge2Mult2Color);
	bool EnableTest(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick, SP_DS_Animator* p_pcTransAnimator, int p_nBindingChoice, map<wxString, map<SP_DS_Edge*, map<wxString,int> > >& p_mmmBind2Edge2Mult2Color, const std::vector<wxString>& values = std::vector<wxString>(), bool p_bIsColSim=false);
	void GetBinding(map<SP_DS_Edge*, map<wxString, int> >& p_mmEdge2Mult2Color);
	SP_VectorString GetBindingSelection() { return m_BindingVector; }// m_OutputVector;//by george
	vector<vector<wxString> > GetColmpleteBinding() { return m_vvCompleteBinding; }//george
	SP_MapString2String GetResolvedRateFunctions(SP_MapString2String& pred2RateMap);
private:
	bool BindingInference(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_CompleteBinding);
	bool Ordering();
	bool Parsing();	
	bool writecolors(vector<SP_CPN_ParseInput> *p_ParseInputVector ,vector<SP_CPN_ExpressionInfo>* p_pcExprInfoList);
	bool Match(wxString p_VariableString, wxString p_ValueString, vector<wxString> &StringVariable, vector<wxString> &StringValue);
	bool CheckVariableBinding(SP_MapString2String p_mCheckVariables, map<wxString,bool> p_mVariables);	
	bool BuildParserTree(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector);
	void ParseMultiSetTuple(wxString p_sExpr);
	void CombineBindings(vector<vector<wxString> > l_vNewBindings, vector<vector<wxString> > &l_vCompleteBindings);

	wxString RemoveRedundantTransInstances();

public:
	bool Diffusion(wxString p_sColorSet, wxString &p_sArcExpr, SP_CPN_ColorSetClass* p_pcColorSetClass);
	wxString GetEdgeName(SP_DS_Edge* p_pcEdge);
	bool IsbalanceParentheses(const wxString& p_sColExp);//by george2020
private:
	wxString m_sTransitionName;
	vector<wxString> m_TransitionInstancesVector;
	map<wxString, bool> m_mExistingBindings;


//not used any longer
public:	
	bool EnableTest(vector<SP_CPN_ExpressionInfo>* p_pcExprInfoVector, bool p_bSingleClick, SP_DS_ColStTransAnimator* p_pcTransAnimator, SP_DS_Node* p_pcParent, map<wxString,wxString>& p_msBindingsToTransition, bool p_bIscolSim= false);
private:
	bool writeConstantcolors(vector<SP_CPN_ParseInput> *p_ParseInputList ,vector<SP_CPN_ExpressionInfo>* p_pcStExprInfoVector);
//not used any longer	


//for the new binding algorithm
private:
	//Store all the patterns, corresponding to PS(t)
	vector<SP_CPN_ExpressionPatternInfo> m_vExprPatterns;
	vector<SP_CPN_ExpressionPatternInfo> m_vGuardTestPatterns;
	SP_MapString2String m_mTransitionVariables;
	SP_MapString2String m_mVariablesBoundbyColorSet;
private:
	bool TestMultiplicity();
	bool MergePatterns();
	bool SortByTokenNumber();
	bool SortVariables();
	bool PreProcession();
//for the new binding algorithm

};

#endif // __SP_CPN_Binding__

