//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										adds graph elements of the given class
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_SETTERBYCLASS_H__)
#define __SP_IA_DP_SETTERBYCLASS_H__

#include "sp_gui/interaction/data_processors/SP_IA_DP_Setter.h"


class SP_IA_DP_SetterByClass : public SP_IA_DP_Setter {

private:
	//the elementclass of which to add the elements
	//these elements are not created but taken from a graph (source or target)
	SP_ElementClass *m_pcElemClass;
	//"true" means the added elements are taken from the target graph
	//"false" means from source graph, which is default
	bool m_bFromTarget;

public:
	SP_IA_DP_SetterByClass();
	SP_IA_DP_SetterByClass(SP_ElementClass *p_pcEC, bool p_bFromTarget = false, bool p_bReset = true);
	virtual ~SP_IA_DP_SetterByClass();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void SetParams(SP_ElementClass *p_pcEC, bool p_bFromTarget = false);

};

#endif // !defined(__SP_IA_DP_SETTERBYCLASS_H__)
