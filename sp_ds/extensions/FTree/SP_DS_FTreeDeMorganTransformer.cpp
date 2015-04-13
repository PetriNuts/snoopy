//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.00 $
// $Date: 2006/12/26 14:21:00 $
// Short Description: transform a fault tree by application of deMorgan
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeDeMorganTransformer.h"
#include "snoopy.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"

SP_DS_FTreeDeMorganTransformer::SP_DS_FTreeDeMorganTransformer()
{
	//determine of the graph to whom the rule should be applied
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);

	m_pcGraph = l_pcDoc->GetGraph();

	varToNodeMap = new map<wxString, SP_DS_Node*>();

}

SP_DS_FTreeDeMorganTransformer::~SP_DS_FTreeDeMorganTransformer()
{
	varToNodeMap->clear();
}

void
SP_DS_FTreeDeMorganTransformer::TransformGraph()
{
	if (IsApplicable())
		CreateGraph();
	else
		SP_MESSAGEBOX(wxString::Format(wxT("There are no elements in the fault tree with which one can apply the de Morgan-rules. ")),
		     wxT("Application of de Morgan"),
		     wxICON_INFORMATION);
}

void
SP_DS_FTreeDeMorganTransformer::CreateGraph()
{

	SP_DS_Nodeclass* topEventClass = m_pcGraph->GetNodeclass(wxT("Top Event"));
	const SP_ListNode* l_pcAllNodes = topEventClass->GetElements();

	if (l_pcAllNodes->size() != 0){

		SP_ListNode::const_iterator nIt = l_pcAllNodes->begin();

		//determine the logical expression of the fault tree
		SP_DS_FTreeLogicalExpressionReader* readExp = new SP_DS_FTreeLogicalExpressionReader();
		NodeMap logExpression = readExp->FindLogicalExpression((*nIt));

		//use de Morgan
		LiteralMap logExprAfterDeMorgan = readExp->DoDeMorgan();

		//look and write in each case the most inner expression
		//of the function in a stack (serves the correct evaluation of the equation)
		CreateForm(logExprAfterDeMorgan);

		//create new document
		m_pcNewGraph = CreateDocument();

		TermMap::iterator termIt;

		//Processing of the stack: the tree is constructed iterative
		//(takes place from below upwards to the root)
		for (termIt = tMap.begin(); termIt != tMap.end(); ++termIt) {
			SP_ListString* list = (*termIt).second;

			SP_ListString::iterator sIt;
			for (sIt = list->begin(); sIt != list->end(); ++sIt){

				if ((*sIt)== wxT("OR") ) {
					CreatePartTree((*termIt).first, list, wxT("OR"));
					break;
				}
				if ((*sIt)== wxT("AND") ) {
					CreatePartTree((*termIt).first, list, wxT("AND"));
					break;
				}
			}
		}
		//The last element in the list is connected with the top event.
		//First a new node from type wxT("Top Event") is created which receives
		//all attributes of the Top Event from the original fault tree.
		SP_DS_Node* topEvent = CreateNode(wxT("Top Event"));
		CopyAttributes((*nIt), topEvent);
		//This node is connected with the last node in the list.
		map<wxString, SP_DS_Node*>::iterator l_pcIt;
		l_pcIt = varToNodeMap->end();
		l_pcIt--;
		drawEdge(topEvent, (*l_pcIt).second);

		//Representation of the graph as a tree.
		DoVisualization();

	}
}

void
SP_DS_FTreeDeMorganTransformer::CreatePartTree(const wxString& varName, SP_ListString* NodeList, const wxString& gateType)
{
	//Dependent on type of the logic operation, the gate which represents
	//the logic operation, is produced first
	SP_DS_Node* gate = NULL;
	if (gateType == wxT("AND") || gateType == wxT("OR"))
		gate = CreateNode(gateType);

	SP_ListNode* nodesToConnect = new SP_ListNode();

	//Afterwards all nodes (events or gates) are created as a new node
	//in each case which are connected by the logic operation with
	//each other. These nodes are inserted in a list. Every node from this
	//list is connected later with the gate by an edge.
	SP_ListString::iterator vIt;
	for (vIt = NodeList->begin(); vIt != NodeList->end(); ++vIt){
		wxString& l_rString = *vIt;
		//If have to be decided whether the variable represents a basic event or a gate.
		//Only basic events have to be created.
		if (wxStrstr(l_rString, wxT("X"))){
			SP_DS_Node* newNode = CreateNode(l_rString);
			nodesToConnect->push_back(dynamic_cast<SP_DS_Node*>(newNode));
		}
		//The variable represents a part tree. Looks for the gate which connects all nodes
		//of this part tree.
		if (wxStrstr(l_rString, wxT("Y"))){
			map<wxString, SP_DS_Node*>::iterator l_pcIt;
			for (l_pcIt = varToNodeMap->begin(); l_pcIt != varToNodeMap->end(); ++l_pcIt){
				if ((*l_pcIt).first== l_rString){
					nodesToConnect->push_back(dynamic_cast<SP_DS_Node*>((*l_pcIt).second));
					break;
				}
			}
		}
	}

	//Connect all nodes from the list with the gate by an edge.
	SP_ListNode::const_iterator nIt;
	if (!nodesToConnect->empty())
		for (nIt = nodesToConnect->begin(); nIt != nodesToConnect->end(); ++nIt)
			drawEdge(gate, (*nIt));

	//remember the variable which represents a part tree. The gate of
	//this part tree will be a node of the next part tree (logical expression).
	varToNodeMap->insert(pair< wxString, SP_DS_Node*>
											(varName, gate));
}

