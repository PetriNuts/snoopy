//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: create a fault tree on the basis of a disjunctive normalform
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportFTree_DNF.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "snoopy.h"

#include "sp_defines.h"

#include "sp_core/FTree/SP_FTree_TermScanner.h"
#include "sp_core/FTree/SP_FTree_TermParser.h"


#include <wx/spinctrl.h>
#include <algorithm>


SP_ImportFTree_DNF::SP_ImportFTree_DNF()
{

}

SP_ImportFTree_DNF::~SP_ImportFTree_DNF()
{

}

bool
SP_ImportFTree_DNF::ReadFile(const wxString& p_sFile)
{
	NodeList = new SP_ListString();
	m_pcScanner = new SP_FTree_TermScanner();
	if(m_pcScanner->Open(p_sFile)){
		m_pcParser = new SP_FTree_TermParser(m_pcScanner);

		wxString l_nodeName;
		m_pcParser->ReadUntilBeginExpression();
		//reads the elements of the disjunctive normalform
		while (m_pcParser->ReadNodeData(l_nodeName))
			NodeList->push_back(wxString::Format(wxT("%s"), l_nodeName.c_str() ));

		if (!m_pcParser->IsEOF()) {
			NodeList->clear();
			SP_MESSAGEBOX(wxT(" The file contains components which are not a member of logical expressions!"),
				wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}else{
			m_pcParser->CheckVariableOfIsNegated();
			CreateGraph(NodeList);
		}
	}
	return TRUE;
}

void
SP_ImportFTree_DNF::CreateGraph(SP_ListString* nodeList)
{
	m_pcGraph = CreateDocument();

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();

	SP_DS_Nodeclass* topClass = m_pcGraph->GetNodeclass(wxT("Top Event"));

	SP_DS_Node* l_topNode = topClass->NewElement(l_pcCanvas->GetNetnumber());

	if (l_topNode)
		l_topNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);


	SP_DS_Nodeclass* basicClass = m_pcGraph->GetNodeclass(wxT("Basic Event"));

	SP_DS_Nodeclass* orClass = m_pcGraph->GetNodeclass(wxT("OR"));
	SP_DS_Node* l_orNode = NULL;

	SP_DS_Nodeclass* andClass = m_pcGraph->GetNodeclass(wxT("AND"));
	SP_DS_Node* l_andNode = NULL;
		SP_ListNode* BasicList = new SP_ListNode();
	SP_DS_Node* prevNode = NULL;

	SP_ListString::iterator bIt;

	for (bIt = nodeList->begin(); bIt != nodeList->end(); ++bIt) {
		wxString& l_rString = *bIt;
		//a variable of the disjunctive normalform presents a basic event in the fault tree
		if (l_rString.Find(wxT("X")) != wxNOT_FOUND) {
			SP_DS_Node* l_basicNode = HasDuplicateNode(l_rString);
			if (!l_basicNode){
			l_basicNode = basicClass->NewElement(l_pcCanvas->GetNetnumber());

				if (l_basicNode) {

					l_basicNode->GetAttribute(wxT("Name"))->SetValueString(l_rString);
					l_basicNode->GetAttribute(wxT("Name"))->SetShow(TRUE);
					wxString bId = l_rString.substr(1, l_rString.length());
					l_basicNode->GetAttribute(wxT("ID"))->SetValueString(bId);
					l_basicNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);
				}

			}	//As long as no OR follows, the basic events are saved in a list
				prevNode = l_basicNode;
				BasicList->push_back(l_basicNode);
		}
		//If an OR follows, the list of basic events forms a conjunction of variables.
		//So, each basic event of this list is connected with an AND-gate.
		if (l_rString == wxT("OR")) {
			if (!l_orNode) {
				l_orNode = orClass->NewElement(l_pcCanvas->GetNetnumber());
				l_orNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);
			}
			 l_andNode = andClass->NewElement(l_pcCanvas->GetNetnumber());
			 l_andNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);
			 drawEdges(BasicList, l_andNode);
			 drawEdge(l_orNode, l_andNode );
			 BasicList = new SP_ListNode();

			prevNode = l_orNode;
		}
		RefreshFrame(m_pcMyDoc);
	}
	//last list of basic events doesn't yet connect with an AND-gate.
	//So, this have to be still done.
	if (!BasicList->empty()){

		l_andNode = andClass->NewElement(l_pcCanvas->GetNetnumber());
		l_andNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);
		drawEdges(BasicList, l_andNode);
		BasicList = new SP_ListNode();
		RefreshFrame(m_pcMyDoc);
	}
	//To all last the AND-gates must be still connected with an OR-gate and
	//the OR-gate with the top event.
	SP_DS_Nodeclass* orList = m_pcGraph->GetNodeclass(wxT("OR"));
	const SP_ListNode* m_AllGates = orList->GetElements();
	if (m_AllGates->empty())
		drawEdge(l_topNode, l_andNode);
	else{
		drawEdge(l_orNode, l_andNode );
		drawEdge(l_topNode, l_orNode);
	}
	RefreshFrame(m_pcMyDoc);
	//Representation of the fault tree as a tree structure
	DoVisualization();


}

SP_DS_Node*
SP_ImportFTree_DNF::HasDuplicateNode(const wxString& basicName)
{
	SP_DS_Node* basicNode = NULL;
	//If there is still a basic event with the same name, the basic event
	//becomes a dependent basic event.
	SP_DS_Nodeclass* basicClass = m_pcGraph->GetNodeclass(wxT("Basic Event"));
	const SP_ListNode* bList = basicClass->GetElements();
	SP_ListNode::const_iterator bIt;

	for (bIt = bList->begin(); bIt != bList->end(); ++bIt){
		wxString bName = (*bIt)->GetAttribute(wxT("Name"))->GetValueString();
		if (bName == basicName) {
			if (!(*bIt)->GetLogical()){
				(*bIt)->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
				(*bIt)->SetLogical(TRUE);
			}
		return (*bIt);
		}
	}

	return basicNode;
}


void
SP_ImportFTree_DNF::drawEdges(SP_ListNode* sourceList, SP_DS_Node* target)
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());

	SP_GUI_Canvas* l_pcCanvas = l_pcView->GetCanvas();

	SP_DS_Nodeclass* negClass = m_pcGraph->GetNodeclass(wxT("NEG"));

	SP_ListNode::const_iterator nIt;
	for (nIt = sourceList->begin(); nIt != sourceList->end(); ++nIt){
		wxString sId = (*nIt)->GetAttribute(wxT("ID"))->GetValueString();
		//If the variable is negated, the basic event is connected with a NEG-gate
		if (m_pcParser->IsNegated(wxT("X" + sId))){
			SP_DS_Node* l_negNode = negClass->NewElement(l_pcCanvas->GetNetnumber());
			l_negNode->ShowOnCanvas(l_pcCanvas, FALSE, 10, 10, 0);
			drawEdge(target, l_negNode);
			drawEdge(l_negNode, (*nIt));
		}else{
			drawEdge(target, (*nIt));
		}


	}
}


