//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/25 09:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "SP_DS_StSimTimeEventTimeline.h"
#include <math.h>
#include "sp_core/tools/MersenneTwister.h"

SP_DS_StSimTimeEventTimeline::SP_DS_StSimTimeEventTimeline()
{
	m_pcMTRand = new MTRand(rand());
}

void
SP_DS_StSimTimeEventTimeline::InsertTimeEvent(SP_DS_StSimTimeEvent* p_pcTimeEvent, double p_nInsertTime)
{
	bool l_bInsert = true;
	SP_MultimapSimTimeEvent::iterator l_it = m_mTimeLine.begin();

	while(l_it != m_mTimeLine.end())
	{
		SP_DS_StSimTimeEvent* l_TimeEvent = l_it->second;

		if(l_TimeEvent == p_pcTimeEvent)
		{
			l_bInsert = false;
			break;
		}
		++l_it;
	}
	if(l_bInsert)
	{
		m_mTimeLine.insert(make_pair(p_nInsertTime, p_pcTimeEvent));
	}
}

void SP_DS_StSimTimeEventTimeline::Clear()
{
	m_mTimeLine.clear();
}

void SP_DS_StSimTimeEventTimeline::RemoveElem(SP_DS_StSimTimeEvent* p_pcElem)
{
	SP_MultimapSimTimeEvent::iterator l_it = m_mTimeLine.begin();

	while(l_it != m_mTimeLine.end())
	{
		SP_DS_StSimTimeEvent* l_TimeEvent = l_it->second;

		if(l_TimeEvent == p_pcElem)
		{
			m_mTimeLine.erase(l_it);
			l_it = m_mTimeLine.begin();
		}
		else
		{
			++l_it;
		}
	}
}

SP_MultimapSimTimeEvent
SP_DS_StSimTimeEventTimeline::GetTimeEvent(double p_nTimePoint)
{
	SP_MultimapSimTimeEvent::iterator l_it = m_mTimeLine.begin();
	SP_MultimapSimTimeEvent l_Return;
	double l_nTimePoint = -1.0;

	while(l_it != m_mTimeLine.end())
	{
		if(l_it->first <= p_nTimePoint)
		{
			l_nTimePoint = l_it->first;
			break;
		}
		++l_it;
	}

	if(l_nTimePoint >= 0)
	{
		pair<SP_MultimapSimTimeEvent::iterator, SP_MultimapSimTimeEvent::iterator> l_Range =
				m_mTimeLine.equal_range(l_nTimePoint);
		for(l_it = l_Range.first; l_it != l_Range.second; ++l_it)
		{
			l_Return.insert(make_pair(l_nTimePoint, l_it->second));
		}
	}

	return l_Return;
}

double
SP_DS_StSimTimeEventTimeline::GetTimeEventTime(SP_DS_StSimTimeEvent* p_pcTimeEvent)
{
	double l_nReturn = -1;
	SP_MultimapSimTimeEvent::iterator l_it = m_mTimeLine.begin();

	while(l_it != m_mTimeLine.end())
	{
		SP_DS_StSimTimeEvent* l_TimeEvent = l_it->second;

		if(l_TimeEvent == p_pcTimeEvent)
		{
			l_nReturn = l_it->first;
			break;
		}
		++l_it;
	}
	return l_nReturn;
}

SP_DS_StSimTimeEventTimeline::~SP_DS_StSimTimeEventTimeline()
{
	delete m_pcMTRand;
	Clear();
}
