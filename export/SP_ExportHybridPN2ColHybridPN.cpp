//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2009/11/9 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportHybridPN2ColHybridPN.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_core/SP_Core.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StParameterList.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"



bool SP_ExportHybridPN2ColHybridPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	m_pcGraph = p_doc->GetGraph();
	return (cName == SP_DS_HYBRIDPN_CLASS);
}

bool SP_ExportHybridPN2ColHybridPN::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
		CHECK_POINTER(p_doc->GetGraph(), return false);
		CHECK_BOOL((!p_fileName.IsEmpty()), return false);


			SP_DS_Metadataclass* l_pcMC;
		l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);   //first check if it exists

		//if(l_pcMC)
			//return SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

		l_pcMC = m_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( m_pcGraph, SP_DS_CPN_BASICCOLORSETCLASS ) );
		l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ColorsetList"), SP_COLLIST_UNSIGNED_INTEGER, 3 ) );
		SP_DS_Metadata* l_pcNewMetadata = l_pcMC->NewElement( 1 );

		SP_DS_ColListAttribute* l_pcColListAttr;
		l_pcNewMetadata = *(l_pcMC->GetElements()->begin());
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));

		l_pcColListAttr->AppendEmptyRow();
		l_pcColListAttr->SetCell(0,0,wxT("Dot"));
		l_pcColListAttr->SetCell(0,1,wxT("dot"));
		l_pcColListAttr->SetCell(0,2,wxT("dot"));

		////constants
		SP_ListMetadata l_pcListConst;
		l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
		SP_DS_Metadataclass* l_pcNewConstants = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		//SP_DS_Metadata* l_pcConstant;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMC->GetElements()->begin(); l_itElem != l_pcMC->GetElements()->end(); l_itElem++)
		{
			SP_DS_Metadata* l_pcNewConstant = l_pcNewConstants->NewElement(1);
			//l_pcConstant = *l_itElem;
			wxString l_sName = (*l_itElem)->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sGroup = (*l_itElem)->GetAttribute(wxT("Group"))->GetValueString();
			wxString l_sType = (*l_itElem)->GetAttribute(wxT("Type"))->GetValueString();
			wxString l_sComment = (*l_itElem)->GetAttribute(wxT("Comment"))->GetValueString();

			l_pcNewConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
			l_pcNewConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);
			l_pcNewConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
			l_pcNewConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

			//copy value lists
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNewConstant->GetAttribute(wxT("ValueList")));

			SP_DS_ColListAttribute * l_pcSourceColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("ValueList")));
			l_pcColList->Clear();

			for (unsigned int i = 0; i < l_pcSourceColList->GetRowCount(); i++)
			{
				int l_nRowCol = l_pcColList->AppendEmptyRow();
				l_pcColList->SetCell(l_nRowCol, 0, l_pcSourceColList->GetCell(l_nRowCol, 0));
				l_pcColList->SetCell(l_nRowCol, 1, l_pcSourceColList->GetCell(l_nRowCol, 1));
			}

			l_pcListConst.push_back(l_pcNewConstant);//to be deleted after exporting
		}

		if(l_pcListConst.size()>0)
		{
		      m_pcGraph->Update();
		}

		bool l_bSuccess = SP_XmlWriter::Write(p_doc->GetGraph(), p_fileName);

		l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		for(auto l_pcConst : l_pcListConst)
		{
	        l_pcMC->RemoveElement(l_pcConst);
			wxDELETE(l_pcConst);
         }

		return l_bSuccess;
}

bool SP_ExportHybridPN2ColHybridPN::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return false);
	CHECK_POINTER(p_pcRoot, return false);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLHPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return true;
}

