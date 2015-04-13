//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/12/22 $
// Short Description: export to latex
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportLatex.h"
#include "export/SP_LatexDC.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include<wx/wx.h>
#include <wx/cmndata.h>
#include <wx/scrolbar.h>
#include <wx/vscroll.h>
#include <wx/scrolwin.h>
#include <wx/checklst.h>
#include <wx/treectrl.h>
#include <wx/rearrangectrl.h>
#include <wx/popupwin.h>
#include <wx/dialog.h>
#include <wx/collpane.h>

#include <wx/dc.h>
#include <wx/paper.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

enum {
	SP_GENERATEPDF_CHECKBOX_SELECTED = SP_ID_LAST_ID + 1,

	SP_ID_BASICS_UPDATE,
	SP_ID_BUTTON_BASICS_UP,
	SP_ID_BUTTON_BASICS_DOWN,

	SP_ID_DECLARATIONS_UPDATE,
	SP_ID_BUTTON_DECLARATIONS_UP,
	SP_ID_BUTTON_DECLARATIONS_DOWN,

	SP_ID_GRAPH_UPDATE,
	SP_ID_BUTTON_GRAPH_UP,
	SP_ID_BUTTON_GRAPH_DOWN,

	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_DEC
};

SP_ExportLatex::SP_ExportLatex()
{
	//general tab options
	tabs_items.Add("Basics");
	tabs_items.Add("Graph Elements");
	tabs_items.Add("Declarations");
	tabs_items.Add("Hierarchy");

	order.Add(0);
	order.Add(1);
	order.Add(2);
	order.Add(3);

	//basics tab options////////////////////////////////
    l_pcOptions_basics.Add("General Informations");
    l_pcOptions_basics.Add("Net Informations");
    l_pcOptions_basics.Add("Report Layout");
    l_pcOptions_basics.Add("Report Typography");

	l_pcOptions_basics_order.Add(0);
	l_pcOptions_basics_order.Add(1);
	l_pcOptions_basics_order.Add(2);
	l_pcOptions_basics_order.Add(3);

	m_Basics_FontFamily_Strings.Add("Roman (rm)");
	m_Basics_FontFamily_Strings.Add("Sans serif (sf)");
	m_Basics_FontFamily_Strings.Add("Monospace (tt)");

	m_Basics_FontSize_Strings.Add("tiny");
	m_Basics_FontSize_Strings.Add("scriptsize");
	m_Basics_FontSize_Strings.Add("footnotesize");
	m_Basics_FontSize_Strings.Add("small");
	m_Basics_FontSize_Strings.Add("normalsize");
	m_Basics_FontSize_Strings.Add("large");
	m_Basics_FontSize_Strings.Add("Large");
	m_Basics_FontSize_Strings.Add("LARGE");
	m_Basics_FontSize_Strings.Add("huge");
	m_Basics_FontSize_Strings.Add("Huge");


	m_Basics_FontStyle_Strings.Add("normal font");
	m_Basics_FontStyle_Strings.Add("italic");
	m_Basics_FontStyle_Strings.Add("slanted");
	m_Basics_FontStyle_Strings.Add("bold");
	m_Basics_FontStyle_Strings.Add("medium");

	m_Basics_PaperSize_Strings.Add("a0paper");
	m_Basics_PaperSize_Strings.Add("a1paper");
	m_Basics_PaperSize_Strings.Add("a2paper");
	m_Basics_PaperSize_Strings.Add("a3paper");
	m_Basics_PaperSize_Strings.Add("a4paper");
	m_Basics_PaperSize_Strings.Add("a5paper");
	m_Basics_PaperSize_Strings.Add("a6paper");
	m_Basics_PaperSize_Strings.Add("b0paper");
	m_Basics_PaperSize_Strings.Add("b1paper");
	m_Basics_PaperSize_Strings.Add("b2paper");
	m_Basics_PaperSize_Strings.Add("b3paper");
	m_Basics_PaperSize_Strings.Add("b4paper");
	m_Basics_PaperSize_Strings.Add("b5paper");
	m_Basics_PaperSize_Strings.Add("b6paper");
	m_Basics_PaperSize_Strings.Add("letterpaper");
	m_Basics_PaperSize_Strings.Add("legalpaper");
	m_Basics_PaperSize_Strings.Add("executivepaper");

	m_pcGraph = NULL;
	p_pcDlg = NULL;
	p_pcDoc = NULL;
	m_pcMeta = NULL;
	m_pcCoarseTreectrl = NULL;
	l_pcTree = NULL;

}

bool
SP_ExportLatex::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{
	SP_LOGMESSAGE("inside AddToDialog for SP_ExportLatex");

	//Change notebook size for export to latex
	p_pcDlg->SetNotebookSize(550, 420);

	this->p_pcDoc = p_pcDoc;
	this->p_pcDlg = p_pcDlg;

    m_Options_Graph.Clear();
    m_Options_Graph_order.Clear();
	m_Options_Declarations.Clear();
	m_Options_Declarations_order.Clear();

    SP_Node2Attributes.clear();
    SP_Index2Node.clear();
    SP_Node2Sizer.clear();

    SP_DecNode2Attributes.clear();
    SP_Index2DecNode.clear();
    SP_DecNode2Sizer.clear();

    SP_Nodeclass2NodeList.clear();
    SP_Node2SelectClearAllCheckBox.clear();
    SP_Node2NodeCheckList.clear();
    SP_Node2AttributeCheckList.clear();

    SP_DecNodeclass2NodeList.clear();
    SP_DecNode2SelectClearAllCheckBox.clear();
    SP_DecNode2NodeCheckList.clear();
    SP_DecNode2AttributeCheckList.clear();

    m_pcCheckBox_BasicsLayout.clear();
    m_pcCheckBox_BasicsGeneral.clear();
    m_pcCheckBox_BasicsNet.clear();

    SP_LatexReferencesIndex2Ref.clear();
    m_pcTextCtrlHeaderFooter.clear();
    SP_HierarchyLabel2ID.clear();

    SP_Name2Node.clear();
    SP_Name2Edge.clear();
    SP_Name2Metadata.clear();
    SP_NetNumber2Label.clear();

    m_nSelectionId_Graph = 0;
    m_nSelectionId_Declarations = 0;

  //  arrays.Clear();

	LoadData();

	AddGeneral();
	AddBasics();
	AddGraphElements();
	AddDeclarations();
	AddHierarchy();

    return true;

}

bool
SP_ExportLatex::OnDialogOk()
{
	return true;
}

bool
SP_ExportLatex::AcceptsDoc(SP_MDI_Doc* p_doc)
{
  CHECK_POINTER(p_doc, return FALSE);
  return TRUE;
}

bool
SP_ExportLatex::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
  CHECK_POINTER(p_doc, return FALSE);
  CHECK_BOOL((!p_fileName.IsEmpty()), return FALSE);

  wxPrintData *pd = new wxPrintData();
  m_printData = *pd;

  StartDoc(p_fileName);
  WriteLatex();
  EndDoc();

  /*
  SP_LatexDC latexDC (*pd);

#if wxABI_VERSION < 30000
  latexDC.SetResolution(72);
#endif

  latexDC.StartDoc(p_fileName);

  wxNode *node = p_doc->GetDiagram()->GetShapeList()->GetFirst();

  while (node) {
    wxShape *shape = (wxShape*)node->GetData();
    //shape->Select(FALSE);
    shape->Draw(latexDC);
    node = node->GetNext();
  }

  latexDC.EndDoc();

  */

  wxDELETE(pd);

  return  !m_bError;
}

void
SP_ExportLatex::LoadData()
{
	m_pcGraph = p_pcDoc->GetGraph();
	NetClass = p_pcDoc->GetNetclassName();
	SP_LOGMESSAGE(NetClass);

	m_pcMeta = m_pcGraph->GetMetadataclass( wxT("General") )->GetElements()->front();
	m_pcCoarseTreectrl = p_pcDoc->GetCoarseTreectrl();

	//Loads list of graph elements for the export dialog////////////////////////////
    SP_ListNodeclass::const_iterator itNC;
    SP_ListAttribute::const_iterator itAttr;
    int i = 0;
    wxArrayString arrays;

    for (itNC = m_pcGraph->GetNodeclasses()->begin(); itNC != m_pcGraph->GetNodeclasses()->end(); ++itNC) {

    	wxString l_sNodename = (*itNC)->GetDisplayName();
    	m_Options_Graph.Add( l_sNodename );
    	m_Options_Graph_order.Add(i);

    	SP_DS_Node* l_pcNode = (*itNC)->GetPrototype();

    	if( l_pcNode ) {
    		itAttr = l_pcNode->GetAttributes()->begin();

    		arrays.Clear();

    		for(;itAttr != l_pcNode->GetAttributes()->end(); itAttr++)
    	    {
    		    arrays.Add( (*itAttr)->GetDisplayName() );
    		}

    		if( l_pcNode->GetClassName() == wxT("Coarse Place") || l_pcNode->GetClassName() == wxT("Coarse Transition") )
    		{
    			arrays.Add(wxT("Net number") );
    		}

    		arrays.Add(wxT("Cross Refs.") );

    		if( l_pcNode->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )
    			arrays.Add( wxT("Order Lexicographically") );

    		SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
    		SP_Index2Node.insert(pair<int, wxString> (i, l_sNodename) );
    		SP_LOGMESSAGE(l_sNodename + wxT("-"));
    	}
    	i++;
    }

    SP_ListEdgeclass::const_iterator itEC;
    for (itEC = m_pcGraph->GetEdgeclasses()->begin(); itEC != m_pcGraph->GetEdgeclasses()->end(); ++itEC) {

    	wxString l_sEdgename = (*itEC)->GetDisplayName();
    	m_Options_Graph.Add( l_sEdgename );
    	m_Options_Graph_order.Add(i);
    	SP_LOGMESSAGE( l_sEdgename + wxT("-"));

    	SP_DS_Edge* l_pcEdge = (*itEC)->GetPrototype();

    	if( l_pcEdge ) {

        	arrays.Clear();

        	for(itAttr = l_pcEdge->GetAttributes()->begin();itAttr != l_pcEdge->GetAttributes()->end(); itAttr++)
        	{
        	    arrays.Add( (*itAttr)->GetDisplayName() );
        	}

        	if( l_pcEdge->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )
        		arrays.Add( wxT("Order Lexicographically") );

        	SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
        	SP_Index2Node.insert(pair<int, wxString> (i, l_sEdgename) );

    	}

    	i++;
    }


    SP_ListMetadataclass::const_iterator itMC;
    SP_DS_Metadata* l_pcMeta = NULL;

    for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC) {
    	if( (*itMC)->GetShowInElementTree() ) {

    		wxString l_sMetaname = (*itMC)->GetDisplayName();
    		m_Options_Graph.Add(  l_sMetaname );
    		m_Options_Graph_order.Add(i);
    		SP_LOGMESSAGE( l_sMetaname + wxT("->>"));
    		//i++;

    		l_pcMeta = (*itMC)->GetPrototype();

    		if( l_pcMeta ) {
        		itAttr = l_pcMeta->GetAttributes()->begin();

        		arrays.Clear();


        		for(;itAttr != l_pcMeta->GetAttributes()->end(); itAttr++)
        		{
        			arrays.Add( (*itAttr)->GetDisplayName() );
        			SP_LOGMESSAGE( (*itAttr)->GetDisplayName() + wxT(" -> Meta") );
        		}

        		if( l_pcMeta->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )
        			arrays.Add( wxT("Order Lexicographically") );

        		SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
        		SP_Index2Node.insert(pair<int, wxString> (i, l_sMetaname) );

    		}
    		i++;
    	}
    }


   //loads Declarations into the export dialog/////////////////////////////////

    i = 0;
 	 if( ((m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS) ))
 	 {

 		for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 		{
 			if( (*itMC)->GetShowInDeclarationTreeColorSet() )
 			{
 				wxString l_sMetaname = (*itMC)->GetDisplayName();
 				m_Options_Declarations.Add(  l_sMetaname );
 				m_Options_Declarations_order.Add(i);
 				//SP_LOGMESSAGE( l_sMetaname + wxT("->>"));

 				l_pcMeta = (*itMC)->GetPrototype();

 				if( l_pcMeta ) {
 					itAttr = l_pcMeta->GetAttributes()->begin();
 					arrays.Clear();

 					for(;itAttr != l_pcMeta->GetAttributes()->end(); itAttr++)
 					{
 						arrays.Add( (*itAttr)->GetDisplayName() );
 					//	SP_LOGMESSAGE( (*itAttr)->GetDisplayName() + wxT(" -> Declarations") );
 					}

 					if( l_pcMeta->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )
 					    arrays.Add( wxT("Order Lexicographically") );

 				    SP_DecNode2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
 				    SP_Index2DecNode.insert(pair<int, wxString> (i, l_sMetaname) );
 					i++;
 				}
 		    }
 		}
 	 }

 	for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 	{
       if( (*itMC)->GetShowInDeclarationTreeOther() )
       {
    	   wxString l_sMetaname = (*itMC)->GetDisplayName();
    	   m_Options_Declarations.Add( l_sMetaname );
    	   m_Options_Declarations_order.Add(i);
    	 //  SP_LOGMESSAGE( l_sMetaname + wxT("->>"));

    	   l_pcMeta = (*itMC)->GetPrototype();

    	   if( l_pcMeta ) {
    		   itAttr = l_pcMeta->GetAttributes()->begin();
    	       arrays.Clear();

    	       for(;itAttr != l_pcMeta->GetAttributes()->end(); itAttr++)
    	       {
    	    		arrays.Add( (*itAttr)->GetDisplayName() );
    	    		//SP_LOGMESSAGE( (*itAttr)->GetDisplayName() + wxT(" -> Declarations") );
    	       }

			   if( l_pcMeta->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )
				    arrays.Add( wxT("Order Lexicographically") );

    	       SP_DecNode2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
    	       SP_Index2DecNode.insert(pair<int, wxString> (i, l_sMetaname) );
    	       i++;
    	   }
       }
     }


