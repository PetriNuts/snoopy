//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/6/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ColPN_ComBinding_H__
#define __SP_ColPN_ComBinding_H__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_ds/SP_DS_Node.h"


#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"



//abstract expression
class SP_ColPN_Expression
{
public:
	wxString							m_sExpression;
	
	SP_CPN_Parse_Context_ptr 				m_pParseContext;
	SP_CPN_Driver_ptr 				m_pDriver;
	
	vector<SP_CPN_EvaluatedSingleValue>	m_ParsedInfo;

public:
   SP_ColPN_Expression()
   {
		m_pParseContext = make_CPN_Parse_Context();
		m_pDriver = make_CPN_Driver(*m_pParseContext);
   }
   ~SP_ColPN_Expression()
   {
   }
};



//Expression for binding
class SP_ColPN_BindExpression : public SP_ColPN_Expression
{
public:
	//SP_CPN_ExprType						m_eExprType;
	wxString							m_sColorSetName;
	//SP_DS_Animator*					    m_pcPlAnimator;
	//SP_DS_Edge*							m_pcEdge;

	SP_CPN_PATTERNTYPE					m_PatternType;
	SP_MapString2String					m_VariableName;
};



//Expression for unfolding
class SP_ColPN_UnfoldExpression : public SP_ColPN_Expression
{
public:
	//SP_CPN_ExprType						m_eExprType;
	wxString							m_sColorSetName;
	
	//SP_DS_Node*							m_pcPlaceNode;	
	//SP_DS_Node*							m_pcTransNode;	
	//SP_DS_Edge*							m_pcEdge;
		
	//vector<SP_CPN_Var_Color>        m_vVariable2Color;
	
	vector<wxString>				m_RateFunctionVector;
	//for rate functions
	//vector<map<wxString, SP_ColPN_RateFunction> > m_RateFunctionTransformation;  
	wxString						m_sPredicate; 
	unsigned int                    m_nColumn;  
	
	//for initial marking
	vector<int>						m_nMaringArray;
	vector<double>					m_doubleMarkingArray;
	vector<wxString>				m_sMaringArray;
};



class SP_ColPN_ComBinding
{

protected:
	//vector<SP_ColPN_Expression>;

public:
    SP_ColPN_ComBinding();
    virtual ~SP_ColPN_ComBinding();

public: 
	bool Binding();

   
};

#endif // __SP_ColPN_ComBinding_H__

