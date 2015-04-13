//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.3 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: find logical expression of the fault tree
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "snoopy.h"


lNode::lNode(bool check, const wxString& name, SP_DS_Node* p_node)
  : isChecked(check),
	m_name(name),
	m_node(p_node)
{}

SP_DS_FTreeLogicalExpressionReader::SP_DS_FTreeLogicalExpressionReader()

{
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	 wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	 SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);
	m_pcGraph = l_pcDoc->GetGraph();
}

SP_DS_FTreeLogicalExpressionReader::~SP_DS_FTreeLogicalExpressionReader()
{
    m_map.clear();
	vMap.clear();
}


NodeMap
SP_DS_FTreeLogicalExpressionReader::FindLogicalExpression(SP_DS_Node* p_pcNode)
{

	//Reading of the logical expression of a fault tree (on the basis of breath search)
	if (FindTopEvent(p_pcNode))  {

	  SP_ListNodeclass::const_iterator ncIt;
	  SP_ListNode::const_iterator nIt;
	  SP_ListEdge::const_iterator eIt;
	  NodeMap::iterator pIt;

	  unsigned long id = 0;
	  //number of incoming edges (used for the right combination of the entrance-nodes of a gate)
	  unsigned long counter = 0;
	  //counter to check, if a node is evaluated completely.
	  unsigned long counterCheck = 0;
	  lNode* p;
	  NodeMap m_tempMap;

	  do {
		  counterCheck = 0;
		  wxString expression;
	  for (pIt = m_map.begin(); pIt != m_map.end(); pIt++) {

		p = (*pIt).second;
		SP_DS_Node* node = p->m_node;

			//If the node is not evaluated completely ...
			if (p->isChecked == false ) {
				counterCheck++;

				//determine the number of incoming edges.
				const SP_ListEdge* edgesSrc = (dynamic_cast<SP_DS_Node*>(node))->GetTargetEdges();
				counter = 0;

				//If the node is a gate form type XOR-, COND- or M-OF-N-gate, then replaces this node
				//on the basis of AND-, OR and NEG-gates.
				if (node->GetClassName()== wxT("XOR") ||
					node->GetClassName()== wxT("M-OF-N")	||
					node->GetClassName()== wxT("COND"))
				{
					NodeMap termMap;
					if (node->GetClassName()== wxT("XOR"))
						termMap = DoReplaceXORGates(node);
					if (node->GetClassName()== wxT("M-OF-N"))
						termMap = DoReplaceMOfNGates(node);
					if (node->GetClassName()== wxT("COND"))
						termMap = DoReplaceCONDGates(node);
					NodeMap::iterator mIt;
					id++;
					lNode* negBegin = new lNode( true, wxT("("), node);
					m_tempMap[id] = negBegin;
					for( mIt = termMap.begin(); mIt != termMap.end(); ++mIt){
						id++;
							m_tempMap[id] = (*mIt).second;
							id++;
					}

					id++;
					lNode* negEnd = new lNode( true, wxT(")"), node);
					m_tempMap[id] = negEnd;
				}else{
					//sets a bracket pair, if the node has more than one incoming edge
					if (edgesSrc->size() > 1) {
						id++;
						lNode* negBegin = new lNode( true, wxT("("), node);
						m_tempMap[id] = negBegin;
					}

					//check each entrance-node of the node...
					for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {
						counter++;
						id++;
						SP_DS_Node* child = (SP_DS_Node*) (*eIt)->GetSource();

						//If the node is a basic event or an undeveloped event, it has to be checked,
						//if this event is connected with a NEG-gate.
						if (child->GetClassName()== wxT("Basic Event") ||
							child->GetClassName()== wxT("Undeveloped Event")){
							wxString sId = child->GetAttribute(wxT("ID"))->GetValueString();
							lNode* n_node;
							//If the event is connected with a NEG-Gate, first this event is marked as evaluated
							//completely. In addition the event is marked as a negated event.
							if (node->GetClassName()== wxT("NEG") ) {

								lNode* negNode = new lNode(true, node->GetClassName(), node);
								m_tempMap[id] = negNode;
								id++;
							}

							n_node = new lNode(true, wxT("X" + sId), child);
							m_tempMap[id] = n_node;
						}else{
						//If the entrance-node is a gate, it is also checked whether the gate is connected
						//with a NEG-gate. So this gate is clipped.
						if (node->GetClassName()== wxT("NEG") ) {

							lNode* neg_node = new lNode(true, node->GetClassName(), node);
							m_tempMap[id] = neg_node;
							id++;

							lNode* negBegin = new lNode( true, wxT("("), node);
							m_tempMap[id] = negBegin;
							id++;

							lNode* n_node = new lNode(false, child->GetClassName(), child);
							m_tempMap[id] = n_node;
							id++;

							lNode* negEnd = new lNode( true, wxT(")"), node);
							m_tempMap[id] = negEnd;
							id++;

						}else{
							lNode* n_node = new lNode(false, child->GetClassName(), child);
							m_tempMap[id] = n_node;
						}
					}

					if (edgesSrc->size() > 1) {
						if (counter < edgesSrc->size()) {
							id++;
							lNode* root = new lNode(true, node->GetClassName(), node);
							m_tempMap[id] = root;
						}
					}
				}
				if (edgesSrc->size() > 1) {
					id++;
					lNode* negEnd = new lNode( true, wxT(")"), node);
					m_tempMap[id] = negEnd;
				}
				}
		} else {
			//a node is still evaluated completely. This node can be inserted immediately in the list.
			id++;
			m_tempMap[id] = p;

		}
	  }
	  m_map = m_tempMap;
	  id = 0;
	  m_tempMap.clear();
		//Is counterCheck = 0, all nodes of the fault tree are evaluated completely.
	  }while (counterCheck != 0);
	 }

	return m_map;
}

