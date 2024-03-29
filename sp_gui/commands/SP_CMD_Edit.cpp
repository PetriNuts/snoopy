#include "SP_CMD_Edit.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_core/SP_Core.h"

SP_CMD_Edit::SP_CMD_Edit(int p_nCmdId, SP_ListGraphic *p_graphics, const wxString& p_sName) :
	wxCommand(true, p_sName), m_pcSource(0), m_pcDelGraph(0), m_nCmdId(p_nCmdId)
{
	if (!m_pcSource)
	{
		m_pcSource = SP_Core::Instance()->GetRootDocument()->GetGraph();
	}
	if (p_graphics && !p_graphics->empty())
	{
		//p_graphics won't exist anymore after DoDelete()
		//so we need our own reference to be able to use it in any redo()-call (Ctrl+Y)
		SP_ListGraphic::iterator it;
		for (it = p_graphics->begin(); it != p_graphics->end(); ++it)
		{
			//this should've been checked before, but you never know...
			if(SP_Find(m_lcGraphics, *it) == m_lcGraphics.end())
			{
				if (((*it)->GetGraphicType() == SP_GRAPHIC_NODE	||
					 (*it)->GetGraphicType() == SP_GRAPHIC_METADATA))
				{
					m_lcGraphics.push_front(*it);
				}
				else if ((*it)->GetGraphicType() == SP_GRAPHIC_EDGE)
				{
					m_lcGraphics.push_back(*it);
				}
			}
		}
	}
}

SP_CMD_Edit::SP_CMD_Edit(int p_nCmdId, SP_Graphic *p_graphic, const wxString& p_sName) :
	wxCommand(true, p_sName), m_pcSource(0), m_pcDelGraph(0), m_nCmdId(p_nCmdId)
{
	if (p_graphic &&
		(p_graphic->GetGraphicType() == SP_GRAPHIC_NODE ||
		 p_graphic->GetGraphicType() == SP_GRAPHIC_EDGE ||
		 p_graphic->GetGraphicType() == SP_GRAPHIC_METADATA))
	{
		m_lcGraphics.push_back(p_graphic);
		m_pcSource = SP_Core::Instance()->GetRootDocument()->GetGraph();
	}
}

SP_CMD_Edit::~SP_CMD_Edit()
{
	//clean up the saved elements
	if (m_pcDelGraph)
	{
		//don't delete the document of coarse nodes, since it's not our own (belongs to the original graph)
		//set the document pointer to null instead
		for (list<SP_DS_Coarse*>::iterator itC = m_lCoarseNodes.begin(); itC != m_lCoarseNodes.end(); ++itC)
		{
			(*itC)->SetCoarseDoc(0);
		}
		//the rest we can delete because it was removed from the original graph
		wxDELETE(m_pcDelGraph);
	}
}

bool SP_CMD_Edit::Do()
{
	switch (m_nCmdId)
	{
	case SP_CMD_COPY:
		SP_LOGDEBUG(wxT("Doing COPY..."))
		;
		//code for wxT("copy") comes here...
		break;
	case SP_CMD_CUT:
		SP_LOGDEBUG(wxT("Doing CUT..."))
		;
		//code for wxT("cut") comes here...
		break;
	case SP_CMD_PASTE:
		SP_LOGDEBUG(wxT("Doing PASTE..."))
		;
		//code for wxT("paste") comes here...
		break;
	case SP_CMD_DELETE:
		return DoDelete();
	case SP_CMD_DELETE_ALL:
		return DoDelete(true);
	case SP_CMD_HIDE:
		return DoHide();
	case SP_CMD_HIDE_ALL:
		return DoHide(true);
	case SP_CMD_UNHIDE:
		return DoUnHide();
	case SP_CMD_UNHIDE_ALL:
		return DoUnHide(true);
	}

	return false;
}

bool SP_CMD_Edit::Undo()
{
	switch (m_nCmdId)
	{
	case SP_CMD_COPY:
		SP_LOGDEBUG(wxT("Undoing COPY..."))
		;
		//code for undoing wxT("copy") comes here...
		break;
	case SP_CMD_CUT:
		SP_LOGDEBUG(wxT("Undoing CUT..."))
		;
		//code for undoing wxT("cut") comes here...
		break;
	case SP_CMD_PASTE:
		SP_LOGDEBUG(wxT("Undoing PASTE..."))
		;
		//code for undoing wxT("paste") comes here...
		break;
	case SP_CMD_DELETE:
	case SP_CMD_DELETE_ALL:
		return UndoDelete();
	case SP_CMD_HIDE:
		return UndoHide();
	case SP_CMD_UNHIDE:
		return UndoUnHide();

	}

	return false;
}

