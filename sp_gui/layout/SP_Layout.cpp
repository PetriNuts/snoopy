//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "SP_Layout.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gr/SP_GR_Attribute.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_core/SP_GPR_Canvas.h"

#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

SP_Layout::SP_Layout():
	m_nMaxX(0), m_nMaxY(0), m_pcGraph(NULL), m_pcDoc(NULL), m_sLayout(wxT(""))
{
}

SP_Layout::~SP_Layout()
{
}

bool SP_Layout::Visualization(SP_DLG_LayoutProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{
	CHECK_POINTER(p_pcDlg, return FALSE);
	CHECK_POINTER(p_pcDoc, return FALSE);

	//set MemberVariable
	m_pcDoc = p_pcDoc;
	m_pcGraph = p_pcDoc->GetGraph();

	m_lAttributes = p_pcDlg->GetAttributes();
	m_sLayout = p_pcDlg->GetLayout();

	switch (p_pcDlg->GetView())
	{

	case SP_LAYOUT_ACTUAL_VIEW:
	{
		SP_MDI_View* l_pcActualView = dynamic_cast<SP_MDI_View*> (m_pcDoc->GetFirstView());
		unsigned int l_nNet = l_pcActualView->GetNetnumber();

		Init(l_nNet);

		DoVisualization();

		ResetNodes();
		ResetEdges();
		ResetEdgesAttributes();
		RefreshNodes();

		Clear();
		break;
	}
	case SP_LAYOUT_ALL_VIEWS:
	{	
		for (unsigned int i = 1; i <= m_pcGraph->GetNetnumber(); ++i)
		{
			if (m_pcGraph->GetCoarseNode(i) || (i == 1))
			{
				Init(i);

				DoVisualization();

				ResetNodes();
				ResetEdges();
				ResetEdgesAttributes();
				RefreshNodes();

				Clear();
			}
		}
		break;
	}
	default:
		SP_LOGDEBUG(wxT("error in selecting with view should be layouted occured"));
		break;
	} //end switch

	m_pcDoc->Refresh();

	return TRUE;
}

void SP_Layout::Init(unsigned int p_nNetNumber)
{
	SP_ListEdgeclass::const_iterator l_EIter;
	const SP_ListEdgeclass* l_plEC = m_pcGraph->GetEdgeclasses();

	//iterate the edgeclasses
	for (l_EIter = l_plEC->begin(); l_EIter != l_plEC->end(); ++l_EIter)
	{
		const SP_ListEdge* l_plElements = (*l_EIter)->GetElements();
		SP_ListEdge::const_iterator l_Iter;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			SP_ListGraphic* l_plGrEdge = (*l_Iter)->GetGraphics();
			SP_ListGraphic::iterator l_GrIter;
			for (l_GrIter = l_plGrEdge->begin(); l_GrIter != l_plGrEdge->end(); ++l_GrIter)
			{
				SP_GR_Edge* l_pcGrEdge = dynamic_cast<SP_GR_Edge*>(*l_GrIter);
				if (l_pcGrEdge->GetNetnumber() == p_nNetNumber)
				{
					EdgeData* l_pcEdgeData = new EdgeData();
					l_pcEdgeData->EdgeGr = l_pcGrEdge;
					l_pcEdgeData->Id = l_pcGrEdge->GetId();
					l_pcEdgeData->SourceId = l_pcGrEdge->GetSource()->GetId();
					l_pcEdgeData->TargetId = l_pcGrEdge->GetTarget()->GetId();
					m_mEdges.insert(make_pair(l_pcEdgeData->Id, l_pcEdgeData));

					if(m_mNodes.find(l_pcEdgeData->SourceId) == m_mNodes.end())
					{
						SP_GR_Node* l_pcGrNode = dynamic_cast<SP_GR_Node*>(l_pcGrEdge->GetSource());
						NodeData* l_pcNodeData = new NodeData();
						l_pcNodeData->NodeGr = l_pcGrNode;
						l_pcNodeData->Id = l_pcGrNode->GetId();
						l_pcNodeData->H = l_pcGrNode->GetHeight();
						l_pcNodeData->W = l_pcGrNode->GetWidth();
						m_mNodes.insert(make_pair(l_pcNodeData->Id, l_pcNodeData));
					}
					if(m_mNodes.find(l_pcEdgeData->TargetId) == m_mNodes.end())
					{
						SP_GR_Node* l_pcGrNode = dynamic_cast<SP_GR_Node*>(l_pcGrEdge->GetTarget());
						NodeData* l_pcNodeData = new NodeData();
						l_pcNodeData->NodeGr = l_pcGrNode;
						l_pcNodeData->Id = l_pcGrNode->GetId();
						l_pcNodeData->H = l_pcGrNode->GetHeight();
						l_pcNodeData->W = l_pcGrNode->GetWidth();
						m_mNodes.insert(make_pair(l_pcNodeData->Id, l_pcNodeData));
					}

				} //end if
			} //end for
		} //end for
	} // end for

}