/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//loading list of individual graph elements from current graph for right panels//////////////////////////

 	//node class
	for(itNC = m_pcGraph->GetNodeclasses()->begin(); itNC != m_pcGraph->GetNodeclasses()->end(); ++itNC)
	{
		SP_DS_Nodeclass* l_pcNodeclass = *itNC;
		wxString NodeclassName = l_pcNodeclass->GetDisplayName();
		SP_ListNode::const_iterator l_itElem;
		arrays.Clear();

		for(l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem) {
			wxString l_sNodeName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValueString();
			arrays.Add( l_sNodeName );

			SP_Name2Node.insert(pair<wxString, SP_DS_Node*> ( l_sNodeName, (*l_itElem) ) );

		}

		SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (NodeclassName, arrays) );
	}

	//edge class
	for(itEC = m_pcGraph->GetEdgeclasses()->begin(); itEC != m_pcGraph->GetEdgeclasses()->end(); ++itEC)
	{
		SP_DS_Edgeclass* l_pcEdgeclass = *itEC;
		SP_ListEdge::const_iterator l_itElem;
		wxString EdgeclassName;
		wxString l_sEdgeName;

		if( l_pcEdgeclass && ( ( l_pcEdgeclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) ||
				( l_pcEdgeclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) ) ) ) {
			EdgeclassName = l_pcEdgeclass->GetDisplayName();

			arrays.Clear();

			for(l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem) {

				if( l_pcEdgeclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )	{
					l_sEdgeName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValueString();
					arrays.Add( l_sEdgeName );
				} else {
					l_sEdgeName = wxT("ID: ")
							+ ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )->GetValueString();
					arrays.Add( l_sEdgeName );
				}
				SP_Name2Edge.insert(pair<wxString, SP_DS_Edge*> ( l_sEdgeName, (*l_itElem) ) );

			}

			SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (EdgeclassName, arrays) );
		}
	}


	//meta class
	for(itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
	{
		if( (*itMC)->GetShowInElementTree() ) {
			SP_DS_Metadataclass* l_pcMetaclass = *itMC;
			SP_ListMetadata::const_iterator l_itElem;
			wxString MetaclassName;
			wxString l_sMetaName;
			arrays.Clear();

			if( l_pcMetaclass ) {
				MetaclassName = l_pcMetaclass->GetDisplayName();

				if ( ( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) ||
							( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )  ) {

					for(l_itElem = l_pcMetaclass->GetElements()->begin(); l_itElem != l_pcMetaclass->GetElements()->end(); ++l_itElem) {

						if( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
							l_sMetaName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValueString();
							arrays.Add( l_sMetaName );
						} else {
							l_sMetaName = wxT("ID: ")
											+ ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )->GetValueString();
							arrays.Add( l_sMetaName );
						}
						SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, (*l_itElem) ) );
					}

					SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (MetaclassName, arrays) );

					SP_LOGMESSAGE( wxT("Metadata class loaded:" ) + MetaclassName );


				} else if ( MetaclassName.compare(wxT("Image") ) == 0 || MetaclassName.compare(wxT("Comment") ) == 0 ) {

					for(l_itElem = l_pcMetaclass->GetElements()->begin(); l_itElem != l_pcMetaclass->GetElements()->end(); ++l_itElem) {
						l_sMetaName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_TEXT) )->GetValueString();
						arrays.Add( l_sMetaName );
					}
					SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, (*l_itElem) ) );

					SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (MetaclassName, arrays) );

					SP_LOGMESSAGE( wxT("Metadata class loaded:" ) + MetaclassName );
				}
			}
		}
	}

	///////////////////////////////////////////////////////////
	// loading individual declarations list into the dialog

 	 if( ((m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS) ))
 	 {

 		for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 		{
 			SP_DS_Metadataclass* l_pcMetaclass = *itMC;
 			SP_ListMetadata::const_iterator l_itElem;
 			wxString MetaclassName;
 			arrays.Clear();

 			if( l_pcMetaclass && l_pcMetaclass->GetShowInDeclarationTreeColorSet() )
 			{
 				MetaclassName = l_pcMetaclass->GetDisplayName();

 				if ( ( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) ||
 										( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )  )
 				{
 					for(l_itElem = l_pcMetaclass->GetElements()->begin(); l_itElem != l_pcMetaclass->GetElements()->end(); ++l_itElem) {
 						wxString l_sMetaName;
 						if( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
 							l_sMetaName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValueString();
 							arrays.Add( l_sMetaName );
 						} else {
 							l_sMetaName = wxT("ID: ")
 									+ ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )->GetValueString();
 							arrays.Add( l_sMetaName );
 						}
 						SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, (*l_itElem) ) );
 					}

 					SP_DecNodeclass2NodeList.insert(pair<wxString, wxArrayString> (MetaclassName, arrays) );
 			    }

 		    }
 		}
 	 }

 	for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 	{
 		SP_DS_Metadataclass* l_pcMetaclass = *itMC;
 		SP_ListMetadata::const_iterator l_itElem;
 		wxString MetaclassName;
 		arrays.Clear();

       if( l_pcMetaclass && l_pcMetaclass->GetShowInDeclarationTreeOther() )
       {
    	   MetaclassName = l_pcMetaclass->GetDisplayName();

    	   if ( ( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) ||
    			   ( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )  )
    	   {
    		   for(l_itElem = l_pcMetaclass->GetElements()->begin(); l_itElem != l_pcMetaclass->GetElements()->end(); ++l_itElem) {
    			   wxString l_sMetaName;
    			   if( l_pcMetaclass->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
    				   l_sMetaName = ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValueString();
    	    		   arrays.Add( l_sMetaName );
    	    		} else {
    	    			l_sMetaName = wxT("ID: ")
    	    					+ ( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) )->GetValueString();
    	    			arrays.Add( l_sMetaName );
    	    		}
    			    SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, (*l_itElem) ) );
    	    	}

    	    	SP_DecNodeclass2NodeList.insert(pair<wxString, wxArrayString> (MetaclassName, arrays) );
    	    }
         }
     }

}


void
SP_ExportLatex::Declarations_UpdateUI(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable( p_cEvent.GetId() == SP_ID_BUTTON_DECLARATIONS_UP ? m_pcRearrangelist_declarations->CanMoveCurrentUp()
		                                           : m_pcRearrangelist_declarations->CanMoveCurrentDown() );
}

void
SP_ExportLatex::Graph_UpdateUI(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable( p_cEvent.GetId() == SP_ID_BUTTON_GRAPH_UP ? m_pcRearrangelist_Graph->CanMoveCurrentUp()
			                                           : m_pcRearrangelist_Graph->CanMoveCurrentDown() );
}

void
SP_ExportLatex::AddGeneral()
{
	m_pcNotebookPageGeneral = p_pcDlg->AddPage(wxT("General"));

	m_pcMainSizerGeneral = new wxBoxSizer( wxVERTICAL );

	wxStaticText* l_pcStaticText = new wxStaticText(m_pcNotebookPageGeneral, wxID_ANY, wxT("Reorder tabs as required"));
	m_pcMainSizerGeneral->Add(l_pcStaticText, 0, wxALL | wxEXPAND, 5);

	wxRearrangeCtrl* rearrangectrl = new wxRearrangeCtrl(m_pcNotebookPageGeneral, -1, wxPoint(50, 50), wxSize(10, 100), order, tabs_items);
	m_pcMainSizerGeneral->Add(rearrangectrl, 1, wxALL | wxEXPAND, 5);


	//checkbox for pdf
	wxCheckBox* l_pcCheckBoxDirectPDF = new wxCheckBox( m_pcNotebookPageGeneral, SP_GENERATEPDF_CHECKBOX_SELECTED, wxT("Generate PDF Report") );
	l_pcCheckBoxDirectPDF->SetValue(true);

	m_pcMainSizerGeneral->Add(l_pcCheckBoxDirectPDF, 0, wxALL, 5);
	m_pcMainSizerGeneral->Add(10, 10);

	l_pcCheckBoxDirectPDF->Bind(wxEVT_CHECKBOX, &SP_ExportLatex::OnClickGeneratePDF, this, SP_GENERATEPDF_CHECKBOX_SELECTED);


	m_pcSizerGeneratePDF = new wxBoxSizer( wxVERTICAL );

	//sizer for latex compiler browse
	wxSizer* l_pcSizerPath1 = new wxBoxSizer( wxHORIZONTAL ); //path for generated pdf file

	l_pcSizerPath1->Add(new wxStaticText( m_pcNotebookPageGeneral, -1, wxT("Latex Compiler Location:") ), 0, wxALL, 5);

	wxString l_sPath1 = wxT("pdftex");
	wxString l_sPDFDescr1 = wxT("Compiler Directory");
	wxString wildcard1 = l_sPDFDescr1 + wxT("*/*");

	l_pcFilePickerCtrl1 = new wxFilePickerCtrl( m_pcNotebookPageGeneral, -1, l_sPath1, wxT("Browse"),
			wildcard1, wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
	l_pcFilePickerCtrl1->SetPath( l_sPath1);

	l_pcSizerPath1->Add(l_pcFilePickerCtrl1, 1, wxALL | wxEXPAND, 5);
	m_pcSizerGeneratePDF->Add(l_pcSizerPath1, 1, wxALL | wxEXPAND, 5);
	///////////////

	//sizer for pdf file browse
	wxSizer* l_pcSizerPath2 = new wxBoxSizer( wxHORIZONTAL ); //path for generated pdf file

	l_pcSizerPath2->Add(new wxStaticText( m_pcNotebookPageGeneral, -1, wxT("PDF Filename:") ), 0, wxALL, 5);

	wxString l_sPath2 = p_pcDlg->GetCurrentFileName();
	l_sPath2 = l_sPath2.Before('.');
	wxString l_sPDFExtension = wxT("pdf");

	l_sPath2 += wxT(".") + l_sPDFExtension;

	wxString l_sPDFDescr2 = wxT("PDF Files");
	wxString wildcard2 = l_sPDFDescr2 + wxT("(*.") + l_sPDFExtension + wxT(")|*.") + l_sPDFExtension;

	l_pcFilePickerCtrl2 = new wxFilePickerCtrl( m_pcNotebookPageGeneral, -1, l_sPath2, wxT("Browse"),
			wildcard2, wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL );
	l_pcFilePickerCtrl2->SetPath( l_sPath2 );

	l_pcSizerPath2->Add(l_pcFilePickerCtrl2, 1, wxALL | wxEXPAND, 5);
	m_pcSizerGeneratePDF->Add(l_pcSizerPath2, 1, wxALL | wxEXPAND, 5);
	//////////

	m_pcMainSizerGeneral->Add(m_pcSizerGeneratePDF, 1, wxALL | wxEXPAND, 5);

	m_pcNotebookPageGeneral->AddControl(m_pcMainSizerGeneral, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

}


void
SP_ExportLatex::AddBasics()
{
	m_pcNotebookPageBasics = p_pcDlg->AddPage(wxT("Basics"));

	m_pcMainSizer_Basics = new wxBoxSizer( wxHORIZONTAL );
	m_pcLeftSizer_Basics = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageBasics, -1, wxT("Basic Attributes") ), wxVERTICAL );
	m_pcRightSizer_Basics = new wxBoxSizer(wxVERTICAL);
    wxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcMainSizer_Basics->Add(m_pcLeftSizer_Basics, 1, wxALL | wxEXPAND, 5);
    m_pcMainSizer_Basics->Add(m_pcRightSizer_Basics, 1, wxALL | wxEXPAND, 5);

    //rearrangelist
	m_pcRearrangelist_basics = new wxRearrangeList(m_pcNotebookPageBasics, SP_ID_BASICS_UPDATE, wxDefaultPosition, wxSize(50, 200), l_pcOptions_basics_order, l_pcOptions_basics);
	m_pcLeftSizer_Basics->Add(m_pcRearrangelist_basics, 0, wxALL | wxEXPAND, 5);

	l_pcButtonUp = new wxButton( m_pcNotebookPageBasics, SP_ID_BUTTON_BASICS_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	l_pcButtonDown = new wxButton( m_pcNotebookPageBasics, SP_ID_BUTTON_BASICS_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	//Dynamic event binding
	m_pcRearrangelist_basics->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Basics, this, SP_ID_BASICS_UPDATE);

	l_pcButtonUp->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Basics_UpdateUI, this, SP_ID_BUTTON_BASICS_UP);
	l_pcButtonDown->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Basics_UpdateUI, this, SP_ID_BUTTON_BASICS_DOWN);

	l_pcButtonUp->Bind(wxEVT_BUTTON, &SP_ExportLatex::Basics_Move, this, SP_ID_BUTTON_BASICS_UP);
	l_pcButtonDown->Bind(wxEVT_BUTTON, &SP_ExportLatex::Basics_Move, this, SP_ID_BUTTON_BASICS_DOWN);

	l_pcButtonSizer->Add(l_pcButtonUp, 1, wxALL | wxEXPAND, 5);
	l_pcButtonSizer->Add(l_pcButtonDown, 1, wxALL | wxEXPAND, 5);

	m_pcLeftSizer_Basics->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

	AddAttributes_BasicsGeneral();
	AddAttributes_BasicsNet();
    AddAttributes_BasicsLayout();
    AddAttributes_BasicsTypography();
    			
	m_pcNotebookPageBasics->AddControl(m_pcMainSizer_Basics, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

}


