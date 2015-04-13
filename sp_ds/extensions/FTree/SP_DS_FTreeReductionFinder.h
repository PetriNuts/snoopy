//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.2 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: find possible reductions of the fault tree
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEREDUCTIONFINDER_H__
#define __SP_DS_FTREEREDUCTIONFINDER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_gui/mdi/SP_MDI_View.h"

class wxSpinCtrl;

struct NodeList
{
	NodeList(long, const wxString&, SP_ListNode*);
	long m_typ; //ID of a reduction-type
	wxString description; //short description of the reduction-rule
	SP_ListNode* r_nodeList; //list of nodes which were found by the reduction rule
};


typedef map<long, NodeList*> ReductionMap;

class SP_DS_FTreeReductionFinder: public SP_Error
{
private:

	SP_DS_Graph* m_pcGraph;

	//counter for reduction results
	long r_id;
	//list of all reductions results
	ReductionMap r_map; 
	//list of one reduction rule
	SP_ListNode* reduce_Nodes; 

protected:
	
	/**
	\brief Find all nodes which are connected with the target node

	\param		targetNode		target node
	\returns	nodeList		source nodes
	*/
    SP_ListNode* FindAllSourceNodes(SP_DS_Node* targetNode);

	/**
	\brief Comparing of two list of nodes
	It concerns M-OF-N gates: Testing following conditions:
	1. number of nodes in both lists must be the same
	2. Comparing of both lists results in one different node

	\param		list1	first list of nodes
	\param		list2	second list of nodes
	\returns	TRUE, FALSE
	*/
    bool IsEmptyAfterMerge(SP_ListNode* list1, SP_ListNode* list2);
    
	/**
	\brief Prove, if the list of combinations can be transformed to a M-OF-N system

	\param		sList	list of combinations
	\returns	TRUE, FALSE
	*/
	bool CheckList(list<SP_ListNode*> sList);

	/**
	\brief Find multiple inputs of type 1 and 2 of the distributive law
	
	\param	sourceType		type of combination of the terms
	\param	secondType		type of combination of the literals of one term
	*/
	void CheckMultipleInputs(const wxString& sourceType, const wxString& secondType);
    

public:
    SP_DS_FTreeReductionFinder(SP_DS_Graph* p_pcGraph);

    ~SP_DS_FTreeReductionFinder();
	/**
	\brief Find long chains of identical gates
	*/
    void FindChainsOfIdenticalGates();

	/**
	\brief Find gates which have only one output edge to another gate
	*/
    void FindUnneededGates();

	/**
	\brief Find negation-gates of the form: -(-X)
	*/
    void FindUnneededNegations();

	/**
	\brief Find multiple inputs on the basis of the distributive law
	The distribute law: 1. (X + Y) * (X + Z) = X + (Y * Z) and
	2. (X * Y) + (X * Z) = X * (Y + Z)
	*/
	void FindMultipleInputs();


	/**
	\brief Find combination of nodes and gates that can be replaced by an M-OF-N-gate
	*/
	void FindPossibleMOFNInputs();
	
	/**
	\brief Returns list of possible reductions
	*/
	ReductionMap GetMap();
};

#endif // __SP_DS_FTREEFINDREDUCTIONS_H__


