//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// $Updated:by george assaf with new operations for colored PN
// Short Description: Nodes classes
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_Parse_Context_H
#define __SP_CPN_Parse_Context_H

#include <map>
#include <vector>
#include <ostream>
#include <stdexcept>
#include <cmath>

#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColCSPSovler.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSet.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

////////////define the structs for parser//////////
 
// Define the node types of parse trees
enum SP_CPN_NODETYPE
{
    CPN_INTEGERCONSTANT_NODE, 
	CPN_DOUBLECONSTANT_NODE,
	CPN_INDEXNAME_NODE,       // Only used to indicate that it is an index name
	CPN_NEIGHBOR1D,
	CPN_NEIGHBOR2D,
	CPN_ALLFUNC,
	CPN_FUNCTIONNAME_NODE,
	CPN_ARGUMENT_NODE,
	CPN_UNIONVARIABLE_NODE,
	CPN_CONSTANT_NODE,
	CPN_VARIABLE_NODE,
	CPN_PREDECESSOR_NODE,
	CPN_SUCCESSOR_NODE,
	CPN_FUNCTION_NODE,
	CPN_NOT_NODE,
	CPN_INDEXABSTRACT_NODE,
	CPN_DOT_NODE,
	CPN_INDEX_NODE,
	CPN_ADD_NODE,
	CPN_SUBSTRACT_NODE,
	CPN_MULTIPLY_NODE,
	CPN_DIVIDE_NODE,
	CPN_MODULOS_NODE,
	CPN_POWER_NODE,
	CPN_EQUAL_NODE,
	CPN_NOTEQUAL_NODE,
	CPN_GREATTHAN_NODE,
	CPN_GREATTHANEQUAL_NODE,
	CPN_LESSTHAN_NODE,
	CPN_LESSTHANEQUAL_NODE,
	CPN_AND_NODE,
	CPN_OR_NODE,
	CPN_COMMA_NODE,
	CPN_BRACKET_NODE,
	CPN_SEPERATOR_NODE,
	CPN_PREDICATE_NODE,
	CPN_CONNECTOR_NODE,
	CPN_PLACE_NODE,
	CPN_ELEMENT_OF_NODE,
	CPN_NUMOF_NODE,
	CPN_RAND_NODE
};

// Define the pattern types for bindings
enum SP_CPN_PATTERNTYPE
{
	CPN_PATTERN_VARIABLE,
	CPN_PATTERN_MULTISETTUPLE,
	CPN_PATTERN_TUPLE,
	CPN_PATTERN_KEYTUPLE,
	CPN_PATTERN_CONSTANT,
	CPN_PATTERN_GUARD_TYPE1,	// such form as variable = constant
	CPN_PATTERN_GUARD_TYPE2,	// such form as variable = variable
	CPN_PATTERN_GUARD_TEST,
	CPN_PATTERN_NULL
};

// Define the expression pattern information, these patterns will be used to bind to markings
struct SP_CPN_ExpressionPatternInfo
{
	wxString						m_Expression;			//the expression of the pattern

	wxString						m_lsExpression;          //the left hand of a guard
	wxString						m_rsExpression;          //the right hand of a guard
	wxString						m_sOpetator;			//the operator of a guard

	SP_CPN_PATTERNTYPE				m_PatternType;
	SP_MapString2String				m_VariableName;			//When not bound in pattern, then will bind it to color
	SP_MapString2String				m_UnionVariableMap;		//Not used

	struct
	{
		int							m_Multiplicity;
		double						m_DoubleMultiplicity;
	};
	bool							m_MultiSet;				//Not use it

	map<wxString,int>				m_mIniMarking;			//Initial marking

	wxString						m_sColorSet;			//Color set of the pattern, not each of the variable
};

//Define the variables structure that are used to binding and unfolding
struct SP_CPN_BindingVariables
{
	wxString m_sVarName;
	wxString m_sVarColorSet;
	wxString m_sPredicateValue;
};


struct SP_CPN_Var_Color
{
	wxString m_sVariable;
	wxString m_sColor;
} ;




//represent evaluated results
struct SP_CPN_EvaluatedSingleValue
{
	wxString						m_ColorValue;

		int							m_Multiplicity;
		double						m_DoubleMultiplicity;
		wxString		m_stringMultiplicity;	//for marking-dependent arcs, added by Fei, 09.2015

	bool							m_Predicate;

	bool					m_bPlaceFlag;	//for marking-dependent arcs, added by Fei, 09.2015
};


// Define the information of each parse node
struct SP_CPN_ParseNode_Info
 {
      SP_CPN_DATATYPE		m_DataType;			// Data type of color set	 
	  wxString				m_ColorSet;			// Name of color set	 
	  
	  wxString				m_CheckColorSet;	// only for checking color set
	  SP_CPN_DATATYPE		m_CheckDataType;	// only for checking data type
	  
	  wxString				m_VariableName;     //only for variable 
	  wxString				m_CheckedString;    //only for checking expression

	  wxString				m_sColorSetList;    //for the new checking algorithm
	  
	  SP_CPN_NODETYPE		m_NodeType; 
	  
	  bool					m_bSeparaterExpression; // evaluate if an expression is a part of separator expression?
	  bool					m_bPlaceFlag;		//for marking-dependent arcs, added by Fei, 09.2015
	  wxString				m_sColoredPlaceName;//for marking-dependent arcs, added by Fei, 09.2015

		  bool				m_Predicate;		// Store the evaluation value of predicate, defalt is true

			  int			m_Multiplicity;
			  double		m_DoubleMultiplicity;
			  wxString		m_stringMultiplicity;	//for marking-dependent arcs, added by Fei, 09.2015
			  bool m_bIsElemOf;//by george.20.2020
			  bool m_bIscuncolorfun;//by george.20.2020
			  int l_nAllPos;//by george.20.2020
			  wxString l_sAllPatern;//by george.20.2020
			  
	  union
      {
		int					m_IntegerValue;		
		wxString*			m_StringValue;
		bool				m_BooleanValue;
		double				m_DoubleValue;
      };

	  vector<SP_CPN_EvaluatedSingleValue> m_EvalResults;   //only for collecting the final results  

	  std::vector<IntVar>*  m_vIntconstraintVector= new std::vector<IntVar>();//by george 
	  IntVar                m_IntConstraintExpr;
	  BoolVar               m_BoolConstraintExpr;
 };



class SP_CPN_ParseNode
{
protected:
	SP_CPN_ParseNode* m_pLeft;
	SP_CPN_ParseNode* m_pRight;
	SP_CPN_ParseNode_Info m_ParseNode_Info;

protected:
	//global variables
	SP_CPN_ColorSetClass *m_pColorSetClass;		// point to the colorset class
	wxString m_ColorSetName;					// the name of a color set
	bool m_bFunctionFlag;						// indicate if it is a function or an experession
	wxString m_sFunctionName; 
	wxString m_sNetClassName;					//net class name //will use node class replace
	wxString m_sErrorPosition;
	wxString m_sPlaceType;
	map<wxString, vector<wxString> > *m_psvColored2UnColoredPlaceNames;
	SP_DS_Graph* m_pcGraph;//by george for fetching number of token of a certain colour in a certain place 29.08.2021

public:
	void SetColorSetName(wxString p_ColorSetName){m_ColorSetName = p_ColorSetName;}
	wxString GetColorSetName() {return m_ColorSetName;}//by george
	void SetColorSetClass( SP_CPN_ColorSetClass* p_pColorSetClass ){m_pColorSetClass = p_pColorSetClass;}
	void SetErrorPosition(wxString p_sErrorPosition){m_sErrorPosition = p_sErrorPosition;}
	void SetNetClassName(wxString p_sNetClassName){m_sNetClassName = p_sNetClassName;}	
	void SetPlaceType(wxString p_sPlaceType){m_sPlaceType = p_sPlaceType;}
	void SetFunctionFlag(bool p_bFunctionFlag){m_bFunctionFlag = p_bFunctionFlag;}
	void SetFunctionName(wxString p_sFunctionName){m_sFunctionName = p_sFunctionName;}	
	void SetColored2UnColoredPlaceNames(map<wxString, vector<wxString> > *p_psvColored2UnColoredPlaceNames){ m_psvColored2UnColoredPlaceNames = p_psvColored2UnColoredPlaceNames;}
	void SetGraph() { m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph(); }//by george 1.9.21

public:    

    SP_CPN_ParseNode()
	{
		// Initialized as empty
		m_pLeft = 0;
		m_pRight = 0;
		m_ParseNode_Info.m_CheckColorSet = wxT("");		
		m_ParseNode_Info.m_Multiplicity = 1;
		m_ParseNode_Info.m_DoubleMultiplicity = 1.0;
		m_ParseNode_Info.m_Predicate = true;
		m_ParseNode_Info.m_CheckedString = wxT("");
		m_ParseNode_Info.m_EvalResults.clear();
		m_ParseNode_Info.m_bPlaceFlag = false; //for marking-dependent arcs, added by Fei, 09.2015
		m_ParseNode_Info.m_bSeparaterExpression = false;
		
	}

 

	virtual ~SP_CPN_ParseNode() { } 	

	/// evaluate the parse tree
    virtual SP_CPN_ParseNode_Info evaluate() = 0;

	/// Check the parse tree
	virtual bool check() = 0;

	virtual wxString GenerateExpression() = 0; //not used so far

	virtual void CollectResult();

	virtual bool GetConstraints(SP_DS_ColCSPSovler &p_cCSPSolver) = 0;


