//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColExtPT2ExtPT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_ExportUnfolding.h"

SP_ExportColExtPT2ExtPT::SP_ExportColExtPT2ExtPT()
{	
}

SP_ExportColExtPT2ExtPT::~SP_ExportColExtPT2ExtPT()
{
}

bool SP_ExportColExtPT2ExtPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLEPN_CLASS);
}

bool SP_ExportColExtPT2ExtPT::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	SP_DS_ColPN_ExportUnfolding l_UnfoldingforExport;	
	if( !l_UnfoldingforExport.Unfolding() )
		return false;

	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(SP_DS_SPN_CLASS);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();
	m_pcExportGraph = new SP_DS_Graph(newClass);

	m_mNewPlaceNodes = l_UnfoldingforExport.GetNewDiscretePlaceNodes();
	m_mNewTransitionNodes = l_UnfoldingforExport.GetNewDiscreteTransitionNodes();
	m_lNewEdges = l_UnfoldingforExport.GetNewEdges();

	bool l_bSuccess = SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

	l_UnfoldingforExport.Reset(); // Reset the net 

	wxDELETE(m_pcExportGraph);

	return l_bSuccess;
}

bool SP_ExportColExtPT2ExtPT::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_EXTPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportColExtPT2ExtPT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );	

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
	l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));

	l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
	p_pcRoot->AddChild( l_pcElem );
	
	if( wxT("Place") == l_sNodeclassName )
	{	
		map<wxString, SP_DS_Node*>::iterator itMap;	
		for ( itMap = m_mNewPlaceNodes.begin(); itMap != m_mNewPlaceNodes.end(); ++itMap )		
		{				
			WriteUnfoldedPlace( itMap->second, l_pcElem);			
		}
	}
	else if( wxT("Transition") == l_sNodeclassName )
	{		
		map<wxString, SP_DS_Node*>::iterator itMap;
		for ( itMap = m_mNewTransitionNodes.begin(); itMap != m_mNewTransitionNodes.end(); ++itMap )
		{				
			WriteUnfoldedTransition( itMap->second, l_pcElem);					
		}		
	}	
	else
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteNode( ( *l_Iter ), l_pcElem );
		}
	}	

	return true;
}

bool SP_ExportColExtPT2ExtPT::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{	
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));	

	for(SP_ListEdge::const_iterator l_it = m_lNewEdges.begin();	l_it != m_lNewEdges.end();l_it++)	
	{
		if( (*l_it)->GetClassName() == l_sEdgeclassName)
			WriteEdge((*l_it), l_pcElem);
	}

	l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportColExtPT2ExtPT::WriteEdge(SP_DS_Edge* p_pcVal,wxXmlNode* p_pcRoot)
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

bool SP_ExportColExtPT2ExtPT::WriteMetadataclass( SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	wxString l_sName = p_pcVal->GetName();
	if(l_sName == SP_DS_META_CONSTANT)
	{
		//TODO
	}
	else if(l_sName == SP_DS_META_FUNCTION)
	{
		//TODO
	}
	else if(m_pcExportGraph->GetMetadataclass(l_sName))
	{
		SP_XmlWriter::WriteMetadataclass(p_pcVal, p_pcRoot);
	}
	return true;
}

bool SP_ExportColExtPT2ExtPT::WriteUnfoldedPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{	
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
    p_pcRoot->AddChild(l_pcElem);

	wxString l_sMarking = wxT("");

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (( *l_Iter )->GetName() == wxT("MarkingList"))
		{
		}		
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColExtPT2ExtPT::WriteUnfoldedTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{	
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
    p_pcRoot->AddChild(l_pcElem);

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
	}  

	return WriteData(p_pcVal, l_pcElem);
}

