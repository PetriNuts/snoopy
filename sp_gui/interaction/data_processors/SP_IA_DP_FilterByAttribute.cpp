// SP_IA_DP_FilterByAttribute.cpp: Implementierung der Klasse SP_IA_DP_FilterByAttribute.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_FilterByAttribute.h"
//needed in CheckContains(...)
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(NULL),
m_bCheckType(false)
{ }

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(SP_DS_Attribute *p_pcAttrib,
																											 bool p_bCheckType, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(NULL),
m_bCheckType(p_bCheckType)
{
	AddParams(p_pcAttrib);
}

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib,
																											 bool p_bCheckType, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(NULL),
m_bCheckType(p_bCheckType)
{
	AddParams(p_eOp, p_pcAttrib);
}

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, const SP_ListAttribute *p_pcAttribs,
																											 bool p_bCheckType, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(NULL),
m_bCheckType(p_bCheckType)
{
	for (SP_ListAttribute::const_iterator it = p_pcAttribs->begin(); it != p_pcAttribs->end(); ++it) {
		AddParams(p_eOp, *it);
	}
	wxDELETE(p_pcAttribs);
}

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(SP_IA_COMPARE_OP p_eOp, SP_ListData *p_pcAttribs,
																											 bool p_bCheckType, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(NULL),
m_bCheckType(p_bCheckType)
{
	for (SP_ListData::iterator it = p_pcAttribs->begin(); it != p_pcAttribs->end(); ++it) {
		SP_Data* l_pcData = *it;
		AddParams(p_eOp, dynamic_cast<SP_DS_Attribute*>(l_pcData));
	}
	wxDELETE(p_pcAttribs);
}

SP_IA_DP_FilterByAttribute::SP_IA_DP_FilterByAttribute(list<SP_PairIACompareAttribute*> *p_pcAttribPairs,
																											 bool p_bCheckType, SP_IA_STATE p_eSelection)
: SP_IA_DP_Filter(p_eSelection),
m_pcAttributes(p_pcAttribPairs),
m_bCheckType(p_bCheckType)
{ }

SP_IA_DP_FilterByAttribute::~SP_IA_DP_FilterByAttribute()
{
	if (!IsCopy() && m_pcAttributes) {
		list<SP_PairIACompareAttribute*>::iterator it;
		SP_PairIACompareAttribute *l_pcPair;
		for (it = m_pcAttributes->begin(); it != m_pcAttributes->end(); ++it) {
			l_pcPair = *it;
			wxDELETE((l_pcPair->second));
			wxDELETE(l_pcPair);
		}
		wxDELETE(m_pcAttributes);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_FilterByAttribute::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_FilterByAttribute(m_pcAttributes, m_bCheckType, m_eSelection);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}

SP_ListData*
SP_IA_DP_FilterByAttribute::DoProcessing(SP_ListData *p_pcData) {
	if (!p_pcData) return 0;

	SP_ListData l_pcResult;
	SP_ListData::iterator it;
	const SP_ListAttribute *l_pcDataAttrList;
	SP_ListAttribute::const_iterator itDAL;
	list<SP_PairIACompareAttribute*>::iterator itAL;

	bool l_bAddData, l_bAttribEquals;
	SP_DS_Attribute *l_pcFirstAttrib, *l_pcSecondAttrib;
	SP_IA_COMPARE_OP l_eOp;

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		//check if we should only process selected elements
		//if so, just transfer the unselected ones
		if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
			l_pcResult.push_back(*it);
			continue;
		}
		//no filtering criterea means no filtering,
		//so everything passes and we can go on to the next
		if (!m_pcAttributes) {
			l_pcResult.push_back(*it);
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) (*it)->SetIAState(m_eSelection);
			continue;
		}

		l_bAddData = false;
		l_pcDataAttrList = (*it)->GetAttributes();
		//iterating attributes of data element
		for (itDAL = l_pcDataAttrList->begin(); itDAL != l_pcDataAttrList->end(); ++itDAL) {
			l_pcFirstAttrib = (*itDAL);
			for (itAL = m_pcAttributes->begin(); itAL != m_pcAttributes->end(); ++itAL) {
				l_eOp = (*itAL)->first;
				l_pcSecondAttrib = (*itAL)->second;
				wxString s = l_pcSecondAttrib->GetValueString();
				//first checking attribute itself (name [and type])...
				l_bAttribEquals = (*l_pcFirstAttrib).Equals(l_pcSecondAttrib, false, true, m_bCheckType);

				//...now checking values (if required)
				if (l_bAttribEquals && l_eOp != SP_IA_NO_VALUE_COMPARE) {
					switch (l_eOp) {
						case SP_IA_EQUALS:
							//checks if attribute values are equal
							l_bAttribEquals &= (*l_pcFirstAttrib).Equals(l_pcSecondAttrib, true, false, false);
							break;
						case SP_IA_EQUALS_NOT:
							l_bAttribEquals &= !(*l_pcFirstAttrib).Equals(l_pcSecondAttrib, true, false, false);
							break;
						case SP_IA_IS_GREATER_THAN:
						case SP_IA_IS_LESS_THAN: {
							//checks if the first attribute value is greater (less) than the second
							//note: this wxT("works") (because of lexicographic comparison) also for non-number attribute types
							wxString	l_sValue1, l_sValue2;
							if (l_eOp == SP_IA_IS_GREATER_THAN) {
								l_sValue1 = (*l_pcFirstAttrib).GetValueString();
								l_sValue2 = (*l_pcSecondAttrib).GetValueString();
							} else {
								l_sValue1 = (*l_pcSecondAttrib).GetValueString();
								l_sValue2 = (*l_pcFirstAttrib).GetValueString();
							}
							double l_nDouble1, l_nDouble2;
							if (l_sValue1.ToDouble(&l_nDouble1) && l_sValue2.ToDouble(&l_nDouble2)) {
								l_bAttribEquals &= (l_nDouble1 > l_nDouble2);
							} else {
								l_bAttribEquals &= (l_sValue1.Cmp(l_sValue2) > 0);
							}
							break;
						}
						case SP_IA_CONTAINS:
						case SP_IA_CONTAINS_CMPL:
							//checks if a list (first attribute) contains a single value or (completely) contains another list
							l_bAttribEquals &= CheckContains(l_pcFirstAttrib, l_pcSecondAttrib);
							break;
						case SP_IA_CONTAINS_NOT:
							l_bAttribEquals &= !CheckContains(l_pcFirstAttrib, l_pcSecondAttrib);
							break;
						case SP_IA_IS_CONTAINED:
						case SP_IA_IS_CONTAINED_CMPL:
							//checks if the first attribute value is contained in the second
							l_bAttribEquals &= CheckContains(l_pcSecondAttrib, l_pcFirstAttrib);
							if (l_bAttribEquals) TransferMultiplicity(dynamic_cast<SP_DS_ListAttribute*>(l_pcSecondAttrib),
																													l_pcFirstAttrib->GetValueString(), *it);
							break;
						case SP_IA_IS_CONTAINED_NOT:
							l_bAttribEquals &= !CheckContains(l_pcSecondAttrib, l_pcFirstAttrib);
							break;
						case SP_IA_NO_VALUE_COMPARE:
						case SP_IA_LAST:
							break;
					}
				}
				if (l_bAttribEquals) {
					//transfer possibly attached #IA#-attributes of param attributes
					const SP_ListAttribute *l_pcTemp = l_pcSecondAttrib->GetAttributes();
					wxString l_sName;
					SP_ListAttribute::const_iterator itA = l_pcTemp->begin(), itA2;
					while (itA != l_pcTemp->end()) {
						if (wxString((*itA)->GetName()).StartsWith(wxT("#IA#PARENT#"), &l_sName)) {
							//transfer #IA#-attributes of parent data that were attached to the param attributes
							(*itA)->SetName(wxString(wxT("#IA#") + l_sName));
							itA2 = itA;
							++itA;
							(*it)->AddAttribute(l_pcSecondAttrib->RemoveAttribute(*itA2));
						} else {
							++itA;
						}
					}
					l_bAddData = true;
				}
			}
		}
		if (l_bAddData) {
			l_pcResult.push_back(*it);
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) (*it)->SetIAState(m_eSelection);
		} else {
			if (m_eSelection == SP_IA_SELECTED || m_eSelection == SP_IA_MAP_SELECTED) {
				//if we just select matching elements take unmatching ones, too
				//but set them to normal state
				l_pcResult.push_back(*it);
				if ((*it)->GetIAState() == m_eSelection) (*it)->SetIAState(SP_IA_NORMAL);
			} else if (m_eSelection == SP_IA_NORMAL) {
				//resetting IAState for elements not matching filtering criteria
				ReleaseData(*it);
			}
		}
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}


