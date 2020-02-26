//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2009/11/9 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportPed2ColPT.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"




#include "sp_gr/attributes/SP_GR_ColListAttribute.h"



#include "sp_core/SP_Core.h"




bool SP_ExportPed2ColPT::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	m_pcGraph = p_doc->GetGraph();
	return (cName == SP_DS_PN_CLASS);
}

bool SP_ExportPed2ColPT::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

    //Add color sets
	SP_DS_Metadataclass* l_pcMC;
	l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);   //first check if it exists
	
	if(l_pcMC)
		return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

	l_pcMC = m_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( m_pcGraph, SP_DS_CPN_BASICCOLORSETCLASS ) );	
	l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ColorsetList"), SP_COLLIST_UNSIGNED_INTEGER, 3 ) );
	SP_DS_Metadata* l_pcNewMetadata = l_pcMC->NewElement( 1 );		

	SP_DS_ColListAttribute* l_pcAttr;
	l_pcNewMetadata = *(l_pcMC->GetElements()->begin());
	l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));

	l_pcAttr->AppendEmptyRow();
	l_pcAttr->SetCell(0,0,wxT("Dot"));
	l_pcAttr->SetCell(0,1,wxT("dot"));
	l_pcAttr->SetCell(0,2,wxT("dot"));
	//end of color sets

	/////////////////george///////////////
	l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_DS_Metadataclass* l_pcNewConstants= m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	SP_DS_Metadata* l_pcConstant;
	SP_ListMetadata::const_iterator l_itElem;
	for (l_itElem = l_pcMC->GetElements()->begin(); l_itElem != l_pcMC->GetElements()->end(); l_itElem++)
	{
		SP_DS_Metadata* l_pcNewConstant = l_pcNewConstants->NewElement(1);
		l_pcConstant = *l_itElem;
		wxString l_sName = l_pcConstant->GetAttribute(wxT("Name"))->GetValueString();
		wxString l_sGroup = l_pcConstant->GetAttribute(wxT("Group"))->GetValueString();
		wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
		wxString l_sComment = l_pcConstant->GetAttribute(wxT("Comment"))->GetValueString();

		l_pcNewConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
		l_pcNewConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);
		l_pcNewConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
		l_pcNewConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);
	//	l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);

		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewConstant->GetAttribute(wxT("ValueList")));

		SP_DS_ColListAttribute * l_pcSourceColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
		l_pcColList->Clear();
		
		for (unsigned int i = 0; i <  l_pcSourceColList->GetRowCount(); i++)
		{
			int l_nRowCol = l_pcColList->AppendEmptyRow();
			l_pcColList->SetCell(l_nRowCol, 0, l_pcSourceColList->GetCell(l_nRowCol, 0));
			l_pcColList->SetCell(l_nRowCol, 1, l_pcSourceColList->GetCell(l_nRowCol, 1));
		}
		//++l_itElem;
	}

	/////////////////////////////////
	/**
	l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_DS_Metadata* l_pcConstant;
	SP_ListMetadata::const_iterator l_itElem;
	vector< vector<wxString> > l_vvValuelist;
	for(l_itElem = l_pcMC->GetElements()->begin(); l_itElem != l_pcMC->GetElements()->end(); l_itElem++)
	{
		vector<wxString> l_vValues;
		l_pcConstant = *l_itElem;
		wxString l_sName = l_pcConstant->GetAttribute(wxT("Name"))->GetValueString();
		wxString l_sGroup = l_pcConstant->GetAttribute(wxT("Group"))->GetValueString();
		wxString l_sType =  l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
		l_vValues.push_back(l_sName);
		l_vValues.push_back(l_sGroup);
		l_vValues.push_back(l_sType);

		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
		for(unsigned int i=0; i < l_pcColList->GetRowCount(); i++)
		{
			l_vValues.push_back(l_pcColList->GetCell(i, 0));
			l_vValues.push_back(l_pcColList->GetCell(i, 1));
		}	
		l_vvValuelist.push_back(l_vValues);
	}	
	

	
	//create new elements and assign values
	l_pcMC->RemoveAll();
	l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ConstantList"), SP_COLLIST_UNSIGNED_INTEGER, 4 ) );
	l_pcNewMetadata = l_pcMC->NewElement( 1 );		
	l_pcNewMetadata = *(l_pcMC->GetElements()->begin());
	l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));

	for(int i=0; i<l_vvValuelist.size();i++)
	{
		l_pcAttr->AppendEmptyRow();
		vector<wxString> l_vValues = l_vvValuelist[i];
	
		l_pcAttr->SetCell(i,0,l_vValues[0]);
		l_pcAttr->SetCell(i,1,l_vValues[2]);
		l_pcAttr->SetCell(i,2,l_vValues[4]);
	}

	*/
	//end of constants


	return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);
}

