//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ANIMATION_H__
#define __SP_DS_ANIMATION_H__

#include "sp_ds/SP_DS_Animator.h"
#include "sp_ds/SP_DS_Graph.h"
#include <wx/event.h>
#include <wx/timer.h>

class SP_GUI_Canvas;
class SP_DLG_AnimationProperties;
class SP_DLG_Animation;
class SP_DLG_Simulation;

class SP_DS_Animation: public SP_IdCounter, public SP_Error, public wxEvtHandler
{
private:
    DECLARE_EVENT_TABLE()

protected:
    wxTimer m_cTimer;
	
	SP_DS_Graph* m_pcGraph;

    map<SP_Data*, SP_ListAnimator > m_mlData2Animator;

    SP_ListAnimator m_lAllAnimators;
    SP_ListAnimator m_lStepAnimators;
    set<SP_GUI_Canvas*> m_tsCanvas;

    bool CleanAnimators(SP_ListAnimator* p_plAnims);

    bool m_bConnected;
    bool m_bRunning;
    bool m_bOneShot;

    unsigned int m_nRefreshFrequ;
    unsigned int m_nStepDuration;
    unsigned int m_nSteps;
    unsigned int m_nActStep;
	bool m_bColSimMode;
	bool m_bInPreStep;

    //indicates direction for animation
	SP_DIRECTION_T	m_nDir;

	wxString m_sDlgFrequency;
    wxString m_sDlgDuration;

    void ConnectEvent();
    void DisconnectEvent();

    bool FillAnimators(SP_DS_Nodeclass* p_pcNodeclass);
    bool FillAnimators(SP_DS_Edgeclass* p_pcEdgeclass);
    bool FillAnimators(SP_DS_Metadataclass* p_pcMetadataclass);
    bool FillAnimators(SP_DS_Node* p_pcNode, SP_ListAnimator* p_plAnims);
    bool FillAnimators(SP_DS_Edge* p_pcEdge, SP_ListAnimator* p_plAnims);
    bool FillAnimators(SP_DS_Metadata* p_pcMetadata, SP_ListAnimator* p_plAnims);

    unsigned int m_nHistorySize;

    virtual bool PreStep();
    virtual bool Step();
    virtual bool PostStep();

    void Refresh();

    // event
    virtual void OnTimer(wxTimerEvent& p_cEvent);

		SP_ListData m_cFiredTransitions;

	//Simulation
	long m_nPlaceCount;
	SP_MapString2String* m_msPlaceNamesById;
	map<wxString, vector<wxString> >* m_msSimulationTables;
	map<wxString, vector<wxString> >* m_msSimulationPlots;
	map<wxString, SP_MapString2String >* m_msMarkingSets;
	map<wxString, SP_MapString2String >* m_msFunctionSets;
	map<wxString, SP_MapString2String >* m_msParameterSets;
	map<double, SP_MapString2String >* m_mnResultsMap;

	bool m_bIsActiveAnimator;//by george

public:
    SP_DS_Animation(unsigned int p_nRefresh, unsigned int p_nDuration, bool p_bColSimMode=false);
    virtual ~SP_DS_Animation();

    virtual bool Initialise(SP_DS_Graph* p_pcGraph);

    virtual bool RestartTimer();
    virtual bool StartTimer();
    virtual bool StopTimer();

    inline void SetOneShot(bool p_bVal) { m_bOneShot = p_bVal; }
    inline bool GetOneShot() const { return m_bOneShot; }

    // clone
    virtual SP_DS_Animation* Clone();

    virtual bool OnLeftClick(SP_Data* p_pcElement);
    virtual bool OnRightClick(SP_Data* p_pcElement);

    // accessors
    inline bool GetRunning() const { return m_bRunning; }
    inline void SetRunning(bool p_bVal) { m_bRunning = p_bVal; }
    inline unsigned int GetRefreshFrequ() const { return m_nRefreshFrequ; }
    inline void SetRefreshFrequ(unsigned int p_nVal) { m_nRefreshFrequ = p_nVal; }
    inline unsigned int GetStepDuration() const { return m_nStepDuration; }
    inline void SetStepDuration(unsigned int p_nVal) { m_nStepDuration = p_nVal; }
    inline void SetActiveAnim(const bool& p_bActive) { m_bIsActiveAnimator = p_bActive; }//george
    inline bool  IsActiveAnimator()  { return m_bIsActiveAnimator; }//george

	inline void SetDirection(SP_DIRECTION_T p_nDir) { m_nDir = p_nDir; }
	inline SP_DIRECTION_T GetDirection() { return m_nDir; }

    bool AddAnimator(SP_Data* p_pcParent, SP_DS_Animator* p_pcAnimator);
    SP_ListAnimator* GetAnimators(SP_Data* p_pcParent);
    SP_DS_Animator* GetAnimator(SP_Data* p_pcParent, SP_DS_ANIMATOR_TYPE p_eType);

    void AddCanvas(SP_GUI_Canvas* p_pcCanvas) { if (p_pcCanvas) m_tsCanvas.insert(p_pcCanvas); }

    virtual bool AddToDialog(SP_DLG_AnimationProperties* p_pcDlg, wxSizer* p_pcSizer);
    virtual bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer) { return TRUE; }
    virtual bool OnDialogOk();
	virtual void OnReset();

	//New virtual functions for Simulation
	virtual bool AddToSimulation(SP_DLG_Simulation* p_pcDlg, wxSizer* p_pcSizer) { return false; }
	virtual void InitSimulation() { }
	virtual long GetPlaceCount() { return m_nPlaceCount; }
	virtual SP_MapString2String* GetPlaceNamesById() { return m_msPlaceNamesById; }
	virtual map<wxString, vector<wxString> >* GetSimulationTables() { return m_msSimulationTables; }
	virtual map<wxString, vector<wxString> >* GetSimulationPlots() { return m_msSimulationPlots; }
	virtual map<wxString,SP_MapString2String >* GetMarkingSets() { return m_msMarkingSets; }
	virtual map<wxString,SP_MapString2String >* GetFunctionSets() { return m_msFunctionSets; }
	virtual map<wxString,SP_MapString2String >* GetParameterSets() { return m_msParameterSets; }
	virtual map<double, SP_MapString2String >* GetResultsMap() { return m_mnResultsMap; }
};

#endif // __SP_DS_ANIMATION_H__


