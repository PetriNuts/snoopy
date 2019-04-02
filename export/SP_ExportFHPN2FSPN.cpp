/*
* SP_ExportHPN2CPN.cpp
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 29.01.2019
* Short Description: FHPN==> FSPN
*/
//======================================================================================
#include "export/SP_ExportFHPN2FSPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"
#include "sp_revision.h"
//////////
//#include "sp_core/SP_Core.h"
//#include "sp_ds/SP_DS_Graph.h"
//#include "sp_gui/mdi/SP_MDI_View.h"
//#include "sp_gui/mdi/SP_MDI_Doc.h"
//#include "sp_gui/dialogs/SP_DLG_ConvertElements.h"
//#include "snoopy.h"
//#include "sp_core/SP_GPR_Elements.h"
//#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
//#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
//#include "sp_gr/edges/SP_GR_ArrowEdge.h"
//#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
//#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
//#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

//#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
//#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
//#include "sp_gr/attributes/SP_GR_ColListAttribute.h" 
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
/////////

bool SP_ExportFHPN2FSPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
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

bool SP_ExportFHPN2FSPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

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
				//////Obtain Marking of the Node//////
				wxString l_sAttributeName = wxT("Marking");
				SP_DS_Attribute* l_pcOldAttribute = (*l_Iter)->GetAttribute(l_sAttributeName);
				wxString valString = l_pcOldAttribute->GetValueString();
				double marking;
				valString.ToDouble(&marking);
				////////////


				wxString m_newType = wxT("Discrete Place");

				SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);
				
				
			 
 				SP_DS_Node* ConvertedNode = m_converter.Clone((**l_Iter),(*l_pcConvertToNodeClass));


				SP_DS_Attribute* l_pcNewAttribute = ConvertedNode->GetAttribute(l_sAttributeName);
				
				SP_DS_Attribute* att;
				
				if (floor(marking) == ceil(marking))
				{
					att = new SP_DS_MarkingAttribute(wxT("Marking"), (int)marking);
				}
				else
				{
					int roundedVal = round(marking);
					att = new SP_DS_MarkingAttribute(wxT("Marking"), roundedVal);
				}

				l_pcNewAttribute->CopyValueFrom(att);
				WritePlace(ConvertedNode, m_pcElem);// replace convertedNode by (*l_Iter)
			}
		}
		else
			if (l_sNodeclassName == SP_DS_STOCHASTIC_TRANS)
			{
				unsigned long l_mStochTransitionCount = m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
				 l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
			//	l_mStochTransitionCount += m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size();
				m_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				m_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_mStochTransitionCount));
				m_pcElem->AddAttribute(wxT("name"), SP_DS_STOCHASTIC_TRANS);
				p_pcRoot->AddChild(m_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteTransition((*l_Iter), m_pcElem);//WriteTransition
				}
			}
			else if (l_sNodeclassName == SP_DS_CONTINUOUS_TRANS)
			{
				 

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{

					wxString m_newType = wxT("Stochastic Transition");//to be removed
					SP_DS_Nodeclass* l_pcConvertToNodeClass = m_graph->GetNodeclassByDisplayedName(m_newType);//to be removed
					//SP_DS_Node* ConvertedNode = ConvertNode(*l_Iter, l_pcConvertToNodeClass);// to be removed
				//SP_DS_Node* ConvertedNode = m_converter.ConvertNode(*l_Iter, l_pcConvertToNodeClass);
					SP_DS_Node* ConvertedNode = m_converter.Clone(**l_Iter,* l_pcConvertToNodeClass);
					ConvertedNode->Update();
					
					WriteTransition(ConvertedNode, m_pcElem);//WriteTransition
				}
			}
			else
			{

				wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
				l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"),(unsigned long) l_plElements->size()));
				l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
				p_pcRoot->AddChild(l_pcElem);

				for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
				{
					WriteNode((*l_Iter), l_pcElem);
				}
			}

	return TRUE;

}

bool SP_ExportFHPN2FSPN::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFHPN2FSPN::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportFHPN2FSPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZYHPN_CLASS);
}

bool SP_ExportFHPN2FSPN::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot)
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

bool
SP_ExportFHPN2FSPN::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (!((*l_Iter)->GetName() == wxT("Reversible") || (*l_Iter)->GetName() == wxT("FunctionList")))
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}

	}

	return WriteData(p_pcVal, l_pcElem);
}

 