	SP_CPN_ParseNode* GetLeftNode()	{ return m_pLeft; }	
	SP_CPN_ParseNode* GetRightNode(){ return m_pRight; }
	SP_CPN_ParseNode_Info* GetParseNodeInfo() { return &m_ParseNode_Info; }

	void   IsAllNodeExists(SP_CPN_ParseNode* p_nodeRoot, bool & P_isFound)
	{
		if (!p_nodeRoot)
			return;

		// if node is leaf node, print its data    
		if (!p_nodeRoot->GetLeftNode() && !p_nodeRoot->GetRightNode())
		{
			if (p_nodeRoot->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)
			{
				P_isFound = true;
				return;

			}
		}

		if (p_nodeRoot->GetLeftNode())
			IsAllNodeExists(p_nodeRoot->GetLeftNode(), P_isFound);

		
		// recursively
		if (p_nodeRoot->GetRightNode())
			IsAllNodeExists(p_nodeRoot->GetRightNode(), P_isFound);
	}

};

/** parse node always returning an integer constant value. */

class SP_CPN_Parse_Integer_Node : public SP_CPN_ParseNode
{
public:  
    explicit SP_CPN_Parse_Integer_Node(int p_value)
	: SP_CPN_ParseNode()
    {
		m_pLeft = 0;
		m_pRight = 0;
		m_ParseNode_Info.m_CheckColorSet = wxT("");	
		m_ParseNode_Info.m_IntegerValue = p_value;
		m_ParseNode_Info.m_DataType = CPN_INTEGER;
		m_ParseNode_Info.m_NodeType = CPN_INTEGERCONSTANT_NODE;	
		m_ParseNode_Info.m_CheckedString = wxString::Format(wxT("%d"),p_value); 
		//for data type checking
		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << CPN_INTEGER;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		CollectResult();
		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxString::Format(wxT("%d"),m_ParseNode_Info.m_IntegerValue);
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, m_ParseNode_Info.m_IntegerValue);		
		return true;
	}

	 
};


/** parse node always returning a double constant value. */

class SP_CPN_Parse_Double_Node : public SP_CPN_ParseNode
{

public:
  
    explicit SP_CPN_Parse_Double_Node(double p_value)
	: SP_CPN_ParseNode()
    {
		m_pLeft = 0;
		m_pRight = 0;

		m_ParseNode_Info.m_CheckColorSet = wxT("");	
		m_ParseNode_Info.m_DoubleValue = p_value;
		m_ParseNode_Info.m_NodeType = CPN_DOUBLECONSTANT_NODE;	
		m_ParseNode_Info.m_CheckedString = wxString::Format(wxT("%f"),p_value); 

		//m_ParseNode_Info.m_sColorSetList = wxT("");
		//m_ParseNode_Info.m_sColorSetList << CPN_INTEGER;  //to be changed
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		//only for multiplicity not color, because color is not allowed to be a real.
		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxString::Format(wxT("%f"),m_ParseNode_Info.m_DoubleValue);
	}

	virtual bool check()
	{
		if (m_ParseNode_Info.m_bSeparaterExpression)
		{			
			m_ParseNode_Info.m_DoubleMultiplicity = m_ParseNode_Info.m_DoubleValue;			
		}
		return true;
	}


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		return false;		
	}


};


/** parse node always returning a string constant value. */
class SP_CPN_Parse_String_Node : public SP_CPN_ParseNode
{

private:
	wxString m_StringConstant;
    
public:
    /// construct a string constant parse node from a value
    explicit SP_CPN_Parse_String_Node(std::string p_value)
	: SP_CPN_ParseNode()
    {
		m_pLeft = 0;
		m_pRight = 0;	
		
		m_ParseNode_Info.m_CheckColorSet = wxT("");	

		m_StringConstant = wxString( p_value.c_str(), wxConvUTF8); // Note that it is necessary	
		m_ParseNode_Info.m_CheckedString = m_StringConstant;
		m_ParseNode_Info.m_DataType  = CPN_STRING; //
		m_ParseNode_Info.m_NodeType  = CPN_CONSTANT_NODE;
		m_ParseNode_Info.m_StringValue = &m_StringConstant;	

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << CPN_STRING;		
    }

	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return m_StringConstant;
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		return false;	
	}

};


/** parse node always returning a variable value. */
class SP_CPN_Parse_Variable_Node : public SP_CPN_ParseNode
{

private:
	wxString m_VariableName; 
	wxString m_String;
    
public:
   
    explicit SP_CPN_Parse_Variable_Node(std::string p_variablename)
	: SP_CPN_ParseNode()
    {	
		m_pLeft = 0;
		m_pRight = 0;
		m_VariableName = wxString( p_variablename.c_str(), wxConvUTF8);	
		m_ParseNode_Info.m_CheckedString = m_VariableName;
		m_ParseNode_Info.m_NodeType = CPN_VARIABLE_NODE; //in fact, not decided yet, but will decide by calling check function.
		//m_bProVarErrorFlag = false;
		//if( ! ProcessingVariable() ) {m_bProVarErrorFlag = true;}
			
    }

    bool ProcessingVariable();
	void SetValue();

	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SetValue();
		if(m_ParseNode_Info.m_NodeType != CPN_PLACE_NODE)
			CollectResult();

		if (m_ParseNode_Info.m_DataType == CPN_INTEGER && m_ParseNode_Info.m_NodeType == CPN_VARIABLE_NODE)
		{
			if (m_ParseNode_Info.m_bSeparaterExpression)
			{
				if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
					m_ParseNode_Info.m_DoubleMultiplicity = m_ParseNode_Info.m_IntegerValue;
				else
					m_ParseNode_Info.m_Multiplicity = m_ParseNode_Info.m_IntegerValue;
			}
		}

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return m_VariableName;
	}

	virtual bool check();    

	
	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver);


};

/** parse node always returning a Neighbor1D value. */
class SP_CPN_Parse_Neighbor12D_Node : public SP_CPN_ParseNode
{

private:
	wxString m_sFuncName; 
	wxString m_String;	
    
public:
   
    explicit SP_CPN_Parse_Neighbor12D_Node(std::string p_sFuncName)
	: SP_CPN_ParseNode()
    {	
		m_pLeft = 0;
		m_pRight = 0;
		m_sFuncName = wxString( p_sFuncName.c_str(), wxConvUTF8);	
		m_ParseNode_Info.m_CheckedString = m_sFuncName;		
		m_ParseNode_Info.m_DataType = CPN_INTEGER;
    }

   
	void GetValue();

	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		GetValue();
		CollectResult();
		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return m_sFuncName;
	}

	virtual bool check();    

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{	
		return false;
	}

};


/** parse node always returning an all function value. */
class SP_CPN_Parse_AllFunc_Node : public SP_CPN_ParseNode
{

private:
	wxString m_sFuncName; 
	wxString m_String;	
    
public:
   
    explicit SP_CPN_Parse_AllFunc_Node(std::string p_sFuncName)
	: SP_CPN_ParseNode()
    {	
		m_pLeft = 0;
		m_pRight = 0;
		m_sFuncName = wxString( p_sFuncName.c_str(), wxConvUTF8);	
		m_ParseNode_Info.m_CheckedString = m_sFuncName;	
		m_ParseNode_Info.m_NodeType = CPN_ALLFUNC;//george 19.12
    }

   
	bool GetValue();
	 
	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		GetValue();
	 
		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return m_sFuncName;
	}

	virtual bool check();    

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}

};

/** parse node always returning an abs function value. */
class SP_CPN_Parse_AbsFunc_Node : public SP_CPN_ParseNode
{

private:
	wxString m_sFuncName; 
	wxString m_String;	
    
public:
   
    explicit SP_CPN_Parse_AbsFunc_Node(std::string p_sFuncName,SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {	
		m_pLeft = 0;
		m_pRight = 0;
		m_sFuncName = wxString( p_sFuncName.c_str(), wxConvUTF8);	
		m_ParseNode_Info.m_CheckedString = m_sFuncName;		
		m_pRight = p_pcNode;				
    }	

	virtual SP_CPN_ParseNode_Info evaluate(); 
  
	virtual wxString GenerateExpression()
	{
		return m_sFuncName;
	}

	virtual bool check();    

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}


};


/** parse node always returning an val function value. */
class SP_CPN_Parse_ValFunc_Node : public SP_CPN_ParseNode
{

private:
	wxString m_sFuncName; 
	wxString m_String;	
    
public:
   
    explicit SP_CPN_Parse_ValFunc_Node(std::string p_sFuncName,SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {	
		m_pLeft = 0;
		m_pRight = 0;
		m_sFuncName = wxString( p_sFuncName.c_str(), wxConvUTF8);	
		m_ParseNode_Info.m_CheckedString = m_sFuncName;		
		m_pRight = p_pcNode;				
    }	

	virtual SP_CPN_ParseNode_Info evaluate(); 
  
	virtual wxString GenerateExpression()
	{
		return m_sFuncName;
	}

	virtual bool check();    

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}

};


/** parse node Predecessor **/
class SP_CPN_Parse_Predecessor_Node : public SP_CPN_ParseNode
{
private:
	wxString m_String;

public:
    explicit SP_CPN_Parse_Predecessor_Node(SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcNode;				
		m_pRight = 0;

		m_ParseNode_Info.m_NodeType  = CPN_PREDECESSOR_NODE ;
    }

    virtual ~SP_CPN_Parse_Predecessor_Node()
    {
		delete m_pLeft;
    }

	
	bool GetPredecessor();