bool
SP_ExportLatex::AddAttributes_GraphElements()
{
	map<int, wxArrayString >::const_iterator itN;
	SP_Node2NodeCheckList.clear();
	SP_Node2SelectClearAllCheckBox.clear();


	for (itN = SP_Node2Attributes.begin(); itN != SP_Node2Attributes.end(); itN++)
	{
		wxString l_sCurrentNode = SP_Index2Node[ (*itN).first ];   //node class
		SP_LOGMESSAGE( wxT("Current node adding sizer: ") + l_sCurrentNode );
		wxSizer* l_pcRightSizer = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageGraph, l_sCurrentNode);

		wxCheckListBox* l_pcCheckList = new wxCheckListBox( m_pcNotebookPageGraph, -1, wxDefaultPosition, wxDefaultSize);
		l_pcRightSizer->Add(l_pcCheckList, 1, wxALL | wxEXPAND, 5);
		l_pcCheckList->Set( (*itN).second );

		wxArrayString arr = (*itN).second;
		for(int i = 0; i < arr.size(); i++)
		{
			if( l_sCurrentNode.compare("Comment") )
			{
				if( arr[i].compare( wxT("Comment") ) && arr[i].compare( wxT("Order Lexicographically") ) )
				{
					l_pcCheckList->Check(i, true);
				}
			}
			else {
				l_pcCheckList->Check(i, true);
			}
		}

		SP_Node2AttributeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList) );

	    if( (l_sCurrentNode.Find("Edge") == wxNOT_FOUND) && l_sCurrentNode.Cmp("Comment") )
	    {
	    	l_pcRightSizer->Add(5, 20);
	    	l_pcRightSizer->Add(new wxStaticText( m_pcNotebookPageGraph, -1, wxT("Wildcard") ), 0, wxALL, 5);
	        wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(m_pcNotebookPageGraph, -1);
	        l_pcRightSizer->Add(l_pcTextCtrl, 0, wxALL | wxEXPAND, 5);

		    wxCheckListBox* l_pcCheckList1 = new wxCheckListBox( m_pcNotebookPageGraph, -1, wxDefaultPosition, wxDefaultSize);
			l_pcRightSizer->Add(l_pcCheckList1, 1, wxALL | wxEXPAND, 5);
			l_pcCheckList1->Set( SP_Nodeclass2NodeList[ l_sCurrentNode ] );
			SP_Node2NodeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList1) );

			wxCheckBox* l_pcSelectClearAllChkBox = new wxCheckBox(m_pcNotebookPageGraph, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH,
						wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);

		    l_pcRightSizer->Add(l_pcSelectClearAllChkBox, 0, wxALL, 5);
			l_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);

			l_pcSelectClearAllChkBox->Bind(wxEVT_CHECKBOX, &SP_ExportLatex::OnSelectClearAllItems_Graph, this,
					SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH);
			SP_Node2SelectClearAllCheckBox.insert(pair<int, wxCheckBox* > ( (*itN).first, l_pcSelectClearAllChkBox) );

	    }

		SP_Node2Sizer.insert(pair<int, wxSizer* > ( (*itN).first, l_pcRightSizer) );

		m_pcRightSizer_Graph->Add(l_pcRightSizer, 1, wxALL | wxEXPAND, 0);

		if( itN != SP_Node2Attributes.begin() ) {
			m_pcRightSizer_Graph->Show( l_pcRightSizer, false, true);
		}

	}

	return true;
}


void
SP_ExportLatex::AddGraphElements()
{
	m_pcNotebookPageGraph = p_pcDlg->AddPage(wxT("Graph Elements"));

    m_pcMainSizer_Graph = new wxBoxSizer( wxHORIZONTAL );
    m_pcLeftSizer_Graph = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageGraph, -1, wxT("Graph Elements") ), wxVERTICAL );
    m_pcRightSizer_Graph = new wxBoxSizer(wxVERTICAL);
    wxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcMainSizer_Graph->Add(m_pcLeftSizer_Graph, 1, wxALL | wxEXPAND, 5);
    m_pcMainSizer_Graph->Add(m_pcRightSizer_Graph, 1, wxALL | wxEXPAND, 5);

    //rearrangelist
    m_pcRearrangelist_Graph = new wxRearrangeList(m_pcNotebookPageGraph, SP_ID_GRAPH_UPDATE, wxDefaultPosition,
    		wxSize(50, 200), m_Options_Graph_order, m_Options_Graph);
    m_pcLeftSizer_Graph->Add(m_pcRearrangelist_Graph, 1, wxALL | wxEXPAND, 5);

    m_pcGraph_ButtonUp = new wxButton(m_pcNotebookPageGraph, SP_ID_BUTTON_GRAPH_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	m_pcGraph_ButtonDown = new wxButton(m_pcNotebookPageGraph, SP_ID_BUTTON_GRAPH_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	//Dynamic event binding
	m_pcRearrangelist_Graph->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Graph, this, SP_ID_GRAPH_UPDATE);

	m_pcGraph_ButtonUp->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Graph_UpdateUI, this, SP_ID_BUTTON_GRAPH_UP);
	m_pcGraph_ButtonDown->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Graph_UpdateUI, this, SP_ID_BUTTON_GRAPH_DOWN);

	m_pcGraph_ButtonUp ->Bind(wxEVT_BUTTON, &SP_ExportLatex::Graph_Move, this, SP_ID_BUTTON_GRAPH_UP);
	m_pcGraph_ButtonDown->Bind(wxEVT_BUTTON, &SP_ExportLatex::Graph_Move, this, SP_ID_BUTTON_GRAPH_DOWN);


	l_pcButtonSizer->Add(m_pcGraph_ButtonUp, 1, wxALL | wxEXPAND, 5);
	l_pcButtonSizer->Add(m_pcGraph_ButtonDown, 1, wxALL | wxEXPAND, 5);

	m_pcLeftSizer_Graph->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

    AddAttributes_GraphElements();

    m_pcNotebookPageGraph->AddControl(m_pcMainSizer_Graph, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);
}

bool
SP_ExportLatex::AddAttributes_Declarations()
{
	map<int, wxArrayString >::const_iterator itN;
	SP_DecNode2NodeCheckList.clear();
	SP_DecNode2SelectClearAllCheckBox.clear();


	for (itN = SP_DecNode2Attributes.begin(); itN != SP_DecNode2Attributes.end(); itN++)
    {
		wxString l_sCurrentNode = SP_Index2DecNode[ (*itN).first ];   //node class

		wxSizer* l_pcRightSizer = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageDeclarations, l_sCurrentNode);

		wxCheckListBox* l_pcCheckList = new wxCheckListBox( m_pcNotebookPageDeclarations, -1, wxDefaultPosition, wxDefaultSize);
		l_pcRightSizer->Add(l_pcCheckList, 1, wxALL | wxEXPAND, 5);
		l_pcCheckList->Set( (*itN).second );

		wxArrayString arr = (*itN).second;
		for(int i = 0; i < arr.size(); i++)
		{
			if( arr[i].compare( wxT("Comment") ) && arr[i].compare( wxT("Order Lexicographically") ) )
			{
				l_pcCheckList->Check(i, true);
			}
		}

		SP_DecNode2AttributeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList) );

		l_pcRightSizer->Add(5, 20);
		l_pcRightSizer->Add(new wxStaticText( m_pcNotebookPageDeclarations, -1, wxT("Wildcard") ), 0, wxALL, 5);
		wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(m_pcNotebookPageDeclarations, -1);
		l_pcRightSizer->Add(l_pcTextCtrl, 0, wxALL | wxEXPAND, 5);

		wxCheckListBox* l_pcCheckList1 = new wxCheckListBox( m_pcNotebookPageDeclarations, -1, wxDefaultPosition, wxDefaultSize);
		l_pcRightSizer->Add(l_pcCheckList1, 1, wxALL | wxEXPAND, 5);
		l_pcCheckList1->Set( SP_DecNodeclass2NodeList[ l_sCurrentNode ] );
		SP_DecNode2NodeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList1) );


		wxCheckBox* l_pcSelectClearAllChkBox = new wxCheckBox(m_pcNotebookPageDeclarations, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_DEC,
								wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);

		l_pcRightSizer->Add(l_pcSelectClearAllChkBox, 0, wxALL, 5);
		l_pcSelectClearAllChkBox->Set3StateValue(wxCHK_UNDETERMINED);

		l_pcSelectClearAllChkBox->Bind(wxEVT_CHECKBOX, &SP_ExportLatex::OnSelectClearAllItems_Dec, this, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_DEC);
		SP_DecNode2SelectClearAllCheckBox.insert(pair<int, wxCheckBox* > ( (*itN).first, l_pcSelectClearAllChkBox) );

		SP_DecNode2Sizer.insert(pair<int, wxSizer* > ( (*itN).first, l_pcRightSizer) );

		m_pcRightSizer_Declarations->Add(l_pcRightSizer, 1, wxALL | wxEXPAND, 0);

		if( itN != SP_DecNode2Attributes.begin() ) {
			m_pcRightSizer_Declarations->Show( l_pcRightSizer, false, true);
		}

		SP_LOGMESSAGE( l_sCurrentNode );
    }

	return true;
}

void
SP_ExportLatex::AddDeclarations()
{
	m_pcNotebookPageDeclarations = p_pcDlg->AddPage(wxT("Declarations"));

    m_pcMainSizer_Declarations = new wxBoxSizer( wxHORIZONTAL );
    m_pcLeftSizer_Declarations = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageDeclarations, -1, wxT("Declarations Attributes") ), wxVERTICAL );
    m_pcRightSizer_Declarations = new wxBoxSizer(wxVERTICAL);
    wxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pcMainSizer_Declarations->Add(m_pcLeftSizer_Declarations, 1, wxALL | wxEXPAND, 5);
    m_pcMainSizer_Declarations->Add(m_pcRightSizer_Declarations, 1, wxALL | wxEXPAND, 5);

	//rearrangelist
	m_pcRearrangelist_declarations = new wxRearrangeList(m_pcNotebookPageDeclarations, SP_ID_DECLARATIONS_UPDATE, wxDefaultPosition,
			wxSize(50, 200), m_Options_Declarations_order, m_Options_Declarations);
	m_pcLeftSizer_Declarations->Add(m_pcRearrangelist_declarations, 1, wxALL | wxEXPAND, 5);

	m_pcDeclarations_ButtonUp = new wxButton( m_pcNotebookPageDeclarations, SP_ID_BUTTON_DECLARATIONS_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	m_pcDeclarations_ButtonDown = new wxButton( m_pcNotebookPageDeclarations, SP_ID_BUTTON_DECLARATIONS_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	//dynamic event binding
	m_pcRearrangelist_declarations->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Declarations, this, SP_ID_DECLARATIONS_UPDATE);


	m_pcDeclarations_ButtonUp->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Declarations_UpdateUI, this, SP_ID_BUTTON_DECLARATIONS_UP);
	m_pcDeclarations_ButtonDown->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Declarations_UpdateUI, this, SP_ID_BUTTON_DECLARATIONS_DOWN);

	m_pcDeclarations_ButtonUp->Bind(wxEVT_BUTTON, &SP_ExportLatex::Declarations_Move, this, SP_ID_BUTTON_DECLARATIONS_UP);
	m_pcDeclarations_ButtonDown->Bind(wxEVT_BUTTON, &SP_ExportLatex::Declarations_Move, this, SP_ID_BUTTON_DECLARATIONS_DOWN);

	l_pcButtonSizer->Add(m_pcDeclarations_ButtonUp, 1, wxALL | wxEXPAND, 5);
	l_pcButtonSizer->Add(m_pcDeclarations_ButtonDown, 1, wxALL | wxEXPAND, 5);

	m_pcLeftSizer_Declarations->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

	//right panel
    AddAttributes_Declarations();

	m_pcNotebookPageDeclarations->AddControl(m_pcMainSizer_Declarations, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);
}