NodeMap
SP_DS_FTreeLogicalExpressionReader::DoReplaceCONDGates(SP_DS_Node* n_pcNode)
{
	//The entrance-nodes of a can be replaced by ANDs.
	const SP_ListEdge* edges = (dynamic_cast<SP_DS_Node*>(n_pcNode))->GetTargetEdges();
	SP_ListEdge::const_iterator eIt;
	SP_ListNode* nodeList = new SP_ListNode();
	SP_ListNode::const_iterator nIt;

	//First, the entrance-nodes of the COND-gate are determinded.
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
		SP_DS_Node* node = (SP_DS_Node*) (*eIt)->GetSource();
		if (node->GetClassName() !=  wxT("COND-Parameter"))
			nodeList->push_back(dynamic_cast<SP_DS_Node*>(node));
	}
	//all entrance-nodes are connected with an AND
	NodeMap termMap = DoCombineEventsWithGate(nodeList);
	return termMap;
}
NodeMap
SP_DS_FTreeLogicalExpressionReader::DoCombineEventsWithGate(SP_ListNode* n_pcAllNodes)
{
	SP_ListNode::const_iterator nIt;
	NodeMap termMap;
	long index = 0;

	lNode* openBracket = new lNode(true, wxT("("), NULL);
	termMap[index] = openBracket;
	index++;

	//all nodes of the COND-gate are combined with an AND-gate
	for (nIt = n_pcAllNodes->begin(); nIt != n_pcAllNodes->end(); ++nIt){
		if (nIt != n_pcAllNodes->begin()){
			lNode* andNode = new lNode(true, wxT("AND"), (*nIt));
			termMap[index] = andNode;
			index++;
		}

		//If an entrance-node is a gate, this gate has to be clipped.
		if ((*nIt)->GetClassName()== wxT("Basic Event")
			|| (*nIt)->GetClassName()== wxT("Undeveloped Event")){
			wxString sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
			lNode* node = new lNode(true, wxT("X1" + sId), (*nIt));
			termMap[index] = node;
			index++;
		}else{
			lNode* openBracket = new lNode(true, wxT("("), (*nIt));
			termMap[index] = openBracket;
			lNode* node = new lNode(false, (*nIt)->GetClassName(), (*nIt));
			termMap[++index] = node;
			lNode* closeBracket = new lNode(true, wxT(")"), (*nIt));
			termMap[(index + 2)] = closeBracket;
			index += 3;
		}

	}

	lNode* closeBracket = new lNode(true, wxT(")"), NULL);
	termMap[index] = closeBracket;

	return termMap;
}

