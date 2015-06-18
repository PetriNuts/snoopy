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

#include <wx/wx.h>
#include <wx/regex.h>

#include <algorithm>     //for sort() function
#include <cstdio>
#include <cstdlib>
#include <fstream>


enum {
	SP_GENERATEPDF_CHECKBOX_SELECTED = SP_ID_LAST_ID + 1,

	SP_ID_GENERAL_PDFLATEX,
	SP_ID_GENERAL_LATEXMK,

	SP_ID_BASICS_UPDATE,
	SP_ID_BASICS_CHECK,
	SP_ID_BUTTON_BASICS_UP,
	SP_ID_BUTTON_BASICS_DOWN,

	SP_ID_DECLARATIONS_UPDATE,
	SP_ID_DECLARATIONS_CHECK,
	SP_ID_BUTTON_DECLARATIONS_UP,
	SP_ID_BUTTON_DECLARATIONS_DOWN,

	SP_ID_GRAPH_UPDATE,
	SP_ID_GRAPH_CHECK,
	SP_ID_BUTTON_GRAPH_UP,
	SP_ID_BUTTON_GRAPH_DOWN,

	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH,
	SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_DEC,

	SP_REGEX_GRAPH,
	SP_REGEX_DEC
};

SP_ExportLatex::SP_ExportLatex()
{
	//General tab options
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
	m_pcTree = NULL;
	m_pcTree2 = NULL;

	m_flagImages = 0;
	m_nflagIncludeSubtrees = 0;

}

