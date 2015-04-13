//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										filters elements by given element class(es) 
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_FILTERBYCLASS_H__)
#define __SP_IA_DP_FILTERBYCLASS_H__

#include "sp_gui/interaction/data_processors/SP_IA_DP_Filter.h"


class SP_IA_DP_FilterByClass : public SP_IA_DP_Filter {

private:
	//element classes to filter by
	list<SP_ElementClass*> *m_pcElemClasses;

public:
	SP_IA_DP_FilterByClass(SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	SP_IA_DP_FilterByClass(SP_ElementClass *p_pcEC, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	SP_IA_DP_FilterByClass(list<SP_ElementClass*> *p_pcEC, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	virtual ~SP_IA_DP_FilterByClass();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void AddParams(SP_ElementClass *p_pcEC);

	void SetParams(SP_ElementClass *p_pcEC);
	void SetParams(list<SP_ElementClass*> *p_pcEC);
};

#endif // !defined(__SP_IA_DP_FILTERBYELEMENTCLASS_H__)
