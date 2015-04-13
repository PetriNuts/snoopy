//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ColPN_ProcessGuard__
#define __SP_ColPN_ProcessGuard__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"


struct SP_CPN_Expr_Guard_Conjunct
{	
	wxString						m_sExpression;
	wxString						m_sColorSetName;	
	//list<SP_CPN_ParsedColors>		m_ParsedColorsList;	
	//SP_MapString2String				m_VariableMap;
	vector<SP_CPN_Var_Color>        m_vVariable2Color;

	SP_CPN_Parse_Context_ptr 			m_pParseContext;
	SP_CPN_Driver_ptr 			m_pDriver;

	vector<SP_CPN_EvaluatedSingleValue>			m_ParsedInfo;
};





class SP_ColPN_ProcessGuard
{

public:
	void ParseGuard(wxString p_sGuardExpr);
	void InitializeParsing();
	void EvaluateGuard(map<wxString,unsigned> p_mBoundVariables,SP_MapString2UInt p_IndexMap,vector<vector<wxString> > &p_CompleteBinding,SP_CPN_ColorSetClass *p_pcColorSetClass);
	

private:
	vector<wxString> m_vConjunctions;

	vector<SP_CPN_Expr_Guard_Conjunct> m_ExpressionVector;

	SP_CPN_Parse_Context_ptr 			m_pParseContext;
	SP_CPN_Driver_ptr 			m_pDriver;
	

public:
	SP_ColPN_ProcessGuard();
	~SP_ColPN_ProcessGuard();

	

};

#endif // __SP_ColPN_ProcessGuard__