NodeMap
SP_DS_FTreeLogicalExpressionReader::DoReplaceMOfNGates(SP_DS_Node* n_pcNode)
{
	//looks for combination of events and gates which present the functional character
	//of the M-OF-N-gate
	const SP_ListEdge* edges = (dynamic_cast<SP_DS_Node*>(n_pcNode))->GetTargetEdges();
	SP_ListEdge::const_iterator eIt;
	SP_ListNode* nodeList = new SP_ListNode();
	SP_ListNode::const_iterator nIt;

	//First, all entrance-nodes are determined which are connected with the M-OF-N-gate.
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
		SP_DS_Node* node = (SP_DS_Node*) (*eIt)->GetSource();
		nodeList->push_back(dynamic_cast<SP_DS_Node*>(node));
	}

	unsigned long mId;
	//reading of the value of M
	wxString mVal = n_pcNode->GetAttribute(wxT("M"))->GetValueString();
	mVal.ToULong(&mId);

	list<SP_ListNode*>* n_pcAllNodes = new list<SP_ListNode*>();

	list<SP_ListNode*>* selectedNodes = new list<SP_ListNode*>();
	list<SP_ListNode*>::iterator sIt;

	list<SP_ListNode*>* tempNodes = new list<SP_ListNode*>();

	//It looks for all combination of events which present the functional character
	//of the M-OF-N-gate. One combination consists of M elements.
	if (mId < nodeList->size()){

		for	(nIt = nodeList->begin(); nIt != nodeList->end(); ++nIt){
			SP_ListNode* nList = new SP_ListNode();
			nList->push_back(*nIt);
			selectedNodes->push_back(nList);
			unsigned long index = 1;

			while (index < mId){
				for (sIt = selectedNodes->begin(); sIt != selectedNodes->end(); ++sIt){
					SP_ListNode* nList = (*sIt);
					SP_ListNode::const_iterator mIt;
					for (mIt = nIt; mIt != nodeList->end(); ++mIt){
						if (!(SP_Find(*nList, *mIt) != nList->end())){
							SP_ListNode* rList = new SP_ListNode();
							SP_ListNode::const_iterator rt;
							for (rt = nList->begin(); rt != nList->end(); ++rt)
								rList->push_back(*rt);
							rList->push_back(*mIt);
							//checks, if the new combination already exists in the list
							if (!HasDuplicateEntry(tempNodes, rList))
								tempNodes->push_back(rList);
						}
					}
				}
				if (!tempNodes->empty()) {
					selectedNodes->clear();
					list<SP_ListNode*>::iterator It;
					for (It = tempNodes->begin(); It != tempNodes->end(); ++It){

						selectedNodes->push_back((*It));
					}
					tempNodes->clear();
				}
				index++;
			}
			//list of all combinations is the new list now
			if(!selectedNodes->empty()){
				for (sIt = selectedNodes->begin(); sIt != selectedNodes->end(); ++sIt){
					SP_ListNode* tempList = (*sIt);
					n_pcAllNodes->push_back(tempList);
				}

				selectedNodes->clear();
			}
		}
	}else{
		n_pcAllNodes->push_back(nodeList);
	}

	//At last, the variables of each combination have to be connected with an AND
	//and each combination with an OR.
	NodeMap termMap;
	long index = 0;

	for (sIt = n_pcAllNodes->begin(); sIt != n_pcAllNodes->end(); ++sIt){


		if ((*sIt)->size() == mId) {

			NodeMap tempMap = DoCombineEventsWithGate((*sIt));
			NodeMap::iterator tIt;
			if (sIt != n_pcAllNodes->begin()){
				lNode* orNode = new lNode(true, wxT("OR"), (*nIt));
				termMap[index] = orNode;
				index++;
			}
			for (tIt = tempMap.begin(); tIt != tempMap.end(); ++tIt){
				termMap[index] = (*tIt).second;
				index++;
			}
		}
	}
	return termMap;
}

bool
SP_DS_FTreeLogicalExpressionReader::HasDuplicateEntry(list<SP_ListNode*>* selectedNodes, SP_ListNode* nList)
{
	list<SP_ListNode*>::iterator nIt;

	//check, if the list of nodes is already exists
	for (nIt = selectedNodes->begin(); nIt != selectedNodes->end(); ++nIt){
		SP_ListNode* n_pcNodes = (*nIt);
		SP_ListNode::const_iterator n_pIt;
		unsigned long identicalIndex = 0;
		for (n_pIt = nList->begin(); n_pIt != nList->end(); ++n_pIt){
			if (SP_Find(*n_pcNodes, *n_pIt) != n_pcNodes->end())
				identicalIndex++;
		}
		if (identicalIndex == nList->size())
			return TRUE;
	}
	return FALSE;
}