void SP_Layout::Clear()
{
	MapNodeData::iterator l_itN = m_mNodes.begin();

	while(l_itN != m_mNodes.end())
	{
		NodeData* l_pcNodeData = l_itN->second;
		delete l_pcNodeData;
		m_mNodes.erase(l_itN);
		l_itN = m_mNodes.begin();
	}

	MapEdgeData::iterator l_itE = m_mEdges.begin();

	while(l_itE != m_mEdges.end())
	{
		EdgeData* l_pcEdgeData = l_itE->second;
		delete l_pcEdgeData;
		m_mEdges.erase(l_itE);
		l_itE = m_mEdges.begin();
	}
}

bool SP_Layout::ResetNodes()
{
	//set new position
	MapNodeData::iterator l_itN;
	for(l_itN = m_mNodes.begin(); l_itN != m_mNodes.end(); ++l_itN)
	{
		SP_GR_Node* l_pcGrNode = l_itN->second->NodeGr;

		double l_nX = l_itN->second->X;
		double l_nY = l_itN->second->Y;

		if(l_nX > m_nMaxX) m_nMaxX = l_nX;
		if(l_nY > m_nMaxY) m_nMaxY = l_nY;

		//Set Node Position
		l_pcGrNode->SetPosX(l_nX);
		l_pcGrNode->SetPosY(l_nY);

		//Set Attributes
		SP_ListAttribute::const_iterator l_AIter;
		const SP_ListAttribute* l_plAttributes = l_pcGrNode->GetParent()->GetAttributes();
		for (l_AIter = l_plAttributes->begin(); l_AIter != l_plAttributes->end(); ++l_AIter)
		{
			SP_ListGraphic::iterator l_GrAttrIter;
			SP_ListGraphic* l_plGrAttr = (*l_AIter)->GetGraphics();
			for (l_GrAttrIter = l_plGrAttr->begin(); l_GrAttrIter != l_plGrAttr->end(); l_GrAttrIter++)
			{
				SP_GR_Attribute* l_pcAttr =	dynamic_cast<SP_GR_Attribute*>(*l_GrAttrIter);
				if (l_pcAttr)
				{
					if (l_pcAttr->GetGraphicParent()->GetId() == l_itN->second->Id)
					{
						double l_nOffsetX = l_pcAttr->GetOffsetX();
						double l_nOffsetY = l_pcAttr->GetOffsetY();

						l_pcAttr->SetPosX(l_nX + l_nOffsetX);
						l_pcAttr->SetPosY(l_nY + l_nOffsetY);

						l_pcAttr->Update(TRUE);
					}
				}
			}
		}
		l_pcGrNode->Update(TRUE);
	}
	return true;
}