bool
SP_ExportLatex::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{
	SP_LOGMESSAGE("inside AddToDialog for SP_ExportLatex");

	//Change notebook size for Export to Latex
	p_pcDlg->SetNotebookSize(550, 380);

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
    SP_LatexGlossaryAbbr2Full.clear();
    m_pcTextCtrlHeaderFooter.clear();
    SP_HierarchyLabel2ID.clear();

    SP_Name2Node.clear();
    SP_Name2Edge.clear();
    SP_Name2Metadata.clear();
    SP_NetNumber2Label.clear();

    m_pEdgesPlace2Transition.clear();
    m_pEdgesTransition2Place.clear();
    SP_Edgeclass2EdgeList.clear();

    m_nSelectionId_Graph = 0;
    m_nSelectionId_Declarations = 0;

    SP_Node2AttrNameMap.clear();
    SP_DecNode2DecAttrNameMap.clear();

    //Load net elements into code
	LoadData();

	//Add tabs with net elements to Export Dialog
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

  ///////////// Export to Latex ////////////////
  wxPrintData *pd = new wxPrintData();
  m_printData = *pd;

  StartDoc(p_fileName);
  WriteLatex();
  EndDoc();

  wxDELETE(pd);


  ///////////// Export to PDF ////////////////

  if(m_pcCheckBoxDirectPDF->IsChecked() ) {

	  wxString l_sFilePath = m_sFilePath;

	  //Sets the latex file directory as current working directory
	  if( wxSetWorkingDirectory( l_sFilePath ) ) {
		  SP_LOGMESSAGE( wxT("Current working directory set.") );
	  } else {
		  SP_LOGMESSAGE( wxT("Failed to set Current working directory.") );
	  }

	  /* Make a new folder AUX to store auxiliary files
	   * generated during PDF generation by Latex compiler
	   */
	  if( wxMkdir( wxT("AUX") ) ) {
		  SP_LOGMESSAGE( wxT("New folder created in ") + wxGetCwd() );
	  } else {
		  SP_LOGMESSAGE( wxT("Failed to create AUX folder. Already exists.") );
	  }

	  wxString l_sMainFileName = m_printData.GetFilename();
	  l_sMainFileName = l_sMainFileName.AfterLast('/');

	  for(int i = 0; i < l_sMainFileName.length(); i++) {
		  if(l_sMainFileName[i] == ' ') {
			  l_sMainFileName = l_sMainFileName.SubString(0, i-1) + wxT("\\") + l_sMainFileName.SubString(i, l_sMainFileName.Length()-1);
			  i++;
		  }
	  }

	  wxString command;
	  int l_pcLog;
	  m_sCompilerPath = l_pcFilePickerCtrl1->GetPath();
	  SP_LOGMESSAGE( wxT("Latex Compiler Path: ") + m_sCompilerPath);

	  if( m_rbPdfLatex->GetValue() ) {

		  command = m_sCompilerPath + wxT(" -interaction=nonstopmode -output-directory ./AUX ") + l_sMainFileName;
		  l_pcLog = wxExecute(command, wxEXEC_SYNC);

		  command = m_sCompilerPath + wxT(" -interaction=nonstopmode -output-directory ./AUX ") + l_sMainFileName;
		  l_pcLog = wxExecute(command, wxEXEC_SYNC);

		  command = m_sCompilerPath + wxT(" -interaction=nonstopmode -output-directory ./AUX ") + l_sMainFileName;
		  l_pcLog = wxExecute(command, wxEXEC_SYNC);

	  } else {

		  command = m_sCompilerPath + wxT(" -pdf -pdflatex=\"")
				  + m_sCompilerPath.BeforeLast('/') + wxT("/pdflatex")
				  + wxT(" --shell-escape %O %S\" -jobname=")
				  + l_sMainFileName.BeforeLast('.')
				  + wxT(" -output-directory=./AUX ")
				  + l_sMainFileName;

		  l_pcLog = wxExecute(command, wxEXEC_SYNC);
	  }

	  //Moving PDF to main directory
	  command = wxT("mv ./AUX/") + l_sMainFileName.BeforeLast('.') + wxT(".pdf ./");
	  l_pcLog = wxExecute(command, wxEXEC_SYNC);
	  SP_LOGMESSAGE( wxT("Moving pdf to main directory: ") + command);

	  SP_LOGMESSAGE( wxT("Export to PDF complete...") );

  }

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


    SP_ListNodeclass::const_iterator itNC;
    SP_ListAttribute::const_iterator itAttr;

    map<wxString, wxString> l_AttrNameMap;
    map<wxString, wxString> l_DecAttrNameMap;

    int i = 0;
    wxArrayString arrays;

	//Loads list of Graph Elements for the dialog
    for (itNC = m_pcGraph->GetNodeclasses()->begin(); itNC != m_pcGraph->GetNodeclasses()->end(); ++itNC) {

    	wxString l_sNodename = (*itNC)->GetDisplayName();
    	m_Options_Graph.Add( l_sNodename );
    	m_Options_Graph_order.Add(i);

    	SP_DS_Node* l_pcNode = (*itNC)->GetPrototype();

    	if( l_pcNode ) {
    		itAttr = l_pcNode->GetAttributes()->begin();

    		arrays.Clear();
    		l_AttrNameMap.clear();

    		for(;itAttr != l_pcNode->GetAttributes()->end(); itAttr++)
    	    {
    		    arrays.Add( (*itAttr)->GetDisplayName() );
    		    l_AttrNameMap.insert( pair<wxString, wxString> ( (*itAttr)->GetDisplayName(), (*itAttr)->GetName()));
    		}

    		if( l_pcNode->GetClassName().Find( wxT("Coarse") ) != wxNOT_FOUND)   // coarse node
    		{
    			arrays.Add(wxT("Net number") );
    			l_AttrNameMap.insert( pair<wxString, wxString> ( wxT("Net number"), wxT("Net number") ));
    		}

    		arrays.Add(wxT("Cross Refs.") );
    		l_AttrNameMap.insert( pair<wxString, wxString> ( wxT("Cross Refs."), wxT("Cross Refs.") ));

    		if( l_pcNode->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
    			arrays.Add( wxT("Order Lexicographically") );
    			l_AttrNameMap.insert( pair<wxString, wxString> (  wxT("Order Lexicographically"), wxT("Order Lexicographically") ));
    		}

    		SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
    		SP_Node2AttrNameMap.insert(pair<wxString, map<wxString, wxString> > (l_sNodename, l_AttrNameMap) );

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
        	l_AttrNameMap.clear();

        	for(itAttr = l_pcEdge->GetAttributes()->begin();itAttr != l_pcEdge->GetAttributes()->end(); itAttr++)
        	{
        	    arrays.Add( (*itAttr)->GetDisplayName() );
        	    l_AttrNameMap.insert( pair<wxString, wxString> ( (*itAttr)->GetDisplayName(), (*itAttr)->GetName()));
        	}

        	if( l_pcEdge->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
        		arrays.Add( wxT("Order Lexicographically") );
        		l_AttrNameMap.insert( pair<wxString, wxString> (  wxT("Order Lexicographically"), wxT("Order Lexicographically") ));
        	}

        	SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
        	SP_Node2AttrNameMap.insert(pair<wxString, map<wxString, wxString> > (l_sEdgename, l_AttrNameMap) );
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
        		l_AttrNameMap.clear();

        		if( l_sMetaname.compare( wxT("Comment") ) == 0) { //first column for comments
        		    arrays.Add( wxT("Net reference") );
        		    l_AttrNameMap.insert( pair<wxString, wxString> ( wxT("Net reference"), wxT("Net reference")));
        		}

        		for(;itAttr != l_pcMeta->GetAttributes()->end(); itAttr++)
        		{
        			arrays.Add( (*itAttr)->GetDisplayName() );
        			l_AttrNameMap.insert( pair<wxString, wxString> ( (*itAttr)->GetDisplayName(), (*itAttr)->GetName()));
        		}

        		if( l_sMetaname.compare( wxT("Image") ) == 0) {
        		    arrays.Add( wxT("Image reference") );
        		    l_AttrNameMap.insert( pair<wxString, wxString> (  wxT("Image reference"), wxT("Image reference") ));
        		}

        		if( l_pcMeta->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
        			arrays.Add( wxT("Order Lexicographically") );
        			l_AttrNameMap.insert( pair<wxString, wxString> (  wxT("Order Lexicographically"), wxT("Order Lexicographically") ));
        		}

        		SP_Node2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
        		SP_Node2AttrNameMap.insert(pair<wxString, map<wxString, wxString> > (l_sMetaname, l_AttrNameMap) );
        		SP_Index2Node.insert(pair<int, wxString> (i, l_sMetaname) );

    		}
    		i++;
    	}
    }


     //loads Declarations for the export dialog
     i = 0;
 	 if( ((m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS) ))
 	 {

 		for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 		{
 			if( (*itMC)->GetShowInDeclarationTreeColorSet() || (*itMC)->GetShowInDeclarationTreeOther() )
 			{
 				wxString l_sMetaname = (*itMC)->GetDisplayName();
 				m_Options_Declarations.Add(  l_sMetaname );
 				m_Options_Declarations_order.Add(i);
 				//SP_LOGMESSAGE( l_sMetaname + wxT("->>"));

 				if( (*itMC) ) {
 					arrays.Clear();

 					//explicitly added column names to COLLIST attributes in colored nets
 					if( l_sMetaname.compare( wxT("Simple Color Sets")) == 0) {
 						arrays.Add( wxT("Name") );
 						arrays.Add( wxT("Type") );
 						arrays.Add( wxT("Colors") );
 						arrays.Add( wxT("Place Color") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Name"), wxT("Name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Type"), wxT("Type") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Colors"), wxT("Colors") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Place Color"), wxT("Place Color") ) );

 					} else if( l_sMetaname.compare( wxT("Compound Color Sets")) == 0) {
 						arrays.Add( wxT("Name") );
 						arrays.Add( wxT("Type") );
 						arrays.Add( wxT("Component color sets") );
 						arrays.Add( wxT("Colors") );
 						arrays.Add( wxT("Place Color") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Name"), wxT("Name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Type"), wxT("Type") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Component color sets"), wxT("Component color sets") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Colors"), wxT("Colors") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Place Color"), wxT("Place Color") ) );

 					} else if( l_sMetaname.compare( wxT("Alias Color Sets")) == 0) {
 						arrays.Add( wxT("Alias name") );
 						arrays.Add( wxT("Original color set") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Alias name"), wxT("Alias name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Original color set"), wxT("Original color set") ) );

 					} else if( l_sMetaname.compare( wxT("Constants")) == 0) {
 						arrays.Add( wxT("Name") );
 						arrays.Add( wxT("Type") );
 						arrays.Add( wxT("Value") );
 						arrays.Add( wxT("Comment") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Name"), wxT("Name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Type"), wxT("Type") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Value"), wxT("Value") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Comment"), wxT("Comment") ) );

 					} else if( l_sMetaname.compare( wxT("Functions")) == 0) {
 						arrays.Add( wxT("Return Type") );
 						arrays.Add( wxT("Function Name") );
 						arrays.Add( wxT("Parameter List") );
 						arrays.Add( wxT("Function Body") );
 						arrays.Add( wxT("Comment") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Return Type"), wxT("Return Type") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Function Name"), wxT("Function Name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Parameter List"), wxT("Parameter List") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Function Body"), wxT("Function Body") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Comment"), wxT("Comment") ) );

 					} else if( l_sMetaname.compare( wxT("Variables")) == 0) {
 						arrays.Add( wxT("Name") );
 						arrays.Add( wxT("Color Set") );
 						arrays.Add( wxT("Comment") );

 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Name"), wxT("Name") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Color Set"), wxT("Color Set") ) );
 						l_DecAttrNameMap.insert( pair<wxString, wxString> ( wxT("Comment"), wxT("Comment") ) );
 					}

 					SP_DecNode2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
					SP_DecNode2DecAttrNameMap.insert(pair<wxString, map<wxString, wxString> > (l_sMetaname, l_DecAttrNameMap) );
					SP_Index2DecNode.insert(pair<int, wxString> (i, l_sMetaname) );
					i++;
 				}
 		    }
 		}

 	 } else {

 	 	for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 	 	{
 	       if( (*itMC)->GetShowInDeclarationTreeOther() )
 	       {
 	    	   wxString l_sMetaname = (*itMC)->GetDisplayName();
 	    	   m_Options_Declarations.Add( l_sMetaname );
 	    	   m_Options_Declarations_order.Add(i);

 	    	   l_pcMeta = (*itMC)->GetPrototype();

 	    	   if( l_pcMeta ) {

 	    		   itAttr = l_pcMeta->GetAttributes()->begin();
 	    	       arrays.Clear();
 	    	       l_DecAttrNameMap.clear();

 	    	       for(;itAttr != l_pcMeta->GetAttributes()->end(); itAttr++)
 	    	       {
 	    	    		arrays.Add( (*itAttr)->GetDisplayName() );
 	    	    		l_DecAttrNameMap.insert( pair<wxString, wxString> ( (*itAttr)->GetDisplayName(), (*itAttr)->GetName()));
 	    	       }

 				   if( l_pcMeta->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) ) {
 					    arrays.Add( wxT("Order Lexicographically") );
 					    l_DecAttrNameMap.insert( pair<wxString, wxString> (  wxT("Order Lexicographically"), wxT("Order Lexicographically") ));
 				   }

 	    	       SP_DecNode2Attributes.insert(pair<int, wxArrayString > (i, arrays) );
 	    	       SP_DecNode2DecAttrNameMap.insert(pair<wxString, map<wxString, wxString> > (l_sMetaname, l_DecAttrNameMap) );
 	    	       SP_Index2DecNode.insert(pair<int, wxString> (i, l_sMetaname) );
 	    	       i++;
 	    	   }
 	       }
 	     }

 	 }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
	//loading list of individual Graph Elements from current graph for the right-side panels

 	//Nodeclass
	for(itNC = m_pcGraph->GetNodeclasses()->begin(); itNC != m_pcGraph->GetNodeclasses()->end(); ++itNC)
	{
		SP_DS_Nodeclass* l_pcNodeclass = *itNC;
		wxString NodeclassName = l_pcNodeclass->GetDisplayName();
		SP_ListNode::const_iterator l_itElem;
		arrays.Clear();

		for(l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem) {
			wxString l_sNodeName =  dynamic_cast<SP_DS_NameAttribute*>( (*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValue();
			arrays.Add( l_sNodeName );

			SP_Name2Node.insert(pair<wxString, SP_DS_Node*> ( l_sNodeName, (*l_itElem) ) );

		}

		SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (NodeclassName, arrays) );
	}

	//Edgeclass
	for(itEC = m_pcGraph->GetEdgeclasses()->begin(); itEC != m_pcGraph->GetEdgeclasses()->end(); ++itEC)
	{
		SP_DS_Edgeclass* l_pcEdgeclass = *itEC;
		SP_ListEdge::const_iterator l_itElem;
		wxString EdgeclassName;
		wxString l_sEdgeName;

		if( l_pcEdgeclass ) {
			EdgeclassName = l_pcEdgeclass->GetDisplayName();
			SP_LOGMESSAGE( wxT("EdgeClassname: ") + EdgeclassName);

			m_pEdgesPlace2Transition.clear();
			m_pEdgesTransition2Place.clear();

			for(l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem) {

				wxString l_sSourceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetSource()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				wxString l_sTargetName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetTarget()->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				l_sEdgeName = l_sSourceName + wxT("->") + l_sTargetName;

				wxString l_sSourceclass = (*l_itElem)->GetSource()->GetClassName();

				if(l_sSourceclass == SP_DS_DISCRETE_PLACE || l_sSourceclass == SP_DS_COARSE_PLACE ||
						l_sSourceclass == SP_DS_CONTINUOUS_PLACE) {
					m_pEdgesPlace2Transition.push_back( make_pair(l_sSourceName, l_sTargetName) );
				} else {
					m_pEdgesTransition2Place.push_back( make_pair(l_sSourceName, l_sTargetName) );
				}

				SP_Name2Edge.insert(pair<wxString, SP_DS_Edge*> ( l_sEdgeName, (*l_itElem) ) );
			}

			SP_Edgeclass2EdgeList.insert(pair<wxString, pair<EdgeVector, EdgeVector> > (EdgeclassName, make_pair(m_pEdgesPlace2Transition, m_pEdgesTransition2Place) ) );
		}
	}


	//Metaclass
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

						SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, (*l_itElem) ) );
					}

					SP_Nodeclass2NodeList.insert(pair<wxString, wxArrayString> (MetaclassName, arrays) );

					SP_LOGMESSAGE( wxT("Metadata class loaded:" ) + MetaclassName );
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// loading individual Declarations list for the right-side panels

	//if Colored net, load only the COLLIST attributes and corresponding elements
 	 if( ((m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS)
 		 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS) ))
 	 {

 		for (itMC = m_pcGraph->GetMetadataclasses()->begin(); itMC != m_pcGraph->GetMetadataclasses()->end(); ++itMC)
 		{
 			SP_DS_Metadataclass* l_pcMetadataclass = *itMC;
 			SP_ListMetadata::const_iterator l_itElem;
 			SP_DS_Metadata* l_pcNewMetadata;
 			SP_DS_ColListAttribute * l_pcColList;
 			wxString l_sMetaclassName;
 			arrays.Clear();

 			if( l_pcMetadataclass && (l_pcMetadataclass->GetShowInDeclarationTreeColorSet() || l_pcMetadataclass->GetShowInDeclarationTreeOther() ) )
 			{
 				l_sMetaclassName = l_pcMetadataclass->GetDisplayName();
 				l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());

 				if(l_pcNewMetadata) {

 					if(l_sMetaclassName.compare( wxT("Simple Color Sets")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
 					}
 					else if(l_sMetaclassName.compare( wxT("Compound Color Sets")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
 					}
 					else if(l_sMetaclassName.compare( wxT("Alias Color Sets")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
 					}
 					else if(l_sMetaclassName.compare( wxT("Constants")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
 					}
 					else if(l_sMetaclassName.compare( wxT("Functions")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
 					}
 					else if(l_sMetaclassName.compare( wxT("Variables")) == 0)
 					{
 						l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
 					} else {
 						l_pcColList = NULL;
 					}

 					if(l_pcColList) {
 						for (unsigned int r = 0; r < l_pcColList->GetRowCount(); r++)
 						{
 							wxString l_sMetaName;
 							if(l_pcColList->GetDisplayName().compare( wxT("FunctionList") ) == 0) {
 								l_sMetaName = l_pcColList->GetCell(r,1);
 							} else {
 								l_sMetaName = l_pcColList->GetCell(r,0);
 							}
 							arrays.Add( l_sMetaName );
 							SP_Name2Metadata.insert(pair<wxString, SP_DS_Metadata*> ( l_sMetaName, l_pcNewMetadata ) );
 						}
 						SP_DecNodeclass2NodeList.insert(pair<wxString, wxArrayString> (l_sMetaclassName, arrays) );
 					}
 					else {
 						SP_LOGMESSAGE( l_sMetaclassName + wxT("has no COLLIST attribute") );
 					}
 				}
 		    }
 		}

 	 } else {
 		 // for Uncolored nets
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

	//Rearrangectrl to facilitate rearranging of tabs for report
	m_pcRearrangeCtrl_General = new wxRearrangeCtrl(m_pcNotebookPageGeneral, -1, wxPoint(50, 50), wxDefaultSize, order, tabs_items);
	m_pcMainSizerGeneral->Add(m_pcRearrangeCtrl_General, 1, wxALL | wxEXPAND, 5);


	m_pcCheckBoxDirectPDF = new wxCheckBox( m_pcNotebookPageGeneral, SP_GENERATEPDF_CHECKBOX_SELECTED, wxT("Generate PDF Report") );
	m_pcCheckBoxDirectPDF->SetValue(true);

	m_pcMainSizerGeneral->Add(10, 10);
	m_pcMainSizerGeneral->Add(m_pcCheckBoxDirectPDF, 0, wxALL, 5);
	m_pcMainSizerGeneral->Add(10, 10);

	m_pcCheckBoxDirectPDF->Bind(wxEVT_CHECKBOX, &SP_ExportLatex::OnClickGeneratePDF, this, SP_GENERATEPDF_CHECKBOX_SELECTED);


	m_pcSizerGeneratePDF = new wxBoxSizer( wxVERTICAL );

	//sizer for Compiler options
	wxSizer* l_pcSizerCompilers = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageGeneral, -1, wxT("Tex Compiler") ), wxHORIZONTAL );

	m_rbPdfLatex = new wxRadioButton(m_pcNotebookPageGeneral, SP_ID_GENERAL_PDFLATEX, wxT("PdfLaTeX"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_rbLatexmk = new wxRadioButton(m_pcNotebookPageGeneral, SP_ID_GENERAL_LATEXMK, wxT("Latexmk"));

	m_rbPdfLatex->SetValue(true);

	m_rbPdfLatex->Bind(wxEVT_RADIOBUTTON, &SP_ExportLatex::OnSelectLatexCompiler_Pdflatex, this, SP_ID_GENERAL_PDFLATEX);
	m_rbLatexmk->Bind(wxEVT_RADIOBUTTON, &SP_ExportLatex::OnSelectLatexCompiler_Latexmk, this, SP_ID_GENERAL_LATEXMK);

	l_pcSizerCompilers->Add(m_rbPdfLatex, 1, wxALL | wxEXPAND, 5);
	l_pcSizerCompilers->Add(m_rbLatexmk, 1, wxALL | wxEXPAND, 5);

	m_pcSizerGeneratePDF->Add(l_pcSizerCompilers, 1, wxALL | wxEXPAND, 5);

	//sizer for latex compiler browse
	wxSizer* l_pcSizerPath1 = new wxBoxSizer( wxHORIZONTAL );

	l_pcSizerPath1->Add(new wxStaticText( m_pcNotebookPageGeneral, -1, wxT("Latex Compiler Location:") ), 0, wxALL | wxEXPAND, 5);


	m_sCompilerPath = wxT("/usr/texbin/pdflatex");

	wxString l_sPath1 = m_sCompilerPath;
	wxString l_sPDFDescr1 = wxT("Compiler Directory");
	wxString wildcard1 = l_sPDFDescr1 + wxT("*/*");

	l_pcFilePickerCtrl1 = new wxFilePickerCtrl( m_pcNotebookPageGeneral, -1, l_sPath1, wxT("Browse"),
			wildcard1, wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
	l_pcFilePickerCtrl1->SetPath( l_sPath1);

	l_pcSizerPath1->Add(l_pcFilePickerCtrl1, 1, wxALL | wxEXPAND, 5);
	m_pcSizerGeneratePDF->Add(l_pcSizerPath1, 1, wxALL | wxEXPAND, 5);

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

    //Rearrangelist for Basic options
	m_pcRearrangelist_basics = new wxRearrangeList(m_pcNotebookPageBasics, SP_ID_BASICS_UPDATE, wxDefaultPosition, wxSize(50, 200), l_pcOptions_basics_order, l_pcOptions_basics);
	m_pcLeftSizer_Basics->Add(m_pcRearrangelist_basics, 1, wxALL | wxEXPAND, 5);

	l_pcButtonUp = new wxButton( m_pcNotebookPageBasics, SP_ID_BUTTON_BASICS_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	l_pcButtonDown = new wxButton( m_pcNotebookPageBasics, SP_ID_BUTTON_BASICS_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	m_pcRearrangelist_basics->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Basics, this, SP_ID_BASICS_UPDATE);
	m_pcRearrangelist_basics->Bind(wxEVT_CHECKLISTBOX, &SP_ExportLatex::OnCheckUncheck_Basics, this, SP_ID_BASICS_UPDATE);

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
    			
	m_pcNotebookPageBasics->AddControl(m_pcMainSizer_Basics, 1, wxALL | wxEXPAND, 5);

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
		//SP_DS_Node* l_pcNodeClass = SP_Name2Node[ l_sCurrentNode ];

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


		SP_DS_Edgeclass* edgeclass = m_pcGraph->GetEdgeclassByDisplayedName( l_sCurrentNode );

	    if( (!edgeclass) && l_sCurrentNode.Cmp("Comment") )  //if not an edge or comment (in metadata)
	    {
	    	l_pcRightSizer->Add(5, 20);
	    	l_pcRightSizer->Add(new wxStaticText( m_pcNotebookPageGraph, -1, wxT("RegEx") ), 0, wxALL, 5);
	        wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(m_pcNotebookPageGraph, SP_REGEX_GRAPH);
	        l_pcRightSizer->Add(l_pcTextCtrl, 0, wxALL | wxEXPAND, 5);

	        l_pcTextCtrl->Bind(wxEVT_TEXT, &SP_ExportLatex::OnRegEx_Graph, this, SP_REGEX_GRAPH);

		    wxCheckListBox* l_pcCheckList1 = new wxCheckListBox( m_pcNotebookPageGraph, -1, wxDefaultPosition, wxDefaultSize);
			l_pcRightSizer->Add(l_pcCheckList1, 1, wxALL | wxEXPAND, 5);
			l_pcCheckList1->Set( SP_Nodeclass2NodeList[ l_sCurrentNode ] );
			SP_Node2NodeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList1) );

			wxCheckBox* l_pcSelectClearAllChkBox = new wxCheckBox(m_pcNotebookPageGraph, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH,
						wxT("Select/deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);

		    l_pcRightSizer->Add(l_pcSelectClearAllChkBox, 0, wxALL, 5);
			l_pcSelectClearAllChkBox->Set3StateValue(wxCHK_CHECKED);

			for (unsigned int i = 0; i < SP_Node2NodeCheckList[ (*itN).first ]->GetCount() ; i++) {
				SP_Node2NodeCheckList[ (*itN).first ]->Check(i, true);
			}

			l_pcSelectClearAllChkBox->Bind(wxEVT_CHECKBOX, &SP_ExportLatex::OnSelectClearAllItems_Graph, this,
					SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_GRAPH);
			SP_Node2SelectClearAllCheckBox.insert(pair<int, wxCheckBox* > ( (*itN).first, l_pcSelectClearAllChkBox) );


	    }

	    //Add ordering criteria for 'Edges'
	    if( edgeclass ) {

	    	 //Order By
	    	 l_pcRightSizer->Add(0, 20);

	    	 wxStaticText* l_pcStaticText_OrderBy = new wxStaticText(m_pcNotebookPageGraph, -1, wxT("Order By"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
			 l_pcRightSizer->Add(l_pcStaticText_OrderBy, 0, wxALL, 2);

	    	 m_pcOrderBySource = new wxRadioButton(m_pcNotebookPageGraph, -1, wxT("Source"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	    	 m_pcOrderbyTarget = new wxRadioButton(m_pcNotebookPageGraph, -1, wxT("Target"));
	    	 m_pcOrderBySource->SetValue(true);

	    	 wxSizer* l_pcSizerOrientation1 = new wxBoxSizer(wxHORIZONTAL);

		 	 l_pcSizerOrientation1->Add(10, 0);
			 l_pcSizerOrientation1->Add(m_pcOrderBySource, 0, wxALL, 2);
			 l_pcSizerOrientation1->Add(m_pcOrderbyTarget, 0, wxALL, 2);

			 l_pcRightSizer->Add(l_pcSizerOrientation1, 0, wxALL, 5);


			 //Group By
			 l_pcRightSizer->Add(0, 10);

	    	 wxStaticText* l_pcStaticText_GroupBy = new wxStaticText(m_pcNotebookPageGraph, -1, wxT("Group By"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
			 l_pcRightSizer->Add(l_pcStaticText_GroupBy, 0, wxALL, 2);

			 m_pcGroupByPlace2Transition = new wxRadioButton(m_pcNotebookPageGraph, -1, wxT("Place to Transition"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		     m_pcGroupbyTransition2Place = new wxRadioButton(m_pcNotebookPageGraph, -1, wxT("Transition to Place"));
		     m_pcGroupByPlace2Transition->SetValue(true);

	    	 wxSizer* l_pcSizerOrientation2 = new wxBoxSizer(wxHORIZONTAL);
	    	 wxSizer* l_pcSizerOrientation3 = new wxBoxSizer(wxVERTICAL);

		 	 l_pcSizerOrientation2->Add(10, 0);
		 	 l_pcSizerOrientation2->Add(l_pcSizerOrientation3, 0, wxALL, 2);
			 l_pcSizerOrientation3->Add(m_pcGroupByPlace2Transition, 0, wxALL, 2);
			 l_pcSizerOrientation3->Add(m_pcGroupbyTransition2Place, 0, wxALL, 2);

			 l_pcRightSizer->Add(l_pcSizerOrientation2, 0, wxALL, 5);
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

    //Rearrangelist for reordering the sequence of Graph Elements
    m_pcRearrangelist_Graph = new wxRearrangeList(m_pcNotebookPageGraph, SP_ID_GRAPH_UPDATE, wxDefaultPosition,
    		wxSize(50, 200), m_Options_Graph_order, m_Options_Graph);
    m_pcLeftSizer_Graph->Add(m_pcRearrangelist_Graph, 1, wxALL | wxEXPAND, 5);

    m_pcGraph_ButtonUp = new wxButton(m_pcNotebookPageGraph, SP_ID_BUTTON_GRAPH_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	m_pcGraph_ButtonDown = new wxButton(m_pcNotebookPageGraph, SP_ID_BUTTON_GRAPH_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	m_pcRearrangelist_Graph->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Graph, this, SP_ID_GRAPH_UPDATE);
	//m_pcRearrangelist_Graph->Bind(wxEVT_CHECKLISTBOX, &SP_ExportLatex::OnCheckUncheck_Graph, this, SP_ID_GRAPH_UPDATE);

	m_pcGraph_ButtonUp->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Graph_UpdateUI, this, SP_ID_BUTTON_GRAPH_UP);
	m_pcGraph_ButtonDown->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Graph_UpdateUI, this, SP_ID_BUTTON_GRAPH_DOWN);

	m_pcGraph_ButtonUp ->Bind(wxEVT_BUTTON, &SP_ExportLatex::Graph_Move, this, SP_ID_BUTTON_GRAPH_UP);
	m_pcGraph_ButtonDown->Bind(wxEVT_BUTTON, &SP_ExportLatex::Graph_Move, this, SP_ID_BUTTON_GRAPH_DOWN);


	l_pcButtonSizer->Add(m_pcGraph_ButtonUp, 1, wxALL | wxEXPAND, 5);
	l_pcButtonSizer->Add(m_pcGraph_ButtonDown, 1, wxALL | wxEXPAND, 5);

	m_pcLeftSizer_Graph->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

    AddAttributes_GraphElements();

    m_pcNotebookPageGraph->AddControl(m_pcMainSizer_Graph, 1, wxALL | wxEXPAND, 5);
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
		l_pcRightSizer->Add(new wxStaticText( m_pcNotebookPageDeclarations, -1, wxT("RegEx") ), 0, wxALL, 5);
		wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(m_pcNotebookPageDeclarations, SP_REGEX_DEC);
		l_pcRightSizer->Add(l_pcTextCtrl, 0, wxALL | wxEXPAND, 5);

		l_pcTextCtrl->Bind(wxEVT_TEXT, &SP_ExportLatex::OnRegEx_Dec, this, SP_REGEX_DEC);

		wxCheckListBox* l_pcCheckList1 = new wxCheckListBox( m_pcNotebookPageDeclarations, -1, wxDefaultPosition, wxDefaultSize);
		l_pcRightSizer->Add(l_pcCheckList1, 1, wxALL | wxEXPAND, 5);
		l_pcCheckList1->Set( SP_DecNodeclass2NodeList[ l_sCurrentNode ] );
		SP_DecNode2NodeCheckList.insert(pair<int, wxCheckListBox* > ( (*itN).first, l_pcCheckList1) );


		wxCheckBox* l_pcSelectClearAllChkBox = new wxCheckBox(m_pcNotebookPageDeclarations, SP_ID_BUTTON_SELECT_CLEAR_ALL_ITEMS_DEC,
								wxT("Select/Deselect all"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);

		l_pcRightSizer->Add(l_pcSelectClearAllChkBox, 0, wxALL, 5);
		l_pcSelectClearAllChkBox->Set3StateValue(wxCHK_CHECKED);

		for (unsigned int i = 0; i < SP_DecNode2NodeCheckList[ (*itN).first ]->GetCount(); i++) {
			SP_DecNode2NodeCheckList[ (*itN).first ]->Check(i, true);
		}

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

    //Rearrangelist for reordering the sequence of Declarations
	m_pcRearrangelist_declarations = new wxRearrangeList(m_pcNotebookPageDeclarations, SP_ID_DECLARATIONS_UPDATE, wxDefaultPosition,
			wxSize(50, 200), m_Options_Declarations_order, m_Options_Declarations);
	m_pcLeftSizer_Declarations->Add(m_pcRearrangelist_declarations, 1, wxALL | wxEXPAND, 5);

	m_pcDeclarations_ButtonUp = new wxButton( m_pcNotebookPageDeclarations, SP_ID_BUTTON_DECLARATIONS_UP, wxT("Up"), wxDefaultPosition, wxDefaultSize );
	m_pcDeclarations_ButtonDown = new wxButton( m_pcNotebookPageDeclarations, SP_ID_BUTTON_DECLARATIONS_DOWN, wxT("Down"), wxDefaultPosition, wxDefaultSize );

	m_pcRearrangelist_declarations->Bind(wxEVT_LISTBOX, &SP_ExportLatex::OnSelChange_Declarations, this, SP_ID_DECLARATIONS_UPDATE);
	//m_pcRearrangelist_declarations->Bind(wxEVT_CHECKLISTBOX, &SP_ExportLatex::OnCheckUncheck_Declarations, this, SP_ID_DECLARATIONS_UPDATE);

	m_pcDeclarations_ButtonUp->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Declarations_UpdateUI, this, SP_ID_BUTTON_DECLARATIONS_UP);
	m_pcDeclarations_ButtonDown->Bind(wxEVT_UPDATE_UI, &SP_ExportLatex::Declarations_UpdateUI, this, SP_ID_BUTTON_DECLARATIONS_DOWN);

	m_pcDeclarations_ButtonUp->Bind(wxEVT_BUTTON, &SP_ExportLatex::Declarations_Move, this, SP_ID_BUTTON_DECLARATIONS_UP);
	m_pcDeclarations_ButtonDown->Bind(wxEVT_BUTTON, &SP_ExportLatex::Declarations_Move, this, SP_ID_BUTTON_DECLARATIONS_DOWN);

	l_pcButtonSizer->Add(m_pcDeclarations_ButtonUp, 1, wxALL | wxEXPAND, 5);
	l_pcButtonSizer->Add(m_pcDeclarations_ButtonDown, 1, wxALL | wxEXPAND, 5);

	m_pcLeftSizer_Declarations->Add(l_pcButtonSizer, 0, wxALL | wxEXPAND, 5);

	//to populate the right panel
    AddAttributes_Declarations();

	m_pcNotebookPageDeclarations->AddControl(m_pcMainSizer_Declarations, 1, wxALL | wxEXPAND, 5);
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
		  child_to = m_pcTree->AppendItem(into, s);

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
	l_pcMainSizer->Add(l_pcTopSizer, 1, wxALL | wxEXPAND, 5);

	wxSizer* l_pcTreeSizer = new wxStaticBoxSizer( new wxStaticBox( m_pcNotebookPageHierarchy, -1,
			wxT("Select the topmost hierarchy level"), wxDefaultPosition, wxDefaultSize ), wxVERTICAL );

 	m_pcCheckBoxHierarchyTree = new wxCheckBox(m_pcNotebookPageHierarchy, -1, wxT("Generate Hierarchy Tree"));
 	m_pcCheckBoxHierarchyTree->SetValue(true);
 	l_pcTopSizer->Add(m_pcCheckBoxHierarchyTree, 0, wxALL | wxEXPAND, 5);

 	m_pcCheckBoxIncludeSubtrees = new wxCheckBox(m_pcNotebookPageHierarchy, -1, wxT("Include Subtrees"));
 	m_pcCheckBoxIncludeSubtrees->SetValue(true);
 	l_pcTopSizer->Add(m_pcCheckBoxIncludeSubtrees, 0, wxALL | wxEXPAND, 5);

 	l_pcTopSizer->Add(0, 20);
 	l_pcTopSizer->Add(l_pcTreeSizer, 1 , wxALL | wxEXPAND, 5);

    //tree implementation
 	m_pcTree = new wxTreeCtrl(m_pcNotebookPageHierarchy, -1, wxDefaultPosition, wxSize(300, 190), wxTR_DEFAULT_STYLE | wxTR_MULTIPLE);
 	l_pcTreeSizer->Add(m_pcTree, 1, wxALL | wxEXPAND, 5);

 	wxTreeItemId root = m_pcTree->AddRoot( wxT("0. Top Level") );
 	wxTreeItemId root_to_copy = m_pcCoarseTreectrl->GetRootItem();
 	wxString name = m_pcCoarseTreectrl->GetItemText(root_to_copy);

 	 SP_HierarchyLabel2ID.insert(pair<wxString, wxString> (name, wxT("0.") ) );
 	 SP_NetNumber2Label.insert( pair<wxString, wxString> (wxT("1"), name) );

 	SP_LOGMESSAGE("Begin Copying Tree...");
 	copyTree_recur( root , root_to_copy, wxT(""), 1);
 	SP_LOGMESSAGE("End Copying Tree...");

    m_pcTree->SelectItem(root, true);
 	m_pcTree->Expand(root);

	m_pcNotebookPageHierarchy->AddControl(l_pcMainSizer, 1, wxALL | wxEXPAND, 5);

}

void
SP_ExportLatex::OnClickGeneratePDF(wxCommandEvent& p_cEvent)
{
	if( p_cEvent.IsChecked() ) {
		SP_LOGMESSAGE("generate PDF");

		l_pcFilePickerCtrl1->Enable(true);
		m_rbPdfLatex->Enable(true);
	    m_rbLatexmk->Enable(true);

	} else {
		SP_LOGMESSAGE("generate PDF Else");

		l_pcFilePickerCtrl1->Enable(false);
		m_rbPdfLatex->Enable(false);
		m_rbLatexmk->Enable(false);
	}

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

    m_pcBasics_Landscape = new wxRadioButton(m_pcNotebookPageBasics, -1, wxT("Landscape"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_pcBasics_Portrait = new wxRadioButton(m_pcNotebookPageBasics, -1, wxT("Portrait"));
    m_pcBasics_Portrait->SetValue(true);

    l_pcSizerOrientation1->Add(10, 0);
    l_pcSizerOrientation1->Add(m_pcBasics_Landscape, 0, wxALL, 2);
    l_pcSizerOrientation1->Add(m_pcBasics_Portrait, 0, wxALL, 2);

    m_pcSizer_Basics_ReportLayout->Add(l_pcSizerOrientation1, 0, wxALL, 5);

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

	//footer
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

	//m_pcSizer_Basics_ReportTypography->Add(new wxStaticText(m_pcNotebookPageBasics, -1, wxT("Report Font") ), 0, wxALL, 5);

	wxSizer* l_pcSizerFontFamily = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSizerPaperSize = new wxBoxSizer(wxHORIZONTAL);

	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontFamily, 0, wxALL, 0);
	m_pcSizer_Basics_ReportTypography->Add(10, 10);
	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerPaperSize, 0, wxALL, 0);

	//Font Family
	//l_pcSizerFontFamily->Add(10, 0);
	l_pcSizerFontFamily->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Family:") ), 0, wxALL, 5);
	m_pcComboBox_FontFamily = new wxComboBox(m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontFamily->Set(m_Basics_FontFamily_Strings);
	m_pcComboBox_FontFamily->SetValue("Roman (rm)");
	l_pcSizerFontFamily->Add(m_pcComboBox_FontFamily, 0, wxALL, 5);

	//Paper Size
	l_pcSizerPaperSize->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Paper Size:") ), 0, wxALL, 5);
	m_pcComboBox_PaperSize = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_PaperSize->Set(m_Basics_PaperSize_Strings);
	m_pcComboBox_PaperSize->SetValue("a4paper");
	l_pcSizerPaperSize->Add(m_pcComboBox_PaperSize, 0, wxALL, 5);

	/*
	 * Part of code unused, as for now as no corresponding commands in latex could be found currently
	 *
	wxSizer* l_pcSizerFontSize = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcSizerFontStyle = new wxBoxSizer(wxHORIZONTAL);

	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontSize, 0, wxALL, 0);
	m_pcSizer_Basics_ReportTypography->Add(l_pcSizerFontStyle, 0, wxALL, 0);

	//Font Size
	l_pcSizerFontSize->Add(10, 0);
	l_pcSizerFontSize->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Size:") ), 0, wxALL, 5);
	m_pcComboBox_FontSize = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontSize->Set(m_Basics_FontSize_Strings);
	m_pcComboBox_FontSize->SetValue("normalsize");
	l_pcSizerFontSize->Add(m_pcComboBox_FontSize, 0, wxALL, 5);

	//Font Style
	l_pcSizerFontStyle->Add(10, 0);
	l_pcSizerFontStyle->Add(new wxStaticText( m_pcNotebookPageBasics, -1, wxT("Font Style:") ), 0, wxALL, 5);
	m_pcComboBox_FontStyle = new wxComboBox( m_pcNotebookPageBasics, -1);
	m_pcComboBox_FontStyle->Set(m_Basics_FontStyle_Strings);
	m_pcComboBox_FontStyle->SetValue("normal font");
	l_pcSizerFontStyle->Add(m_pcComboBox_FontStyle, 0, wxALL, 5);
	*
	*/

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
		break;

	case 1:
		SP_LOGMESSAGE("Net Info prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, true, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);
		break;

	case 2:
		SP_LOGMESSAGE("Report Layout prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, true, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, false, true);
		break;

	case 3:
		SP_LOGMESSAGE("Report Typography prompt");
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_GeneralInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_NetInfo, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportLayout, false, true);
		m_pcRightSizer_Basics->Show(m_pcSizer_Basics_ReportTypography, true, true);
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
	UpdateRightPanel_Basics( m_pcRearrangelist_basics->GetCurrentOrder()[ p_cEvent.GetInt() ] );
}

void
SP_ExportLatex::OnCheckUncheck_Basics(wxCommandEvent& p_cEvent)
{
	int id = m_pcRearrangelist_basics->GetCurrentOrder()[ p_cEvent.GetInt() ];

	bool show = m_pcRearrangelist_basics->IsChecked(id);

	switch( id ) {

		case 0:
			SP_LOGMESSAGE("General Info Checkbox");

			for(int i = 0; i < m_pcCheckBox_BasicsGeneral.size(); i++) {
				m_pcCheckBox_BasicsGeneral[i]->Enable(show);
			}

			break;

		case 1:
			SP_LOGMESSAGE("Net Info Checkbox");

			for(int i = 0; i < m_pcCheckBox_BasicsNet.size(); i++) {
				m_pcCheckBox_BasicsNet[i]->Enable(show);
			}

			break;

		case 2:
			SP_LOGMESSAGE("Report Layout Checkbox");

			m_pcBasics_Landscape->Enable(show);
			m_pcBasics_Portrait->Enable(show);

			m_pcTextCtrlHeaderFooter[0]->Enable(show);
			m_pcTextCtrlHeaderFooter[1]->Enable(show);

			for(int i = 0; i < m_pcCheckBox_BasicsLayout.size(); i++) {
				m_pcCheckBox_BasicsLayout[i]->Enable(show);
			}

			break;

		case 3:
			SP_LOGMESSAGE("Report Typography Checkbox");

			m_pcComboBox_FontFamily->Enable(show);
			m_pcComboBox_PaperSize->Enable(show);
			break;

		default:
			SP_LOGMESSAGE("Default prompt Checkbox");
	}

}


void
SP_ExportLatex::OnSelChange_Graph(wxCommandEvent& p_cEvent)
{
	m_nSelectionId_Graph = m_pcRearrangelist_Graph->GetCurrentOrder()[ p_cEvent.GetInt() ];
	UpdateRightPanel_Graph( m_nSelectionId_Graph );
}

void
SP_ExportLatex::OnCheckUncheck_Graph(wxCommandEvent& p_cEvent)
{
	int id = m_pcRearrangelist_Graph->GetCurrentOrder()[ p_cEvent.GetInt() ];

	SP_LOGMESSAGE( wxT("id check:" ) + wxString::Format( wxT("%i"), id) );
	bool show = m_pcRearrangelist_Graph->IsChecked(id);

	wxString l_sCurrentNode = SP_Index2Node[ id ];   //node class
    //SP_LOGMESSAGE( wxT("Node = ") + l_sCurrentNode );
	SP_Node2AttributeCheckList[id]->Enable(show);

	if( (l_sCurrentNode.Find( wxT("Edge") ) == wxNOT_FOUND) && l_sCurrentNode.Cmp("Comment") )  //if not an edge or comment
    {
		SP_Node2NodeCheckList[id]->Enable(show);
		SP_Node2SelectClearAllCheckBox[id]->Enable(show);

    } else if (l_sCurrentNode.Find( wxT("Edge") ) != wxNOT_FOUND) {  //is an edge

    	m_pcOrderBySource->Enable(show);
    	m_pcOrderbyTarget->Enable(show);
    	m_pcGroupByPlace2Transition->Enable(show);
    	m_pcGroupbyTransition2Place->Enable(show);
    }

}

void
SP_ExportLatex::OnSelChange_Declarations(wxCommandEvent& p_cEvent)
{
	m_nSelectionId_Declarations = m_pcRearrangelist_declarations->GetCurrentOrder()[ p_cEvent.GetInt() ];
	UpdateRightPanel_Declarations( m_nSelectionId_Declarations );
}

void
SP_ExportLatex::OnCheckUncheck_Declarations(wxCommandEvent& p_cEvent)
{
	int id =  m_pcRearrangelist_declarations->GetCurrentOrder()[ p_cEvent.GetInt() ];

	bool show = m_pcRearrangelist_declarations->IsChecked(id);

	SP_DecNode2AttributeCheckList[id]->Enable(show);
	SP_DecNode2NodeCheckList[id]->Enable(show);
	SP_DecNode2SelectClearAllCheckBox[id]->Enable(show);
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

void
SP_ExportLatex::OnRegEx_Graph(wxCommandEvent& p_cEvent)
{
	int id = m_nSelectionId_Graph;
	wxRegEx l_cRegEx;

	wxString l_sNameRegEx = p_cEvent.GetString();

	if(l_cRegEx.Compile( l_sNameRegEx )) {
		wxArrayString arr = SP_Node2NodeCheckList[ id ]->GetStrings();

		for(int i = 0; i < arr.size(); i++)
		{
			SP_Node2NodeCheckList[ id ]->Check(i, false);
			if( l_cRegEx.Matches( arr[i] ) )
			{
				SP_Node2NodeCheckList[ id ]->Check(i, true);
			}
		}
	}
}

void
SP_ExportLatex::OnRegEx_Dec(wxCommandEvent& p_cEvent)
{
	int id = m_nSelectionId_Declarations;
	wxRegEx l_cRegEx;

	wxString l_sNameRegEx = p_cEvent.GetString();

	if(l_cRegEx.Compile( l_sNameRegEx )) {
		wxArrayString arr = SP_DecNode2NodeCheckList[ id ]->GetStrings();

		for(int i = 0; i < arr.size(); i++)
		{
			SP_DecNode2NodeCheckList[ id ]->Check(i, false);
			if( l_cRegEx.Matches( arr[i] ) )
			{
				SP_DecNode2NodeCheckList[ id ]->Check(i, true);
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

//predicate for sort() function
bool
compareTarget(const pair<wxString, wxString> p_pTarget1, const pair<wxString, wxString> p_pTarget2) {
	return ( p_pTarget1.second < p_pTarget2.second );
}

bool
SP_ExportLatex::StartDoc(const wxString& p_fileName)
{
	m_bitmapCount = 0;
	m_sFilePath = p_fileName.BeforeLast('/') + wxT("/");

	SetUpPrintData(m_printData, p_fileName);

	m_pstream = wxFopen(m_printData.GetFilename().c_str(), wxT("w+"));

	if (!m_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	InputNetname = EditStringforLatex( p_pcDoc->GetUserReadableName() );

	wxFprintf(m_pstream, wxT("\\documentclass[pdftex,12pt]{article}\n") );
	wxFprintf(m_pstream, wxT("\n") );

	wxString out = wxT("\\usepackage[top=1.0in, ");

	//Set paper size
	out += m_pcComboBox_PaperSize->GetValue();

	//Set orientation
	if( m_pcBasics_Landscape->GetValue() ) {
		out += wxT(", landscape ");
	}

	out += wxT("]{geometry}\n\n");

	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\\usepackage[ddmmyyyy]{datetime}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{url}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{hyperref}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{needspace}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{lastpage}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{multirow}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{adjustbox}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{longtable, tabu}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{pgf}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{tikz}\n") );
	wxFprintf(m_pstream, wxT("\\usepackage{verbatim}\n") );
	wxFprintf(m_pstream, wxT("\\usetikzlibrary{positioning,arrows,shapes,backgrounds,calc,patterns,trees}\n") );
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

	out = wxT("\\newcommand{\\UnderscoreCommands}{ \n\t");
	out += wxT("\\do\\citeNP \\do\\citeA \\do\\citeANP \\do\\citeN \\do\\shortcite \n\t");
	out += wxT("\\do\\shortciteNP \\do\\shortciteA \\do\\shortciteANP \\do\\shortciteN \n\t");
	out += wxT("\\do\\citeyear \\do\\citeyearNP \n\t");
	out += wxT("} \n");

	out += wxT("\\usepackage[strings]{underscore}");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	wxFprintf(m_pstream, wxT("\n\\newcommand{\\linkto}[1]{\\phantomsection \\label{#1}}") );

	//new command and settings for inserting image thumbnail
	// set up a counter for links
	wxFprintf(m_pstream, wxT("\\newcounter{thumbnail}\n\n") );
	wxFprintf(m_pstream, wxT("\\newbox\\savedimgs\n") );
	wxFprintf(m_pstream, wxT("\\setbox\\savedimgs\\vbox{}\n\n") );

	wxFprintf(m_pstream, wxT("% thumbnail and appendix command\n") );

	out = wxT("\\newcommand{\\thumbnailandappendix}[1]{\n\t");
	out += wxT("\\refstepcounter{thumbnail}\n\t");
	out += wxT("\\hypertarget{small\\thethumbnail}{}\n\t");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());

	out = wxT("\\hyperlink{big\\thethumbnail}{\\includegraphics[width=1cm,height=1cm]{#1}}\n\t");
	out += wxT("\\global\\setbox\\savedimgs\\vbox{\n\t\t");
	out += wxT("\\unvbox\\savedimgs\n\t\t");
	out += wxT("\\bigskip\n\t\t");
	out += wxT("\\filbreak\n\t\t");
	out += wxT("\\noindent\n\t\t");
	out += wxT("\\hypertarget{big\\thethumbnail}{}\n\t\t");
	out += wxT("\\hyperlink{small\\thethumbnail}{\\includegraphics[width=10cm,height=10cm]{#1}}\n\t");
	out += wxT("}\n}\n");
	wxFprintf(m_pstream, wxT("%s"), out.c_str());
	wxFprintf(m_pstream, wxT("\\let\\realwrite\\write\n\n") );


	wxFprintf(m_pstream, wxT("\n\n\\begin{document}\n\n") );

	return true;
}

bool
SP_ExportLatex::WriteLatex()
{

	//Title page
	//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Title Page") );

	WriteTitlePage();

	//Basic Page Layout
	wxFprintf(m_pstream, wxT("\\pagestyle{fancy}\n") );
	wxFprintf(m_pstream, wxT("\\fancyhf{}\n") );
	wxFprintf(m_pstream, wxT("\\rhead{Snoopy2\\LaTeX}\n") );

	//Insert Date to footer
	if( m_pcCheckBox_BasicsLayout[1]->GetValue() && m_pcCheckBox_BasicsLayout[2]->GetValue()) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\today \\hspace{1pt} \\currenttime}\n") );
	} else if( m_pcCheckBox_BasicsLayout[1]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\today}\n") );
	} else if( m_pcCheckBox_BasicsLayout[2]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\lfoot{\\currenttime}\n") );
	}

	wxString out;

	//Insert user-defined header
	if( m_pcCheckBox_BasicsLayout[3]->GetValue() ) {
		out = wxT("\\lhead{ ");
		out += EditStringforLatex( m_pcTextCtrlHeaderFooter[0]->GetValue(), false) + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	} else {
		out = wxT("\\lhead{ ");
		out += InputNetname + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}

	//Insert user-defined footer
	if( m_pcCheckBox_BasicsLayout[4]->GetValue() ) {
		out = wxT("\\cfoot{ ");
		out += EditStringforLatex( m_pcTextCtrlHeaderFooter[1]->GetValue(), false) + wxT(" }\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}


	//Add table of contents
	wxFprintf(m_pstream, wxT("\n\\newpage\n") );
	wxFprintf(m_pstream, wxT("\\tableofcontents\n\n") );

	////Insert Page number to footer
	if( m_pcCheckBox_BasicsLayout[0]->GetValue() ) {
		wxFprintf(m_pstream, wxT("\\pagenumbering{arabic}\n") );
		wxFprintf(m_pstream, wxT("\\rfoot{Page \\thepage \\hspace{1pt} of \\pageref*{LastPage} }\n\n") );
	}

	wxRearrangeList* l_pcRearrangelist_General = m_pcRearrangeCtrl_General->GetList();

	for(int i = 0; i < l_pcRearrangelist_General->GetCount(); i++) {
		if( l_pcRearrangelist_General->IsChecked(i) ) {
			int order = l_pcRearrangelist_General->GetCurrentOrder()[ i ];

			if(order == 0) {
				//Add Basics section

				//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Basics") );
				WriteBasics();

			} else if(order == 1) {
			    //Add Graph Elements section

				//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Graph Elements") );
				WriteGraphElements();

			} else if(order == 2) {
				//Add Declarations section

				//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Declarations") );

				if( ((m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
					 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
					 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS)
					 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS)
					 || (m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS) )) {

					WriteDeclarations_Colored();
				} else {

					WriteDeclarations();
				}

			} else if(order == 3) {
				//Add Hierarchy section
				//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Hierarchy") );
			    WriteHierarchy();
			}
		}
	}

	if( m_flagImages ) {
		wxFprintf(m_pstream, wxT("\\clearpage\n") );
		wxFprintf(m_pstream, wxT("\\section{Full-size Images}\n\t") );
		wxFprintf(m_pstream, wxT("\\unvbox\\savedimgs\n\n") );

	}

	//Add references page
	//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...References") );
	WriteReferences();

	//Add Glossary Page
	//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Glossary") );
	WriteGlossary();

	return true;
}

bool
SP_ExportLatex::WriteTitlePage()
{
	const wxString latexTitlePage_file = m_sFilePath + wxT("TitlePage.tex");
	SP_LOGMESSAGE( latexTitlePage_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexTitlePage_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream, wxT("\\begin{titlepage}\n\n\t") );

	wxFprintf(l_pstream, wxT("\\thispagestyle{fancy}\n\t") );
	wxFprintf(l_pstream, wxT("\\fancyhf{}\n\t") );
	wxFprintf(l_pstream, wxT("\\rhead{Snoopy2\\LaTeX}\n\t") );

	wxString out = wxT("\\lhead{");
	out << InputNetname + wxT("}\n\t");
	out << wxT("\\cfoot{\\today}\n\n");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	wxFprintf(l_pstream, wxT("\\begin{center}\n\n\t") );
	wxFprintf(l_pstream, wxT("\\vspace*{4cm}\n\t") );

	out = wxT("\\includegraphics[width=0.20\\textwidth]");
	out += wxT("{/Users/anjali/workspace/snoopy/images/snoopy_logo.png}~\\\\[1cm]\n\t");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	wxFprintf(l_pstream, wxT("\\vspace{5mm}\n\t") );
	wxFprintf(l_pstream, wxT("\\hrule\n\t") );
	wxFprintf(l_pstream, wxT("\\vspace{2mm}\n\t") );

	out = wxT("\\huge{\\bfseries ");
	out += InputNetname + wxT("}\n\t");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	wxFprintf(l_pstream, wxT("\\vspace{1.4mm}\n\t") );
	wxFprintf(l_pstream, wxT("\\hrule\n\t") );
	wxFprintf(l_pstream, wxT("\\vspace{8mm}\n\t") );
	wxFprintf(l_pstream, wxT("\\small {\\url {http://www-dssz.informatik.tu-cottbus.de/DSSZ/Software/Snoopy} }\n") );

	wxFprintf(l_pstream, wxT("\\end{center}\n\n") );
	wxFprintf(l_pstream, wxT("\\end{titlepage}\n\n") );

	//Disclaimer
	wxFprintf(l_pstream, wxT("\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\begin{center}\n\t") );

	if(m_pcBasics_Landscape->GetValue() ) {
		wxFprintf(l_pstream, wxT("\\vspace*{12cm}\n\t") );
	} else {
		wxFprintf(l_pstream, wxT("\\vspace*{20cm}\n\t") );
	}
	wxFprintf(l_pstream, wxT("This document has been generated with Snoopy \\cite{heiner:pn:2012,Rohr:bi:2010}. \\\\ \n\t") );
	wxFprintf(l_pstream, wxT("If you have any comments or suggestions, \\\\ \n\t") );
	wxFprintf(l_pstream, wxT("Please contact \\url{snoopy@informatik.tu-cottbus.de}\n") );

	wxFprintf(l_pstream, wxT("\\end{center}\n\n") );


	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./TitlePage.tex}\n") );

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
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\section{Basics}\n") );
	wxFprintf(l_pstream, wxT("This section contains basic information about the input net.") );

    //General Information
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\subsection{General Informations}\n") );
	wxFprintf(l_pstream, wxT("\\vspace{5mm}\n") );
	wxFprintf(l_pstream, wxT("\\noindent\n{") );

	wxString out;

	//File name
	if( m_pcCheckBox_BasicsGeneral[0]->GetValue() ) {
		out = wxT("\\textbf{File Name:} ");
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
		wxString authors = m_pcMeta->GetAttribute(wxT("Authors"))->GetValueString();
		if( authors.length() ) {
			out = wxT("\\textbf{Authors:} ");
			out += authors + wxT("\\\\ \n");
			wxFprintf(l_pstream, wxT("%s"), out.c_str());
		}
	}

	//Creation Timestamp
	if( m_pcCheckBox_BasicsGeneral[3]->GetValue() ) {
	    out = wxT("\\textbf{Creation Timestamp:} ");
	    out += m_pcMeta->GetAttribute(wxT("Created"))->GetValueString() + wxT("\\\\ \n");
	    wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Description
	if( m_pcCheckBox_BasicsGeneral[4]->GetValue() ) {
		wxString desc = m_pcMeta->GetAttribute(wxT("Description"))->GetValueString();
		if(desc.length()) {
			out = wxT("\\textbf{Description:} ");
			out += desc + wxT("\\\\ \n");
			wxFprintf(l_pstream, wxT("%s"), out.c_str());
		}
	}

	//Keywords
	if( m_pcCheckBox_BasicsGeneral[5]->GetValue() ) {
		wxString keywords = m_pcMeta->GetAttribute(wxT("Keywords"))->GetValueString();

		if(keywords.length()) {
			out = wxT("\\textbf{Keywords:} ");
			out += keywords + wxT("\\\\ \n");
			wxFprintf(l_pstream, wxT("%s"), out.c_str());
		}
	}

	//References
	if( m_pcCheckBox_BasicsGeneral[6]->GetValue() ) {

		//load references to export: add to map
		wxString references = m_pcMeta->GetAttribute(wxT("References"))->GetValueString();

		if(references.length()) {

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

			out = wxT("\\textbf{References:} \\cite{ ");

			map< pair<int, wxString>, wxString>::iterator it;
			for(it = SP_LatexReferencesIndex2Ref.begin(); it != SP_LatexReferencesIndex2Ref.end();) {
				out += (*it).first.second;  //add label to cite
				it++;

				if(it != SP_LatexReferencesIndex2Ref.end() ) {
					out += wxT(", ");
				}
			}

			out += wxT("}\n");

			wxFprintf(l_pstream, wxT("%s"), out.c_str());
		}

	}

	wxFprintf(l_pstream, wxT("}\n\n") );


	//Net Information
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\subsection{Net Informations}\n") );
	wxFprintf(l_pstream, wxT("\\vspace{5mm}\n\n") );


	//Net Class
	if( m_pcCheckBox_BasicsNet[0]->GetValue() ) {
		out = wxT("\\noindent{\\textbf{Net Class: } ");
		out += p_pcDoc->GetNetclassName() + wxT("}  \\\\ \\\\ \n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	//Element count
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

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\section{Graph Elements}\n") );
	wxFprintf(l_pstream, wxT("This section contains information related to graph elements specific to the net.\n") );
	//////////////////////

	for(int i = 0; i < m_pcRearrangelist_Graph->GetCount(); i++) {

		if( m_pcRearrangelist_Graph->IsChecked(i) ) {

			int order = m_pcRearrangelist_Graph->GetCurrentOrder()[ i ];

			wxString l_sElementDisplayName = m_Options_Graph[ order ];
			wxString element = l_sElementDisplayName;

			if( element.GetChar( element.size()-1) == 'y') {
				element = element.BeforeLast('y') + wxT("ies");
			} else {
				element += wxT("s");
			}

			//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...") + element );

			wxString latexEle_file = m_sFilePath + EditStringforLatex( element ) + wxT(".tex");

			wxPrintData *pd1 = new wxPrintData();
			SetUpPrintData(*pd1, latexEle_file);

			FILE* l_pstream1 = wxFopen( (*pd1).GetFilename().c_str(), wxT("w+"));

			if (!l_pstream1)
			{
				SP_LOGERROR(_("Cannot open file for Latex output!"));
				return FALSE;
			}

			//wxFprintf(l_pstream1, wxT("\\needspace{10\\baselineskip}\n\n") );

			wxFprintf(l_pstream1, wxT("\\subsection{") + element + wxT("}\n") );
			wxFprintf(l_pstream1, wxT("\\begin{center}\n") );

			//-----------

			//create table for subsection

			wxArrayInt l_nArrayIntAttributes;
			wxCheckListBox* l_pcCheckListAttributes = SP_Node2AttributeCheckList[ order ];
			int l_nAttributes = l_pcCheckListAttributes->GetCheckedItems( l_nArrayIntAttributes );

			SP_DS_Nodeclass* nodeclass = m_pcGraph->GetNodeclassByDisplayedName( l_sElementDisplayName );
			SP_DS_Edgeclass* edgeclass = m_pcGraph->GetEdgeclassByDisplayedName( l_sElementDisplayName );
			SP_DS_Metadataclass* metadataclass = m_pcGraph->GetMetadataclassByDisplayedName( l_sElementDisplayName );

			if( nodeclass ) {

				wxArrayInt l_nArrayIntElements;
				wxCheckListBox* l_pcCheckListElements = SP_Node2NodeCheckList[ order ];
				map<wxString, wxString> l_sAttrNameMap = SP_Node2AttrNameMap[ l_sElementDisplayName ];

				int l_nElements = l_pcCheckListElements->GetCheckedItems( l_nArrayIntElements );

				if( l_nAttributes && l_nElements ) { //if non-zero number of attributes and nodes selected -> draw table

					SP_LOGMESSAGE( wxT("nodeclass found: ") + nodeclass->GetDisplayName() + wxT("->")+ wxString::Format(wxT("%i"), l_nElements));

					SP_ListAttribute::const_iterator itAttr;
					SP_DS_Node* l_pcNode;
					wxArrayString l_sNodeList; //list of node names

					for(int j = 0; j < l_nElements; j++) {
						int index = l_nArrayIntElements[ j ];
						wxString element_name = l_pcCheckListElements->GetString( index );
						l_sNodeList.Add( element_name );
					}

					l_pcNode = SP_Name2Node[ l_sNodeList[0] ];

					wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
					wxString out = wxT("{ | ");

					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = l_pcNode->GetAttribute( l_sAttrNameMap[ name ] );

						if( name.compare( wxT("Order Lexicographically")) ) {
							if( (name.compare( wxT("ID") ) == 0) ||
									(name.compare( wxT("Logic") ) == 0) ||
									(name.compare( wxT("Marking") ) == 0) ||
									(name.compare( wxT("Net number") ) == 0) ) {
								out += wxT(">{\\hspace{0pt}} X[-0.5, l] |");
							} else {
								if(sp && sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned k = 0; k < l_pcColAttr->GetColCount(); k++) {
										if(k < 2) {
											out += wxT(">{\\hspace{0pt}} X[-1, l] |");
										}
									}

								} else {
									out += wxT(">{\\hspace{0pt}} X[-1, l] |");
								}
							}
						}
					}
					out += wxT("}\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

					out = "";
					int l_ncols = 0;
					int flag_crossRef = 0;
					int flag_Order = 0;

					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = l_pcNode->GetAttribute( l_sAttrNameMap[ name ] );

						if( sp ) {  //not cross-ref or ordering
							SP_LOGMESSAGE( wxT("Nodeclass attribute it is: >>>>>> ") + name);

							if( name.compare( wxT("Logic") ) == 0) {
								out += wxT("\\hspace{0pt}LOG. &");
								l_ncols++;
							} else if( name.compare( wxT("Marking") ) == 0) {
								out += wxT("\\hspace{0pt}MAR. &");
								l_ncols++;
							} else {

								//collist attribute (added to table as a column entry if count is 1)
								if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned k = 0; k < l_pcColAttr->GetColCount(); k++) {
										wxString l_sColLabel = l_pcColAttr->GetColLabel(k);

										if(l_sColLabel.IsEmpty()) {
											SP_LOGMESSAGE("Unnamed entry in COLLIST");
										}

										if( k < 2 ) {
											out += wxT("\\hspace{0pt}") + l_sColLabel.MakeUpper() + wxT(" & ");
											l_ncols++;
										} else {
											out = out.BeforeLast('&');
											out += wxT("; ") + l_sColLabel.MakeUpper() + wxT(" & ");
										}
									}

								} else {
									out += wxT("\\hspace{0pt}") + name.MakeUpper();
									out += wxT(" & ");
									l_ncols++;
								}
							}
						} else if( name.compare( wxT("Cross Refs.") ) == 0) {
							out += wxT("\\hspace{0pt}");
							out += name.MakeUpper();
							flag_crossRef = 1;
							l_ncols++;
						} else if(name.compare( wxT("Net number") ) == 0) {
							out += wxT("\\hspace{0pt}NET.");
							out += wxT(" & ");
							l_ncols++;
						} else if( name.compare( wxT("Order Lexicographically") ) == 0 ) {
							flag_Order = 1;
						} else {
							out += wxT("\\hspace{0pt}") + name.MakeUpper();
							out += wxT(" & ");
							l_ncols++;
						}
					}

					if( !flag_crossRef  ) {
						out = out.BeforeLast('&');
					}

					out += wxT("\\\\ \\hline \\hline \\endhead\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
							+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
					out+= wxT("\\hline \\hline \\endlastfoot\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = "";

					if(flag_Order) {
						l_sNodeList.Sort();
					}

					//populate table with element details
					for(int j = 0; j < l_nElements; j++) {
						out = "";
						wxString element_name = l_sNodeList[j];
						l_pcNode = SP_Name2Node[ element_name ];

						if(l_pcNode) SP_LOGMESSAGE( element_name );

						for(int i = 0; i < l_nAttributes; i++) {

							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = l_pcNode->GetAttribute( l_sAttrNameMap[ name ] );

							if( sp ) { //not cross-ref or ordering
								SP_LOGMESSAGE( wxT("Nodeclass attribute it is: >>>2>>> ") + name);

								wxString value = sp->GetValueString();

								if( name.compare( wxT("Name") ) == 0 ) {   //add cross-reference label
									if(value.size() == 0) {   //no name attribute
										//fetch default assigned name
										value = dynamic_cast<SP_DS_NameAttribute*>( (l_pcNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME) )->GetValue();
									}
									wxString l_slabel = EditStringforCrossRef(value);

									value = EditStringforLatex(value);
									out += value + wxT(" \\linkto{") + l_slabel + wxT("} &");
								} else if( name.compare( wxT("Logic") ) == 0 ) {

									if(value == wxT("0") )
										value = wxT("\\xmark");
									else
										value = wxT("\\cmark");

									out += value + wxT("&");

								} else {

									//collist attribute
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);


										if(l_pcColAttr->GetRowCount() >= 2) {

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(row, 0);

												if(!l_sSetName.IsEmpty()) {
													l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
													SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
												}

												out += l_sSetName;

												if( row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 1; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {    //single row

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 0; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										}

									} else {
										out += value + wxT("&");
									}
								}

							} else if( name.compare( wxT("Cross Refs.") ) == 0 ) {
								//add cross-refs here
								wxString value = SP_NetNumber2Label[ wxString::Format(wxT("%i"), l_pcNode->GetNetnumber()) ];
								wxString l_slinkLabel = EditStringforCrossRef( value);

								if(i == 0) {
									value = EditStringforLatex( value );

									out += wxT("\\hyperref[") + l_slinkLabel
											+ wxT("]{") + value +  wxT("} &");
								} else {

									SP_ListGraphic* l_lGraphics = l_pcNode->GetGraphics();

									SP_ListGraphic::iterator l_Iter;
									set<int> l_SetNetNumber;
									set<int>::iterator it;

									for(l_Iter = l_lGraphics->begin(); l_Iter != l_lGraphics->end(); ++l_Iter)
									{
										l_SetNetNumber.insert( (*l_Iter)->GetNetnumber() );
									}

									for(it = l_SetNetNumber.begin(); it != l_SetNetNumber.end(); it++) {
										value = SP_NetNumber2Label[ wxString::Format(wxT("%i"), (*it) ) ];
										l_slinkLabel = EditStringforCrossRef(value);

										value = EditStringforLatex( value );
										out += wxT("\\hyperref[") + l_slinkLabel
												+ wxT("]{") + value +  wxT("} \\linebreak\n");
									}

									if( l_SetNetNumber.size() ) {
										out = out.BeforeLast(' ');
									}
									out += wxT(" \\\\ \n");
								}

							} else if( name.compare( wxT("Net number") ) == 0 ) {
								wxString value = wxString::Format(wxT("%i"), l_pcNode->GetNetnumber());
								if(i == 0) {
									out += value + wxT("&");
								} else {
									out += value + wxT("&");
								}
							} else if(name.compare( wxT("Order Lexicographically") ) ) {
								out += wxT("&");
							}
						}

						if( !flag_crossRef  ) {
							out = out.BeforeLast('&') + wxT("\\\\");
						}
						out += wxT("\\hline\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					}

					wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );

				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}

			}

			else if( edgeclass ) {

				SP_LOGMESSAGE( wxT("Edgeclass found: ") + edgeclass->GetDisplayName() );

				SP_ListAttribute::const_iterator itAttr;
				SP_DS_Edge* l_pcEdge;
				map<wxString, wxString> l_sAttrNameMap = SP_Node2AttrNameMap[ l_sElementDisplayName ];

				m_pEdgesPlace2Transition.clear();
				m_pEdgesTransition2Place.clear();

				m_pEdgesPlace2Transition = SP_Edgeclass2EdgeList[ l_sElementDisplayName ].first;
				m_pEdgesTransition2Place = SP_Edgeclass2EdgeList[ l_sElementDisplayName ].second;

				int l_nEdgesCount1 = m_pEdgesPlace2Transition.size();
				int l_nEdgesCount2 = m_pEdgesTransition2Place.size();


				if( l_nAttributes && ( l_nEdgesCount1 || l_nEdgesCount2 ) ) {  //if non-zero number of attributes selected -> draw table

					//order edges as per user-customized ordering and grouping
					if( m_pcOrderBySource->GetValue() ) {
						//order by source
						sort(m_pEdgesPlace2Transition.begin(), m_pEdgesPlace2Transition.end() );
						sort(m_pEdgesTransition2Place.begin(), m_pEdgesTransition2Place.end() );
					} else {
						//order by target
						sort(m_pEdgesPlace2Transition.begin(), m_pEdgesPlace2Transition.end(), compareTarget);
						sort(m_pEdgesTransition2Place.begin(), m_pEdgesTransition2Place.end(), compareTarget);
					}

					wxString element_name;

					//fetching one edge for reference
					if( l_nEdgesCount1 ) {
						element_name = m_pEdgesPlace2Transition[0].first + wxT("->") + m_pEdgesPlace2Transition[0].second;
					} else {
						element_name = m_pEdgesTransition2Place[0].first + wxT("->") + m_pEdgesTransition2Place[0].second;
					}
					l_pcEdge = SP_Name2Edge[ element_name ];

					wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
					wxString out = wxT("{ | >{\\hspace{0pt}} X[-1, l] | >{\\hspace{0pt}} X[-0.5, l] | >{\\hspace{0pt}} X[-1, l] | >{\\hspace{0pt}} X[-0.5, l] | ");

					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

						if( name.compare( wxT("Order Lexicographically")) ) {
							if( name.compare( wxT("Multiplicity") ) == 0) {
								out += wxT(">{\\hspace{0pt}} X[-0.5, l] |");
							} else {
								if(sp && sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned int k = 0; k < l_pcColAttr->GetColCount(); k++) {
										out += wxT(">{\\hspace{0pt}} X[-1, l] |");
									}

								} else {
									out += wxT(">{\\hspace{0pt}} X[-1, l] |");
								}
							}
						}
					}
					out += wxT("}\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

					int flag_crossRef = 0;

					out = wxT("\\multicolumn{2}{|c|}{SOURCE} & \\multicolumn{2}{|c|}{TARGET} ");
					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

						if(sp) {

							if(sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
								SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

								for(unsigned int k = 0; k < l_pcColAttr->GetColCount(); k++) {
									if(k < 2) {
										out += wxT("& ");
									}
								}
							} else {
								out += wxT("& ");
							}
						} else {
							out += wxT("& ");
						}
					}

					out += wxT("\\\\ \\cline{1-4}\n");

					out += wxT("NAME & TYPE & NAME & TYPE &");

					int l_ncols = 4;
					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
						SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

						if( sp ) {  //not cross-ref or ordering

							if( name.compare( wxT("Multiplicity")) == 0) {
								out += wxT("\\hspace{0pt} MUL. &");
								l_ncols++;
							} else {

								//collist attribute (added to table as a column entry if count is 1)
								if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned k = 0; k < l_pcColAttr->GetColCount(); k++) {
										wxString l_sColLabel = l_pcColAttr->GetColLabel(k);

										if(l_sColLabel.IsEmpty()) {
											SP_LOGMESSAGE("Unnamed entry in COLLIST");
										}

										if( k < 2 ) {
											out += wxT("\\hspace{0pt}") + l_sColLabel.MakeUpper() + wxT(" & ");
											l_ncols++;
										} else {
											out = out.BeforeLast('&');
											out += wxT("; ") + l_sColLabel.MakeUpper() + wxT(" & ");
										}
									}

								} else {
									out += wxT("\\hspace{0pt}") + name.MakeUpper();
									out += wxT(" & ");
									l_ncols++;
								}
							}
						} else if( name.compare( wxT("Cross Refs.") ) == 0) {
							out += wxT("\\hspace{0pt}");
							out += name.MakeUpper();
							flag_crossRef = 1;
							l_ncols++;
						} else if(name.compare( wxT("Order Lexicographically") )) {
							out += wxT("\\hspace{0pt}") + name.MakeUpper();
							out += wxT(" & ");
							l_ncols++;
						}
					}

					if( !flag_crossRef  ) {
						out = out.BeforeLast('&');
					}

					out += wxT("\\\\ \\hline \\hline \\endhead\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
							+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
					out+= wxT("\\hline \\hline \\endlastfoot\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					if( m_pcGroupByPlace2Transition->GetValue() ) {
						SP_LOGMESSAGE( wxT("Group By Place2Transition"));

						//Place to Transition
						for(int j = 0; j < l_nEdgesCount1; j++) {
							wxString l_sSourceName = m_pEdgesPlace2Transition[j].first;
							wxString l_sTargetName = m_pEdgesPlace2Transition[j].second;
							wxString element_name = l_sSourceName + wxT("->") + l_sTargetName;
							l_pcEdge = SP_Name2Edge[ element_name ];

							wxString l_slinklabel = EditStringforCrossRef(l_sSourceName);
							l_sSourceName = EditStringforLatex( l_sSourceName );

							wxString l_sAbbrSource = l_pcEdge->GetSource()->GetClassObject()->GetAbbreviation();
							wxString l_sAbbrTarget = l_pcEdge->GetTarget()->GetClassObject()->GetAbbreviation();
							wxString l_sSourceClass = l_pcEdge->GetSource()->GetClassName();
							wxString l_sTargetClass = l_pcEdge->GetTarget()->GetClassName();

							//For glossary
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrSource, l_sSourceClass));
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrTarget, l_sTargetClass));

							out = wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sSourceName
									+ wxT("} & ") + l_sAbbrSource + (" &");

							l_slinklabel = EditStringforCrossRef(l_sTargetName);
							l_sTargetName = EditStringforLatex( l_sTargetName );

							out += wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sTargetName
									+ wxT("} & ") + l_sAbbrTarget + (" &");



							if(l_pcEdge) SP_LOGMESSAGE( element_name );

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

								if( sp ) { //not cross-ref or ordering
									wxString value = sp->GetValueString();

									//collist attribute
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										if(l_pcColAttr->GetRowCount() >= 2) {

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(row, 0);

												if(!l_sSetName.IsEmpty()) {
													l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
													SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
												}

												out += l_sSetName;

												if( row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 1; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {    //single row

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 0; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										}


									} else {
										value = EditStringforLatex( value );
										out += value + wxT("&");
									}
								} else {
									out += wxT("&");
								}
							}

							out = out.BeforeLast('&');
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}

						//Transition to Place
						for(int j = 0; j < l_nEdgesCount2; j++) {
							wxString l_sSourceName = m_pEdgesTransition2Place[j].first;
							wxString l_sTargetName = m_pEdgesTransition2Place[j].second;
							wxString element_name = l_sSourceName + wxT("->") + l_sTargetName;
							l_pcEdge = SP_Name2Edge[ element_name ];

							wxString l_slinklabel = EditStringforCrossRef(l_sSourceName);
							l_sSourceName = EditStringforLatex( l_sSourceName );

							wxString l_sAbbrSource = l_pcEdge->GetSource()->GetClassObject()->GetAbbreviation();
							wxString l_sAbbrTarget = l_pcEdge->GetTarget()->GetClassObject()->GetAbbreviation();
							wxString l_sSourceClass = l_pcEdge->GetSource()->GetClassName();
							wxString l_sTargetClass = l_pcEdge->GetTarget()->GetClassName();

							//For glossary
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrSource, l_sSourceClass));
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrTarget, l_sTargetClass));

							out = wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sSourceName
									+ wxT("} & ") + l_sAbbrSource + (" &");

							l_slinklabel = EditStringforCrossRef(l_sTargetName);
							l_sTargetName = EditStringforLatex( l_sTargetName );

							out += wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sTargetName
									+ wxT("} & ") + l_sAbbrTarget + (" &");

							if(l_pcEdge) SP_LOGMESSAGE( element_name );

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

								if( sp ) { //not cross-ref or ordering
									wxString value = sp->GetValueString();

									//collist attribute
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										if(l_pcColAttr->GetRowCount() >= 2) {

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(row, 0);

												if(!l_sSetName.IsEmpty()) {
													l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
													SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
												}

												out += l_sSetName;

												if( row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 1; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {    //single row

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 0; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										}


									} else {
										value = EditStringforLatex( value );
										out += value + wxT("&");
									}
								} else {
									out += wxT("&");
								}
							}

							out = out.BeforeLast('&');
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}
					} else {

						//Transition to Place
						for(int j = 0; j < l_nEdgesCount2; j++) {
							wxString l_sSourceName = m_pEdgesTransition2Place[j].first;
							wxString l_sTargetName = m_pEdgesTransition2Place[j].second;
							wxString element_name = l_sSourceName + wxT("->") + l_sTargetName;
							l_pcEdge = SP_Name2Edge[ element_name ];

							wxString l_slinklabel = EditStringforCrossRef(l_sSourceName);
							l_sSourceName = EditStringforLatex( l_sSourceName );

							wxString l_sAbbrSource = l_pcEdge->GetSource()->GetClassObject()->GetAbbreviation();
							wxString l_sAbbrTarget = l_pcEdge->GetTarget()->GetClassObject()->GetAbbreviation();
							wxString l_sSourceClass = l_pcEdge->GetSource()->GetClassName();
							wxString l_sTargetClass = l_pcEdge->GetTarget()->GetClassName();

							//For glossary
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrSource, l_sSourceClass));
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrTarget, l_sTargetClass));

							out = wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sSourceName
									+ wxT("} & ") + l_sAbbrSource + (" &");

							l_slinklabel = EditStringforCrossRef(l_sTargetName);
							l_sTargetName = EditStringforLatex( l_sTargetName );

							out += wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sTargetName
									+ wxT("} & ") + l_sAbbrTarget + (" &");


							if(l_pcEdge) SP_LOGMESSAGE( element_name );

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

								if( sp ) { //not cross-ref or ordering
									wxString value = sp->GetValueString();

									//collist attribute
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										if(l_pcColAttr->GetRowCount() >= 2) {

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(row, 0);

												if(!l_sSetName.IsEmpty()) {
													l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
													SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
												}

												out += l_sSetName;

												if( row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 1; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {    //single row

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 0; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										}


									} else {
										value = EditStringforLatex( value );
										out += value + wxT("&");
									}
								} else {
									out += wxT("&");
								}
							}

							out = out.BeforeLast('&');
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}

						//Place to Transition
						for(int j = 0; j < l_nEdgesCount1; j++) {
							wxString l_sSourceName = m_pEdgesPlace2Transition[j].first;
							wxString l_sTargetName = m_pEdgesPlace2Transition[j].second;
							wxString element_name = l_sSourceName + wxT("->") + l_sTargetName;
							l_pcEdge = SP_Name2Edge[ element_name ];

							wxString l_slinklabel = EditStringforCrossRef(l_sSourceName);
							l_sSourceName = EditStringforLatex( l_sSourceName );

							wxString l_sAbbrSource = l_pcEdge->GetSource()->GetClassObject()->GetAbbreviation();
							wxString l_sAbbrTarget = l_pcEdge->GetTarget()->GetClassObject()->GetAbbreviation();
							wxString l_sSourceClass = l_pcEdge->GetSource()->GetClassName();
							wxString l_sTargetClass = l_pcEdge->GetTarget()->GetClassName();

							//For glossary
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrSource, l_sSourceClass));
							SP_LatexGlossaryAbbr2Full.insert(pair< wxString, wxString > (l_sAbbrTarget, l_sTargetClass));

							out = wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sSourceName
									+ wxT("} & ") + l_sAbbrSource + (" &");

							l_slinklabel = EditStringforCrossRef(l_sTargetName);
							l_sTargetName = EditStringforLatex( l_sTargetName );

							out += wxT("\\hyperref[") + l_slinklabel
									+ wxT("]{") + l_sTargetName
									+ wxT("} & ") + l_sAbbrTarget + (" &");

							if(l_pcEdge) SP_LOGMESSAGE( element_name );

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcEdge->GetAttribute( l_sAttrNameMap[ name] );

								if( sp ) { //not cross-ref or ordering
									wxString value = sp->GetValueString();

									//collist attribute
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										if(l_pcColAttr->GetRowCount() >= 2) {

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(row, 0);

												if(!l_sSetName.IsEmpty()) {
													l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
													SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
												}

												out += l_sSetName;

												if( row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 1; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {    //single row

											for(unsigned row = 0; row < l_pcColAttr->GetRowCount(); row++)
											{
												for(unsigned col = 0; col < l_pcColAttr->GetColCount(); col++)
												{
													wxString l_sSetName = l_pcColAttr->GetCell(row, col);

													if(!l_sSetName.IsEmpty()) {
														l_sSetName = wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
														SP_LOGMESSAGE( wxT("COLLIST:") + l_sSetName);
													}

													if(col < 2) {
														out += l_sSetName + wxT("&");
													} else {
														if(col == 2) {
															out = out.BeforeLast('&');
															out += wxT("; ");
														}
														out += l_sSetName + wxT("; ");
													}
												}

												if(l_pcColAttr->GetColCount() > 2) {
													out = out.BeforeLast(';');
												} else {
													out = out.BeforeLast('&');
												}

												if(row < l_pcColAttr->GetRowCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										}


									} else {
										value = EditStringforLatex( value );
										out += value + wxT("&");
									}
								} else {
									out += wxT("&");
								}
						    }

							out = out.BeforeLast('&');
							out += wxT("\\\\ \\hline\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						}

					}

					wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );
				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}

			}

			else if( metadataclass ) {

				SP_LOGMESSAGE( wxT("Metadataclass found: ") + metadataclass->GetDisplayName() );

				SP_ListAttribute::const_iterator itAttr;
				SP_DS_Metadata* l_pcProtoMeta = metadataclass->GetPrototype();
				SP_DS_Metadata* l_pcMetadata;
				map<wxString, wxString> l_sAttrNameMap = SP_Node2AttrNameMap[ l_sElementDisplayName ];

				if( element.compare( wxT("Comments")) == 0) {  //Comment

					if( l_nAttributes ) {

						wxArrayString l_sComments = SP_Nodeclass2NodeList[ wxT("Comment") ];

						if( !l_sComments.IsEmpty() )
						{
							wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
							wxString out = wxT("{ |");

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcProtoMeta->GetAttribute( l_sAttrNameMap[name] );

								if( name.compare( wxT("Order Lexicographically")) ) {

									if(sp && sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
										SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										for(unsigned k = 1; k < l_pcProtoColAttr->GetColCount(); k++) {
											out += wxT(">{\\hspace{0pt}} X[-1, l] |");
										}

									} else {
										out += wxT(">{\\hspace{0pt}} X[-1, l] |");
									}
								}
							}
							out += wxT("}\n\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());

							wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

							out = "";
							int l_ncols = 0;
							int flag_crossRef = 0;

							for(int i = 0; i < l_nAttributes; i++) {
								wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
								SP_DS_Attribute* sp = l_pcProtoMeta->GetAttribute( l_sAttrNameMap[name] );

								if( sp ) {  //not cross-ref or ordering

									//collist attribute (added to table as a column entry if count is 1)
									if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

										SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

										for(unsigned k = 1; k < l_pcProtoColAttr->GetColCount(); k++) {
											wxString l_sColLabel = l_pcProtoColAttr->GetColLabel(k);
											out += wxT("\\hspace{0pt}") + l_sColLabel.MakeUpper() + wxT(" & ");
											l_ncols++;
										}
									} else {
										out += wxT("\\hspace{0pt}") + name.MakeUpper();
										out += wxT(" & ");
										l_ncols++;
									}
								} else if( name.compare( wxT("Cross Refs.") ) == 0) {
									out += wxT("\\hspace{0pt}");
									out += name.MakeUpper();
									flag_crossRef = 1;
									l_ncols++;
								} else if( name.compare( wxT("Net reference") ) == 0) {
									out += wxT("\\hspace{0pt}");
									out += name.MakeUpper();
									out += wxT(" & ");
									l_ncols++;
								}
							}

							if( !flag_crossRef  ) {
								out = out.BeforeLast('&');
							}

							out += wxT("\\\\ \\hline \\hline \\endhead\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());

							out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
									+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
							out+= wxT("\\hline \\hline \\endlastfoot\n\n");
							wxFprintf(l_pstream1, wxT("%s"), out.c_str());

							//populate table with element details

							int l_nComments = l_sComments.size();


							wxArrayString l_sComments = SP_Nodeclass2NodeList[ wxT("Comment") ];

							for(int i = 0; i < l_nComments; i++) {
								out = "";
								l_pcMetadata = SP_Name2Metadata[ l_sComments[i] ];

								for(int i = 0; i < l_nAttributes; i++) {
									wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
									SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sAttrNameMap[name] );

									if( sp ) { //not cross-ref or ordering

										wxString value = sp->GetValueString();

										//collist attribute
										if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
											SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

											for(unsigned k = 1; k < l_pcColAttr->GetColCount(); k++)
											{
												wxString l_sSetName = l_pcColAttr->GetActiveCellValue(k);
												out += wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");

												if(k < l_pcColAttr->GetColCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}
										} else {

											if( name.compare( wxT("Comment")) == 0) {
												value = EditStringforLatex(value, false);
											} else {
												value = EditStringforLatex( value );
											}											//value = wxT("$") + value + wxT("$");
											out += value + wxT("&");
										}

									} else if( name.compare( wxT("Net reference")) == 0) {
										wxString value = SP_NetNumber2Label[ wxString::Format(wxT("%i"), l_pcMetadata->GetNetnumber()) ];
										wxString l_slinkLabel = EditStringforCrossRef( value);
										value = EditStringforLatex( value );

										out += wxT("\\hyperref[") + l_slinkLabel
												+ wxT("]{") + value +  wxT("} &");
									} else {
										out += wxT("&");
									}
								}

								out = out.BeforeLast('&');
								out += wxT("\\\\ \\hline\n");
								wxFprintf(l_pstream1, wxT("%s"), out.c_str());
							}

							wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );
						} else {
							wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
						}

					} else {
						wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
					}
				}
				else {  //if metadata class is not comment
					wxArrayInt l_nArrayIntElements;
					wxCheckListBox* l_pcCheckListElements = SP_Node2NodeCheckList[ order ];
					int l_nElements = l_pcCheckListElements->GetCheckedItems( l_nArrayIntElements );

					if( l_nAttributes && l_nElements) {  //if non-zero number of attributes selected -> draw table

						wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
						wxString out = wxT("{ | ");

						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = l_pcProtoMeta->GetAttribute( l_sAttrNameMap[name] );

							if( name.compare( wxT("Order Lexicographically")) ) {

								if(sp && sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
									SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned k = 1; k < l_pcProtoColAttr->GetColCount(); k++) {
										out += wxT(">{\\hspace{0pt}} X[-1, l] |");
									}

								} else {
									out += wxT(">{\\hspace{0pt}} X[-1, l] |");
								}
							}
						}
						out += wxT("}\n\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());

						wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

						out = "";
						int l_ncols = 0;
						int flag_crossRef = 0;
						int flag_Order = 0;

						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
							SP_DS_Attribute* sp = l_pcProtoMeta->GetAttribute( l_sAttrNameMap[name] );

							if( sp ) {  //not cross-ref or ordering

								if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

									SP_DS_ColListAttribute* l_pcProtoColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									for(unsigned k = 1; k < l_pcProtoColAttr->GetColCount(); k++) {
										wxString l_sColLabel = l_pcProtoColAttr->GetColLabel(k);
										out += wxT("\\hspace{0pt}") + l_sColLabel.MakeUpper() + wxT(" & ");
										l_ncols++;
									}

								} else {
									out += wxT("\\hspace{0pt}") + name.MakeUpper();
									out += wxT(" & ");
									l_ncols++;
								}

							} else if( name.compare( wxT("Cross Refs.") ) == 0) {
								out += wxT("\\hspace{0pt}");
								out += name.MakeUpper();
								out += wxT(" & ");
								flag_crossRef = 1;
								l_ncols++;
							} else if( name.compare( wxT("Order Lexicographically") ) == 0 ) {
								flag_Order = 1;
							} else {
								out += wxT("\\hspace{0pt}");
								out += name.MakeUpper();
								out += wxT(" & ");
								l_ncols++;
							}
						}

						if( !flag_crossRef  ) {
							out = out.BeforeLast('&');
						}

						out += wxT("\\\\ \\hline \\hline \\endhead\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());

						out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
								+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
						out+= wxT("\\hline \\hline \\endlastfoot\n\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());


						//populate table with element details

						if( element.compare( wxT("Images")) == 0) {

							wxArrayString l_ImageList;
							for(int j = 0; j < l_nElements; j++) {
								int index = l_nArrayIntElements[ j ];
								wxString element_name = l_pcCheckListElements->GetString( index );
								l_pcMetadata = SP_Name2Metadata[ element_name ];

								SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( wxT("Filename") );

								if( sp ) {
									wxString value = EditStringforLatex( sp->GetValueString() );
									l_ImageList.Add(value);
								}
							}

							if(!l_ImageList.IsEmpty()) {
								m_flagImages = 1;
							}

							for(int j = 0; j < l_nElements; j++) {
								out = "";
								int index = l_nArrayIntElements[ j ];
								wxString element_name = l_pcCheckListElements->GetString( index );
								l_pcMetadata = SP_Name2Metadata[ element_name ];

								if(l_pcMetadata) SP_LOGMESSAGE( element_name );

								for(int i = 0; i < l_nAttributes; i++) {
									wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );

									if(name.compare( wxT("Image reference")) == 0) {

										out += wxT("\n\\linebreak \\ifx\\write\\realwrite\\thumbnailandappendix{")
												+ l_ImageList[j]
												+ wxT("} \\fi \\medskip");
										out += wxT("&");
									} else {
										SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sAttrNameMap[name] );

										if( sp ) {
											wxString value = EditStringforLatex( sp->GetValueString() );
											out += value + wxT("&");
										}
									}
								}

								out = out.BeforeLast('&');
								out += wxT("\\\\ \\hline\n");
								wxFprintf(l_pstream1, wxT("%s"), out.c_str());
							}


						} else {

							for(int j = 0; j < l_nElements; j++) {
								out = "";
								int index = l_nArrayIntElements[ j ];
								wxString element_name = l_pcCheckListElements->GetString( index );
								l_pcMetadata = SP_Name2Metadata[ element_name ];

								if(l_pcMetadata) SP_LOGMESSAGE( element_name );

								for(int i = 0; i < l_nAttributes; i++) {
									wxString name = l_pcCheckListAttributes->GetString( l_nArrayIntAttributes[i] );
									SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sAttrNameMap[name] );

									if( sp ) { //not cross-ref or ordering
										wxString value = sp->GetValueString();

										//collist attribute
										if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
											SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

											for(unsigned k = 1; k < l_pcColAttr->GetColCount(); k++)
											{
												wxString l_sSetName = l_pcColAttr->GetActiveCellValue(k);
												out += wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");

												if(k < l_pcColAttr->GetColCount()-1) {
													out += wxT("\\linebreak\n");
												} else {
													out += wxT("&");
												}
											}

										} else {
											value = EditStringforLatex( value );
											out += value + wxT("&");
										}
									} else {
										out += wxT("&");
									}
								}

								out = out.BeforeLast('&');
								out += wxT("\\\\ \\hline\n");
								wxFprintf(l_pstream1, wxT("%s"), out.c_str());
							}
						}

						wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );
					} else {
						wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
					}
				}
			}

			wxFprintf(l_pstream1, wxT("\\end{center}\n") );

			//-----------
		    wxFprintf(l_pstream, wxT("\\input{./") + EditStringforLatex( element ) + wxT(".tex}\n") );

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
SP_ExportLatex::WriteDeclarations_Colored()
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

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\section{Declarations}\n") );
	wxFprintf(l_pstream, wxT("This section contains information related to declarations specific to the net.") );

	//////////////////////

	for(int i = 0; i < m_pcRearrangelist_declarations->GetCount(); i++) {

		if( m_pcRearrangelist_declarations->IsChecked(i) ) {

			int order = m_pcRearrangelist_declarations->GetCurrentOrder()[ i ];

			wxString element = m_Options_Declarations[ order ];

			//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...") + element );

			wxString latexDecEle_file = m_sFilePath + EditStringforLatex( element ) + wxT(".tex");

			wxPrintData *pd1 = new wxPrintData();
			SetUpPrintData(*pd1, latexDecEle_file);

			FILE* l_pstream1 = wxFopen( (*pd1).GetFilename().c_str(), wxT("w+"));

			if (!l_pstream1)
			{
				SP_LOGERROR(_("Cannot open file for Latex output!"));
				return FALSE;
			}

			//wxFprintf(l_pstream1, wxT("\\needspace{10\\baselineskip}\n\n") );

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
				SP_DS_Metadata* l_pcMetadata;
				map<wxString, wxString> l_sDecAttrNameMap = SP_DecNode2DecAttrNameMap[ element ];

				if( l_nAttributes && l_nElements ) {  //if non-zero number of attributes selected -> draw table

		            //fetching one entry (first) for reference
					l_pcMetadata = *(metadataclass->GetElements()->begin());
					SP_DS_ColListAttribute * l_pcColList;
					wxString l_sColListName;

					wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
					wxString out = wxT("{ | ");

					//wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );

 					if(element.compare( wxT("Simple Color Sets")) == 0) {
 						l_sColListName = wxT("ColorsetList");
 					}
 					else if(element.compare( wxT("Compound Color Sets")) == 0) {
 						l_sColListName = wxT("StructuredColorsetList");
 					}
 					else if(element.compare( wxT("Alias Color Sets")) == 0) {
 						l_sColListName = wxT("AliasColorsetList");
 					}
 					else if(element.compare( wxT("Constants")) == 0) {
 						l_sColListName = wxT("ConstantList");
 					}
 					else if(element.compare( wxT("Functions")) == 0) {
 						l_sColListName = wxT("FunctionList");
 					}
 					else if(element.compare( wxT("Variables")) == 0) {
 						l_sColListName = wxT("VariableList");
 					}

 					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(l_sColListName) );

					if( l_pcColList ) {

						SP_LOGMESSAGE( wxT("<<<<<<<<<<COLLIST") + l_sColListName + wxT(" ") + l_sDecAttrNameMap[l_sColListName] );

						for(unsigned int k = 0; k < l_nAttributes; k++) {
							out += wxT(">{\\hspace{0pt}} X[-1, l] |");
						}

					} else {
						SP_LOGMESSAGE( wxT("<<<<<<<<<<NOT SP") + l_sColListName + wxT(" ") + l_sDecAttrNameMap[l_sColListName] );
					}


					out += wxT("}\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

					out = "";
					int l_ncols = 0;

					for(int i = 0; i < l_nAttributes; i++) {
						int check_index = l_nArrayIntDecAttributes[i];
						wxString l_sColAttrName = l_pcCheckListDecAttributes->GetString( check_index );
						out += wxT("\\hspace{0pt}") + l_sColAttrName.MakeUpper();
						out += wxT(" & ");
						l_ncols++;
					}

					out = out.BeforeLast('&');
					out += wxT("\\\\ \\hline \\hline \\endhead \n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
							+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
					out+= wxT("\\hline \\hline \\endlastfoot\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = "";

					//populate table with element details

					for(int j = 0; j < l_nElements; j++) {
						int checked_element = l_nArrayIntDecElements[j];
						wxString l_sCheckedElement = l_pcCheckListDecElements->GetString( checked_element );

						if(l_sColListName.compare( wxT("FunctionList") ) == 0) {
							if(l_sCheckedElement.compare( l_pcColList->GetCell(checked_element, 1)) == 0)
							{ // is a checked element
								for(unsigned int c = 0; c < l_nAttributes; c++) {
									int check_index = l_nArrayIntDecAttributes[c];
									wxString l_sSetName = l_pcColList->GetCell(checked_element, check_index);

									if(!l_sSetName.IsEmpty()) {
										out += wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
									}

									out += wxT("&");
								}
							}
						} else {
							if(l_sCheckedElement.compare( l_pcColList->GetCell(checked_element, 0)) == 0)
							{ // is a checked element
								for(unsigned int c = 0; c < l_nAttributes; c++) {
									int check_index = l_nArrayIntDecAttributes[c];
									wxString l_sSetName = l_pcColList->GetCell(checked_element, check_index);

									if(!l_sSetName.IsEmpty()) {
										out += wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
									}

									out += wxT("&");
								}
							}
						}

						out = out.BeforeLast('&');
						out += wxT("\\\\ \\hline\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
						out = "";
					}

					wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );
				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}
			}

			wxFprintf(l_pstream1, wxT("\\end{center}\n") );

		    wxFprintf(l_pstream, wxT("\\input{./") + EditStringforLatex( element ) + wxT(".tex}\n") );

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

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\section{Declarations}\n") );
	wxFprintf(l_pstream, wxT("This section contains information related to declarations specific to the net.") );

	//////////////////////

	for(int i = 0; i < m_pcRearrangelist_declarations->GetCount(); i++) {

		if( m_pcRearrangelist_declarations->IsChecked(i) ) {

			int order = m_pcRearrangelist_declarations->GetCurrentOrder()[ i ];

			wxString element = m_Options_Declarations[ order ];

			//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...") + element );

			wxString latexDecEle_file = m_sFilePath + EditStringforLatex( element ) + wxT(".tex");

			wxPrintData *pd1 = new wxPrintData();
			SetUpPrintData(*pd1, latexDecEle_file);

			FILE* l_pstream1 = wxFopen( (*pd1).GetFilename().c_str(), wxT("w+"));

			if (!l_pstream1)
			{
				SP_LOGERROR(_("Cannot open file for Latex output!"));
				return FALSE;
			}

			//wxFprintf(l_pstream1, wxT("\\needspace{10\\baselineskip}\n\n") );

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
				SP_DS_Metadata* l_pcMetadata;
				map<wxString, wxString> l_sDecAttrNameMap = SP_DecNode2DecAttrNameMap[ element ];

				if( l_nAttributes && l_nElements ) {  //if non-zero number of attributes selected -> draw table

					wxArrayString l_sDecNodeList;

					for(int j = 0; j < l_nElements; j++) {
						int index = l_nArrayIntDecElements[ j ];
						wxString element_name = l_pcCheckListDecElements->GetString( index );
						l_sDecNodeList.Add( element_name );
					}

		            //fetching one entry (first) for reference
					l_pcMetadata = SP_Name2Metadata[ l_sDecNodeList[0] ];


					wxFprintf(l_pstream1, wxT("\\begin{longtabu}") );
					wxString out = wxT("{ | ");

					for(int i = 0; i < l_nAttributes; i++)
					{
						wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );
						SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sDecAttrNameMap[name] );

						if( name.compare( wxT("Order Lexicographically")) ) //any attribute except ordering
						{
							if(sp) {

								if(sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

									SP_LOGMESSAGE( wxT("<<<<<<<<<<COLLIST") + name + wxT(" ") + l_sDecAttrNameMap[name] );

									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									 if(l_pcColAttr->GetRowCount() > 0) {
										 for(unsigned int r = 0; r < l_pcColAttr->GetRowCount(); r++) {
											 out += wxT(">{\\hspace{0pt}} X[-1, l] |");
										 }
									 }

								} else {
									out += wxT(">{\\hspace{0pt}} X[-1, l] |");
								}

							} else if( name.compare( wxT("ID")) == 0 ) {
								out += wxT(">{\\hspace{0pt}} X[-0.5, l] |");
							} else {
								out += wxT(">{\\hspace{0pt}} X[-1, l] |");
							}
						}
					}
					out += wxT("}\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					wxFprintf(l_pstream1, wxT("\\caption{") + element + wxT(" Table}\\\\ \\hline\n") );

					out = "";
					int l_ncols = 0;
					int flag_Order = 0;

					for(int i = 0; i < l_nAttributes; i++) {
						wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );
						SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sDecAttrNameMap[name] );

						if( sp ) {  //not cross-ref or ordering

							//collist attribute (added to table as a column entry if count is 1)
							if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {

								SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

								if(l_pcColAttr->GetRowCount() > 0) {  //if non-zero number of entries in collist table

									for(unsigned int r = 0; r < l_pcColAttr->GetRowCount(); r++) {
										wxString l_sColLabel = l_pcColAttr->GetCell(r, 0);

										SP_LOGMESSAGE( wxT("==============================") + name + wxT("==>") + l_sColLabel);

										if(l_sColLabel.IsEmpty()) {
											SP_LOGMESSAGE("Unnamed entry in COLLIST");
										}
										out += wxT("\\hspace{0pt}") + l_sColLabel.MakeUpper() + wxT(" & ");
										l_ncols++;
									}
								}

							} else {
								out += wxT("\\hspace{0pt}") + name.MakeUpper();
								out += wxT(" & ");
								l_ncols++;
							}
						} else if( name.compare( wxT("Order Lexicographically")) == 0) {
							flag_Order = 1;
						} else {
							out += wxT("\\hspace{0pt}") + name.MakeUpper();
							out += wxT(" & ");
							l_ncols++;
						}
					}

					out = out.BeforeLast('&');

					out += wxT("\\\\ \\hline \\hline \\endhead \n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = wxT("\\hline \\multicolumn{") + wxString::Format(wxT("%i"), l_ncols)
							+ wxT("}{|r|}{{Continued on next page}}\\\\ \\hline \\endfoot\n");
					out+= wxT("\\hline \\hline \\endlastfoot\n\n");
					wxFprintf(l_pstream1, wxT("%s"), out.c_str());

					out = "";

					if(flag_Order) {
						l_sDecNodeList.Sort();
					}

					//////////////////////////////popoulate table with element details

					for(int j = 0; j < l_nElements; j++) {
						out = "";
						wxString element_name = l_sDecNodeList[j];
						l_pcMetadata = SP_Name2Metadata[ element_name ];

						if(l_pcMetadata) SP_LOGMESSAGE( element_name );

						for(int i = 0; i < l_nAttributes; i++) {
							wxString name = l_pcCheckListDecAttributes->GetString( l_nArrayIntDecAttributes[i] );
							SP_DS_Attribute* sp = l_pcMetadata->GetAttribute( l_sDecAttrNameMap[name] );

							if( sp ) { //not cross-ref or ordering
								wxString value = EditStringforLatex( sp->GetValueString() );

								//collist attribute
								if (sp->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST) {
									SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(sp);

									if(l_pcColAttr->GetRowCount() > 0) {

										for(unsigned c = 1; c < l_pcColAttr->GetColCount(); c++) {
											for(unsigned r = 0; r < l_pcColAttr->GetRowCount(); r++)
											{
												wxString l_sSetName = l_pcColAttr->GetCell(r, c);

												if(!l_sSetName.IsEmpty()) {
													out += wxT("$") + EditStringforLatex(l_sSetName, false) + wxT("$");
												}

												out += wxT("&");
											}
										}
									}

								} else {
									out += value + wxT("&");
								}

							} else if( name.compare( wxT("Cross Refs.") ) == 0 ) {
								//add cross-refs here
								out += wxT(" & ");
							} else if( name.compare( wxT("Order Lexicographically")) ) {
								out += wxT(" & ");
							}
						}

						out = out.BeforeLast('&');
						out += wxT("\\\\ \\hline\n");
						wxFprintf(l_pstream1, wxT("%s"), out.c_str());
					}

					wxFprintf(l_pstream1, wxT("\\end{longtabu}\n") );
				} else {
					wxFprintf(l_pstream1, wxT("\\emph{No elements available for description}\n\n") );
				}
			}

			wxFprintf(l_pstream1, wxT("\\end{center}\n") );

		    wxFprintf(l_pstream, wxT("\\input{./") + EditStringforLatex( element ) + wxT(".tex}\n") );

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
SP_ExportLatex::WriteHierarchyTreeRecur(const wxTreeItemId& p_Id, FILE* l_pstream, int &p_nChildCount)
{
	if(!p_Id.IsOk() ) { return; }

	  wxTreeItemIdValue cookie= NULL;
	  wxTreeItemId l_nId = m_pcTree->GetFirstChild(p_Id, cookie);


	  while( l_nId.IsOk() )
	  {
		  p_nChildCount++;

		  int l_nChildCount = 0;
		  wxString level_label = m_pcTree->GetItemText( l_nId );

		  SP_LOGMESSAGE( wxT(" =================== ") + level_label );

		  wxString l_slinkLabel = EditStringforCrossRef( level_label.AfterFirst(' ') );
		  level_label = EditStringforLatex( level_label );
		  wxString out = wxT("\\hyperref[") + l_slinkLabel
								+ wxT("]{") + level_label +  wxT("} ");

		  out = wxT("child { node {") + out + wxT("}\n");
		  wxFprintf(l_pstream, wxT("%s"), out.c_str());

		  int l_nChildMissing = 0;

		  if( m_nflagIncludeSubtrees ) {
			  SP_LOGMESSAGE( level_label + wxT("is being sent to recursive call for Tree. Include = true.") );
			  WriteHierarchyTreeRecur(l_nId, l_pstream, l_nChildCount);
			  l_nChildMissing = m_pcTree->GetChildrenCount(l_nId);

		  } else {
			  if( m_pcTree->ItemHasChildren(l_nId) && m_pcTree->IsExpanded( l_nId) ) {
				  SP_LOGMESSAGE( level_label + wxT("is expanded. Include = false."));
				  WriteHierarchyTreeRecur(l_nId, l_pstream, l_nChildCount);
			  }
		  }

		  wxFprintf(l_pstream, wxT("}\n") );

		  for(int i = 0; i < l_nChildCount; i++) {
			  wxFprintf(l_pstream, wxT("child [missing] {}\n") );
		  }

		  l_nId = m_pcTree->GetNextChild(p_Id, cookie);

		  p_nChildCount += l_nChildCount;
	  };

}

bool
SP_ExportLatex::WriteHierarchyTree(FILE* l_pstream)
{
	const wxString latexHierarchyTree_file = m_sFilePath + wxT("HierarchyTree.tex");
	SP_LOGMESSAGE( latexHierarchyTree_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexHierarchyTree_file);

	FILE* l_pstream1 = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream1)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream1, wxT("\\newpage\n") );
	//wxFprintf(l_pstream1, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream1, wxT("\\subsection{Hierarchy Tree}\n\n") );
	wxFprintf(l_pstream1, wxT("\\emph{This section contains the hierarchical tree structure for selected levels.}\n\n") );

	int l_nSelections = m_pcTree->GetSelections(m_ArrayTreeItemIds);

	if( l_nSelections ) {

		wxTreeItemId local_tree;

		for(int i = 0; i < l_nSelections; i++) {
			local_tree = m_ArrayTreeItemIds[i];

			if( i > 0 && IsAncestorSelected(local_tree) ) {
				continue;
			} else {
				//make new tree
				wxFprintf(l_pstream1, wxT("\n\n\n") );

				wxString label = m_pcTree->GetItemText( local_tree );

				if(i) {
					wxFprintf(l_pstream1, wxT("\\newpage\n") );
				}

				wxString out = wxT("\\subsubsection*{") + label + wxT("}\n");
				wxFprintf(l_pstream1, wxT("%s"), out.c_str());

				out = wxT("\\addcontentsline{toc}{subsubsection}{") + label + wxT("}");
				wxFprintf(l_pstream1, wxT("%s"), out.c_str());

				//////////////////////////tree
				wxFprintf(l_pstream1, wxT("\\tikzstyle{every node}=[draw=black,thick,anchor=west]\n") );
				wxFprintf(l_pstream1, wxT("\\maxsizebox{\\linewidth}{.99\\textheight}{\n") );

				out = wxT("\\begin{tikzpicture}[");
				out += wxT("grow via three points={one child at (0.5,-0.8) and ");
				out += wxT("two children at (0.5,-0.8) and (0.5,-1.6)}, ");
				out += wxT("edge from parent path={(\\tikzparentnode.south) ");
				out += wxT("|- (\\tikzchildnode.west)}]\n\n");
				wxFprintf(l_pstream1, wxT("%s"), out.c_str());


				wxString l_slinkLabel = EditStringforCrossRef( label.AfterFirst(' ') );
				label = EditStringforLatex( label );
				out = wxT("\\hyperref[") + l_slinkLabel
									+ wxT("]{") + label +  wxT("} ");

				out = wxT("\\node{") + out + wxT("}\n");
				wxFprintf(l_pstream1, wxT("%s"), out.c_str());


				int l_nChildCount = 0;
				if( m_nflagIncludeSubtrees ) {
					WriteHierarchyTreeRecur( local_tree, l_pstream1, l_nChildCount);
				} else {   //if unchecked, include subtrees only if expanded
					if( m_pcTree->IsExpanded( local_tree ) ) {
						WriteHierarchyTreeRecur( local_tree, l_pstream1, l_nChildCount);
					}
				}

				wxFprintf(l_pstream1, wxT(";\n\\end{tikzpicture}\n"));
				wxFprintf(l_pstream1, wxT("}\n\n"));
				////////////////////////////
			}
		}

	} else {
		//only top level (no hierarchy)
		wxFprintf(l_pstream1, wxT("\\emph{No Hierarchial structure found.}\n") );
	}

	fclose( l_pstream1 );
	l_pstream1 = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(l_pstream, wxT("\n\\input{./HierarchyTree.tex}\n") );

	return true;
}

void
SP_ExportLatex::WriteHierarchyFigureRec(const wxTreeItemId& p_Id, FILE* l_pstream)
{
	if(!p_Id.IsOk() ) { return; }

	wxTreeItemIdValue cookie= NULL;
	wxTreeItemId l_nId = m_pcTree->GetFirstChild(p_Id, cookie);

	while( l_nId.IsOk() )
	{
		wxString level_label = m_pcTree->GetItemText( l_nId );

		//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...") + level_label );

		SP_LOGMESSAGE(wxT("<<<<<<<<<<<<<<<<<<<<<<<<<<<") + level_label);

		wxString label_name =  EditStringforLatex( level_label, false );

		wxString main_label = level_label.AfterFirst(' ');
		wxString level_id = SP_HierarchyLabel2ID[ main_label ];
		wxString l_slinkLabel =  EditStringforCrossRef( main_label );

		wxTreeItemId local_item = FindTreeItemRec(m_pcCoarseTreectrl->GetRootItem(), main_label);

		wxFprintf(l_pstream, wxT("\n\\newpage\n\\linkto{") + l_slinkLabel + wxT("}\n") );

		wxString out = wxT("\\subsubsection*{") + label_name  + wxT("}\n");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());

		out = wxT("\\addcontentsline{toc}{subsubsection}{") + label_name + wxT("}");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());

		label_name =  wxT("Level") + level_id + wxT("tex");

		wxString latexDC_file = m_sFilePath + label_name;

		wxPrintData *pd1 = new wxPrintData();
		pd1->SetFilename(latexDC_file);
		pd1->SetPaperId(wxPAPER_A2);
		pd1->SetOrientation(wxPORTRAIT);
		pd1->SetPrintMode(wxPRINT_MODE_FILE);

		SP_LatexDC latexDC(*pd1, false);
		latexDC.StartDoc(latexDC_file);

		#if wxABI_VERSION < 30000
		  latexDC.SetResolution(72);
		#endif

		//Get_Doc

		wxTreeItemData* data =  m_pcCoarseTreectrl->GetItemData( local_item );
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

		if( m_nflagIncludeSubtrees ) {
			SP_LOGMESSAGE( m_pcTree->GetItemText(l_nId) + wxT("is being sent to recursive call. Include = true"));
			WriteHierarchyFigureRec(l_nId, l_pstream);

		} else {      //if not checked, include subtrees only if expanded in treectrl
			if( m_pcTree->ItemHasChildren(l_nId) && m_pcTree->IsExpanded( l_nId ) ) {
				SP_LOGMESSAGE( m_pcTree->GetItemText(l_nId) + wxT("is expanded. Include = false"));
				WriteHierarchyFigureRec(l_nId, l_pstream);
			}
		}

		l_nId = m_pcTree->GetNextChild(p_Id, cookie);
	};

}

