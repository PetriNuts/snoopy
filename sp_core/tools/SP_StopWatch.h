/*
 * SP_StopWatch.h
 *
 *  Created on: Jun 3, 2015
 *      Author: crohr
 */

#ifndef SP_CORE_TOOLS_SP_STOPWATCH_H_
#define SP_CORE_TOOLS_SP_STOPWATCH_H_

#include <wx/datetime.h>
#include <wx/stopwatch.h>

class SP_StopWatch
{
public:
	/**
	 * @brief The only constructor
	 *
	 * @param startMsg  message to output when the timer starts
	 * @param beforeMsg  message to output before the elapsed time
	 * @param afterMsg  message to output after the elapsed time
	 */
	SP_StopWatch(const wxString& startMsg,
				const wxString& beforeMsg, const wxString& afterMsg = {});

	/**
	 * @brief Destructor, outputs the elapsed time and
	 * messages to the destination stream
	 */
	~SP_StopWatch();

private:
	wxStopWatch sw_;
	const wxString startMsg_;
	const wxString beforeMsg_;
	const wxString afterMsg_;

};

#endif /* SP_CORE_TOOLS_SP_STOPWATCH_H_ */
