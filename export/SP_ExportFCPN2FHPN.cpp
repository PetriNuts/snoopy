/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 28.01.2019
* Short Description:FCPN===>FHPN
*/
//======================================================================================
#include "export/SP_ExportFCPN2FHPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_revision.h"

bool SP_ExportFCPN2FHPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;

	m_graph = m_doc->GetGraph();
	m_pcMetaDataConstants = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
	m_fileName = p_fileName;
	//m_graph->AddMetadataclass(m_pcMetaDataConstants);
	//Get the total number of place
		m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
//	m_nPlaceCount += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	//Get the total number of transitions
	m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	//m_mTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
	//m_mTransitionCount += m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size();
//	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size();
	//m_mTransitionCount += m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size();


	////////////////////////////

	///////////////////////
	bool b = SP_XmlWriter::Write(m_graph, m_fileName);
	//m_graph->AddMetadataclass(m_pcMetaDataConstants);
	return b;
}



bool SP_ExportFCPN2FHPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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
			m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
			m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), m_nPlaceCount));
			m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_PLACE);
			p_pcRoot->AddChild(m_pcElem);

			for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
			{
				WritePlace((*l_Iter), m_pcElem);
			}
		}
		else
			if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
				SP_ListNode::const_iterator l_itElem;

				unsigned long	l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				////////////////////////////

				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_STOCHASTIC_TRANS);//fron stoch trans 2 cont. trans
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					SP_DS_Node* l_pcNode = *l_Iter;
					wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					WriteTransition((*l_Iter), m_pcElem);

				}
			}
			else if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS)
			{
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
				SP_ListNode::const_iterator l_itElem;

				unsigned long	l_mContinuousTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
				////////////////////////////

				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mContinuousTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_TRANS);//fron stoch trans 2 cont. trans
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					SP_DS_Node* l_pcNode = *l_Iter;
					wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
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

bool SP_ExportFCPN2FHPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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


bool SP_ExportFCPN2FHPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFCPN2FHPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYCPN_CLASS);
}



bool SP_ExportFCPN2FHPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
{
	if (p_pcVal->GetClassName() == SP_DS_META_CONSTANT)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
		if (l_pcAttr)
		{
			//if (l_pcAttr->GetValueString() != wxT("int"))
			//	return false;
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
