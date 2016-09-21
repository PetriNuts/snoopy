//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/6/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include <wx/file.h>
#include <wx/textfile.h>
#include "sp_ds/extensions/Col_PN/Transform/SP_CPNToolXmlWriter.h"
#include "sp_utilities.h"

#include "sp_revision.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Metadata.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


SP_CPNToolXmlWriter::SP_CPNToolXmlWriter():
m_pcGraph(NULL)
{
	m_sInstanceID = wxT("id0");
	m_sPageID = wxT("id1");
	m_nIDCount = 100; // 0-99 reserved ID for other possible special cases	
}

SP_CPNToolXmlWriter::~SP_CPNToolXmlWriter()
{

}

bool
SP_CPNToolXmlWriter::Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_BOOL((!p_sFile.IsEmpty()), return FALSE);

    m_pcGraph = p_pcGraph;

	if( ! m_ValueAssign.InitializeColorset(m_ColorSetClass) )
		return false;

    wxXmlDocument* l_pcDoc = new wxXmlDocument();
	l_pcDoc->SetFileEncoding(wxT("iso-8859-1") );


	wxXmlNode* l_pcRootElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("workspaceElements"));

	wxXmlNode*  l_pcGenerator = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("generator"));
    l_pcGenerator->AddAttribute(wxT("tool"), wxT("CPN Tools"));
	l_pcGenerator->AddAttribute(wxT("version"), wxT("2.2.0"));
	l_pcGenerator->AddAttribute(wxT("format"), wxT("6"));
    l_pcRootElem->AddChild(l_pcGenerator);

	wxXmlNode*  l_pcCPNet = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("cpnet"));
	l_pcRootElem->AddChild(l_pcCPNet);

    l_pcDoc->SetRoot(l_pcRootElem);

	WriteCPNet(p_pcGraph, l_pcCPNet);


	if(!l_pcDoc->Save(p_sFile))
    {
        m_sMessage << wxT("An error occurred creating the document") << wxT("\n");
        m_bError = TRUE;
    }
    wxDELETE(l_pcDoc);

    //TODO: workaround for wxXML_PI_NODE
    if(!m_bError)
    {
		wxTextFile l_File(p_sFile);
		if(l_File.Open())
		{			
			wxString l_sProcessingInstruction = wxT("<!DOCTYPE workspaceElements PUBLIC \"-//CPN//DTD CPNXML 1.0//EN\" \"http://www.daimi.au.dk/~cpntools/bin/DTD/2/cpn.dtd\">");
			l_File.InsertLine(l_sProcessingInstruction, 1);
			l_File.Write();
		}
    }

    m_pcGraph = NULL;
    return !m_bError;
}


bool
SP_CPNToolXmlWriter::WriteCPNet(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode* l_pcElemDL = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("globbox"));
    p_pcRoot->AddChild(l_pcElemDL);

	if( ! WriteDeclaration(p_pcVal, l_pcElemDL) )
		return false;

	wxXmlNode* l_pcElemPage = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("page"));
	l_pcElemPage->AddAttribute(wxT("id"), m_sPageID);
    p_pcRoot->AddChild(l_pcElemPage);

	if( ! WritePage(p_pcVal, l_pcElemPage) )
		return false;

	//write the size of the net, so that it can be correctly shown.

	if( ! WriteNetSize(p_pcVal, p_pcRoot) ) 
		return false;

	return true;
}

bool
SP_CPNToolXmlWriter::WriteDeclaration(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	//Write color sets
	vector<SP_CPN_ColorSet*>* l_pvColorSet = m_ColorSetClass.GetColorSetVector();
	CHECK_POINTER(l_pvColorSet, return FALSE);

	for(unsigned i = 0; i < l_pvColorSet->size(); i++)
	{
		SP_CPN_ColorSet* l_pcColorSet = (*l_pvColorSet)[i];
		WriteDeclColorSet(l_pcColorSet, p_pcRoot);
	}	

	//write variables	
	map<wxString, SP_CPN_Variable_Constant>* l_pmVariable= m_ColorSetClass.GetVariableMap();
	CHECK_POINTER(l_pmVariable, return FALSE);

	map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	for( itMap = l_pmVariable->begin(); itMap != l_pmVariable->end(); itMap++)
	{
		SP_CPN_Variable_Constant l_cVaribale = (*itMap).second;
		WriteDelcVariable( (*itMap).first, l_cVaribale, p_pcRoot );
	}


	//write constants 
	map<wxString, SP_CPN_Variable_Constant>* l_pmConstant= m_ColorSetClass.GetConstantMap();
	CHECK_POINTER(l_pmConstant, return FALSE);

	//map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	for( itMap = l_pmConstant->begin(); itMap != l_pmConstant->end(); itMap++)
	{
		SP_CPN_Variable_Constant l_cVaribale = (*itMap).second;
		WriteDeclConstant( (*itMap).first, l_cVaribale, p_pcRoot );
	}

	//write functions 
	map<wxString, SP_CPN_Function>* l_pmFunction = m_ColorSetClass.GetFunctionMap();
	CHECK_POINTER(l_pmFunction, return FALSE);
	map<wxString, SP_CPN_Function>::iterator itFuncMap;

	//map<wxString, SP_CPN_Variable_Constant>::iterator itMap;
	for( itFuncMap = l_pmFunction->begin(); itFuncMap != l_pmFunction->end(); itFuncMap++)
	{
		SP_CPN_Function l_cFunc = (*itFuncMap).second;
		WriteDeclFunction( (*itFuncMap).first, l_cFunc, p_pcRoot );
	}

	return true;
}

