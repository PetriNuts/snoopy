/*
 * SP_DLG_SelectNodes.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 10.10.2011
 * Short Description:
 */
//=================================================

#ifndef SP_DLG_SELECTNODES_H_
#define SP_DLG_SELECTNODES_H_

#include <wx/wx.h>
#include <wx/gauge.h>
#include "spsa/spsa.h"
#include "sp_utilities.h"


class SP_NodeClientData :public wxClientData
{
 private:
	     unsigned long m_nMatrixPosition;
	     spsa::RM_ROW_TYPE m_nNodeType;
public:
	     SP_NodeClientData(const unsigned long& p_nMatrixPosition,
	    		          const spsa::RM_ROW_TYPE& p_nNodeType)
	                      :wxClientData(),
	    		          m_nMatrixPosition(p_nMatrixPosition),
	    		          m_nNodeType(p_nNodeType){}

	     ~SP_NodeClientData(){}

	     //matrix position
	     void SetMatrixPosition(const unsigned long& p_nPosition){m_nMatrixPosition=p_nPosition;}
	     unsigned long GetMatrixPosition(){return m_nMatrixPosition;}

	     //node type (colored/uncolored)
	     void SetNodeType(const spsa::RM_ROW_TYPE& p_nNodeType){m_nNodeType=p_nNodeType;}
	     spsa::RM_ROW_TYPE GetNodeType(){return m_nNodeType;}

	     //make a copy
	     SP_NodeClientData* Clone(){return new SP_NodeClientData(m_nMatrixPosition,m_nNodeType);}
};

class SP_DLG_SelectNodes: public wxDialog
{
private:
	      //current model type: colored/uncolored
	      bool m_bIsColoredModel;

	      //current view ID
	      unsigned int m_nCurrentView;

	      spsa::Model * m_pcCurrentModel;
private:
	      //list of available nodes
	      wxListBox* m_pcAvailableNodes;

	      //list of selected nodes
	      wxListBox* m_pcSelectedNodes;

	      //selected node type(place/transitions)
	      wxRadioBox* m_pcNodeTypes;

	      //colored/ uncolored nodes
	      wxCheckBox* m_pcSelectColorNodes;

	      //colored name
	      wxChoice* m_pcColorName;
protected:
	      void OnNodeTypeChanged(wxCommandEvent& event);

	      void OnMoveLeftToRight(wxCommandEvent& event);

	      void OnMoveRightToLeft(wxCommandEvent& event);

	      void OnMoveAllLeftToRight(wxCommandEvent& event);

	      void OnMoveAllRightToLeft(wxCommandEvent& event);

	      void OnSelectColorNodesCheckUncheck(wxCommandEvent& event);

	      void OnColorNameChanged(wxCommandEvent& event);

	      void OnOk(wxCommandEvent& event);

protected:	      //move selected elements from source list to dest list
	      void MoveElements(wxListBox* p_pcSourceList,wxListBox* p_pcDestList, wxArrayInt& p_anSelections);

	      //move all elements from source to dest list
	      void MoveAllElement(wxListBox* p_pcSourceList,wxListBox* p_pcDestList);

	      void LoadData();

	      void ReadOldUserSelection();

	      //change color names in the checkbox
	      void ChangeColorName();

	      //clear the current node names and add new names
	      void AddNodesToAvailable(const SP_VectorString& p_anNames,const spsa::RM_ROW_TYPE& p_nNodeType);

	      //copy an item from  one list to another
	      void CopyListItem(wxListBox* p_pcSourceList,wxListBox* p_pcDestList,
	      		                             const unsigned int& p_nItemID);

	      //remove items from a list
	      void RemoveItemsFromList(wxListBox* p_pcList,const SP_VectorString& p_anSelections);

	      //change the available list items, when color name changed
	      void ChangeUncoloredNodes(const unsigned int& p_NodeID);

	      DECLARE_EVENT_TABLE();
public:
	      SP_DLG_SelectNodes(wxWindow* p_pcParentWnd,spsa::Model * p_pcCurrentModel,const unsigned int& p_nCurrentView);
	      virtual ~SP_DLG_SelectNodes();
};

#endif /* SP_DLG_SELECTNODES_H_ */
