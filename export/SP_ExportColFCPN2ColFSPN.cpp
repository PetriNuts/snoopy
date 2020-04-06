//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: G.Assaf $
// $Version: 2.0 $
// $Date: 2020/04/2   $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportColFCPN2ColFSPN.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

bool SP_ExportColFCPN2ColFSPN::Write(SP_MDI_Doc* p_doc,
	const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();
	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportColFCPN2ColFSPN::WriteNetclass(SP_DS_Netclass* p_pcVal,
	wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_FUZZY_ColSPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportColFCPN2ColFSPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
	wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sNodeclassName = p_pcVal->GetName();
	wxString l_pcNodeclassName;

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_plElements->size()));
	if (SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		l_pcNodeclassName = SP_DS_DISCRETE_PLACE;
		l_pcElem->AddAttribute(wxT("name"), l_pcNodeclassName);
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteStochPlace((*l_Iter), l_pcElem);
		}
		return true;
	}
	else if (SP_DS_CONTINUOUS_TRANS == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Transition");
		l_pcElem->AddAttribute(wxT("name"), l_pcNodeclassName);

		p_pcRoot->AddChild(l_pcElem);
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteStochTransition((*l_Iter), l_pcElem);
		}
		return true;
	}
	else if (SP_DS_PARAM == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Parameter");
	}
	else if (SP_DS_COARSE_PLACE == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Coarse Place");
	}
	else if (SP_DS_COARSE_TRANSITION == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Coarse Transition");
	}
	else if (SP_DS_COARSE_PARAMETER == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Coarse Parameter");
	}
	else if (wxT("Comment") == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Comment");
	}
	else
	{
		return TRUE;
	}
	l_pcElem->AddAttribute(wxT("name"), l_pcNodeclassName);
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		WriteNode((*l_Iter), l_pcElem);

	return TRUE;
}


bool
SP_ExportColFCPN2ColFSPN::WriteStochPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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
		WriteAttribute((*l_Iter), l_pcElem);
	}

	return WriteData(p_pcVal, l_pcElem);

}

bool
SP_ExportColFCPN2ColFSPN::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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
		if ((*l_Iter)->GetName() != wxT("Reversible"))
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}

	}

	return WriteData(p_pcVal, l_pcElem);

}


bool SP_ExportColFCPN2ColFSPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (SP_DS_FUZZY_ColCPN_CLASS == cName);
}