bool
SP_CPNToolXmlWriter::WritePage(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode* l_pcElemPageAttr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("pageattr"));
	l_pcElemPageAttr->AddAttribute(wxT("name"), wxT("Top"));
    p_pcRoot->AddChild(l_pcElemPageAttr);

	SP_DS_Nodeclass* p_pcTransitionNodeClass = p_pcVal->GetNodeclass(wxT("Transition"));
	CHECK_POINTER(p_pcTransitionNodeClass, return FALSE);	
	SP_ListNode::const_iterator l_Iter;

	const SP_ListNode* l_plElements = p_pcTransitionNodeClass->GetElements();
	CHECK_POINTER(l_plElements, return FALSE);

	for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
	{
		WriteTransitionNode((*l_Iter), p_pcRoot);
	}

	SP_DS_Nodeclass* p_pcPlaceNodeClass = p_pcVal->GetNodeclass(wxT("Place"));
	CHECK_POINTER(p_pcPlaceNodeClass, return FALSE);

	const SP_ListNode* l_plPlaceElements = p_pcPlaceNodeClass->GetElements();
	CHECK_POINTER(l_plPlaceElements, return FALSE);

	for (l_Iter = l_plPlaceElements->begin(); l_Iter != l_plPlaceElements->end(); ++l_Iter)
	{
		WritePlaceNode((*l_Iter), p_pcRoot);
	}

	SP_ListEdge::const_iterator l_IterEdge;
	SP_DS_Edgeclass* p_pcEdgeNodeClass = p_pcVal->GetEdgeclass(wxT("Edge"));
	CHECK_POINTER(p_pcEdgeNodeClass, return FALSE);

	const SP_ListEdge* l_plEdgeElements = p_pcEdgeNodeClass->GetElements();
	CHECK_POINTER(l_plEdgeElements, return FALSE);

	for (l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge)
	{
		WriteNormalEdge((*l_IterEdge), p_pcRoot);
	}

	p_pcEdgeNodeClass = p_pcVal->GetEdgeclass(wxT("Read Edge"));
	CHECK_POINTER(p_pcEdgeNodeClass, return FALSE);

	l_plEdgeElements = p_pcEdgeNodeClass->GetElements();
	CHECK_POINTER(l_plEdgeElements, return FALSE);

	for (l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge)
	{
		WriteNormalEdge((*l_IterEdge), p_pcRoot);
	}

	p_pcEdgeNodeClass = p_pcVal->GetEdgeclass(wxT("Inhibitor Edge"));
	CHECK_POINTER(p_pcEdgeNodeClass, return FALSE);

	l_plEdgeElements = p_pcEdgeNodeClass->GetElements();
	CHECK_POINTER(l_plEdgeElements, return FALSE);

	for (l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge)
	{
		WriteNormalEdge((*l_IterEdge), p_pcRoot);
	}

	p_pcEdgeNodeClass = p_pcVal->GetEdgeclass(wxT("Equal Edge"));
	CHECK_POINTER(p_pcEdgeNodeClass, return FALSE);

	l_plEdgeElements = p_pcEdgeNodeClass->GetElements();
	CHECK_POINTER(l_plEdgeElements, return FALSE);

	for (l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge)
	{
		WriteNormalEdge((*l_IterEdge), p_pcRoot);
	}

	p_pcEdgeNodeClass = p_pcVal->GetEdgeclass(wxT("Reset Edge"));
	CHECK_POINTER(p_pcEdgeNodeClass, return FALSE);

	l_plEdgeElements = p_pcEdgeNodeClass->GetElements();
	CHECK_POINTER(l_plEdgeElements, return FALSE);

	for (l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge)
	{
		WriteNormalEdge((*l_IterEdge), p_pcRoot);
	}




	return true;
}

