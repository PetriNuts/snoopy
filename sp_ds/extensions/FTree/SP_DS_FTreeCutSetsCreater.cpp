//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:10:00 $
// Short Description: Determine of all cut sets of a fault tree
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/FTree/SP_DS_FTreeCutSetsCreater.h"

SP_DS_FTreeCutSetsCreater::SP_DS_FTreeCutSetsCreater(SP_DS_Graph* p_pcGraph)
: SP_DS_FTreeDNFCreater(p_pcGraph)
{
	//list of all possible cut sets of the fault tree
	cutSets = new list<SP_ListNode*>();
}

SP_DS_FTreeCutSetsCreater::~SP_DS_FTreeCutSetsCreater()
{

}

SP_ListString*
SP_DS_FTreeCutSetsCreater::SelectCutSets (LiteralMap mMap)
{
	//At first, determine the disjunctive normalform of the fault tree
	dnfForm = CreateDNF(mMap);

	//looks for the nodes of the fault tree, that belong to the variables in a cut set
	FindAllCutSets();

	return dnfForm;
}

void
SP_DS_FTreeCutSetsCreater::FindAllCutSets(){

	SP_ListString::iterator pIt;
	SP_ListNode::const_iterator mIt;
	SP_ListNode* cuts;
	SP_ListNode* tmpList = new SP_ListNode();

	//each conjunction of variables form a cut set. Each variable of one cut set,
	//represent  a node in the fault tree. These nodes have to be found.

	for (pIt = dnfForm->begin(); pIt != dnfForm->end(); ++pIt)
	{
		wxString& l_rsP = *pIt;
		cuts = new SP_ListNode();

		//OR gates can be ignored, negated variables are not shown in dialog window
		if (l_rsP !=  wxT("OR") && !LiteralIsNegated(l_rsP)){

			const SP_ListNodeclass* n_pcAllNodeClasses = m_pcGraph->GetNodeclasses();
			SP_ListNodeclass::const_iterator nIt;

			for (nIt = n_pcAllNodeClasses->begin(); nIt != n_pcAllNodeClasses->end(); ++nIt){
				const SP_ListNode* n_pcAllNodes = (*nIt)->GetElements();
				for (mIt = n_pcAllNodes->begin(); mIt != n_pcAllNodes->end(); ++mIt){

					if ((*mIt)->GetClassName()== wxT("Basic Event")
					|| (*mIt)->GetClassName()== wxT("Undeveloped Event")){
						wxString nId = (*mIt)->GetAttribute(wxT("ID"))->GetValueString();

						//the right node of the variable was found. The node is saved in a list that
						//represent a cut set.
						if (nId == l_rsP.SubString(2,l_rsP.length())){
							tmpList->push_back((*mIt));
							break;
						}
					}
				}
			}
		}else{
			//The nodes of one cut sets were found completely. The list is been attached
			//to the main list of all cut sets of a fault tree
			if (!tmpList->empty()){
				for (mIt = tmpList->begin(); mIt != tmpList->end(); ++mIt){
					SP_DS_Node* node = (*mIt);
					cuts->push_back(node);
				}
				cutSets->push_back(cuts);
				tmpList->clear();
			}
		}
	}
	//last cut set of the disjunctive normalform doesn't yet list into the
	//main list of all cut sets. So, this have to be still done.
	if (!tmpList->empty())
		cutSets->push_back(tmpList);
}

list<SP_ListNode*>*
SP_DS_FTreeCutSetsCreater::GetCutSets(){
	return cutSets;
}

