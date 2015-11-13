//////////////////////////////////////////////////////////////////////
// $Author: kw $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: toolbox to colourize temporary some nodes
//////////////////////////////////////////////////////////////////////

#include "sp_core/tools/SP_NodeColourizer.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "sp_core/SP_Core.h"
#include <algorithm>

SP_NodeColourizer::SP_NodeColourizer(SP_DS_Graph *p_pcGraph) :
	m_pcGraph(p_pcGraph),
	m_pclCoarseNodes(NULL),
	m_nodeList(NULL),
	m_grList(NULL),
	m_bSelect(false)
{
	m_pclCoarseNodes = new SP_ListData ();
}

SP_NodeColourizer::~SP_NodeColourizer()
{
	wxDELETE(m_pclCoarseNodes);
}

void SP_NodeColourizer::ColourNodeSet(SP_ListNode* p_nodesToColour, wxColour p_pcCol)
{
	CHECK_POINTER(p_nodesToColour, return);
	m_pclCoarseNodes->clear();

	if (p_nodesToColour->size() != 0)
	{
		m_nodeIt = p_nodesToColour->begin();
		m_grList = NULL;
		for (; m_nodeIt != p_nodesToColour->end(); m_nodeIt++)
		{
			m_grList = (*m_nodeIt)->GetGraphics();
			if (m_grList->size() != 0)
			{
				m_grIt = m_grList->begin();
				while (m_grIt != m_grList->end())
				{
					SaveColour((*m_grIt));
					ColourGraphic((*m_grIt), p_pcCol, true);
					ColourNeighbourNodesAndEdges((*m_nodeIt), p_pcCol);
					m_grIt++;
				}
			}
		}
		ColourCoarseNodes(p_pcCol);

		SP_MDI_Doc* doc = SP_Core::Instance()->GetRootDocument();
		doc->ShowAll();
		doc->Refresh();
		doc->Modify(TRUE);
	}
}