bool SP_CPNToolXmlWriter::WriteNetSize(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxString l_sNetClassName = p_pcVal->GetNetclass()->GetName();
	int l_nSize = wxGetApp().GetCanvasPrefs()->GetGridSize( l_sNetClassName );
	wxString l_sSize = wxString::Format(wxT("%d"),l_nSize);

	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("instances"));	
    p_pcRoot->AddChild(l_pcElem);

	wxXmlNode* l_pcEleminstance = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("instance"));
	l_pcEleminstance->AddAttribute(wxT("id"), m_sInstanceID );   //
	l_pcEleminstance->AddAttribute(wxT("page"), m_sPageID   );
    l_pcElem->AddChild(l_pcEleminstance);


	//
	wxXmlNode* l_pcbinders = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("binders"));	
    p_pcRoot->AddChild(l_pcbinders);

	wxXmlNode* l_pccpnbinder = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("cpnbinder"));	
	l_pccpnbinder->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++));
	l_pccpnbinder->AddAttribute(wxT("x"), wxT("257"));
	l_pccpnbinder->AddAttribute(wxT("y"), wxT("122"));
	l_pccpnbinder->AddAttribute(wxT("width"), l_sSize );
	l_pccpnbinder->AddAttribute(wxT("height"), l_sSize );
    l_pcbinders->AddChild(l_pccpnbinder);


	wxXmlNode* l_pcsheets = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("sheets"));	
    l_pccpnbinder->AddChild(l_pcsheets);

	wxXmlNode* l_pccpnsheet = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("cpnsheet"));	
	l_pccpnsheet->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++) );
	l_pccpnsheet->AddAttribute(wxT("panx"), wxT("-6.000000") );
	l_pccpnsheet->AddAttribute(wxT("pany"), wxT("-5.000000") );
	l_pccpnsheet->AddAttribute(wxT("zoom"), wxT("1.000000") );
	l_pccpnsheet->AddAttribute(wxT("instance"), m_sInstanceID );
    l_pcsheets->AddChild(l_pccpnsheet);

	wxXmlNode* l_pczorder = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("zorder"));	
    l_pccpnsheet->AddChild(l_pczorder);

	wxXmlNode* l_pcposition = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("position"));	
	l_pcposition->AddAttribute(wxT("value"), wxT("0") );
    l_pczorder->AddChild(l_pcposition);


	wxXmlNode* l_pczorder1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("zorder"));	
    l_pccpnbinder->AddChild(l_pczorder1);

	wxXmlNode* l_pcposition1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("position"));	
	l_pcposition1->AddAttribute(wxT("value"), wxT("0") );
    l_pczorder1->AddChild(l_pcposition1);



	return true;
}


bool
SP_CPNToolXmlWriter::WriteDeclColorSet(SP_CPN_ColorSet* p_pcColorSet, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcColorSet, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);	

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("color"));
	l_pcElem->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++) );
	p_pcRoot->AddChild(l_pcElem);

	SP_CPN_DATATYPE l_DataType = p_pcColorSet->GetDataType();

	wxXmlNode*  l_pcElemID = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
	l_pcElem->AddChild(l_pcElemID);

	wxXmlNode*  l_pcElemTextID = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetName());
	l_pcElemID->AddChild(l_pcElemTextID);

	if(l_DataType == CPN_INTEGER)
	{
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("int"));		
		l_pcElem->AddChild(l_pcElemType);

		wxXmlNode*  l_pcElemWith = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("with"));		
		l_pcElemType->AddChild(l_pcElemWith);

		wxXmlNode*  l_pcElemML1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemWith->AddChild(l_pcElemML1);

		wxXmlNode*  l_pcElemTextValue1 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetStringValue()[0]);
		l_pcElemML1->AddChild(l_pcElemTextValue1);

		wxXmlNode*  l_pcElemML2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemWith->AddChild(l_pcElemML2);

		wxXmlNode*  l_pcElemTextValue2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetStringValue()[p_pcColorSet->GetStringValue().size()-1]);
		l_pcElemML2->AddChild(l_pcElemTextValue2);
	}

	else if(l_DataType == CPN_ENUM || l_DataType == CPN_DOT)
	{
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("enum"));		
		l_pcElem->AddChild(l_pcElemType);

		for(unsigned i = 0; i < p_pcColorSet->GetStringValue().size(); i++)
		{
			wxXmlNode*  l_pcElemEnumID = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
			l_pcElemType->AddChild(l_pcElemEnumID);

			wxXmlNode*  l_pcElemTextValue = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetStringValue()[i]);
			l_pcElemEnumID->AddChild(l_pcElemTextValue);
		}
	}

	else if(l_DataType == CPN_STRING)
	{
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("string"));		
		l_pcElem->AddChild(l_pcElemType);

		wxXmlNode*  l_pcElemWith = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("with"));		
		l_pcElemType->AddChild(l_pcElemWith);

		wxXmlNode*  l_pcElemML1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemWith->AddChild(l_pcElemML1);

		wxXmlNode*  l_pcElemTextValue1 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetStringValue()[0]);
		l_pcElemML1->AddChild(l_pcElemTextValue1);

		wxXmlNode*  l_pcElemML2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemWith->AddChild(l_pcElemML2);

		wxXmlNode*  l_pcElemTextValue2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetStringValue()[p_pcColorSet->GetStringValue().size()-1]);
		l_pcElemML2->AddChild(l_pcElemTextValue2);
	
	}

	else if(l_DataType == CPN_INDEX)
	{
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("index"));		
		l_pcElem->AddChild(l_pcElemType);	

		wxXmlNode*  l_pcElemML1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemType->AddChild(l_pcElemML1);

		wxString l_sColorLow = p_pcColorSet->GetStringValue()[0];
		l_sColorLow = l_sColorLow.AfterFirst(wxT('['));
		l_sColorLow = l_sColorLow.BeforeFirst(wxT(']'));

		wxXmlNode*  l_pcElemTextValue1 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), l_sColorLow);
		l_pcElemML1->AddChild(l_pcElemTextValue1);

		wxXmlNode*  l_pcElemML2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));		
		l_pcElemType->AddChild(l_pcElemML2);

		l_sColorLow = p_pcColorSet->GetStringValue()[p_pcColorSet->GetStringValue().size()-1];
		l_sColorLow = l_sColorLow.AfterFirst(wxT('['));
		l_sColorLow = l_sColorLow.BeforeFirst(wxT(']'));

		wxXmlNode*  l_pcElemTextValue2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), l_sColorLow);
		l_pcElemML2->AddChild(l_pcElemTextValue2);
	
	}

	else if(l_DataType == CPN_PRODUCT)
	{
	
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("product"));		
		l_pcElem->AddChild(l_pcElemType);

		for(unsigned i = 0; i < p_pcColorSet->GetComponentName().size(); i++)
		{
			wxXmlNode*  l_pcElemComID = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
			l_pcElemType->AddChild(l_pcElemComID);

			wxXmlNode*  l_pcElemTextValue = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_pcColorSet->GetComponentName()[i]);
			l_pcElemComID->AddChild(l_pcElemTextValue);
		}
	}

	else if(l_DataType == CPN_UNION)
	{
		wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("union"));		
		l_pcElem->AddChild(l_pcElemType);

		for(unsigned i = 0; i < p_pcColorSet->GetComponentName().size(); i++)
		{
			wxXmlNode*  l_pcElemUnionField = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("unionfield"));		
			l_pcElemType->AddChild(l_pcElemUnionField);

			wxXmlNode*  l_pcElemComID = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
			l_pcElemUnionField->AddChild(l_pcElemComID);

			wxXmlNode*  l_pcElemTextValue = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxT("U") + p_pcColorSet->GetComponentName()[i]);
			l_pcElemComID->AddChild(l_pcElemTextValue);

			wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("type"));		
			l_pcElemUnionField->AddChild(l_pcElemType);

			wxXmlNode*  l_pcElemComID2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
			l_pcElemType->AddChild(l_pcElemComID2);

			wxXmlNode*  l_pcElemTextValue2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), p_pcColorSet->GetComponentName()[i]);
			l_pcElemComID2->AddChild(l_pcElemTextValue2);			
		}
	
	}
	else
	{
		//do nothing
	}

	return true;
}




