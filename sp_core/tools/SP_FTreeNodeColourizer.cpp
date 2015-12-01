//////////////////////////////////////////////////////////////////////
// $Source: kwinder
// $Author (modification): akurth $
// $Version: 1.0 $
// $Date: 2006/08/18 $
// Short Description: toolbox to colourize temporary some nodes
//////////////////////////////////////////////////////////////////////

#include "sp_core/tools/SP_FTreeNodeColourizer.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include "snoopy.h"

SP_FTreeNodeColourizer::SP_FTreeNodeColourizer(SP_DS_Graph *p_pcGraph)
:
	m_pcMapNodeNum2wxColour(NULL),
	m_pcAllNodes(NULL)

{
  m_pcMapNodeNum2wxColour = new map<SP_DS_Node*, SP_PairColour*>();
  m_pcAllNodes = new SP_ListNode();

}

SP_FTreeNodeColourizer::~SP_FTreeNodeColourizer()
{
	wxDELETE(m_pcMapNodeNum2wxColour);
	wxDELETE(m_pcAllNodes);

}

void
SP_FTreeNodeColourizer::ColourNodes(SP_ListNode* m_nodesToColour, wxColour p_pcCol)
{

	CHECK_POINTER(m_nodesToColour, return);

    if(m_nodesToColour->size() != 0){
 		m_nodeIt = m_nodesToColour->begin();
		for (; m_nodeIt != m_nodesToColour->end(); m_nodeIt++) {

			SaveColour((*m_nodeIt));
			ColourGraphic((*m_nodeIt), p_pcCol, true);
		}
	}
}

void
SP_FTreeNodeColourizer::ColourNode(SP_DS_Node* m_nodeToColour, wxColour p_pcCol)
{
	CHECK_POINTER(m_nodeToColour, return);

  	SaveColour((m_nodeToColour));
	ColourGraphic((m_nodeToColour), p_pcCol, true);
}


bool
SP_FTreeNodeColourizer::SaveColour(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(m_pcMapNodeNum2wxColour, return FALSE);

	SP_ListGraphic* sGraphic = p_pcNode->GetGraphics();
	SP_ListGraphic::iterator sIt;
	sIt = sGraphic->begin();

	wxColour l_pcColBrush = (*sIt)->GetBrush()->GetColour();

	wxColour l_pcColPen =	(*sIt)->GetPen()->GetColour();

	SP_PairColour *l_pPair = new SP_PairColour(
			l_pcColBrush, l_pcColPen);

	m_pcMapNodeNum2wxColour->insert(pair< SP_DS_Node*,
										SP_PairColour*>
											(p_pcNode, l_pPair ));
	m_pcAllNodes->push_back(dynamic_cast<SP_DS_Node*>(p_pcNode));
	return true;
}
void
SP_FTreeNodeColourizer::RestoreColourNode(SP_DS_Node* node)
{
	map<SP_DS_Node*, SP_PairColour*>* m_pcMap = new map<SP_DS_Node*, SP_PairColour*>();

	CHECK_POINTER(m_pcMapNodeNum2wxColour, return);
	CHECK_POINTER(m_pcAllNodes, return);
	if (m_pcMapNodeNum2wxColour->size() == 0){
		return;
	}
	if (m_pcAllNodes->size() == 0){
		return;
	}

	map<SP_DS_Node*, SP_PairColour*>::iterator l_pcIt = m_pcMapNodeNum2wxColour->begin();

	while(l_pcIt != m_pcMapNodeNum2wxColour->end())
	{
		if ((*l_pcIt).first == node){
			RestoreOneNodeColour((*l_pcIt).first, (*l_pcIt).second);
		}else{
			m_pcMap->insert(pair< SP_DS_Node*,
										SP_PairColour*>
											((*l_pcIt).first, (*l_pcIt).second ));
		}
		l_pcIt++;
	}
	m_pcMapNodeNum2wxColour->clear();
	m_pcMapNodeNum2wxColour = m_pcMap;
	SP_ListNode::const_iterator nIt;
	SP_ListNode* tempList = new SP_ListNode();
	//delete node from the list
	for (nIt = m_pcAllNodes->begin(); nIt != m_pcAllNodes->end(); ++nIt) {
		if ((*nIt) != node)
			tempList->push_back((*nIt));
	}
	m_pcAllNodes->clear();
	m_pcAllNodes = tempList;
}

