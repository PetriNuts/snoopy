//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.2 $
// $Date: 2006/08/30 10:10:00 $
// Short Description: find possible reductions of the fault tree
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionFinder.h"
#include "sp_core/SP_Core.h"

NodeList::NodeList(long typ, const wxString& descr, SP_ListNode* r_list)
:  m_typ(typ),
   description(descr),
   r_nodeList(r_list)
{}

SP_DS_FTreeReductionFinder::SP_DS_FTreeReductionFinder(SP_DS_Graph* p_pcGraph)
{
	//actual fault tree
	m_pcGraph = p_pcGraph;

	//counter for number of reduction results in the main list
	r_id = 0;
	//clear list of all reduction results
	r_map.clear();

}

SP_DS_FTreeReductionFinder::~SP_DS_FTreeReductionFinder()
{
	r_map.clear();
}

// reduction rule looks for gates, which have only one
// input and one going out edge and which is interconnected with another gate.
void
SP_DS_FTreeReductionFinder::FindUnneededGates()
{

	//short-description and number for identification of the rule
	wxString typDescription = wxT("unneeded Gates");
	long typ = 1;

	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_pcGraph->GetNodeclasses();

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;
	const SP_ListEdge* edgesTrg;
	SP_ListEdge::const_iterator lIt;

	//list of gates of one reduction rule
	SP_ListNode* red_Nodes = new SP_ListNode();

	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)  {
		nc = (*ncIt);
		nList = nc->GetElements();

		for (nIt = nList->begin(); nIt != nList->end(); nIt++) {
			//all gates are checked. With this reduction, Neg-gates are not checked,
			//because they are not unneeded gates. Deleting of these nodes change the
			//logical expression of the fault tree.
			if ((*nIt)->GetClassName()== wxT("OR")
			|| (*nIt)->GetClassName()== wxT("AND")
			|| (*nIt)->GetClassName()== wxT("XOR")
			|| (*nIt)->GetClassName()== wxT("M-OF-N")
			|| (*nIt)->GetClassName()== wxT("COND")){

				edgesSrc = (*nIt)->GetTargetEdges();
				edgesTrg = (*nIt)->GetSourceEdges();
				//if the gate has one input and one out going egde...
				if (edgesSrc->size() == 1 && edgesTrg->size() == 1) {
					lIt = edgesSrc->begin();
					SP_DS_Node* lowerNode = (SP_DS_Node*) (*lIt)->GetSource();
					//check the node, which has an input edge to the gate. If there is not an event,
					//the gate can be deleted now. (it is not allowed to delete gates which have an entrance-event
					// or an exit-event.
					if (lowerNode->GetClassName() !=  wxT("Intermediate Event")
						&& lowerNode->GetClassName() !=  wxT("Basic Event")
						&& lowerNode->GetClassName() !=  wxT("Undeveloped Event"))
							red_Nodes->push_back(*nIt);
				}
			}
		}
	}
	//if unneeded gates were found, the list of gates are saved in a list of all reduction results
	if (!red_Nodes->empty()) {
		r_id++;
		//beside of saving the list of unneeded gates in the main list, an identification-number
		//a short description of the reduction rule are saved in addition.
		NodeList* n_list = new NodeList(typ, typDescription, red_Nodes);
		r_map[r_id] = n_list;
	}
}

