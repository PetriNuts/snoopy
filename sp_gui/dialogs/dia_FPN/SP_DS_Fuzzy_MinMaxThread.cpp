#include "SP_DS_Fuzzy_MinMaxThread.h"

SP_DS_Fuzzy_MinMaxThread::SP_DS_Fuzzy_MinMaxThread(SP_Vector2DDouble& p_vvFinalMax, SP_Vector2DDouble& p_vvFinalMin, long p_lDataCol, ResultFuzzyBand p_resultBand, long p_lStartTime, long p_lEndTime) : wxThread(wxTHREAD_JOINABLE),
m_vvFinalMax(p_vvFinalMax),
m_vvFinalMin(p_vvFinalMin),
m_lDataCol(p_lDataCol),
m_ResultBand(p_resultBand),
m_lStartTime(p_lStartTime),
m_lEndTime(p_lEndTime) {

}

SP_DS_Fuzzy_MinMaxThread::~SP_DS_Fuzzy_MinMaxThread()
{}


void * SP_DS_Fuzzy_MinMaxThread::Entry()
{
	long lastTime = m_ResultBand[0].fuzzyTrace.size();
	for (long pos = m_lStartTime; pos < m_lEndTime; pos++)
	{
		 
		double minVal = m_ResultBand[0].fuzzyTrace[pos][m_lDataCol];
		double maxVal = m_ResultBand[0].fuzzyTrace[pos][m_lDataCol];
		for (unsigned long tracePos = 0; tracePos < m_ResultBand.size(); tracePos++)
		{
			 
			SP_Vector2DDouble elementTrace = m_ResultBand[tracePos].fuzzyTrace;
			double dataValue = elementTrace[pos][m_lDataCol];
			if (dataValue < minVal)
			{
				minVal = dataValue;

			}
			(dataValue > maxVal) ? maxVal = dataValue : maxVal = maxVal;

		}
		m_vvFinalMin[m_lDataCol][pos] = minVal;
		m_vvFinalMax[m_lDataCol][pos] = maxVal;
	}
	 
	return NULL;
}