void
SP_FTreeNodeColourizer::RestoreColours()
{

	CHECK_POINTER(m_pcMapNodeNum2wxColour, return);
	CHECK_POINTER(m_pcAllNodes, return);
	if (m_pcMapNodeNum2wxColour->size() == 0){
		return;
	}
	if (m_pcAllNodes->size() == 0){
		return;
	}

	map<SP_DS_Node*, SP_PairColour*>::iterator l_pcIt = m_pcMapNodeNum2wxColour->begin();

	while(l_pcIt != m_pcMapNodeNum2wxColour->end())
	{
  		RestoreOneNodeColour((*l_pcIt).first, (*l_pcIt).second);
		l_pcIt++;
	}
	 m_pcMapNodeNum2wxColour->clear();
	 m_pcAllNodes->clear();
}

void
SP_FTreeNodeColourizer::RestoreOneNodeColour(SP_DS_Node* restoreNode, SP_PairColour* pairColour)
{

	CHECK_POINTER(restoreNode, return);


  	SP_DS_Node* m_pcNode = restoreNode;
	wxColour l_pcColBrush =	(*pairColour).first;
	wxColour l_pcColPen =	(*pairColour).second;

	SP_ListGraphic* sGraphic = m_pcNode->GetGraphics();
	SP_ListGraphic::iterator sIt;
	for (sIt = sGraphic->begin(); sIt != sGraphic->end(); ++sIt){
		(*sIt)->SetBrush(wxTheBrushList->FindOrCreateBrush(l_pcColBrush));
		(*sIt)->SetPen(wxThePenList->FindOrCreatePen(l_pcColPen, 1));
		(*sIt)->Update(TRUE);
	}

	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

	l_pcDoc->Refresh();

}

bool
SP_FTreeNodeColourizer::ColourGraphic( SP_DS_Node* p_pcNode, wxColour col,
    bool p_bColourCoarse)
{
	return ColourGraphic(p_pcNode, col, TRUE, p_bColourCoarse);
}


bool
SP_FTreeNodeColourizer::ColourGraphic(
    SP_DS_Node* p_pcNode,
    wxColour col,
    bool fullColouring,
    bool p_bColourCoarse
    )
{
	CHECK_POINTER(p_pcNode, return false);

	SP_ListGraphic* sGraphic = p_pcNode->GetGraphics();
	SP_ListGraphic::iterator sIt;

	for (sIt = sGraphic->begin(); sIt != sGraphic->end(); ++sIt){

		if(fullColouring == TRUE){
			(*sIt)->SetBrush(wxTheBrushList->FindOrCreateBrush(col));
		}else{
			(*sIt)->SetPen(wxThePenList->FindOrCreatePen(col, 1));
		}

		(*sIt)->Update(TRUE);
	}

	SP_MDI_Doc *doc;

	doc = SP_Core::Instance()->GetRootDocument();
	doc->ShowAll();

	if(doc){
		doc->Refresh();
		doc->Modify(TRUE);
  }
	return true;
}



void
SP_FTreeNodeColourizer::DiscardOldColours()
{
	CHECK_POINTER(m_pcMapNodeNum2wxColour, return);
	CHECK_POINTER(m_pcAllNodes, return);
	if (m_pcMapNodeNum2wxColour->size() == 0){
		return;
	}

	if (m_pcAllNodes->size() == 0){
		return;
	}
	m_pcMapNodeNum2wxColour->clear();
	m_pcAllNodes->clear();

}

SP_ListNode*
SP_FTreeNodeColourizer::GetNodeList()
{
	return m_pcAllNodes;
}


bool
SP_FTreeNodeColourizer::IsNodeRestore(SP_DS_Node* n_pcNode)
{

	if (SP_Find(*m_pcAllNodes, n_pcNode) != m_pcAllNodes->end()) {
		return FALSE;
	}
	return TRUE;
}
