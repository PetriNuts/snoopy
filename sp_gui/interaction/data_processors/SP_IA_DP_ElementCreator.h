//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										creates nodes and edges of the given classes
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_ELEMENTCREATOR_H__)
#define __SP_IA_DP_ELEMENTCREATOR_H__


#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"


class SP_IA_DP_ElementCreator : public SP_IA_DataProcessor {

private:
	//the nodeclass of which to create the new nodes
	SP_DS_Nodeclass *m_pcNodeclass;
	//the edgeclass of which to create the new edges
	SP_DS_Edgeclass *m_pcEdgeclass;

public:
	SP_IA_DP_ElementCreator();
	SP_IA_DP_ElementCreator(SP_DS_Nodeclass *p_pcNC, SP_DS_Edgeclass *p_pcEC = 0);
	virtual ~SP_IA_DP_ElementCreator();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void SetParams(SP_DS_Nodeclass *p_pcNC, SP_DS_Edgeclass *p_pcEC);

};

#endif // !defined(__SP_IA_DP_ELEMENTCREATOR_H__)
