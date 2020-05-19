/*
 * SP_ExportHPN2CPN.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 04.02.2011
 * Short Description:
 */
//======================================================================================
#include "export/SP_ExportHPN2CPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"

bool SP_ExportHPN2CPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;

	m_graph = m_doc->GetGraph();

	m_fileName = p_fileName;

	//Get the total number of place
	m_nPlaceCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceCount += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	//Get the total number of transitions
	m_mTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size();
	m_mTransitionCount += m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size();

	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportHPN2CPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE);
	CHECK_POINTER( p_pcRoot, return FALSE);

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE);
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (l_sNodeclassName == SP_DS_DISCRETE_PLACE)
	{
		m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
		m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), m_nPlaceCount));
		m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_PLACE);
		p_pcRoot->AddChild(m_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
		
			/*lines 64-69 Added by G.A and line 70 is modified also*/
			/**
			wxString m_newType = wxT("Continuous Place");//to be removed
			SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed
			SP_DS_Node* ConvertedNode1 = m_converter.Clone((**l_Iter), *l_pcConvertToNodeClass);// ConvertNode((*l_Iter), l_pcConvertToNodeClass);// to be removed
 			SP_DS_Attribute* nameAttr = ConvertedNode1->GetAttribute(wxT("ID"));
			wxString valString = nameAttr->GetValueString();
			m_names.push_back(valString);
			WritePlace(ConvertedNode1, m_pcElem);
			 */
			SP_DS_Node* l_pcOldNode = dynamic_cast<SP_DS_Node*>(*l_Iter);
			m_converter.ChangeRepresentation(l_pcOldNode);
			WriteNode(l_pcOldNode, m_pcElem);
			m_converter.ResetNodeRepresentation(l_pcOldNode);
		}
	}
	else
		if (l_sNodeclassName == SP_DS_CONTINUOUS_PLACE)
		{
			for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
			{
				/**
				SP_DS_Attribute* nameAttr = (*l_Iter)->GetAttribute(wxT("Name"));
				wxString valString = nameAttr->GetValueString();
				bool isConv = m_converter.IsConverted(valString);
				for (int i = 0; i < m_names.size(); i++)
				{
					if (m_names[i] == valString)
					{
						return TRUE;
					}
				}
				*/
				WritePlace((*l_Iter), m_pcElem);
				 
			}
		}
		else
			if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{
				unsigned long l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
				l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUS_TRANS);
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					/**
					wxString m_newType = wxT("Continuous Transition"); 

					SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);

					SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter), *l_pcConvertToNodeClass);

 					ConvertedNode->Update();

					SP_DS_Attribute* nameAttr = ConvertedNode->GetAttribute(wxT("ID"));

					wxString valString = nameAttr->GetValueString();

					m_names.push_back(valString);

					WriteTransition(ConvertedNode, m_pcElem); 
					*/
					SP_DS_Node* l_pcOldNode = dynamic_cast<SP_DS_Node*>(*l_Iter);
					m_converter.ChangeRepresentation(l_pcOldNode);
					WriteNode(l_pcOldNode, m_pcElem);
					m_converter.ResetNodeRepresentation(l_pcOldNode);
 				}
			}
			else
				if (l_sNodeclassName == SP_DS_IMMEDIATE_TRANS)
				{

					for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
					{
						WriteTransition((*l_Iter), m_pcElem);
					}
				}
				else
					if (l_sNodeclassName == SP_DS_DETERMINISTIC_TRANS)
					{
						for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
						{
							WriteTransition((*l_Iter), m_pcElem);
						}
					}
					else
						if (l_sNodeclassName == SP_DS_SCHEDULED_TRANS)
						{

							for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
							{
								WriteTransition((*l_Iter), m_pcElem);
							}
						}
						else if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS)
						{


							for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
							{

								SP_DS_Attribute* IdAttr = (*l_Iter)->GetAttribute(wxT("ID"));

								wxString valString = IdAttr->GetValueString();

								for (int i = 0; i < m_names.size(); i++)
								{
									if (m_names[i] == valString) return TRUE;
								}

								WriteTransition((*l_Iter), m_pcElem); 
							}
						}
						else
						{

							wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
							l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_plElements->size()));
							l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
							p_pcRoot->AddChild(l_pcElem);

							for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
							{
								WriteNode((*l_Iter), l_pcElem);
							}
						}

	return TRUE;

}

bool SP_ExportHPN2CPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE);
	CHECK_POINTER( p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

		WriteAttribute((*l_Iter), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportHPN2CPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE);
	CHECK_POINTER( p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE);

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

bool SP_ExportHPN2CPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_HYBRIDPN_CLASS);
}
