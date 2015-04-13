// SP_IA_DP_ConverterQuantity2Value.cpp: Implementierung der Klasse SP_IA_DP_ConverterQuantity2Value.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_ConverterQuantity2Value.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_ConverterQuantity2Value::SP_IA_DP_ConverterQuantity2Value()
: SP_IA_DataProcessor(),
m_pcAttribute(NULL)
{ }

SP_IA_DP_ConverterQuantity2Value::SP_IA_DP_ConverterQuantity2Value(SP_DS_NumberAttribute *p_pcAttrib)
: SP_IA_DataProcessor(),
m_pcAttribute(p_pcAttrib)
{ }

SP_IA_DP_ConverterQuantity2Value::~SP_IA_DP_ConverterQuantity2Value()
{
	if (!IsCopy()) {
		if (m_pcAttribute) wxDELETE(m_pcAttribute);
	}
}

SP_IA_DataProcessor*
SP_IA_DP_ConverterQuantity2Value::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_ConverterQuantity2Value(m_pcAttribute);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_ConverterQuantity2Value::DoProcessing(SP_ListData *p_pcData) {

	if (!p_pcData || !m_pcAttribute) return p_pcData;

	SP_ListData::iterator it;
	SP_ListData l_pcResult;
	SP_DS_NumberAttribute *l_pcAttrib;
	long multi;
	wxString l_sKey = wxString(wxT("#IA#VALUE#")) + m_pcAttribute->GetName();

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		//first check if we should only process selected elements
		//if so, just transfer the unselected ones
		if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
			l_pcResult.push_back(*it);
			continue;
		}

		//check for any multiplicity
		l_pcAttrib = dynamic_cast<SP_DS_NumberAttribute*>((*it)->GetAttribute(wxT("#IA#MULTI#")));
		if (l_pcAttrib)
		{
			multi = l_pcAttrib->GetValue();
		} else multi = 1;

		if (SP_Find(l_pcResult,*it) != l_pcResult.end()) {
			//if this element already exists, we increment the existing value by the found multiplicity...
			l_pcAttrib = dynamic_cast<SP_DS_NumberAttribute*>((*it)->GetAttribute(l_sKey));
			l_pcAttrib->SetValue(l_pcAttrib->GetValue() + multi);
		} else {
			//...if it doesn't, we take the found multiplicity as starting value and append it to the result
			(*it)->AddAttribute(new SP_DS_NumberAttribute(l_sKey, multi));
			l_pcResult.push_back(*it);
		}
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}

