//////////////////////////////////////////////////////////////////////
// $Author: kw $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: dialog to search for nodes by id
//////////////////////////////////////////////////////////////////////
#ifndef __SP_NODE_COLOURIZER_H__
#define __SP_NODE_COLOURIZER_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_ds/SP_DS_NodeVector.h"

/**	\brief	Helper to colour nodes, save and restore node colours

*/
class SP_NodeColourizer:
	public SP_Error
{
 private:

 // Map to save the colours of nodes
  map<SP_Graphic*, SP_PairColour> m_pcMapNodeNum2wxColour;
  map<SP_Graphic*, std::pair<wxBrushStyle, wxPenStyle> > m_pcMapNodeNum2Style;
  SP_DS_Graph* m_pcGraph;
  SP_ListData* m_pclCoarseNodes;

 /**
	\brief colour actual graphic of node
	The actual graphic m_grIt points to is coloured.

  \param	p_pnode	the graphic to colour
	\param	p_pcTarget	the graphic of the target
	\param	p_pcSelf	the graphic we just connected

*/
  bool ColourGraphic(SP_Graphic* p_pnode, wxColour col, bool fullColouring, bool p_bColourCoarse);

 /**
	\brief brush colour actual graphic of node
	The actual graphic m_grIt points to is coloured.
	parameter geplant: farbe, voll oder halb (@todo entfernen)

*/
  bool ColourGraphic(SP_Graphic* p_pnode, wxColour col, bool p_bColourCoarse);

  // colour all neighbour nodes of graphic, but only pen, not brush
  bool ColourNeighbourNodesAndEdges(SP_DS_Node * p_pnode, wxColour col);

  // save colour for a node to backup the colour if wished
  bool SaveColour(SP_Graphic* p_pcNode);

	bool ColourGraphics(
		//SP_DS_Node * p_pnode,
		SP_Data * p_pnode,
		wxColour col,
		bool fullColouring,
    bool p_bColourCoarse);

  void ColourCoarseNodes(wxColour p_pcCol);
  wxColour GetGradientColour(int p_min, int p_max, int p_val, wxColour p_col);

  void SelectGraphic(SP_Graphic* p_Graphic);

protected:
  SP_ListNode* m_nodeList;
  SP_ListNode::const_iterator m_nodeIt;
  SP_ListGraphic* m_grList;
  SP_ListGraphic::const_iterator m_grIt;

  bool m_bSelect;

public:
  SP_NodeColourizer(SP_DS_Graph *m_pcGraph);

	~SP_NodeColourizer();

 /**
	\brief Colours a node set and his environment

  The nodes of the m_nodesToColour - List are full coloured.
  The neighboured edges and nodes are coloured as pen, not full.

  \param	m_nodesToColour	the nodes to be coloured
  \param	p_pcCol	the colour
  */

  void ColourNodeSet(SP_ListNode* p_nodesToColour, wxColour p_pcCol);


  /**
	\brief Colours a invariant and the environment

  The nodes wich belongs to the node vector are full coloured.
  The neighboured edges and nodes are coloured as pen, not full.
  If a colour is given, the nodes and edges are coloure with this colour.
  Else, all main nodes are coloured in reference to occurrence with lighter or darker p_pcCol.

  \param	m_nodesToColour	the nodes to be coloured
  \param	p_pcCol	the colour
  */
  void ColourNodeVector(SP_DS_NodeVector* p_pcNodeVector, wxColour p_pcCol, bool p_bColourGradient = false);
  void ColourNodeVector(SP_DS_NodeVector* p_pcNodeVector);


	// restore all the stored colours
	void RestoreColours();

	// discard all the stored colours
  void DiscardOldColours();

  void SetSelectNodes(bool p_bSelect) { m_bSelect = p_bSelect;}
};

#endif // __SP_NODE_COLOURIZER_H__
