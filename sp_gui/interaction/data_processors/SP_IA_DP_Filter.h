//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/25 $
// Short Description: base class for filter processors
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_FILTER_H__)
#define __SP_IA_DP_FILTER_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"

class SP_IA_DP_Filter : public SP_IA_DataProcessor {

private:

protected:
	SP_IA_DP_Filter(SP_IA_STATE p_eSelection = SP_IA_NORMAL) 
		: SP_IA_DataProcessor(), m_eSelection(p_eSelection) { }
	
	//SP_IA_SELECTED: the filtered elements are just selected (SP_Data::m_eIAState = SP_IA_SELECTED)
	//SP_IA_NORMAL: the elements that don't match the filtering criteria are removed (no selection takes place)
	//SP_IA_MAP_SELECTED: the filtered elements are selected for mapping
	SP_IA_STATE m_eSelection;

public:
	virtual ~SP_IA_DP_Filter() { }
};


#endif // !defined(__SP_IA_DP_FILTER_H__)