//reduction rule searchs for chains of identical gates, that are combined to one gate of the same type
void
SP_DS_FTreeReductionFinder::FindChainsOfIdenticalGates()
{
	//short-description and number for identification of the rule
	wxString typDescription = wxT("chains of identical gates");
	long typ = 2;

	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_pcGraph->GetNodeclasses();

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;


	list<SP_ListNode*>* chainsOfGates = new list<SP_ListNode*>();
	SP_ListNode* r_Nodes = new SP_ListNode();


	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)  {
		nc = (*ncIt);
		nList = nc->GetElements();

		for (nIt = nList->begin(); nIt != nList->end(); nIt++) {

			r_Nodes = new SP_ListNode();

			if ((*nIt)->GetClassName()== wxT("OR")
			|| (*nIt)->GetClassName()== wxT("AND")
			|| (*nIt)->GetClassName()== wxT("XOR")
			|| (*nIt)->GetClassName()== wxT("COND")){

				edgesSrc = (*nIt)->GetTargetEdges();
				SP_DS_Node* nextNode;

				if (edgesSrc->size() > 1){
					for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); eIt++) {
						nextNode = (SP_DS_Node*) (*eIt)->GetSource();
						//It is checked whether the gate is connected with a gate
						//of the same type
						if ((*nIt)->GetClassName()== nextNode->GetClassName()) {

							//If gates are found which form a chain of gates of the same type,
							//it has to be checked whether it already exists a list in which
							//gates from the same type are listed and these gates are connected with
							//the gates of the list about an edge.
							//If it is so, then only the entrance gate is taken in the list.
							//If not, then both gates form a new list.
							if (chainsOfGates->empty()){
								SP_ListNode* nodeList = new SP_ListNode();
								nodeList->push_back(*nIt);
								nodeList->push_back(nextNode);
								chainsOfGates->push_back(nodeList);

							}else{
								const SP_ListEdge* edgesOfNode;
								edgesOfNode = (*nIt)->GetSourceEdges();
								SP_ListEdge::const_iterator etIt = edgesOfNode->begin();

								SP_DS_Node* targetNode = (SP_DS_Node*) (*etIt)->GetTarget();

								list<SP_ListNode*>::iterator mIt;
								for (mIt = chainsOfGates->begin(); mIt != chainsOfGates->end(); ++mIt){
									if (!(SP_Find(*(*mIt), *nIt) != (*mIt)->end())){
										if (!(SP_Find(*(*mIt), nextNode) != (*mIt)->end())){
											SP_ListNode* nodeList = new SP_ListNode();
											nodeList->push_back(*nIt);
											nodeList->push_back(nextNode);
											chainsOfGates->push_back(nodeList);

										}else
											(*mIt)->push_back(*nIt);
									}
									else{
										edgesOfNode = (nextNode)->GetSourceEdges();
										etIt = edgesOfNode->begin();
										targetNode = (SP_DS_Node*)(*etIt)->GetTarget();
										if (!(SP_Find(*(*mIt), nextNode) != (*mIt)->end()))
											(*mIt)->push_back(nextNode);
									}

								}

							}
						}
					}
				}
			}
		}


	if (!chainsOfGates->empty()) {
		//beside of saving each list of identical gates in the main list, an identification-number
		//a short description of the reduction rule are saved in addition.
		list<SP_ListNode*>::iterator sIt;
		for (sIt = chainsOfGates->begin(); sIt != chainsOfGates->end(); sIt++) {

			r_id++;
			NodeList* n_list = new NodeList(typ, typDescription, (*sIt));
			r_map[r_id] = n_list;
		}
		chainsOfGates->clear();
	}
	}
}

// reduction rule searchs for neg-gates, that are interconnected with an edge
// (law of the double negation)
void
SP_DS_FTreeReductionFinder::FindUnneededNegations()
{
	//short-description and number for identification of the rule
	wxString typDescription = wxT("unneeded Negations");
	long typ = 3;

	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_pcGraph->GetNodeclasses();

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;

	SP_ListNode* red_Nodes = new SP_ListNode();

	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)  {
		nc = (*ncIt);
		nList = nc->GetElements();

		for (nIt = nList->begin(); nIt != nList->end(); nIt++) {

			// if the gate is a neg-gate, check the node, that has an input edge
			//to this gate.
			if ((*nIt)->GetClassName()== wxT("NEG")){

				edgesSrc = (*nIt)->GetTargetEdges();

				eIt = edgesSrc->begin();
				SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
				//if this node is a neg-gate, check whether this node is already saved in the
				//reduction list. --> because only two neg-gates, that are interconnected with an edge,
				//can be deleted
				if (lowerNode->GetClassName()== wxT("NEG")) {
					bool stillExists = false;
					SP_ListNode::const_iterator mIt;


					for (mIt = red_Nodes->begin(); mIt != red_Nodes->end(); ++ mIt){
						//node
						if ((*mIt) == (*nIt) || (*mIt) == lowerNode){
							stillExists = true;
							break;
						}
					}
					//if this node is not exist in the list, save this node
					if (!stillExists){
						red_Nodes->push_back(*nIt);
						red_Nodes->push_back(lowerNode);
					}
				}

			}
		}
	}
	//if unneeded negations were found, the list of gates are saved in a list of all reduction results
	if (!red_Nodes->empty()) {
		r_id++;
		//beside of saving the list of unneeded negations in the main list, an identification-number
		//a short description of the reduction rule are saved in addition.
		NodeList* n_list = new NodeList(typ, typDescription, red_Nodes);
		r_map[r_id] = n_list;
	}
}

