/*
 * SP_DS_SimulatorThreadHybrid.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 21.10.2011
 * Short Description:
 */
 //==============================================================================
#include "sp_ds/extensions/HybridPN/SP_DS_SimulatorThreadHybrid.h"
#include "sp_utilities.h"


SP_DS_SimulatorThreadHybrid::SP_DS_SimulatorThreadHybrid(wxWindow* p_pcParent,spsim::Simulator* p_pcSimulator)
:SP_DS_SimulatorThread(p_pcParent,p_pcSimulator)
{

}

SP_DS_SimulatorThreadHybrid::~SP_DS_SimulatorThreadHybrid()
{

}
void* SP_DS_SimulatorThreadHybrid::Entry()
{
	spsim::StochasticSimulator* l_pcSimulator;

	CHECK_POINTER(m_pcSimulator, return NULL);

	//transform this simulator into hybrid simulator
	l_pcSimulator=dynamic_cast<spsim::StochasticSimulator*>(m_pcSimulator);

	//start recording the runtime
	m_cSimulationStopWatch.Start(0);

	m_nCurrentRunCount=0;

	l_pcSimulator->SetSimulationRunningState(true);

	unsigned int l_nThreadCount=1;

	unsigned int l_nRunCount=1;

	if(m_pcSimulator->GetSimulatorOptions()->GetOption("ThreadCount")!=NULL)
	{
	 l_nThreadCount=m_pcSimulator->GetSimulatorOptions()->GetOption("ThreadCount")->GetValuelong();
	}

	//number of runs
	if(m_pcSimulator->GetSimulatorOptions()->GetOption("RunCount")!=NULL)
	{
	 l_nRunCount=m_pcSimulator->GetSimulatorOptions()->GetOption("RunCount")->GetValuelong();
	}

	if(l_nThreadCount>1 && l_nRunCount>1)
	{
	 m_pcSimulator->RunSimulation();
	}
	else
	{
	 //run the simulation
	  while(m_nCurrentRunCount<m_nRunCount && m_pcSimulator->IsSimulationRunning())
	  {
		 l_pcSimulator->SimulateSingleRun();


		 m_nCurrentRunCount++;

		 //tell the GUI that we finished one run
		 SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN,m_nCurrentRunCount,NULL);
	  }

	}

	//stop recording the runtime
	m_cSimulationStopWatch.Pause();

	//abort simulator
	l_pcSimulator->AbortSimulation();

	m_nCurrentRunCount=m_nRunCount;

	long l_nRunTime=GetSimulationRuntime();

	SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION,l_nRunTime,NULL);

	return NULL;
}