bool SP_ExportPed2ColPT::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);
	

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportPed2ColPT::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
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
			WriteExtPlace( ( *l_Iter ), l_pcElem );
		}
	}
	else if( wxT("Transition") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteExtTransition( ( *l_Iter ), l_pcElem );
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



bool
SP_ExportPed2ColPT::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    

    SP_ListEdge::const_iterator l_Iter;
    const SP_ListEdge* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteExtEdge((*l_Iter), l_pcElem);

    return TRUE;
}

bool SP_ExportPed2ColPT::WriteExtPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;

	wxString l_sMarking = wxT("");
	

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
		if( (*l_Iter)->GetName() == wxT("Marking") )
		{
			l_sMarking = ( *l_Iter )->GetValueString();
		}
	}

	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_MARKINGLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );

	l_pcColList->SetColLabel( 0, wxT("Color") );
	l_pcColList->SetColLabel( 1, wxT("Marking") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("dot"));
	l_pcColList->SetCell(l_nNewRow, 1, l_sMarking);

	SP_Graphic* l_pcGrAttr;
	l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_ColListAttribute( l_pcColList ) );
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25); 

	WriteAttribute( l_pcColList, l_pcElem );


	SP_DS_TextAttribute* l_pcNameAttribute = new SP_DS_TextAttribute(SP_DS_CPN_COLORSETNAME);
	l_pcNameAttribute->SetValue(wxT("Dot"));

	l_pcGrAttr = l_pcNameAttribute->AddGraphic(new SP_GR_TextAttribute(l_pcNameAttribute));
	l_pcGrAttr->SetOffsetY(-20);

	WriteAttribute(l_pcNameAttribute, l_pcElem );

	return WriteData(p_pcVal, l_pcElem);
}

bool SP_ExportPed2ColPT::WriteExtTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );
	

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;
	//SP_Graphic* l_pcGrAttr;

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		WriteAttribute( ( *l_Iter ), l_pcElem );
	}

	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );
	//l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_StFunctionAttribute( l_pcColList ) );

	//l_pcGrAttr->SetOffsetY( 30 );
	//l_pcGrAttr->SetShow( FALSE );

	//l_pcColList->SetGlobalShow();

	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	SP_Graphic* l_pcGrAttr;
	l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
	l_pcGrAttr->SetOffsetX(45);
	l_pcGrAttr->SetOffsetY(-25); 

	WriteAttribute( l_pcColList, l_pcElem );

	return WriteData(p_pcVal, l_pcElem);
}


bool SP_ExportPed2ColPT::WriteExtEdge( SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
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

	
	wxString l_sMultiplicity = wxT("1");
	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{		
		if( (*l_Iter)->GetName() == wxT("Multiplicity") )
		{
			l_sMultiplicity = ( *l_Iter )->GetValueString();
			( *l_Iter )->SetShow(false);
		}
		WriteAttribute( ( *l_Iter ), l_pcElem );
	}

	l_sMultiplicity += wxT("`dot");
	
	SP_DS_ColListAttribute* l_pcColList;
	SP_Graphic* l_pcGrAttr;
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 );

	l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_ColListAttribute( l_pcColList ) );
	l_pcGrAttr->SetOffsetY( 25 );
	l_pcGrAttr->SetOffsetY(0);

	l_pcColList->SetColLabel( 0, wxT("Expression set") );
	l_pcColList->SetColLabel( 1, wxT("Expression") );
	
	if( l_sMultiplicity == wxT("1`dot"))
	{
		l_pcGrAttr->SetShow( FALSE );
		//let the arc expression not show		
	}

	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, l_sMultiplicity);

	WriteAttribute( l_pcColList, l_pcElem );

    return WriteData(p_pcVal, l_pcElem);
}




