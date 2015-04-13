//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#ifndef __SP_DS_SP_ST_SIM_TIME_EVENT_H__
#define __SP_DS_SP_ST_SIM_TIME_EVENT_H__

#include "sp_utilities.h"

typedef enum {

	SP_ST_SIM_TIME_EVENT_DEFAULT,

	SP_ST_SIM_TIME_EVENT_DET_FIRING_SINGLE,
	SP_ST_SIM_TIME_EVENT_DET_FIRING_PERIODIC,

	SP_ST_SIM_TIME_EVENT_TIMED_FIRING_SINGLE

} SP_DS_StSimTimeEventType;

class SP_DS_StSimTimeEvent {

  public :

    SP_DS_StSimTimeEvent();
    SP_DS_StSimTimeEvent(const SP_DS_StSimTimeEvent& p_TimeEvent);

    virtual ~SP_DS_StSimTimeEvent();

    SP_DS_StSimTimeEventType eventType;

    long arrayPos;

    double fixedTime;
    double period;
    double intervalStart;
    double intervalEnd;

};


#endif
