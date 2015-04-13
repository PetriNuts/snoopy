//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: progress of reductions
//////////////////////////////////////////////////////////////////////


#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gr/SP_GR_Edge.h"

#include "snoopy.h"

#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionTransformer.h"


SP_DS_FTreeReductionTransformer::SP_DS_FTreeReductionTransformer(SP_DS_Graph *p_pcGraph, SP_MDI_Doc* l_pcMyDoc):
	m_pcMyDoc(l_pcMyDoc),
	m_pcGraph(p_pcGraph)
{

}

SP_DS_FTreeReductionTransformer::~SP_DS_FTreeReductionTransformer()
{

}

void
SP_DS_FTreeReductionTransformer::drawEdge(SP_DS_Node* source, SP_DS_Node* target)
{
	//connects to nodes with each other
	SP_ListGraphic* sGraphic = source->GetGraphics();
	SP_ListGraphic::iterator sIt;
	sIt = sGraphic->begin();
	double lnXs = (*sIt)->GetPrimitive()->GetX();
	double lnYs = (*sIt)->GetPrimitive()->GetY();


	SP_ListGraphic* tGraphic = target->GetGraphics();
	SP_ListGraphic::iterator tIt;
	tIt = tGraphic->begin();
	double lnXt = (*tIt)->GetPrimitive()->GetX();
	double lnYt = (*tIt)->GetPrimitive()->GetY();


	SP_ListEdgeclass::const_iterator esIt;
	const SP_ListEdgeclass* eList = m_pcMyDoc->GetGraph()->GetEdgeclasses();
	esIt= eList->begin();

	(*esIt)->OnStartElement((*tIt), lnXt, lnYt, 0, 0);
	SP_DS_Edge* l_pcEdge= (*esIt)->OnEndElement((*sIt), lnXs, lnYs, 0, 0);
	(*esIt)->AddElement(l_pcEdge);
	RefreshFrame(m_pcMyDoc);
}

void
SP_DS_FTreeReductionTransformer::RefreshFrame(SP_MDI_Doc* l_doc)
{
	//Refresh the Frames
	l_doc->Refresh();

}

void
SP_DS_FTreeReductionTransformer::DoEliminateUnneededNodes(SP_ListNode* l_pclAllNodes)
{

	//Unneeded gates are deleted.
	// Between the nodes, that were interconnected with the
	//deleted node, a new edge is drawn.
	SP_ListNode::const_iterator nodeIt;
	for(nodeIt = l_pclAllNodes->begin();
		nodeIt != l_pclAllNodes->end();
		nodeIt++){

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* edgesSrc;
		const SP_ListEdge* edgesTrg;
		SP_ListEdge::const_iterator etIt;

		edgesSrc = (*nodeIt)->GetTargetEdges();
		edgesTrg = (*nodeIt)->GetSourceEdges();


		if (edgesSrc->size() != 0 && edgesTrg->size() != 0) {
			//remember the entrance-node of this gate and the exit
			//node of the gate, before deleting the gate
			etIt = edgesTrg->begin();
			SP_DS_Node* upperNode = (SP_DS_Node*) (*etIt)->GetTarget();

			eIt = edgesSrc->begin();
			SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();

			SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

			SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();
			l_pcCanvas->Modify(TRUE);


			DeleteNode(*nodeIt);
			//connects the entrance-node with the exit edge
			drawEdge(upperNode, lowerNode);
		}
	}
}