    virtual SP_CPN_ParseNode_Info evaluate() 
    {		
		m_pLeft->evaluate();
		GetPredecessor();	

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("-") + m_pLeft->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{				
		return false;
	}



};




/** parse node successor. **/
class SP_CPN_Parse_Successor_Node : public SP_CPN_ParseNode
{
private:
	wxString m_String;

public:
    explicit SP_CPN_Parse_Successor_Node(SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcNode;
		m_pRight = 0;
		m_ParseNode_Info.m_NodeType  = CPN_SUCCESSOR_NODE ;
    }

    virtual ~SP_CPN_Parse_Successor_Node()
    {
		delete m_pLeft;
    }

    bool GetSuccessor();

	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		GetSuccessor();

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("+") + m_pLeft->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{	
		return false;
	}


};


/** parse node index abstract. **/
class SP_CPN_Parse_IndexAbstract_Node : public SP_CPN_ParseNode
{

public:
    explicit SP_CPN_Parse_IndexAbstract_Node(SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcNode;
		m_pRight = 0;
		m_ParseNode_Info.m_NodeType = CPN_INDEXABSTRACT_NODE ;
		m_ParseNode_Info.m_DataType = CPN_INTEGER;
    }

    virtual ~SP_CPN_Parse_IndexAbstract_Node()
    {
		delete m_pLeft;
    }
	
	bool GetIndex();
    
	virtual SP_CPN_ParseNode_Info evaluate() 
    {
		GetIndex();	

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("@") + m_pLeft->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}


};

//Rand node, george

class SP_CPN_Parse_Rand_Node : public SP_CPN_ParseNode
{

public:
	explicit SP_CPN_Parse_Rand_Node(SP_CPN_ParseNode* p_pcNodeleft, SP_CPN_ParseNode* p_pcNoderight)
		: SP_CPN_ParseNode()
	{
		m_pLeft = p_pcNodeleft;
		m_pRight = p_pcNoderight;
		m_ParseNode_Info.m_NodeType = CPN_RAND_NODE;
		m_ParseNode_Info.m_DataType = CPN_STRING;
	}

	virtual ~SP_CPN_Parse_Rand_Node()
	{
		delete m_pLeft;
		delete m_pRight;
	}

	//bool GetIndex();

	virtual SP_CPN_ParseNode_Info evaluate();


	virtual wxString GenerateExpression()
	{
		return wxT("@") + m_pLeft->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
	{
		return false;
	}


};

/** parse node not **/
class SP_CPN_Parse_Not_Node : public SP_CPN_ParseNode
{

public:
    explicit SP_CPN_Parse_Not_Node(SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcNode;
		m_pRight = 0;

		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;
		m_ParseNode_Info.m_NodeType = CPN_NOT_NODE;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_Not_Node()
    {
		delete m_pLeft;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {		
		m_ParseNode_Info.m_BooleanValue = ! (m_pLeft->evaluate().m_BooleanValue); 

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("!") + m_pLeft->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;

		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, ! l_pcLeftParseNode_Info->m_BoolConstraintExpr );		
		
		return true;
	}
	

};

/** parse node function. **/
class SP_CPN_Parse_Function_Node : public SP_CPN_ParseNode
{    
private:
	wxString m_String;
public:
    explicit SP_CPN_Parse_Function_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_FUNCTION_NODE ;
    }

    virtual ~SP_CPN_Parse_Function_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

	virtual SP_CPN_ParseNode_Info evaluate();

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver);
	

};


/** parse node argument nodes. */

class SP_CPN_Parse_Argument_Node : public SP_CPN_ParseNode
{

private:
	wxString m_String; // Store the string for the m_ParseNode_Info.m_StringValue

public:
    explicit SP_CPN_Parse_Argument_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_ARGUMENT_NODE;
		m_ParseNode_Info.m_DataType = CPN_STRING;
    }

    virtual ~SP_CPN_Parse_Argument_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();
		wxString l_sLeftString, l_sRightString;

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER)
			l_sLeftString = wxString::Format(wxT("%d"),l_LeftNodeInfo.m_IntegerValue);
		else if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			if(l_LeftNodeInfo.m_BooleanValue == true)
				l_sLeftString = wxT("true");
			else
				l_sLeftString = wxT("false");
		}
		else			
		{
			l_sLeftString = *(l_LeftNodeInfo.m_StringValue);
		}

		if(l_RightNodeInfo.m_DataType == CPN_INTEGER)
			l_sRightString = wxString::Format(wxT("%d"),l_RightNodeInfo.m_IntegerValue);
		else if(l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			if(l_RightNodeInfo.m_BooleanValue == true)
				l_sRightString = wxT("true");
			else
				l_sRightString = wxT("false");
		}
		else			
		{
			l_sRightString = *(l_RightNodeInfo.m_StringValue);
		}		
		
		m_String = l_sLeftString + wxT(",") + l_sRightString ;
		m_ParseNode_Info.m_StringValue = &m_String;
		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("!") + m_pLeft->GenerateExpression();
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{				
		return false;
	}


};


/** parse node dot. **/
class SP_CPN_Parse_Dot_Node : public SP_CPN_ParseNode
{    
private:
	wxString m_String;
	int m_nPosition;
public:
    //explicit SP_CPN_Parse_Dot_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	explicit SP_CPN_Parse_Dot_Node(SP_CPN_ParseNode* p_pcLeft, int p_value)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		//m_pRight = p_pcRight;
		m_nPosition = p_value;

		m_ParseNode_Info.m_NodeType = CPN_DOT_NODE ;
    }

    virtual ~SP_CPN_Parse_Dot_Node()
    {
		delete m_pLeft;
		//delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate();

	virtual wxString GenerateExpression()
	{
		return wxT("") ;
	}

	virtual bool check();	


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}



};

/** parse node index. **/
class SP_CPN_Parse_Index_Node : public SP_CPN_ParseNode
{
private:
	wxString m_String;
    
public:
    explicit SP_CPN_Parse_Index_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_INDEX_NODE ;
		m_ParseNode_Info.m_DataType = CPN_INDEX;
    }

    virtual ~SP_CPN_Parse_Index_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		m_pLeft->evaluate();
		m_pRight->evaluate();
		SP_CPN_ParseNode_Info* l_LeftNodeInfo =  m_pLeft->GetParseNodeInfo();
		SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

		if (m_ParseNode_Info.m_bPlaceFlag == true)
		{
			wxString l_sColorValue;
			if (l_RightNodeInfo->m_DataType == CPN_INTEGER)
			{
				l_sColorValue = wxString::Format(wxT("%d"), l_RightNodeInfo->m_IntegerValue);
			}

			if (l_RightNodeInfo->m_DataType == CPN_BOOLEAN)
			{
				if (l_RightNodeInfo->m_BooleanValue)
					l_sColorValue = wxT("true");
				else
					l_sColorValue = wxT("false");
			}

			if (l_RightNodeInfo->m_DataType == CPN_STRING ||
				l_RightNodeInfo->m_DataType == CPN_INDEX ||
				l_RightNodeInfo->m_DataType == CPN_DOT ||
				l_RightNodeInfo->m_DataType == CPN_PRODUCT ||
				l_RightNodeInfo->m_DataType == CPN_ENUM)
			{				
				l_sColorValue = *(l_RightNodeInfo->m_StringValue);

				l_sColorValue.Replace(wxT("("), wxT("_"));
				l_sColorValue.Replace(wxT(")"), wxT("_"));
				l_sColorValue.Replace(wxT(","), wxT("_"));
			}

			wxString l_sPlaceName = l_LeftNodeInfo->m_sColoredPlaceName;
			l_sPlaceName = l_sPlaceName + wxT("_") + l_sColorValue; // get an uncolored place name
			m_ParseNode_Info.m_stringMultiplicity = l_sPlaceName;
		}


		if (l_LeftNodeInfo->m_NodeType == CPN_INDEXNAME_NODE)
		{
			wxString l_LeftString = *(l_LeftNodeInfo->m_StringValue);
			int l_nRight = l_RightNodeInfo->m_IntegerValue;
			wxString l_RightString = wxString::Format(wxT("%d"), l_nRight);
			m_String = l_LeftString + wxT("[") + l_RightString + wxT("]");

			m_ParseNode_Info.m_StringValue = &m_String;

			CollectResult();
		}

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("") ;
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{	
		return false;
	}


};



/** parse node adding two operand nodes. */
class SP_CPN_Parse_Add_Node : public SP_CPN_ParseNode
{
	private:
	wxString m_String;
    
public:
    explicit SP_CPN_Parse_Add_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_ADD_NODE ;		
    }

    virtual ~SP_CPN_Parse_Add_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if (!m_ParseNode_Info.m_bSeparaterExpression)
		{
			if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
			{
				m_ParseNode_Info.m_IntegerValue = l_LeftNodeInfo.m_IntegerValue + l_RightNodeInfo.m_IntegerValue;
			}
			if (l_LeftNodeInfo.m_DataType == CPN_STRING && l_RightNodeInfo.m_DataType == CPN_STRING)
			{
				wxString l_LeftString = *(l_LeftNodeInfo.m_StringValue);
				l_LeftString = l_LeftString.BeforeLast('\"');
				wxString l_RightString = *(l_RightNodeInfo.m_StringValue);
				l_RightString = l_RightString.AfterFirst('\"');

				m_String = l_LeftString + l_RightString;
				m_ParseNode_Info.m_StringValue = &m_String;
			}
			CollectResult();
		}
		else
		{
			//for marking-dependent arcs -- multiplicity
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{					
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("+") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("+") + wxString::Format(wxT("%f"), l_RightNodeInfo.m_DoubleMultiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%f"), l_LeftNodeInfo.m_DoubleMultiplicity) + wxT("+") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_DoubleMultiplicity + l_RightNodeInfo.m_DoubleMultiplicity;
				}
			}
			else
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("+") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("+") + wxString::Format(wxT("%d"), l_RightNodeInfo.m_Multiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%d"), l_LeftNodeInfo.m_Multiplicity) + wxT("+") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_Multiplicity = l_LeftNodeInfo.m_Multiplicity + l_RightNodeInfo.m_Multiplicity;
				}
			}
		}

		return m_ParseNode_Info;
	}

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr + l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}
	

};