SP_DS_Node*
SP_DS_FTreeDeMorganTransformer::CreateNode(const wxString& nodeName)
{
	SP_DS_Node* newNode = NULL;

	if (wxStrstr(nodeName, wxT("X"))){

		//A variable represents a basic event. So, looks for the
		//ID of the basic event in the original tree which is identical
		//to the ID of the variable. Copy the attributes of this basic event.
		//These attributes are the same attributes of the variable.
		wxString varID = nodeName.SubString(2, nodeName.length());

		newNode = HasDuplicateNode(varID);


		const SP_ListNodeclass* nodeClasses = m_pcGraph->GetNodeclasses();
		SP_ListNodeclass::const_iterator nIt;
		for (nIt = nodeClasses->begin(); nIt != nodeClasses->end(); ++nIt){
			const SP_ListNode* l_pcAllNodes = (*nIt)->GetElements();
			SP_ListNode::const_iterator mIt;
			for (mIt = l_pcAllNodes->begin(); mIt != l_pcAllNodes->end(); ++mIt){
				if ((*mIt)->GetClassName()== wxT("Basic Event")){
					wxString nodeID = (*mIt)->GetAttribute(wxT("ID"))->GetValueString();
					if (nodeID== varID){

						if (!newNode){
							SP_DS_Nodeclass* basicClass = m_pcNewGraph->GetNodeclass(wxT("Basic Event"));
							newNode = basicClass->NewElement(m_pcCanvas->GetNetnumber());
						}
						newNode->ShowOnCanvas(m_pcCanvas, FALSE, 10, 10, 0);
						CopyAttributes((*mIt), newNode);

						if (wxStrstr(nodeName, wxT("X0"))){
							SP_DS_Nodeclass* negClass = m_pcNewGraph->GetNodeclass(wxT("NEG"));
							SP_DS_Node* negNode = negClass->NewElement(m_pcCanvas->GetNetnumber());
							if (negNode){
								negNode->ShowOnCanvas(m_pcCanvas, FALSE, 10, 10, 0);
								drawEdge(negNode, newNode);
							}
							return negNode;
						}

						return newNode;
					}
				}
			}
		}
	}

	if (nodeName== wxT("AND")){

		SP_DS_Nodeclass* andClass = m_pcNewGraph->GetNodeclass(wxT("AND"));

		newNode = andClass->NewElement(m_pcCanvas->GetNetnumber());

		if (newNode)
			newNode->ShowOnCanvas(m_pcCanvas, FALSE, 10, 10, 0);
	}

	if (nodeName== wxT("OR")){

		SP_DS_Nodeclass* orClass = m_pcNewGraph->GetNodeclass(wxT("OR"));

		newNode = orClass->NewElement(m_pcCanvas->GetNetnumber());

		if (newNode)
			newNode->ShowOnCanvas(m_pcCanvas, FALSE, 10, 10, 0);
	}

	if (nodeName== wxT("Top Event")){

		SP_DS_Nodeclass* topEventClass = m_pcNewGraph->GetNodeclass(wxT("Top Event"));

		newNode = topEventClass->NewElement(m_pcCanvas->GetNetnumber());

		if (newNode)
			newNode->ShowOnCanvas(m_pcCanvas, FALSE, 10, 10, 0);
	}

	return newNode;
}

SP_DS_Node*
SP_DS_FTreeDeMorganTransformer::HasDuplicateNode(const wxString& nodeID)
{
	SP_DS_Node* basicNode = NULL;
	//If there is still a basic event with the same name, the basic event
	//becomes a dependent basic event.
	SP_DS_Nodeclass* basicClass = m_pcNewGraph->GetNodeclass(wxT("Basic Event"));
	const SP_ListNode* bList = basicClass->GetElements();
	SP_ListNode::const_iterator bIt;

	for (bIt = bList->begin(); bIt != bList->end(); ++bIt){
		wxString ID = (*bIt)->GetAttribute(wxT("ID"))->GetValueString();
		if (ID== nodeID) {
			if (!(*bIt)->GetLogical()){
				(*bIt)->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
				(*bIt)->SetLogical(TRUE);
			}
		return (*bIt);
		}
	}

	return basicNode;
}


bool
SP_DS_FTreeDeMorganTransformer::IsApplicable()
{
	SP_DS_Nodeclass* negNodeclass = m_pcGraph->GetNodeclass(wxT("NEG"));

	const SP_ListNode* l_pcAllNodes = negNodeclass->GetElements();

	if (l_pcAllNodes->size() == 0)
		return FALSE;
	else{
		SP_ListNode::const_iterator nIt;
		for (nIt = l_pcAllNodes->begin(); nIt != l_pcAllNodes->end(); ++nIt){
			const SP_ListEdge* trgEdges = (*nIt)->GetTargetEdges();
			SP_ListEdge::const_iterator eIt;
			for (eIt = trgEdges->begin(); eIt != trgEdges->end(); ++eIt){
				SP_DS_Node* sourceNode = (SP_DS_Node*) (*eIt)->GetSource();
				if (sourceNode->GetClassName() !=  wxT("Basic Event")
					&& sourceNode->GetClassName() !=  wxT("Undeveloped Event")
					&& sourceNode->GetClassName() !=  wxT("NEG"))
					return TRUE;
			}
		}
	}
	return FALSE;
}