void
SP_DS_FTreeReductionTransformer::DoCombineIdenticalGates(SP_ListNode* l_pclAllNodes)
{

	// Chains of gates of identical type, is summarized to one gate.
	// All nodes, that were interconnected with these gates, now are
	// connected with the new gate.
	SP_ListNode::const_iterator nodeIt;
	nodeIt = l_pclAllNodes->begin();
	SP_DS_Node* sourceNode = (*nodeIt);

	SP_ListNode* l_pcNodeList = new SP_ListNode();

	nodeIt++;
	while (nodeIt != l_pclAllNodes->end()){

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* edgesSrc;

		edgesSrc = (*nodeIt)->GetTargetEdges();

		if (edgesSrc->size() != 0 ) {

			//remember the nodes which are connected with the gate, before
			//deleting the gate
			for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {
				SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
				if (lowerNode != sourceNode)
					l_pcNodeList->push_back(dynamic_cast<SP_DS_Node*>(lowerNode));
			}

			DeleteNode((*nodeIt));

		}
		nodeIt++;
	}
	SP_ListNode::const_iterator l_It;
	//connects all nodes of the list with the new gate
    for (l_It = l_pcNodeList->begin(); l_It != l_pcNodeList->end(); ++l_It)
		if ((*l_It)->GetGraphics())
			drawEdge(sourceNode, (*l_It));

	//checks, if each gate is connected with an event or another gate
	const SP_ListNodeclass* allClasses = m_pcGraph->GetNodeclasses();
	SP_ListNodeclass::const_iterator nIt;

	const SP_ListNode* l_pcAllNodes;
	SP_ListNode::const_iterator l_pcIt;

	for (nIt = allClasses->begin(); nIt != allClasses->end(); ++nIt){
		if ((*nIt)->GetName()!= wxT("Basic Event")
			&& (*nIt)->GetName() !=  wxT("Undeveloped Event")
			&& (*nIt)->GetName() !=  wxT("COND-Parameter")){
			l_pcAllNodes = (*nIt)->GetElements();
			for (l_pcIt = l_pcAllNodes->begin(); l_pcIt != l_pcAllNodes->end(); ++l_pcIt){

				const SP_ListEdge* edgesSrc = (*l_pcIt)->GetTargetEdges();
				//if it is not so, draw a new edge
				if (edgesSrc->size() == 0){
					const SP_ListEdge* edgesTrg = (dynamic_cast<SP_DS_Node*>(sourceNode))->GetSourceEdges();
					if (edgesTrg->size() == 0){
						drawEdge((*l_pcIt), sourceNode);
						break;
					}
				}
			}
		}
	}

}

void
SP_DS_FTreeReductionTransformer::DoCombineToMofNGate(SP_ListNode* l_pclAllNodes)
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();

	/*before deleting a node, remember the first node in the list,
	the positions x and y of this
	node should be the positions of the new M-OF-N-Gate.
	Check also the exit edge: the target-node of this node
	should be the new target node of the M-Of-N-Gate*/
	SP_ListNode::const_iterator nodeIt;
	nodeIt = l_pclAllNodes->begin();

	SP_ListGraphic* sGraphic = (*nodeIt)->GetGraphics();
	SP_ListGraphic::iterator sIt;
	sIt = sGraphic->begin();
	double lnXs = (*sIt)->GetPrimitive()->GetX();
	double lnYs = (*sIt)->GetPrimitive()->GetY();

	SP_DS_Node* upperNode = FindTargetNode((*nodeIt));
	DeleteNode(*nodeIt);

	//create M-Of-N-gate and draw new edge
	SP_DS_Nodeclass* nClass = m_pcMyDoc->GetGraph()->GetNodeclass(wxT("M-OF-N"));
	SP_DS_Node* sourceNode = nClass->NewElement(l_pcView->GetCanvas()->GetNetnumber());
	sourceNode->ShowOnCanvas(l_pcCanvas, FALSE, lnXs, lnYs, 0);
	drawEdge(upperNode, sourceNode);

	long m_id = 0;
	long n_id = 0;
	wxString idOfM;
	wxString idOfN;


	SP_ListNode* l_pcNodeList = new SP_ListNode();

	nodeIt++;
	while (nodeIt != l_pclAllNodes->end()){
		n_id++;

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* edgesSrc;

		edgesSrc = (*nodeIt)->GetTargetEdges();
		m_id = edgesSrc->size();

		if (edgesSrc->size() != 0 ) {

			for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {

				//remember the basic/undevelopement nodes, before deleting the gate
				SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
				if (lowerNode->GetClassName() ==  wxT("Basic Event")
					|| lowerNode->GetClassName() ==  wxT("Undeveloped Event"))
						l_pcNodeList->push_back(dynamic_cast<SP_DS_Node*>(lowerNode));

			}
			DeleteNode((*nodeIt));
		}
		nodeIt++;
	}
	// check whether nodes occur doubly. These nodes will be deleted.
	SP_ListNode* tList = EliminateDuplicateNodes(l_pcNodeList);
	l_pcNodeList->clear();
	l_pcNodeList = tList;

	//set M and N before drawing the rest of the edges
	idOfM.Printf(wxT("%ld"),m_id);
	idOfN.Printf(wxT("%ld"),n_id);
	sourceNode->GetAttribute(wxT("N"))->SetValueString(idOfN);
	sourceNode->GetAttribute(wxT("M"))->SetValueString(idOfM);


	SP_ListNode::const_iterator l_It;
    for (l_It = l_pcNodeList->begin(); l_It != l_pcNodeList->end(); ++l_It)
		drawEdge(sourceNode, (*l_It));

	// Although is tested, whether double nodes (basic events) exist,
	// it can happen that there are nodes without edges.
	// These nodes must also be deleted.
	FindNodesWithoutEdges(l_pcNodeList);
}

