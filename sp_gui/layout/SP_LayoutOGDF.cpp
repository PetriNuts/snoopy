//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "SP_LayoutOGDF.h"
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/FastHierarchyLayout.h>

#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/planarity/FastPlanarSubgraph.h>
#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>


SP_LayoutOGDF::SP_LayoutOGDF()
{
}

SP_LayoutOGDF::~SP_LayoutOGDF()
{
}

bool SP_LayoutOGDF::DoVisualization()
{
	Graph l_Graph;
	GraphAttributes l_GraphAttr(l_Graph, GraphAttributes::nodeGraphics
											| GraphAttributes::edgeGraphics
											| GraphAttributes::edgeArrow);

	for(const auto l_itN : m_mNodes)
	{
		const NodeData& l_pcNodeData = l_itN.second;
		node l_Node = l_Graph.newNode();
		l_GraphAttr.height(l_Node) = l_pcNodeData.H;
		l_GraphAttr.width(l_Node) = l_pcNodeData.W;
        node2id[l_Node] = l_pcNodeData.Id;
        id2node[l_pcNodeData.Id] = l_Node;
	}

	for(const auto l_itE : m_mEdges)
	{
		const EdgeData& l_pcEdgeData = l_itE.second;
		node l_Source = id2node[l_pcEdgeData.SourceId];
		node l_Target = id2node[l_pcEdgeData.TargetId];
		if(l_Source && l_Target)
		{
			edge l_Edge = l_Graph.newEdge(l_Source, l_Target);
			l_GraphAttr.arrowType(l_Edge) = ogdf::EdgeArrow::eaLast;
			edge2id[l_Edge] = l_pcEdgeData.Id;
		}
		else
		{
			SP_LOGERROR(wxT("adding edge between node(") + std::to_string(l_pcEdgeData.SourceId)
				+ wxT(") and node(") + std::to_string(l_pcEdgeData.TargetId) + wxT(")"));
		}
	}

	if(m_sLayout == wxT("FMMM"))
	{
		SP_FMMMLayout(l_Graph, l_GraphAttr);
	}
	else if(m_sLayout == wxT("Sugiyama"))
	{
		SP_SugiyamaLayout(l_Graph, l_GraphAttr);
	}
	else if(m_sLayout == wxT("Planarization"))
	{
		SP_PlanarizationLayout(l_Graph, l_GraphAttr);
	}

	node l_Node = l_Graph.firstNode();
	while(l_Node)
	{
		unsigned long l_nId = node2id[l_Node];
		m_mNodes[l_nId].X = l_GraphAttr.x(l_Node);
		m_mNodes[l_nId].Y = l_GraphAttr.y(l_Node);

		l_Node = l_Node->succ();
	}

	edge l_Edge = l_Graph.firstEdge();
	while(l_Edge)
	{
		unsigned long l_nId = edge2id[l_Edge];
		ogdf::DPolyline &l_Polyline = l_GraphAttr.bends(l_Edge);
		m_mEdges[l_nId].X.resize(l_Polyline.size());
		m_mEdges[l_nId].Y.resize(l_Polyline.size());
		for(int i = 0; i < l_Polyline.size(); ++i)
		{
			m_mEdges[l_nId].X[i] = (*(l_Polyline.get(i))).m_x;
			m_mEdges[l_nId].Y[i] = (*(l_Polyline.get(i))).m_y;
		}
		l_Edge = l_Edge->succ();
	}

	return true;
}

bool SP_LayoutOGDF::SP_FMMMLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr)
{
	using ogdf::FMMMLayout;
	FMMMLayout l_Layout;

	l_Layout.useHighLevelOptions(true);
	l_Layout.unitEdgeLength(40.0);
	l_Layout.newInitialPlacement(true);
	l_Layout.minDistCC(40.0);
	l_Layout.tipOverCCs(FMMMLayout::toNoGrowingRow);
	l_Layout.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);

	l_Layout.call(p_GraphAttr);

	return true;
}

bool SP_LayoutOGDF::SP_SugiyamaLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr)
{
	ogdf::SugiyamaLayout l_Layout;
	auto* l_FHL = new ogdf::FastHierarchyLayout();
	l_FHL->layerDistance(20.0);
	l_FHL->nodeDistance(20.0);
	l_Layout.setLayout(l_FHL);

	l_Layout.call(p_GraphAttr);

	return true;
}

bool SP_LayoutOGDF::SP_PlanarizationLayout(Graph& p_Graph, GraphAttributes& p_GraphAttr)
{
	ogdf::PlanarizationLayout pl;

	auto *crossMin = new ogdf::SubgraphPlanarizer();

	auto *ps = new ogdf::FastPlanarSubgraph;
	ps->runs(100);
	auto *ves = new ogdf::VariableEmbeddingInserter();
	ves->removeReinsert(ogdf::rrAll);

	crossMin->setSubgraph(ps);
	crossMin->setInserter(ves);

	auto *emb = new ogdf::EmbedderMinDepthMaxFaceLayers();
	pl.setEmbedder(emb);

	auto *ol = new ogdf::OrthoLayout();
	ol->separation(20.0);
	ol->cOverhang(0.4);
	pl.setPlanarLayouter(ol);

	pl.call(p_GraphAttr);

	return true;
}
