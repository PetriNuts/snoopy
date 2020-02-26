#include "sp_gui/dialogs/dia_FPN/SP_DS_FcpnSimulThread.h"

SP_DS_FcpnSimulThread::SP_DS_FcpnSimulThread(wxWindow* p_pcParent, spsim::Simulator* p_pcSimulator,double alpha,vector<double> vsample,bool isAbort) : wxThread(wxTHREAD_JOINABLE),
m_pcParentWin(p_pcParent),
m_pcMainSimulator(p_pcSimulator),
m_currentAlpha(alpha),
m_currentSample(vsample),
m_bIsAbort(isAbort)
{

}
SP_DS_FcpnSimulThread::~SP_DS_FcpnSimulThread()
{

}

void* SP_DS_FcpnSimulThread::Entry()
{

	double l_nCurrentTime = 0;

	
	while (l_nCurrentTime <= m_pcMainSimulator->GetOutputEndPoint() && m_pcMainSimulator->IsSimulationRunning())
	{
		if (m_bIsAbort) {
			break;
	}
		l_nCurrentTime = m_pcMainSimulator->RunSimulationToNextOutputPoint();

		//UpdateSimulationDialog(m_pcMainSimulator->GetGeneratedResultPointsCount());//
	}
	
	spsim::Matrix2DDouble vvDresultMat;
	vvDresultMat = m_pcMainSimulator->GetResultMatrix();
	TraceElement* structTraceElement = new TraceElement();

	 structTraceElement->sample = m_currentSample;
	 structTraceElement->currentLevel = m_currentAlpha;
	structTraceElement->fuzzyTrace = vvDresultMat;
	//m_pcMainSimulator->AbortSimulation();
	if (m_bIsAbort)
	{ 
	SendGuiEvent1(SP_THREAD_EVENT_END_SIMULATION, 0, structTraceElement); 
	return NULL; 
	}
	SendGuiEvent1(SP_THREAD_EVENT_END_ONE_RUN, 0,structTraceElement);

	//structTraceElement.sample = m_vCurrentSample;
	//structTraceElement.currentLevel = m_fuzzyReasoning.GetAlphaSet()[ialpha];
	//structTraceElement.levelNum = ialpha;
	//structTraceElement.fuzzyTrace = vvDresultMat;
	//m_fuzzyBand.push_back(structTraceElement);
	return NULL;
}

void  SP_DS_FcpnSimulThread::SendGuiEvent1(const int& p_nEventTask, const long& l_nLongValue , void* p_pcUserData )
{
		SP_DS_ThreadEvent l_eGuiEvent(spEVT_THREAD, SP_SIMULATION_THREAD_EVENT);

		l_eGuiEvent.SetEventTask(p_nEventTask);
		l_eGuiEvent.SetUserData(p_pcUserData);

		l_eGuiEvent.SetLongValue(l_nLongValue);

		//add a pending event to the main gui thread
		m_pcParentWin->GetEventHandler()->AddPendingEvent(l_eGuiEvent);
		 
		 
		
	 
}
