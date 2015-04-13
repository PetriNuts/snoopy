//////////////////////////////////////////////////////////////////////
// $Author: slehrack&Liu $
// $Version: 0.2 $
// $Date: 2009/3/13 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportStochPed2ExtPT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include <algorithm>

bool SP_ExportStochPed2ExtPT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();	

	if((wxT("Transition") == l_sNodeclassName ))
	{
		
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));		
		m_pcTransitionNodeclass = l_pcElem;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran+m_nScheduledTran;
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), m_nTransitionNumber));
		l_pcElem->AddAttribute( wxT("name"),  wxT("Transition" ));
		p_pcRoot->AddChild(l_pcElem);
		m_nTransitionNumber=0;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}

	}		     
    else if((wxT("Immediate Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
			
		m_nTransitionNumber=m_nTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
			  
    else if((wxT("Deterministic Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
			  
	else if((wxT("Scheduled Transition") == l_sNodeclassName ) )
	{
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	} 
	else
	{
		
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));		
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteNode( ( *l_Iter ), l_pcElem);
		}
	}

	return TRUE;
}

bool
SP_ExportStochPed2ExtPT::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListMetadata::const_iterator l_Iter;
	const SP_ListMetadata* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sMetadataclassName = p_pcVal->GetName();

	if((l_sMetadataclassName == SP_DS_META_CONSTANT))
	{
		wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("metadataclass"));
		l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%d"), l_plElements->size()));
		l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter) {

			if (( *l_Iter )->GetAttribute(wxT("Type"))->GetValueString() == wxT("int") )
				WriteMetadata((*l_Iter), l_pcElem);
		}

	}

	return TRUE;

}

//Added by Laarz 16.10.2012
bool
SP_ExportStochPed2ExtPT::WriteMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return false );
	CHECK_POINTER( p_pcRoot, return false );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("metadata"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
		WriteAttribute((*l_Iter), l_pcElem);



	return WriteData(p_pcVal, l_pcElem);

}


bool SP_ExportStochPed2ExtPT::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);
	m_pcTransitionNodeclass = NULL;
	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	/////By Liu
	 m_nTran = m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size();
	 m_nImmediateTran = m_graph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size();
	 m_nDeterministicTran = m_graph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size();
	 m_nScheduledTran = m_graph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size();		 
	/////
	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportStochPed2ExtPT::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_EXTPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportStochPed2ExtPT::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();

	if (wxT("Modifier Edge") != l_sEdgeclassName )
	{
		SP_XmlWriter::WriteEdgeclass(p_pcVal, p_pcRoot);
	}

	return TRUE;
}

bool SP_ExportStochPed2ExtPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}


bool SP_ExportStochPed2ExtPT::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{    
	CHECK_POINTER(p_pcVal, return FALSE);    
	CHECK_POINTER(p_pcRoot, return FALSE);    
		
	SP_ListAttribute::const_iterator l_Iter;	
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();	
	CHECK_POINTER( l_plAttributes, return FALSE );	
	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));	
	p_pcRoot->AddChild(l_pcElem);	

	/* old solution
	 * Changed by Laarz 16.10.2012
	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)	
	{
		if (( *l_Iter )->GetName() == wxT("FunctionList"))
		{

		}
		else if (( *l_Iter )->GetName() == wxT("DelayList"))
		{

		}
		else if (( *l_Iter )->GetName() == wxT("PeriodicList"))
		{

		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}
	*/

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
		{
			if ((( *l_Iter )->GetName() == wxT("FunctionList")) ||
					(( *l_Iter )->GetName() == wxT("Delay")) ||
					(( *l_Iter )->GetName() == wxT("Weight")) ||
					(( *l_Iter )->GetName() == wxT("Start")) ||
					(( *l_Iter )->GetName() == wxT("Repetition")) ||
					(( *l_Iter )->GetName() == wxT("End")) )
			{

			}
			else
			{
				WriteAttribute( ( *l_Iter ), l_pcElem);
			}
		}
	return WriteData(p_pcVal, l_pcElem);
}
