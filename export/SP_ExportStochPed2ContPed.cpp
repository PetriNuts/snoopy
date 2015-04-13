//////////////////////////////////////////////////////////////////////
// $Author: slehrack $
// $Version: 0.1 $
// $Date: 2007/12/01 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportStochPed2ContPed.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

bool SP_ExportStochPed2ContPed::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (wxT("Place") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_PLACE;
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WritePlace( ( *l_Iter ), l_pcElem);
		}
	}
	else if (wxT("Transition") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));		
		m_pcTransitionNodeclass = l_pcElem;

		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran+m_nScheduledTran;

	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), m_nTransitionNumber));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		m_nTransitionNumber=0;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if((wxT("Immediate Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;			
		m_nTransitionNumber=m_nTran;

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if((wxT("Deterministic Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	}
			  
	else if((wxT("Scheduled Transition") == l_sNodeclassName ) )
	{
		l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
		wxXmlNode* l_pcElem = m_pcTransitionNodeclass;
		m_nTransitionNumber=m_nTran+m_nImmediateTran+m_nDeterministicTran;
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteTransition( ( *l_Iter ), l_pcElem);
		}
	} 
	else if (wxT("Parameter") == l_sNodeclassName)
	{
		l_sNodeclassName = SP_DS_PARAM;

		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteParameter( ( *l_Iter ), l_pcElem);
		}
	}
	else if((wxT("Coarse Parameter") == l_sNodeclassName ) ||
			(wxT("Coarse Place") == l_sNodeclassName ) ||
			(wxT("Coarse Transition") == l_sNodeclassName )	||
			(wxT("Comment") == l_sNodeclassName ))
	{
		if (wxT("Coarse Parameter") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_PARAMETER;
		}
		else if (wxT("Coarse Place") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_PLACE;
		}
		else if (wxT("Coarse Transition") == l_sNodeclassName)
		{
			l_sNodeclassName = SP_DS_COARSE_TRANSITION;
		}
		else if (wxT("Comment") == l_sNodeclassName)
		{
			l_sNodeclassName = wxT("Comment");
		}

		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
	    l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  l_plElements->size()));
		l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteNode( ( *l_Iter ), l_pcElem);
		}
	}

	return TRUE;

}

bool SP_ExportStochPed2ContPed::WritePlace(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

			WriteAttribute( ( *l_Iter ), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportStochPed2ContPed::WriteTransition(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{

	    if (( *l_Iter )->GetName() == wxT("DelayList"))
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
	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportStochPed2ContPed::WriteParameter(SP_DS_Node* p_pcVal,
		wxXmlNode* p_pcRoot)
{

	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{


		WriteAttribute( ( *l_Iter ), l_pcElem);

	}

	return WriteData(p_pcVal, l_pcElem);

}

bool SP_ExportStochPed2ContPed::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

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

bool SP_ExportStochPed2ContPed::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_CONTINUOUSPED_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportStochPed2ContPed::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();

	if (wxT("Edge") == l_sEdgeclassName ||
		wxT("Inhibitor Edge") == l_sEdgeclassName ||
		wxT("Read Edge") == l_sEdgeclassName||
		wxT("Equal Edge") == l_sEdgeclassName||
		wxT("Reset Edge")== l_sEdgeclassName||
		wxT("Modifier Edge") == l_sEdgeclassName)
	{
		wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
		l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
		}

		if(wxT("Edge") == l_sEdgeclassName||
			wxT("Equal Edge") == l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_EDGE;
		}
		else if(wxT("Inhibitor Edge") == l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_INHIBITOR_EDGE;
		}
		else if( wxT("Read Edge") == l_sEdgeclassName||
			wxT("Reset Edge")== l_sEdgeclassName)
		{
			l_sEdgeclassName = SP_DS_READ_EDGE;
		}
		else if(wxT("Modifier Edge") == l_sEdgeclassName)
		{

		}
		l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);
	}

	return TRUE;
}

bool
SP_ExportStochPed2ContPed::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format( wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format( wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
    {
      WriteAttribute((*l_Iter), l_pcElem);
    }

    return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportStochPed2ContPed::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}

