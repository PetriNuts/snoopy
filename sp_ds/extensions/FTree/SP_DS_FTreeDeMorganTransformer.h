//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.0 $
// $Date: 2006/12/26 14:18:00 $
// Short Description: transform a fault tree by application of deMorgan
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEDEMORGANTRANSFORMER_H__
#define __SP_DS_FTREEDEMORGANTRANSFORMER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeAlgebraicEquationCreater.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCreater.h"
#include <wx/wx.h>

	/**
	\brief Create a new fault tree after application of deMorgan
	*/

class SP_DS_FTreeDeMorganTransformer: public SP_DS_FTreeAlgebraicEquationCreater, public SP_DS_FTreeCreater
{
private:
	
	map<wxString, SP_DS_Node*> *varToNodeMap;

protected:
	
	/**
	\brief Checks whether the rule of de Morgan is applicable
	
	\returns	TRUE, FALSE
	*/
	bool IsApplicable(); 

	/**
	\brief Creates new fault tree after Application of de Morgan

	*/
	void CreateGraph();

	/**
	\brief Creates a new node
	
	\param		nodeName	type of the node
	\returns	newNode		created node
	*/
	SP_DS_Node* CreateNode(const wxString& nodeName);

	/**
	\brief Creates a part tree
	On the basis of a logical expression a tree is created.
	
	\param	varName		name of the variable which represents a part tree
	\param	NodeList	elements of the logical expression
	\param	gateType	type of logical operation
	*/
	void CreatePartTree(const wxString& varName, SP_ListString* NodeList, const wxString& gateType);

	/**
	\brief The test whether there is a basic event with the same name 
	
	 \param		nodeID		ID of the node
	 \returns	basicNode		dependent node
	 */
	SP_DS_Node* HasDuplicateNode(const wxString& nodeID);

public:
    SP_DS_FTreeDeMorganTransformer();

    ~SP_DS_FTreeDeMorganTransformer();

	/**
	\brief Start to transform the graph on the basis of de Morgan

	*/
	void TransformGraph();

};

#endif // __SP_DS_FTREEDEMORGANTRANSFORMER_H__