//=================================
/** parse node numOf two operand nodes. */ // computes how many token of a certain color (right-hand operand) are exist in a certain place (right-hand operand)
class SP_CPN_Parse_NumOf_Node : public SP_CPN_ParseNode
{
public:

	explicit SP_CPN_Parse_NumOf_Node(SP_CPN_ParseNode* p_pLeft, SP_CPN_ParseNode* p_pcRight)
		: SP_CPN_ParseNode()
	{
		m_pLeft = p_pLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_NUMOF_NODE;
	}

	virtual ~SP_CPN_Parse_NumOf_Node()
	{
		delete m_pLeft;
		delete m_pRight;
	}

	virtual wxString eval(wxString colour, wxString l_sTokenNUM, SP_DS_Node* node);

	virtual SP_CPN_ParseNode_Info evaluate()
		{

			SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
			SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();//->evaluate();

			m_ParseNode_Info.m_DataType = l_RightNodeInfo->m_DataType;
			m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

			if (!m_ParseNode_Info.m_bSeparaterExpression)
			{
				if (l_LeftNodeInfo.m_DataType == CPN_INTEGER)
				{
					if (this->m_pcGraph)//for numOf
					{
						SP_VectorString l_vPlaceTypes = { SP_DS_DISCRETE_PLACE ,SP_DS_CONTINUOUS_PLACE };
						for (auto itV = l_vPlaceTypes.begin(); itV != l_vPlaceTypes.end(); ++itV)
						{
							SP_DS_Nodeclass* l_pcNodeclass = this->m_pcGraph->GetNodeclass(*itV);
							if (!l_pcNodeclass) continue;
							SP_ListNode::const_iterator l_itElem;
							for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
							{
								SP_DS_Node* l_pcPlaceNode = (*l_itElem);

								if (l_pcNodeclass)
								{
									wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
									if (l_sPlaceName == l_RightNodeInfo->m_sColoredPlaceName)
									{
										SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlaceNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
										if (l_pcColList)
										{

											for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
											{
												wxString l_sColorExpr = l_pcColList->GetCell(i, 0);
												wxString l_sTupeExpression = l_pcColList->GetCell(i, 0 + 2);
												wxString l_sTokenNum = l_pcColList->GetCell(i, 0 + 1);
												wxString l_sColour;
												l_sColour << l_LeftNodeInfo.m_IntegerValue;
												if (l_sColorExpr == l_sColour)
												{
													if (*itV == SP_DS_DISCRETE_PLACE)
													{
														long l_nvalue;
														if (l_sTokenNum.ToLong(&l_nvalue))
														{
															m_ParseNode_Info.m_IntegerValue = l_nvalue;
															m_ParseNode_Info.m_DataType = CPN_INTEGER;
															break;
														}
													}
													else
													{//cont place
														double l_dvalue;
														if (l_sTokenNum.ToDouble(&l_dvalue))
														{
															m_ParseNode_Info.m_DoubleValue = l_dvalue;
															m_ParseNode_Info.m_DoubleMultiplicity = l_dvalue;

															break;
														}
													}
												}
												else if (l_sColorExpr == wxT("all()"))
												{
													wxString l_sVal = this->eval(l_sColour, l_sTokenNum, l_pcPlaceNode);

													if (*itV == SP_DS_DISCRETE_PLACE)
													{
														long l_nvalue;

														if (l_sVal.ToLong(&l_nvalue))
														{
															m_ParseNode_Info.m_IntegerValue = l_nvalue;
															break;
														}
													}
													else
													{
														double l_dvalue;

														if (l_sVal.ToDouble(&l_dvalue))
														{
															m_ParseNode_Info.m_DoubleValue = l_dvalue;
															m_ParseNode_Info.m_DoubleMultiplicity = l_dvalue;
															break;
														}
													}

												}
											}
										}
									}
								}
							}
						}
					}
				}
				else if (l_LeftNodeInfo.m_DataType == CPN_PRODUCT)
				{

					if (this->m_pcGraph)//for numOf
					{
						SP_VectorString l_vPlaceTypes = { SP_DS_DISCRETE_PLACE ,SP_DS_CONTINUOUS_PLACE };
						for (auto itV = l_vPlaceTypes.begin(); itV != l_vPlaceTypes.end(); ++itV)
						{
							SP_DS_Nodeclass* l_pcNodeclass = this->m_pcGraph->GetNodeclass(*itV);
							if (!l_pcNodeclass) continue;
							SP_ListNode::const_iterator l_itElem;
							for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
							{
								SP_DS_Node* l_pcPlaceNode = (*l_itElem);

								if (l_pcNodeclass)
								{
									wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
									if (l_sPlaceName == l_RightNodeInfo->m_sColoredPlaceName)
									{
										SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlaceNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
										if (l_pcColList)
										{

											for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
											{
												wxString l_sColorExpr = l_pcColList->GetCell(i, 0);
												wxString l_sTupeExpression = l_pcColList->GetCell(i, 0 + 2);
												wxString l_sTokenNum = l_pcColList->GetCell(i, 0 + 1);

												wxString l_sColour = l_LeftNodeInfo.m_EvalResults[0].m_ColorValue;
											//
												if (l_sColorExpr == l_sColour)
												{
													if (*itV == SP_DS_DISCRETE_PLACE)
													{
														long l_nvalue;
														if (l_sTokenNum.ToLong(&l_nvalue))
														{
															m_ParseNode_Info.m_IntegerValue = l_nvalue;
															break;
														}
													}
													else
													{//cont place
														double l_dvalue;
														if (l_sTokenNum.ToDouble(&l_dvalue))
														{
															m_ParseNode_Info.m_DoubleValue = l_dvalue;
															m_ParseNode_Info.m_DoubleMultiplicity = l_dvalue;

															break;
														}
													}
												}
												else if (l_sColorExpr == wxT("all()"))
												{
													//TODO
													wxString l_sVal = this->eval(l_sColour, l_sTokenNum, l_pcPlaceNode);

													if (*itV == SP_DS_DISCRETE_PLACE)
													{
														long l_nvalue;

														if (l_sVal.ToLong(&l_nvalue))
														{
															m_ParseNode_Info.m_IntegerValue = l_nvalue;
															break;
														}
													}
													else
													{
														double l_dvalue;

														if (l_sVal.ToDouble(&l_dvalue))
														{
															m_ParseNode_Info.m_DoubleValue = l_dvalue;
															m_ParseNode_Info.m_DoubleMultiplicity = l_dvalue;
															break;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

				}
				else
				{
				//TODO?
				}

				CollectResult();
			}
			else
			{

			}


			return m_ParseNode_Info;
		}

	virtual void CollectResult();

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
	{
		if (!m_pLeft->GetConstraints(p_cCSPSolver))
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if (!m_pRight->GetConstraints(p_cCSPSolver))
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr - l_pcRightParseNode_Info->m_IntConstraintExpr);

		return true;
	}


};
//=================================

/** parse node subtracting two operand nodes. */
class SP_CPN_Parse_Substract_Node : public SP_CPN_ParseNode
{
public:

    explicit SP_CPN_Parse_Substract_Node(SP_CPN_ParseNode* p_pLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_SUBSTRACT_NODE ;
    }

    virtual ~SP_CPN_Parse_Substract_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate()
    {

		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if (!m_ParseNode_Info.m_bSeparaterExpression)
		{
			if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
			{
				m_ParseNode_Info.m_IntegerValue = l_LeftNodeInfo.m_IntegerValue - l_RightNodeInfo.m_IntegerValue;
			}

			CollectResult();
		}
		else
		{
			//for marking-dependent arcs -- multiplicity
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("-") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("-") + wxString::Format(wxT("%f"), l_RightNodeInfo.m_DoubleMultiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%f"), l_LeftNodeInfo.m_DoubleMultiplicity) + wxT("-") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_DoubleMultiplicity - l_RightNodeInfo.m_DoubleMultiplicity;
				}
			}
			else
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("-") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("-") + wxString::Format(wxT("%d"), l_RightNodeInfo.m_Multiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%d"), l_LeftNodeInfo.m_Multiplicity) + wxT("-") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_Multiplicity = l_LeftNodeInfo.m_Multiplicity - l_RightNodeInfo.m_Multiplicity;
				}
			}
		}

		return m_ParseNode_Info;


    }
	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();


	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
	{
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr - l_pcRightParseNode_Info->m_IntConstraintExpr);

		return true;
	}


};

/** parse node multiplying two operand nodes. */
class SP_CPN_Parse_Multiply_Node : public SP_CPN_ParseNode
{
    
public:

    explicit SP_CPN_Parse_Multiply_Node(SP_CPN_ParseNode* p_pLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_MULTIPLY_NODE ;	
    }

    virtual ~SP_CPN_Parse_Multiply_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if (!m_ParseNode_Info.m_bSeparaterExpression)
		{
			if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
			{
				m_ParseNode_Info.m_IntegerValue = l_LeftNodeInfo.m_IntegerValue * l_RightNodeInfo.m_IntegerValue;
			}

			CollectResult();
		}
		else
		{
			//for marking-dependent arcs -- multiplicity
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("*") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("*") + wxString::Format(wxT("%f"), l_RightNodeInfo.m_DoubleMultiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%f"), l_LeftNodeInfo.m_DoubleMultiplicity) + wxT("*") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_DoubleMultiplicity * l_RightNodeInfo.m_DoubleMultiplicity;
				}
			}
			else
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("*") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("*") + wxString::Format(wxT("%d"), l_RightNodeInfo.m_Multiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%d"), l_LeftNodeInfo.m_Multiplicity) + wxT("*") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_Multiplicity = l_LeftNodeInfo.m_Multiplicity * l_RightNodeInfo.m_Multiplicity;
				}
			}
		}

		return m_ParseNode_Info;

    }
	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr * l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};

/** parse node dividing two operand nodes. */
class SP_CPN_Parse_Divide_Node : public SP_CPN_ParseNode
{
    
public:

    explicit SP_CPN_Parse_Divide_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_DIVIDE_NODE ;
    }


    virtual ~SP_CPN_Parse_Divide_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {

		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if (!m_ParseNode_Info.m_bSeparaterExpression && l_RightNodeInfo.m_IntegerValue)
		{
			if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
			{
				m_ParseNode_Info.m_IntegerValue = l_LeftNodeInfo.m_IntegerValue / l_RightNodeInfo.m_IntegerValue;
			}

			CollectResult();
		}
		else
		{
			//for marking-dependent arcs -- multiplicity
			if (m_sPlaceType == SP_DS_CONTINUOUS_PLACE)
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("/") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("/") + wxString::Format(wxT("%f"), l_RightNodeInfo.m_DoubleMultiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%f"), l_LeftNodeInfo.m_DoubleMultiplicity) + wxT("/") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_DoubleMultiplicity = l_LeftNodeInfo.m_DoubleMultiplicity / l_RightNodeInfo.m_DoubleMultiplicity;
				}
			}
			else
			{
				if (l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("/") + l_sRightMul;
				}
				else if (l_LeftNodeInfo.m_bPlaceFlag &&  !l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sLeftMul = l_LeftNodeInfo.m_stringMultiplicity;
					if (l_LeftNodeInfo.m_NodeType == CPN_ADD_NODE || l_LeftNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sLeftMul = wxT("(") + l_sLeftMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = l_sLeftMul + wxT("/") + wxString::Format(wxT("%d"), l_RightNodeInfo.m_Multiplicity);
				}
				else if (!l_LeftNodeInfo.m_bPlaceFlag &&  l_RightNodeInfo.m_bPlaceFlag)
				{
					wxString l_sRightMul = l_RightNodeInfo.m_stringMultiplicity;
					if (l_RightNodeInfo.m_NodeType == CPN_ADD_NODE || l_RightNodeInfo.m_NodeType == CPN_SUBSTRACT_NODE)
					{
						l_sRightMul = wxT("(") + l_sRightMul + wxT(")");
					}
					m_ParseNode_Info.m_stringMultiplicity = wxString::Format(wxT("%d"), l_LeftNodeInfo.m_Multiplicity) + wxT("/") + l_sRightMul;
				}
				else
				{
					m_ParseNode_Info.m_Multiplicity = l_LeftNodeInfo.m_Multiplicity / l_RightNodeInfo.m_Multiplicity;
				}
			}
		}

		return m_ParseNode_Info;

    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr / l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};

/** parse node calculating the remainder of an integer division of two
 * operand nodes. */
class SP_CPN_Parse_Modulo_Node : public SP_CPN_ParseNode
{
    
public:

    explicit SP_CPN_Parse_Modulo_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_MODULOS_NODE ;
    }

    virtual ~SP_CPN_Parse_Modulo_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_IntegerValue = l_LeftNodeInfo.m_IntegerValue % l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr % l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};


/** parse node calculating the power two
 * operand nodes. */
class SP_CPN_Parse_Power_Node : public SP_CPN_ParseNode
{
    
public:

    explicit SP_CPN_Parse_Power_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_POWER_NODE ;
    }

    virtual ~SP_CPN_Parse_Power_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_DataType = l_LeftNodeInfo.m_DataType;
		m_ParseNode_Info.m_ColorSet = l_LeftNodeInfo.m_ColorSet;

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_IntegerValue = (int)(std::pow((float)(l_LeftNodeInfo.m_IntegerValue),l_RightNodeInfo.m_IntegerValue));
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{	
		/*
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_IntConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr % l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
		*/
		return false;
	}

};


/** parse node equal. **/

class SP_CPN_Parse_Equal_Node : public SP_CPN_ParseNode
{
    
public:

