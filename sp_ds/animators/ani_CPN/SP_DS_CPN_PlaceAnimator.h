//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/28 10:20:00 $
// Short Description: colored place animator class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_CPN_PlaceAnimator_H__
#define __SP_DS_CPN_PlaceAnimator_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_gr/animators/SP_GR_MarkAnimator.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_PlaceMultiSet.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"


class SP_DS_CPN_TransAnimator;

class SP_DS_CPN_PlaceAnimator: public SP_DS_Animator
{
private:
protected:
    SP_GR_MarkAnimator* m_pcProtoshape;

    SP_DS_Node* m_pcNode;
    wxString m_sAttributeName;
	SP_DS_ColListAttribute* m_pcColList;
    SP_DS_MarkingAttribute* m_pcAttribute;
	SP_DS_TextAttribute* m_pcNameAttibute;

	//special to colored petri nets
	SP_CPN_PlaceMultiSet m_cPlaceMultiSet;
	SP_MapString2Int m_mDefaultValue;
	map<wxString, wxString > m_mDefaultMarking;	

	bool m_bChooseRandomColor;

    // special to places
    map<SP_DS_CPN_TransAnimator*, SP_ListEdge > m_mlCandidates;

    long m_nDefaultValue;
    virtual bool DecrementMark();
   
	SP_MapString2Int SumCandidatesNew();
	SP_MapString2Int SumEdgesNew(SP_DS_CPN_TransAnimator* p_pcTransAnimator, const SP_ListEdge* p_plEdges);
	SP_MapString2Int SumEdgeNew(SP_DS_CPN_TransAnimator* p_pcTransAnimator, SP_DS_Edge* p_pcEdge);

public:
    SP_DS_CPN_PlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_CPN_PlaceAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    // special to places
    bool AddCandidate(SP_DS_Edge* p_pcEdge, SP_DS_CPN_TransAnimator* p_pcAnimator);
    bool RemoveCandidate(SP_DS_CPN_TransAnimator* p_pcAnimator);
    virtual bool TestMarking();

    virtual bool ResolveConflicts();
    bool ResolveConflicts1();

    bool ResetMarking();
    bool SetMarking();

    bool Enable(SP_DS_CPN_TransAnimator* p_pcAnim);

    bool IncrementMark(SP_DS_CPN_TransAnimator* p_pcTransAnimator, SP_DS_Edge* p_pcEdge);

    SP_DS_Node* GetNode() { return m_pcNode; }


    void SetMarking( long p_nMarking );
    void SetDefaultMarking( long p_nMarking );

    void InitCanvas();

    long GetMarking();

    bool OnLeftClick();
    bool OnRightClick();

	///special to colored petri nets
	void UpdateMultiSet();
	SP_CPN_PlaceMultiSet* GetPlaceMultiSet(){ return &m_cPlaceMultiSet;}	

	void ResetEdge(SP_DS_Edge* p_pcEdge);
	bool UpdateMarking();
	void UpdateDefaultMarking();
};

#endif // __SP_DS_CPN_PlaceAnimator_H__


