//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/12/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColPN_ExportUnfolding_H__
#define __SP_DS_ColPN_ExportUnfolding_H__

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

class SP_DS_ColPN_ExportUnfolding : public SP_DS_ColPN_Unfolding
{

public:

	SP_DS_ColPN_ExportUnfolding();
	virtual ~SP_DS_ColPN_ExportUnfolding();

protected:

	double m_nNetSize;

	map<wxString, SP_DS_Node*> m_mOldPlaceNodes;
	map<wxString, SP_DS_Node*> m_mOldTransitionNodes;
	map<wxString, SP_DS_Edge*> m_mOldEdges;

	



	map<wxString, SP_DS_Node*> m_mNewDiscretePlaceNodes;
	map<wxString, SP_DS_Node*> m_mNewContinuousPlaceNodes;

	map<wxString, SP_DS_Node*> m_mNewDiscreteTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewContinuousTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewImmediateTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewDeterministicTransitionNodes;
	map<wxString, SP_DS_Node*> m_mNewScheduledTransitionNodes;

	SP_ListEdge m_lNewEdges;


	//for removing redandunt graphics
	map<SP_Graphic*, SP_DS_Node*> m_mGraphic2PlaceNode;
	map<SP_Graphic*, SP_DS_Node*> m_mGraphic2TransNode;



protected:

	virtual bool ProcessResult();


private:
	void Initialize();
	void GetOldTransitons(wxString p_sNodeClass);
	bool CreateDuplicatePlaceInstances(); 
	bool CreateDuplicatePlaceNodeType( SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces, 
									   wxString p_sNodeClass, map<wxString, SP_DS_Node*>& p_mNewPlaceNodes); 


	bool CreateDuplicateTransInstances();
	bool CreateDuplicateTransNodeType( SP_CPN_UnfoldedTransitions& p_mmTransitions, 
		                               wxString p_sNodeClass, map<wxString, SP_DS_Node*>& p_mNewTransNodes); 

	bool CreateDuplicateEdge(SP_DS_Node* p_pcTransOldNode, SP_DS_Node* p_pcTransNewNode, SP_CPN_UnfoldedArcInfo& p_UnfoldedArcInfo, bool p_bInput);


    void RemoveIsolatedNoed( map<wxString, SP_DS_Node*>* p_pmNodes );

	double GetNetSize();



public:
	map<wxString, SP_DS_Node*>& GetNewDiscretePlaceNodes() {return m_mNewDiscretePlaceNodes;}
	map<wxString, SP_DS_Node*>& GetNewContinuousPlaceNodes()  {return m_mNewContinuousPlaceNodes;}

	map<wxString, SP_DS_Node*>& GetNewDiscreteTransitionNodes() {return m_mNewDiscreteTransitionNodes;}
	map<wxString, SP_DS_Node*>& GetNewContinuousTransitionNodes() {return m_mNewContinuousTransitionNodes;}
	map<wxString, SP_DS_Node*>& GetNewImmediateTransitionNodes() {return m_mNewImmediateTransitionNodes;}
	map<wxString, SP_DS_Node*>& GetNewDeterministicTransitionNodes() {return m_mNewDeterministicTransitionNodes;}
	map<wxString, SP_DS_Node*>& GetNewScheduledTransitionNodes() {return m_mNewScheduledTransitionNodes;}

	SP_ListEdge& GetNewEdges() { return m_lNewEdges;}

	void Reset();



	
};

#endif // __SP_DS_ColPN_ExportUnfolding_H__


