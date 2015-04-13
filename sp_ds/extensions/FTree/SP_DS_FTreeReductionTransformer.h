//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: transform fault tree 
////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEREDUCTIONTRANSFORMER_H__
#define __SP_DS_FTREEREDUCTIONTRANSFORMER_H__

#include "sp_gui/mdi/SP_MDI_Doc.h"



class SP_DS_FTreeReductionTransformer 
{
 private:
	
	SP_MDI_Doc* m_pcMyDoc;

	SP_DS_Graph* m_pcGraph;
	
 protected:

	/**
	\brief Find all nodes which are connected with the target node

	\param	targetNode		target node
	*/
	SP_ListNode* FindAllSourceNodes(SP_DS_Node* targetNode);
  
	/**
	\brief Find target node of an edge
	*/
	SP_DS_Node* FindTargetNode(SP_DS_Node* sourceNode);

	/**
	\brief Delete duplicate nodes
	It concerns the distributive law: Logical basic/undeveloped events which are identical,
	will be combined to one logical basic/undeveloped event.

	\param	l_pcAllNodes	list of logical basic/undeveloped events
	*/
	SP_ListNode* EliminateDuplicateNodes(SP_ListNode* l_pcAllNodes);

	/**
	\brief Delete a node

	\param	del_Node	node to delete
	*/
	void DeleteNode(SP_DS_Node* del_Node);
  
	/**
	\brief Find and delete nodes which have no egdes
	It is possible that after a transformation there are still logical
	basic/undeveloped events which are not be combined with an edge. So delete these nodes.

	\param	l_pcNodeList	list of logical basic/undeveloped events
	*/
	void FindNodesWithoutEdges(SP_ListNode* l_pcNodeList);

	/**
	\brief Select node and delete graphic of node
	*/
	void DoDeleteGraphic();
	
	/**
	\brief Update the frame
	
	\param	l_doc		document
	*/
	void RefreshFrame(SP_MDI_Doc* l_doc);
  
	/**
	\brief Draw new edge
	Combine source node with target node.

	\param	source	source node
	\param	target	target node
	*/
	void drawEdge(SP_DS_Node* source, SP_DS_Node* target);
  
 
 public:
	SP_DS_FTreeReductionTransformer(SP_DS_Graph *m_pcGraph, SP_MDI_Doc* l_pcMyDoc);
	~SP_DS_FTreeReductionTransformer();
  
	/**
	\brief Eliminate unneeded gates 
	Eliminate gates which have only one output edge to another gate and
	eliminate duplicate negations.

	\param	l_pcAllNodes	list of nodes to delete
	*/
	void DoEliminateUnneededNodes(SP_ListNode* l_pcAllNodes);

	/**
	\brief Combine identical gates to one gate
	All gates which are identical, have to be combine to one gate.

	\param	l_pcAllNodes	list of gates to combine
	*/
	void DoCombineIdenticalGates(SP_ListNode* l_pcAllNodes);
  
	/**
	\brief Combine combination of nodes and gates (AND, OR) to a M-OF-N gate

	\param	l_pcAllNodes	list of gates which will be replaced by M-OF-N gate
	*/
	void DoCombineToMofNGate(SP_ListNode* l_pclAllNodes);

	/**
	\brief Relocate multiple inputs
	Use distributive law to minimised input edges and gates.

	\param	l_pcAllNodes	list of nodes (gates and logical event)
	*/
	void DoRelocateMultipleInputs(SP_ListNode* l_pclAllNodes);
};

#endif // __SP_DS_FTREEREDUCTIONTRANSFORMER_H__
