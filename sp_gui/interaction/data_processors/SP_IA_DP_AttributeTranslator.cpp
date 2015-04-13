// SP_IA_DP_AttributeTranslator.cpp: Implementierung der Klasse SP_IA_DP_AttributeTranslator.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_AttributeTranslator.h"

#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SP_IA_DP_AttributeTranslator::SP_IA_DP_AttributeTranslator()
: SP_IA_DataProcessor(),
m_pcFromAttrib(NULL),
m_pcToAttrib(NULL)
{ }


SP_IA_DP_AttributeTranslator::SP_IA_DP_AttributeTranslator(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom)
: SP_IA_DataProcessor(),
m_pcFromAttrib(NULL),
m_pcToAttrib(NULL)

{
	SetParams(p_pcTo, p_eOp, p_pcFrom);
}


SP_IA_DP_AttributeTranslator::~SP_IA_DP_AttributeTranslator()
{
	//only not copied dp's can delete their parameters
	//copied dp's only hold references, so they must not delete their parameters
	if (!IsCopy()) {
		if (m_pcFromAttrib) wxDELETE(m_pcFromAttrib);
		if (m_pcToAttrib) wxDELETE(m_pcToAttrib);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_AttributeTranslator::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_AttributeTranslator(m_pcToAttrib, m_eMapOp, m_pcFromAttrib);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_AttributeTranslator::DoProcessing(SP_ListData *p_pcData) {

	SP_ListData l_pcResult;
	SP_ListData::iterator it;

	if (m_pcFromAttrib->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST
			&& m_pcToAttrib->GetAttributeType() != SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST) {
		for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			//check if we should only process selected elements
			//if so, just transfer the unselected ones
			if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
				l_pcResult.push_back(*it);
				continue;
			}

			SP_DS_Attribute *l_pcFound = (*it)->GetAttribute(m_pcFromAttrib->GetName());
			if (!l_pcFound) {
				SP_LOGDEBUG(wxT("WARNING in SP_IA_DP_AttributeTranslator::DoProcessing(...):")
											wxT("Couldn't find specified attribute in one data element while mapping."));
				ReleaseData(*it);
				continue;
			}
			if (m_eMapOp == SP_IA_IS_CONTAINED || m_eMapOp == SP_IA_IS_CONTAINED_NOT) {
				//just use the found list-attribute,
				//but clone it and then rename it in order to be able to identify the target attributes with it
				(l_pcFound = (*l_pcFound).Clone())->SetName(m_pcToAttrib->GetName());
				TransferAttachedAttributes(*it, l_pcFound);
				//transfer IAState of parent to created attribute
				l_pcFound->SetIAState((*it)->GetIAState());
				l_pcResult.push_back(l_pcFound);
			} else {
				//map each item of the list attributes of every data element onto a single attribute
				//note: afterwards one cannot distinguish which attribute came from which data element
				//			so be careful when using this for more than one data element
				for (int i = 0; i < dynamic_cast<SP_DS_ListAttribute*>(l_pcFound)->Size(); i++) {
					bool l_pcAdd = true;
					SP_DS_Attribute *l_pcTo = m_pcToAttrib->Clone();
					(*l_pcTo).SetValueString(dynamic_cast<SP_DS_ListAttribute*>(l_pcFound)->GetValue(i));
					//if this attribute was already added, then don't add it again but mark it as wxT("multiple")...
					for (SP_ListData::iterator itR = l_pcResult.begin(); itR != l_pcResult.end(); ++itR) {
						SP_Data* l_pcData = *itR;
						if (dynamic_cast<SP_DS_Attribute*>(l_pcData)->Equals(l_pcTo, true, false, false)) {
							SP_DS_NumberAttribute *l_pcMulti = dynamic_cast<SP_DS_NumberAttribute*>((*itR)->GetAttribute(wxT("#IA#MULTI#")));
							if (l_pcMulti) {
								l_pcMulti->SetValue(l_pcMulti->GetValue() + 1);
							} else {
								(*itR)->AddAttribute(new SP_DS_NumberAttribute(wxT("#IA#MULTI#"), 2));
							}
							l_pcAdd = false;
						}
					}
					//...else just add it
					if (l_pcAdd) {
						TransferAttachedAttributes(*it, l_pcTo);
						l_pcTo->SetIAState((*it)->GetIAState());
						l_pcResult.push_back(l_pcTo);
					}
				}
			}
			ReleaseData(*it);
		}
	} else if (m_pcFromAttrib->GetAttributeType() != SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST
			&& m_pcToAttrib->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST) {
		SP_DS_ListAttribute *l_pcTo = dynamic_cast<SP_DS_ListAttribute*>(m_pcToAttrib->Clone());
		for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			//check if we should only process selected elements
			//if so, just transfer the unselected ones
			if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
				l_pcResult.push_back(*it);
				continue;
			}

			SP_DS_Attribute *l_pcFound = (*it)->GetAttribute(m_pcFromAttrib->GetName());
			if (!l_pcFound) {
				SP_LOGDEBUG(wxT("WARNING in SP_IA_DP_AttributeTranslator::DoProcessing(...): ")
											wxT("Couldn't find specified attribute in one data element while mapping."));
				ReleaseData(*it);
				continue;
			}
			if (m_eMapOp == SP_IA_CONTAINS) {
				//just add the found attribute
				(l_pcFound = (*l_pcFound).Clone())->SetName(m_pcToAttrib->GetName());
				TransferAttachedAttributes(*it, l_pcFound);
				l_pcFound->SetIAState((*it)->GetIAState());
				l_pcResult.push_back(l_pcFound);
			} else {
				//if we find one selected parent, the entire list attribute is marked as SP_IA_SELECTED
				if ((*it)->GetIAState() == SP_IA_SELECTED) l_pcTo->SetIAState(SP_IA_SELECTED);
				l_pcTo->AddValue((*l_pcFound).GetValueString());
				SP_DS_NumberAttribute *tmp = dynamic_cast<SP_DS_NumberAttribute*>((*it)->GetAttribute(wxT("#IA#MULTI#")));
				if (tmp) {
					//add additional values for multiples
					for (int i = 1;	i < tmp->GetValue(); i++) {
						l_pcTo->AddValue((*l_pcFound).GetValueString());
					}
					(*it)->RemoveAttribute(tmp);
				}
				//combine the attribute values of every data element into one list attribute
				TransferAttachedAttributes(*it, l_pcTo);
			}
			ReleaseData(*it);
		}
		if (m_eMapOp != SP_IA_CONTAINS)
		{
			if (l_pcTo->Size() == 0 && ProcessSelected())
			{
				l_pcTo->SetIAState(SP_IA_SELECTED);
			}
			l_pcResult.push_back(l_pcTo);
		}
		else
		{
			wxDELETE(l_pcTo);
		}

	} else {
		//just transfer the attribute value from m_pcFromAttrib to m_pcToAttrib
		for (it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			//check if we should only process selected elements
			//if so, just transfer the unselected ones
			if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
				l_pcResult.push_back(*it);
				continue;
			}

			SP_DS_Attribute *l_pcFound = (*it)->GetAttribute(m_pcFromAttrib->GetName());
			if (!l_pcFound) {
				SP_LOGDEBUG(wxT("WARNING in SP_IA_DP_AttributeTranslator::DoProcessing(...):")
											wxT("Couldn't find specified attribute in one data element while mapping."));
			} else {
				SP_DS_Attribute *l_pcTo = m_pcToAttrib->Clone();
				(*l_pcTo).SetValueString((*l_pcFound).GetValueString());
				TransferAttachedAttributes(*it, l_pcTo);
				l_pcTo->SetIAState((*it)->GetIAState());
				l_pcResult.push_back(l_pcTo);
			}
			ReleaseData(*it);
		}
	}
	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}


