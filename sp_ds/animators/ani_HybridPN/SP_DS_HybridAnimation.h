/*
 * SP_DS_HybridAnimation.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 26.8.2010
 * Short Description:
 */
 //==============================================================================


#ifndef SP_DS_HYBRIDANIMATOR_H_
#define SP_DS_HYBRIDANIMATOR_H_
#include "sp_ds/extensions/SP_DS_Animation.h"

class SP_DS_HybridAnimation: public SP_DS_Animation
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
	SP_DS_HybridAnimation(unsigned int p_nRefresh = 1000,
								 unsigned int p_nDuration = 1000,
								 unsigned int p_nRounds = 50,
								 double p_nRoundSize = 1.0);
	virtual ~SP_DS_HybridAnimation();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	SP_DS_Animation* Clone();

	bool AddToControl(SP_DLG_Animation* p_pcCtrl, wxSizer* p_pcSizer);

	// special t oHybrid animation

	void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

	void OnOpenSimulation(wxCommandEvent& p_cEvent);
	void OpenSimulation();
};

#endif /* SP_DS_HYBRIDANIMATOR_H_ */
