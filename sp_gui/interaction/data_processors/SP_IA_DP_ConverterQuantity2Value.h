//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										"translates" multiplicity of elements to an attribute value
/////////////////////////////////////////////////////////////////////////////

#if !defined __SP_IA_DP_CONVERTERQUANTITY2VALUE_H__
#define __SP_IA_DP_CONVERTERQUANTITY2VALUE_H__

#include "sp_gui/interaction/data_processors/SP_IA_DataProcessor.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"


class SP_IA_DP_ConverterQuantity2Value : public SP_IA_DataProcessor {

protected:
	//attribute to set the value to
	SP_DS_NumberAttribute *m_pcAttribute;

public:
	SP_IA_DP_ConverterQuantity2Value();
	SP_IA_DP_ConverterQuantity2Value(SP_DS_NumberAttribute *p_pcAttrib);
	virtual ~SP_IA_DP_ConverterQuantity2Value();

	//these implement the SP_IA_DataProcessor interface
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

};

#endif // !defined(__SP_IA_DP_CONVERTERQUANTITY2VALUE_H__
