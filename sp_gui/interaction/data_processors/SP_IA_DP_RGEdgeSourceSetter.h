//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/25 $
// Short Description: sets the current edge source while constructing rg
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_RGEDGESOURCESETTER_H__)
#define __SP_IA_DP_RGEDGESOURCESETTER_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"


class SP_IA_DP_RGEdgeSourceSetter : public SP_IA_DataProcessor {

private:
	SP_MapString2Int m_mPlaces2Marking;

public:
	SP_IA_DP_RGEdgeSourceSetter();
	virtual ~SP_IA_DP_RGEdgeSourceSetter();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

};

#endif // !defined(__SP_IA_DP_RGEDGESOURCESETTER_H__)
