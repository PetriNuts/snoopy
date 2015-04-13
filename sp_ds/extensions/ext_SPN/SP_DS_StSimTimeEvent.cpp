//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/12 09:35:00 $
// Short Description:
//////////////////////////////////////
#include "SP_DS_StSimTimeEvent.h"

SP_DS_StSimTimeEvent::SP_DS_StSimTimeEvent() :
	eventType(SP_ST_SIM_TIME_EVENT_DEFAULT),
	arrayPos( 0), fixedTime( 0), period( 0), intervalStart( 0), intervalEnd( 0)
{
}

SP_DS_StSimTimeEvent::SP_DS_StSimTimeEvent(const SP_DS_StSimTimeEvent& p_TimeEvent):
		eventType(p_TimeEvent.eventType),
		arrayPos(p_TimeEvent.arrayPos),
		fixedTime(p_TimeEvent.fixedTime),
		period(p_TimeEvent.period),
		intervalStart(p_TimeEvent.intervalStart),
		intervalEnd(p_TimeEvent.intervalEnd)
{
}

SP_DS_StSimTimeEvent::~SP_DS_StSimTimeEvent()
{
}
