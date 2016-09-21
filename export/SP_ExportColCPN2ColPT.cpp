//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/09/2 11:40:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportColCPN2ColPT.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/gra_SPN/SP_GR_StParameterListAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"

#include "sp_revision.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"

bool SP_ExportColCPN2ColPT::Write(SP_MDI_Doc* p_doc,
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

bool SP_ExportColCPN2ColPT::WriteNetclass(SP_DS_Netclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

bool SP_ExportColCPN2ColPT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal,
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
	l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
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
		return true;
	}
	else if (SP_DS_COARSE_PLACE == l_sNodeclassName)
	{
		l_pcNodeclassName = SP_DS_COARSE_PLACE;
	}
	else if (SP_DS_COARSE_TRANSITION == l_sNodeclassName)
	{
		l_pcNodeclassName = SP_DS_COARSE_TRANSITION;
	}
	else if (SP_DS_COARSE_PARAMETER == l_sNodeclassName)
	{
		return true;
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
SP_ExportColCPN2ColPT :: WriteStochPlace( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{

  CHECK_POINTER( p_pcVal, return FALSE );
  CHECK_POINTER( p_pcRoot, return FALSE );
  

  SP_ListAttribute::const_iterator l_Iter;
  const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
  CHECK_POINTER( l_plAttributes, return FALSE );

  wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
  p_pcRoot->AddChild( l_pcElem );

   SP_DS_ColListAttribute* l_pcColList = NULL;
  SP_DS_Attribute* l_pcMarking = NULL;


  	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		SP_DS_Attribute* l_pcAttribute = *l_Iter;

		if ((*l_Iter)->GetName() == wxT("Fixed") )
		{
			continue;
		}

		if (( *l_Iter )->GetName() == SP_DS_CPN_MARKINGLIST)
		{			
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttribute);
			continue;
		}
		
		if (( *l_Iter )->GetName() == wxT("Marking"))
		{			
			l_pcMarking = l_pcAttribute;
			continue;			
		}

		WriteAttribute( ( *l_Iter ), l_pcElem);	
	}

	unsigned int l_nActiveColumn =  l_pcColList->GetActiveColumn();
	if( l_nActiveColumn >=  l_pcColList->GetColCount() )
	{
		l_nActiveColumn = 0;
	}
	vector<wxString> l_vFirstColumn;
	for(unsigned i = 0; i < l_pcColList->GetRowCount(); i++)
	{	
		l_vFirstColumn.push_back(l_pcColList->GetCell(i,1));		
		l_pcColList->SetCell(i, 1,  l_pcColList->GetCell(i,l_nActiveColumn));
	}
	WriteAttribute( l_pcColList, l_pcElem );

	//
	for(unsigned i = 0; i < l_vFirstColumn.size(); i++)
	{			
		l_pcColList->SetCell(i, 1,  l_vFirstColumn[i]);
	}

	wxString l_sMarking = l_pcMarking->GetValueString();
    l_pcMarking->SetValueString( l_pcColList->GetCell(0,l_nActiveColumn) );
	WriteAttribute( l_pcMarking, l_pcElem );
	l_pcMarking->SetValueString(l_sMarking );	

  return WriteData(p_pcVal, l_pcElem);

}

bool
SP_ExportColCPN2ColPT :: WriteStochTransition( SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
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
	  if( ! ((*l_Iter)->GetName() == wxT("Reversible") || ( *l_Iter )->GetName() == wxT("FunctionList") ) )
	  {		  
			WriteAttribute( ( *l_Iter ), l_pcElem );		  
	  }	 
  }

  return WriteData(p_pcVal, l_pcElem);

}




bool SP_ExportColCPN2ColPT::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal,
		wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();
	wxString l_pcEdgeclassName;

	if (SP_DS_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = wxT("Edge");
	}
	else if (SP_DS_INHIBITOR_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = wxT("Edge");
	}
	else if (SP_DS_READ_EDGE == l_sEdgeclassName)
	{
		l_pcEdgeclassName = wxT("Edge");
	}
	else if (wxT("Modifier Edge") == l_sEdgeclassName)
	{
		return true;
	}
	else
	{
		return true;
	}
	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));

	l_pcElem->AddAttribute(wxT("name"), l_pcEdgeclassName);
	p_pcRoot->AddChild(l_pcElem);

	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
	{
		WriteEdge((*l_Iter), l_pcElem);
	}

	return TRUE;
}

bool
SP_ExportColCPN2ColPT::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportColCPN2ColPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (SP_DS_COLCPN_CLASS == cName);
}

