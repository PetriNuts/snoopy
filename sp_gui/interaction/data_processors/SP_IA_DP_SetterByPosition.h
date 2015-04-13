//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										sets elements by given position 
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_SETTERBYPOSITION_H__)
#define __SP_IA_DP_SETTERBYPOSITION_H__

#include "sp_gui/interaction/data_processors/SP_IA_DP_Setter.h"


enum SP_IA_POSITION_T {
	SP_IA_ID = 0, 	//just a default value where nothing is done
	SP_IA_NEXT,
	SP_IA_PREV,
	//TODO?: may be useful to be able to filter for the next equal element class
	//SP_IA_NEXT_EQUAL,
	//SP_IA_PREV_EQUAL
};


class SP_IA_DP_SetterByPosition : public SP_IA_DP_Setter {

private:
	SP_IA_POSITION_T m_ePos;

public:
	SP_IA_DP_SetterByPosition(SP_IA_POSITION_T p_ePos = SP_IA_ID, bool p_bReset = true);
	virtual ~SP_IA_DP_SetterByPosition();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void SetParams(SP_IA_POSITION_T p_ePos = SP_IA_ID) { m_ePos = p_ePos; }
};

#endif // !defined(__SP_IA_DP_SETTERBYPOSITION_H__)
