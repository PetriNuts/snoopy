//////////////////////////////////////////////////////////////////////
// $Author: fei $
// $Version: 0.1 $
// $Date: 2009/04/23 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPT2ColExtPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

bool SP_ExportColPT2ColExtPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS);
}

bool SP_ExportColPT2ColExtPT::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool SP_ExportColPT2ColExtPT::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLEPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportColPT2ColExtPT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteStochPlace( ( *l_Iter ), l_pcElem );
		}
	}
	else if( wxT("Transition") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteStochTransition( ( *l_Iter ), l_pcElem );
		}
	}
	else
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteNode( ( *l_Iter ), l_pcElem );
		}
	}

	return TRUE;
}

bool SP_ExportColPT2ColExtPT::WriteStochPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );		
	}

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColPT2ColExtPT::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );


	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
	}

	return WriteData(p_pcVal, l_pcElem);
}

