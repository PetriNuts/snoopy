#ifndef _SP_DS_FCPNSIMTHREAD_H_
#define _SP_DS_FCPNSIMTHREAD_H_
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include"sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include "FuzzyReasoning.h"
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
//#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
//#include "sp_ds/extensions/SP_DS_ThreadEvent.h"
class SP_DS_ThreadEvent;
//class SP_DS_SimulatorThread;
class SP_DS_FcpnSimulThread : public wxThread
{
public:
	SP_DS_FcpnSimulThread(wxWindow* p_pcParent, spsim::Simulator* p_pcSimulator,double alpha,std::vector<double> m_currentSample, bool m_bIsAbort=false);
	~SP_DS_FcpnSimulThread();
protected:
	void * Entry();
public:


	virtual void  SendGuiEvent1(const int& p_nEventTask, const long& l_nLongValue = 0, void* p_pcUserData = NULL);
	virtual void OnSimulatorThreadEvent(SP_DS_ThreadEvent& event) {}
	virtual void SetAbort(bool b = true) { m_bIsAbort = b; }
  
 
private:
	wxWindow*               m_pcParentWin;
	spsim::Simulator*       m_pcMainSimulator;
	std::vector<double>     m_currentSample;
	double                  m_currentAlpha;
	bool                    m_bIsAbort;
	 
};


#endif /* _SP_DS_FCPNSIMTHREAD_H_ */