//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/22 12:05:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportContPed2PedConvert.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"

bool SP_ExportContPed2PedConvert::Write(SP_MDI_Doc* p_doc,
		const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	SP_LOGWARNING(wxT("Tokens and Multiplicities are converted to the smallest integer value not less than value!"));

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();
	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportContPed2PedConvert::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"),SP_DS_PN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportContPed2PedConvert::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
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
	l_pcElem->AddAttribute(wxT("count"),wxString::Format( wxT("%d"), l_plElements->size()));
	if (SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Place");
		l_pcElem->AddAttribute(wxT("name"), l_pcNodeclassName);
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WritePlace((*l_Iter), l_pcElem);
		}
		return true;
	}
	else if (SP_DS_CONTINUOUS_TRANS == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Transition");
	}
	else if (SP_DS_PARAM == l_sNodeclassName)
	{
		return TRUE;
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
		return TRUE;
	}
	else if (wxT("Comment") == l_sNodeclassName)
	{
		l_pcNodeclassName = wxT("Comment");
	}
	else
	{
		return TRUE;
	}
	l_pcElem->AddAttribute(wxT("name"),l_pcNodeclassName);
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		WriteNode((*l_Iter), l_pcElem);

	return TRUE;
}

bool
SP_ExportContPed2PedConvert::WritePlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
	  if ((*l_Iter)->GetName() != wxT("Fixed") )
	  {
		  WriteAttribute( ( *l_Iter ), l_pcElem );
	  }
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportContPed2PedConvert::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();
	wxString l_pcEdgeclassName;

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	if (SP_DS_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = wxT("Edge");
		l_pcElem->AddAttribute(wxT("count"),wxString::Format( wxT("%d"), l_plElements->size()));

		l_pcElem->AddAttribute(wxT("name"),l_pcEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
			WriteEdge((*l_Iter), l_pcElem);
	}
	else if (SP_DS_INHIBITOR_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = l_sEdgeclassName;
	}
	else if (SP_DS_READ_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = wxT("Edge");
		l_pcElem->AddAttribute(wxT("count"),wxString::Format( wxT("%d"), l_plElements->size() * 2));

		l_pcElem->AddAttribute(wxT("name"),l_pcEdgeclassName);
		p_pcRoot->AddChild(l_pcElem);

		for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
		{
			WriteEdge((*l_Iter), l_pcElem);
			WriteOppositeEdge((*l_Iter), l_pcElem);
		}
	}

	return TRUE;
}

bool SP_ExportContPed2PedConvert::WriteEdge(SP_DS_Edge* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER(l_plAttributes, return FALSE);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
	if (p_pcVal->GetSource())
		l_pcElem->AddAttribute(wxT("source"),wxString::Format( wxT("%ld"), p_pcVal->GetSource()->GetId()));
	if (p_pcVal->GetTarget())
		l_pcElem->AddAttribute(wxT("target"),wxString::Format( wxT("%ld"), p_pcVal->GetTarget()->GetId()));
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if ((*l_Iter)->GetName() == wxT("Equation"))
		{
			(*l_Iter)->SetName(wxT("Multiplicity"));
			WriteAttribute((*l_Iter), l_pcElem);
			(*l_Iter)->SetName(wxT("Equation"));
		}
		else
		{
			WriteAttribute((*l_Iter), l_pcElem);
		}
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportContPed2PedConvert::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (SP_DS_CONTINUOUSPED_CLASS == cName);
}

bool
SP_ExportContPed2PedConvert::WriteAttribute( SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot )
{
    CHECK_POINTER( p_pcVal, return FALSE );
    CHECK_POINTER( p_pcRoot, return FALSE );

    if( p_pcVal->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST )
    {
      return WriteColListAttribute( dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal ), p_pcRoot );
    }

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("attribute"));
    wxString l_sVal = p_pcVal->GetValueString();
    wxString l_sName = p_pcVal->GetName();
    double l_nVal = 0;

    if((l_sName == wxT("Marking") || l_sName == wxT("Multiplicity"))
    		&& l_sVal.ToDouble(&l_nVal))
    {
    	unsigned l_newVal = std::ceil(l_nVal);
    	l_sVal = wxString::Format(wxT("%u"), l_newVal);
    }

    l_pcElem->AddAttribute(wxT("name"), l_sName);
    p_pcRoot->AddChild(l_pcElem);

    wxXmlNode* l_pcCDATA = new wxXmlNode(NULL, wxXML_CDATA_SECTION_NODE, wxT(""), l_sVal);
    l_pcElem->AddChild(l_pcCDATA);

    return WriteData( p_pcVal, l_pcElem );

}

bool SP_ExportContPed2PedConvert::WriteMetadata(SP_DS_Metadata *p_pcVal, wxXmlNode *p_pcRoot) {
	if(p_pcVal->GetClassName() == SP_DS_META_CONSTANT)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
		if(l_pcAttr)
		{
			if(l_pcAttr->GetValueString() != wxT("int"))
				return false;
		}
	}
	else if(p_pcVal->GetClassName() == SP_DS_META_FUNCTION)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Return"));
		if(l_pcAttr)
		{
			if(l_pcAttr->GetValueString() != wxT("int"))
				return false;
		}
	}

	return SP_XmlWriter::WriteMetadata(p_pcVal, p_pcRoot);
}