bool SP_ExportHybridPN2ColHybridPN::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListNode::const_iterator l_Iter;
	const SP_ListNode* l_plElements = p_pcVal->GetElements();

	CHECK_POINTER( l_plElements, return FALSE );
	wxString l_sNodeclassName = p_pcVal->GetName();

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
	l_pcElem->AddAttribute( wxT("count"), wxString::Format( wxT("%d"), l_plElements->size()));

	l_pcElem->AddAttribute(wxT("name"), l_sNodeclassName);
	p_pcRoot->AddChild( l_pcElem );

	if( wxT("Place") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteExtPlace( ( *l_Iter ), l_pcElem);
		}
	}
	else if( SP_DS_CONTINUOUS_PLACE == l_sNodeclassName)
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteExtPlace( ( *l_Iter ), l_pcElem);
		}
	}
	else if( wxT("Transition") == l_sNodeclassName)
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteExtTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if( SP_DS_CONTINUOUS_TRANS == l_sNodeclassName)
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteExtTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if( wxT("Immediate Transition") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteImmediateTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if( wxT("Deterministic Transition") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteDerterministicTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else if( wxT("Scheduled Transition") == l_sNodeclassName )
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteScheduledTransition( ( *l_Iter ), l_pcElem);
		}
	}
	else
	{
		for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
		{
			WriteNode( ( *l_Iter ), l_pcElem);
		}
	}

	return TRUE;
}



bool
SP_ExportHybridPN2ColHybridPN::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListEdge::const_iterator l_Iter;
    const SP_ListEdge* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format( wxT("%d"),l_plElements->size()));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteExtEdge((*l_Iter), l_pcElem);

    return TRUE;
}

bool SP_ExportHybridPN2ColHybridPN::WriteExtPlace(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild( l_pcElem );
/*
	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_ColListAttribute* l_pcMarkingColList;	

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{
		if( (*l_Iter)->GetName() == SP_DS_CPN_MARKINGLIST )
		{
			l_pcMarkingColList = (SP_DS_ColListAttribute*)(*l_Iter);			
		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
	}

	wxString l_sMarking = wxT("");
	unsigned l_nColCount = l_pcMarkingColList->GetRowCount() * 2;
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_MARKINGLIST, SP_COLLIST_UNSIGNED_INTEGER, l_nColCount );

	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	for(unsigned i = 0; i < l_pcMarkingColList->GetRowCount(); i++)
	{
		if(i==0)
		{
			l_sMarking = l_pcMarkingColList->GetCell(i,1);
		}
		l_pcColList->SetColLabel( 2*i,  l_pcMarkingColList->GetCell(i,0) + wxT(": Colors") );
		l_pcColList->SetColLabel(2*i+1, l_pcMarkingColList->GetCell(i,0)   + wxT(": Marking"));	

		l_pcColList->SetCell(l_nNewRow,2*i,  wxT("dot"));
		l_pcColList->SetCell(l_nNewRow,2*i+1,l_pcMarkingColList->GetCell(i,1));
	}	

	SP_DS_TextAttribute* l_pcNameAttribute = new SP_DS_TextAttribute(SP_DS_CPN_COLORSETNAME);
	l_pcNameAttribute->SetValue(wxT("Dot"));

	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_ColListAttribute( l_pcColList ) );
		l_pcGrAttr->SetOffsetX(45);
		l_pcGrAttr->SetOffsetY(-25); 
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());

		l_pcGrAttr = l_pcNameAttribute->AddGraphic(new SP_GR_TextAttribute(l_pcNameAttribute));
		l_pcGrAttr->SetOffsetY(-20);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	SP_DS_TextAttribute* l_pcMarkingAttr = new SP_DS_TextAttribute(wxT("Marking"));
	l_pcMarkingAttr->SetValue(l_sMarking);

	WriteAttribute( l_pcColList, l_pcElem );
	WriteAttribute(l_pcMarkingAttr, l_pcElem );
	WriteAttribute(l_pcNameAttribute, l_pcElem );
*/	

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

bool SP_ExportHybridPN2ColHybridPN::WriteExtTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_ColListAttribute* l_pcFunctionColList;	

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{

		if( (*l_Iter)->GetName() == wxT("FunctionList") )
		{
			l_pcFunctionColList = (SP_DS_ColListAttribute*)(*l_Iter);			
		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
		
	}

	//Add guards
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));


	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
		l_pcGrAttr->SetOffsetX(45);
		l_pcGrAttr->SetOffsetY(-25); 
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);



	//Add rate functions
	unsigned l_nColCount = l_pcFunctionColList->GetRowCount() + 1;
	l_pcColList = new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_UNSIGNED_INTEGER, l_nColCount );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	for(unsigned i = 0; i < l_pcFunctionColList->GetRowCount(); i++)
	{
		l_pcColList->SetColLabel(i+1,l_pcFunctionColList->GetCell(i,0) + wxT(": Function"));
		
		l_pcColList->SetCell(0,i+1,l_pcFunctionColList->GetCell(i,1));
	}

	
	l_pcColList->SetColLabel(0,wxT("Predicate"));
	l_pcColList->SetCell(0,0,wxT("true"));	

	

	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("%") ));
		l_pcGrAttr->SetOffsetX(45);
		l_pcGrAttr->SetOffsetY(-25);
		l_pcGrAttr->SetShow(FALSE);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);

	return WriteData(p_pcVal, l_pcElem);
}