//reduction rule searchs for combination of gates and events, that can be represented with a
//M-OF-N-gate (Hint: only for a part-tree, whose last level consists of only basic events or undeveloped events)
void
SP_DS_FTreeReductionFinder::FindPossibleMOFNInputs()
{
	//short-description and number for identification of the rule
	wxString typDescription = wxT("possible gates for M-OF-N-Gate");
	long typ = 4;

	SP_DS_Nodeclass* orClass = m_pcGraph->GetNodeclass(wxT("OR"));
	const SP_ListNode* nodeList = orClass->GetElements();
	SP_ListNode::const_iterator oIt;
	SP_ListNode* r_Nodes = new SP_ListNode();

	for (oIt = nodeList->begin(); oIt != nodeList->end(); ++oIt) {
		const SP_ListEdge* edgesSrc = (*oIt)->GetTargetEdges();
		if (edgesSrc->size() > 1) {
			r_Nodes->push_back(*oIt);

			SP_ListEdge::const_iterator eIt;
			list<SP_ListNode*> sList;
			for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++ eIt){
				SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
				SP_ListNode* list = new SP_ListNode();

				if (lowerNode->GetClassName()== wxT("AND")) {

					r_Nodes->push_back(lowerNode);
					//determines all entrance-events which are connected with the AND
					//about incoming edges.
					list = FindAllSourceNodes(lowerNode);

					if (list->empty()) {
						r_Nodes->clear();
						break;
					}else
						sList.push_back(list);
				}else{
					r_Nodes->clear();
					break;
				}
			}
			//The combination of gates and events can be replaced only with a
			//M-OF-N-gate, if the found combinations of events correspond to
			//the functional character of the M-OF-N-gate. The test takes place
			//by the call of the method CheckList().
			if (CheckList(sList) && !r_Nodes->empty()){
				r_id++;
				NodeList* n_list = new NodeList(typ, typDescription, r_Nodes);
				r_map[r_id] = n_list;
				r_Nodes = new SP_ListNode();
			}
		}
	}
}

//reduction rule looks for combination of gates and events, where can apply
//the distributive law
void
SP_DS_FTreeReductionFinder::FindMultipleInputs()
{
	//first rule: check on the basis of (a AND b) OR (a AND c) = a AND (b OR c)
	CheckMultipleInputs(wxT("AND"), wxT("OR"));
	//second rule: check on the basis of (a OR b) AND (a OR c) = a OR (b AND c)
	CheckMultipleInputs(wxT("OR"), wxT("AND"));
}

void
SP_DS_FTreeReductionFinder::CheckMultipleInputs(const wxString& sourceType, const wxString& secondType)
{
	//short-description and number for identification of the rule
	wxString typDescription = wxT("multiple inputs");
	long typ = 5;

	const SP_ListNodeclass* classList = m_pcGraph->GetNodeclasses();
	SP_ListNodeclass::const_iterator nIt;
	const SP_ListNode* nodeList;
	SP_ListNode::const_iterator mIt;

	for (nIt = classList->begin(); nIt != classList->end(); ++nIt) {
		//looks for basic events or undeveloped event, that are dependent
		//basic events
		if ((*nIt)->GetName()== wxT("Basic Event")
			|| (*nIt)->GetName()== wxT("Undeveloped Event")){

			nodeList = (*nIt)->GetElements();
			//looks for combination of events and gates that correspond to the
			//rules of the distributive law
			for (mIt = nodeList->begin(); mIt != nodeList->end(); ++mIt){

				if ((*mIt)->GetLogical()){

					SP_ListNode* nodes = new SP_ListNode();

					const SP_ListEdge* edgeList = (*mIt)->GetSourceEdges();
					SP_ListEdge::const_iterator eIt;
					for (eIt = edgeList->begin(); eIt != edgeList->end(); ++eIt){
						SP_DS_Node* node = (SP_DS_Node*) (*eIt)->GetTarget();
						if (node->GetClassName()== secondType){
							if (!(SP_Find(*nodes, node) != nodes->end())){
								SP_DS_Node* nodeToReduce = node;
								nodes->push_back(nodeToReduce);
							}
						}
					}

					SP_ListNode::const_iterator sIt;
					SP_ListEdge::const_iterator esIt;

					SP_DS_Node* sourceNode = NULL;
					SP_ListNode* temp_List = new SP_ListNode();
					for (sIt = nodes->begin(); sIt != nodes->end(); ++sIt){
						const SP_ListEdge* edgesTrg = (*sIt)->GetSourceEdges();
						esIt = edgesTrg->begin();
						SP_DS_Node* n_Node = dynamic_cast<SP_DS_Node*>((*esIt)->GetTarget());
						if (n_Node->GetClassName()== sourceType) {
							sourceNode = n_Node;
							temp_List->push_back(*mIt);
							temp_List->push_back(n_Node);
							temp_List->push_back(*sIt);
							sIt++;
							break;
						}else{
							temp_List->clear();
						}
					}

					while (sIt != nodes->end()){
						const SP_ListEdge* edgesTrg = (*sIt)->GetSourceEdges();
						esIt = edgesTrg->begin();
						SP_DS_Node* n_Node = (SP_DS_Node*) (*esIt)->GetTarget();
						if (n_Node == sourceNode) {
							SP_DS_Node* newNode = (*sIt);
							temp_List->push_back(newNode);
						}
						sIt++;
					}
					nodes->clear();
					nodes = temp_List;

					if (!nodes->empty() && sourceNode && nodes->size() > 3){
						r_id++;
						NodeList* n_list = new NodeList(typ, typDescription, nodes);
						r_map[r_id] = n_list;
						nodes = new SP_ListNode();
					}
				}
			}
		}
	}
}