void SP_NodeColourizer::ColourNodeVector(SP_DS_NodeVector* p_pcNodeVector, wxColour p_col, bool p_bColourGradient)
{
	CHECK_POINTER(p_pcNodeVector , return);
	CHECK_POINTER(p_pcNodeVector->GetNodeVectorSystem() , return);
	m_pclCoarseNodes->clear();

	SP_DS_NodeVectorSystem* l_pcNvs = p_pcNodeVector->GetNodeVectorSystem();
	const SP_ListNode& l_pclAllNodes = p_pcNodeVector->GetNodeVectorSystem()->GetNodes();

	SP_VectorLong::const_iterator l_aVector = p_pcNodeVector->GetVector().begin();

	for (m_nodeIt = l_pclAllNodes.begin(); m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
	{
		if (*l_aVector > 0)
		{
			//m_nodeList->push_back(dynamic_cast<SP_DS_Node*>(*m_nodeIt));
			m_grList = NULL;
			m_grList = (*m_nodeIt)->GetGraphics();
			if (m_grList->size() != 0)
			{
				m_grIt = m_grList->begin();
				while (m_grIt != m_grList->end())
				{
					SaveColour((*m_grIt));
					wxColor l_Color = p_col;
					if (p_bColourGradient)
					{
						l_Color = GetGradientColour(l_pcNvs->GetOccurrenceRangeMin(), l_pcNvs->GetOccurrenceRangeMax(), *l_aVector, p_col);
					}
					ColourGraphic((*m_grIt), l_Color, true);
					ColourNeighbourNodesAndEdges((*m_nodeIt), p_col);

					m_grIt++;
				}
			}
		}
		l_aVector++;
	}
	ColourCoarseNodes(p_col);

	SP_MDI_Doc* doc = SP_Core::Instance()->GetRootDocument();
	doc->ShowAll();
	doc->Refresh();
	doc->Modify(TRUE);
}

void SP_NodeColourizer::ColourNodeVector(SP_DS_NodeVector* p_pcNodeVector)
{
	ColourNodeVector(p_pcNodeVector, *wxRED, true);
}

void SP_NodeColourizer::ColourCoarseNodes(wxColour p_pcCol)
{
	// @todo test coarse node: if it has all internal transitions coloured,
	// colour the coarse node full. else, colour only the brush of coarsenode

	CHECK_POINTER(m_pclCoarseNodes , return);
	for (SP_Data* l_node : *m_pclCoarseNodes)
	{
		ColourGraphics(l_node, p_pcCol, FALSE, FALSE);
	}
}

bool SP_NodeColourizer::SaveColour(SP_Graphic* p_pcNode)
{
	wxColour l_pcColBrush = p_pcNode->GetBrush()->GetColour();
	wxColour l_pcColPen = p_pcNode->GetPen()->GetColour();
	m_pcMapNodeNum2wxColour.insert(make_pair(p_pcNode, make_pair(l_pcColBrush, l_pcColPen)));

	auto l_StyleBrush = p_pcNode->GetBrush()->GetStyle();
	auto l_StylePen = p_pcNode->GetPen()->GetStyle();
	m_pcMapNodeNum2Style.insert(make_pair(p_pcNode, make_pair(l_StyleBrush, l_StylePen)));
	return true;
}

void SP_NodeColourizer::RestoreColours()
{
	if (m_pcMapNodeNum2wxColour.size() == 0)
	{
		return;
	}

	auto l_pcIt = m_pcMapNodeNum2wxColour.begin();
	auto l_itStyle = m_pcMapNodeNum2Style.begin();

	while (l_pcIt != m_pcMapNodeNum2wxColour.end())
	{
		SP_Graphic* l_pcNode = (*l_pcIt).first;

		wxColour l_pcColBrush = (*l_pcIt).second.first;
		wxColour l_pcColPen = (*l_pcIt).second.second;

		auto l_styleBrush = (*l_itStyle).second.first;
		auto l_stylePen = (*l_itStyle).second.second;

		l_pcNode->SetBrush(wxTheBrushList->FindOrCreateBrush(l_pcColBrush, l_styleBrush));
		l_pcNode->SetPen(wxThePenList->FindOrCreatePen(l_pcColPen, l_pcNode->GetPen()->GetWidth(), l_stylePen));

		l_pcNode->Update(TRUE);

		l_pcIt++;
		l_itStyle++;
	}
	m_pcMapNodeNum2wxColour.clear();
	m_pcMapNodeNum2Style.clear();

	SP_MDI_Doc *doc = SP_Core::Instance()->GetRootDocument();

	if (doc)
	{
		doc->Refresh();
		doc->Modify(true);
	}

}

bool SP_NodeColourizer::ColourGraphic(SP_Graphic* p_pcNode, wxColour col, bool p_bColourCoarse)
{
	return ColourGraphic(p_pcNode, col, TRUE, p_bColourCoarse);
}

bool SP_NodeColourizer::ColourGraphic(SP_Graphic* p_pcNode, wxColour col, bool fullColouring, bool p_bColourCoarse)
{
	CHECK_POINTER(p_pcNode, return false);

	if (fullColouring)
	{
		p_pcNode->SetBrush(wxTheBrushList->FindOrCreateBrush(col, p_pcNode->GetBrush()->GetStyle()));
	}
	else
	{
		p_pcNode->SetPen(wxThePenList->FindOrCreatePen(col, p_pcNode->GetPen()->GetWidth(), p_pcNode->GetPen()->GetStyle()));
	}

	p_pcNode->Update(TRUE);

	SelectGraphic(p_pcNode);

	if (p_bColourCoarse)
	{
		unsigned int netNr = p_pcNode->GetNetnumber();
		while(netNr > 1)
		{
			SP_Data *data = m_pcGraph->GetCoarseNode(netNr);
			netNr = data->GetNetnumber();
			// put in list to colourize coarse node later
			// but only if p_bColourCoarse is true
			if (SP_Find(*m_pclCoarseNodes, data) == m_pclCoarseNodes->end())
			{
				m_pclCoarseNodes->push_back(data);
			}
		}
	}
	return true;
}

bool SP_NodeColourizer::ColourGraphics(SP_Data * p_pcNode,
//SP_DS_Node * p_pcNode,
		wxColour col, bool fullColouring, bool p_bColourCoarse)
{
	CHECK_POINTER(p_pcNode, return FALSE);

	SP_ListGraphic* l_pcGraphicsToColourList;
	SP_ListGraphic::const_iterator grTC_It;

	l_pcGraphicsToColourList = p_pcNode->GetGraphics();
	if (l_pcGraphicsToColourList && l_pcGraphicsToColourList->size() != 0)
	{
		grTC_It = l_pcGraphicsToColourList->begin();
		while (grTC_It != l_pcGraphicsToColourList->end())
		{
			SaveColour((*grTC_It));
			ColourGraphic((*grTC_It), col, fullColouring, p_bColourCoarse);
			grTC_It++;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;

}

// colour all neighbour nodes of actual graphic, but only pen not brush
bool SP_NodeColourizer::ColourNeighbourNodesAndEdges(SP_DS_Node * p_pcNode, wxColour col)
{
	CHECK_POINTER(p_pcNode, return FALSE);
	//CHECK_BOOL(p_pcNode != m_grList->end(), return false);


	SP_DS_Node* nodeToColour;
	SP_ListGraphic* graphicsToColourList;
	SP_ListGraphic::const_iterator grTC_It;

	if (p_pcNode->GetTargetEdges()->size() != 0)
	{
		// colour all prePlaces
		SP_ListEdge::const_iterator l_itEdges;
		for (l_itEdges = p_pcNode->GetTargetEdges()->begin(); l_itEdges != p_pcNode->GetTargetEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetSource())
			{
				nodeToColour = (SP_DS_Node*) (*l_itEdges)->GetSource();
				// @todo colour only the real neighbours
				ColourGraphics(nodeToColour, col, FALSE, FALSE);

			}

			graphicsToColourList = (*l_itEdges)->GetGraphics();
			if (graphicsToColourList->size() != 0)
			{
				grTC_It = graphicsToColourList->begin();

				while (grTC_It != graphicsToColourList->end())
				{
					SaveColour((*grTC_It));
					ColourGraphic((*grTC_It), col, TRUE, FALSE);
					ColourGraphic((*grTC_It), col, FALSE, FALSE);
					grTC_It++;
				}
			}

		}

	}

	if (p_pcNode->GetSourceEdges()->size() != 0)
	{
		// colour all postPlaces
		SP_ListEdge::const_iterator l_itEdges;
		for (l_itEdges = p_pcNode->GetSourceEdges()->begin(); l_itEdges != p_pcNode->GetSourceEdges()->end(); ++l_itEdges)
		{
			if ((*l_itEdges)->GetTarget())
			{
				nodeToColour = (SP_DS_Node*) (*l_itEdges)->GetTarget();
				ColourGraphics(nodeToColour, col, FALSE, FALSE);

			}

			graphicsToColourList = (*l_itEdges)->GetGraphics();
			if (graphicsToColourList->size() != 0)
			{
				grTC_It = graphicsToColourList->begin();

				while (grTC_It != graphicsToColourList->end())
				{
					SaveColour((*grTC_It));

					ColourGraphic((*grTC_It), col, TRUE, FALSE);
					ColourGraphic((*grTC_It), col, FALSE, FALSE);
					grTC_It++;
				}
			}

		}

	}
	return TRUE;

}

void SP_NodeColourizer::DiscardOldColours()
{
	if (m_pcMapNodeNum2wxColour.size() == 0)
	{
		return;
	}
	m_pcMapNodeNum2wxColour.clear();
	m_pcMapNodeNum2Style.clear();
}

wxColour SP_NodeColourizer::GetGradientColour(int p_min, int p_max, int p_val, wxColour p_col)
{

	if (p_val < p_min || p_val > p_max)
	{
		SP_LOGDEBUG(wxT("occurrence not gradient-colourable"));
		return *wxRED;
	}

	// @todo At the moment, p_bColourGradient is only with the colour blue possible. Should be flexible...
	// ligther blue goes from (200,200,255) to (0,0,255), darker blue from (0,0,255) to (0,0,170)
	// (200,200,255) belongs to p_min, (0,0,170) to p_max.
	// so we have 200 + 85 colours...
	int l_transValue = (p_val - p_min) * 285.0 / (p_max - p_min);

	//SP_LOGDEBUG(wxString::Format(wxT("max:%d , min:%d , val:%d , transval: %d"), p_max, p_min, p_val, l_transValue));

	if (l_transValue >= 200)
	{
		if (p_col.Blue() >= p_col.Red() && p_col.Blue() >= p_col.Green())
		{
			return wxColor(0, 0, 255 - l_transValue + 200);
		}

		if (p_col.Green() >= p_col.Red() && p_col.Green() >= p_col.Blue())
		{
			return wxColor(0, 255 - l_transValue + 200, 0);
		}

		if (p_col.Red() >= p_col.Blue() && p_col.Red() >= p_col.Green())
		{
			return wxColor(255 - l_transValue + 200, 0, 0);
		}

	}
	else
	{
		int rg = 200 - l_transValue;
		if (p_col.Blue() >= p_col.Red() && p_col.Blue() >= p_col.Green())
		{
			return wxColor(rg, rg, 255);
		}

		if (p_col.Green() >= p_col.Red() && p_col.Green() >= p_col.Blue())
		{
			return wxColor(rg, 255, rg);
		}

		if (p_col.Red() >= p_col.Blue() && p_col.Red() >= p_col.Green())
		{
			return wxColor(255, rg, rg);
		}
	}

	return wxColor(255, 0, 0);

}

void SP_NodeColourizer::SelectGraphic(SP_Graphic* p_Graphic)
{
	if(m_bSelect)
	{
		SP_MDI_View *l_pcView = dynamic_cast<SP_MDI_View*>(m_pcGraph->GetParentDoc()->GetFirstView());

		SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();
		if(l_pcCanvas)
		{
			wxClientDC l_cDC(l_pcCanvas);
			l_pcCanvas->DoPrepareDC(l_cDC);

			p_Graphic->GetPrimitive()->Select(true, &l_cDC);
		}
	}
}