    explicit SP_CPN_Parse_Equal_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_EQUAL_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_Equal_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();
		
		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue == l_RightNodeInfo.m_IntegerValue;
		}
		else if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN && l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_BooleanValue == l_RightNodeInfo.m_BooleanValue;
		}
		else if (l_LeftNodeInfo.m_DataType == CPN_DOUBLE && l_RightNodeInfo.m_DataType == CPN_DOUBLE)
		{//for numOf by george
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_DoubleValue == l_RightNodeInfo.m_DoubleValue;
		}
		else if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_DOUBLE && l_RightNodeInfo.m_NodeType==CPN_NUMOF_NODE)
		{//for numOf, case of ColHPN , testing two different kinds of markings by george
			int l_nRoundRight = round(l_RightNodeInfo.m_DoubleValue);

			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue == l_nRoundRight;
		}
		else if (l_LeftNodeInfo.m_DataType == CPN_DOUBLE && l_RightNodeInfo.m_DataType == CPN_INTEGER  && l_RightNodeInfo.m_NodeType == CPN_NUMOF_NODE)
		{//for numOf, case of ColHPN , testing two different kinds of markings by george
			int l_nRoundLeft = round(l_LeftNodeInfo.m_DoubleValue);

			m_ParseNode_Info.m_BooleanValue = l_nRoundLeft == l_RightNodeInfo.m_IntegerValue;
		}
		else
		{
			wxString l_LeftString = *(l_LeftNodeInfo.m_StringValue);
			wxString l_RightString = *(l_RightNodeInfo.m_StringValue);
			m_ParseNode_Info.m_BooleanValue = l_LeftString == l_RightString;
		}		

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();   

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr == l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};



/** parse node not equal. */

class SP_CPN_Parse_NotEqual_Node : public SP_CPN_ParseNode
{
    
public:
    explicit SP_CPN_Parse_NotEqual_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_NOTEQUAL_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_NotEqual_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue != l_RightNodeInfo.m_IntegerValue;
		}
		else if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN && l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_BooleanValue != l_RightNodeInfo.m_BooleanValue;
		}
		else
		{
			wxString l_LeftString = *(l_LeftNodeInfo.m_StringValue);
			wxString l_RightString = *(l_RightNodeInfo.m_StringValue);
			m_ParseNode_Info.m_BooleanValue = l_LeftString != l_RightString;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr != l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};


/** parse node greater than. */

class SP_CPN_Parse_GreaterThan_Node : public SP_CPN_ParseNode
{
    
public:
    explicit SP_CPN_Parse_GreaterThan_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_GREATTHAN_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_GreaterThan_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue > l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();  

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr > l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};

/** parse node Greater equal. */

class SP_CPN_Parse_GreaterEqual_Node : public SP_CPN_ParseNode
{
    
public:
    explicit SP_CPN_Parse_GreaterEqual_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_GREATTHANEQUAL_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_GreaterEqual_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {		
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue >= l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr >= l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};
////////////////////////////////////////////////
/** parse node elemOf . */

class SP_CPN_Parse_Element_Of_Node : public SP_CPN_ParseNode
{

public:
	explicit SP_CPN_Parse_Element_Of_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
		: SP_CPN_ParseNode()
	{
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_ELEMENT_OF_NODE;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
		 
	}

	virtual ~SP_CPN_Parse_Element_Of_Node()
	{
		delete m_pLeft;
		delete m_pRight;
	}

