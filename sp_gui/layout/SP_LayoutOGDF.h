//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Extension for netelements (only nodes actually)
//////////////////////////////////////////////////////////////////////
#ifndef __SP_LAYOUTOGDF_H__
#define __SP_LAYOUTOGDF_H__

#include "SP_Layout.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>

using ogdf::node;
using ogdf::edge;
using ogdf::Graph;
using ogdf::GraphAttributes;

class SP_LayoutOGDF: public SP_Layout
{
private:
	bool SP_FMMMLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr);
	bool SP_SugiyamaLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr);
	bool SP_PlanarizationLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr);

	std::map<node, unsigned long> node2id;
    std::map<unsigned long, node> id2node;
    std::map<edge, unsigned long> edge2id;

protected:
	virtual bool DoVisualization();

public:

    SP_LayoutOGDF();
    virtual ~SP_LayoutOGDF();

};

#endif // __SP_LAYOUTOGDF_H__


