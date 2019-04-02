/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 26.01.2019
* Short Description:export FCPN ==>  CPN
*/
//======================================================================================
#include "export/SP_ExportFCPN2CPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

bool SP_ExportFCPN2CPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;

	m_graph = m_doc->GetGraph();
	m_pcMetaDataConstants = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcMetaDataConstants;

 

	
	m_fileName = p_fileName;

	//Get the total number of place
	m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();


	//Get the total number of transitions
	m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();


	bool b= SP_XmlWriter::Write(m_graph, m_fileName);
	 
	return b;
}

bool SP_ExportFCPN2CPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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
			if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS  )
			{

				SP_DS_ColListAttribute * l_pcColList;
				SP_DS_ColListAttribute * l_pcColList1;
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
				SP_ListNode::const_iterator l_itElem;
				SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();


				unsigned long	l_mContTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();


				////////////////////////////
				//GetAttribute(wxT("Type"));
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mContTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_TRANS);
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					SP_DS_Node* l_pcNode = *l_Iter;
					wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
					//l_pcColList->c

					wxString l_sTransitionFunction = l_pcColList->GetActiveCellValue(1);

					//wxString KinetikP;

						WriteTransition((*l_Iter), m_pcElem);
					 
				}
			}
			else if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{
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

bool SP_ExportFCPN2CPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFCPN2CPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFCPN2CPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYCPN_CLASS);
}
 
bool SP_ExportFCPN2CPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot) 
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


wxString  SP_ExportFCPN2CPN::GetCrispValue(wxString& param,wxString& type )
{
	wxString crispValue;
	if(type== wxT("TFN") || type == wxT("BFN"))
	{
		SP_VectorDouble fn_param = GetFNConstants(param);
		if (fn_param.size() > 0)
		{
			crispValue << fn_param[1];
		}
	}
	return crispValue;

}

SP_VectorDouble   SP_ExportFCPN2CPN::GetFNConstants( wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string s = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		numConst++;
		token = s.substr(0, pos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		s.erase(0, pos + delimiter.length());

	}
	wxString ss = s;
	double d;

	if (ss.ToDouble(&d)) { v_dConstants.push_back(d); }


	return v_dConstants;
}