	virtual SP_CPN_ParseNode_Info evaluate()
	{

		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if (l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue == l_RightNodeInfo.m_IntegerValue;
		}

		if (l_LeftNodeInfo.m_DataType != CPN_PRODUCT /*&& l_RightNodeInfo.m_DataType == CPN_ENUM*/)
		{
			
			bool l_bResult = false;
			wxString l_sRightOperand = *(l_RightNodeInfo.m_StringValue);
			l_sRightOperand.Replace(wxT(" "), wxT(""));

			wxString l_sCSName = (l_RightNodeInfo.m_ColorSet);
			wxString l_sLeftOperand;
			if (l_LeftNodeInfo.m_DataType == CPN_ENUM || l_LeftNodeInfo.m_DataType == CPN_STRING)
			{
				l_sLeftOperand = *(l_LeftNodeInfo.m_StringValue);
				l_sRightOperand = l_sRightOperand.Trim();
			}
			else if (l_LeftNodeInfo.m_DataType == CPN_INTEGER)
			{
				int val = l_LeftNodeInfo.m_IntegerValue;
				l_sLeftOperand << val;
			}
				 
		

			std::vector<wxString> l_sVColors;

			if (!DoEval(l_sCSName, l_sVColors))
			{
				m_ParseNode_Info.m_BooleanValue = false;
				return m_ParseNode_Info;
			}

			for (auto color : l_sVColors)
			{

				if (color.IsSameAs(l_sLeftOperand))
				{
					l_bResult = true;
					break;
				}
			}

			m_ParseNode_Info.m_BooleanValue = l_bResult;
		}

		//left operand is a tupel and the right operand is a product colour set
		if (l_LeftNodeInfo.m_DataType == CPN_PRODUCT && l_RightNodeInfo.m_DataType == CPN_PRODUCT)
		{
			bool l_bResult = true;
			wxString l_sRightOperand = *(l_RightNodeInfo.m_StringValue);
			l_bResult= l_LeftNodeInfo.m_StringValue->IsSameAs(l_sRightOperand);
		 
			wxString l_sLeftOperand = *(l_LeftNodeInfo.m_StringValue);
			l_sRightOperand.Replace(wxT(" "), wxT(""));
			l_sRightOperand=l_sRightOperand.Trim();

			std::vector<wxString> l_sVColors;

			wxString l_sCSName = (l_RightNodeInfo.m_ColorSet);

			if (!DoEval(l_sCSName, l_sVColors))
			{
				m_ParseNode_Info.m_BooleanValue = false;
				return m_ParseNode_Info;
			}
			 
			//check the membership
			for (auto color : l_sVColors)
			{
				if (color.IsSameAs(l_sLeftOperand) )
				{
					l_bResult = true;
					break;
				}
			}

			m_ParseNode_Info.m_BooleanValue = l_bResult;
			/**
			std::vector<wxString> l_vRightVector;
			std::vector<wxString> l_vLeftVector;

			
			wxStringTokenizer tokenizer(l_sRightOperand, "(,)");
			while (tokenizer.HasMoreTokens())
			{
				wxString token = tokenizer.GetNextToken();
				if(!token.IsEmpty())
				l_vRightVector.push_back(token);
			}

			wxStringTokenizer tokenizer1(l_sLeftOperand, "(,)");
			while (tokenizer1.HasMoreTokens())
			{
				wxString token = tokenizer1.GetNextToken();
				if (!token.IsEmpty())
					l_vLeftVector.push_back(token);
			}
			bool l_bRes = true;
			if (l_vLeftVector.size() == l_vRightVector.size())
			{
				for (int i = 0; i < l_vLeftVector.size(); i++)
				{
					if (l_vRightVector[i] != l_vLeftVector[i])
					{
						l_bRes = false;
						break;
					}
				}
			}
			else
			{
				l_bRes = false;
			}

			m_ParseNode_Info.m_BooleanValue = l_bRes;
			*/
			/**
			bool uniqueElt = true;
			if (l_vLeftVector.size() == l_vRightVector.size())
			{
				 
				wxString firstItem = *l_vLeftVector.begin();
				for (std::vector<wxString>::const_iterator it = l_vLeftVector.begin() + 1; it != l_vLeftVector.end(); ++it) {
					if (*it != firstItem) {
						uniqueElt = false;
						break;
					}
				}

				if (uniqueElt)
				{
					wxString firstItem = *l_vRightVector.begin();
					for (std::vector<wxString>::const_iterator it = l_vRightVector.begin() + 1; it != l_vRightVector.end(); ++it) {
						if (*it != firstItem) {
							uniqueElt = false;
							break;
						}
					}
				}
			}
			m_ParseNode_Info.m_BooleanValue = uniqueElt;

			if (l_vLeftVector.size() != l_vRightVector.size())
			{
				m_ParseNode_Info.m_BooleanValue = false;
			}
			*/
		}
		CollectResult();

		return m_ParseNode_Info;
    }
	bool DoEval(wxString p_sColorSet, std::vector<wxString>& p_vColors);
  
	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver);
 

};




////////////////////////////////////////////////
/** parse node less than. */

class SP_CPN_Parse_LessThan_Node : public SP_CPN_ParseNode
{

public:
    explicit SP_CPN_Parse_LessThan_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_LESSTHAN_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_LessThan_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue < l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr < l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};


/** parse node less equal. */

class SP_CPN_Parse_LessEqual_Node : public SP_CPN_ParseNode
{    
   
public:
    explicit SP_CPN_Parse_LessEqual_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_LESSTHANEQUAL_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_LessEqual_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER && l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_IntegerValue <= l_RightNodeInfo.m_IntegerValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_IntConstraintExpr <= l_pcRightParseNode_Info->m_IntConstraintExpr);		
		
		return true;
	}

};



/** parse node calculating logcial And of two operand nodes. */

class SP_CPN_Parse_And_Node : public SP_CPN_ParseNode
{
      
public:
    explicit SP_CPN_Parse_And_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_AND_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_And_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN && l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_BooleanValue && l_RightNodeInfo.m_BooleanValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();

		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_BoolConstraintExpr && l_pcRightParseNode_Info->m_BoolConstraintExpr);		
		
		return true;
	}

};


/** parse node calculating logcial or of two operand nodes. */

class SP_CPN_Parse_Or_Node : public SP_CPN_ParseNode
{
    
public:
    explicit SP_CPN_Parse_Or_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_OR_NODE ;
		m_ParseNode_Info.m_DataType = CPN_BOOLEAN;

		m_ParseNode_Info.m_sColorSetList = wxT("");
		m_ParseNode_Info.m_sColorSetList << m_ParseNode_Info.m_DataType;
    }

    virtual ~SP_CPN_Parse_Or_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN && l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			m_ParseNode_Info.m_BooleanValue = l_LeftNodeInfo.m_BooleanValue || l_RightNodeInfo.m_BooleanValue;
		}

		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return wxT("");
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if( ! m_pLeft->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcLeftParseNode_Info = m_pLeft->GetParseNodeInfo();

		if( ! m_pRight->GetConstraints(p_cCSPSolver) )
			return false;
		SP_CPN_ParseNode_Info* l_pcRightParseNode_Info = m_pRight->GetParseNodeInfo();
		m_ParseNode_Info.m_BoolConstraintExpr = expr(p_cCSPSolver, l_pcLeftParseNode_Info->m_BoolConstraintExpr || l_pcRightParseNode_Info->m_BoolConstraintExpr);		
		
		return true;
	}

};


/** parse node calculating comma of two operand nodes. */

