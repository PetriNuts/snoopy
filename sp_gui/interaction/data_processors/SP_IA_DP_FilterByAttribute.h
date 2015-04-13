//////////////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: 1.0 $
// $Revision: $
// $Date: 2006/10/05 $
// Short Description: derived data processor
//										filters elements by given attribute(s) and operator(s)
/////////////////////////////////////////////////////////////////////////////

#if !defined(__SP_IA_DP_FILTERBYATTRIBUTE_H__)
#define __SP_IA_DP_FILTERBYATTRIBUTE_H__

#include "sp_gui/interaction/data_processors/SP_IA_DP_Filter.h"

class SP_DS_ListAttribute;

typedef pair<SP_IA_COMPARE_OP, SP_DS_Attribute*> SP_PairIACompareAttribute;

class SP_IA_DP_FilterByAttribute : public SP_IA_DP_Filter {

private:
	//list of filter attributes and operators that belong to them
	list<SP_PairIACompareAttribute*> *m_pcAttributes;
	//indicates whether to check attribute types, too
	bool m_bCheckType;

	//helper function for checking "CONTAIN"-operators (for list attributes)
	bool CheckContains(SP_DS_Attribute *first, SP_DS_Attribute *second);
	void TransferMultiplicity(SP_DS_ListAttribute *p_pcList, const wxString& p_sValue, SP_Data *p_pcTo);

public:
	//default constructor
	SP_IA_DP_FilterByAttribute(SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	//constructor for filtering only by attribute name and type, not by value (operator is set to SP_IA_NO_VALUE_COMPARE)
	SP_IA_DP_FilterByAttribute(SP_DS_Attribute *p_pcAttrib,
															bool p_bCheckType = true,	SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	//constructor for filtering by one attribute (including value)
	SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib,
															bool p_bCheckType = false, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	//constructors for filtering by a list of attributes, but all with the same compare operator
	SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, const SP_ListAttribute *p_pcAttribs,
															bool p_bCheckType = false, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, SP_ListData *p_pcAttribs,
															bool p_bCheckType = false, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	//constructor for filtering by several attributes with arbitrary compare operators
	SP_IA_DP_FilterByAttribute(list<SP_PairIACompareAttribute*> *p_pcAttribPairs,
															bool p_bCheckType = false, SP_IA_STATE p_eSelection = SP_IA_NORMAL);
	virtual ~SP_IA_DP_FilterByAttribute();

	//these implement the SP_IA_DataProcessor
	SP_IA_DataProcessor *Copy();
	SP_ListData *DoProcessing(SP_ListData *p_pcData);

	void AddParams(SP_DS_Attribute *p_pcAttrib);
	void AddParams(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib);

	void SetParams(bool p_bCheckType) { m_bCheckType = p_bCheckType; }
	void SetParams(SP_DS_Attribute *p_pcAttrib);
	void SetParams(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib);
	void SetParams(list<SP_PairIACompareAttribute*> *p_pcAttribPairs);
};

#endif // !defined(__SP_IA_DP_FILTERBYATTRIBUTE_H__)
