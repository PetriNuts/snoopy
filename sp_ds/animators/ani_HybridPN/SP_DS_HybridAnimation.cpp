/*
 * SP_DS_HybridAnimation.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 26.8.2010
 * Short Description:
 */
 //==============================================================================
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>

#include "sp_ds/animators/ani_HybridPN/SP_DS_HybridAnimation.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_gui/dialogs/dia_HybridPN/SP_DLG_HybridSimultionResults.h"
#include "sp_ds/extensions/HybridPN/SP_DS_CheckTransitions.h"
enum
{
	SP_ID_HYBRIDSET = SP_ID_LAST_ID + 1,
	SP_ID_HYBRIDSIMULPLACESELECTION,
	SP_ID_HYBRIDOPENSIMULATION
};

BEGIN_EVENT_TABLE(SP_DS_HybridAnimation, SP_DS_Animation)
EVT_BUTTON(SP_ID_HYBRIDOPENSIMULATION, SP_DS_HybridAnimation::OnOpenSimulation)
EVT_UPDATE_UI_RANGE(SP_ID_HYBRIDSET, SP_ID_HYBRIDSIMULPLACESELECTION, SP_DS_HybridAnimation::OnUpdateUI)
END_EVENT_TABLE()

SP_DS_HybridAnimation::SP_DS_HybridAnimation(unsigned int p_nRefresh,
                                                           unsigned int p_nDuration,
                                                           unsigned int p_nRounds,
                                                           double p_nRoundSize):
SP_DS_Animation(p_nRefresh, p_nDuration), m_pcParent(NULL),
m_pcDlg(NULL),m_pcGraph(NULL), m_sRefreshFrequ(wxT(""))
{


}

SP_DS_HybridAnimation::~SP_DS_HybridAnimation()
{


}

SP_DS_Animation* SP_DS_HybridAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_HybridAnimation(m_nRefreshFrequ,
			m_nStepDuration));
}

bool SP_DS_HybridAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
	SP_MESSAGEBOX(wxT("No animation for Hybrid Petri Nets!"), wxT("Notice"));
	return false;
/*
	 m_pcGraph = p_pcGraph;

	 SP_DS_CheckTransitions l_checkTrans(m_pcGraph);
	 if(!l_checkTrans.CheckRateFunction())
	 {
		 SP_MESSAGEBOX(wxT("Simulation can not initialize"),wxT("Hybrid Simulation"));
		 return false; //there is error in one of the transitions rate functions
	 }

	return true;
*/
}




bool SP_DS_HybridAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl,
		wxSizer* p_pcSizer)
{
	if (!p_pcCtrl || !p_pcSizer)
		return FALSE;

	m_pcParent = p_pcCtrl;

	p_pcSizer->Add(new wxStaticLine(p_pcCtrl,
			-1,
			wxDefaultPosition,
			wxDefaultSize,
			wxLI_HORIZONTAL), 0, wxEXPAND);
	wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	p_pcSizer->Add(new wxStaticText(p_pcCtrl,
			-1,
			wxT("Hybrid Petri Net Simulation:")), 0, wxEXPAND | wxLEFT | wxTOP,
			5);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxButton(p_pcCtrl,
			SP_ID_HYBRIDOPENSIMULATION,
			wxT("Open Simulation")), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	p_pcSizer->Add(l_pcSizer, 1, wxEXPAND);

	p_pcCtrl->PushEventHandler(this);

	return TRUE;
}

void SP_DS_HybridAnimation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable(!m_bRunning);
}

void SP_DS_HybridAnimation::OnOpenSimulation(wxCommandEvent& p_cEvent)
{
	OpenSimulation();
}

void SP_DS_HybridAnimation::OpenSimulation()
{
	//hide Animation dialog because of wxSTAY_ON_TOP
	m_pcParent->Show(false);

	SP_DLG_HybridSimulationResults * l_pcDlg = new SP_DLG_HybridSimulationResults( m_pcGraph, NULL );
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();

	//show Animation dialog after Simulationresults close
	m_pcParent->Show(true);

}