void
SP_ExportLatex::copyTree_recur(const wxTreeItemId& into, const wxTreeItemId& from, wxString label, int id)
{
	if(!from.IsOk() ) { return; }

	  wxTreeItemIdValue cookie = NULL;
	  wxTreeItemId child_to;

	  wxString local_label;
	  int local_id = id;

	  wxTreeItemId child_from = m_pcCoarseTreectrl->GetFirstChild(from, cookie);
	  local_label = label + wxString::Format(wxT("%i"), local_id) + wxT(".");


	  while( child_from.IsOk() ) {

		  wxString name = m_pcCoarseTreectrl->GetItemText(child_from);
		  wxString s =  local_label + wxT(" ") + name;
		  child_to = l_pcTree->AppendItem(into, s);

		  SP_HierarchyLabel2ID.insert(pair<wxString, wxString> (name, local_label) );

		  wxString l_pcNetnumber = name.AfterLast('(');
		  l_pcNetnumber = l_pcNetnumber.BeforeLast(')');
		  SP_NetNumber2Label.insert( pair<wxString, wxString> (l_pcNetnumber, name) );

		  copyTree_recur(child_to, child_from, local_label, id);

		  local_id++;
		  local_label = label + wxString::Format(wxT("%i"), local_id) + wxT(".");

		  child_from = m_pcCoarseTreectrl->GetNextChild(from, cookie);
	  }

	  id++;

	  return;
}

void
SP_ExportLatex::AddHierarchy()
{

	m_pcNotebookPageHierarchy = p_pcDlg->AddPage(wxT("Hierarchy"));

	wxSizer* l_pcMainSizer = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* l_pcTopSizer = new wxBoxSizer( wxVERTICAL );

	l_pcMainSizer->Add(50, 0);
	l_pcMainSizer->Add(l_pcTopSizer, 0, wxALL | wxEXPAND, 5);

	wxSizer* l_pcTreeSizer = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageHierarchy, -1,
			wxT("Select the topmost hierarchy level"), wxDefaultPosition, wxDefaultSize ), wxVERTICAL );

 	wxCheckBox* l_pcCheckBoxHierarchyTree = new wxCheckBox(m_pcNotebookPageHierarchy, -1, wxT("Generate Hierarchy Tree"));
 	l_pcCheckBoxHierarchyTree->SetValue(true);
 	l_pcTopSizer->Add(l_pcCheckBoxHierarchyTree, 0, wxALL | wxEXPAND, 5);
 	l_pcTopSizer->Add(0, 20);
 	l_pcTopSizer->Add(l_pcTreeSizer, 1 , wxALL | wxEXPAND, 5);

    //tree implementation

 	l_pcTree = new wxTreeCtrl(m_pcNotebookPageHierarchy, -1, wxDefaultPosition, wxSize(300, 190), wxTR_DEFAULT_STYLE);
 	l_pcTreeSizer->Add(l_pcTree, 0, wxALL | wxEXPAND, 5);

 	wxTreeItemId root = l_pcTree->AddRoot( wxT("0. Top Level") );
 	wxTreeItemId root_to_copy = m_pcCoarseTreectrl->GetRootItem();
 	wxString name = m_pcCoarseTreectrl->GetItemText(root_to_copy);

 	 SP_HierarchyLabel2ID.insert(pair<wxString, wxString> (name, wxT("0.") ) );
 	 SP_NetNumber2Label.insert( pair<wxString, wxString> (wxT("1"), name) );

 	SP_LOGMESSAGE("Begin Copying Tree...");
 	copyTree_recur( root , root_to_copy, wxT(""), 1);
 	SP_LOGMESSAGE("End Copying Tree...");

 	l_pcTree->Expand(root);

	m_pcNotebookPageHierarchy->AddControl(l_pcMainSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

}

void
SP_ExportLatex:: OnClickGeneratePDF(wxCommandEvent& p_cEvent)
{
	if( p_cEvent.IsChecked() ) {
		SP_LOGMESSAGE("generate pdf");

		l_pcFilePickerCtrl1->Enable(true);
		l_pcFilePickerCtrl2->Enable(true);

	} else {
		SP_LOGMESSAGE("generate pdf Else");

		l_pcFilePickerCtrl1->Enable(false);
		l_pcFilePickerCtrl2->Enable(false);
	}
	m_pcSizerGeneratePDF->Layout();

}

void
SP_ExportLatex::Basics_UpdateUI(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable( p_cEvent.GetId() == SP_ID_BUTTON_BASICS_UP ? m_pcRearrangelist_basics->CanMoveCurrentUp()
		                                           : m_pcRearrangelist_basics->CanMoveCurrentDown() );
}

void
SP_ExportLatex::Basics_Move(wxCommandEvent& p_cEvent)
{
	if( p_cEvent.GetId() == SP_ID_BUTTON_BASICS_UP ) {
		if( m_pcRearrangelist_basics->CanMoveCurrentUp() )
			m_pcRearrangelist_basics->MoveCurrentUp();
	} else {
		if( m_pcRearrangelist_basics->CanMoveCurrentDown() )
			m_pcRearrangelist_basics->MoveCurrentDown();
	}
}

void
SP_ExportLatex::Graph_Move(wxCommandEvent& p_cEvent)
{
	if( p_cEvent.GetId() == SP_ID_BUTTON_GRAPH_UP ) {
			if( m_pcRearrangelist_Graph->CanMoveCurrentUp() )
				m_pcRearrangelist_Graph->MoveCurrentUp();
	} else {
			if( m_pcRearrangelist_Graph->CanMoveCurrentDown() )
				m_pcRearrangelist_Graph->MoveCurrentDown();
	}

}


void
SP_ExportLatex::Declarations_Move(wxCommandEvent& p_cEvent)
{
	if( p_cEvent.GetId() == SP_ID_BUTTON_DECLARATIONS_UP ) {
		if( m_pcRearrangelist_declarations->CanMoveCurrentUp() )
			m_pcRearrangelist_declarations->MoveCurrentUp();
	} else {
		if( m_pcRearrangelist_declarations->CanMoveCurrentDown() )
			m_pcRearrangelist_declarations->MoveCurrentDown();
	}
}


void
SP_ExportLatex::AddAttributes_BasicsGeneral()
{
	m_pcSizer_Basics_GeneralInfo = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageBasics, wxT("General Informations"));
	m_pcRightSizer_Basics->Add(m_pcSizer_Basics_GeneralInfo, 1, wxALL | wxEXPAND, 0);

	wxSizer* l_pcSizerFilename = new wxBoxSizer(wxHORIZONTAL);

	wxCheckBox* l_pcCheckBox_Filename = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("File Name") );
	l_pcCheckBox_Filename->SetValue(true);
	m_pcSizer_Basics_GeneralInfo->Add(l_pcCheckBox_Filename, 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_Filename);

	m_pcSizer_Basics_GeneralInfo->Add(l_pcSizerFilename, 0, wxALL, 5);
	wxCheckBox* l_pcCheckBox_FilePath = new wxCheckBox(m_pcNotebookPageBasics, -1, wxT("Absolute File Path"));
	l_pcSizerFilename->Add(20, 0);
	l_pcSizerFilename->Add(l_pcCheckBox_FilePath, 0, wxALL, 2);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_FilePath);

	wxCheckBox* l_pcCheckBox_Authors = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Authors") );
	l_pcCheckBox_Authors->SetValue(true);
	m_pcSizer_Basics_GeneralInfo->Add(l_pcCheckBox_Authors, 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_Authors);

	wxCheckBox* l_pcCheckBox_CreationTimestamp = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Creation Timestamp") );
	l_pcCheckBox_CreationTimestamp->SetValue(true);
	m_pcSizer_Basics_GeneralInfo->Add( l_pcCheckBox_CreationTimestamp, 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back( l_pcCheckBox_CreationTimestamp );

	wxCheckBox* l_pcCheckBox_Description = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Description") );
	m_pcSizer_Basics_GeneralInfo->Add(l_pcCheckBox_Description , 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_Description);

	wxCheckBox* l_pcCheckBox_Keywords = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Keywords") );
	l_pcCheckBox_Keywords->SetValue(true);
	m_pcSizer_Basics_GeneralInfo->Add(l_pcCheckBox_Keywords, 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_Keywords);

	wxCheckBox* l_pcCheckBox_References = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("References") );
	l_pcCheckBox_References->SetValue(true);
	m_pcSizer_Basics_GeneralInfo->Add(l_pcCheckBox_References, 0, wxALL, 5);
	m_pcCheckBox_BasicsGeneral.push_back(l_pcCheckBox_References);

}


void
SP_ExportLatex::AddAttributes_BasicsNet()
{
	m_pcSizer_Basics_NetInfo = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageBasics, wxT("Net Informations"));
	m_pcRightSizer_Basics->Add(m_pcSizer_Basics_NetInfo, 1, wxALL | wxEXPAND, 0);

	wxCheckBox* l_pcCheckBox_NetClass = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Net Class") );
	l_pcCheckBox_NetClass->SetValue(true);
	m_pcSizer_Basics_NetInfo->Add(l_pcCheckBox_NetClass, 0, wxALL, 5);
	m_pcCheckBox_BasicsNet.push_back(l_pcCheckBox_NetClass);

	wxCheckBox* l_pcCheckBox_ElementCount = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Element Count") );
	l_pcCheckBox_ElementCount->SetValue(true);
	m_pcSizer_Basics_NetInfo->Add(l_pcCheckBox_ElementCount, 0, wxALL, 5);
	m_pcCheckBox_BasicsNet.push_back(l_pcCheckBox_ElementCount);

	m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
}


void
SP_ExportLatex::AddAttributes_BasicsLayout()
{
    m_pcSizer_Basics_ReportLayout = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageBasics, wxT("Report Layout"));
    m_pcRightSizer_Basics->Add(m_pcSizer_Basics_ReportLayout, 1, wxALL | wxEXPAND, 0);

    wxSizer* l_pcSizerOrientation1 = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* l_pcStaticText_Orientation = new wxStaticText(m_pcNotebookPageBasics, -1, wxT("Orientation"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    m_pcSizer_Basics_ReportLayout->Add(l_pcStaticText_Orientation, 0, wxALL, 2);

    wxRadioButton* l_pcRB_Landscape = new wxRadioButton(m_pcNotebookPageBasics, -1, wxT("Landscape"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton* l_pcRB_Portrait = new wxRadioButton(m_pcNotebookPageBasics, -1, wxT("Portrait"));
    l_pcRB_Portrait->SetValue(true);

    l_pcSizerOrientation1->Add(10, 0);
    l_pcSizerOrientation1->Add(l_pcRB_Landscape, 0, wxALL, 2);
    l_pcSizerOrientation1->Add(l_pcRB_Portrait, 0, wxALL, 2);

    m_pcSizer_Basics_ReportLayout->Add(l_pcSizerOrientation1, 0, wxALL, 5);

	wxCheckBox* l_pcCheckBox_TitlePage = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Title Page") );
	l_pcCheckBox_TitlePage->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_TitlePage, 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_TitlePage);

	wxCheckBox* l_pcCheckBox_InsertPageNum = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Insert Page Numbers") );
	l_pcCheckBox_InsertPageNum->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_InsertPageNum, 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_InsertPageNum);

	wxCheckBox* l_pcCheckBox_InsertDate = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Insert Date") );
	l_pcCheckBox_InsertDate->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_InsertDate, 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_InsertDate);

	wxCheckBox* l_pcCheckBox_InsertTime = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Insert Time") );
	l_pcCheckBox_InsertTime->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_InsertTime, 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_InsertTime);

	//header
	wxCheckBox* l_pcCheckBox_Header = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Header") );
	l_pcCheckBox_Header->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_Header, 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_Header);

	wxSizer* l_pcSizerHeader = new wxBoxSizer(wxHORIZONTAL);
	m_pcSizer_Basics_ReportLayout->Add(l_pcSizerHeader, 0, wxALL | wxEXPAND, 0);
	wxTextCtrl* l_pcTextCtrl_Header = new wxTextCtrl( m_pcNotebookPageBasics, -1, p_pcDoc->GetUserReadableName() );
	l_pcSizerHeader->Add(20, 0);
	l_pcSizerHeader->Add(l_pcTextCtrl_Header, 1, wxALL | wxEXPAND, 2);
	m_pcTextCtrlHeaderFooter.push_back(l_pcTextCtrl_Header);

	//sizer
	wxCheckBox* l_pcCheckBox_Footer = new wxCheckBox( m_pcNotebookPageBasics, -1, wxT("Footer") );
	l_pcCheckBox_Footer->SetValue(true);
	m_pcSizer_Basics_ReportLayout->Add(l_pcCheckBox_Footer , 0, wxALL, 5);
	m_pcCheckBox_BasicsLayout.push_back(l_pcCheckBox_Footer);

	wxSizer* l_pcSizerFooter = new wxBoxSizer(wxHORIZONTAL);
	m_pcSizer_Basics_ReportLayout->Add(l_pcSizerFooter, 0, wxALL | wxEXPAND, 0);
	wxTextCtrl* l_pcTextCtrl_Footer = new wxTextCtrl( m_pcNotebookPageBasics, -1, m_pcMeta->GetAttribute(wxT("Authors"))->GetValueString() );
	l_pcSizerFooter->Add(20, 0);
	l_pcSizerFooter->Add(l_pcTextCtrl_Footer, 1, wxALL | wxEXPAND, 2);
	m_pcTextCtrlHeaderFooter.push_back(l_pcTextCtrl_Footer);


	m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);

}