bool
SP_ExportLatex::WriteHierarchyFigure(FILE* l_pstream)
{
	const wxString latexHierarchyFigures_file = m_sFilePath + wxT("HierarchyFigures.tex");
	SP_LOGMESSAGE( latexHierarchyFigures_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexHierarchyFigures_file);

	FILE* l_pstream1 = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream1)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream1, wxT("\\newpage\n") );
	//wxFprintf(l_pstream1, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream1, wxT("\\subsection{Hierarchy Figures}\n\n") );
	wxFprintf(l_pstream1, wxT("\\emph{This section contains the hierarchical figures for selected levels.}\n\n") );


	int l_nSelections = m_pcTree->GetSelections(m_ArrayTreeItemIds);

	if( l_nSelections ) {

		wxTreeItemId local_tree;

		for(int i = 0; i < l_nSelections; i++) {
			local_tree = m_ArrayTreeItemIds[i];

			if(i > 0) {
				if( IsAncestorSelected(local_tree) ) {
					continue;
				}
			}

			wxString local_label = m_pcTree->GetItemText( local_tree );
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

			//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...") + local_label );

			wxString level_label = m_pcCoarseTreectrl->GetItemText( main_treeItem );
			wxString level_id = SP_HierarchyLabel2ID[ level_label ];
			wxString label_name =  EditStringforLatex( (level_id + wxT(" ") + level_label), false );
			wxString l_slinkLabel =  EditStringforCrossRef( level_label);

			wxFprintf(l_pstream1, wxT("\n\\newpage\n\\linkto{") + l_slinkLabel + wxT("}\n") );

			wxString out = wxT("\\subsubsection*{") + label_name  + wxT("}\n");
			wxFprintf(l_pstream1, wxT("%s"), out.c_str());

			out = wxT("\\addcontentsline{toc}{subsubsection}{") + label_name + wxT("}");
			wxFprintf(l_pstream1, wxT("%s"), out.c_str());

			label_name =  wxT("Level") + level_id + wxT("tex");

			wxString latexDC_file = m_sFilePath + label_name;

			wxPrintData *pd1 = new wxPrintData();
			pd1->SetFilename(latexDC_file);
			pd1->SetPaperId(wxPAPER_A2);
			pd1->SetOrientation(wxPORTRAIT);
			pd1->SetPrintMode(wxPRINT_MODE_FILE);

			SP_LatexDC latexDC(*pd1, false);
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
			wxFprintf(l_pstream1, wxT("%s"), out.c_str());


			if( m_nflagIncludeSubtrees ) {
				WriteHierarchyFigureRec( local_tree, l_pstream1);
			} else {      //if not checked, include subtrees only if expanded in treectrl
				if( m_pcTree->IsExpanded( local_tree ) ) {
					WriteHierarchyFigureRec( local_tree, l_pstream1);
				}
			}

		}

	} else {

		wxFprintf(l_pstream1, wxT("\\emph{No Hierarchy level available for description.}\n") );
	}
	/////////////////////////////////////

	fclose( l_pstream1 );
	l_pstream1 = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(l_pstream, wxT("\\input{./HierarchyFigures.tex}\n") );

	return true;
}