NodeMap
SP_DS_FTreeLogicalExpressionReader::DoReplaceXORGates(SP_DS_Node* n_pcNode)
{
	const SP_ListEdge* edges = (dynamic_cast<SP_DS_Node*>(n_pcNode))->GetTargetEdges();
	SP_ListEdge::const_iterator eIt;
	SP_ListNode* nodeList = new SP_ListNode();
	NodeMap tempMap;
	long index = 0;

	//First, all entrance-nodes are determined which are connected with the XOR-gate.
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt){
		SP_DS_Node* node = (SP_DS_Node*) (*eIt)->GetSource();
		nodeList->push_back(node);
	}

	//Replacement of the XOR-gate on the basis of OR, AND and NEG
	SP_ListNode::const_iterator nIt;
	for (nIt = nodeList->begin(); nIt != nodeList->end(); ++nIt){

		if (nIt != nodeList->begin()){
			lNode* orNode = new lNode(true, wxT("OR"), (*nIt));
			tempMap[index] = orNode;
			index++;
		}
		lNode* openBracket = new lNode(true, wxT("("), NULL);
		tempMap[index] = openBracket;
		index++;
		if ((*nIt)->GetClassName()== wxT("Basic Event")
			|| (*nIt)->GetClassName()== wxT("Undeveloped Event")){
			wxString sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
			lNode* node = new lNode( true, wxT("X1"+sId), (*nIt));
			tempMap[index] = node;
			index++;
		}else{
			lNode* openBracket = new lNode(true, wxT("("), (*nIt));
			tempMap[index] = openBracket;

			lNode* node = new lNode(false, (*nIt)->GetClassName(), (*nIt));
			tempMap[++index] = node;
			lNode* closeBracket = new lNode(true, wxT(")"), (*nIt));
			tempMap[(index + 2)] = closeBracket;
			index += 3;
		}

		SP_ListNode::const_iterator mIt;
		for (mIt = nodeList->begin(); mIt != nodeList->end(); ++mIt){

			if ((*mIt) != (*nIt)){
				lNode* andNode = new lNode(true, wxT("AND"), (*mIt));
				tempMap[index] = andNode;
				index++;
				if ((*mIt)->GetClassName()== wxT("Basic Event")
					|| (*mIt)->GetClassName()== wxT("Undeveloped Event")){
						lNode* n_node = new lNode(true, wxT("NEG"), (*mIt));
						tempMap[index] = n_node;
						index++;
						wxString sId = (*mIt)->GetAttribute(wxT("ID"))->GetValueString();
						lNode* node = new lNode( true, wxT("X"+sId), (*mIt));
						tempMap[index] = node;
						index ++;
					}else{

						lNode* n_node = new lNode(true, wxT("NEG"), (*mIt));
						tempMap[index] = n_node;
						index++;
						lNode* openBracket = new lNode(true, wxT("("), (*mIt));
						tempMap[index] = openBracket;

						lNode* node = new lNode(false, (*mIt)->GetClassName(), (*mIt));
						tempMap[(++index)] = node;
						lNode* closeBracket = new lNode(true, wxT(")"), (*mIt));
						tempMap[(index +2)] = closeBracket;
						index += 3;
					}
			}
		}
		lNode* closeBracket = new lNode(true, wxT(")"), NULL);
		tempMap[index] = closeBracket;
		index++;
	}

	return tempMap;

}