bool SP_CMD_Edit::DoDelete(bool p_bDeleteAll)
{
	//SP_LOGDEBUG(wxT("entered DoDelete()..."));

	if (!m_pcSource)
		return false;

	//prepare the graph we will copy to
	m_pcDelGraph = m_pcSource->CloneDefinition();
	if (!m_pcDelGraph)
		return false;
	m_pcDelGraph->SetIsCopy(true);
	m_mGraphic2Parent.clear();

	SP_ListGraphic::iterator itG;
	for (itG = m_lcGraphics.begin(); itG != m_lcGraphics.end(); ++itG)
	{
		SP_Graphic* l_pcGr = *itG;
		SP_Data *l_pcParent = l_pcGr->GetParent();
		//every graphic should have a parent, so this shouldn't happen, but check it anyway
		if (!l_pcParent)
			continue;

		if (!p_bDeleteAll && (l_pcParent->GetGraphics()->size() > 1) )//&& (l_pcParent->GetElementType() != SP_ELEMENT_EDGE))
		{
			//if there is more than one graphic, we just remove the current one...
			//and remember the connection to its parent

			//if the graphic represents an edge
			if(l_pcGr->GetGraphicType() == SP_GRAPHIC_EDGE)
			{
				//has a parent outside the coarse node
				//has any graphics in coarse nodes they should be deleted, too
				if (l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
					|| l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
				{
					for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
					{
						if ((l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP
							|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				} //end if
			}
			else if(l_pcGr->GetGraphicType() == SP_GRAPHIC_NODE)
			{
				//loose edges are not allowed, so we remove them
				AppendEdges(dynamic_cast<SP_DS_Node*> (l_pcParent)->GetSourceEdges(), l_pcGr);
				AppendEdges(dynamic_cast<SP_DS_Node*> (l_pcParent)->GetTargetEdges(), l_pcGr);

				for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
				{
					if (l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
						|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
					{
						if (!l_pcGr2->HasUndeletedEdges()
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				}
			}

			//when a node is logical, then remove the current graphic of the node only
			//and remove the graphics from the child attributes too

			if (l_pcParent->GetLogical())
			{
			    for(auto itC = l_pcGr->GetGraphicChildren()->begin();
			    		itC != l_pcGr->GetGraphicChildren()->end();
			    		itC = l_pcGr->GetGraphicChildren()->begin())
				{
			    	SP_Graphic* l_pcGraphicChild = *itC;
					if (l_pcGraphicChild)
					{
						SP_Data* l_pcAttr = l_pcGraphicChild->GetParent();
						// add the attribute graphics to the map for undo-ing
						m_mGraphic2Parent[l_pcGraphicChild] = l_pcAttr;
						//remove the graphics of the child attributes
						l_pcGraphicChild->RemoveFromCanvas();
						l_pcAttr->RemoveGraphic(l_pcGraphicChild, false);
					}
				}
			} // end if

			//finally removing the graphic itself
			m_mGraphic2Parent[l_pcGr] = l_pcParent;
			l_pcGr->RemoveFromCanvas();
			l_pcParent->RemoveGraphic(l_pcGr, false);

		}
		else
		{
			//wxT("delete all") means we remove the parent element completely (including all graphics)
			//the same applies if the current graphic is the only one of its parent


			RemoveGraphicsFromCanvas(l_pcParent);

			//now actually remove the elements from the graph
			//and with it automatically all of its graphics
			switch (l_pcParent->GetElementType())
			{
			case SP_ELEMENT_NODE:
			{
				SP_DS_Node *l_pcNodeCopy = dynamic_cast<SP_DS_Node*> (l_pcParent);
				//append future loose edges to the list of data to delete
				AppendEdges(l_pcNodeCopy->GetSourceEdges());
				AppendEdges(l_pcNodeCopy->GetTargetEdges());
				//if we talk about coarse, append its subelements as well
				AppendCoarse(l_pcParent->GetCoarse());
				//transfer the data element to our DelGraph
				m_pcDelGraph->AddElement(l_pcNodeCopy);
				break;
			}
			case SP_ELEMENT_EDGE:
			{
				SP_DS_Edge *l_pcEdgeCopy = dynamic_cast<SP_DS_Edge*> (l_pcParent);
				SP_Data* l_pcNode = l_pcEdgeCopy->GetTarget();
				l_pcNode->RemoveTargetEdge(l_pcEdgeCopy);
				l_pcEdgeCopy->SetTarget(l_pcNode);

				l_pcNode = l_pcEdgeCopy->GetSource();
				l_pcNode->RemoveSourceEdge(l_pcEdgeCopy);
				l_pcEdgeCopy->SetSource(l_pcNode);

				//transfer the data element to our DelGraph
				m_pcDelGraph->AddElement(l_pcEdgeCopy);
				break;
			}
			case SP_ELEMENT_METADATA:
			{
				//transfer the data element to our DelGraph
				m_pcDelGraph->AddElement(l_pcParent);
				break;
			}
			default:
				break;
			}
		}
	}
	CleanUpCoarseNodes();

	m_pcSource->GetParentDoc()->Refresh();
	m_pcSource->GetParentDoc()->Modify(true);

	return true;
}

bool SP_CMD_Edit::UndoDelete()
{
	//SP_LOGDEBUG(wxT("entered UndoDelete()..."));

	if (!m_pcDelGraph || !m_pcSource)
		return false;

	//we just move back the elements stored in m_pcDelGraph
	SP_ListNodeclass::const_iterator itNC;
	SP_ListNode::const_iterator itN;
	//nodes...
	for (itNC = m_pcDelGraph->GetNodeclasses()->begin(); itNC != m_pcDelGraph->GetNodeclasses()->end(); ++itNC)
	{
		itN = (*itNC)->GetElements()->begin();
		while (itN != (*itNC)->GetElements()->end())
		{
			SP_DS_Node *l_pcNode = *itN;

			if (!m_pcSource->GetNetclass()->NodeRequirement(l_pcNode))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to noderequirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}

			m_pcSource->AddElement(l_pcNode);
			//if we got a coarse node we need to reset it
			SP_DS_Coarse *l_pcCoarse = l_pcNode->GetCoarse();
			if (l_pcCoarse)
			{
				l_pcCoarse->SetUpdate(false);
				l_pcCoarse->Update(false, false);
			}
			itN = (*itNC)->GetElements()->begin();
		}
	}
	//edges...
	SP_ListEdgeclass::const_iterator itEC;
	SP_ListEdge::const_iterator itE;
	for (itEC = m_pcDelGraph->GetEdgeclasses()->begin(); itEC != m_pcDelGraph->GetEdgeclasses()->end(); ++itEC)
	{
		itE = (*itEC)->GetElements()->begin();
		SP_DS_Edgeclass* l_pcEdgeclass = (*itEC);
		while (itE != (*itEC)->GetElements()->end())
		{
			SP_DS_Edge *l_pcEdge = *itE;

			if (!m_pcSource->GetNetclass()->EdgeRequirement(l_pcEdgeclass, l_pcEdge->GetSource(), l_pcEdge->GetTarget()))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to edgerequirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}

			m_pcSource->AddElement(l_pcEdge);

			// set the links to this edge in the corresponding nodes.
			SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetSource());
			l_pcNode->AddSourceEdge(l_pcEdge);

			l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetTarget());
			l_pcNode->AddTargetEdge(l_pcEdge);

			itE = (*itEC)->GetElements()->begin();
		}
	}
	SP_ListMetadataclass::const_iterator itMC;
	SP_ListMetadata::const_iterator itM;
	//metadata...
	for (itMC = m_pcDelGraph->GetMetadataclasses()->begin(); itMC != m_pcDelGraph->GetMetadataclasses()->end(); ++itMC)
	{
		itM = (*itMC)->GetElements()->begin();
		while (itM != (*itMC)->GetElements()->end())
		{
			SP_DS_Metadata *l_pcMetadata = *itM;

			if (!m_pcSource->GetNetclass()->MetadataRequirement(l_pcMetadata))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to metadatarequirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
			m_pcSource->AddElement(l_pcMetadata);

			itM = (*itMC)->GetElements()->begin();
		}
	}

	//putting back the loose coarse graphics we removed from their parents
	for (map<SP_Graphic*, SP_Data*>::iterator it = m_mGraphic2Parent.begin(); it != m_mGraphic2Parent.end(); ++it)
	{
		(*it).second->AddGraphic((*it).first);
	}

	//Refresh() doesn't work here, because we haven't added any RefreshRects to the canvas
	//so we need to actually redraw all elements
	m_pcSource->GetParentDoc()->DrawAllElements(false, true);

	//clean up m_pcDelGraph
	//though, at this point, it should be empty anyway
	wxDELETE(m_pcDelGraph);
	m_pcDelGraph = 0;

	return true;
}

void SP_CMD_Edit::AppendEdges(const SP_ListEdge *p_pcEdges, SP_Graphic *p_pcGrNode)
{
	if (!p_pcEdges)
		return;

	for (SP_DS_Edge* l_pcEdge : *p_pcEdges)
	{
		if(EdgeInGraph(l_pcEdge))
		{
			//if this edge is still part of the graph append all its graphics to the delete-list
			for (SP_Graphic* l_pcGr : *l_pcEdge->GetGraphics())
			{
				SP_GR_Edge* l_pcEdgeGraphic = dynamic_cast<SP_GR_Edge*>(l_pcGr);
				SP_Graphic* l_pcSourceGraphic = l_pcEdgeGraphic->GetSource();
				SP_Graphic* l_pcTargetGraphic = l_pcEdgeGraphic->GetTarget();

				if (p_pcGrNode && l_pcSourceGraphic != p_pcGrNode && l_pcTargetGraphic != p_pcGrNode)
				{
					continue;
				}
				bool l_bIsSource = FALSE;
				if (l_pcSourceGraphic == p_pcGrNode)
				{
					l_bIsSource = TRUE;
				}
				if (SP_Find(m_lcGraphics, l_pcGr) == m_lcGraphics.end())
				{
					m_lcGraphics.push_back(l_pcGr);

					if (l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
						|| l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
					{
						SP_Data *l_pcParent = l_pcGr->GetParent();
						//every graphic should have a parent, so this shouldn't happen, but check it anyway
						if (!l_pcParent)
							continue;

						for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
						{
							if (l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
								&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
							{
								m_lcGraphics.push_back(l_pcGr2);
								SP_GR_Edge* l_pcGraphicEdge = dynamic_cast<SP_GR_Edge*> (l_pcGr2);
								if(l_bIsSource)
								{
									if(SP_Find(m_lcGraphics, l_pcGraphicEdge->GetSource()) == m_lcGraphics.end())
									{
										m_lcGraphics.push_back(l_pcGraphicEdge->GetSource());
									}
								}
								else
								{
									if(SP_Find(m_lcGraphics, l_pcGraphicEdge->GetTarget()) == m_lcGraphics.end())
									{
										m_lcGraphics.push_back(l_pcGraphicEdge->GetTarget());
									}
								}
							} //end if
						} //end for
					} //end if
				} //end if
			}//end for
		} //end if
	} //end for
} //end AppendEdges

void SP_CMD_Edit::AppendHiddenEdges(SP_Graphic *p_pcGrNode)
{
	wxNode* l_pcNode = p_pcGrNode->GetPrimitive()->GetLines().GetFirst();
	wxLineShape* l_pcLine;
	SP_GR_Edge* edge;
	while (l_pcNode)
	{
		l_pcLine = (wxLineShape*)l_pcNode->GetData();
		edge = dynamic_cast<SP_GR_Edge*>(SP_Core::Instance()->ResolveExtern(l_pcLine));

		if(edge->GetHide())
		{
			m_lcGraphics.push_back(edge);
		}

		l_pcNode = l_pcNode->GetNext();
	}
}

void SP_CMD_Edit::AppendHiddenEdges(SP_Data *p_pcNode)
{
	for(SP_Graphic* l_pcGr : *p_pcNode->GetGraphics())
	{
		AppendHiddenEdges(l_pcGr);
	}
}

void SP_CMD_Edit::AppendCoarse(SP_DS_Coarse *p_pcCoarse)
{

	if (!p_pcCoarse)
		return;

	SP_MDI_CoarseDoc* l_pcDoc = p_pcCoarse->GetCoarseDoc();
	if(!l_pcDoc)
	{
		p_pcCoarse->SetUpdate(false);
		p_pcCoarse->Show();
		p_pcCoarse->GetCoarseDoc()->SetClose(false);
		p_pcCoarse->GetCoarseDoc()->Modify(FALSE);
		p_pcCoarse->GetCoarseDoc()->Close();
	}
	//we remember the deleted coarse nodes (for special treatment later on)
	m_lCoarseNodes.push_front(p_pcCoarse);

	SP_ListGraphic l_lCoarseGraphics;

	//retrieving graphics in coarse node
	p_pcCoarse->GetParent()->GetClassObject()->GetParentGraph()->GetGraphicsInNet(&l_lCoarseGraphics, p_pcCoarse->GetNetnumber(), SP_ELEMENT_NODE | SP_ELEMENT_EDGE);

	for (SP_Graphic* l_pcGr : l_lCoarseGraphics)
	{
		//appending the just collected graphics to the delete-list
		if (SP_Find(m_lcGraphics, l_pcGr) == m_lcGraphics.end())
			m_lcGraphics.push_back(l_pcGr);
	}
}

void SP_CMD_Edit::RemoveGraphicsFromCanvas(SP_Data *p_pcData)
{
	//SP_LOGDEBUG(wxT("entered RemoveGraphics()...") + p_pcData->GetClassName());

	if (!p_pcData || !p_pcData->GetGraphics())
		return;

	//we don't actually remove any graphics from the graph
	//we just remove them from canvas
	for (SP_Graphic* l_pcGr : *p_pcData->GetGraphics())
	{
		RemoveGraphicFromCanvas(l_pcGr);
	}
}

void SP_CMD_Edit::RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic)
{
	//SP_LOGDEBUG(wxT("entered RemoveGraphicFromCanvas(graphic)..."));

	p_pcGraphic->RemoveFromCanvas();
	//also removing graphic children (e.g. token graphics, labels)
	SP_ListGraphic *gr = p_pcGraphic->GetGraphicChildren();
	if (!gr)
		return;
	SP_ListGraphic::iterator itg;
	for (SP_Graphic* l_pcGr : *gr)
	{
		RemoveGraphicFromCanvas(l_pcGr);
	}
}

bool SP_CMD_Edit::EdgeInGraph(SP_DS_Edge *p_pcEdge)
{

	if (!p_pcEdge || !m_pcSource)
		return false;

	//we get here with all source (target) edges of one node
	//but some of them may not be part of the original graph anymore
	//so this makes sure that we only take those that are really still there
	SP_DS_Edgeclass *l_pcEC = m_pcSource->GetEdgeclass(p_pcEdge->GetClassName());
	if (!l_pcEC)
		return false;

	for (SP_DS_Edge* l_pcEdge : *l_pcEC->GetElements())
	{
		if (l_pcEdge == p_pcEdge)
		{
			return true;
		}
	}

	return false;
}

void SP_CMD_Edit::CleanUpCoarseNodes()
{
	for (SP_DS_Coarse* l_pcCoarse : m_lCoarseNodes)
	{
		//removing the deleted coarse node from the coarse tree...
		wxTreeItemId l_nCoarseId = l_pcCoarse->GetCoarseId();
		m_pcSource->CreateCoarseTree()->InvalidateAllChildren(l_nCoarseId);
		m_pcSource->CreateCoarseTree()->Delete(l_nCoarseId);
		SP_MDI_CoarseDoc *l_pcCoarseDoc = l_pcCoarse->GetCoarseDoc();
		//... and closing the associated document
		if (l_pcCoarseDoc)
		{
			l_pcCoarseDoc->SetClose(TRUE);
			l_pcCoarseDoc->Modify(FALSE);
			l_pcCoarseDoc->Close();
			l_pcCoarseDoc->DeleteAllViews();
			l_pcCoarse->SetCoarseDoc(0);
		}
	}
}

bool SP_CMD_Edit::DoHide(bool p_bHideAll)
{
	SP_LOGDEBUG(wxT("entered DoHide()..."));

	if (!m_pcSource)
		return false;

	//prepare the graph we will copy to
	m_pcDelGraph = m_pcSource->CloneDefinition();
	if (!m_pcDelGraph)
		return false;
	m_pcDelGraph->SetIsCopy(true);

	SP_ListGraphic::iterator itG;
	for (itG = m_lcGraphics.begin(); itG != m_lcGraphics.end(); ++itG)
	{
		SP_Graphic* l_pcGr = *itG;
		SP_Data *l_pcParent = l_pcGr->GetParent();
		//every graphic should have a parent, so this shouldn't happen, but check it anyway
		if (!l_pcParent || l_pcGr->GetHide() == true)
			continue;

		if (!p_bHideAll && (l_pcParent->GetGraphics()->size() > 1) )
		{
			//if there is more than one graphic, we just hide the current one...
			//and remember the connection to its parent

			//if the graphic represents an edge
			if(l_pcGr->GetGraphicType() == SP_GRAPHIC_EDGE)
			{
				//has a parent outside the coarse node
				//has any graphics in coarse nodes they should be hidden, too
				if (l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
					|| l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
				{
					for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
					{
						if ((l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP
							|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				} //end if
			}
			else if(l_pcGr->GetGraphicType() == SP_GRAPHIC_NODE)
			{
				//loose edges are not allowed, so we hide them
				AppendEdges(dynamic_cast<SP_DS_Node*> (l_pcParent)->GetSourceEdges(), l_pcGr);
				AppendEdges(dynamic_cast<SP_DS_Node*> (l_pcParent)->GetTargetEdges(), l_pcGr);

				for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
				{
					if (l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
						|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
					{
						if (!l_pcGr2->HasUndeletedEdges()
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				}
			}

			//when a node is logical, then hide the current graphic of the node only
			//and hide the graphics from the child attributes too

			if (l_pcParent->GetLogical())
			{
			    for(SP_Graphic* l_pcGraphicChild : *l_pcGr->GetGraphicChildren())
				{
					if (l_pcGraphicChild)
					{
						l_pcGraphicChild->SetHide(true);
					}
				}
			} // end if

			//finally hide the graphic itself
			l_pcGr->SetHide(true);

		}
		else
		{
			//wxT("hide all") means we hide the parent element completely (including all graphics)
			//the same applies if the current graphic is the only one of its parent
			m_pcSource->SetHide(l_pcParent, true);
			switch (l_pcParent->GetElementType())
			{
			case SP_ELEMENT_NODE:
			{
				SP_DS_Node *l_pcNodeCopy = dynamic_cast<SP_DS_Node*> (l_pcParent);
				AppendEdges(l_pcNodeCopy->GetSourceEdges());
				AppendEdges(l_pcNodeCopy->GetTargetEdges());
				//if we talk about coarse, append its subelements as well
				AppendCoarse(l_pcParent->GetCoarse());
				//transfer the data element to our DelGraph
				m_pcDelGraph->AddElement(l_pcNodeCopy);
				break;
			}
			case SP_ELEMENT_EDGE:
			{
				SP_DS_Edge *l_pcEdgeCopy = dynamic_cast<SP_DS_Edge*> (l_pcParent);

				SP_Data* l_pcSource = l_pcEdgeCopy->GetSource();
				l_pcSource->RemoveSourceEdge(l_pcEdgeCopy);
				l_pcEdgeCopy->SetSource(l_pcSource);

				SP_Data* l_pcTarget = l_pcEdgeCopy->GetTarget();
				l_pcTarget->RemoveTargetEdge(l_pcEdgeCopy);
				l_pcEdgeCopy->SetTarget(l_pcTarget);

				m_pcDelGraph->AddElement(l_pcEdgeCopy);
				break;
			}
			case SP_ELEMENT_METADATA:
			{
				//transfer the data element to our DelGraph
				m_pcDelGraph->AddElement(l_pcParent);
				break;
			}
			default:
				break;
			}
		}
	}

	m_pcSource->GetParentDoc()->DrawAllElements(false, true);

	return true;
}

bool SP_CMD_Edit::UndoHide()
{
	SP_LOGDEBUG(wxT("entered UndoHide()..."));

	if (!m_pcDelGraph || !m_pcSource)
		return false;

	//we just move back the elements stored in m_pcDelGraph
	SP_ListNodeclass::const_iterator itNC;
	SP_ListNode::const_iterator itN;
	//nodes...
	for (itNC = m_pcDelGraph->GetNodeclasses()->begin(); itNC != m_pcDelGraph->GetNodeclasses()->end(); ++itNC)
	{
		itN = (*itNC)->GetElements()->begin();
		while (itN != (*itNC)->GetElements()->end())
		{
			SP_DS_Node *l_pcNode = *itN;

			if (!m_pcSource->GetNetclass()->NodeRequirement(l_pcNode))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to node requirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
			m_pcSource->AddElement(l_pcNode);
			//if we got a coarse node we need to reset it
			SP_DS_Coarse *l_pcCoarse = l_pcNode->GetCoarse();
			if (l_pcCoarse)
			{
				l_pcCoarse->SetUpdate(false);
				l_pcCoarse->Update(false, false);
			}
			m_pcSource->SetHide(l_pcNode, false);
			itN = (*itNC)->GetElements()->begin();
		}
	}
	//edges...
	SP_ListEdgeclass::const_iterator itEC;
	SP_ListEdge::const_iterator itE;
	for (itEC = m_pcDelGraph->GetEdgeclasses()->begin(); itEC != m_pcDelGraph->GetEdgeclasses()->end(); ++itEC)
	{
		itE = (*itEC)->GetElements()->begin();
		SP_DS_Edgeclass* l_pcEdgeclass = (*itEC);
		while (itE != (*itEC)->GetElements()->end())
		{
			SP_DS_Edge *l_pcEdge = *itE;

			if (!m_pcSource->GetNetclass()->EdgeRequirement(l_pcEdgeclass, l_pcEdge->GetSource(), l_pcEdge->GetTarget()))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to edge requirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
			m_pcSource->AddElement(l_pcEdge);
			// set the links to this edge in the corresponding nodes.
			SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetSource());
			l_pcNode->AddSourceEdge(l_pcEdge);

			l_pcNode = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetTarget());
			l_pcNode->AddTargetEdge(l_pcEdge);

			m_pcSource->SetHide(l_pcEdge, false);
			itE = (*itEC)->GetElements()->begin();
		}
	}
	SP_ListMetadataclass::const_iterator itMC;
	SP_ListMetadata::const_iterator itM;
	//Metadatas...
	for (itMC = m_pcDelGraph->GetMetadataclasses()->begin(); itMC != m_pcDelGraph->GetMetadataclasses()->end(); ++itMC)
	{
		itM = (*itMC)->GetElements()->begin();
		while (itM != (*itMC)->GetElements()->end())
		{
			SP_DS_Metadata *l_pcMetadata = *itM;

			if (!m_pcSource->GetNetclass()->MetadataRequirement(l_pcMetadata))
			{
				SP_MESSAGEBOX(wxT(" Undo not possible due to node requirement in netclass"), wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
			m_pcSource->AddElement(l_pcMetadata);
			m_pcSource->SetHide(l_pcMetadata, false);
			itM = (*itMC)->GetElements()->begin();
		}
	}

	m_pcSource->GetParentDoc()->DrawAllElements(false, true);

	//clean up m_pcDelGraph
	//though, at this point, it should be empty anyway
	wxDELETE(m_pcDelGraph);

	return true;
}

bool SP_CMD_Edit::DoUnHide(bool p_bUnHideAll)
{
	SP_LOGDEBUG(wxT("entered DoUnHide()..."));

	if (!m_pcSource)
		return false;

	SP_ListGraphic::iterator itG;
	for (itG = m_lcGraphics.begin(); itG != m_lcGraphics.end(); ++itG)
	{
		SP_Graphic* l_pcGr = *itG;
		SP_Data *l_pcParent = l_pcGr->GetParent();
		//every graphic should have a parent, so this shouldn't happen, but check it anyway
		if (!l_pcParent || l_pcGr->GetHide() == false)
			continue;

		if (!p_bUnHideAll && (l_pcParent->GetGraphics()->size() > 1) )
		{
			//if there is more than one graphic, we just unhide the current one...
			//and remember the connection to its parent

			//if the graphic represents an edge
			if(l_pcGr->GetGraphicType() == SP_GRAPHIC_EDGE)
			{
				//has a parent outside the coarse node
				//has any graphics in coarse nodes they should be unhide, too
				if (l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
					|| l_pcGr->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
				{
					for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
					{
						if ((l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP
							|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN)
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				} //end if
			}
			else if(l_pcGr->GetGraphicType() == SP_GRAPHIC_NODE)
			{
				AppendHiddenEdges(l_pcGr);

				for (SP_Graphic* l_pcGr2 : *l_pcParent->GetGraphics())
				{
					if (l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERDOWN
						|| l_pcGr2->GetGraphicState() == SP_STATE_COARSEBORDERTOP)
					{
						if (!l_pcGr2->HasUndeletedEdges()
							&& SP_Find(m_lcGraphics, l_pcGr2) == m_lcGraphics.end())
						{
							m_lcGraphics.push_back(l_pcGr2);
						} //end if
					} //end for
				}
			}

			//when a node is logical, then unhide the current graphic of the node only
			//and unhide the graphics from the child attributes too

			if (l_pcParent->GetLogical())
			{
			    for(SP_Graphic* l_pcGraphicChild : *l_pcGr->GetGraphicChildren())
				{
					if (l_pcGraphicChild)
					{
						l_pcGraphicChild->SetHide(false);
					}
				}
			} // end if

			//finally unhide the graphic itself
			l_pcGr->SetHide(false);

			//if the parent is hidden, add it to the graph and unhide
			if(l_pcParent->GetHide())
			{
				m_pcSource->AddElement(l_pcParent);
				m_pcSource->SetHide(l_pcParent, false, false);
			}
		}
		else
		{
			switch (l_pcParent->GetElementType())
			{
			case SP_ELEMENT_NODE:
			{
				SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*>(l_pcParent);
				if (!m_pcSource->GetNetclass()->NodeRequirement(l_pcNode))
				{
					SP_LOGERROR(wxT(" Unhide not possible due to node requirement in netclass"));
					return FALSE;
				}
				AppendCoarse(l_pcParent->GetCoarse());
				AppendHiddenEdges(l_pcParent);
				m_pcSource->AddElement(l_pcNode);
				//if we got a coarse node we need to reset it
				SP_DS_Coarse *l_pcCoarse = l_pcNode->GetCoarse();
				if (l_pcCoarse)
				{
					l_pcCoarse->SetUpdate(false);
					l_pcCoarse->Update(false, false);
				}
				break;
			}
			case SP_ELEMENT_EDGE:
			{
				SP_DS_Edge *l_pcEdge = dynamic_cast<SP_DS_Edge*> (l_pcParent);
				SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetSource());
				SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*> (l_pcEdge->GetTarget());

				if (!m_pcSource->GetNetclass()->EdgeRequirement(l_pcEdge->GetEdgeclass(), l_pcSource, l_pcTarget)
					|| l_pcSource->GetHide() || l_pcTarget->GetHide())
				{
					SP_LOGERROR(wxT(" Unhide not possible due to edge requirement in netclass"));
					return FALSE;
				}
				m_pcSource->AddElement(l_pcEdge);
				// set the links to this edge in the corresponding nodes.
				l_pcSource->AddSourceEdge(l_pcEdge);
				l_pcTarget->AddTargetEdge(l_pcEdge);
				break;
			}
			case SP_ELEMENT_METADATA:
			{
				SP_DS_Metadata* l_pcMetadata = dynamic_cast<SP_DS_Metadata*>(l_pcParent);
				if (!m_pcSource->GetNetclass()->MetadataRequirement(l_pcMetadata))
				{
					SP_LOGERROR(wxT(" Unhide not possible due to metadata requirement in netclass"));
					return FALSE;
				}
				m_pcSource->AddElement(l_pcMetadata);
				break;
			}
			default:
				break;
			}
			m_pcSource->SetHide(l_pcParent, false);
		}
	}

	m_pcSource->GetParentDoc()->DrawAllElements(false, true);

	return false;
}

bool SP_CMD_Edit::UndoUnHide()
{
	SP_LOGDEBUG(wxT("entered UndoUnHide()..."));

	if (!m_pcDelGraph || !m_pcSource)
		return false;

	return true;
}
