// SP_IA_DP_MultiplyerByAttributeValue.cpp: Implementierung der Klasse SP_IA_DP_MultiplyerByAttributeValue.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_ConverterValue2Quantity.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_ConverterValue2Quantity::SP_IA_DP_ConverterValue2Quantity()
: SP_IA_DataProcessor(),
m_pcAttribute(NULL)
{ }

SP_IA_DP_ConverterValue2Quantity::SP_IA_DP_ConverterValue2Quantity(SP_DS_NumberAttribute *p_pcAttrib)
: SP_IA_DataProcessor(),
m_pcAttribute(p_pcAttrib)
{ }

SP_IA_DP_ConverterValue2Quantity::~SP_IA_DP_ConverterValue2Quantity()
{
	if (!IsCopy()) {
		if (m_pcAttribute) wxDELETE(m_pcAttribute);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_ConverterValue2Quantity::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_ConverterValue2Quantity(m_pcAttribute);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_ConverterValue2Quantity::DoProcessing(SP_ListData *p_pcData) {

	if (!p_pcData || !m_pcAttribute) return p_pcData;

	SP_ListData::iterator it;
	SP_ListData l_pcResult;
	int count;

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {

		//check if we should only process selected elements
		//if so, just transfer the unselected ones
		if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
			l_pcResult.push_back(*it);
			continue;
		}

		SP_DS_NumberAttribute *l_pcDataAttrib =
						dynamic_cast<SP_DS_NumberAttribute*>((*it)->GetAttribute(m_pcAttribute->GetName()));

		if (!l_pcDataAttrib) {
			//if the current element does not have the proper attribute the value is set to '0'
			count = 0;
		} else {
			//... otherwise we use the value of the element's attribute
			count = l_pcDataAttrib->GetValue();
		}
		if (count > 1) (*it)->AddAttribute(new SP_DS_NumberAttribute(wxT("#IA#MULTI#"), count));
		if (count > 0) {
			l_pcResult.push_back(*it);
		} else {
			ReleaseData(*it);
		}
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}
