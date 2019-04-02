/* SP_DS_ThreadEvent.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.10.2011
 * Short Description:
 */
 //==============================================================================
#include <wx/event.h>


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(spEVT_THREAD, -1)
END_DECLARE_EVENT_TYPES()

static const int SP_SIMULATION_THREAD_EVENT=1100;

enum
{
	SP_THREAD_EVENT_END_SIMULATION,

	SP_THREAD_EVENT_END_ONE_RUN,
	SP_THREAD_EVENT_START_SIM
	//TODO: add here other event types which need to be dispatched
};

class SP_DS_ThreadEvent: public wxEvent
{
private: 
	     int m_nEventTask;
	     void* m_pcUserData;

	     long m_nLongValue;
protected:
	     wxEvent *Clone(void) const { return new SP_DS_ThreadEvent(*this); }
public:
	    SP_DS_ThreadEvent(wxEventType eventType = spEVT_THREAD,const int& p_nID=0);
		~SP_DS_ThreadEvent(void);

		void SetEventTask(const int& p_nTask){m_nEventTask=p_nTask;}
		int GetEventTask(){return m_nEventTask;}

		void SetUserData(void* p_pcUserData){m_pcUserData=p_pcUserData;}
		void* GetUserData(){return m_pcUserData; }

		//long value to use
		void SetLongValue(const unsigned long& l_pValue){m_nLongValue=l_pValue;}
		long GetLongValue(){return m_nLongValue;}
};

typedef void (wxEvtHandler::*spThreadEventFunction)(SP_DS_ThreadEvent&);

#define spThreadEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(spThreadEventFunction, &func)

#define EVT_SIMTHREAD(id, func) \
    wx__DECLARE_EVT1(spEVT_THREAD, id, spThreadEventHandler(func))


