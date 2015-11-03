//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: sle $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/11/03 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_STANIMATION_H__
#define __SP_DS_STANIMATION_H__

#include <wx/wx.h>
#include "sp_ds/extensions/SP_DS_PedAnimation.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimGillespie.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"


#include "spsim/spsim.h"

class SP_DS_StAnimation: public SP_DS_PedAnimation
{

  private:

    DECLARE_EVENT_TABLE()

    spsim::Gillespie* m_pcSimulator;

    bool m_bRestartAnimationFlag;

	vector<SP_DS_ColListAttribute*> m_apcColListAttr;
	vector<wxChoice*> m_apcComboBoxes;

    wxStaticText* m_pcOutputLabelStaticText;

    //bysl
    SP_SetString  m_choices;

    void LoadSets();
    void UpdateChoices();

    void OnModifyFunctionSets( wxCommandEvent& p_cEvent );
	void OnModifyDelaySets( wxCommandEvent& p_cEvent );
	void OnModifyScheduleSets( wxCommandEvent& p_cEvent );
	void OnModifyWeightSets( wxCommandEvent& p_cEvent );

    void OnOpenSimulation( wxCommandEvent& p_cEvent );

    void LoadCurrentMarking();

    void OnSetsChanged( wxCommandEvent& p_cEvent );
    void OnConstantSetsChanged(wxCommandEvent& p_cEvent);


    void SetCurrentMarking();
    void UpdateCurrentMarking();

	bool CheckRateFunction( );

    bool ReduceTransitions();

    map< int, SP_DS_TransAnimator* > m_mpcTransitionAnimators;
    map< int, SP_DS_PlaceAnimator* > m_mpcPlaceAnimators;

    bool StartTimer();
    int ChooseTransition();

    virtual bool PreStep();

  protected:

   virtual void LoadPlaces();

   virtual void LoadTransitions();

   virtual void LoadConnections();

   virtual void LoadParameters();

   virtual void LoadConnectionOfType(const wxString& p_sArcType,const spsim::ConnectionType& p_ArcType);

   virtual wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);

  public:

    SP_DS_StAnimation(unsigned int p_nRefresh, unsigned int p_nDuration, SP_ANIM_STEP_T p_eStepping);
    virtual ~SP_DS_StAnimation();

    bool Initialise(SP_DS_Graph* p_pcGraph);

    SP_DS_Animation* Clone();

    virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
    virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);

    virtual bool OnDialogOk();

	virtual void OnReset();


};

#endif // __SP_DS_PEDANIMATION_H__