bool
SP_CPNToolXmlWriter::WriteDelcVariable( wxString p_sVariableName, SP_CPN_Variable_Constant p_cVaribale, wxXmlNode* p_pcRoot )
{
	
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("var"));
	l_pcElem->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++) );
	p_pcRoot->AddChild(l_pcElem);

	wxXmlNode*  l_pcElemType = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("type"));		
	l_pcElem->AddChild(l_pcElemType);

	wxXmlNode*  l_pcElemID = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
	l_pcElemType->AddChild(l_pcElemID);

	wxXmlNode*  l_pcElemText = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_cVaribale.m_ColorSet);
	l_pcElemID->AddChild(l_pcElemText);



	wxXmlNode*  l_pcElemID2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("id"));		
	l_pcElem->AddChild(l_pcElemID2);

	wxXmlNode*  l_pcElemTextID = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),p_sVariableName);
	l_pcElemID2->AddChild(l_pcElemTextID);

	return true;
}


bool
SP_CPNToolXmlWriter::WriteDeclConstant( wxString p_sVariableName, SP_CPN_Variable_Constant p_cVaribale, wxXmlNode* p_pcRoot )
{    
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));
	l_pcElem->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++) );
	p_pcRoot->AddChild(l_pcElem);

	SP_CPN_DATATYPE l_DataType = p_cVaribale.m_DataType;

	wxString l_sValue = wxT("");
	if(l_DataType == CPN_INTEGER)
	{
		l_sValue = wxString::Format(wxT("%d"),p_cVaribale.m_IntegerValue);
	}
	else
	{
		l_sValue = p_cVaribale.m_StringValue;
	}

	wxXmlNode*  l_pcElemText = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxT("val ") + p_sVariableName + wxT(" = ") + l_sValue);
	l_pcElem->AddChild(l_pcElemText);

    return true;
}


bool
SP_CPNToolXmlWriter::WriteDeclFunction( wxString p_sFunctionName, SP_CPN_Function p_cFunction, wxXmlNode* p_pcRoot )
{   
	CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ml"));
	l_pcElem->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++) );
	p_pcRoot->AddChild(l_pcElem);

	map<wxString,SP_CPN_Parameter> m_ParameterMap = p_cFunction.m_ParameterMap;
	map<wxString,SP_CPN_Parameter>::iterator itMap;
	wxString l_sParameterlist = wxT("");
 	for(itMap = m_ParameterMap.begin(); itMap != m_ParameterMap.end(); itMap++)
	{
		l_sParameterlist += itMap->first + wxT(",");
	}
	l_sParameterlist = l_sParameterlist.BeforeLast(wxT(','));

	wxXmlNode*  l_pcElemText = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""), wxT("fun ") + 
																	p_cFunction.m_sFunctionName + wxT("( ") + l_sParameterlist + wxT(" ) = ")
																	+ p_cFunction.m_sFunctionBody + wxT(";"));
	l_pcElem->AddChild(l_pcElemText);

    return true;
}


