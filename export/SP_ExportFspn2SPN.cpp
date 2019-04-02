/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 25.01.2019
* Short Description:FSPN===>SPN
*/
//======================================================================================
#include "export/SP_ExportFspn2SPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
//#include "SP_DS_FunctionRegistry.h"
#include "sp_revision.h"

bool SP_ExportFspn2SPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
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
//	m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceCount += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	//Get the total number of transitions
	//m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size();

		
		////////////////////////////
	
		///////////////////////
	bool b= SP_XmlWriter::Write(m_graph, m_fileName);
	return b;
}

 

bool SP_ExportFspn2SPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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
				SP_DS_ColListAttribute * l_pcColList1;
				SP_DS_Nodeclass* l_pcNodeclass;
				l_pcNodeclass = m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
				SP_ListNode::const_iterator l_itElem;
				SP_DS_FunctionRegistry* l_pcFR = m_graph->GetFunctionRegistry();
			 
				 
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
				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteTransition((*l_Iter), m_pcElem);
				}
			}
			else
			{

				wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), (unsigned long int )l_plElements->size()));
				l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
				p_pcRoot->AddChild(l_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteNode((*l_Iter), l_pcElem);
				}
			}

	return TRUE;

}

bool SP_ExportFspn2SPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFspn2SPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFspn2SPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYSPN_CLASS);
}

wxString SP_ExportFspn2SPN::GetKenitikParam(wxString& func) 
{
	 
	
	unsigned int  first = func.find("(");
	unsigned int last = func.find(")");
	//param.Trim();
	if (first >= 0 && last >= 0)
		return func.substr(first + 1, last - first - 1);//stlstring.
	else
		return func;
	 

}



bool SP_ExportFspn2SPN::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	SP_DS_ColListAttribute* l_pcColList;
	SP_Graphic* l_pcGrAttr;

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		WriteAttribute((*l_Iter), l_pcElem);
	}

	l_pcColList = new SP_DS_ColListAttribute(wxT("FunctionList"), SP_COLLIST_STRING, 2);
	l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute(l_pcColList, wxT("%")));

	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25);
	l_pcGrAttr->SetShow(FALSE);

	l_pcColList->SetGlobalShow();

	l_pcColList->SetColLabel(0, wxT("Function set"));
	l_pcColList->SetColLabel(1, wxT("Function"));
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("MassAction(1)"));

	WriteAttribute(l_pcColList, l_pcElem);

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportFspn2SPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
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

wxString  SP_ExportFspn2SPN::GetCrispValue(wxString& param, wxString& type)
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

SP_VectorDouble   SP_ExportFspn2SPN::GetFNConstants(wxString &val)
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