/*
* SP_ExportFHPN2HPN.h
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 28.01.2019
* Short Description: export FHPN ===> HPN
*/
//======================================================================================
#include "export/SP_ExportFHPN2HPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_revision.h"

bool SP_ExportFHPN2HPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;

	m_graph = m_doc->GetGraph();
	//m_pcMetaDataConstants = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	m_fileName = p_fileName;
	//m_graph->AddMetadataclass(m_pcMetaDataConstants);
	//Get the total number of place
	m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceCount += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	//Get the total number of transitions
	m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size();


	///////////////////////
	bool b = SP_XmlWriter::Write(m_graph, m_fileName);
 	return b;
}


bool SP_ExportFHPN2HPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	//SP_DS_Nodeclass* p_pcVal1= p_pcVal->getclo
	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (l_sNodeclassName == SP_DS_DISCRETE_PLACE)
	{
		m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
		m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), m_nPlaceCount));
		m_pcElem->AddAttribute(wxT("name"), SP_DS_DISCRETE_PLACE);
		p_pcRoot->AddChild(m_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WritePlace((*l_Iter), m_pcElem);
		}
	}
	else
		if (l_sNodeclassName == SP_DS_CONTINUOUS_PLACE)
		{

			for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
			{
				WritePlace((*l_Iter), m_pcElem);
			}
			
		}
		else
			if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{

				SP_DS_ColListAttribute * l_pcColList;
 				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
				SP_ListNode::const_iterator l_itElem;

				unsigned long	l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();

				////////////////////////////
 				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_STOCHASTIC_TRANS);
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
						WriteTransition((*l_Iter), m_pcElem);
					 
				}
			}
			else if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS)
			{
				SP_DS_ColListAttribute * l_pcColList;
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
				SP_ListNode::const_iterator l_itElem;
				unsigned long	l_mContTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();

				////////////////////////////
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mContTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_TRANS);
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					  
						WriteTransition((*l_Iter), m_pcElem);
				}

			}
			else
			{

				wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), (unsigned long int)l_plElements->size()));
				l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
				p_pcRoot->AddChild(l_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteNode((*l_Iter), l_pcElem);
				}
			}

	return TRUE;

}

bool SP_ExportFHPN2HPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

		WriteAttribute((*l_Iter), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportFHPN2HPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

		if ((*l_Iter)->GetName() == wxT("DelayList"))
		{

		}
		else
			if ((*l_Iter)->GetName() == wxT("PeriodicList"))
			{

			}
			else
			{
				WriteAttribute((*l_Iter), l_pcElem);
			}
	}
	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportFHPN2HPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYHPN_CLASS);
}
/*
bool SP_ExportFHPN2HPN::IsParamFN(wxString& param)
{
	bool result = false;

	SP_DS_Metadataclass* l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	///////////////////

	SP_ListMetadata::const_iterator l_itElem;
	for (l_itElem = l_pcMetadataclass->GetElements()->begin();
		l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Metadata* l_pcMetadata = *l_itElem;
		wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
		//wxString l_sMetadataValue = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Value")))->GetValue();
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
		wxString l_sMetadataValue;
		wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();

		if (param == l_sMetadataName && (l_sMetadataType.Cmp(wxT("TFN")) == 0 || l_sMetadataType.Cmp(wxT("TZN")) == 0 || l_sMetadataType.Cmp(wxT("BFN")) == 0))
		{
			result = true;
		}
	}

	return result;

}
*/
bool SP_ExportFHPN2HPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
{
	if (p_pcVal->GetClassName() == SP_DS_META_CONSTANT)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
		if (l_pcAttr)
		{
			if (l_pcAttr->GetValueString() == wxT("TFN") || l_pcAttr->GetValueString() == wxT("BFN"))
			{

				SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (p_pcVal->GetAttribute(wxT("ValueList")));
				wxString m_type = l_pcAttr->GetValueString();
				std::vector<wxString> oldValues;
				for (unsigned int i = 0; i < l_pcColList->GetRowCount(); ++i)
				{
					/*changing the FN Constant to its Crisp vlaue*/
					bool l_bValue = p_pcVal->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
					wxString oldval = l_pcColList->GetCell(i, 1);
					wxString secondset = l_pcColList->GetCell(1, 1);
					oldValues.push_back(oldval);
					wxString crsip = GetCrispValue(oldval, m_type);
					l_pcColList->SetCell(i, 1, crsip);
				}
				bool res = SP_XmlWriter::WriteMetadata(p_pcVal, p_pcRoot);
				bool l_bValue1 = p_pcVal->GetAttribute(wxT("Type"))->SetValueString(m_type);

				for (unsigned int i = 0; i < l_pcColList->GetRowCount(); ++i)
				{
					wxString val = oldValues[i];
					l_pcColList->SetCell(i, 1, val);
				}
				return res;

			}
		}
	}
	else if (p_pcVal->GetClassName() == SP_DS_META_FUNCTION)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Return"));
		if (l_pcAttr)
		{
			if (l_pcAttr->GetValueString() != wxT("int"))
				return false;
		}
	}

	return SP_XmlWriter::WriteMetadata(p_pcVal, p_pcRoot);
}
/*
bool SP_ExportFHPN2HPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
{
	if (p_pcVal->GetClassName() == SP_DS_META_CONSTANT)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
		if (l_pcAttr)
		{
			if (l_pcAttr->GetValueString() == wxT("TFN")  || l_pcAttr->GetValueString() == wxT("BFN"))
				return false;
		}
	}
	else if (p_pcVal->GetClassName() == SP_DS_META_FUNCTION)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Return"));
		if (l_pcAttr)
		{
			if (l_pcAttr->GetValueString() != wxT("int"))
				return false;
		}
	}

	return SP_XmlWriter::WriteMetadata(p_pcVal, p_pcRoot);
}
*/
wxString  SP_ExportFHPN2HPN::GetCrispValue(wxString& param, wxString& type)
{
	wxString crispValue;
	if (type == wxT("TFN") || type == wxT("BFN"))
	{
		SP_VectorDouble fn_param = GetFNConstants(param);
		if (fn_param.size() > 0)
		{
			crispValue << fn_param[1];
		}
	}
	return crispValue;

}

SP_VectorDouble   SP_ExportFHPN2HPN::GetFNConstants(wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string sVal = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = sVal.find(delimiter)) != std::string::npos) {
		numConst++;
		token = sVal.substr(0, pos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		sVal.erase(0, pos + delimiter.length());

	}
	wxString sLastConts = sVal;
	double dLastConst;

	if (sLastConts.ToDouble(&dLastConst)) { v_dConstants.push_back(dLastConst); }


	return v_dConstants;
}