void
SP_DS_FTreeReductionTransformer::DoRelocateMultipleInputs(SP_ListNode* l_pclAllNodes)
{
	//The reduction takes place for both distributive-laws
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();

	wxString sourceType = wxT("");
	wxString secondType = wxT("");

	SP_ListNode::const_iterator nodeIt;
	nodeIt = l_pclAllNodes->begin();
	SP_DS_Node* logNode = (*nodeIt);
	SP_ListGraphic* sGraphic = (*nodeIt)->GetGraphics();
	SP_ListGraphic::iterator sIt;
	sIt = sGraphic->begin();
	double lnX = (*sIt)->GetPrimitive()->GetX();
	double lnY = (*sIt)->GetPrimitive()->GetY();
	SP_DS_Node* orNode = NULL;
	SP_ListNode* l_pcNodeList = new SP_ListNode();

	nodeIt++;
	while (nodeIt != l_pclAllNodes->end()){

		if (sourceType.empty() && secondType.empty()){
			if ((*nodeIt)->GetClassName() ==  wxT("AND")){
				sourceType = wxT("AND");
				secondType = wxT("OR");
			}else{
				sourceType = wxT("OR");
				secondType = wxT("AND");
			}
		}

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* edgesSrc;

		edgesSrc = (*nodeIt)->GetTargetEdges();

		for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {

			//remember the basic/undevelopement nodes, before deleting the gate
			SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
			l_pcNodeList->push_back(dynamic_cast<SP_DS_Node*>(lowerNode));

		}

		if((*nodeIt)->GetClassName() ==  sourceType){

			//remember position of old gate
			SP_ListGraphic* sGraphic = (*nodeIt)->GetGraphics();
			SP_ListGraphic::iterator sIt;
			sIt = sGraphic->begin();
			double lnXs = (*sIt)->GetPrimitive()->GetX();
			double lnYs = (*sIt)->GetPrimitive()->GetY();

			//create new gate
			SP_DS_Nodeclass* nClass = m_pcMyDoc->GetGraph()->GetNodeclass(secondType);
			orNode = nClass->NewElement(l_pcView->GetCanvas()->GetNetnumber());
			orNode->ShowOnCanvas(l_pcCanvas, FALSE, lnXs, lnYs - 30, 0);

			SP_DS_Node* upperNode = NULL;
			//if gate has more incoming edges
			if (edgesSrc->size() > (l_pclAllNodes->size() - 2)) {
				upperNode = (*nodeIt);
			}else{
				//remember the target-node for drawing the edge between  new gate an old targetNode
				upperNode = FindTargetNode((*nodeIt));
				DeleteNode((*nodeIt));
				SP_ListNode::const_iterator It;
				for (It = l_pcNodeList->begin(); It != l_pcNodeList->end(); ++It) {
					if ((*It) != upperNode)
						drawEdge(orNode, (*It));
				}
			}
			drawEdge(upperNode, orNode);
			l_pcNodeList->clear();

		}else
			DeleteNode((*nodeIt));
		nodeIt++;
	}

	SP_DS_Nodeclass* nClass = m_pcMyDoc->GetGraph()->GetNodeclass(sourceType);
	SP_DS_Node* andNode = nClass->NewElement(l_pcView->GetCanvas()->GetNetnumber());
	andNode->ShowOnCanvas(l_pcCanvas, FALSE, (lnX + 10), (lnY - 20), 0);
	drawEdge(orNode, andNode);

	//draws all edges
	SP_ListNode::const_iterator nIt;
	for (nIt = l_pcNodeList->begin(); nIt != l_pcNodeList->end(); ++nIt) {
		if ((*nIt) == logNode) {
			drawEdge(orNode, (*nIt));
		}else{
			drawEdge(andNode, (*nIt));
		}

	}
	// Although is tested, whether double nodes (basic events) exist,
	// it can happen that there are nodes without edges.
	// These nodes must also be deleted.
	FindNodesWithoutEdges(l_pcNodeList);
}

void
SP_DS_FTreeReductionTransformer::DeleteNode(SP_DS_Node* del_Node)
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
	//determines the graphic type of the node to delete
	SP_ListGraphic* sGraphic = del_Node->GetGraphics();
	SP_ListGraphic::iterator s_itGraphic;
	s_itGraphic = sGraphic->begin();
	(*s_itGraphic)->Select(true);
	l_pcView->DoDelete(sGraphic);
}

