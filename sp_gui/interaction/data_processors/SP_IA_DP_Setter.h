//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/25 $
// Short Description: base class for setter-processors
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_SETTER_H__)
#define __SP_IA_DP_SETTER_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"

class SP_IA_DP_Setter : public SP_IA_DataProcessor {

private:

protected:
	SP_IA_DP_Setter(bool p_bReset = true) 
		: SP_IA_DataProcessor(), m_bReset(p_bReset) { }
	
	//true if the processed elements are to be discarded
	bool m_bReset;

public:
	virtual ~SP_IA_DP_Setter() { }
};


#endif // !defined(__SP_IA_DP_SETTER_H__)