bool
SP_CPNToolXmlWriter::WriteTransitionNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	wxString l_stransName =p_pcVal->GetAttribute( wxT("Name") )->GetValueString();
	wxString l_sID =p_pcVal->GetAttribute( wxT("ID") )->GetValueString();
	long l_nID;
	if( ! l_sID.ToLong(&l_nID))
		return false;

	int l_nCPNID = m_nIDCount++ ;
	wxString l_sCPNID = wxT("id")+wxString::Format(wxT("%d"), l_nCPNID );
	m_mNodeIDMap[wxT("Transition")+l_sID] = l_sCPNID ;

	SP_Graphic* l_pcNewGr =  *(p_pcVal->GetAttribute( wxT("ID") )->GetGraphics()->begin());
	double l_nNodePosX = l_pcNewGr->GetPosX();
	double l_nNodePosY = - l_pcNewGr->GetPosY();


	//to get the guard
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcVal->GetAttribute(SP_DS_CPN_GUARDLIST) );
	CHECK_POINTER(l_pcColList, return FALSE);
	l_pcNewGr =  *(l_pcColList->GetGraphics()->begin() );
	double l_nGuardPosX = l_pcNewGr->GetPosX();
	double l_nGuardPosY = - l_pcNewGr->GetPosY();
	wxString l_sGuard = wxT("");
	if(l_pcColList->GetRowCount() > 0)
		l_sGuard = l_pcColList->GetCell(0, 1);	
	//end

	l_sGuard = ProcessExpression( l_sGuard );
	
	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("trans"));
	l_pcElem->AddAttribute(wxT("id"), l_sCPNID );
	p_pcRoot->AddChild(l_pcElem);
	
	wxXmlNode*  l_pcPosattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nNodePosX) );
	l_pcPosattr->AddAttribute(wxT("y"),  wxString::Format(wxT("%f"),l_nNodePosY) );
	l_pcElem->AddChild(l_pcPosattr);

	wxXmlNode*  l_pcfillattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr->AddAttribute(wxT("colour"), wxT("White") );
	l_pcfillattr->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr->AddAttribute(wxT("filled"),  wxT("false") );
	l_pcElem->AddChild(l_pcfillattr);

	wxXmlNode*  l_pclineattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pclineattr->AddAttribute(wxT("thick"),  wxT("1") );
	l_pclineattr->AddAttribute(wxT("type"),  wxT("solid") );
	l_pcElem->AddChild(l_pclineattr);

	wxXmlNode*  l_pctextattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pctextattr->AddAttribute(wxT("bold"),  wxT("false") );
	l_pcElem->AddChild(l_pctextattr);

	wxXmlNode*  l_pctext = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
	l_pcElem->AddChild(l_pctext);
	wxXmlNode*  l_pcElemText = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_stransName );
	l_pctext->AddChild(l_pcElemText);

	wxXmlNode*  l_pcbox = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("box"));	
	l_pcbox->AddAttribute(wxT("w"), wxT("60.000000") );
	l_pcbox->AddAttribute(wxT("h"),  wxT("40.000000") );
	l_pcElem->AddChild(l_pcbox);


	//guard
	wxXmlNode*  l_pcCond = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("cond"));	
	l_pcCond->AddAttribute(wxT("id"), wxT("id")+ wxString::Format(wxT("%d"),m_nIDCount++) );
	l_pcElem->AddChild(l_pcCond);

	wxXmlNode*  l_pcPosattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr3->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nGuardPosX) );
	l_pcPosattr3->AddAttribute(wxT("y"), wxString::Format(wxT("%f"),l_nGuardPosY) );
	l_pcCond->AddChild(l_pcPosattr3);

	wxXmlNode*  l_pcfillattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr3->AddAttribute(wxT("colour"), wxT("White") );
	l_pcfillattr3->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr3->AddAttribute(wxT("filled"),  wxT("false") );
	l_pcCond->AddChild(l_pcfillattr3);

	wxXmlNode*  l_pclineattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr3->AddAttribute(wxT("colour"), wxT("Black") );
	l_pclineattr3->AddAttribute(wxT("thick"),  wxT("0") );
	l_pclineattr3->AddAttribute(wxT("type"),  wxT("solid") );
	l_pcCond->AddChild(l_pclineattr3);

	wxXmlNode*  l_pctextattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr3->AddAttribute(wxT("colour"), wxT("Black") );
	l_pctextattr3->AddAttribute(wxT("bold"),  wxT("false") );
	l_pcCond->AddChild(l_pctextattr3);

	wxXmlNode*  l_pctext3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
	l_pctext3->AddAttribute(wxT("tool"), wxT("CPN Tools") );
	l_pctext3->AddAttribute(wxT("version"),  wxT("2.2.0") );
	l_pcCond->AddChild(l_pctext3);

	wxXmlNode*  l_pcElemText3 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_sGuard );
	l_pctext3->AddChild(l_pcElemText3);


    return true;
}

