//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/04/23 12:20:00 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPT2Structure.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

SP_ExportColPT2Structure::SP_ExportColPT2Structure()
{	
}

SP_ExportColPT2Structure::~SP_ExportColPT2Structure()
{
}

bool SP_ExportColPT2Structure::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	m_sNetClassName  = cName;
	return (cName == SP_DS_COLPN_CLASS ||
		    cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS	);
}

bool SP_ExportColPT2Structure::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);
	return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

}

bool SP_ExportColPT2Structure::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	wxString l_sExportNetClassName;
	if(m_sNetClassName == SP_DS_COLPN_CLASS )
	{
		l_sExportNetClassName = SP_DS_PN_CLASS;
	}
	if( m_sNetClassName == SP_DS_COLEPN_CLASS )
	{
		l_sExportNetClassName = SP_DS_EXTPN_CLASS;
	}
	if(	m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		l_sExportNetClassName = SP_DS_SPN_CLASS;
	}
	if( m_sNetClassName == SP_DS_COLCPN_CLASS )
	{
		l_sExportNetClassName = SP_DS_CONTINUOUSPED_CLASS;
	}
	l_pcElem->AddAttribute(wxT("name"), l_sExportNetClassName);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportColPT2Structure::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot )
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
	

	if( wxT("Place") == l_sNodeclassName || SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WritePlace( *l_Iter, l_pcElem);		
		}
	}

	//-----------------------------------------------------------------------//

	else if( wxT("Transition") == l_sNodeclassName ||
		     SP_DS_IMMEDIATE_TRANS == l_sNodeclassName ||
			 SP_DS_DETERMINISTIC_TRANS == l_sNodeclassName ||
			 SP_DS_SCHEDULED_TRANS == l_sNodeclassName ||
			 SP_DS_CONTINUOUS_TRANS == l_sNodeclassName )
	{			
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteTransition( *l_Iter, l_pcElem);		
		}
	}

	//-----------------------------------------------------------------------//

	else
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteNode( ( *l_Iter ), l_pcElem );
		}
	}	

	return true;
}

bool SP_ExportColPT2Structure::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	

	SP_ListEdge::const_iterator l_Iter;
	const SP_ListEdge* l_plElements = p_pcVal->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);
	wxString l_sEdgeclassName = p_pcVal->GetName();
	//


	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
	l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));	
	
	for(SP_ListEdge::const_iterator l_it = l_plElements->begin();	l_it != l_plElements->end();l_it++)	
	{
		// also do not write the edeges of the original color set		
		if( (*l_it)->GetClassName() == l_sEdgeclassName)
			WriteEdge((*l_it), l_pcElem);
	}

	l_pcElem->AddAttribute(wxT("name"), l_sEdgeclassName);
	p_pcRoot->AddChild(l_pcElem);



	return TRUE;
}
bool SP_ExportColPT2Structure::WriteEdge(SP_DS_Edge* p_pcVal,wxXmlNode* p_pcRoot)
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

bool SP_ExportColPT2Structure::WritePlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{	
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
    p_pcRoot->AddChild(l_pcElem);

	SP_DS_ColListAttribute* l_pcColList = NULL;
	wxString l_sMarking = wxT("");

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (( *l_Iter )->GetName() == wxT("Marking") )
			l_sMarking = ( *l_Iter )->GetValueString();
	}

	for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
	{
		if (( *l_Iter )->GetName() == wxT("MarkingList") && m_sNetClassName == SP_DS_COLPN_CLASS)
		{
		}	
		else if (( *l_Iter )->GetName() == wxT("MarkingList") && m_sNetClassName == SP_DS_COLEPN_CLASS)
		{
		}	
		else if ((( *l_Iter )->GetName() == wxT("MarkingList") && m_sNetClassName == SP_DS_COLSPN_CLASS) ||
			(( *l_Iter )->GetName() == wxT("MarkingList") && m_sNetClassName == SP_DS_COLCPN_CLASS))
		{
			l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( *l_Iter ) ;
			l_pcColList->Clear();
			l_pcColList->SetColLabel( 0, wxT("Marking set") );
			l_pcColList->SetColLabel( 1, wxT("Marking") );
			int l_nNewRow = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
			l_pcColList->SetCell(l_nNewRow, 1, l_sMarking);

			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
		
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColPT2Structure::WriteTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
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



wxString SP_ExportColPT2Structure::GetName()
{ 
	wxString l_sName;
	if(m_sNetClassName == SP_DS_COLPN_CLASS )
	{
		l_sName = wxT("Export the structure of a colored Petri Net");
	}
	if( m_sNetClassName == SP_DS_COLEPN_CLASS )
	{
		l_sName = wxT("Export the structure of a colored extended Petri Net");
	}
	if(	m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		l_sName = wxT("Export the structure of a colored stochastic Petri Net");
	}
	if( m_sNetClassName == SP_DS_COLCPN_CLASS )
	{
		l_sName = wxT("Export the structure of a colored continuous Petri Net");
	}	

	return l_sName;
}

wxString SP_ExportColPT2Structure::GetDescr()
{ 
	wxString l_sName;
	if(m_sNetClassName == SP_DS_COLPN_CLASS )
	{
		l_sName = wxT("Export an Colored Petri Net into Petri Net Format"); 
	}
	if( m_sNetClassName == SP_DS_COLEPN_CLASS )
	{
		l_sName = wxT("Export an Colored Petri Net into Extended Petri Net Format"); 
	}
	if(	m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		l_sName = wxT("Export an Colored Petri Net into Stochastic Petri Net Format"); 
	}
	if( m_sNetClassName == SP_DS_COLCPN_CLASS )
	{
		l_sName = wxT("Export an Colored Petri Net into Continuous Petri Net Format"); 
	}
	return l_sName; 
}

wxString SP_ExportColPT2Structure::GetExtension()
{ 
	wxString l_sExtension;
	if(m_sNetClassName == SP_DS_COLPN_CLASS )
	{
		l_sExtension = wxT("pn"); 
	}
	if( m_sNetClassName == SP_DS_COLEPN_CLASS )
	{
		l_sExtension = wxT("xpn"); 
	}
	if(	m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		l_sExtension = wxT("spn"); 
	}
	if( m_sNetClassName == SP_DS_COLCPN_CLASS )
	{
		l_sExtension = wxT("cpn"); 
	}
	return l_sExtension; 
}

wxString SP_ExportColPT2Structure::GetFileDescr()
{ 
	if(m_sNetClassName == SP_DS_COLPN_CLASS )
	{
	}
	if( m_sNetClassName == SP_DS_COLEPN_CLASS )
	{
	}
	if(	m_sNetClassName == SP_DS_COLSPN_CLASS )
	{
	}
	if( m_sNetClassName == SP_DS_COLCPN_CLASS )
	{
	}
	return wxT("PT Net Files"); 
}