void
SP_DS_FTreeReductionTransformer::DoDeleteGraphic()
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

	wxList l_lShapes;
    SP_ListGraphic l_lGraphic;
    l_pcView->FindSelectedShapes(l_lShapes);

    l_lGraphic.clear();
    // filter out attributes
    wxNode* l_pcNode = l_lShapes.GetFirst();
    while (l_pcNode){
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(dynamic_cast<wxShape*>(l_pcNode->GetData()));
        if (l_pcGraphic &&
            l_pcGraphic->GetGraphicType() != SP_GRAPHIC_ATTRIBUTE){
            l_lGraphic.push_back(l_pcGraphic);
            dynamic_cast<wxShape*>(l_pcNode->GetData())->Select(FALSE);
        }
        l_pcNode = l_pcNode->GetNext();
    }

    l_pcView->DoDelete(&l_lGraphic);
}


SP_ListNode*
SP_DS_FTreeReductionTransformer::FindAllSourceNodes(SP_DS_Node* targetNode)
{
	//Determines all entrance-nodes of a gate which are connected with an incoming
	//edge with this gate
	const SP_ListEdge* edges = (dynamic_cast<SP_DS_Node*>(targetNode))->GetTargetEdges();
	SP_ListEdge::const_iterator eIt;
	SP_ListNode* nodeList = new SP_ListNode();

	if (edges->size() != 0 ) {
		for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
			SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
			nodeList->push_back(dynamic_cast<SP_DS_Node*>(lowerNode));
		}
	}
	return nodeList;
}

SP_DS_Node*
SP_DS_FTreeReductionTransformer::FindTargetNode(SP_DS_Node* sourceNode)
{
	//Determines the node which is connected with this node about an out
	//going edge.
	const SP_ListEdge* edges = (dynamic_cast<SP_DS_Node*>(sourceNode))->GetSourceEdges();
	SP_ListEdge::const_iterator eIt;

	eIt = edges->begin();
	SP_DS_Node* upperNode = (SP_DS_Node*) (*eIt)->GetTarget();

	return upperNode;
}

SP_ListNode*
SP_DS_FTreeReductionTransformer::EliminateDuplicateNodes(SP_ListNode* l_pcAllNodes)
{
	// List wxT("l_pcAllNodes") contains all nodes, that were interconnected
	// with the deleted gates. Before these nodes are connected
	// with a new node, double nodes from the list are deleted.
	SP_ListNode::const_iterator nIt;
	SP_ListNode* temp_List = new SP_ListNode();
	nIt = l_pcAllNodes->begin();
	while (nIt != l_pcAllNodes->end()){
		if (!(SP_Find(*temp_List, *nIt) != temp_List->end()))
			temp_List->push_back(*nIt);

		nIt++;
	}

	l_pcAllNodes->clear();
	l_pcAllNodes = temp_List;

	return l_pcAllNodes;
}

void
SP_DS_FTreeReductionTransformer::FindNodesWithoutEdges(SP_ListNode* l_pcAllNodes)
{
	// After a reduction, nodes can remain behind occurrence doubly.
	// Since these nodes haven't any more edges, so that these nodes
	// simply can be found and deleted.
	SP_ListNode::const_iterator nIt;
	nIt = l_pcAllNodes->begin();
	bool wasFound = FALSE;
	while (nIt != l_pcAllNodes->end()){
		SP_ListGraphic* l_graphic = (*nIt)->GetGraphics();
		SP_ListGraphic::iterator gIt;
		const SP_ListEdge* edges = (*nIt)->GetSourceEdges();
		SP_ListEdge::const_iterator eIt;
		for (gIt = l_graphic->begin(); gIt != l_graphic->end(); ++gIt){
			wasFound = FALSE;
			double lnX = (*gIt)->GetPosX();
			double lnY = (*gIt)->GetPosY();
			SP_ListGraphic::iterator l_GrIter;
			SP_ListGraphic* l_plGrEdge;
			for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
				l_plGrEdge = (*eIt)->GetGraphics();
				for (l_GrIter = l_plGrEdge->begin(); l_GrIter != l_plGrEdge->end(); ++l_GrIter){
					SP_Graphic* l_pcGr = *l_GrIter;
					SP_GR_Edge* l_pcEdge = dynamic_cast<SP_GR_Edge*>(l_pcGr);
					double lX = l_pcEdge->GetSource()->GetPosX();
					double lY = l_pcEdge->GetSource()->GetPosY();

					if (lnX == lX && lnY == lY){
						wasFound = TRUE;
						break;
					}
				}
			}
			if (!wasFound){
				(*gIt)->Select(TRUE);
				DoDeleteGraphic();
				break;
			}
		}
		nIt++;
	}
}

