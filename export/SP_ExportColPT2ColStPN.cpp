//////////////////////////////////////////////////////////////////////
// $Author: fei $
// $Version: 0.1 $
// $Date: 2009/04/23 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPT2ColStPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

bool SP_ExportColPT2ColStPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS);
}

bool SP_ExportColPT2ColStPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool SP_ExportColPT2ColStPN::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLSPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportColPT2ColStPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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

bool SP_ExportColPT2ColStPN::WriteStochPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );


//	SP_DS_ColListAttribute* l_pcColList;

//	wxString l_sMarking = wxT("");

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
		//if( (*l_Iter)->GetName() == wxT("Marking") )
		{
		//	l_sMarking = ( *l_Iter )->GetValueString();
		}
	}
/*
	l_pcColList = new SP_DS_ColListAttribute( wxT("MarkingList"), SP_COLLIST_UNSIGNED_INTEGER, 2 );

	l_pcColList->SetColLabel( 0, wxT("Marking set") );
	l_pcColList->SetColLabel( 1, wxT("Marking") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, l_sMarking);

	WriteAttribute( l_pcColList, l_pcElem );
*/
	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportColPT2ColStPN::WriteStochTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

//	SP_DS_ColListAttribute* l_pcColList;
//	SP_Graphic* l_pcGrAttr;

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
	}
/*
	l_pcColList = new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_STRING, 2 );
	l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_StFunctionAttribute( l_pcColList ) );

	l_pcGrAttr->SetOffsetY( 30 );
	l_pcGrAttr->SetShow( FALSE );

	l_pcColList->SetGlobalShow();

	l_pcColList->SetColLabel( 0, wxT("Function set") );
	l_pcColList->SetColLabel( 1, wxT("Function") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT("1"));

	WriteAttribute( l_pcColList, l_pcElem );
*/
	return WriteData(p_pcVal, l_pcElem);
}

