//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										"translates" list attributes to single attributes and vice versa
//										usually used for mapping between graphs within other processors
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_ATTRIBUTETRANSLATOR_H__)
#define __SP_IA_DP_ATTRIBUTETRANSLATOR_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"
#include "sp_gui/interaction/commands/SP_IA_Command.h"


class SP_IA_DP_AttributeTranslator : public SP_IA_DataProcessor {

private:
	//source attribute 
	SP_DS_Attribute *m_pcFromAttrib;
	//target attribute
	SP_DS_Attribute *m_pcToAttrib;
	//no translation needed if this is some "CONTAIN"-operator
	//which means we just transfer values
	SP_IA_COMPARE_OP m_eMapOp;

	//transfers attributes from data parent (source) 
	//to target attribute and later possibly back to data parent (target)
	void TransferAttachedAttributes(SP_Data *p_pcSource, SP_Data *p_pcTarget);

public:
	SP_IA_DP_AttributeTranslator();
	SP_IA_DP_AttributeTranslator(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom);
	virtual ~SP_IA_DP_AttributeTranslator();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	//usual setter
	void SetParams(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom);
};

#endif // !defined(__SP_IA_DP_ATTRIBUTETRANSLATOR_H__)