bool
SP_DS_FTreeReductionFinder::CheckList(list<SP_ListNode*> sList)
{

	list<SP_ListNode*>::iterator spIt;
	list<SP_ListNode*>::iterator sqIt;

	//sList consists of all lists of entrance-events of each gate.
	//These lists are compared together.
	for (spIt = sList.begin(); spIt != sList.end(); ++spIt){

		for (sqIt = sList.begin(); sqIt != sList.end(); ++sqIt){
			//number of entrance-events of each list have to be always the same
			if ((*spIt)->size() != (*sqIt)->size())
				return FALSE;

			//Comparison of two lists: in each list, one element have to be different
			if ((*spIt) != (*sqIt))
				if (!IsEmptyAfterMerge((*spIt), (*sqIt)))
					return FALSE;
		}
		//all entrance-events have to be a dependent event
		SP_ListNode::const_iterator nIt;
		for (nIt = (*spIt)->begin(); nIt != (*spIt)->end(); ++ nIt){
			if (!(*nIt)->GetLogical())
				return FALSE;
		}
	}

	return TRUE;
}

bool
SP_DS_FTreeReductionFinder::IsEmptyAfterMerge(SP_ListNode* firstList, SP_ListNode* secondList)
{
	SP_ListNode::const_iterator sIt;
	SP_ListNode::const_iterator tIt;
	unsigned long counter = 0;
	//With the M-OF-N-gate, the separate combinations of events have to differ
	//together only around an element
	for (sIt = firstList->begin(); sIt != firstList->end(); ++sIt)
		for (tIt = secondList->begin(); tIt != secondList->end(); ++tIt){

			wxString sId = (*sIt)->GetAttribute(wxT("ID"))->GetValueString();
			wxString tId = (*tIt)->GetAttribute(wxT("ID"))->GetValueString();

			if (sId== tId)
				counter++;
		}

	if (firstList->size() != (counter + 1))
		return FALSE;

	return TRUE;
}

SP_ListNode*
SP_DS_FTreeReductionFinder::FindAllSourceNodes(SP_DS_Node* targetNode)
{
	const SP_ListEdge* edges = (targetNode)->GetTargetEdges();
	SP_ListEdge::const_iterator eIt;
	SP_ListNode* nodeList = new SP_ListNode();

	//All entrance-events of the gate are determined.
	//(Hint by M-OF-N-gate: only for a part-tree, whose last level consists
	//of only basic events or undeveloped events)
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
		SP_DS_Node* lowerNode = (SP_DS_Node*) (*eIt)->GetSource();
		if (lowerNode->GetClassName()== wxT("Basic Event")
			|| lowerNode->GetClassName()== wxT("Undeveloped Event")){
			nodeList->push_back(lowerNode);
		}else{
			nodeList->clear();
		}
	}
	return nodeList;
}

ReductionMap
SP_DS_FTreeReductionFinder::GetMap()
{
		return r_map;
}