class SP_CPN_Parse_Comma_Node : public SP_CPN_ParseNode
{

private:
	wxString m_String; // Store the string for the m_ParseNode_Info.m_StringValue

public:
    explicit SP_CPN_Parse_Comma_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_COMMA_NODE ;
		m_ParseNode_Info.m_DataType = CPN_PRODUCT;
    }

    virtual ~SP_CPN_Parse_Comma_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }


    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();
		m_ParseNode_Info.m_EvalResults.clear();
		wxString l_sLeftString;
		wxString l_sRightString;
		wxString l_sTotal;
		bool l_bIsAllExist = false;
		IsAllNodeExists(m_pLeft, l_bIsAllExist);
		if (!l_bIsAllExist)
		{
			IsAllNodeExists(m_pRight, l_bIsAllExist);
		}
		if (l_LeftNodeInfo.m_NodeType == CPN_CONNECTOR_NODE|| l_LeftNodeInfo.m_NodeType == CPN_ALLFUNC)//by george
		{//george
			 vector<SP_CPN_EvaluatedSingleValue> l_vRes;
			for (auto it = l_RightNodeInfo.m_EvalResults.begin(); it != l_RightNodeInfo.m_EvalResults.end(); ++it)
			{
				l_sRightString = (*it).m_ColorValue;
			}
			for (auto it = l_LeftNodeInfo.m_EvalResults.begin(); it != l_LeftNodeInfo.m_EvalResults.end(); ++it)
			{
				SP_CPN_EvaluatedSingleValue l_colour;
				wxString l_sTupel;
				l_sLeftString = (*it).m_ColorValue;
				if ((it)->m_Multiplicity == 0) continue;
				l_sTotal << wxT("(") << l_sLeftString << wxT(",") << l_sRightString << wxT(")")<<wxT("|");

				l_sTupel = l_sTotal.BeforeLast(wxChar('|'));
				wxString color;
				color<<wxT("(") << l_sLeftString << wxT(",") << l_sRightString << wxT(")");
				l_colour.m_ColorValue = color;
				l_colour.m_Multiplicity = (*it).m_Multiplicity;
				l_colour.m_DoubleMultiplicity = (*it).m_DoubleMultiplicity;
			//	l_colour.m_Multiplicity = (*it).m_stringMultiplicity;
				l_colour.m_bPlaceFlag = (*it).m_bPlaceFlag;
				l_colour.m_stringMultiplicity = (*it).m_stringMultiplicity;
				l_colour.m_Predicate = true;
				m_ParseNode_Info.m_EvalResults.push_back(l_colour);
			}
			//l_sTotal= l_sTotal.BeforeLA
			m_String = l_sTotal.BeforeLast(wxChar('|'));
			//m_ParseNode_Info.m_StringValue = &m_String.BeforeLast(wxChar('|'));
			m_ParseNode_Info.m_StringValue = &m_String;
			return m_ParseNode_Info;
		}

		if (l_bIsAllExist)//by george
		{
			bool l_bAllLeft = false;
			if (m_pLeft->GetLeftNode())
			{
				if(m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType==CPN_ALLFUNC)
				{
					l_bAllLeft = true;
				}
			}
			if (l_LeftNodeInfo.m_NodeType == CPN_COMMA_NODE && l_RightNodeInfo.m_NodeType != CPN_ALLFUNC)
			{

				vector<SP_CPN_EvaluatedSingleValue> l_vRes;
				for (auto it = l_RightNodeInfo.m_EvalResults.begin(); it != l_RightNodeInfo.m_EvalResults.end(); ++it)
				{
					l_sRightString = (*it).m_ColorValue;
				}

				if (l_sRightString.IsEmpty())
				{
					l_sRightString = *(l_RightNodeInfo.m_StringValue);
				}
				for (auto it = l_LeftNodeInfo.m_EvalResults.begin(); it != l_LeftNodeInfo.m_EvalResults.end(); ++it)
				{
					SP_CPN_EvaluatedSingleValue l_colour;
					wxString l_sTupel;
					l_sLeftString = (*it).m_ColorValue;
					if ((it)->m_Multiplicity == 0) continue;
					l_sTotal  << l_sLeftString << wxT(",") << l_sRightString  << wxT("|");

					l_sTupel = l_sTotal.BeforeLast(wxChar('|'));
					wxString color;
					color  << wxT("(")<<l_sLeftString << wxT(",") << l_sRightString <<wxT(")");
					l_colour.m_ColorValue = color;
					l_colour.m_Multiplicity = (*it).m_Multiplicity;
					l_colour.m_DoubleMultiplicity = (*it).m_DoubleMultiplicity;
					//	l_colour.m_Multiplicity = (*it).m_stringMultiplicity;
					l_colour.m_bPlaceFlag = (*it).m_bPlaceFlag;
					l_colour.m_stringMultiplicity = (*it).m_stringMultiplicity;
					l_colour.m_Predicate = true;
					m_ParseNode_Info.m_EvalResults.push_back(l_colour);
				}
			}
			else if (l_LeftNodeInfo.m_NodeType != CPN_ALLFUNC&&l_bIsAllExist&& l_RightNodeInfo.m_NodeType == CPN_ALLFUNC)
			{
				vector<SP_CPN_EvaluatedSingleValue> l_vRes;
				for (auto it = l_LeftNodeInfo.m_EvalResults.begin(); it != l_LeftNodeInfo.m_EvalResults.end(); ++it)
				{
					l_sLeftString = (*it).m_ColorValue;
				}
				if (l_sLeftString.IsEmpty())
				{
					l_sLeftString = *(l_LeftNodeInfo.m_StringValue);
				}

				for (auto it = l_RightNodeInfo.m_EvalResults.begin(); it != l_RightNodeInfo.m_EvalResults.end(); ++it)
				{
					SP_CPN_EvaluatedSingleValue l_colour;
					wxString l_sTupel;
					l_sRightString = (*it).m_ColorValue;
					if ((it)->m_Multiplicity == 0) continue;
					l_sTotal << l_sLeftString << wxT(",") << l_sRightString << wxT("|");

					l_sTupel = l_sTotal.BeforeLast(wxChar('|'));
					wxString color;
					color << wxT("(")<<l_sLeftString << wxT(",") << l_sRightString<<wxT(")");
					l_colour.m_ColorValue = color;
					l_colour.m_Multiplicity = (*it).m_Multiplicity;
					l_colour.m_DoubleMultiplicity = (*it).m_DoubleMultiplicity;
					//	l_colour.m_Multiplicity = (*it).m_stringMultiplicity;
					l_colour.m_bPlaceFlag = (*it).m_bPlaceFlag;
					l_colour.m_stringMultiplicity = (*it).m_stringMultiplicity;
					l_colour.m_Predicate = true;
					m_ParseNode_Info.m_EvalResults.push_back(l_colour);
				}
			}
			m_String = l_sTotal.BeforeLast(wxChar('|'));;
			//m_ParseNode_Info.m_StringValue = &m_String.BeforeLast(wxChar('|'));
			m_ParseNode_Info.m_StringValue = &m_String;
			return m_ParseNode_Info;
		}
		 

		if(l_LeftNodeInfo.m_DataType == CPN_INTEGER)
		{
			l_sLeftString = wxString::Format(wxT("%d"),l_LeftNodeInfo.m_IntegerValue);
		}
		else if(l_LeftNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			if(l_LeftNodeInfo.m_BooleanValue == true)
				l_sLeftString = wxT("true");
			else
				l_sLeftString = wxT("false");
		}
		else			
		{
			l_sLeftString = *(l_LeftNodeInfo.m_StringValue);
		}


		if(l_RightNodeInfo.m_DataType == CPN_INTEGER)
		{
			l_sRightString = wxString::Format(wxT("%d"),l_RightNodeInfo.m_IntegerValue);
		}
		else if(l_RightNodeInfo.m_DataType == CPN_BOOLEAN)
		{
			if(l_RightNodeInfo.m_BooleanValue == true)
				l_sRightString = wxT("true");
			else
				l_sRightString = wxT("false");
		}
		else			
		{
			l_sRightString = *(l_RightNodeInfo.m_StringValue);
		}		
		
		m_String = l_sLeftString + wxT(",") + l_sRightString ;
		m_ParseNode_Info.m_StringValue = &m_String;

		return m_ParseNode_Info;
    }


	virtual wxString GenerateExpression()
	{
		return m_pLeft->GenerateExpression() +  wxT(",") + m_pRight->GenerateExpression();
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{		
		if (!this->GetParseNodeInfo()->m_bIsElemOf)
			return false;

		if (!m_pLeft->GetConstraints(p_cCSPSolver))
			return false;
		SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();

		if (!m_pRight->GetConstraints(p_cCSPSolver))
			return false;
		SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

		 
		m_ParseNode_Info.m_vIntconstraintVector->push_back(l_LeftNodeInfo->m_IntConstraintExpr);
		m_ParseNode_Info.m_vIntconstraintVector->push_back(l_RightNodeInfo->m_IntConstraintExpr);
		return true;
	}


};


/** parse node "( )". **/
class SP_CPN_Parse_Bracket_Node : public SP_CPN_ParseNode
{
private:
	wxString m_String;

public:
    explicit SP_CPN_Parse_Bracket_Node(SP_CPN_ParseNode* p_pcNode)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcNode;
		m_pRight = 0;
		m_ParseNode_Info.m_NodeType  = CPN_BRACKET_NODE ;
		m_ParseNode_Info.m_DataType = CPN_PRODUCT;
    }

    virtual ~SP_CPN_Parse_Bracket_Node()
    {
		delete m_pLeft;
    } 

	virtual SP_CPN_ParseNode_Info evaluate()
	{
	 
	  if (m_pLeft->GetLeftNode())//george 19.12
	  {
		  if (m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_CONNECTOR_NODE
			  || m_pLeft->GetLeftNode()->GetParseNodeInfo()->m_NodeType == CPN_ALLFUNC)//george 19.12

		  {
			  SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
			  //m_ParseNode_Info.m_EvalResults = l_LeftNodeInfo.m_EvalResults;
			  //m_String = wxT("(") + *(l_LeftNodeInfo.m_StringValue) + wxT(")");
			  //m_ParseNode_Info.m_StringValue = &m_String;
			  CollectResult();

			  return m_ParseNode_Info;
		  }
	  }

	  SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
	 
	   
		m_String = wxT("(") + *(l_LeftNodeInfo.m_StringValue) + wxT(")");
		m_ParseNode_Info.m_StringValue = &m_String;
	   
		CollectResult();

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return  wxT("(") + m_pLeft->GenerateExpression() +  wxT(")") ;
	}

	virtual bool check();

	bool  IsAllExist(SP_CPN_ParseNode* p_node, wxString p_cs);
	void  TraversAll(SP_CPN_ParseNode* p_node, SP_VectorString l_vComponents, std::vector<int>& p_vIndex);
	void  IsAllExistsInBrackets(SP_CPN_ParseNode* p_node,bool & P_isFound);
	//bool IsAllExist(SP_CPN_ParseNode*,SP_VectorString p_vComponentcs, int pos, unsigned int& pos1);
	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		if (!this->GetParseNodeInfo()->m_bIsElemOf)
			return false;
		m_pLeft->GetParseNodeInfo()->m_bIsElemOf = true;//set comma node elemOf flag

		if (!m_pLeft->GetConstraints(p_cCSPSolver))
			return false;
		m_ParseNode_Info.m_vIntconstraintVector->push_back( m_pLeft->GetParseNodeInfo()->m_vIntconstraintVector->at(0));
		m_ParseNode_Info.m_vIntconstraintVector->push_back(m_pLeft->GetParseNodeInfo()->m_vIntconstraintVector->at(1));

		return true;
	}

	SP_VectorString  ComputeTupeles()
	{
		SP_VectorString l_vTupels;
		return l_vTupels;

	}

};


/** parse node calculating Seperator of two operand nodes. */

class SP_CPN_Parse_Seperator_Node : public SP_CPN_ParseNode
{

private:
	wxString m_String;
    
public:
    explicit SP_CPN_Parse_Seperator_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;
		m_ParseNode_Info.m_NodeType = CPN_SEPERATOR_NODE ;		
    }

    virtual ~SP_CPN_Parse_Seperator_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() ;
    

	virtual wxString GenerateExpression()
	{
		return  wxT("")  ;
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}

};


/** parse node calculating predicate of two operand nodes. */

class SP_CPN_Parse_Predicate_Node : public SP_CPN_ParseNode
{
private:
	wxString m_String;

public:
    explicit SP_CPN_Parse_Predicate_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_PREDICATE_NODE ;
    }

    virtual ~SP_CPN_Parse_Predicate_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

    virtual SP_CPN_ParseNode_Info evaluate() ;

	virtual wxString GenerateExpression()
	{
		return  wxT("")  ;
	}

	virtual bool check();

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}




};



/** parse node calculating CNN of two operand nodes. */

class SP_CPN_Parse_CNN_Node : public SP_CPN_ParseNode
{
    
public:
    explicit SP_CPN_Parse_CNN_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
	: SP_CPN_ParseNode()
    {
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_CONNECTOR_NODE ;
    }

    virtual ~SP_CPN_Parse_CNN_Node()
    {
		delete m_pLeft;
		delete m_pRight;
    }