void
SP_ExportLatex::AddAttributes_BasicsTypography()
{
	m_pcSizer_Basics_ReportTypography = new wxStaticBoxSizer(wxVERTICAL, m_pcNotebookPageBasics, wxT("Report Typography"));
	m_pcRightSizer_Basics->Add(m_pcSizer_Basics_ReportTypography, 1, wxALL | wxEXPAND, 0);

	//wxStaticText* l_pcStaticText_Font = new wxStaticText(m_pcNotebookPageBasics, -1, wxT("Report Font"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_pcSizer_Basics_ReportTypography->Add(new wxStaticText(m_pcNotebookPageBasics, -1, wxT("Report Font") ), 0, wxALL, 5);

	wxSizer* l_pcSizerFontFamily = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSizerFontSize = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSizerFontStyle = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSizerPaperSize = new wxBoxSizer(wxHORIZONTAL);

	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontFamily, 0, wxALL, 0);
	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontSize, 0, wxALL, 0);
	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontStyle, 0, wxALL, 0);
	m_pcSizer_Basics_ReportTypography->Add(20, 20);
	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerPaperSize, 0, wxALL, 0);


	//Font Family
	l_pcSizerFontFamily->Add(10, 0);
	l_pcSizerFontFamily->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Family:") ), 0, wxALL, 5);
	m_pcComboBox_FontFamily = new wxComboBox(m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontFamily->Set(m_Basics_FontFamily_Strings);
	m_pcComboBox_FontFamily->SetValue("Roman (rm)");
	l_pcSizerFontFamily->Add(m_pcComboBox_FontFamily, 0, wxALL, 5);

	//Font Size
	l_pcSizerFontSize->Add(10, 0);
	l_pcSizerFontSize->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Size:") ), 0, wxALL, 5);
	m_pcComboBox_FontSize = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontSize->Set(m_Basics_FontSize_Strings);
	m_pcComboBox_FontSize->SetValue("normalsize");
	l_pcSizerFontSize->Add(m_pcComboBox_FontSize, 0, wxALL, 5);

	l_pcSizerFontStyle->Add(10, 0);
	l_pcSizerFontStyle->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Style:") ), 0, wxALL, 5);
	m_pcComboBox_FontStyle = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontStyle->Set(m_Basics_FontStyle_Strings);
	m_pcComboBox_FontStyle->SetValue("normal font");
	l_pcSizerFontStyle->Add(m_pcComboBox_FontStyle, 0, wxALL, 5);

	l_pcSizerPaperSize->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Paper Size:") ), 0, wxALL, 5);
	m_pcComboBox_PaperSize = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_PaperSize->Set(m_Basics_PaperSize_Strings);
	m_pcComboBox_PaperSize->SetValue("a4paper");
	l_pcSizerPaperSize->Add(m_pcComboBox_PaperSize, 0, wxALL, 5);


	m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);

}

void
SP_ExportLatex::UpdateRightPanel_Basics(int id)
{
	switch( id ) {

	case 0:
		SP_LOGMESSAGE("General Info prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, true, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);
		//m_pcRightSizer_Basics->Hide(m_pcSizer_Basics_NetInfo, true);
		//m_pcRightSizer_Basics->Add(m_pcSizer_Basics_GeneralInfo, 1, wxALL, 5);
		break;

	case 1:
		SP_LOGMESSAGE("Net Info prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, true, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);
		//m_pcRightSizer_Basics->Hide(m_pcSizer_Basics_GeneralInfo, true);
		//m_pcRightSizer_Basics->Add(m_pcSizer_Basics_NetInfo, 1, wxALL, 5);
		break;

	case 2:
		SP_LOGMESSAGE("Report Layout prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, true, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);
		//m_pcRightSizer_Basics->Hide(m_pcSizer_Basics_ReportLayout, true);
		break;

	case 3:
		SP_LOGMESSAGE("Report Typography prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, true, true);
		//m_pcRightSizer_Basics->Hide(m_pcSizer_Basics_ReportTypography, true);
		break;

	default:
		SP_LOGMESSAGE("Default prompt");
	}
	m_pcRightSizer_Basics->Layout();
}

void
SP_ExportLatex::UpdateRightPanel_Graph( int id )
{
	map<int, wxSizer* >::const_iterator itN = SP_Node2Sizer.begin();

	for(; itN != SP_Node2Sizer.end(); itN++)
	{
		if((*itN).first == id) {
			SP_LOGMESSAGE( wxT("Updating sizer for: ") + SP_Index2Node[ id ] );
			m_pcRightSizer_Graph->Show( (*itN).second, true, true);

		} else {
			m_pcRightSizer_Graph->Show( (*itN).second, false, true);
		}
	}

	m_pcRightSizer_Graph->Layout();

}

void
SP_ExportLatex::UpdateRightPanel_Declarations(int id)
{

	map<int, wxSizer* >::const_iterator itN = SP_DecNode2Sizer.begin();

	for(; itN != SP_DecNode2Sizer.end(); itN++)
	{
		if( (*itN).first == id ) {
			SP_LOGMESSAGE( wxT("Updating sizer for: ") + SP_Index2DecNode[ id ] );
			m_pcRightSizer_Declarations->Show( (*itN).second, true, true);

		} else {
			m_pcRightSizer_Declarations->Show( (*itN).second, false, true);
		}
	}

	m_pcRightSizer_Declarations->Layout();
}

void
SP_ExportLatex::OnSelChange_Basics(wxCommandEvent& p_cEvent)
{
	//l_pcOptions_basics_order = m_pcRearrangelist_basics->GetCurrentOrder();
	//UpdateRightPanel_Basics( l_pcOptions_basics_order[ p_cEvent.GetInt() ] );
	UpdateRightPanel_Basics( m_pcRearrangelist_basics->GetCurrentOrder()[ p_cEvent.GetInt() ] );
}

void
SP_ExportLatex::OnSelChange_Graph(wxCommandEvent& p_cEvent)
{
	m_nSelectionId_Graph = m_pcRearrangelist_Graph->GetCurrentOrder()[ p_cEvent.GetInt() ];
	UpdateRightPanel_Graph( m_nSelectionId_Graph );
}

void
SP_ExportLatex::OnSelChange_Declarations(wxCommandEvent& p_cEvent)
{
	m_nSelectionId_Declarations = m_pcRearrangelist_declarations->GetCurrentOrder()[ p_cEvent.GetInt() ];
	UpdateRightPanel_Declarations( m_nSelectionId_Declarations );
}


void
SP_ExportLatex::OnSelectClearAllItems_Graph(wxCommandEvent& p_cEvent)
{

	int id = m_nSelectionId_Graph;

	if ( SP_Node2SelectClearAllCheckBox[ id ]->Get3StateValue() == wxCHK_CHECKED) {
		for (unsigned int i = 0; i < SP_Node2NodeCheckList[ id ]->GetCount() ; i++) {
					SP_Node2NodeCheckList[ id ]->Check(i, true);
		}
	} else {
		if (SP_Node2SelectClearAllCheckBox[ id ]->Get3StateValue() == wxCHK_UNCHECKED) {
			for (unsigned int i = 0; i < SP_Node2NodeCheckList[ id ]->GetCount(); i++) {
				SP_Node2NodeCheckList[ id ]->Check(i, false);
			}
		}
	}
}

void
SP_ExportLatex::OnSelectClearAllItems_Dec(wxCommandEvent& p_cEvent)
{
	int id = m_nSelectionId_Declarations;

	if ( SP_DecNode2SelectClearAllCheckBox[ id ]->Get3StateValue() == wxCHK_CHECKED) {
		for (unsigned int i = 0; i < SP_DecNode2NodeCheckList[ id ]->GetCount() ; i++) {
					SP_DecNode2NodeCheckList[ id ]->Check(i, true);
		}
	} else {
		if (SP_DecNode2SelectClearAllCheckBox[ id ]->Get3StateValue() == wxCHK_UNCHECKED) {
			for (unsigned int i = 0; i < SP_DecNode2NodeCheckList[ id ]->GetCount(); i++) {
				SP_DecNode2NodeCheckList[ id ]->Check(i, false);
			}
		}
	}
}

wxTreeItemId
SP_ExportLatex::FindTreeItemRec(const wxTreeItemId& p_Id, wxString label)
{
  wxTreeItemIdValue cookie= NULL;
  wxTreeItemId l_nId = m_pcCoarseTreectrl->GetFirstChild(p_Id, cookie);
  wxTreeItemId found = wxTreeItemId();

  while (l_nId.IsOk()) {
      if ( m_pcCoarseTreectrl->GetItemText( l_nId ).compare( label) == 0) {
    	     SP_LOGMESSAGE( wxT("Found item for:") + m_pcCoarseTreectrl->GetItemText( l_nId ) );
    	  	 found = l_nId;
    	  	 break;
      } else {
    	  found = FindTreeItemRec(l_nId, label);

    	  if (found.IsOk()) break;
      }
      l_nId = m_pcCoarseTreectrl->GetNextChild(p_Id, cookie);
  }
  return found;
}



bool
SP_ExportLatex::StartDoc(const wxString& p_fileName)
{
	m_bitmapCount = 0;
	m_sFilePath = p_fileName.BeforeLast('/') + wxT("/");

	SetUpPrintData(m_printData, p_fileName);

/*	if (m_printData.GetFilename() == wxT(""))
	{
		wxString filename = wxFileName::CreateTempFileName(wxT("tex"));
		SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"), filename.c_str()));
		m_printData.SetFilename(filename);
	}
*/
	m_pstream = wxFopen(m_printData.GetFilename().c_str(), wxT("w+"));

	if (!m_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
//		m_ok = FALSE;
		return FALSE;
	}

//	m_ok = TRUE;
	InputNetname = EditStringforLatex( p_pcDoc->GetUserReadableName() );

	wxFprintf(m_pstream, wxT("\\documentclass[pdftex,12pt,a4paper]{article}\n") );
	wxFprintf(m_pstream, wxT("\n") );
	wxFprintf(m_pstream, wxT("\\usepackage[ddmmyyyy]{datetime}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{url}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{hyperref}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{lastpage}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{multirow}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{adjustbox}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{longtable}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{pgf}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{tikz}\n") );
	wxFprintf(m_pstream, wxT("\\usetikzlibrary{positioning,arrows,shapes,backgrounds,calc,patterns}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{fancyhdr}\n") );
	wxFprintf(m_pstream, wxT("\\pagestyle{fancy}\n") );
	wxFprintf(m_pstream, wxT("\n") );
	wxFprintf(m_pstream, wxT("\\newlength{\\imagewidth}\n") );
	wxFprintf(m_pstream, wxT("\\newlength{\\imagescale}\n") );
	wxFprintf(m_pstream, wxT("\\pagestyle{empty}\n") );
	wxFprintf(m_pstream, wxT("\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{amssymb}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{pifont}\n") );
	wxFprintf(m_pstream, wxT("\\newcommand{\\cmark}{\\ding{51}}\n") );
	wxFprintf(m_pstream, wxT("\\newcommand{\\xmark}{\\ding{55}}\n") );

	wxString out;

	//Set font family
	switch( m_pcComboBox_FontFamily->GetSelection() )
	{
		case 0: out = wxT("\\renewcommand*{\\familydefault}{\\rmdefault}");
				break;

		case 1: out = wxT("\\renewcommand*{\\familydefault}{\\sfdefault}");
				break;

		case 3: out = wxT("\\renewcommand*{\\familydefault}{\\ttdefault}");
				break;
	}

	//Set paper size
	out = wxT("\\usepackage[top=1.0in, ") + m_pcComboBox_PaperSize->GetValue() + wxT("]{geometry}\n\n");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\\begin{document}\n\n") );

	return true;
}

bool
SP_ExportLatex::WriteLatex()
{

	//Title page
	if( m_pcCheckBox_BasicsLayout[0]->GetValue() ) {
		WriteTitlePage();
	}

	//Basic Page Layout
	wxFprintf(m_pstream, wxT("\\pagestyle{fancy}\n") );
	wxFprintf(m_pstream, wxT("\\fancyhf{}\n") );
	wxFprintf(m_pstream, wxT("\\rhead{snoopy2\\LaTeX}\n") );

	//Insert Date to footer
	if( m_pcCheckBox_BasicsLayout[2]->GetValue() && m_pcCheckBox_BasicsLayout[3]->GetValue()) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\today \\hspace{1pt} \\currenttime}\n") );
	} else if( m_pcCheckBox_BasicsLayout[2]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\today}\n") );
	} else if( m_pcCheckBox_BasicsLayout[3]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\currenttime}\n") );
	}

	wxString out;

	//Insert user-defined header
	if( m_pcCheckBox_BasicsLayout[4]->GetValue() ) {
		out = wxT("\\lhead{ ");
		out += EditStringforLatex( m_pcTextCtrlHeaderFooter[0]->GetValue() ) + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	} else {
		out = wxT("\\lhead{ ");
		out += InputNetname + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}

	//Insert user-defined footer
	if( m_pcCheckBox_BasicsLayout[5]->GetValue() ) {
		out = wxT("\\cfoot{ ");
		out += EditStringforLatex( m_pcTextCtrlHeaderFooter[1]->GetValue() ) + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}


	//Disclaimer
	wxFprintf(m_pstream, wxT("\\newpage\n") );
	wxFprintf(m_pstream, wxT("\\begin{center}\n\t") );

	wxFprintf(m_pstream, wxT("\\vspace*{20cm}\n\t") );
	wxFprintf(m_pstream, wxT("This document has been generated with Snoopy \\cite{heiner:pn:2012,Rohr:bi:2010}. \\\\ \n\t") );
	wxFprintf(m_pstream, wxT("If you have any comments or suggestions, \\\\ \n\t") );
	wxFprintf(m_pstream, wxT("Please contact \\url{snoopy@informatik.tu-cottbus.de}\n\t") );

	wxFprintf(m_pstream, wxT("\\end{center}\n\n") );

	//Add table of contents
	wxFprintf(m_pstream, wxT("\\newpage\n") );
	wxFprintf(m_pstream, wxT("\\tableofcontents\n\n") );

	////Insert Page number to footer
	if( m_pcCheckBox_BasicsLayout[1]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\pagenumbering{arabic}\n") );
		wxFprintf(m_pstream, wxT("\\rfoot{Page \\thepage \\hspace{1pt} of \\pageref{LastPage} }\n\n") );
	}


	//Add Basics section
	WriteBasics();

	//Add Graph Elements section
    WriteGraphElements();

    //Add Declarations section
    WriteDeclarations();

    //Add Hierarchy section
    WriteHierarchy();

	//Add references page
	WriteReferences();

	return true;
}

