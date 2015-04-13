// SP_IA_DP_ElementCreator.cpp: Implementierung der Klasse SP_IA_DP_ElementCreator.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_DP_ElementCreator.h"


SP_IA_DP_ElementCreator::SP_IA_DP_ElementCreator()
: SP_IA_DataProcessor(),
m_pcNodeclass(NULL),
m_pcEdgeclass(NULL)
{ }


SP_IA_DP_ElementCreator::SP_IA_DP_ElementCreator(SP_DS_Nodeclass *p_pcNC, SP_DS_Edgeclass *p_pcEC)
: SP_IA_DataProcessor(),
m_pcNodeclass(p_pcNC),
m_pcEdgeclass(p_pcEC)
{ }


SP_IA_DP_ElementCreator::~SP_IA_DP_ElementCreator()
{
	if (!IsCopy()) {
		if (m_pcNodeclass) wxDELETE(m_pcNodeclass);
		if (m_pcEdgeclass) wxDELETE(m_pcEdgeclass);
	}
}


SP_IA_DataProcessor*
SP_IA_DP_ElementCreator::Copy() {

	SP_IA_DataProcessor *l_pcNewDP = new SP_IA_DP_ElementCreator(m_pcNodeclass, m_pcEdgeclass);
	l_pcNewDP->SetProcessSelected(ProcessSelected());
	l_pcNewDP->SetCopyStamp(true);

	return l_pcNewDP;
}


