/*
 * SP_DS_SimulatorThread.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.10.2011
 * Short Description:
 */
//==============================================================================
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"

SP_DS_SimulatorThread::SP_DS_SimulatorThread(wxWindow* p_pcParent, spsim::Simulator* p_pcSimulator) :
	wxThread(wxTHREAD_JOINABLE), p_pcParentWnd(p_pcParent), m_pcSimulator(p_pcSimulator), m_nRunCount(1), m_nCurrentRunCount(0)
{

}

SP_DS_SimulatorThread::~SP_DS_SimulatorThread()
{

}

void SP_DS_SimulatorThread::SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue, void* p_pcUserData)
{
	SP_DS_ThreadEvent l_eGuiEvent(spEVT_THREAD, SP_SIMULATION_THREAD_EVENT);

	l_eGuiEvent.SetEventTask(p_nEventTask);
	l_eGuiEvent.SetUserData(p_pcUserData);

	l_eGuiEvent.SetLongValue(l_nLongValue);

	//add a pending event to the main gui thread
	p_pcParentWnd->GetEventHandler()->AddPendingEvent(l_eGuiEvent);
}