bool
SP_CPNToolXmlWriter::WritePlaceNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

	//to get the place information
	wxString l_sPlaceName =p_pcVal->GetAttribute( wxT("Name") )->GetValueString();
	wxString l_sID =p_pcVal->GetAttribute( wxT("ID") )->GetValueString();
	long l_nID;
	if( ! l_sID.ToLong(&l_nID))
		return false;

	int l_nCPNID = m_nIDCount++ ;
	wxString l_sCPNID = wxT("id")+wxString::Format(wxT("%d"), l_nCPNID );
	m_mNodeIDMap[wxT("Place")+l_sID] = l_sCPNID ;

	SP_Graphic* l_pcNewGr =  *(p_pcVal->GetAttribute( wxT("ID") )->GetGraphics()->begin());
	double l_nNodePosX = l_pcNewGr->GetPosX();
	double l_nNodePosY = - l_pcNewGr->GetPosY();

	l_pcNewGr =  *(p_pcVal->GetAttribute( wxT("Colorset") )->GetGraphics()->begin());
	double l_nColorsetPosX = l_pcNewGr->GetPosX();
	double l_nColorsetPosY = - l_pcNewGr->GetPosY();


	l_pcNewGr =  *(p_pcVal->GetAttribute( wxT("MarkingList") )->GetGraphics()->begin());
	double l_nMarkingPosX = l_pcNewGr->GetPosX();
	double l_nMarkingPosY = - l_pcNewGr->GetPosY();

	SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcVal->GetAttribute(SP_DS_CPN_COLORSETNAME) );
	wxString l_sColorset = l_pcNameAttibute->GetValue();

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcVal->GetAttribute(SP_DS_CPN_MARKINGLIST) );

	int l_nActiveColumn = l_pcColList->GetActiveColumn(); 

	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if( !l_cSyntaxChecking.Initialize())
			return false;


	//////////////////////////////////////////////
	map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
	if( ! l_cSyntaxChecking.ComputeInitialMarking(p_pcVal,l_mColorToMarkingMap,false) )
		return false;

	wxString l_sMarking = wxT("");
	bool l_bFirst = true;
	map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;						
	for(itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
	{		
		wxString l_sColor = itMap->first;
		int l_nMulti = itMap->second[l_nActiveColumn].m_intMultiplicity;
		if (l_bFirst) 
		{
			l_sMarking = wxString::Format(wxT("%d"),l_nMulti) + wxT("`") + l_sColor;
			l_bFirst = false;
		}
		else
		{
			l_sMarking = l_sMarking + wxT("++") + wxString::Format(wxT("%d"),l_nMulti) + wxT("`") + l_sColor;	
		}		
	}


	

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("place"));
	l_pcElem->AddAttribute(wxT("id"), l_sCPNID );
	p_pcRoot->AddChild(l_pcElem);
	
	wxXmlNode*  l_pcPosattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nNodePosX) );
	l_pcPosattr->AddAttribute(wxT("y"),  wxString::Format(wxT("%f"),l_nNodePosY) );
	l_pcElem->AddChild(l_pcPosattr);

	wxXmlNode*  l_pcfillattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr->AddAttribute(wxT("colour"), wxT("White") );
	l_pcfillattr->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr->AddAttribute(wxT("filled"),  wxT("false") );
	l_pcElem->AddChild(l_pcfillattr);

	wxXmlNode*  l_pclineattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pclineattr->AddAttribute(wxT("thick"),  wxT("1") );
	l_pclineattr->AddAttribute(wxT("type"),  wxT("solid") );
	l_pcElem->AddChild(l_pclineattr);

	wxXmlNode*  l_pctextattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pctextattr->AddAttribute(wxT("bold"),  wxT("false") );
	l_pcElem->AddChild(l_pctextattr);

	wxXmlNode*  l_pctext = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
	l_pcElem->AddChild(l_pctext);
	wxXmlNode*  l_pcElemText = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_sPlaceName );
	l_pctext->AddChild(l_pcElemText);

	wxXmlNode*  l_pcellipse = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ellipse"));	
	l_pcellipse->AddAttribute(wxT("w"), wxT("60") );
	l_pcellipse->AddAttribute(wxT("h"),  wxT("40") );
	l_pcElem->AddChild(l_pcellipse);

	wxXmlNode*  l_pctoken = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("token"));	
	l_pctoken->AddAttribute(wxT("x"), wxT("-10.000000") );
	l_pctoken->AddAttribute(wxT("y"),  wxT("0.000000") );
	l_pcElem->AddChild(l_pctoken);

	wxXmlNode*  l_pcmarking = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("marking"));	
	l_pcmarking->AddAttribute(wxT("x"), wxT("-1.413428") );
	l_pcmarking->AddAttribute(wxT("y"),  wxT("4.9") );
	l_pcElem->AddChild(l_pcmarking);


	////
	wxXmlNode*  l_pctype = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("type"));	
	l_pctype->AddAttribute(wxT("id"), wxT("id")+ wxString::Format(wxT("%d"),m_nIDCount++) );	
	l_pcElem->AddChild(l_pctype);


	wxXmlNode*  l_pcPosattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr2->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nColorsetPosX) );
	l_pcPosattr2->AddAttribute(wxT("y"), wxString::Format(wxT("%f"),l_nColorsetPosY) );
	l_pctype->AddChild(l_pcPosattr2);

	wxXmlNode*  l_pcfillattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr2->AddAttribute(wxT("colour"), wxT("White") );
	l_pcfillattr2->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr2->AddAttribute(wxT("filled"),  wxT("false") );
	l_pctype->AddChild(l_pcfillattr2);

	wxXmlNode*  l_pclineattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr2->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pclineattr2->AddAttribute(wxT("thick"),  wxT("1") );
	l_pclineattr2->AddAttribute(wxT("type"),  wxT("solid") );
	l_pctype->AddChild(l_pclineattr2);

	wxXmlNode*  l_pctextattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr2->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pctextattr2->AddAttribute(wxT("bold"),  wxT("false") );
	l_pctype->AddChild(l_pctextattr2);

	wxXmlNode*  l_pctext2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
	l_pctext2->AddAttribute(wxT("tool"), wxT("CPN Tools") );
	l_pctext2->AddAttribute(wxT("version"),  wxT("2.2.0") );
	l_pctype->AddChild(l_pctext2);
	wxXmlNode*  l_pcElemText2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_sColorset );
	l_pctext2->AddChild(l_pcElemText2);


	/////////////////

	if ( l_sMarking != wxT("")) 
	{
		wxXmlNode*  l_pcMarking = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("initmark"));	
		l_pcMarking->AddAttribute(wxT("id"), wxT("id")+ wxString::Format(wxT("%d"),m_nIDCount++) );	
		l_pcElem->AddChild(l_pcMarking);


		wxXmlNode*  l_pcPosattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
		l_pcPosattr3->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nMarkingPosX) );
		l_pcPosattr3->AddAttribute(wxT("y"), wxString::Format(wxT("%f"),l_nMarkingPosY) );
		l_pcMarking->AddChild(l_pcPosattr3);

		wxXmlNode*  l_pcfillattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
		l_pcfillattr3->AddAttribute(wxT("colour"), wxT("White") );
		l_pcfillattr3->AddAttribute(wxT("pattern"),  wxT("solid") );
		l_pcfillattr3->AddAttribute(wxT("filled"),  wxT("false") );
		l_pcMarking->AddChild(l_pcfillattr3);

		wxXmlNode*  l_pclineattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
		l_pclineattr3->AddAttribute(wxT("colour"), wxT("Navy") );
		l_pclineattr3->AddAttribute(wxT("thick"),  wxT("1") );
		l_pclineattr3->AddAttribute(wxT("type"),  wxT("solid") );
		l_pcMarking->AddChild(l_pclineattr3);

		wxXmlNode*  l_pctextattr3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
		l_pctextattr3->AddAttribute(wxT("colour"), wxT("Navy") );
		l_pctextattr3->AddAttribute(wxT("bold"),  wxT("false") );
		l_pcMarking->AddChild(l_pctextattr3);

		wxXmlNode*  l_pctext3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
		l_pctext3->AddAttribute(wxT("tool"), wxT("CPN Tools") );
		l_pctext3->AddAttribute(wxT("version"),  wxT("2.2.0") );
		l_pcMarking->AddChild(l_pctext3);
		wxXmlNode*  l_pcElemText3 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_sMarking );
		l_pctext3->AddChild(l_pcElemText3);
	}

	return true;
}


