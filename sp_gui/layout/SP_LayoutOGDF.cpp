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
	Graph* l_Graph = new Graph();
	GraphAttributes* l_GraphAttr = new GraphAttributes(*l_Graph, GraphAttributes::nodeGraphics
																| GraphAttributes::edgeGraphics
																| GraphAttributes::nodeLabel
																| GraphAttributes::edgeLabel
																| GraphAttributes::edgeArrow);

	MapNodeData::iterator l_itN;
	for(l_itN = m_mNodes.begin(); l_itN != m_mNodes.end(); ++l_itN)
	{
		NodeData* l_pcNodeData = l_itN->second;
		node l_Node = l_Graph->newNode();
		l_GraphAttr->label(l_Node) = std::to_string(l_pcNodeData->Id);
		l_GraphAttr->height(l_Node) = l_pcNodeData->H;
		l_GraphAttr->width(l_Node) = l_pcNodeData->W;
	}

	MapEdgeData::iterator l_itE;
	for(l_itE = m_mEdges.begin(); l_itE != m_mEdges.end(); ++l_itE)
	{
		EdgeData* l_pcEdgeData = l_itE->second;
		node l_Source = NULL;
		node l_Target = NULL;
		node l_Node = l_Graph->firstNode();
		while(l_Node)
		{
			std::string l_SourceId = std::to_string(l_pcEdgeData->SourceId);
			if((l_GraphAttr->label(l_Node)) == l_SourceId)
			{
				l_Source = l_Node;
			}
			std:string l_TargetId = std::to_string(l_pcEdgeData->TargetId);
			if(l_GraphAttr->label(l_Node) == l_TargetId)
			{
			  l_Target = l_Node;
			}
			if(l_Source && l_Target)
			{
			  l_Node = NULL;
			}
			else
			{
			  l_Node = l_Node->succ();
			}
		}
		edge l_Edge = l_Graph->newEdge(l_Source, l_Target);
		l_GraphAttr->label(l_Edge) = std::to_string(l_pcEdgeData->Id);
		l_GraphAttr->arrowType(l_Edge) = EdgeArrow::eaLast;
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

	node l_Node = l_Graph->firstNode();
	while(l_Node)
	{
		long l_nId = atol(l_GraphAttr->label(l_Node).c_str());
		m_mNodes[l_nId]->X = l_GraphAttr->x(l_Node);
		m_mNodes[l_nId]->Y = l_GraphAttr->y(l_Node);

		l_Node = l_Node->succ();
	}

	edge l_Edge = l_Graph->firstEdge();
	while(l_Edge)
	{
		long l_nId = atol(l_GraphAttr->label(l_Edge).c_str());
		DPolyline &l_Polyline = l_GraphAttr->bends(l_Edge);
		m_mEdges[l_nId]->X.resize(l_Polyline.size());
		m_mEdges[l_nId]->Y.resize(l_Polyline.size());
		for(int i = 0; i < l_Polyline.size(); ++i)
		{
			m_mEdges[l_nId]->X[i] = (*(l_Polyline.get(i))).m_x;
			m_mEdges[l_nId]->Y[i] = (*(l_Polyline.get(i))).m_y;
		}
		l_Edge = l_Edge->succ();
	}

	delete l_GraphAttr;
	delete l_Graph;

	return true;
}

bool SP_LayoutOGDF::SP_FMMMLayout(Graph* p_Graph, GraphAttributes* p_GraphAttr)
{
	FMMMLayout l_Layout;

	l_Layout.useHighLevelOptions(true);
	l_Layout.unitEdgeLength(40.0);
	l_Layout.newInitialPlacement(true);
	l_Layout.minDistCC(40.0);
	l_Layout.tipOverCCs(FMMMLayout::toNoGrowingRow);
	l_Layout.qualityVersusSpeed(FMMMLayout::qvsGorgeousAndEfficient);

	l_Layout.call(*p_GraphAttr);

	return true;
}

bool SP_LayoutOGDF::SP_SugiyamaLayout(Graph* p_Graph, GraphAttributes* p_GraphAttr)
{
	SugiyamaLayout l_Layout;
	FastHierarchyLayout* l_FHL = new FastHierarchyLayout();
	l_FHL->layerDistance(20.0);
	l_FHL->nodeDistance(20.0);
	l_Layout.setLayout(l_FHL);

	l_Layout.call(*p_GraphAttr);

	return true;
}

bool SP_LayoutOGDF::SP_PlanarizationLayout(Graph* p_Graph, GraphAttributes* p_GraphAttr)
{
	PlanarizationLayout pl;

	SubgraphPlanarizer *crossMin = new SubgraphPlanarizer();

	FastPlanarSubgraph *ps = new FastPlanarSubgraph;
	ps->runs(100);
	VariableEmbeddingInserter *ves = new VariableEmbeddingInserter();
	ves->removeReinsert(rrAll);

	crossMin->setSubgraph(ps);
	crossMin->setInserter(ves);

	EmbedderMinDepthMaxFaceLayers *emb = new EmbedderMinDepthMaxFaceLayers();
	pl.setEmbedder(emb);

	OrthoLayout *ol = new OrthoLayout();
	ol->separation(20.0);
	ol->cOverhang(0.4);
	pl.setPlanarLayouter(ol);

	pl.call(*p_GraphAttr);

	return true;
}
