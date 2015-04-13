/*
 * SP_DS_SimulatorThread.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 21.10.2011
 * Short Description:
 */
 //==============================================================================

#ifndef SP_DS_SIMULATORTHREAD_H_
#define SP_DS_SIMULATORTHREAD_H_

#include "spsim/spsim.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/extensions/SP_DS_ThreadEvent.h"

/*
 * Base class for running the simulation inside separate thread.
 * This will free the GUI thread during running the simulation
 */
class SP_DS_SimulatorThread: public wxThread, public SP_Error
{
protected:

	      //pointer to the parent window, to communicate with
	      wxWindow* p_pcParentWnd;

	      spsim::Simulator* m_pcSimulator;

	      //number of simulation run to perform
	      unsigned int m_nRunCount;

	      unsigned int m_nCurrentRunCount;

	      wxStopWatch m_cSimulationStopWatch;

protected:
	      //send an event to the caller GUI thread
	      void SendGuiEvent(const int& p_nEventTask,const long& l_nLongValue=0,void* p_pcUserData=NULL);
public:
	      SP_DS_SimulatorThread(wxWindow* p_pcParent,spsim::Simulator* p_pcSimulator);
	      virtual ~SP_DS_SimulatorThread();

	      //get the current simulation run time
	      virtual long GetSimulationRuntime(){return m_cSimulationStopWatch.Time();}

	      //set the current simulator
	      virtual void SetSimulator( spsim::Simulator* p_pcSimulator){m_pcSimulator=p_pcSimulator;}

	      virtual void SetRunCount(const unsigned int& p_nRunCount){m_nRunCount=p_nRunCount;}

	      //get run count
	      virtual unsigned int GetRunCount(){return m_nRunCount;}

	      unsigned int GetCurrentRunCount(){return m_nCurrentRunCount;}
};

#endif /* SP_DS_SIMULATORTHREAD_H_ */