bool SP_ExportHybridPN2ColHybridPN::WriteExtEdge( SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format( wxT("%ld"),p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format( wxT("%ld"),p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

	
	wxString l_sMultiplicity = wxT("1");
	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{		
		if( (*l_Iter)->GetName() == wxT("Multiplicity") )
		{
			l_sMultiplicity = ( *l_Iter )->GetValueString();
			( *l_Iter )->SetShow(false);
		}
		WriteAttribute( ( *l_Iter ), l_pcElem);
	}

	l_sMultiplicity += wxT("`dot");
	

	SP_DS_ColListAttribute* l_pcColList;	
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2 );

	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic( new SP_GR_ColListAttribute( l_pcColList ) );
		l_pcGrAttr->SetOffsetY( 25 );
		l_pcGrAttr->SetOffsetY(0);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

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

	WriteAttribute( l_pcColList, l_pcElem);

    return WriteData(p_pcVal, l_pcElem);
}



bool SP_ExportHybridPN2ColHybridPN::WriteImmediateTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_ColListAttribute* l_pcFunctionColList;	

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{

		if( (*l_Iter)->GetName() == wxT("FunctionList") )
		{
			l_pcFunctionColList = (SP_DS_ColListAttribute*)(*l_Iter);			
		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
		
	}

	//Add guards
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
		l_pcGrAttr->SetOffsetX(-45);
		l_pcGrAttr->SetOffsetY(-25); 
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);



	//Add rate functions
	unsigned l_nColCount = l_pcFunctionColList->GetRowCount() + 1;
	l_pcColList = new SP_DS_ColListAttribute( wxT("FunctionList"), SP_COLLIST_UNSIGNED_INTEGER, l_nColCount );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	for(unsigned i = 0; i < l_pcFunctionColList->GetRowCount(); i++)
	{
		l_pcColList->SetColLabel(i+1,l_pcFunctionColList->GetCell(i,0)  + wxT(": Weight"));
		
		l_pcColList->SetCell(0,i+1,l_pcFunctionColList->GetCell(i,1));
	}

	
	l_pcColList->SetColLabel(0,wxT("Predicate"));
	l_pcColList->SetCell(0,0,wxT("true"));	

	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("%") ));
		l_pcGrAttr->SetOffsetX(45);
		l_pcGrAttr->SetOffsetY(-25);
		l_pcGrAttr->SetShow(FALSE);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);

	return WriteData(p_pcVal, l_pcElem);
}


bool SP_ExportHybridPN2ColHybridPN::WriteDerterministicTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_ColListAttribute* l_pcFunctionColList;
	//SP_Graphic* l_pcGrAttr;

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{

		if( (*l_Iter)->GetName() == wxT("DelayList") )
		{
			l_pcFunctionColList = (SP_DS_ColListAttribute*)(*l_Iter);			
		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
		
	}

	//Add guards
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
		l_pcGrAttr->SetOffsetX(-45);
		l_pcGrAttr->SetOffsetY(-25); 
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);



	//Add rate functions
	unsigned l_nColCount = l_pcFunctionColList->GetRowCount() + 1;
	l_pcColList = new SP_DS_ColListAttribute( wxT("DelayList"), SP_COLLIST_UNSIGNED_INTEGER, l_nColCount );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	for(unsigned i = 0; i < l_pcFunctionColList->GetRowCount(); i++)
	{
		l_pcColList->SetColLabel(i+1,l_pcFunctionColList->GetCell(i,0) + wxT(": Delay"));
		
		l_pcColList->SetCell(0,i+1,l_pcFunctionColList->GetCell(i,1));
	}

	
	l_pcColList->SetColLabel(0,wxT("Predicate"));
	l_pcColList->SetCell(0,0,wxT("true"));	


	l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("<%>") ));
	l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetOffsetY(40);
	l_pcGrAttr->SetShow(FALSE);

	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("<%>") ));
		l_pcGrAttr->SetOffsetX(0);
		l_pcGrAttr->SetOffsetY(40);
		l_pcGrAttr->SetShow(FALSE);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);

	return WriteData(p_pcVal, l_pcElem);
}


