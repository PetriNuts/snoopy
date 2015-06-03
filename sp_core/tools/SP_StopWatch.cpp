/*
 * SP_StopWatch.cpp
 *
 *  Created on: Jun 3, 2015
 *      Author: crohr
 */

#include "sp_core/tools/SP_StopWatch.h"
#include "sp_utilities.h"

SP_StopWatch::SP_StopWatch(const wxString& startMsg, const wxString& beforeMsg, const wxString& afterMsg)
: startMsg_(startMsg), beforeMsg_(beforeMsg), afterMsg_(afterMsg)
{
	SP_LOGMESSAGE(startMsg_);
}

SP_StopWatch::~SP_StopWatch()
{
	auto interval = wxTimeSpan::Milliseconds(sw_.Time());
	SP_LOGMESSAGE(beforeMsg_ + wxT(" ") + interval.Format(wxT("%H:%M:%S.%l")) + wxT(" ") + afterMsg_);
}