bool
SP_IA_DP_FilterByAttribute::CheckContains(SP_DS_Attribute *first, SP_DS_Attribute *second) {
	//check if wxT("first" contains "second")

	if (first->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST) {
		return dynamic_cast<SP_DS_ListAttribute*>(first)->Contains((*second).GetValueString());
	}
	//if the first attribute is no list, we might check for equality... (only values)
	return (*first).Equals(second, true, false, false);
}


void
SP_IA_DP_FilterByAttribute::TransferMultiplicity(SP_DS_ListAttribute *p_pcList, const wxString& p_sValue, SP_Data *p_pcTo) {

	if (!p_pcList || !p_pcTo) return;
	int count = 0;

	for (int i = 0; i < p_pcList->Size(); i++) {
		if (p_pcList->GetValue(i).Cmp(p_sValue) == 0) {
			count++;
		}
	}
	if (count > 1) p_pcTo->AddAttribute(new SP_DS_NumberAttribute(wxT("#IA#MULTI#"), count));
}


void
SP_IA_DP_FilterByAttribute::AddParams(SP_DS_Attribute *p_pcAttrib) {

	AddParams(SP_IA_NO_VALUE_COMPARE, p_pcAttrib);
}


void
SP_IA_DP_FilterByAttribute::AddParams(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib) {

	if (!p_pcAttrib || p_eOp < SP_IA_NO_VALUE_COMPARE || p_eOp >= SP_IA_LAST) return;

	if (!m_pcAttributes) {
		m_pcAttributes = new list<SP_PairIACompareAttribute*>();
	}
	m_pcAttributes->push_back(new SP_PairIACompareAttribute(p_eOp, p_pcAttrib));
}