SP_ListData*
SP_IA_DP_ElementCreator::DoProcessing(SP_ListData *p_pcData) {

	//TODO?:	at the moment this processor doesn't really care about #IA#MULTI#
	//				which means, if we get multiple attributes they are only used once
	//				but they are transfered to the new node though

	if (!p_pcData || p_pcData->empty()) return 0;

	//this is true as long as there are only equal (name and type) attributes
	bool l_bCreateManyElems = true;

	SP_DS_Attribute *l_pcFirstAttrib = 0;
	//this loop determines whether we will create only one or more nodes
	//...one node if p_pcData contains different attribute types (and names),
	//...many nodes if p_pcData contains only attributes of the same type (and name)
	for (SP_ListData::iterator it = p_pcData->begin(); it != p_pcData->end(); ++it) {
		//data element is not an attribute or is not to be processed
		SP_Data* l_pcData = *it;
		if (!dynamic_cast<SP_DS_Attribute*>(l_pcData)
				|| (ProcessSelected() && l_pcData->GetIAState() != SP_IA_SELECTED)) continue;
		//first loop iteration (no need to compare anything)
		if (!l_pcFirstAttrib) {
			l_pcFirstAttrib = dynamic_cast<SP_DS_Attribute*>(l_pcData);
			continue;
		}
		l_bCreateManyElems &= l_pcFirstAttrib->Equals(dynamic_cast<SP_DS_Attribute*>(l_pcData), false);
		if (!l_bCreateManyElems) {
			//we can stop once we've found a different attribute
			break;
		}
	}
	SP_ListData l_pcResult;
	//if there is a node- and an edgeclass we add a proper wxT("dummy")-egde
	//in general, edges can be added either before every single node, giving the opportunity to have different edges...
	//...or (like here) just at the beginning, which means that this edge type is used to connect all following nodes
	//in any case, wxT("edges first - nodes second") is important for the following SP_IA_Command_Insert
	if (m_pcNodeclass && m_pcEdgeclass) {
		SP_DS_Edge *l_pcNewEdge = new SP_DS_Edge(m_pcEdgeclass);
		l_pcResult.push_back(l_pcNewEdge);
	}

	if (l_bCreateManyElems) {
		//in here we create one element for every attribute in p_pcData
		for (SP_ListData::iterator it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
				l_pcResult.push_back(*it);
				continue;
			}
			SP_Data* l_pcData = *it;
			SP_DS_Attribute *l_pcAttrib = dynamic_cast<SP_DS_Attribute*>(l_pcData);
			if (l_pcAttrib) {
				//creating a wxT("dummy")-element of the proper node-/edgeclass in order to hold the needed attribute(s)
				SP_Data *l_pcNewElem;
				if (m_pcNodeclass) {
					l_pcNewElem = new SP_DS_Node(m_pcNodeclass);
				} else if (m_pcEdgeclass) {
					l_pcNewElem = new SP_DS_Edge(m_pcEdgeclass);
				} else {
					ReleaseData(*it, true);
					continue;
				}
				l_pcAttrib->SetName(wxString(wxT("#IA#VALUE#")) + l_pcAttrib->GetName());
				l_pcNewElem->AddAttribute(l_pcAttrib);

				//transfer attached #IA#PARENT#- and #IA#MULTI# attributes from l_pcAttrib to l_pcNewElem
				const SP_ListAttribute *l_pcSubAttribs = l_pcAttrib->GetAttributes();
				SP_ListAttribute::const_iterator itSA = l_pcSubAttribs->begin(), itSA2;
				while (itSA != l_pcSubAttribs->end()) {
					wxString l_sAttribName;
					if (wxString((*itSA)->GetName()).StartsWith(wxT("#IA#PARENT#"), &l_sAttribName)) {
						(*itSA)->SetName(wxString(wxT("#IA#VALUE#")) + l_sAttribName);
						itSA2 = itSA;
						++itSA;
						l_pcNewElem->AddAttribute(l_pcAttrib->RemoveAttribute(*itSA2));
					} else if (wxString((*itSA)->GetName()).Cmp(wxT("#IA#MULTI#")) == 0) {
						itSA2 = itSA;
						++itSA;
						l_pcNewElem->AddAttribute(l_pcAttrib->RemoveAttribute(*itSA2));
					} else {
						++itSA;
					}
				}
				l_pcResult.push_back(l_pcNewElem);
			} else {
				//this data element will not be put into the list anymore
				ReleaseData(*it);
			}
		}
	} else {
		//in here we create only one node and give it all attributes in p_pcData
		//creating a wxT("dummy")-node of the proper nodeclass to hold the needed attribute(s)
		SP_Data *l_pcNewElem = 0;
		if (m_pcNodeclass) {
			l_pcNewElem = new SP_DS_Node(m_pcNodeclass);
		} else if (m_pcEdgeclass) {
			l_pcNewElem = new SP_DS_Edge(m_pcEdgeclass);
		}

		for (SP_ListData::iterator it = p_pcData->begin(); it != p_pcData->end(); ++it) {
			if (ProcessSelected() && (*it)->GetIAState() != SP_IA_SELECTED) {
				l_pcResult.push_back(*it);
				continue;
			}
			SP_Data* l_pcData = *it;
			SP_DS_Attribute *l_pcAttrib = dynamic_cast<SP_DS_Attribute*>(l_pcData);
			//other than attributes will not be put into the list anymore, so reset their IAState
			if (!l_pcAttrib) {
				ReleaseData(*it);
				continue;
			}
			if (!l_pcNewElem) {
				//if there was no element created, there is nowhere where we can put this attribute
				//so we delete it
				ReleaseData(*it, true);
				continue;
			}

			l_pcAttrib->SetName(wxString(wxT("#IA#VALUE#")) + l_pcAttrib->GetName());
			l_pcNewElem->AddAttribute(l_pcAttrib);

			//transfer attached #IA#PARENT#- and #IA#MULTI#-attributes from l_pcAttrib to l_pcNewNode
			const SP_ListAttribute *l_pcSubAttribs = l_pcAttrib->GetAttributes();
			SP_ListAttribute::const_iterator itSA = l_pcSubAttribs->begin(), itSA2;
			while (itSA != l_pcSubAttribs->end()) {
				wxString l_sAttribName;
				if (wxString((*itSA)->GetName()).StartsWith(wxT("#IA#PARENT#"), &l_sAttribName)) {
					(*itSA)->SetName(wxString(wxT("#IA#VALUE#")) + l_sAttribName);
					itSA2 = itSA;
					++itSA;
					l_pcNewElem->AddAttribute(l_pcAttrib->RemoveAttribute(*itSA2));
				} else if (wxString((*itSA)->GetName()).Cmp(wxT("#IA#MULTI#")) == 0) {
					itSA2 = itSA;
					++itSA;
					l_pcNewElem->AddAttribute(l_pcAttrib->RemoveAttribute(*itSA2));
				} else {
					++itSA;
				}
			}
		}
		l_pcResult.push_back(l_pcNewElem);
	}

	*p_pcData = l_pcResult;
	l_pcResult.clear();

	return p_pcData;
}


void
SP_IA_DP_ElementCreator::SetParams(SP_DS_Nodeclass *p_pcNC, SP_DS_Edgeclass *p_pcEC) {
	//clean up old parameter
	if (!IsCopy()) {
		if (m_pcNodeclass) wxDELETE(m_pcNodeclass);
		if (m_pcEdgeclass) wxDELETE(m_pcEdgeclass);
	}

	m_pcNodeclass = p_pcNC;
	m_pcEdgeclass = p_pcEC;
}
