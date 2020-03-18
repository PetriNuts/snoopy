//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/03/15 12:20:00 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColFCPN2FCPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_ExportUnfolding.h"


SP_ExportColFCPN2FCPN::SP_ExportColFCPN2FCPN()
{
}

SP_ExportColFCPN2FCPN::~SP_ExportColFCPN2FCPN()
{
}

bool SP_ExportColFCPN2FCPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZY_ColCPN_CLASS);
}

bool SP_ExportColFCPN2FCPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	SP_DS_ColPN_ExportUnfolding l_UnfoldingforExport;
	if (!l_UnfoldingforExport.Unfolding())
		return false;

	m_mNewPlaceNodes = l_UnfoldingforExport.GetNewContinuousPlaceNodes();
	m_mNewTransitionNodes = l_UnfoldingforExport.GetNewContinuousTransitionNodes();
	m_lNewEdges = l_UnfoldingforExport.GetNewEdges();

	//commented by george


	//parameter to constant
	//if(!p_doc->GetGraph()->GetNodeclass(wxT("Parameter")))
	//	 return false;

	//	const SP_ListNode* l_pcParams = p_doc->GetGraph()->GetNodeclass(wxT("Parameter"))->GetElements();
	//	CHECK_POINTER(l_pcParams,return false);

	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(SP_DS_FUZZYCPN_CLASS);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();
	m_pcExportGraph = new SP_DS_Graph(newClass);

	SP_DS_Metadataclass* l_pcMC = m_pcExportGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	l_pcMC->RemoveAll();
	/**
	SP_ListNode::const_iterator l_itParam;
	for(l_itParam=l_pcParams->begin();l_itParam!= l_pcParams->end(); l_itParam++)
	{
	SP_DS_Metadata* l_pcConstant;
	l_pcConstant = l_pcMC->NewElement(1);
	l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(wxT("parameter"));
	l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
	SP_DS_ColListAttribute* l_pcValueColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
	l_pcValueColListAttr->Clear();
	wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itParam)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
	SP_DS_ColListAttribute* l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(( *l_itParam )->GetAttribute(wxT("ParameterList")));
	for(unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
	{
	wxString l_sLabel = l_pcColListAttr->GetCell(i,0);
	wxString l_sValue = l_pcColListAttr->GetCell(i,1);
	int l_nRowCol = l_pcValueColListAttr->AppendEmptyRow();
	l_pcValueColListAttr->SetCell(l_nRowCol, 0, l_sLabel);
	l_pcValueColListAttr->SetCell(l_nRowCol, 1, l_sValue);
	}

	}
	*/
	//end of convertion

	bool l_bSuccess = SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

	l_UnfoldingforExport.Reset(); // Reset the net 

	wxDELETE(m_pcExportGraph);

	return l_bSuccess;
}

bool SP_ExportColFCPN2FCPN::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_FUZZYCPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportColFCPN2FCPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (!m_pcExportGraph->GetNodeclass(l_sNodeclassName))
	{
		return true;
	}

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_plElements->size()));

	l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
	p_pcRoot->AddChild(l_pcElem);

	if (SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		map<wxString, SP_DS_Node*>::iterator itMap;
		for (itMap = m_mNewPlaceNodes.begin(); itMap != m_mNewPlaceNodes.end(); ++itMap)
		{
			WriteUnfoldedPlace(itMap->second, l_pcElem);
		}
	}
	else if (SP_DS_CONTINUOUS_TRANS == l_sNodeclassName)
	{

		map<wxString, SP_DS_Node*>::iterator itMap;
		for (itMap = m_mNewTransitionNodes.begin(); itMap != m_mNewTransitionNodes.end(); ++itMap)
		{
			WriteUnfoldedTransition(itMap->second, l_pcElem);
		}
	}
	else
	{
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteNode((*l_Iter), l_pcElem);
		}
	}

	return true;
}

