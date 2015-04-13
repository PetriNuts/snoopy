//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										maps between source and target using attribute(s) 
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_MAPPERBYATTRIBUTE_H__)
#define __SP_IA_DP_MAPPERBYATTRIBUTE_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"
#include "sp_gui/interaction/commands/SP_IA_Command.h"


class SP_IA_DP_MapperByAttribute : public SP_IA_DataProcessor {

private:
	//attributes for mapping
	//using a AttributeTranslator and a FilterByAttribute
	SP_DS_Attribute *m_pcFromAttrib;
	SP_DS_Attribute *m_pcToAttrib;
	SP_IA_COMPARE_OP m_eMapOp;
	//true: maps only to selected (IA-State == SP_IA_MAP_SELECTED) data elements
	bool m_bMap2Selected;

public:
	SP_IA_DP_MapperByAttribute();
	SP_IA_DP_MapperByAttribute(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom, bool p_bMap2Selected = false);
	virtual ~SP_IA_DP_MapperByAttribute();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void SetParams(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom);
	void SetParams(bool p_bMap2Selected) { m_bMap2Selected = p_bMap2Selected; }
};


#endif // !defined(__SP_IA_DP_MAPPERBYATTRIBUTE_H__)
