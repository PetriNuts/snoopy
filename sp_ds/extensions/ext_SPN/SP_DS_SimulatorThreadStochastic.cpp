/*
 * SP_DS_SimulatorThreadStochastic.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 16.11.2011
 * Short Description:
 */
//==============================================================================

#include "sp_ds/extensions/ext_SPN/SP_DS_SimulatorThreadStochastic.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"
#include "sp_utilities.h"


SP_DS_SimulatorThreadStochastic::SP_DS_SimulatorThreadStochastic(wxWindow* p_pcParent, spsim::Simulator* p_pcSimulator) :
	SP_DS_SimulatorThread(p_pcParent, p_pcSimulator)
{

}

SP_DS_SimulatorThreadStochastic::~SP_DS_SimulatorThreadStochastic()
{

}
void* SP_DS_SimulatorThreadStochastic::Entry()
{
	spsim::StochasticSimulator* l_pcSimulator;

	CHECK_POINTER(m_pcSimulator, return NULL);

	//transform this simulator into hybrid simulator
	l_pcSimulator = dynamic_cast<spsim::StochasticSimulator*> (m_pcSimulator);
	//SendGuiEvent(SP_THREAD_EVENT_START_SIM, 0, NULL);

	//start recording the runtime
	m_cSimulationStopWatch.Start(0);

	m_nCurrentRunCount = 0;

	l_pcSimulator->SetSimulationRunningState(true);

	unsigned int l_nThreadCount = 1;

	if (m_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount")) != NULL)
	{
		l_nThreadCount = m_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount"))->GetValuelong();
	}

	if (l_nThreadCount > 1)
	{
		m_pcSimulator->RunSimulation();
	}
	else
	{
		//run the simulation
		while (m_nCurrentRunCount < m_nRunCount && m_pcSimulator->IsSimulationRunning())
		{
			l_pcSimulator->SimulateSingleRun();

			if (p_pcParentWnd == NULL)
			{
				return NULL;
			}

			//compute the auxiliaries variables for colored Net
			if (dynamic_cast<SP_DLG_Simulation*> (p_pcParentWnd)->GetComAuxVarSingleRun())
			{
				dynamic_cast<SP_DLG_StSimulationResults*> (p_pcParentWnd)->ComputeAverageAuxiliaryVars(m_nCurrentRunCount, m_nRunCount);
			}

			//single run export
			dynamic_cast<SP_DLG_StSimulationResults*> (p_pcParentWnd)->DoSingleExport(m_nCurrentRunCount);

			//tell the GUI that we finished one run
			SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, m_nCurrentRunCount, NULL);

			m_nCurrentRunCount++;
		}

	}

	//stop recording the runtime
	m_cSimulationStopWatch.Pause();

	//abort simulator
	l_pcSimulator->AbortSimulation();

	m_nCurrentRunCount = m_nRunCount;

	long l_nRunTime = GetSimulationRuntime();

	SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION, l_nRunTime, NULL);
	
	return NULL;
}