bool SP_ExportColFCPN2FCPN::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_plElements->size()));

	for (SP_ListEdge::const_iterator l_it = m_lNewEdges.begin(); l_it != m_lNewEdges.end(); l_it++)
	{
		if ((*l_it)->GetClassName() == l_sEdgeclassName)
			WriteEdge((*l_it), l_pcElem);
	}
	l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}
bool SP_ExportColFCPN2FCPN::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
	if (p_pcVal->GetSource())
		l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
	if (p_pcVal->GetTarget())
		l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));

	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		WriteAttribute((*l_Iter), l_pcElem);
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColFCPN2FCPN::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	wxString l_sName = p_pcVal->GetName();
	/**
	if(l_sName == SP_DS_META_CONSTANT)
	{
	SP_DS_Metadataclass* l_pcMC = m_pcExportGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_DS_ColListAttribute* l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(p_pcVal->GetElements()->front()->GetAttribute(wxT("ConstantList")));
	for(unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
	{
	wxString l_sName = l_pcColListAttr->GetCell(i,0);
	wxString l_sType = l_pcColListAttr->GetCell(i,1);
	wxString l_sValue = l_pcColListAttr->GetCell(i,2);

	if(l_sType == wxT("int"))
	{
	SP_DS_Metadata* l_pcConstant = l_pcMC->NewElement(1);
	l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(wxT("all"));
	l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(wxT("int"));
	l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);

	SP_DS_ColListAttribute* l_pcValueColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
	l_pcValueColListAttr->SetCell(0, 1, l_sValue);
	}
	}
	SP_XmlWriter::WriteMetadataclass(l_pcMC, p_pcRoot);
	}
	*/
	/**
	if (l_sName == wxT("Constant Class1"))//by george
	{
		SP_DS_Metadataclass* l_pcMC = m_pcExportGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	
		SP_DS_Metadataclass* l_pcCOnstants = p_pcVal;
		SP_ListMetadata::const_iterator l_itElem;
		l_itElem = l_pcCOnstants->GetElements()->begin();
		for (size_t l_nCon = 0; l_nCon < l_pcCOnstants->GetElements()->size(); l_nCon++)
		{
			SP_DS_Metadata* l_pcConstant = l_pcMC->NewElement(1);
			///l_pcConstant = *l_itElem;
			SP_DS_Metadata* l_pcConst;
			l_pcConst = *l_itElem;
			l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_pcConst->GetAttribute(wxT("Group"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_pcConst->GetAttribute(wxT("Type"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_pcConst->GetAttribute(wxT("Name"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_pcConst->GetAttribute(wxT("Comment"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_pcConst->GetAttribute(wxT("Name"))->GetValueString());
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			SP_DS_ColListAttribute * l_pcSourceColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConst->GetAttribute(wxT("ValueList")));


			l_pcColList->Clear();
			//l_pcColList->SetCell(0, 0, wxT("Main"));
			//l_pcColList->SetCell(0, 1, l_pcSourceColList->GetCell(0, 1));

			for (size_t l_nRow = 0; l_nRow < l_pcSourceColList->GetRowCount(); l_nRow++)
			{
				int l_nRowCol = l_pcColList->AppendEmptyRow();
				l_pcColList->SetCell(l_nRowCol, 0, l_pcSourceColList->GetCell(l_nRowCol, 0));
				l_pcColList->SetCell(l_nRowCol, 1, l_pcSourceColList->GetCell(l_nRowCol, 1));
			}
			++l_itElem;
		}
		
		SP_XmlWriter::WriteMetadataclass(l_pcMC, p_pcRoot);
	}
	*/
	 if (l_sName == SP_DS_META_FUNCTION)
	{
		//TODO
	}
	else if (m_pcExportGraph->GetMetadataclass(l_sName))
	{
		if (l_sName != wxT("Constant Class1"))//this line by george
			SP_XmlWriter::WriteMetadataclass(p_pcVal, p_pcRoot);
	}
	return true;
}

bool SP_ExportColFCPN2FCPN::WriteUnfoldedPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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
		if ((*l_Iter)->GetName() == wxT("MarkingList"))
		{
		}
		else
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColFCPN2FCPN::WriteUnfoldedTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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
