
#ifndef _SP_DS_FUZZYRESULT_THREAD_H_
#define _SP_DS_FUZZYRESULT_THREAD_H_


#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include "FuzzyReasoning.h"
#include "sp_ds/extensions/SP_DS_SimulatorThread.h"
using namespace fuzzification;
class SP_DS_ThreadEvent;
class SP_DS_FuzzyResult_Thread : public wxThread
{

public:
	SP_DS_FuzzyResult_Thread(wxWindow* m_parentWin, ResultFuzzyBand m_fuzzyBand, FuzzyReasoning* m_fuzzyReasoning);
	~SP_DS_FuzzyResult_Thread();
protected:
	void * Entry();
	virtual void  SendGuiEvent(const int& p_nEventTask, const long& l_nLongValue = 0, void* p_pcUserData = NULL);
private:
	wxWindow*        m_parentWin;
	ResultFuzzyBand m_fuzzyBand;
	fuzzification::FuzzyReasoning*  m_fuzzyReasoning;

};

#endif /* _SP_DS_FUZZYRESULT_THREAD_H_ */
