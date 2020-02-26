#ifndef _SP_DS_FUZZY_MINMAX_H_
#define _SP_DS_FUZZY_MINMAX_H_
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include "FuzzyReasoning.h"
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
class SP_DS_Fuzzy_MinMaxThread : public wxThread
{
public:
	SP_DS_Fuzzy_MinMaxThread(SP_Vector2DDouble& p_vvFinalMax,SP_Vector2DDouble& p_vvFinalMin,long p_lDataCol,ResultFuzzyBand p_resultBand,long p_lStartTime,long p_lEndTime );
	~SP_DS_Fuzzy_MinMaxThread();
protected:
	void * Entry();
public:

private:
	SP_Vector2DDouble& m_vvFinalMax;
	SP_Vector2DDouble& m_vvFinalMin;
	long              m_lDataCol;
	ResultFuzzyBand   m_ResultBand;
	long              m_lStartTime;
	long              m_lEndTime;

};


#endif /* _SP_DS_FUZZY_MINMAX_H_*/