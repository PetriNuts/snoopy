//////////////////////////////////////////////////////////////////////
// $Source: SP_IA_Event.h $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/29 $
// Short Description: class for interaction events
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_EVENT_H__)
#define __SP_IA_EVENT_H__

#include "sp_utilities.h"

#include "sp_core/base/SP_Data.h"
#include "sp_core/SP_List.h"

//trigger event types
//note: order is synchronized with m_lsTypeStrings[]
enum SP_IA_EVENT_T {
	SP_IA_EVT_GENERAL = 0,
	SP_IA_EVT_SINGLE,
	SP_IA_EVT_MAKRO,
	SP_IA_EVT_PEDANIMSTEP_FWD,
	SP_IA_EVT_PEDANIMSTEP_BWD,
	SP_IA_EVT_PEDANIMPRESTEP_FWD
};


class SP_IA_Event {

private:
	//the source elements which trigger an event
	SP_ListData *m_lSource;
	//true, if we have our own copy of source data and take care of their deletion, too
	bool m_bHoldsDataCopy;
	//the type of trigger (see above)
	SP_IA_EVENT_T m_eTrigger;

protected:

public:
	SP_IA_Event(SP_Data* source, SP_IA_EVENT_T trigger = SP_IA_EVT_SINGLE, bool p_bHoldsDataCopy = false);
	SP_IA_Event(SP_ListData*source, SP_IA_EVENT_T trigger = SP_IA_EVT_MAKRO, bool p_bHoldsDataCopy = false);
	virtual ~SP_IA_Event();

	//usual getters
	SP_IA_EVENT_T GetTriggerType() const { return m_eTrigger; }
	wxString GetTriggerTypeString() const;
	SP_ListData *GetSourceElements() const { return m_lSource; }
	SP_DS_Graph *GetSourceGraph();

	//inits the event with a source graph
	SP_IA_Event* InitializedCopy(SP_DS_Graph* p_pcGraph);

	//usual comparison function
	bool Equals(SP_IA_Event *p_pcEvent, bool p_bCheckGraph = true);
};

#endif // !defined(__SP_IA_EVENT_H__)