LiteralMap
SP_DS_FTreeLogicalExpressionReader::DoDeMorgan()
{

	lNode* p;
	long id = 0;

	NodeMap::iterator nIt;
	LiteralMap tempMap;
	LiteralMap::iterator mIt;

	wxString previousNode = wxT("");

	//First, mark of the basic events/undeveloped events
	//as negated nodes if these are connected with a NEG-gate.
	//This takes place by setting the second parameter of the literal
	// as TRUE and by replacing wxT("X" with "X0").
	for (nIt = m_map.begin(); nIt != m_map.end(); ++nIt){

		p = (*nIt).second;

		id++;

		if (p->m_name== wxT("NEG")){
			NodeMap::iterator sIt;
			sIt = nIt;
			++sIt;
			if (!wxStrstr((*sIt).second->m_name, wxT("X")))
				tempMap.insert(SP_PairLongString(id, p->m_name));
		}
		if (wxStrstr(p->m_name, wxT("X"))){
			if (previousNode== wxT("NEG"))
				p->m_name.Replace(wxT("X"), wxT("X0"), TRUE);
			else
				p->m_name.Replace(wxT("X"), wxT("X1"), TRUE);
			tempMap.insert(SP_PairLongString(id, p->m_name));
		}else if (p->m_name !=  wxT("NEG"))
			tempMap.insert(SP_PairLongString(id, p->m_name));
		previousNode = p->m_name;
	}

	LiteralMap nodeToReplace;
	int idBegin = -1;
	int idEnd = -1;
	int counter = 0;
	int closedBracket = 0;
	int openBracket = 0;
	bool nodesToReplace = FALSE;
	//The most inner expression of the logical expression is determined
	//in each case and the rule of deMorgan is applied to this expression.
	for (mIt = tempMap.begin(); mIt != tempMap.end(); ++mIt){

			counter++;
			if ((*mIt).second== wxT("NEG")){
				idBegin = counter;
				nodesToReplace = TRUE;
				openBracket = 0;
				closedBracket = 0;
			}
			if ((*mIt).second== wxT("("))
				openBracket++;
			if ((*mIt).second== wxT(")"))
				closedBracket++;
			//the most inner expression is found. This expression is also
			//connected with an NEG. So replace this expression on the basis
			//of deMorgan.
			if ((*mIt).second== wxT(")") && nodesToReplace
				&& (openBracket == closedBracket)){
				idEnd = counter - 1;
				LiteralMap newMap = DoReplaceNodesByDeMorgan(tempMap, idBegin, idEnd);
				tempMap.clear();
				tempMap = newMap;
				//After the replacement, begins to examine the expression again
				//from the front
				mIt = tempMap.begin();
				nodesToReplace = FALSE;
				counter = 0;
				openBracket = 0;
				closedBracket = 0;
			}
	}

	vMap = tempMap;

	return vMap;
}


LiteralMap
SP_DS_FTreeLogicalExpressionReader::DoReplaceNodesByDeMorgan(LiteralMap nodeMap, int idBegin, int idEnd)
{
	LiteralMap newMap;
	LiteralMap::iterator nIt;
	int counter = 0;
	int id = 0;
	//Replacement of an expression on the basis of deMorgan: Each AND is replaced by
	//OR, each OR by AND and each variable is negated or the negation of the variable
	//is deleted
	for (nIt = nodeMap.begin(); nIt != nodeMap.end(); ++nIt){
		counter++;
		if (counter >= idBegin && counter <= idEnd){
			if ((*nIt).second != wxT("NEG")){
				id++;
				if (wxStrstr((*nIt).second.SubString(0,1), wxT("X0"))){
					(*nIt).second.Replace(wxT("X0"), wxT("X1"), TRUE);
					newMap.insert(SP_PairLongString(id, (*nIt).second));
				}else
					if (wxStrstr((*nIt).second.SubString(0,1), wxT("X1"))){
						(*nIt).second.Replace(wxT("X1"), wxT("X0"), TRUE);
						newMap.insert(SP_PairLongString(id, (*nIt).second));
					}
				if ((*nIt).second== wxT("AND"))
					newMap.insert(SP_PairLongString(id, wxT("OR")));

				if ((*nIt).second== wxT("OR"))
					newMap.insert(SP_PairLongString(id, wxT("AND")));

				//brackets can be immediately inserted in the list
				if ((*nIt).second == wxT("(") || (*nIt).second == wxT(")"))
					newMap.insert(SP_PairLongString(id, (*nIt).second));
			}
		}else{
			id++;
			newMap.insert(SP_PairLongString(id, (*nIt).second));
		}
	}
	return newMap;
}

bool
SP_DS_FTreeLogicalExpressionReader::FindTopEvent(SP_DS_Node* p_pcNode)
{

	//The gate is determined that is connected with the event about
	//an incoming edge. This gate represents the start element of the
	//breath search.
	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;

	m_map.clear();

	edgesSrc = (dynamic_cast<SP_DS_Node*>(p_pcNode))->GetTargetEdges();
	if (edgesSrc->size() > 0) {
		eIt = edgesSrc->begin();
		SP_DS_Node* root = (SP_DS_Node*) (*eIt)->GetSource();
		lNode* negBegin = new lNode( true, wxT("("), root);
		m_map[0] = negBegin;

		lNode* firstNode = new lNode (false, root->GetClassName(), root );
		m_map[1] = firstNode;

		lNode* negEnd = new lNode( true, wxT(")"), root);
		m_map[2] = negEnd;
	}
	return TRUE;
}