bool
SP_CPNToolXmlWriter::WriteNormalEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
	
	long l_nTransID;
	long l_nPlaceID;	
	
	SP_DS_Node* l_pcSourceNode = (SP_DS_Node*)(p_pcVal->GetSource());
	SP_DS_Node* l_pcTargetNode = (SP_DS_Node*)(p_pcVal->GetTarget());

	 CHECK_POINTER(l_pcSourceNode, return FALSE);
	 CHECK_POINTER(l_pcTargetNode, return FALSE);

	 wxString l_sOrientation = wxT("");

	 wxString l_sSourceID =l_pcSourceNode->GetAttribute( wxT("ID") )->GetValueString();
	 wxString l_sTargetID =l_pcTargetNode->GetAttribute( wxT("ID") )->GetValueString();

	 if( l_pcSourceNode->GetNodeclass()->GetName() == wxT("Place") )
	 {
		l_sOrientation = wxT("PtoT");
		if( ! l_sSourceID.ToLong(&l_nPlaceID))
			return false;
	 }
	 else
	 {
		l_sOrientation = wxT("TtoP");
		if( ! l_sSourceID.ToLong(&l_nTransID))
		return false;
	 }
	 
	 if( l_pcTargetNode->GetNodeclass()->GetName() == wxT("Transition") )
	 {
		if( ! l_sTargetID.ToLong(&l_nTransID))
		return false;
	 }
	 else
	 {
		if( ! l_sTargetID.ToLong(&l_nPlaceID))
		return false;
	 }

	 //
	
	wxString l_sTransID = wxT("Transition")+wxString::Format(wxT("%d"), l_nTransID );
	wxString l_sPlaceID = wxT("Place")+wxString::Format(wxT("%d"), l_nPlaceID );

	if(m_mNodeIDMap.find(l_sTransID) == m_mNodeIDMap.end() )
		return false;
	
	wxString l_sCPNTransID = m_mNodeIDMap.find(l_sTransID)->second;

	if(m_mNodeIDMap.find(l_sPlaceID) == m_mNodeIDMap.end() )
		return false;
	
	wxString l_sCPNPlaceID = m_mNodeIDMap.find(l_sPlaceID)->second;


	 //


	 wxString l_sArcExpr = wxT("");

	 SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcVal->GetAttribute(SP_DS_CPN_INSCRIPTION));

	 CHECK_POINTER(l_pcColList, return FALSE);

	if(l_pcColList->GetRowCount() > 0)
		l_sArcExpr = l_pcColList->GetCell(0, 1);

	l_sArcExpr = ProcessExpression( l_sArcExpr );

	SP_Graphic* l_pcNewGr =  *(p_pcVal->GetAttribute( SP_DS_CPN_INSCRIPTION )->GetGraphics()->begin());
	double l_nExprPosX = l_pcNewGr->GetPosX();
	double l_nExprPosY = - l_pcNewGr->GetPosY();


	 ////////////////////////////////////

	wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("arc"));
	l_pcElem->AddAttribute(wxT("id"), wxT("id")+wxString::Format(wxT("%d"),m_nIDCount++ ) );
	l_pcElem->AddAttribute(wxT("orientation"), l_sOrientation );
	p_pcRoot->AddChild(l_pcElem);
	
	wxXmlNode*  l_pcPosattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr->AddAttribute(wxT("x"),  wxT("0.000000") );
	l_pcPosattr->AddAttribute(wxT("y"),  wxT("0.000000") );
	l_pcElem->AddChild(l_pcPosattr);

	wxXmlNode*  l_pcfillattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pcfillattr->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr->AddAttribute(wxT("filled"),  wxT("false") );
	l_pcElem->AddChild(l_pcfillattr);

	wxXmlNode*  l_pclineattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pclineattr->AddAttribute(wxT("thick"),  wxT("2") );
	l_pclineattr->AddAttribute(wxT("type"),  wxT("solid") );
	l_pcElem->AddChild(l_pclineattr);

	wxXmlNode*  l_pctextattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr->AddAttribute(wxT("colour"), wxT("Black") );
	l_pctextattr->AddAttribute(wxT("bold"),  wxT("false") );
	l_pcElem->AddChild(l_pctextattr);

	wxXmlNode*  l_pcarrowattr = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("arrowattr"));	
	l_pcarrowattr->AddAttribute(wxT("headsize"), wxT("1.000000") );
	l_pcarrowattr->AddAttribute(wxT("currentcyckle"),  wxT("2") );
	l_pcElem->AddChild(l_pcarrowattr);

	wxXmlNode*  l_pctransend = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("transend")  );	
	l_pctransend->AddAttribute(wxT("idref"), l_sCPNTransID );
	l_pcElem->AddChild(l_pctransend);

	wxXmlNode*  l_pcplaceend = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,  wxT("placeend"));	
	l_pcplaceend->AddAttribute(wxT("idref"), l_sCPNPlaceID );
	l_pcElem->AddChild(l_pcplaceend);


	////
	wxXmlNode*  l_pcannot = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("annot"));	
	l_pcannot->AddAttribute(wxT("id"), wxT("id")+ wxString::Format(wxT("%d"),m_nIDCount++) );	
	l_pcElem->AddChild(l_pcannot); 

	wxXmlNode*  l_pcPosattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("posattr"));	
	l_pcPosattr2->AddAttribute(wxT("x"), wxString::Format(wxT("%f"),l_nExprPosX) );
	l_pcPosattr2->AddAttribute(wxT("y"), wxString::Format(wxT("%f"),l_nExprPosY ) );
	l_pcannot->AddChild(l_pcPosattr2);

	wxXmlNode*  l_pcfillattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("fillattr"));	
	l_pcfillattr2->AddAttribute(wxT("colour"), wxT("White") );
	l_pcfillattr2->AddAttribute(wxT("pattern"),  wxT("solid") );
	l_pcfillattr2->AddAttribute(wxT("filled"),  wxT("false") );
	l_pcannot->AddChild(l_pcfillattr2);

	wxXmlNode*  l_pclineattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("lineattr"));	
	l_pclineattr2->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pclineattr2->AddAttribute(wxT("thick"),  wxT("0") );
	l_pclineattr2->AddAttribute(wxT("type"),  wxT("solid") );
	l_pcannot->AddChild(l_pclineattr2);

	wxXmlNode*  l_pctextattr2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("textattr"));	
	l_pctextattr2->AddAttribute(wxT("colour"), wxT("Navy") );
	l_pctextattr2->AddAttribute(wxT("bold"),  wxT("false") );
	l_pcannot->AddChild(l_pctextattr2);

	wxXmlNode*  l_pctext2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("text"));
	l_pcannot->AddChild(l_pctext2);
	wxXmlNode*  l_pcElemText2 = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxT(""),l_sArcExpr );
	l_pctext2->AddChild(l_pcElemText2);
   

    return true;
}


wxString SP_CPNToolXmlWriter::ProcessExpression( wxString p_sExpression)
{
	wxString l_sExpression = p_sExpression;

	l_sExpression.Replace(wxT("%"),wxT(" mod ")); //replace "%" with "mod" andalso
	l_sExpression.Replace(wxT("&"),wxT(" andalso ")); //replace "&" with "andalso" 
	l_sExpression.Replace(wxT("|"),wxT(" orelse ")); //replace "|" with "orelse"

	//handle predicate
/*
	l_sExpression.Replace(wxT(" "),wxT(""));
	l_sExpression.Replace(wxT("\n"),wxT(""));

	l_sExpression.Replace(wxT("++"),wxT("?"));

	wxString l_sSubExp;
	vector<wxString> l_vSubExpression;

	while(l_sExpression.Find((wxT('?'))) != -1)
	{							
		l_sSubExp = l_sExpression.BeforeFirst(wxT('?'));			
		l_sExpression = l_sExpression.AfterFirst(wxT('?'));
		l_vSubExpression.push_back( l_sSubExp );
	}
*/

	return l_sExpression;

}
