void
SP_IA_DP_FilterByAttribute::SetParams(SP_DS_Attribute *p_pcAttrib) {

	if (!p_pcAttrib) return;

	//clean up old attributes
	if (!IsCopy() && m_pcAttributes) {
		list<SP_PairIACompareAttribute*>::iterator it;
		SP_PairIACompareAttribute *l_pcPair;
		for (it = m_pcAttributes->begin(); it != m_pcAttributes->end(); ++it) {
			l_pcPair = *it;
			wxDELETE((l_pcPair->second));
			wxDELETE(l_pcPair);
		}
		m_pcAttributes->clear();
	}

	AddParams(SP_IA_NO_VALUE_COMPARE, p_pcAttrib);
}


void
SP_IA_DP_FilterByAttribute::SetParams(SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcAttrib) {

	if (!p_pcAttrib) return;

	//clean up old attributes
	if (!IsCopy() && m_pcAttributes) {
		list<SP_PairIACompareAttribute*>::iterator it;
		SP_PairIACompareAttribute *l_pcPair;
		for (it = m_pcAttributes->begin(); it != m_pcAttributes->end(); ++it) {
			l_pcPair = *it;
			wxDELETE((l_pcPair->second));
			wxDELETE(l_pcPair);
		}
		m_pcAttributes->clear();
	}

	AddParams(p_eOp, p_pcAttrib);
}


void
SP_IA_DP_FilterByAttribute::SetParams(list<SP_PairIACompareAttribute*> *p_pcAttribPairs) {

	//clean up old attributes
	if (!IsCopy() && m_pcAttributes) {
		list<SP_PairIACompareAttribute*>::iterator it;
		SP_PairIACompareAttribute *l_pcPair;
		for (it = m_pcAttributes->begin(); it != m_pcAttributes->end(); ++it) {
			l_pcPair = *it;
			wxDELETE((l_pcPair->second));
			wxDELETE(l_pcPair);
		}
		wxDELETE(m_pcAttributes);
	}

	m_pcAttributes = p_pcAttribPairs;
}