bool
SP_ExportLatex::WriteTitlePage()
{
	SP_LOGMESSAGE( wxT("Inside write title...") );

	wxFprintf(m_pstream, wxT("\\begin{titlepage}\n\n\t") );

	wxFprintf(m_pstream, wxT("\\thispagestyle{fancy}\n\t") );
	wxFprintf(m_pstream, wxT("\\fancyhf{}\n\t") );
	wxFprintf(m_pstream, wxT("\\rhead{snoopy2\\LaTeX}\n\t") );

	wxString out = wxT("\\lhead{");
	out << InputNetname + wxT("}\n\t");
	out << wxT("\\cfoot{\\today}\n\n\t");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\\begin{center}\n\n\t") );
	wxFprintf(m_pstream, wxT("\\vspace*{4cm}\n\t") );

	out = wxT("\\includegraphics[width=0.20\\textwidth]");
	out += wxT("{/Users/anjali/workspace/snoopy/images/snoopy_logo.png}~\\\\[1cm]\n\t");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\\vspace{5mm}\n\t") );
	wxFprintf(m_pstream, wxT("\\hrule\n\t") );
	wxFprintf(m_pstream, wxT("\\vspace{2mm}\n\t") );

	out = wxT("\\huge{\\bfseries ");
	out += InputNetname + wxT("}\n\t");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\\vspace{1.4mm}\n\t") );
	wxFprintf(m_pstream, wxT("\\hrule\n\t") );
	wxFprintf(m_pstream, wxT("\\vspace{8mm}\n\t") );
	wxFprintf(m_pstream, wxT("\\small {\\url {http://www-dssz.informatik.tu-cottbus.de/DSSZ/Software/Snoopy} }\n") );

	wxFprintf(m_pstream, wxT("\\end{center}\n\n") );
	wxFprintf(m_pstream, wxT("\\end{titlepage}\n\n") );

	return true;
}

