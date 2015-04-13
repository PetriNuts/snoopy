//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Animation.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global animation preferences
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_GPR_ANIMATION_H__)
#define __SP_GPR_ANIMATION_H__

#define DEFAULT_REFRESH 50
#define DEFAULT_DURATION 2000
#define DEFAULT_STEPPING SP_ANIM_STEP_INTERMEDIATE
#define DEFAULT_KEEPMARKING false
#define DEFAULT_ANALYSIS SP_FT_QUALITATIVE
#define DEFAULT_SYSTEM SP_FT_SYSTEM_REPAIRABLE
#define DEFAULT_SHOW_PATH true
#define DEFAULT_ROUNDSIZE 1
#define DEFAULT_ROUNDS 50
#define DEFAULT_TOLERANCE 0.000001
#define DEFAULT_REFRESHFREQU 1
#define HISTORY_MAX 10

#include "sp_defines.h"
#include "sp_core/SP_GPR_Base.h"

class SP_DLG_GlobalPreferences;
class wxSpinCtrl;

class SP_GPR_Animation: public SP_GPR_Base {

private:

protected:
	//member variables
	SP_MapString2Int m_mnRefresh;
	SP_MapString2Int m_mnDuration;
	SP_MapString2Int m_mnHistorySize;
	map<wxString, SP_ANIM_STEP_T> m_meStepping;
	bool m_bKeepMarking;

	//Fault Trees
	map<wxString, SP_FT_ANALYSIS_TYPE> m_analysisType;
	map<wxString, SP_FT_SYSTEM_TYPE> m_systemType;
	SP_MapString2Bool m_mShowPath;

  // Continuous Petri Nets
	SP_MapString2Int m_mnRoundSize;
	SP_MapString2Int m_mnRounds;
	SP_MapString2Double m_mnTolerance;
	SP_MapString2Int m_mnRefreshFrequ;

	// music petri net
	SP_MapString2String m_mSoundPath;

	//dialog controls
	wxSpinCtrl *m_scRefresh;
	wxSpinCtrl *m_scDuration;
	wxSpinCtrl *m_scHistorySize;
	wxRadioBox *m_rbStepping;
	wxRadioBox *m_analysisTypes;
	wxRadioBox *m_systemTypes;
	wxCheckBox *m_showPath;

  // Continuous Petri Nets
  wxSpinCtrl *m_scRoundSize;
  wxSpinCtrl *m_scRounds;
  wxTextCtrl *m_tcTolerance;
  wxSpinCtrl *m_scRefreshFrequ;

	// sound library path as used by music petri nets
	wxTextCtrl *m_tcSoundPath;


public:
	SP_GPR_Animation();
	virtual ~SP_GPR_Animation();

	//setter
	void SetRefresh(const wxString& p_sNetClass, int p_nVal);
	void SetDuration(const wxString& p_sNetClass, int p_nVal);
	void SetStepping(const wxString& p_sNetClass, SP_ANIM_STEP_T p_eVal);
	void SetHistorySize(const wxString& p_sNetClass, int p_nVal);
	inline void SetKeepMarking(bool p_bVal) { m_bKeepMarking = p_bVal; }
	void SetAnalysisType(const wxString& p_sNetClass, SP_FT_ANALYSIS_TYPE p_eVal);
	void SetSystemType(const wxString& p_sNetClass, SP_FT_SYSTEM_TYPE p_eVal);
	void SetShowPath(const wxString& p_sNetClass, bool p_bVal);

  void SetRoundSize(const wxString& p_sNetClass, int p_nVal);
  void SetRounds(const wxString& p_sNetClass, int p_nVal);
  void SetTolerance(const wxString& p_sNetClass, double p_nVal);
  void SetRefreshFrequ(const wxString& p_sNetClass, int p_nVal);
  void SetSoundPath(const wxString& p_sNetClass, const wxString& p_sVal);

	//getter
	int GetRefresh(const wxString& p_sNetClass);
	int GetDuration(const wxString& p_sNetClass);
	SP_ANIM_STEP_T GetStepping(const wxString& p_sNetClass);
	int GetHistorySize(const wxString& p_sNetClass);
	inline bool GetKeepMarking() { return m_bKeepMarking; }
	SP_FT_ANALYSIS_TYPE GetAnalysisType(const wxString& p_sNetClass);
	SP_FT_SYSTEM_TYPE GetSystemType(const wxString& p_sNetClass);
	inline bool ShowPath(const wxString& p_sNetClass) { return m_mShowPath[p_sNetClass]; }

  int GetRoundSize(const wxString& p_sNetClass);
  int GetRounds(const wxString& p_sNetClass);
  double GetTolerance(const wxString& p_sNetClass);
  int GetRefreshFrequ(const wxString& p_sNetClass);

  wxString GetSoundPath(const wxString& p_sNetClass);

	bool AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage);
	bool OnDialogOk(const wxString& p_sNetClass);
	bool OnKlick(const wxString& p_sNetClass);

	bool UpdateFromConfig(wxConfig& p_config);
	bool WriteToConfig(wxConfig& p_config);

	void OnUpdateSound();
};

#endif // !defined(__SP_GPR_ANIMATION_H__)
