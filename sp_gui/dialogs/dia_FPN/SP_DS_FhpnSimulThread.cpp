#include"SP_DS_FhpnSimulThread.h"
SP_DS_FhpnSimulThread::SP_DS_FhpnSimulThread(wxWindow* p_pcParent, spsim::Simulator* p_pcSimulator, double alpha, vector<double> vsample, bool isAbort) : wxThread(wxTHREAD_JOINABLE),
m_pcParentWin(p_pcParent),
m_pcMainSimulator(p_pcSimulator),
m_dcurrentAlpha(alpha),
m_vdCurrentSample(vsample),
m_bIsAbort(isAbort)
{

}
SP_DS_FhpnSimulThread::~SP_DS_FhpnSimulThread()
{

}
void  SP_DS_FhpnSimulThread::SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue, void* p_pcUserData)
{
	SP_DS_ThreadEvent l_eGuiEvent(spEVT_THREAD, SP_SIMULATION_THREAD_EVENT);

	l_eGuiEvent.SetEventTask(p_nEventTask);
	l_eGuiEvent.SetUserData(p_pcUserData);

	l_eGuiEvent.SetLongValue(l_nLongValue);

	//add a pending event to the main gui thread
	m_pcParentWin->GetEventHandler()->AddPendingEvent(l_eGuiEvent);
}
void* SP_DS_FhpnSimulThread::Entry()
{

	m_pcMainSimulator->Initialise(true);
	m_pcMainSimulator->SetSimulationRunningState(true);
	//DoStartSimulation();
	spsim::HybridSimulator*		l_pcSimulator = dynamic_cast<spsim::HybridSimulator*> (m_pcMainSimulator);
	l_pcSimulator->Initialise(true);
	l_pcSimulator->SetSimulationRunningState(true);
	long l_nRunCount;
	long l_nThreadCount;
	spsim::Property* l_pcPropRunCounts = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("RunCount"));
	spsim::Property* l_pcPropThreadCount = l_pcSimulator->GetSimulatorOptions()->GetOption(wxT("ThreadCount"));
	spsim::Matrix2DDouble vvdResultMat;
	if (l_pcPropThreadCount != NULL)
	{
		l_nThreadCount = l_pcPropThreadCount->GetValuelong();
	}
	if (l_pcPropRunCounts != NULL)
	{
		l_nRunCount = l_pcPropRunCounts->GetValuelong();
	}


	if (l_nThreadCount == 1)
	{
		//single thread

		for (int iRun = 0; iRun < l_nRunCount; iRun++)
		{
			if (m_bIsAbort)
			{
				break;
			}
			l_pcSimulator->SimulateSingleRun();
			if (!l_pcSimulator->IsSimulationRunning()) {
				break;
			}
		}

		if (l_nRunCount > 1) {
			vvdResultMat = l_pcSimulator->GetResultMatrix();
			long	l_nColCount = l_pcSimulator->GetPlaceCount();
			for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++) {
				if (m_bIsAbort)
				{
					break;
				}
				for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
				{
					if (m_bIsAbort)
					{
						break;
					}
					vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
				}
			}
		}
		else {
			// no averaging when number of runs is no greater than one run
			vvdResultMat = l_pcSimulator->GetResultMatrix();
		}
	}
	else {

		//multithreading

		l_pcSimulator->RunSimulation();

		// do averaging
		vvdResultMat = l_pcSimulator->GetResultMatrix();
		long	l_nColCount = l_pcSimulator->GetPlaceCount();
		for (unsigned int l_nRow = 0; l_nRow < vvdResultMat.size(); l_nRow++)
		{
			if (m_bIsAbort)
			{
				break;
			}
			for (unsigned int l_nCol = 0; l_nCol < l_nColCount; l_nCol++)
			{
				if (m_bIsAbort)
				{
					break;
				}
				vvdResultMat[l_nRow][l_nCol] /= l_nRunCount;
			}
		}


	}


	l_pcSimulator->AbortSimulation();
	l_pcSimulator->Initialise(false);
	m_pcMainSimulator->Initialise(false);
	m_pcMainSimulator->SetSimulationRunningState(false);
	l_pcSimulator->SetSimulationRunningState(false);

	TraceElement* structTraceElement = new TraceElement();

	structTraceElement->sample = m_vdCurrentSample;
	structTraceElement->currentLevel = m_dcurrentAlpha;
	structTraceElement->fuzzyTrace = vvdResultMat;
	structTraceElement->levelNum = 0;
	if (m_bIsAbort)
	{
		SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION, 0, structTraceElement);
		return NULL;
	}
	SendGuiEvent(SP_THREAD_EVENT_END_ONE_RUN, 0, structTraceElement);
	return NULL;
}
