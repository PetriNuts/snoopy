//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.3 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: find logical expression of the fault tree
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREELOGICALEXPRESSIONREADER_H__
#define __SP_DS_FTREELOGICALEXPRESSIONREADER_H__

#include "sp_core/SP_Core.h"


struct lNode
{
  lNode(bool, const wxString&, SP_DS_Node*);
  bool isChecked; //marks a node as completely determined.
  wxString m_name; //name of the node or element
  SP_DS_Node* m_node; //node


};

typedef map<long, lNode*> NodeMap;
typedef map<wxString, list<lNode*>*> StackMap;
typedef map<wxString, SP_ListString*> TermMap;

//list of elements in a logical expressions. It is needed for quantitative and qualitative analysis
typedef SP_MapLong2String LiteralMap;

class SP_DS_FTreeLogicalExpressionReader
{
private:
 
	SP_DS_Graph* m_pcGraph;
	//list of the logical expression of the fault tree
	NodeMap m_map;
	//list of the logical expression after the application of deMorgan,
	//the list has to be used for the quantitative analysis
	LiteralMap vMap;

protected:

	/**
	\brief Check start element
	Nodes with more than one input edge, will be write in the list in form
	of "(" node ")".
	
	\param		p_pcNode	start element of the fault tree
	\returns	TRUE, FALSE
	*/
	bool FindTopEvent(SP_DS_Node* p_pcNode);
	
	/**
	\brief Transform M-Of-N-Gates
	Transform a M-OF-N-gate on the basis of ANDs.
	
	\param		n_pcNode	M-Of-N-gate
	\returns	termMap		replaced M-Of-N-Gates
	*/
	NodeMap DoReplaceMOfNGates(SP_DS_Node* n_pcNode);

	/**
	\brief Transform COND-Gates 
	Transform a COND-gate on the basis of ANDs.
	
	\param		n_pcNode	COND-gate
	\returns	termMap		replaced COND-gate
	*/
	NodeMap DoReplaceCONDGates(SP_DS_Node* n_pcNode);

	/**
	\brief Transform XOR-Gates
	Transform a XOR-gate on the basis of ANDs, ORs and NEGs.
		
	\param		n_pcNode	XOR-gate
	\returns	termMap		replaced XOR-gate
	*/
	NodeMap DoReplaceXORGates(SP_DS_Node* n_pcNode);

	/**
	\brief Combine events with gates
	After transformation: Nodes which are connected with a M-OF-N gate or COND-gate,
	will be combined with ANDs now.
	
	\param		n_pcAllNodes	events which have to be combined
	\returns	termMap			list of events and gates 
	*/
	NodeMap DoCombineEventsWithGate(SP_ListNode* n_pcAllNodes);

	/**
	\brief Check of duplicate entrys in a list
	Transforming a M-OF-N-Gate into ANDs: Before saving a new combination,
	we proove, if there is still a identical entry in the list.
	
	\param		selectedNodes		list of all combinations
	\param		nList				list of a new combination
	\returns	TRUE, FALSE
	*/
	bool HasDuplicateEntry(list<SP_ListNode*>* selectedNodes, SP_ListNode* nList);

	 /**
	\brief Transforms an expression by deMorgan
	Each AND is replaced by OR, each OR by AND and the variables
	are marked as negated variable or the negation of the variable
	is deleted.
	
	\param	nodeMap		expression to replace
	\param	idBegin		Beginning of the expression to be replaced 
	\param	int idEnd	End of the expression to be replaced 
	\returns newMap		replaced expression
	*/
	LiteralMap DoReplaceNodesByDeMorgan(LiteralMap nodeMap, int idBegin, int idEnd);


public:
    SP_DS_FTreeLogicalExpressionReader();

   ~SP_DS_FTreeLogicalExpressionReader();

	/**
	\brief Read logical expression of a fault tree
	The logical expression of a fault tree will be find out with the aid of
	breath searching.
	
	\param		p_pcNode	start element for breath searching
	\returns	m_map		logical expression of the fault tree
	*/	
   NodeMap FindLogicalExpression(SP_DS_Node* p_pcNode);
   
   /**
	\brief Use DeMorgan to transform logical expression
	DeMorgan predicate: -(X + X) = -X * -X and -(X * X) = -X + -X.
	First, all events, which are connected with an NEG-Gate, are
	marked as negated events. Then the method
	looks for the most inner expression in each case to evaluate
	this expression by deMorgan.
	
	\returns vMap	transformed logical expression
	*/
   LiteralMap DoDeMorgan();
};

#endif // __SP_DS_FTREELOGICALEXPRESSIONREADER_H__