bool SP_Layout::RefreshNodes()
{
	SP_MDI_View* l_pcView =	dynamic_cast<SP_MDI_View*> (m_pcDoc->GetFirstView());
	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();
	wxClientDC l_cDC(l_pcCanvas);
	l_pcCanvas->DoPrepareDC(l_cDC);

	MapNodeData::iterator l_itN;
	for(l_itN = m_mNodes.begin(); l_itN != m_mNodes.end(); ++l_itN)
	{
		SP_GR_Node* l_pcGrNode = l_itN->second->NodeGr;
		// move it by 0,0
		if (l_pcGrNode->GetPrimitive())
		{
			l_pcGrNode->GetPrimitive()->Move(l_cDC,
					l_pcGrNode->GetPrimitive()->GetX(),
					l_pcGrNode->GetPrimitive()->GetY());
		}
	}
	return TRUE;
}

bool SP_Layout::ResetEdges()
{
	MapEdgeData::iterator l_itE;
	for(l_itE = m_mEdges.begin(); l_itE != m_mEdges.end(); ++l_itE)
	{
		SP_GR_Edge* l_pcEdge = l_itE->second->EdgeGr;
		std::vector<double> l_aX = l_itE->second->X;
		std::vector<double> l_aY = l_itE->second->Y;

		wxList l_lPoints;
		if(l_aX.size() < 2 || l_aY.size() < 2)
		{
			double l_nX, l_nY;
			l_nX = m_mNodes[l_itE->second->SourceId]->X;
			l_nY = m_mNodes[l_itE->second->SourceId]->Y;
			l_lPoints.Append((wxObject*) new wxRealPoint(l_nX, l_nY));
			l_nX = m_mNodes[l_itE->second->TargetId]->X;
			l_nY = m_mNodes[l_itE->second->TargetId]->Y;
			l_lPoints.Append((wxObject*) new wxRealPoint(l_nX, l_nY));
		}
		else
		{
			for(unsigned long i = 0; i < l_aX.size(); ++i)
			{
				l_lPoints.Append((wxObject*) new wxRealPoint(l_aX[i], l_aY[i]));
			}
		}
		l_pcEdge->SetControlPoints(&l_lPoints);

		wxNode* l_pcLNode = l_lPoints.GetFirst();
		while (l_pcLNode)
		{
			wxRealPoint* pt = (wxRealPoint*) l_pcLNode->GetData();
			wxDELETE(pt);
			l_pcLNode = l_pcLNode->GetNext();
		}
		l_pcEdge->Update();
	}
	return TRUE;
}

bool SP_Layout::ResetEdgesAttributes()
{
	MapEdgeData::iterator l_itE;
	for(l_itE = m_mEdges.begin(); l_itE != m_mEdges.end(); ++l_itE)
	{
		SP_GR_Edge* l_pcEdge = l_itE->second->EdgeGr;
		//Set Attributes
		SP_ListAttribute::const_iterator l_AIter;
		const SP_ListAttribute* l_plAttributes = l_pcEdge->GetParent()->GetAttributes();
		for (l_AIter = l_plAttributes->begin(); l_AIter	!= l_plAttributes->end(); ++l_AIter)
		{
			SP_ListGraphic::iterator l_GrAttrIter;
			SP_ListGraphic* l_plGrAttr = (*l_AIter)->GetGraphics();
			for (l_GrAttrIter = l_plGrAttr->begin(); l_GrAttrIter != l_plGrAttr->end(); l_GrAttrIter++)
			{
				SP_GR_Attribute* l_pcAttr =	dynamic_cast<SP_GR_Attribute*>(*l_GrAttrIter);
				if (l_pcAttr->GetGraphicParent()->GetId() == l_itE->second->Id)
				{
					l_pcAttr->SetPosX((l_pcAttr->GetGraphicParent())->GetPosX());
					l_pcAttr->SetPosY((l_pcAttr->GetGraphicParent())->GetPosY());
					l_pcAttr->SetOffsetX(0.0);
					l_pcAttr->SetOffsetY(0.0);
				} //end if
			} //end for
		}//endfor
	}

	return TRUE;
}