bool SP_ExportHybridPN2ColHybridPN::WriteScheduledTransition(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot )
{
	CHECK_POINTER( p_pcVal, return FALSE );
	CHECK_POINTER( p_pcRoot, return FALSE );

	SP_ListAttribute::const_iterator l_Iter;
	const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
	CHECK_POINTER( l_plAttributes, return FALSE );

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
	p_pcRoot->AddChild( l_pcElem );

	SP_DS_ColListAttribute* l_pcColList;
	SP_DS_ColListAttribute* l_pcFunctionColList;
	//SP_Graphic* l_pcGrAttr;

	for( l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter )
	{

		if( (*l_Iter)->GetName() == wxT("PeriodicList") )
		{
			l_pcFunctionColList = (SP_DS_ColListAttribute*)(*l_Iter);			
		}
		else
		{
			WriteAttribute( ( *l_Iter ), l_pcElem);
		}
		
	}

	//Add guards
	l_pcColList = new SP_DS_ColListAttribute( SP_DS_CPN_GUARDLIST, SP_COLLIST_UNSIGNED_INTEGER, 2 );
	l_pcColList->SetColLabel( 0, wxT("Guard set") );
	l_pcColList->SetColLabel( 1, wxT("Guard") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	SP_Graphic* l_pcGrAttr;
	SP_ListGraphic::const_iterator l_itGr;
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
		l_pcGrAttr->SetOffsetX(-45);
		l_pcGrAttr->SetOffsetY(-25); 
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);



	//Add rate functions
	unsigned l_nColCount = 3*l_pcFunctionColList->GetRowCount() + 1;
	l_pcColList = new SP_DS_ColListAttribute( wxT("PeriodicList"), SP_COLLIST_UNSIGNED_INTEGER, l_nColCount );
	l_nNewRow = l_pcColList->AppendEmptyRow();
	for(unsigned i = 0; i < l_pcFunctionColList->GetRowCount(); i++)
	{
		l_pcColList->SetColLabel(3*i+1,l_pcFunctionColList->GetCell(i,0) + wxT(": ")+ wxT(": Begin"));
		l_pcColList->SetColLabel(3*i+2,l_pcFunctionColList->GetCell(i,0) + wxT(": ")+ wxT(": Repetition"));
		l_pcColList->SetColLabel(3*i+3,l_pcFunctionColList->GetCell(i,0) + wxT(": ")+ wxT(": End"));

		l_pcColList->SetCell(0,3*i+1,l_pcFunctionColList->GetCell(i,1));
		l_pcColList->SetCell(0,3*i+2,l_pcFunctionColList->GetCell(i,2));
		l_pcColList->SetCell(0,3*i+3,l_pcFunctionColList->GetCell(i,3));
	}

	
	l_pcColList->SetColLabel(0,wxT("Predicate"));
	l_pcColList->SetCell(0,0,wxT("true"));

	
	for( l_itGr = p_pcVal->GetGraphics()->begin(); l_itGr != p_pcVal->GetGraphics()->end(); l_itGr++)
	{
		l_pcGrAttr = l_pcColList->AddGraphic(new SP_GR_ColListAttribute( l_pcColList, wxT("[%]") ));
		l_pcGrAttr->SetOffsetX(0);
		l_pcGrAttr->SetOffsetY(40);
		l_pcGrAttr->SetShow(FALSE);
		l_pcGrAttr->SetGraphicParent(*l_itGr);
		l_pcGrAttr->SetNetnumber((*l_itGr)->GetNetnumber());
	}

	WriteAttribute( l_pcColList, l_pcElem);

	return WriteData(p_pcVal, l_pcElem);
}