bool
SP_ExportLatex::WriteBasics()
{
	const wxString latexBasics_file = m_sFilePath + wxT("Basics.tex");
	SP_LOGMESSAGE( latexBasics_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexBasics_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}


	wxFprintf(l_pstream, wxT("\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\section{Basics}\n") );
	wxFprintf(l_pstream, wxT("This section contains basic information about the input net.") );

    //General Information

	wxFprintf(l_pstream, wxT("\\subsection{General Informations}\n") );
	wxFprintf(l_pstream, wxT("\\vspace{5mm}\n") );
	wxFprintf(l_pstream, wxT("\\noindent\n") );

	wxString out;

	//File name
	if( m_pcCheckBox_BasicsGeneral[0]->GetValue() ) {
		out = wxT("{\\textbf{File Name:} ");
		out += InputNetname + wxT("\\\\ \n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//File path
	if( m_pcCheckBox_BasicsGeneral[1]->GetValue() ) {
		out = wxT("\\textbf{File Path:} ");
	    out += EditStringforLatex( p_pcDoc->GetFilename() ) + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Authors
	if( m_pcCheckBox_BasicsGeneral[2]->GetValue() ) {
	    out = wxT("\\textbf{Authors:} ");
	    out += m_pcMeta->GetAttribute(wxT("Authors"))->GetValueString() + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Creation Timestamp
	if( m_pcCheckBox_BasicsGeneral[3]->GetValue() ) {
	    out = wxT("\\textbf{Creation Timestamp:} ");
	    out += m_pcMeta->GetAttribute(wxT("Created"))->GetValueString() + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Description
	if( m_pcCheckBox_BasicsGeneral[4]->GetValue() ) {
		out = wxT("\\textbf{Description:} ");
	    out += m_pcMeta->GetAttribute(wxT("Description"))->GetValueString() + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Keywords
	if( m_pcCheckBox_BasicsGeneral[5]->GetValue() ) {
		out = wxT("\\textbf{Keywords:} ");
	    out += m_pcMeta->GetAttribute(wxT("Keywords"))->GetValueString() + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//References
	if( m_pcCheckBox_BasicsGeneral[6]->GetValue() ) {

		//load references to export: add to map
		wxString references = m_pcMeta->GetAttribute(wxT("References"))->GetValueString();

		int found = references.find(wxT("bibitem"));
		wxString label;
		int found2;
		int key = 2;

		while(found != wxString::npos) {
			found2 = references.find(wxT("bibitem"), found + 1);

			//add label of found reference to map SP_LatexReferencesIndex2Ref
			label = "";
			int i = found;

			for(; i < references.size() && references.at(i) != '}';) {

				if( references.at(i) == '{') {
					i++;
					while( references.at(i) != '}' ) {
						label += references.substr(i, 1);
						i++;
					};
				} else
					i++;
			}

			if(found2 != wxString::npos) {
				out = references.SubString(i + 1, found2-2);
			} else {
				out = references.SubString(i + 1, references.length()-1);
			}
			SP_LatexReferencesIndex2Ref.insert(pair< pair<int, wxString>, wxString> ( make_pair(++key, label), out ) );

			found = found2;
		};

		//////////////////////////////
		out = wxT("\\textbf{References:} \\cite{ ");

		map< pair<int, wxString>, wxString>::iterator it;
		for(it = SP_LatexReferencesIndex2Ref.begin(); it != SP_LatexReferencesIndex2Ref.end();) {
			out += (*it).first.second;  //add label to cite
			it++;

			if(it != SP_LatexReferencesIndex2Ref.end() ) {
				out += wxT(", ");
			}
		}

		out += wxT("}\n\n");

	    wxFprintf(l_pstream, wxT("%s"), out.c_str());

	}

	//Net Information

	wxFprintf(l_pstream, wxT("\\subsection{Net Informations}\n") );
	wxFprintf(l_pstream, wxT("\\vspace{5mm}\n\n") );


	if( m_pcCheckBox_BasicsNet[0]->GetValue() ) {
		out = wxT("\\noindent{\\textbf{Net Class: } ");
		out += p_pcDoc->GetNetclassName() + wxT("}  \\\\ \\\\ \n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	if( m_pcCheckBox_BasicsNet[1]->GetValue() ) {
		wxFprintf(l_pstream, wxT("\\begin{tabular}{l c }\n\t") );
		wxFprintf(l_pstream, wxT("\\textbf{Element} & \\textbf{Count} \\\\ \n\t") );
		wxFprintf(l_pstream, wxT("\\hline\n\t") );

		SP_ListNodeclass::const_iterator ncIt;
		for (ncIt = m_pcGraph->GetNodeclasses()->begin(); ncIt != m_pcGraph->GetNodeclasses()->end(); ++ncIt)
		{
			if ((*ncIt)->GetShowInElementTree())
			{
				if ((*ncIt)->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))
				{
					const SP_ListNode* l_pcNodeList = (*ncIt)->GetElements();
					int l_nSize = l_pcNodeList->size();
					wxString l_sName = (*ncIt)->GetDisplayName();

					out = l_sName + wxT(" & ") + wxString::Format(wxT("%i"), l_nSize) + wxT(" \\\\ \n");
					wxFprintf(l_pstream, wxT("%s"), out.c_str());
				}
			}
		}

		SP_ListEdgeclass::const_iterator ecIt;
		for (ecIt = m_pcGraph->GetEdgeclasses()->begin(); ecIt != m_pcGraph->GetEdgeclasses()->end(); ++ecIt)
		{
			const SP_ListEdge* l_pcEdgeList = (*ecIt)->GetElements();
			int l_nSize = l_pcEdgeList->size();
			wxString l_sName = (*ecIt)->GetDisplayName();

			out = l_sName + wxT(" & ") + wxString::Format(wxT("%i"), l_nSize) + wxT(" \\\\ \n");
			wxFprintf(l_pstream, wxT("%s"), out.c_str());

		}

		wxFprintf(l_pstream, wxT("\\end{tabular}\n\n") );
	}


	//////////
	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./Basics.tex}\n") );

	return true;
}

bool
SP_ExportLatex::WriteGraphElements()
{
	const wxString latexGraphEle_file = m_sFilePath + wxT("GraphElements.tex");

	SP_LOGMESSAGE( latexGraphEle_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexGraphEle_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream, wxT("\n\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\section{Graph Elements}\n") );
	wxFprintf(l_pstream, wxT("This section contains information related to graph elements specific to the net.\n") );
	//////////////////////

	for(int i = 0; i < m_pcRearrangelist_Graph->GetCount(); i++) {

		if( m_pcRearrangelist_Graph->IsChecked(i) ) {

			int order = m_pcRearrangelist_Graph->GetCurrentOrder()[ i ];

			wxString element = m_Options_Graph[ order ] + wxT("s");
			wxString latexEle_file = m_sFilePath + EditStringforLatex( element ) + wxT(".tex");

			wxPrintData *pd1 = new wxPrintData();
			SetUpPrintData(*pd1, latexEle_file);

			FILE* l_pstream1 = wxFopen( (*pd1).GetFilename().c_str(), wxT("w+"));

			if (!l_pstream1)
			{
				SP_LOGERROR(_("Cannot open file for Latex output!"));
				return FALSE;
			}

			wxFprintf(l_pstream1, wxT("\\subsection{") + element + wxT("}\n") );
			wxFprintf(l_pstream1, wxT("\\begin{center}\n") );

			//-----------

			//create table for subsection

			wxArrayInt l_nArrayIntAttributes;
			wxCheckListBox* l_pcCheckListAttributes = SP_Node2AttributeCheckList[ order ];
			int l_nAttributes = l_pcCheckListAttributes->GetCheckedItems( l_nArrayIntAttributes );

			SP_DS_Nodeclass* nodeclass = m_pcGraph->GetNodeclassByDisplayedName( m_Options_Graph[ order ] );
			SP_DS_Edgeclass* edgeclass = m_pcGraph->GetEdgeclassByDisplayedName( m_Options_Graph[ order ] );
			SP_DS_Metadataclass* metadataclass = m_pcGraph->GetMetadataclassByDisplayedName( m_Options_Graph[ order ] );

			if( nodeclass ) {

				wxArrayInt l_nArrayIntElements;
				wxCheckListBox* l_pcCheckListElements = SP_Node2NodeCheckList[ order ];
				int l_nElements = l_pcCheckListElements->GetCheckedItems( l_nArrayIntElements );

				if( l_nAttributes && l_nElements ) { //if non-zero number of attributes and nodes selected -> draw table

					SP_LOGMESSAGE( wxT("nodeclass found: ") + nodeclass->GetDisplayName() );

					SP_ListAttribute::const_iterator itAttr;
					SP_DS_Node* nd = nodeclass->GetPrototype();
					SP_DS_Node* l_pcNode;

					wxFprintf(l_pstream1, wxT("\\emph{") + element + wxT(" Table}\n\n") );

					wxFprintf(l_pstream1, wxT("\\begin{longtable}") );
					wxString out = wxT("{ | ");

					for(int i = 0; i < l_nAttributes; i++) {
						out += wxT("l |");
					}
					out += wxT("}\n\n\t");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					wxFprintf(l_pstream1, wxT("\\hline\n\t") );

					int flag_crossRef = 0;

					out = "";
					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = nd->GetAttribute( name );

						if( sp || name.compare( wxT("Net number") ) == 0 ) {  //not cross-ref or ordering
						//	SP_LOGMESSAGE( wxT("attribute: ") + sp->GetDisplayName() );
							out += name;
							out += wxT(" & ");
						} else if( name.compare( wxT("Cross Refs.") ) == 0) {
							out += name;
							flag_crossRef = 1;
						}
					}

					if( !flag_crossRef  ) {
						out = out.BeforeLast('&');
					}

					out += wxT("\\\\ \\hline \\hline \\endhead\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());


					//popoulate table with element details
					for(int j = 0; j < l_nElements; j++) {
						out = "";
						int index = l_nArrayIntElements[ j ];
						wxString element_name = l_pcCheckListElements->GetString( index );
						l_pcNode = SP_Name2Node[ element_name ];

						if(l_pcNode) SP_LOGMESSAGE( element_name );

						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = l_pcNode->GetAttribute( name );

							if( sp ) { //not cross-ref or ordering
								wxString value = EditStringforLatex( sp->GetValueString() );

								if(i == 0) {
									out += wxT("\\multirow{1}{*}{") + value +  wxT("}&");
								} else {
									out += value + wxT("&");
								}

							} else if( name.compare( wxT("Cross Refs.") ) == 0 ) {
								//add cross-refs here
								wxString value = SP_NetNumber2Label[ wxString::Format(wxT("%i"), l_pcNode->GetNetnumber()) ];

								if(i == 0) {
									out += wxT("\\multirow{1}{*}{") + EditStringforLatex( value ) +  wxT("}&");
								} else {
									//out +=  EditStringforLatex( value ) + wxT("\\\\ \n");

									SP_Graphic* l_pcGraphics = l_pcNode->GetGraphicInSubnet( l_pcNode->GetNetnumber() );
									SP_ListGraphic l_lGraphics;
									//l_pcNode->GetCoupledGraphics(l_lGraphics, l_pcGraphics);
									//SP_ListGraphic l_lGraphics;
									l_pcNode->GetGraphicsInNet(&l_lGraphics, l_pcNode->GetNetnumber());

									SP_ListGraphic::iterator l_Iter;
									set<int> l_SetNetNumber;
									set<int>::iterator it;

									for(l_Iter = l_lGraphics.begin(); l_Iter != l_lGraphics.end(); ++l_Iter)
									{
										l_SetNetNumber.insert( (*l_Iter)->GetNetnumber() );
									}

									for(it = l_SetNetNumber.begin(); it != l_SetNetNumber.end(); it++) {

										value = SP_NetNumber2Label[ wxString::Format(wxT("%i"), (*it) ) ];

										wxString ref = "";

										if(it != l_SetNetNumber.begin()) {
											for(int k = 0; k < i; k++) ref += wxT("&");
										}

										ref += EditStringforLatex( value ) + wxT(" \\\\ \n");
										out += ref;
									}
								}

							} else if( name.compare( wxT("Net number") ) == 0 ) {
								wxString value = wxString::Format(wxT("%i"), l_pcNode->GetNetnumber());
								if(i == 0) {
									out += wxT("\\multirow{1}{*}{") + value +  wxT("}&");
								} else {
									out += value + wxT("&");
								}
							}
						}

						if( !flag_crossRef  ) {
							out = out.BeforeLast('&') + wxT("\\\\");
						}
						out += wxT("\\hline\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					}

					wxFprintf(l_pstream1, wxT("\\end{longtable}\n") );

				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}

			}

			else if( edgeclass ) {

				SP_LOGMESSAGE( wxT("Edgeclass found: ") + edgeclass->GetDisplayName() );

				SP_ListAttribute::const_iterator itAttr;
				SP_DS_Edge* nd = edgeclass->GetPrototype();

				if( l_nAttributes ) {  //if non-zero number of attributes selected -> draw table
					wxFprintf(l_pstream1, wxT("\\emph{") + element + wxT(" Table}\n\n") );

					wxFprintf(l_pstream1, wxT("\\begin{longtable}") );
					wxString out = wxT("{ | ");

					for(int i = 0; i < l_nAttributes; i++) {
						out += wxT("l |");
					}
					out += wxT("}\n\n\t");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					wxFprintf(l_pstream1, wxT("\\hline\n\t") );

					int flag_crossRef = 0;

					out = "";
					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = nd->GetAttribute( name );

						if( sp ) {  //not cross-ref or ordering
							SP_LOGMESSAGE( wxT("attribute: ") + sp->GetDisplayName() );
							out += name;
							out += wxT(" & ");
						} else if( name.compare( wxT("Cross Refs.") ) == 0) {
								out += name;
								flag_crossRef = 1;
						}
					}

					if( !flag_crossRef  ) {
						out = out.BeforeLast('&');
					}

					out += wxT("\\\\ \\hline \\hline \\endhead\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					wxFprintf(l_pstream1, wxT("\\end{longtable}\n") );
				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}

			}

			else if( metadataclass ) {

				SP_LOGMESSAGE( wxT("Metadataclass found: ") + metadataclass->GetDisplayName() );

				SP_ListAttribute::const_iterator itAttr;
				SP_DS_Metadata* nd = metadataclass->GetPrototype();
				SP_DS_Metadata* l_pcMetadata;

				if( element.compare( wxT("Comments") ) ) {
					wxArrayInt l_nArrayIntElements;
					wxCheckListBox* l_pcCheckListElements = SP_Node2NodeCheckList[ order ];
					int l_nElements = l_pcCheckListElements->GetCheckedItems( l_nArrayIntElements );

					if( l_nAttributes && l_nElements) {  //if non-zero number of attributes selected -> draw table
						wxFprintf(l_pstream1, wxT("\\emph{") + element + wxT(" Table}\n\n") );

						wxFprintf(l_pstream1, wxT("\\begin{longtable}") );
						wxString out = wxT("{ | ");

						for(int i = 0; i < l_nAttributes; i++) {
							out += wxT("l |");
						}
						out += wxT("}\n\n\t");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						wxFprintf(l_pstream1, wxT("\\hline\n\t") );

						int flag_crossRef = 0;

						out = "";
						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = nd->GetAttribute( name );

							if( sp ) {  //not cross-ref or ordering
								SP_LOGMESSAGE( wxT("attribute: ") + sp->GetDisplayName() );
								out += name;
								out += wxT(" & ");
							} else if( name.compare( wxT("Cross Refs.") ) == 0) {
									out += name;
									flag_crossRef = 1;
							}
						}

						if( !flag_crossRef  ) {
							out = out.BeforeLast('&');
						}

						out += wxT("\\\\ \\hline \\hline \\endhead\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());

						//popoulate table with element details
						for(int j = 0; j < l_nElements; j++) {
							out = "";
							int index = l_nArrayIntElements[ j ];
							wxString element_name = l_pcCheckListElements->GetString( index );
							l_pcMetadata = SP_Name2Metadata[ element_name ];

							if(l_pcMetadata) SP_LOGMESSAGE( element_name );

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( name );

								if( sp ) { //not cross-ref or ordering
									wxString value = EditStringforLatex( sp->GetValueString() );
									out += value + wxT("&");
								}
							}

							out = out.BeforeLast('&');
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}

						wxFprintf(l_pstream1, wxT("\\end{longtable}\n") );
					} else {
						wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
					}
				} else {  //Comment

					if( l_nAttributes ) {
						wxFprintf(l_pstream1, wxT("\\emph{") + element + wxT(" Table}\n\n") );

						wxFprintf(l_pstream1, wxT("\\begin{longtable}") );
						wxString out = wxT("{ | ");

						for(int i = 0; i < l_nAttributes; i++) {
							out += wxT("l |");
						}
						out += wxT("}\n\n\t");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						wxFprintf(l_pstream1, wxT("\\hline\n\t") );

						int flag_crossRef = 0;

						out = "";
						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = nd->GetAttribute( name );

							if( sp ) {  //not cross-ref or ordering
								SP_LOGMESSAGE( wxT("attribute: ") + sp->GetDisplayName() );
								out += name;
								out += wxT(" & ");
							} else if( name.compare( wxT("Cross Refs.") ) == 0) {
									out += name;
									flag_crossRef = 1;
							}
						}

						if( !flag_crossRef  ) {
							out = out.BeforeLast('&');
						}

						out += wxT("\\\\ \\hline \\hline \\endhead\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());

						//popoulate table with element details

						wxArrayString l_sComments = SP_Nodeclass2NodeList[ wxT("Comment") ];

						if( !l_sComments.IsEmpty() ) {
							SP_LOGMESSAGE( wxT("Not Empty") );
						} else {
							SP_LOGMESSAGE( wxT("Is Empty") );
						}

						int l_nComments = l_sComments.size();

						for(int i = 0; i < l_nComments; i++) {
							out = EditStringforLatex( l_sComments[i], false );
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}

						wxFprintf(l_pstream1, wxT("\\end{longtable}\n") );

					} else {
						wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
					}
				}
			}

			wxFprintf(l_pstream1, wxT("\\end{center}\n") );

			//-----------
		    wxFprintf(l_pstream, wxT("\\input{./") + EditStringforLatex( element ) + wxT(".tex }\n") );

			fclose( l_pstream1 );
			l_pstream1 = (FILE *) NULL;
			wxDELETE(pd1);

		}
	}

	//////////
	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./GraphElements.tex}\n") );

	return true;
}


bool
SP_ExportLatex::WriteDeclarations()
{
	const wxString latexDec_file = m_sFilePath + wxT("Declarations.tex");
	SP_LOGMESSAGE( latexDec_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexDec_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxArrayInt l_nArrayIntDecAttributes;
	wxArrayInt l_nArrayIntDecElements;

	wxFprintf(l_pstream, wxT("\n\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\section{Declarations}\n") );
	wxFprintf(l_pstream, wxT("This section contains information related to declarations specific to the net.") );

	//////////////////////

	for(int i = 0; i < m_pcRearrangelist_declarations->GetCount(); i++) {

		if( m_pcRearrangelist_declarations->IsChecked(i) ) {

			int order = m_pcRearrangelist_declarations->GetCurrentOrder()[ i ];

			wxString element = m_Options_Declarations[ order ];
			wxString latexDecEle_file = m_sFilePath + EditStringforLatex( element ) + wxT(".tex");

			wxPrintData *pd1 = new wxPrintData();
			SetUpPrintData(*pd1, latexDecEle_file);

			FILE* l_pstream1 = wxFopen( (*pd1).GetFilename().c_str(), wxT("w+"));

			if (!l_pstream1)
			{
				SP_LOGERROR(_("Cannot open file for Latex output!"));
				return FALSE;
			}

			wxFprintf(l_pstream1, wxT("\\subsection{") + element + wxT("}\n") );
			wxFprintf(l_pstream1, wxT("\\begin{center}\n") );

			//-----------

			//create table for subsection
			wxCheckListBox* l_pcCheckListDecAttributes = SP_DecNode2AttributeCheckList[ order ];
			wxCheckListBox* l_pcCheckListDecElements = SP_DecNode2NodeCheckList[ order ];

			int l_nAttributes = l_pcCheckListDecAttributes->GetCheckedItems( l_nArrayIntDecAttributes );
			int l_nElements = l_pcCheckListDecElements->GetCheckedItems( l_nArrayIntDecElements );

			SP_DS_Metadataclass* metadataclass = m_pcGraph->GetMetadataclassByDisplayedName( element );

			if( metadataclass ) {

				SP_LOGMESSAGE( wxT("Metadata Declaration class found: ") + metadataclass->GetDisplayName() );

				SP_ListMetadata::const_iterator l_itElem;

				SP_ListAttribute::const_iterator itAttr;
				SP_DS_Metadata* nd = metadataclass->GetPrototype();
				SP_DS_Metadata* l_pcMetadata;


				if( l_nAttributes && l_nElements ) {  //if non-zero number of attributes selected -> draw table
					wxFprintf(l_pstream1, wxT("\\emph{") + element + wxT(" Table}\n\n") );

					wxFprintf(l_pstream1, wxT("\\begin{longtable}") );
					wxString out = wxT("{ | ");

					for(int i = 0; i < l_nAttributes; i++) {
						out += wxT("l |");
					}
					out += wxT("}\n\n\t");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					wxFprintf(l_pstream1, wxT("\\hline\n\t") );

					out = "";
					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );
						SP_DS_Attribute* sp = nd->GetAttribute( name );

						if( sp ) {  //not cross-ref or ordering
							SP_LOGMESSAGE( wxT("attribute: ") + sp->GetDisplayName() );
							out += name;
							out += wxT(" & ");
						}
					}

					out = out.BeforeLast('&');

					out += wxT("\\\\ \\hline \\hline \\endhead \n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());


					//popoulate table with element details
					for(int j = 0; j < l_nElements; j++) {
						out = "";
						int index = l_nArrayIntDecElements[ j ];
						wxString element_name = l_pcCheckListDecElements->GetString( index );
						l_pcMetadata = SP_Name2Metadata[ element_name ];

						if(l_pcMetadata) SP_LOGMESSAGE( element_name );

						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );
							SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( name );

							if( sp ) { //not cross-ref or ordering
								wxString value = EditStringforLatex( sp->GetValueString() );
								out += value + wxT("&");
							} else if( name.compare( wxT("Cross Refs.") ) == 0 ) {
								//add cross-refs here
								out += wxT("&");
							}
						}

						out = out.BeforeLast('&');
						out += wxT("\\\\ \\hline\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					}

					wxFprintf(l_pstream1, wxT("\\end{longtable}\n") );
				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}
			}

			wxFprintf(l_pstream1, wxT("\\end{center}\n") );

		    wxFprintf(l_pstream, wxT("\\input{./") + EditStringforLatex( element ) + wxT(".tex }\n") );

			//-----------
			fclose( l_pstream1 );
			l_pstream1 = (FILE *) NULL;
			wxDELETE(pd1);

			l_nArrayIntDecAttributes.Clear();
			l_nArrayIntDecElements.Clear();
		}
	}

	//////////
	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./Declarations.tex}\n") );

	return true;
}

void
SP_ExportLatex::WriteHierarchylevelRec(const wxTreeItemId& p_Id, FILE* l_pstream)
{
	if(!p_Id.IsOk() ) { return; }

	wxTreeItemIdValue cookie= NULL;
	wxTreeItemId l_nId = m_pcCoarseTreectrl->GetFirstChild(p_Id, cookie);

	while( l_nId.IsOk() )
	{
		wxString level_label = m_pcCoarseTreectrl->GetItemText( l_nId );
		wxString level_id = SP_HierarchyLabel2ID[ level_label ];
		wxString label_name =  EditStringforLatex( level_id + wxT(" ") + level_label );

		//SP_LOGMESSAGE( wxT("Into WriteHierarchylevelRec(): ") + label_name );

		wxString out = wxT("\n\\newpage\n\\subsection{Level ") + label_name  + wxT("}\n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());

		label_name =  wxT("Level") + level_id + wxT("tex");

		wxString latexDC_file = m_sFilePath + label_name;

		wxPrintData *pd1 = new wxPrintData();
		pd1->SetFilename(latexDC_file);
		pd1->SetPaperId(wxPAPER_A2);
		pd1->SetOrientation(wxPORTRAIT);
		pd1->SetPrintMode(wxPRINT_MODE_FILE);

		SP_LatexDC latexDC(*pd1);
		latexDC.StartDoc(latexDC_file);

		#if wxABI_VERSION < 30000
		  latexDC.SetResolution(72);
		#endif

		//Get_Doc
		wxTreeItemData* data =  m_pcCoarseTreectrl->GetItemData( l_nId );
		SP_CoarseTreeItemdata* coarse_data = dynamic_cast<SP_CoarseTreeItemdata*>( data );
		SP_DS_Coarse* l_pcCoarse = coarse_data->GetCoarse();
		SP_MDI_Doc* l_pcDoc = l_pcCoarse->GetCoarseDoc();

		if(!l_pcDoc)
		{
			l_pcCoarse->SetUpdate(false);
			l_pcCoarse->Show();
			l_pcDoc = l_pcCoarse->GetCoarseDoc();
			l_pcDoc->SetClose(false);
			l_pcDoc->Modify(FALSE);
			l_pcDoc->Close();
		}

		wxNode *node = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();

		while (node) {
			wxShape *shape = (wxShape*)node->GetData();
			//shape->Select(FALSE);
			shape->Draw(latexDC);
			node = node->GetNext();
		};

		latexDC.EndDoc();

		wxDELETE(pd1);
		wxDELETE(node);

		out = wxT("\\maxsizebox{\\linewidth}{\\textheight}{\\input{./") + label_name + wxT("} }\n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());

		//////////////////////////////

		WriteHierarchylevelRec(l_nId, l_pstream);

		l_nId = m_pcCoarseTreectrl->GetNextChild(p_Id, cookie);
	};

}

bool
SP_ExportLatex::WriteHierarchy()
{
	const wxString latexHierarchy_file = m_sFilePath + wxT("Hierarchy.tex");
	SP_LOGMESSAGE( latexHierarchy_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexHierarchy_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\section{Hierarchy}\n") );
	wxFprintf(l_pstream, wxT("This section contains information about the net hierarchy.") );


	///////////////////////////////////
    wxTreeItemId local_tree = l_pcTree->GetFocusedItem();

	wxString local_label = l_pcTree->GetItemText( local_tree );
	wxString label = local_label.AfterFirst(' ');

	wxTreeItemId main_treeItem = m_pcCoarseTreectrl->GetRootItem();
	SP_MDI_Doc* l_pcDoc;

	if( m_pcCoarseTreectrl->GetItemText( main_treeItem ).compare( label ) == 0 )
	{
		l_pcDoc = m_pcGraph->GetParentDoc();
	}
	else
	{
		main_treeItem = FindTreeItemRec(m_pcCoarseTreectrl->GetRootItem(), label);

		wxTreeItemData* data =  m_pcCoarseTreectrl->GetItemData( main_treeItem );
		SP_CoarseTreeItemdata* coarse_data = dynamic_cast<SP_CoarseTreeItemdata*>( data );
		SP_DS_Coarse* l_pcCoarse = coarse_data->GetCoarse();
		l_pcDoc = l_pcCoarse->GetCoarseDoc();

		if(!l_pcDoc)
		{
			l_pcCoarse->SetUpdate(false);
			l_pcCoarse->Show();
			l_pcDoc = l_pcCoarse->GetCoarseDoc();
			l_pcDoc->SetClose(false);
			l_pcDoc->Modify(FALSE);
			l_pcDoc->Close();
		}
	}

	wxString level_label = m_pcCoarseTreectrl->GetItemText( main_treeItem );
	wxString level_id = SP_HierarchyLabel2ID[ level_label ];
	wxString label_name =  EditStringforLatex( level_id + wxT(" ") + level_label );

	//SP_LOGMESSAGE( wxT("Start WriteHierarchylevelRec(): ") + label_name );

	wxString out = wxT("\n\\newpage\n\\subsection{Level ") + label_name  + wxT("}\n");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	label_name =  wxT("Level") + level_id + wxT("tex");

	wxString latexDC_file = m_sFilePath + label_name;

	wxPrintData *pd1 = new wxPrintData();
	pd1->SetFilename(latexDC_file);
	pd1->SetPaperId(wxPAPER_A2);
	pd1->SetOrientation(wxPORTRAIT);
	pd1->SetPrintMode(wxPRINT_MODE_FILE);

	SP_LatexDC latexDC(*pd1);
	latexDC.StartDoc(latexDC_file);

	#if wxABI_VERSION < 30000
	  latexDC.SetResolution(72);
	#endif

	wxNode *node = l_pcDoc->GetDiagram()->GetShapeList()->GetFirst();

	while (node) {
		wxShape *shape = (wxShape*)node->GetData();
		//shape->Select(FALSE);
		shape->Draw(latexDC);
		node = node->GetNext();
	};

	latexDC.EndDoc();

	wxDELETE(pd1);
	wxDELETE(node);

	out = wxT("\\maxsizebox{\\linewidth}{\\textheight}{\\input{./") + label_name + wxT("} }\n");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	WriteHierarchylevelRec(main_treeItem, l_pstream);

	/////////////////////////////////////

	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./Hierarchy.tex }\n") );

	return true;
}

bool
SP_ExportLatex::WriteReferences()
{
	wxFprintf(m_pstream, wxT("\n\\newpage\n") );
	wxFprintf(m_pstream, wxT("\\addcontentsline{toc}{section}{References}\n") );
	wxFprintf(m_pstream, wxT("\\begin{thebibliography}{0}\n\n") );

	//Two basic references

	wxFprintf(m_pstream, wxT("\\bibitem{heiner:pn:2012}\n") );
	wxString out = wxT("M. Heiner, M. Herajy, F. Liu, C. Rohr and M. Schwarick.\n"
			"Snoopy a unifying Petri net tool. \n"
			"Proc. PETRI NETS 2012, Volume 7347, Springer, June 2012, pages 398-407.\n\n");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());
//	SP_LatexReferencesIndex2Ref.insert(pair< pair<int, wxString>, wxString> ( make_pair(1, wxT("heiner:pn:2012") ), out ) );

	wxFprintf(m_pstream, wxT("\\bibitem{Rohr:bi:2010}\n") );
	out = wxT("C. Rohr, W. Marwan and M. Hiener.\n"
			"Snoopy - a unifying Perti net framework to investigate biomolecular networks. \n"
			"Bioinformatics, 26(7):974-975, 2010. \n\n");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());
//	SP_LatexReferencesIndex2Ref.insert(pair< pair<int, wxString>, wxString> ( make_pair(2, wxT("Rohr:bi:2010") ), out ) );

	//adding dynamic references
	map< pair<int, wxString>, wxString>::iterator it;
	for(it = SP_LatexReferencesIndex2Ref.begin(); it != SP_LatexReferencesIndex2Ref.end(); it++) {
		 out = wxT("\\bibitem{") + (*it).first.second + wxT("}\n");  //add label to cite
		 out += (*it).second;
		 wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}

	wxFprintf(m_pstream, wxT("\\end{thebibliography}\n\n") );

	return true;
}

void
SP_ExportLatex::EndDoc()
{
	wxFprintf(m_pstream, wxT("\n\\end{document}\n\n"));

	fclose(m_pstream);
	m_pstream = (FILE *) NULL;

	SP_LOGMESSAGE(wxT("End of end doc of export latex.."));
}

bool
SP_ExportLatex::SetUpPrintData(wxPrintData& pd, const wxString& p_fileName)
{
	pd.SetFilename(p_fileName);
	pd.SetPaperId(wxPAPER_A2);
	pd.SetOrientation(wxPORTRAIT);
	pd.SetPrintMode(wxPRINT_MODE_FILE);

	if (pd.GetFilename() == wxT(""))
	{
		wxString filename = wxFileName::CreateTempFileName(wxT("tex"));
		SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"), filename.c_str()));
		pd.SetFilename(filename);
	}

	return true;
}

wxString
SP_ExportLatex::EditStringforLatex(wxString filename,  bool remove_space)
{
	//This function appends backslash to underscores in strings for inserting in latex code

	for(int i = 0; i < filename.length(); i++) {
		if(filename[i] == '_') {
			filename = filename.SubString(0, i-1) + wxT("\\") + filename.SubString(i, filename.Length()-1);
			i++;
		}

		if(remove_space && filename[i] == ' ') {
			filename.Remove(i, 1);
		}
	}

	return filename;
}