bool
SP_ExportLatex::IsAncestorSelected(const wxTreeItemId& tree_item)
{
	if( !tree_item.IsOk() ) { return false; }

	wxTreeItemId parent = m_pcTree->GetItemParent(tree_item);

	if( !parent.IsOk() ) { return false; }

	return ( m_pcTree->IsSelected(parent) || IsAncestorSelected(parent) );
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
	//wxFprintf(l_pstream, wxT("\\needspace{10\\baselineskip}\n\n") );

	wxFprintf(l_pstream, wxT("\\section{Hierarchy}\n") );
	wxFprintf(l_pstream, wxT("This section contains information about the net hierarchy.") );
	//////////////////////////////////

	//Include Subtrees
	if(m_pcCheckBoxIncludeSubtrees->IsChecked() ) {
		m_nflagIncludeSubtrees = 1;
	} else {
		m_nflagIncludeSubtrees = 0;
	}

	//Hierarchy Tree
	if(m_pcCheckBoxHierarchyTree->IsChecked() ) {

		//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Hierarchy Tree") );

		WriteHierarchyTree(l_pstream);
		wxFprintf(l_pstream, wxT("\\tikzstyle{every node}=[draw=none]\n") );
	}

	///////////////////////////////////
	//m_pcProgressDlg->Pulse( wxT("Exporting to Latex...Hierarchy Figures") );

	//Hierarchy Figures
	WriteHierarchyFigure(l_pstream);

	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./Hierarchy.tex}\n") );

	return true;
}

