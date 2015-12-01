//////////////////////////////////////////////////////////////////////
// $Source: kwinder
// $Author (modification): akurth $
// $Version: 1.0 $
// $Date: 2006/09/28 $
// Short Description: dialog to search for nodes by id
//////////////////////////////////////////////////////////////////////
#ifndef __SP_FTREENODECOLOURIZER_H__
#define __SP_FTREENODECOLOURIZER_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/base/SP_Graphic.h"

/**	\brief	Helper to colour nodes, save and restore node colours

*/
class SP_FTreeNodeColourizer:
	public SP_Error
{
 private:

 // Map to save the colours of nodes
  map<SP_DS_Node*, SP_PairColour*> *m_pcMapNodeNum2wxColour;

  SP_ListNode::const_iterator m_nodeIt;
  SP_ListGraphic::const_iterator m_grIt;
  SP_ListNode* m_pcAllNodes;


protected:

  /**
	\brief colour actual graphic of node
	The actual graphic m_grIt points to is coloured.

	\param	p_pnode			the graphic to colour
	\param	col				the colour
	\param	fullColouring	flag to setting fullcolouring
	\param	p_bColourCoarse	flag for colouring Coarse Events

	*/
  bool ColourGraphic(SP_DS_Node* p_pnode, wxColour col, bool fullColouring, bool p_bColourCoarse);

	/**
	\brief brush colour actual graphic of node
	The actual graphic m_grIt points to is coloured.

	\param	p_pnode			the graphic to colour
	\param	col				the colour
	\param	p_bColourCoarse	flag for colouring Coarse Events
	*/
  bool ColourGraphic(SP_DS_Node* p_pnode, wxColour col, bool p_bColourCoarse);


  // save colour for a node to backup the colour if wished
  bool SaveColour(SP_DS_Node* p_pcNode);

  /**
	\brief restore stored colours of one node.

	\param	restoreNode		the node which colour should be restored
	\param	pairColour		the default colour and stored colour
	*/
  void RestoreOneNodeColour(SP_DS_Node* restoreNode, SP_PairColour* pairColour);

public:
  SP_FTreeNodeColourizer(
 			SP_DS_Graph *m_pcGraph

 	);

	~SP_FTreeNodeColourizer();

	/**
	\brief colour a list of nodes
	every node is coloured by the call ColourGraphic().

	\param	m_nodesToColour		list of nodes
	\param	p_pcCol				the colour

	*/
	void ColourNodes(SP_ListNode* m_nodesToColour, wxColour p_pcCol);

	/**
	\brief colour oone node
	node is coloured by the call ColourGraphic().

	\param	m_nodeToColour		node to colour
	\param	p_pcCol				the colour

	*/
	void ColourNode(SP_DS_Node* m_nodeToColour, wxColour p_pcCol);

	// restore all the stored colours
	void RestoreColours();
	//restore stored colour of one node
	void RestoreColourNode(SP_DS_Node* node);

	// discard all the stored colours
	void DiscardOldColours();

	/**
	\brief returns list of nodes whose graphic are changed

	\returns	l_pcAllNodes	list of coloured nodes
	*/
	SP_ListNode* GetNodeList();

	/**
	\brief check, if colour of the node ist still restored

	\returns TRUE, FALSE
	*/
	bool IsNodeRestore(SP_DS_Node* n_pcNode);
};

#endif // __SP_FTREENODECOLOURIZER_H__
