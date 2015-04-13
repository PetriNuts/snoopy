//////////////////////////////////////////////////////////////////////
// $Author: F. Liu $
// $Version: 0.2 $
// $Date: 2010/10/23 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColHPN2ColCPN.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"


bool SP_ExportColHPN2ColCPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportColHPN2ColCPN::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	m_nTransitionNumber = m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size();
	m_nTransitionNumber += m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();
	m_nTransitionNumber += m_graph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size();
	m_nTransitionNumber += m_graph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size();
    m_nTransitionNumber += m_graph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size();

	m_nPlaceNumber =  m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceNumber += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	m_pcTransitionNodeclass = NULL;
	m_pcPlaceNodeclass = NULL;


	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportColHPN2ColCPN::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLCPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}


bool SP_ExportColHPN2ColCPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );	

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();

	if (wxT("Place") == l_sNodeclassName ||
		SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		if(!m_pcPlaceNodeclass)
		{
			l_sNodeclassName = SP_DS_CONTINUOUS_PLACE;
			wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));
			m_pcPlaceNodeclass = l_pcElem;	

			l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"),  m_nPlaceNumber));
			l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
			p_pcRoot->AddChild(l_pcElem);
		}

		wxXmlNode* l_pcElem = m_pcPlaceNodeclass;		
		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WritePlace( ( *l_Iter ), l_pcElem);
		}
	}	
	else if (wxT("Transition") == l_sNodeclassName ||
		     wxT("Immediate Transition") == l_sNodeclassName||
			 wxT("Deterministic Transition") == l_sNodeclassName||
			 wxT("Scheduled Transition") == l_sNodeclassName||
			 SP_DS_CONTINUOUS_TRANS == l_sNodeclassName)
	{
		if(!m_pcTransitionNodeclass)
		{
			l_sNodeclassName = SP_DS_CONTINUOUS_TRANS;
			wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("nodeclass"));		
			m_pcTransitionNodeclass = l_pcElem;	

			l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), m_nTransitionNumber));
			l_pcElem->AddAttribute( wxT("name"),  l_sNodeclassName );
			p_pcRoot->AddChild(l_pcElem);
		}

	    wxXmlNode* l_pcElem = m_pcTransitionNodeclass;		
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

bool SP_ExportColHPN2ColCPN::WritePlace(SP_DS_Node* p_pcVal,
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

bool SP_ExportColHPN2ColCPN::WriteTransition(SP_DS_Node* p_pcVal,
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

bool SP_ExportColHPN2ColCPN::WriteParameter(SP_DS_Node* p_pcVal,
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



bool SP_ExportColHPN2ColCPN::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();
	m_edgeClass = p_pcVal->GetName();

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

		if( wxT("Read Edge") == l_sEdgeclassName||
			wxT("Reset Edge")== l_sEdgeclassName )
		{
			l_sEdgeclassName = SP_DS_READ_EDGE;
		}

		l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);
	}

	return TRUE;
}

bool
SP_ExportColHPN2ColCPN::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
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



bool
SP_ExportColHPN2ColCPN::WriteEdgeGraphic(SP_GR_Edge* p_pcVal,
				     wxXmlNode* p_pcRoot)
{
  if (m_edgeClass != wxT("Edge")) 
	  m_changeColour = true;
  bool res = SP_XmlWriter::WriteEdgeGraphic(p_pcVal, p_pcRoot);
  m_changeColour = false;

  return res;
}

bool
SP_ExportColHPN2ColCPN::WriteColourInformation(SP_Graphic* p_pcVal,
					   wxXmlNode* p_pcNode)
{
  CHECK_POINTER(p_pcVal, return FALSE);
  CHECK_POINTER(p_pcNode, return FALSE);

  if (m_changeColour) 
  { // write our own colors

   if( m_edgeClass == wxT("Edge") )
	   return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);

    if (m_edgeClass == wxT("Read Edge") )
	       return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);

	if(m_edgeClass == wxT("Inhibitor Edge") )
	     return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);

    wxString colour;
	
	if (m_edgeClass == wxT("Reset Edge")) 
	{
      colour=wxT("255,0,255");
    } 
	if (m_edgeClass == wxT("Equal Edge")) 
	{
      colour=wxT("255,220,0");
    }
    p_pcNode->AddAttribute(wxT("pen"), colour.c_str());
    p_pcNode->AddAttribute(wxT("brush"), colour.c_str());
  } 
   else 
  {
    return SP_XmlWriter::WriteColourInformation(p_pcVal, p_pcNode);
  }
  return true;
}


