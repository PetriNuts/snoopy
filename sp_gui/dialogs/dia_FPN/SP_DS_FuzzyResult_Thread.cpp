#include "SP_DS_FuzzyResult_Thread.h"
#include <wx/busyinfo.h>


SP_DS_FuzzyResult_Thread::SP_DS_FuzzyResult_Thread(wxWindow* p_parentWin, ResultFuzzyBand p_fuzzyBand, FuzzyReasoning* p_fuzzyReasoning) :wxThread(wxTHREAD_JOINABLE),
m_parentWin(p_parentWin),
m_fuzzyBand(p_fuzzyBand),
m_fuzzyReasoning(p_fuzzyReasoning)
{

}
SP_DS_FuzzyResult_Thread::~SP_DS_FuzzyResult_Thread()
{}

void* SP_DS_FuzzyResult_Thread::Entry()
{
	//wxBusyInfo info(wxT("Finalizing the processing, please wait."), m_parentWin);
	SP_Compressed_Fuzzy_Band* m_pcCompressedBand = m_fuzzyReasoning->CompressResults(m_fuzzyBand);
	if(m_fuzzyReasoning->GetWorkingStatus()==true)
	{
		SendGuiEvent(10, 0, NULL);
	}
	else
	{
		SendGuiEvent(SP_THREAD_EVENT_END_SIMULATION, 0, m_pcCompressedBand);
	}

	return NULL;
}
void  SP_DS_FuzzyResult_Thread::SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue, void* p_pcUserData)
{
	SP_DS_ThreadEvent l_eGuiEvent(spEVT_THREAD, SP_SIMULATION_THREAD_EVENT);

	l_eGuiEvent.SetEventTask(p_nEventTask);
	l_eGuiEvent.SetUserData(p_pcUserData);

	l_eGuiEvent.SetLongValue(l_nLongValue);

	//add a pending event to the main gui thread
	m_parentWin->GetEventHandler()->AddPendingEvent(l_eGuiEvent);




}
