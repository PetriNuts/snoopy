//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/07 12:15:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>

#include "sp_ds/extensions/SP_DS_ContinuousPedAnimation.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/SP_DLG_Animation.h"
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_ds/extensions/HybridPN/SP_DS_CheckTransitions.h"
//
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNSimulationResults.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

enum
{
	SP_ID_CONTINUOUSPEDRESET = SP_ID_LAST_ID + 1,
	SP_ID_CONTINUOUSPEDSAVEALLVALUES,
	SP_ID_CONTINUOUSPEDSIMULPLACESELECTION,
	SP_ID_CONTINUOUSPEDOPENSIMULATION
};

BEGIN_EVENT_TABLE(SP_DS_ContinuousPedAnimation, SP_DS_Animation)
EVT_BUTTON(SP_ID_CONTINUOUSPEDOPENSIMULATION, SP_DS_ContinuousPedAnimation::OnOpenSimulation)
EVT_UPDATE_UI_RANGE(SP_ID_CONTINUOUSPEDRESET, SP_ID_CONTINUOUSPEDSIMULPLACESELECTION, SP_DS_ContinuousPedAnimation::OnUpdateUI)
END_EVENT_TABLE()

SP_DS_ContinuousPedAnimation::SP_DS_ContinuousPedAnimation(unsigned int p_nRefresh,
                                                           unsigned int p_nDuration,
                                                           unsigned int p_nRounds,
                                                           double p_nRoundSize):
SP_DS_Animation(p_nRefresh, p_nDuration), m_pcParent(NULL),
m_pcDlg(NULL),m_pcGraph(NULL), m_sRefreshFrequ(wxT(""))
{


}

SP_DS_ContinuousPedAnimation::~SP_DS_ContinuousPedAnimation()
{


}

SP_DS_Animation* SP_DS_ContinuousPedAnimation::Clone()
{
	return dynamic_cast<SP_DS_Animation*>(new SP_DS_ContinuousPedAnimation(m_nRefreshFrequ,
			m_nStepDuration));
}

bool SP_DS_ContinuousPedAnimation::Initialise(SP_DS_Graph* p_pcGraph)
{
	SP_MESSAGEBOX(wxT("No animation for Continuous Petri Nets!"), wxT("Notice"));
	return false;
/*
	 m_pcGraph = p_pcGraph;

	 if( m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
	 {
		SP_CPN_SyntaxChecking l_cSyntaxChecking;
		if( !l_cSyntaxChecking.SyntaxChecking())
			return false;
	 }
	 else
	 {
		 SP_DS_CheckTransitions l_checkTrans(m_pcGraph);
		 if(!l_checkTrans.CheckRateFunction())
		 {
			 SP_MESSAGEBOX(wxT("Simulation can not initialize"),wxT("Continuous Simulation"));
			 return false; //there is error in one of the transitions rate functions
		 }
	 }

	 return true;
*/
}




bool SP_DS_ContinuousPedAnimation::AddToControl(SP_DLG_Animation* p_pcCtrl,
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
			wxT("Continuous Petri Net Simulation:")), 0, wxEXPAND | wxLEFT | wxTOP,
			5);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxButton(p_pcCtrl,
			SP_ID_CONTINUOUSPEDOPENSIMULATION,
			wxT("Open Simulation")), 1, wxLEFT | wxRIGHT | wxTOP, 5);
	p_pcSizer->Add(l_pcSizer, 1, wxEXPAND);

	p_pcCtrl->PushEventHandler(this);

	return TRUE;
}

void SP_DS_ContinuousPedAnimation::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable(!m_bRunning);
}



void SP_DS_ContinuousPedAnimation::OnOpenSimulation(wxCommandEvent& p_cEvent)
{
	OpenSimulation();
}

void SP_DS_ContinuousPedAnimation::OpenSimulation()
{
	//hide Animationdialog because of wxSTAY_ON_TOP
		m_pcParent->Show(false);

		wxString l_sNetClass = m_pcGraph->GetNetclass()->GetName();

		if( l_sNetClass == SP_DS_CONTINUOUSPED_CLASS ) 
		{
			SP_DLG_CPNSimulationResults* l_pcDlg = new SP_DLG_CPNSimulationResults( m_pcGraph, NULL );
			l_pcDlg->ShowModal();
			l_pcDlg->Destroy();		
		}
		if( l_sNetClass == SP_DS_COLCPN_CLASS )
		{
			SP_DS_ColPN_Unfolding* l_pcUnfoldedNet = new SP_DS_ColPN_Unfolding();

			if(! l_pcUnfoldedNet->Unfolding() )
			{
				return ;
			}

			SP_DLG_ColCPNSimulationResults * l_pcDlg = new SP_DLG_ColCPNSimulationResults( m_pcGraph,l_pcUnfoldedNet, NULL );
			l_pcDlg->ShowModal();
			l_pcDlg->Destroy();		
		}

		
		//show Animationdialog after Simulationresults close
		m_pcParent->Show(true);
}


