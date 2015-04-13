//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/25 09:35:00 $
// Short Description:
//////////////////////////////////////

#ifndef __SP_DS_STSIMTIMEEVENTTimeline_H__
#define __SP_DS_STSIMTIMEEVENTTimeline_H__

#include "sp_ds/extensions/ext_SPN/SP_DS_StSimTimeEvent.h"

class MTRand;
typedef multimap<double, SP_DS_StSimTimeEvent*> SP_MultimapSimTimeEvent;

class SP_DS_StSimTimeEventTimeline
{

public:

	SP_DS_StSimTimeEventTimeline();
	virtual ~SP_DS_StSimTimeEventTimeline();

	void InsertTimeEvent(SP_DS_StSimTimeEvent* p_pcTimeEvent, double p_nInsertTime);

	void RemoveElem(SP_DS_StSimTimeEvent* p_pcElem);

	void Clear();

	SP_MultimapSimTimeEvent GetTimeEvent(double p_nTimePoint);
	double GetTimeEventTime(SP_DS_StSimTimeEvent* p_pcTimeEvent);

protected:

private:
	MTRand* m_pcMTRand;
	SP_MultimapSimTimeEvent m_mTimeLine;
};

#endif