	void SetEvalResult(SP_CPN_EvaluatedSingleValue p_SingleEvalResult);

    virtual SP_CPN_ParseNode_Info evaluate() 
    {
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_EvalResults.clear();

		for(unsigned i = 0; i < l_LeftNodeInfo.m_EvalResults.size(); i++ )
		{			
			SetEvalResult( l_LeftNodeInfo.m_EvalResults[i] );
		}
		for(unsigned i = 0; i < l_RightNodeInfo.m_EvalResults.size(); i++ )
		{			
			SetEvalResult( l_RightNodeInfo.m_EvalResults[i] );
		}

		return m_ParseNode_Info;
    }

	virtual wxString GenerateExpression()
	{
		return  wxT("")  ;
	}

	virtual bool check()
	{
		if(!m_pLeft->check())
			return false;
		if(!m_pRight->check())
			return false;	

		SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
		SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

		if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
		{
			m_ParseNode_Info.m_bPlaceFlag = true;
		}

		return true;
	}  

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver) 
	{			
		return false;
	}


};





/** parse node calculating CDD of two operand nodes on 12.2020 by george. */

class SP_CPN_Parse_CDD_Node : public SP_CPN_ParseNode
{

public:
	explicit SP_CPN_Parse_CDD_Node(SP_CPN_ParseNode* p_pcLeft, SP_CPN_ParseNode* p_pcRight)
		: SP_CPN_ParseNode()
	{
		m_pLeft = p_pcLeft;
		m_pRight = p_pcRight;

		m_ParseNode_Info.m_NodeType = CPN_CONNECTOR_NODE;
	}

	virtual ~SP_CPN_Parse_CDD_Node()
	{
		delete m_pLeft;
		delete m_pRight;
	}

	void SetEvalResult(SP_CPN_EvaluatedSingleValue p_SingleEvalResult, const bool& p_bIsRight=false);

	virtual SP_CPN_ParseNode_Info evaluate()
	{
		SP_CPN_ParseNode_Info l_LeftNodeInfo = m_pLeft->evaluate();
		SP_CPN_ParseNode_Info l_RightNodeInfo = m_pRight->evaluate();

		m_ParseNode_Info.m_EvalResults.clear();

		for (unsigned i = 0; i < l_LeftNodeInfo.m_EvalResults.size(); i++)
		{
			SetEvalResult(l_LeftNodeInfo.m_EvalResults[i]);
		}
		for (unsigned i = 0; i < l_RightNodeInfo.m_EvalResults.size(); i++)
		{
			SetEvalResult(l_RightNodeInfo.m_EvalResults[i],true);
		}

		return m_ParseNode_Info;
	}

	virtual wxString GenerateExpression()
	{
		return  wxT("");
	}

	virtual bool check()
	{
		if (!m_pLeft->check())
			return false;
		if (!m_pRight->check())
			return false;

		SP_CPN_ParseNode_Info* l_LeftNodeInfo = m_pLeft->GetParseNodeInfo();
		SP_CPN_ParseNode_Info* l_RightNodeInfo = m_pRight->GetParseNodeInfo();

		if (l_LeftNodeInfo->m_bPlaceFlag == true || l_RightNodeInfo->m_bPlaceFlag == true)
		{
			m_ParseNode_Info.m_bPlaceFlag = true;
		}

		return true;
	}

	virtual bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver)
	{
		return false;
	}


};



/** parse context used to save the parsed expressions. This context is
 * passed along to the example::Driver class and fill during parsing via bison
 * actions. */

class SP_CPN_Parse_Context
{

public:    

	SP_CPN_Parse_Context();
	SP_CPN_Parse_Context(SP_CPN_ColorSetClass* p_pColorSetClass, wxString p_ColorSetName);
	~SP_CPN_Parse_Context();     

private:
	SP_CPN_ColorSetClass *m_pColorSetClass;		// point to the colorset class
	wxString m_ColorSetName;					// the name of a color set
	bool m_bFunctionFlag;						// indicate if it is a function or an experession
	wxString m_sFunctionName; 
	wxString m_sNetClassName;					//net class name //will use node class replace
	wxString m_sErrorPosition;
	wxString m_sPlaceType;

	map<wxString, vector<wxString> > *m_psvColored2UnColoredPlaceNames;
	

private:
	void Configuration(SP_CPN_ParseNode* p_pcNode);
	void ConfigurationColorSets(SP_CPN_ParseNode* p_pcNode,SP_CPN_ColorSetClass *p_pColorSetClass);

private:
	SP_MapString2String m_VariableNode;							// for pattern finding
	vector<wxString> m_ConstantNode;							// for pattern finding
	vector<wxString> m_OtherNode;								// for pattern finding
	vector<SP_CPN_EvaluatedSingleValue> m_EvalValue;			// for evaluation	
	wxString m_String;	

public:   
	std::vector<SP_CPN_ParseNode*>	m_ExpressionsVector;		// array of unassigned expressions found by the parser. these are then outputted to the user.

public:
	bool CheckExpression();										//Check one expression once	
	bool EvaluateExpression( vector<SP_CPN_EvaluatedSingleValue> &p_EvaluateResultArray);			//Evaluate one expression only not including checking

	SP_CPN_ExpressionPatternInfo  ExtractPattern();				// Extract the patterns to binding enable test	
	SP_CPN_DATATYPE GetHighLevelNodeDataType();					// for checking the type of a guard, a guard should be boolean	
	wxString GetHighLevelNodeColorSet();

	vector<SP_CPN_Var_Color> GetVariables();					// Get all variables in the expressions, for unfolding not for binding

private:	
	bool AssignDataType(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName);
	bool TraversePattern(SP_CPN_ParseNode* p_Node);																//for EvaluateExpression
	bool TraverseVariables(SP_CPN_ParseNode* p_Node, SP_CPN_ExpressionPatternInfo & p_PatternInfo);				//for EvaluateExpression
	bool TraverseVariablesforUnfolding(SP_CPN_ParseNode* p_Node,vector<SP_CPN_Var_Color> &p_vVariable2Color);	//for GetVariables

	void clearExpressions();    /// free all saved expression trees

	bool TraverseMultiSetTuplePatterns(SP_CPN_ParseNode* p_pcNode, vector<SP_CPN_ParseNode*> &p_vTupleParseNode);

	//bool DecideVariableNodeType(SP_CPN_ParseNode* p_pcNode);
	
public:
	//set functions
	void SetColorSetName(wxString p_ColorSetName);
	void SetColorSetClass( SP_CPN_ColorSetClass* p_pColorSetClass );
	void SetErrorPosition(wxString p_sErrorPosition);
	void SetNetClassName(wxString p_sNetClassName);	
	void SetPlaceType(wxString p_sPlaceType);

	void SetColored2UnColoredPlaceNames(map<wxString, vector<wxString> > *p_psvColored2UnColoredPlaceNames = NULL){ m_psvColored2UnColoredPlaceNames = p_psvColored2UnColoredPlaceNames; }

	void SetFunctionFlag(bool p_bFunctionFlag);
	void SetFunctionName(wxString p_sFunctionName);	
	void ConfigurateParser();



public:
	bool CSPIntTypeTest();

	bool GetConstraints(SP_DS_ColCSPSovler& p_cCSPSolver);

	std::vector<SP_CPN_ParseNode*>*	GetExpressionsVector(){ return &m_ExpressionsVector; }

private:
	bool CSPIntTypeTraverse(SP_CPN_ParseNode* p_pcNode, bool &p_bIntType);

	bool CSPConstraintTraverse(SP_CPN_ParseNode* p_pcNode, SP_DS_ColCSPSovler& p_cCSPSolver);







//unused now for new pattern binding 
public:
	//That will be used in the new binding algorithm
	bool ExtractPatternNew(vector<SP_CPN_ExpressionPatternInfo> &p_ExprPatterns);      // New function
private:
	bool ProcessforPattern(SP_CPN_ParseNode* p_pcParseNode,vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns); //for new function
	bool TraverseTuplePattern(SP_CPN_ParseNode* p_Node, SP_MapString2String &p_mVariableNode,vector<wxString> &p_vConstantNode,vector<wxString> &p_vOtherNode);

	bool TraverseGuardPatterns1(SP_CPN_ParseNode* p_pcParseNode, vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns);
	bool TraverseGuardPatterns2(SP_CPN_ParseNode* p_pcParseNode, vector<SP_CPN_ExpressionPatternInfo> & p_ExprPatterns);	

	//map<wxString,map<wxString, SP_CPN_BindingVariables> > GetSortVariables();
	//void TraverseSortedVariables( SP_CPN_ParseNode* p_pcParseNode, map<wxString,map<wxString, SP_CPN_BindingVariables> > &p_mmVariables);
	//void TraversePredicateValue(SP_CPN_ParseNode* p_pcParseNode, SP_MapString2String &p_mPredicateValue);
	//bool traverse(SP_CPN_ParseNode* p_pcNode);
	//bool AssignType(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName);	
	//bool GetEvaluationValue(SP_CPN_ParseNode* p_Node);	 //Recursive function to get the final evaluation results
	//bool DealUnionWithProduct(SP_CPN_ParseNode* p_pcNode, wxString p_sColorSetName);
	//bool CheckUnionWithProduct(SP_CPN_ParseNode* p_pcNode, vector<wxString> p_ComponentNameVector,bool &p_bCheck);
	//void PrintErrorMessage();
};

typedef std::shared_ptr<SP_CPN_Parse_Context> SP_CPN_Parse_Context_ptr;

SP_CPN_Parse_Context_ptr make_CPN_Parse_Context();

#endif // _SP_CPN_Parse_Context_H