bool
SP_ExportLatex::WriteReferences()
{
	const wxString latexReferences_file = m_sFilePath + wxT("References.tex");
	SP_LOGMESSAGE( latexReferences_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexReferences_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\addcontentsline{toc}{section}{References}\n") );
	wxFprintf(l_pstream, wxT("\\begin{thebibliography}{0}\n\n") );

	//Two basic references

	wxFprintf(l_pstream, wxT("\\bibitem{heiner:pn:2012}\n") );
	wxString out = wxT("M. Heiner, M. Herajy, F. Liu, C. Rohr and M. Schwarick.\n");
	out += wxT("Snoopy a unifying Petri net tool. \n");
	out += wxT("Proc. PETRI NETS 2012, Volume 7347, Springer, June 2012, pages 398-407.\n\n");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	wxFprintf(l_pstream, wxT("\\bibitem{Rohr:bi:2010}\n") );
	out = wxT("C. Rohr, W. Marwan and M. Heiner.\n");
	out += wxT("Snoopy - a unifying Petri net framework to investigate biomolecular networks. \n");
	out += wxT("Bioinformatics, 26(7):974-975, 2010. \n\n");
	wxFprintf(l_pstream, wxT("%s"), out.c_str());

	//adding dynamic references
	map< pair<int, wxString>, wxString>::iterator it;
	for(it = SP_LatexReferencesIndex2Ref.begin(); it != SP_LatexReferencesIndex2Ref.end(); it++) {
		 out = wxT("\\bibitem{") + (*it).first.second + wxT("}\n");  //add label to cite
		 out += (*it).second;
		 wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	wxFprintf(l_pstream, wxT("\\end{thebibliography}\n\n") );


	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./References.tex}\n") );

	return true;
}

bool
SP_ExportLatex::WriteGlossary()
{
	const wxString latexGlossary_file = m_sFilePath + wxT("Glossary.tex");
	SP_LOGMESSAGE( latexGlossary_file );

	wxPrintData *pd = new wxPrintData();
	SetUpPrintData(*pd, latexGlossary_file);

	FILE* l_pstream = wxFopen( (*pd).GetFilename().c_str(), wxT("w+"));

	if (!l_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		return FALSE;
	}

	wxFprintf(l_pstream, wxT("\\newpage\n") );
	wxFprintf(l_pstream, wxT("\\addcontentsline{toc}{section}{Glossary}\n") );
	wxFprintf(l_pstream, wxT("\\section*{Glossary}\n\n") );

	wxFprintf(l_pstream, wxT("\\begin{description}\n\t") );
	wxFprintf(l_pstream, wxT("\\item[CROSS REFS.] Cross-References\n\t") );
	wxFprintf(l_pstream, wxT("\\item[LOG.] Logic\n\t") );
	wxFprintf(l_pstream, wxT("\\item[MAR.] Marking\n\t") );
	wxFprintf(l_pstream, wxT("\\item[MUL.] Multiplicity\n\t") );
	wxFprintf(l_pstream, wxT("\\item[NET.] Net number\n\t") );

	wxString out;
	map< wxString, wxString > :: iterator it;
	it = SP_LatexGlossaryAbbr2Full.begin();

	for(; it != SP_LatexGlossaryAbbr2Full.end(); it++) {
		out = wxT("\\item[") + it->first + wxT("] ") + it->second + wxT("\n\t");
		wxFprintf(l_pstream, wxT("%s"), out.c_str());
	}

	wxFprintf(l_pstream, wxT("\\end{description}\n") );

	fclose( l_pstream );
	l_pstream = (FILE *) NULL;
	wxDELETE(pd);

	wxFprintf(m_pstream, wxT("\\input{./Glossary.tex}\n") );

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
	//This function appends backslash to & in strings and remove space for inserting in latex code

	for(int i = 0; i < filename.length(); i++) {

		while(remove_space && filename[i] == ' ') {
			filename.Remove(i, 1);
		};

		//other special characters
		if(filename[i] == '&' || filename[i] == '%' || filename[i] == '$' ||
				filename[i] == '#' || filename[i] == '{' || filename[i] == '}' ||
				filename[i] == '~' || filename[i] == '^' || filename[i] == '\\' ||
				filename[i] == '_') { //underscore added later for words with multiple underscore and digit
			filename = filename.SubString(0, i-1) + wxT("\\") + filename.SubString(i, filename.Length()-1);
			i+=2;
		}
	}

	return filename;
}

wxString
SP_ExportLatex::EditStringforCrossRef(wxString filename)
{
	for(int i = 0; i < filename.length(); i++) {
		if((filename[i] >= 'a' && filename[i] <= 'z') ||
				(filename[i] >= 'A' && filename[i] <= 'Z') ||
				(filename[i] >= '0' && filename[i] <= '9') ) {
		} else {
			filename[i] = ':';
		}
	}

	return filename;
}


void
SP_ExportLatex::OnSelectLatexCompiler_Pdflatex(wxCommandEvent& p_cEvent)
{

#ifdef __APPLE__
	SP_LOGMESSAGE("##### Mac OS detected #####");
	m_sCompilerPath = wxT("/usr/texbin/pdflatex");

#elif __linux
	  SP_LOGMESSAGE("##### Linux OS detected #####");
	  m_sCompilerPath = wxT("/usr/bin/pdflatex");

#elif _WIN32
	  SP_LOGMESSAGE("##### Windows OS detected #####");
	  m_sCompilerPath = wxT("/usr/bin/pdflatex");

#endif

	  l_pcFilePickerCtrl1->SetPath( m_sCompilerPath );
}

void
SP_ExportLatex::OnSelectLatexCompiler_Latexmk(wxCommandEvent& p_cEvent)
{

#ifdef __APPLE__
	SP_LOGMESSAGE("##### Mac OS detected #####");
	m_sCompilerPath = wxT("/usr/texbin/latexmk");

#elif __linux
	  SP_LOGMESSAGE("##### Linux OS detected #####");
	  m_sCompilerPath = wxT("/usr/bin/latexmk");

#elif _WIN32
	  SP_LOGMESSAGE("##### Windows OS detected #####");
	  m_sCompilerPath = wxT("/usr/bin/latexmk");

#endif

	l_pcFilePickerCtrl1->SetPath( m_sCompilerPath );
}
