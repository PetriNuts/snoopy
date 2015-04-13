//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/07 12:15:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_CONTINUOUSPEDANIMATION_H__
#define __SP_DS_CONTINUOUSPEDANIMATION_H__

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/spinctrl.h>

#include "sp_ds/extensions/SP_DS_Animation.h"

class SP_DS_ContinuousPedAnimation: public SP_DS_Animation
{
private:
	DECLARE_EVENT_TABLE()
	wxWindow* m_pcParent;

	SP_DLG_AnimationProperties* m_pcDlg;
	SP_DS_Graph* m_pcGraph;

	wxString m_sRefreshFrequ;
	wxSpinCtrl* m_pcRefreshFrequSpinCtrl;

protected:


public:
	SP_DS_ContinuousPedAnimation(unsigned int p_nRefresh = 1000,
								 unsigned int p_nDuration = 1000,
								 unsigned int p_nRounds = 50,
								 double p_nRoundSize = 1.0);
	virtual ~SP_DS_ContinuousPedAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);

	// special to continuous ped animation

	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

	void OnOpenSimulation(wxCommandEvent& p_cEvent);
	void OpenSimulation();
};

#endif // __SP_DS_CONTINUOUSPEDANIMATION_H__