void
SP_IA_DP_AttributeTranslator::TransferAttachedAttributes(SP_Data *p_pcSource, SP_Data *p_pcTarget) {
	const SP_ListAttribute *l_pcAttribList = p_pcSource->GetAttributes();
	SP_ListAttribute::const_iterator it = l_pcAttribList->begin(), it2;
	wxString l_sName;

	while (it != l_pcAttribList->end()) {
		if (wxString((*it)->GetName()).StartsWith(wxT("#IA#"), &l_sName)) {
			it2 = it;
			++it;
			(*it2)->SetName(wxString::Format(wxT("#IA#PARENT#%s"), l_sName.c_str()));
			p_pcTarget->AddAttribute(p_pcSource->RemoveAttribute(*it2));
		} else {
			++it;
		}
	}
}


void
SP_IA_DP_AttributeTranslator::SetParams(SP_DS_Attribute *p_pcTo, SP_IA_COMPARE_OP p_eOp, SP_DS_Attribute *p_pcFrom) {

	if (!p_pcTo || !p_pcFrom || p_eOp < SP_IA_NO_VALUE_COMPARE || p_eOp >= SP_IA_LAST) return;

	//cleaning up old parameter, only if we possess them, of course
	if (!IsCopy()) {
		if (m_pcFromAttrib) wxDELETE(m_pcFromAttrib);
		if (m_pcToAttrib) wxDELETE(m_pcToAttrib);
	}

	m_pcFromAttrib = p_pcFrom;
	m_pcToAttrib = p_pcTo;
	m_eMapOp = p_eOp;
}
